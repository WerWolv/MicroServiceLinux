// SPDX-License-Identifier: GPL-2.0
#include <linux/atomic.h>
#include <linux/anon_inodes.h>
#include <linux/completion.h>
#include <linux/cred.h>
#include <linux/fcntl.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/overflow.h>
#include <linux/pagemap.h>
#include <linux/refcount.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/user_namespace.h>
#include <linux/wait.h>

#include "ipc.h"

struct ipc_message {
	void *data;
	u32 size;
	struct file *handles[IPC_HANDLES_MAX];
	u16 handle_count;
	struct file *region_files[IPC_REGIONS_MAX];
	struct ipc_region regions[IPC_REGIONS_MAX];
	u16 region_count;
};

enum ipc_txn_state {
	IPC_TXN_QUEUED,
	IPC_TXN_DELIVERING,
	IPC_TXN_DELIVERING_ABANDONED,
	IPC_TXN_INFLIGHT,
	IPC_TXN_ABANDONED,
	IPC_TXN_REPLIED,
};

struct ipc_service;

struct ipc_transaction {
	struct list_head node;
	refcount_t refs;
	struct completion done;
	struct ipc_service *service;
	u64 token;
	enum ipc_txn_state state;
	int transport_status;
	s32 result;
	struct pid *peer_pid;
	kuid_t peer_uid;
	struct ipc_message request;
	struct ipc_message response;
};

struct ipc_service {
	struct list_head global_node;
	struct kref ref;
	char name[IPC_SERVICE_NAME_MAX];
	spinlock_t lock;
	struct list_head pending;
	struct list_head inflight;
	atomic_t pending_count;
	wait_queue_head_t waitq;
	bool dead;
};

struct ipc_file {
	struct ipc_service *service;
};

static LIST_HEAD(ipc_services);
static DEFINE_MUTEX(ipc_services_lock);
static DECLARE_WAIT_QUEUE_HEAD(ipc_services_waitq);
static atomic64_t ipc_services_generation = ATOMIC64_INIT(0);
static atomic64_t ipc_next_token = ATOMIC64_INIT(0);
static const struct file_operations ipc_client_file_operations;
static const struct file_operations ipc_server_file_operations;

static void ipc_message_release(struct ipc_message *message)
{
	unsigned int i;

	for (i = 0; i < message->handle_count; ++i)
		fput(message->handles[i]);
	for (i = 0; i < message->region_count; ++i)
		fput(message->region_files[i]);
	kfree(message->data);
	memset(message, 0, sizeof(*message));
}

static int ipc_capture_message(struct ipc_message *message, u64 data_ptr,
				u32 size, u64 handles_ptr, u16 handle_count,
				u64 regions_ptr, u16 region_count)
{
	s32 handle_fds[IPC_HANDLES_MAX];
	struct ipc_region regions[IPC_REGIONS_MAX];
	unsigned int i;
	u64 end;
	int ret = 0;

	if (size > IPC_PAYLOAD_MAX || handle_count > IPC_HANDLES_MAX ||
	    region_count > IPC_REGIONS_MAX)
		return -EMSGSIZE;

	if (size) {
		message->data = memdup_user(u64_to_user_ptr(data_ptr), size);
		if (IS_ERR(message->data)) {
			ret = PTR_ERR(message->data);
			message->data = NULL;
			return ret;
		}
		message->size = size;
	}

	if (handle_count && copy_from_user(handle_fds,
				u64_to_user_ptr(handles_ptr),
				handle_count * sizeof(handle_fds[0]))) {
		ret = -EFAULT;
		goto error;
	}

	for (i = 0; i < handle_count; ++i) {
		message->handles[i] = fget(handle_fds[i]);
		if (!message->handles[i]) {
			ret = -EBADF;
			goto error;
		}
		message->handle_count++;
	}

	if (region_count && copy_from_user(regions,
				u64_to_user_ptr(regions_ptr),
				region_count * sizeof(regions[0]))) {
		ret = -EFAULT;
		goto error;
	}

	for (i = 0; i < region_count; ++i) {
		if (!regions[i].size || !IS_ALIGNED(regions[i].offset, PAGE_SIZE) ||
		    (regions[i].prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC)) ||
		    check_add_overflow(regions[i].offset, regions[i].size, &end)) {
			ret = -EINVAL;
			goto error;
		}

		message->region_files[i] = fget(regions[i].fd);
		if (!message->region_files[i]) {
			ret = -EBADF;
			goto error;
		}
		message->region_count++;
		if (!(message->region_files[i]->f_mode & FMODE_READ) ||
		    ((regions[i].prot & PROT_WRITE) &&
		     !(message->region_files[i]->f_mode & FMODE_WRITE))) {
			ret = -EACCES;
			goto error;
		}
		message->regions[i] = regions[i];
	}

	return 0;

error:
	ipc_message_release(message);
	return ret;
}

static int ipc_deliver_message(const struct ipc_message *message,
				u64 data_ptr, u64 handles_ptr, u64 regions_ptr)
{
	int reserved[IPC_HANDLES_MAX + IPC_REGIONS_MAX];
	s32 handle_fds[IPC_HANDLES_MAX];
	struct ipc_region regions[IPC_REGIONS_MAX];
	unsigned int reserved_count = 0;
	unsigned int i;
	int fd;
	int ret = 0;

	for (i = 0; i < message->handle_count; ++i) {
		fd = get_unused_fd_flags(O_CLOEXEC);
		if (fd < 0) {
			ret = fd;
			goto error;
		}
		reserved[reserved_count++] = fd;
		handle_fds[i] = fd;
	}

	for (i = 0; i < message->region_count; ++i) {
		fd = get_unused_fd_flags(O_CLOEXEC);
		if (fd < 0) {
			ret = fd;
			goto error;
		}
		reserved[reserved_count++] = fd;
		regions[i] = message->regions[i];
		regions[i].fd = fd;
	}

	if (message->size && copy_to_user(u64_to_user_ptr(data_ptr), message->data,
					 message->size)) {
		ret = -EFAULT;
		goto error;
	}
	if (message->handle_count && copy_to_user(u64_to_user_ptr(handles_ptr),
				handle_fds,
				message->handle_count * sizeof(handle_fds[0]))) {
		ret = -EFAULT;
		goto error;
	}
	if (message->region_count && copy_to_user(u64_to_user_ptr(regions_ptr),
				regions,
				message->region_count * sizeof(regions[0]))) {
		ret = -EFAULT;
		goto error;
	}

	for (i = 0; i < message->handle_count; ++i) {
		get_file(message->handles[i]);
		fd_install(handle_fds[i], message->handles[i]);
	}
	for (i = 0; i < message->region_count; ++i) {
		get_file(message->region_files[i]);
		fd_install(regions[i].fd, message->region_files[i]);
	}
	return 0;

error:
	while (reserved_count)
		put_unused_fd(reserved[--reserved_count]);
	return ret;
}

static void ipc_service_destroy(struct kref *ref)
{
	struct ipc_service *service = container_of(ref, struct ipc_service, ref);

	kfree(service);
}

static void ipc_service_put(struct ipc_service *service)
{
	kref_put(&service->ref, ipc_service_destroy);
}

static void ipc_transaction_put(struct ipc_transaction *transaction)
{
	if (!refcount_dec_and_test(&transaction->refs))
		return;

	ipc_message_release(&transaction->request);
	ipc_message_release(&transaction->response);
	put_pid(transaction->peer_pid);
	ipc_service_put(transaction->service);
	kfree(transaction);
}

static struct ipc_service *ipc_find_service(const char *name)
{
	struct ipc_service *service;

	mutex_lock(&ipc_services_lock);
	list_for_each_entry(service, &ipc_services, global_node) {
		if (!strcmp(service->name, name)) {
			kref_get(&service->ref);
			mutex_unlock(&ipc_services_lock);
			return service;
		}
	}
	mutex_unlock(&ipc_services_lock);
	return NULL;
}

static bool ipc_valid_name(const char name[IPC_SERVICE_NAME_MAX])
{
	return name[0] && memchr(name, '\0', IPC_SERVICE_NAME_MAX);
}

static struct ipc_service *ipc_alloc_service(const char *name)
{
	struct ipc_service *service;

	service = kzalloc(sizeof(*service), GFP_KERNEL);
	if (!service)
		return NULL;

	INIT_LIST_HEAD(&service->global_node);
	if (name)
		strscpy(service->name, name, sizeof(service->name));
	kref_init(&service->ref);
	spin_lock_init(&service->lock);
	INIT_LIST_HEAD(&service->pending);
	INIT_LIST_HEAD(&service->inflight);
	atomic_set(&service->pending_count, 0);
	init_waitqueue_head(&service->waitq);
	return service;
}

static long ipc_create_service(unsigned long arg)
{
	struct ipc_create_service creation = {};
	struct ipc_service *service;
	struct file *server_file;
	struct file *client_file;
	int server_fd;
	int client_fd;

	service = ipc_alloc_service(NULL);
	if (!service)
		return -ENOMEM;

	server_file = anon_inode_getfile("[ipc-server]",
			&ipc_server_file_operations, service, O_RDWR);
	if (IS_ERR(server_file)) {
		ipc_service_put(service);
		return PTR_ERR(server_file);
	}

	kref_get(&service->ref);
	client_file = anon_inode_getfile("[ipc-client]",
			&ipc_client_file_operations, service, O_RDWR);
	if (IS_ERR(client_file)) {
		ipc_service_put(service);
		fput(server_file);
		return PTR_ERR(client_file);
	}

	server_fd = get_unused_fd_flags(O_CLOEXEC);
	if (server_fd < 0) {
		fput(client_file);
		fput(server_file);
		return server_fd;
	}
	client_fd = get_unused_fd_flags(O_CLOEXEC);
	if (client_fd < 0) {
		put_unused_fd(server_fd);
		fput(client_file);
		fput(server_file);
		return client_fd;
	}

	creation.server_handle = server_fd;
	creation.client_handle = client_fd;
	if (copy_to_user((void __user *)arg, &creation, sizeof(creation))) {
		put_unused_fd(client_fd);
		put_unused_fd(server_fd);
		fput(client_file);
		fput(server_file);
		return -EFAULT;
	}

	fd_install(server_fd, server_file);
	fd_install(client_fd, client_file);
	return 0;
}

static long ipc_get_service(unsigned long arg)
{
	struct ipc_get_service lookup;
	struct ipc_service *service;
	struct file *service_file;
	u64 generation;
	int fd;
	int ret;

	if (copy_from_user(&lookup, (void __user *)arg, sizeof(lookup)))
		return -EFAULT;
	if (!ipc_valid_name(lookup.name))
		return -EINVAL;

	for (;;) {
		generation = atomic64_read(&ipc_services_generation);
		service = ipc_find_service(lookup.name);
		if (service)
			break;

		ret = wait_event_interruptible(ipc_services_waitq,
			atomic64_read(&ipc_services_generation) != generation);
		if (ret)
			return ret;
	}

	service_file = anon_inode_getfile("[ipc-service]",
			&ipc_client_file_operations, service, O_RDWR);
	if (IS_ERR(service_file)) {
		ipc_service_put(service);
		return PTR_ERR(service_file);
	}

	fd = get_unused_fd_flags(O_CLOEXEC);
	if (fd < 0) {
		fput(service_file);
		return fd;
	}
	lookup.handle = fd;
	lookup.reserved = 0;
	if (copy_to_user((void __user *)arg, &lookup, sizeof(lookup))) {
		put_unused_fd(fd);
		fput(service_file);
		return -EFAULT;
	}

	fd_install(fd, service_file);
	return 0;
}

static long ipc_register_service(struct file *file, unsigned long arg)
{
	struct ipc_register_service registration;
	struct ipc_file *context = file->private_data;
	struct ipc_service *service;
	struct ipc_service *existing;

	if (copy_from_user(&registration, (void __user *)arg,
			   sizeof(registration)))
		return -EFAULT;
	if (!ipc_valid_name(registration.name))
		return -EINVAL;
	service = ipc_alloc_service(registration.name);
	if (!service)
		return -ENOMEM;

	mutex_lock(&ipc_services_lock);
	if (context->service) {
		mutex_unlock(&ipc_services_lock);
		kfree(service);
		return -EBUSY;
	}
	list_for_each_entry(existing, &ipc_services, global_node) {
		if (!strcmp(existing->name, service->name)) {
			mutex_unlock(&ipc_services_lock);
			kfree(service);
			return -EEXIST;
		}
	}
	list_add_tail(&service->global_node, &ipc_services);
	smp_store_release(&context->service, service);
	mutex_unlock(&ipc_services_lock);
	atomic64_inc(&ipc_services_generation);
	wake_up_interruptible_all(&ipc_services_waitq);

	return 0;
}

static long ipc_send_sync(struct file *file, unsigned long arg)
{
	struct ipc_send_sync send;
	struct ipc_transaction *transaction;
	struct ipc_service *service;
	bool drop_service_ref = false;
	int ret;

	if (copy_from_user(&send, (void __user *)arg, sizeof(send)))
		return -EFAULT;
	if (send.response_capacity > IPC_PAYLOAD_MAX ||
	    send.response_handle_capacity > IPC_HANDLES_MAX ||
	    send.response_region_capacity > IPC_REGIONS_MAX)
		return -EMSGSIZE;

	service = file->private_data;

	transaction = kzalloc(sizeof(*transaction), GFP_KERNEL);
	if (!transaction)
		return -ENOMEM;
	INIT_LIST_HEAD(&transaction->node);
	refcount_set(&transaction->refs, 1);
	init_completion(&transaction->done);
	kref_get(&service->ref);
	transaction->service = service;
	transaction->token = atomic64_inc_return(&ipc_next_token);
	transaction->state = IPC_TXN_QUEUED;
	transaction->peer_pid = get_pid(task_tgid(current));
	transaction->peer_uid = current_euid();

	ret = ipc_capture_message(&transaction->request, send.request,
			send.request_size, send.handles, send.handle_count,
			send.regions, send.region_count);
	if (ret)
		goto out_put;

	spin_lock(&service->lock);
	if (service->dead) {
		spin_unlock(&service->lock);
		ret = -ESHUTDOWN;
		goto out_put;
	}
	refcount_inc(&transaction->refs); /* Queue/inflight ownership. */
	list_add_tail(&transaction->node, &service->pending);
	atomic_inc(&service->pending_count);
	spin_unlock(&service->lock);
	wake_up_interruptible(&service->waitq);

	ret = wait_for_completion_interruptible(&transaction->done);
	if (ret) {
		spin_lock(&service->lock);
		if (transaction->state == IPC_TXN_QUEUED) {
			list_del_init(&transaction->node);
			atomic_dec(&service->pending_count);
			transaction->state = IPC_TXN_ABANDONED;
			drop_service_ref = true;
		} else if (transaction->state == IPC_TXN_DELIVERING) {
			transaction->state = IPC_TXN_DELIVERING_ABANDONED;
			ret = -EINTR;
		} else if (transaction->state == IPC_TXN_INFLIGHT) {
			transaction->state = IPC_TXN_ABANDONED;
			ret = -EINTR;
		} else if (transaction->state == IPC_TXN_REPLIED) {
			ret = 0;
		}
		spin_unlock(&service->lock);
		if (drop_service_ref)
			ipc_transaction_put(transaction);
		if (ret)
			goto out_put;
	}

	ret = transaction->transport_status;
	if (ret)
		goto copy_status;

	send.response_size = transaction->response.size;
	send.response_handle_count = transaction->response.handle_count;
	send.response_region_count = transaction->response.region_count;
	send.result = transaction->result;
	if (send.response_capacity < send.response_size ||
	    send.response_handle_capacity < send.response_handle_count ||
	    send.response_region_capacity < send.response_region_count)
		ret = -EMSGSIZE;

copy_status:
	if (copy_to_user((void __user *)arg, &send, sizeof(send))) {
		ret = -EFAULT;
		goto out_put;
	}
	if (!ret)
		ret = ipc_deliver_message(&transaction->response, send.response,
				send.response_handles, send.response_regions);

out_put:
	ipc_transaction_put(transaction);
	return ret;
}

static int ipc_commit_reply(struct ipc_service *service,
			     struct ipc_receive_and_reply *call)
{
	struct ipc_transaction *transaction = NULL;
	struct ipc_transaction *candidate;
	struct ipc_message response = {};
	bool complete_transaction = false;
	int ret;

	if (!call->reply_token) {
		return 0;
	}

	ret = ipc_capture_message(&response, call->reply, call->reply_size,
			call->reply_handles, call->reply_handle_count,
			call->reply_regions, call->reply_region_count);
	if (ret)
		return ret;

	spin_lock(&service->lock);
	list_for_each_entry(candidate, &service->inflight, node) {
		if (candidate->token == call->reply_token) {
			transaction = candidate;
			break;
		}
	}
	if (!transaction) {
		spin_unlock(&service->lock);
		ipc_message_release(&response);
		return -ENOENT;
	}
	if (transaction->state == IPC_TXN_DELIVERING ||
	    transaction->state == IPC_TXN_DELIVERING_ABANDONED) {
		spin_unlock(&service->lock);
		ipc_message_release(&response);
		return -EBUSY;
	}

	list_del_init(&transaction->node);
	if (transaction->state == IPC_TXN_INFLIGHT) {
		transaction->response = response;
		memset(&response, 0, sizeof(response));
		transaction->result = call->reply_result;
		transaction->transport_status = 0;
		transaction->state = IPC_TXN_REPLIED;
		complete_transaction = true;
	}
	spin_unlock(&service->lock);

	ipc_message_release(&response);
	if (complete_transaction)
		complete(&transaction->done);
	ipc_transaction_put(transaction);
	call->reply_token = 0;
	return 0;
}

static void ipc_fail_delivery(struct ipc_transaction *transaction, int error)
{
	struct ipc_service *service = transaction->service;
	bool complete_transaction = false;

	spin_lock(&service->lock);
	if (transaction->state == IPC_TXN_DELIVERING ||
	    transaction->state == IPC_TXN_INFLIGHT) {
		list_del_init(&transaction->node);
		transaction->transport_status = error;
		transaction->state = IPC_TXN_REPLIED;
		complete_transaction = true;
	} else if (transaction->state == IPC_TXN_DELIVERING_ABANDONED ||
		   transaction->state == IPC_TXN_ABANDONED) {
		list_del_init(&transaction->node);
	}
	spin_unlock(&service->lock);

	if (complete_transaction)
		complete(&transaction->done);
	ipc_transaction_put(transaction);
}

static long ipc_receive_and_reply(struct ipc_service *service,
				   unsigned long arg)
{
	struct ipc_receive_and_reply call;
	struct ipc_transaction *transaction;
	bool committed_reply;
	int ret;

	if (!service)
		return -ENOTCONN;
	if (copy_from_user(&call, (void __user *)arg, sizeof(call)))
		return -EFAULT;
	if (call.request_capacity > IPC_PAYLOAD_MAX ||
	    call.request_handle_capacity > IPC_HANDLES_MAX ||
	    call.request_region_capacity > IPC_REGIONS_MAX)
		return -EMSGSIZE;

	committed_reply = call.reply_token != 0;
	if (committed_reply && fault_in_writeable(
			(char __user *)&((struct ipc_receive_and_reply __user *)arg)->reply_token,
			sizeof(call.reply_token)))
		return -EFAULT;
	ret = ipc_commit_reply(service, &call);
	if (ret)
		return ret;
	if (committed_reply && put_user((__u64)0,
			&((struct ipc_receive_and_reply __user *)arg)->reply_token))
		return -EFAULT;

	for (;;) {
		ret = wait_event_interruptible(service->waitq,
			READ_ONCE(service->dead) ||
			atomic_read(&service->pending_count));
		if (ret)
			return ret;

		spin_lock(&service->lock);
		if (service->dead) {
			spin_unlock(&service->lock);
			return -ESHUTDOWN;
		}
		if (list_empty(&service->pending)) {
			spin_unlock(&service->lock);
			continue;
		}

		transaction = list_first_entry(&service->pending,
				struct ipc_transaction, node);
		call.request_token = transaction->token;
		call.request_size = transaction->request.size;
		call.request_handle_count = transaction->request.handle_count;
		call.request_region_count = transaction->request.region_count;
		call.peer_pid = pid_vnr(transaction->peer_pid);
		call.peer_uid = from_kuid_munged(current_user_ns(),
					 transaction->peer_uid);
		if (call.request_capacity < call.request_size ||
		    call.request_handle_capacity < call.request_handle_count ||
		    call.request_region_capacity < call.request_region_count) {
			spin_unlock(&service->lock);
			if (copy_to_user((void __user *)arg, &call, sizeof(call)))
				return -EFAULT;
			return -EMSGSIZE;
		}

		list_move_tail(&transaction->node, &service->inflight);
		atomic_dec(&service->pending_count);
		transaction->state = IPC_TXN_DELIVERING;
		spin_unlock(&service->lock);
		break;
	}

	if (copy_to_user((void __user *)arg, &call, sizeof(call))) {
		ipc_fail_delivery(transaction, -EFAULT);
		return -EFAULT;
	}

	ret = ipc_deliver_message(&transaction->request, call.request,
			call.request_handles, call.request_regions);
	if (ret) {
		ipc_fail_delivery(transaction, ret);
	} else {
		spin_lock(&service->lock);
		if (transaction->state == IPC_TXN_DELIVERING)
			transaction->state = IPC_TXN_INFLIGHT;
		else if (transaction->state == IPC_TXN_DELIVERING_ABANDONED)
			transaction->state = IPC_TXN_ABANDONED;
		spin_unlock(&service->lock);
	}
	return ret;
}

static long ipc_control_ioctl(struct file *file, unsigned int command,
			       unsigned long arg)
{
	switch (command) {
	case IPC_IOC_REGISTER_SERVICE:
		return ipc_register_service(file, arg);
	case IPC_IOC_CREATE_SERVICE:
		return ipc_create_service(arg);
	case IPC_IOC_GET_SERVICE:
		return ipc_get_service(arg);
	case IPC_IOC_RECEIVE_AND_REPLY: {
		struct ipc_file *context = file->private_data;

		return ipc_receive_and_reply(
			smp_load_acquire(&context->service), arg);
	}
	default:
		return -ENOTTY;
	}
}

static void ipc_shutdown_service(struct ipc_service *service)
{
	struct ipc_transaction *transaction;
	struct ipc_transaction *next;
	bool named;
	LIST_HEAD(aborted);

	mutex_lock(&ipc_services_lock);
	named = !list_empty(&service->global_node);
	if (named)
		list_del_init(&service->global_node);

	spin_lock(&service->lock);
	service->dead = true;
	list_for_each_entry(transaction, &service->pending, node) {
		transaction->transport_status = -ESHUTDOWN;
		transaction->state = IPC_TXN_REPLIED;
	}
	list_for_each_entry(transaction, &service->inflight, node) {
		transaction->transport_status = -ESHUTDOWN;
		transaction->state = IPC_TXN_REPLIED;
	}
	list_splice_init(&service->pending, &aborted);
	list_splice_init(&service->inflight, &aborted);
	atomic_set(&service->pending_count, 0);
	spin_unlock(&service->lock);
	mutex_unlock(&ipc_services_lock);

	if (named) {
		atomic64_inc(&ipc_services_generation);
		wake_up_interruptible_all(&ipc_services_waitq);
	}
	wake_up_interruptible_all(&service->waitq);

	list_for_each_entry_safe(transaction, next, &aborted, node) {
		list_del_init(&transaction->node);
		complete(&transaction->done);
		ipc_transaction_put(transaction);
	}
}

static long ipc_client_ioctl(struct file *file, unsigned int command,
			      unsigned long arg)
{
	if (command != IPC_IOC_SEND_SYNC_REQUEST)
		return -ENOTTY;
	return ipc_send_sync(file, arg);
}

static int ipc_client_release(struct inode *inode, struct file *file)
{
	(void)inode;
	ipc_service_put(file->private_data);
	return 0;
}

static const struct file_operations ipc_client_file_operations = {
	.owner = THIS_MODULE,
	.release = ipc_client_release,
	.unlocked_ioctl = ipc_client_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ipc_client_ioctl,
#endif
	.llseek = noop_llseek,
};

static long ipc_server_ioctl(struct file *file, unsigned int command,
			      unsigned long arg)
{
	if (command != IPC_IOC_RECEIVE_AND_REPLY)
		return -ENOTTY;
	return ipc_receive_and_reply(file->private_data, arg);
}

static int ipc_server_release(struct inode *inode, struct file *file)
{
	struct ipc_service *service = file->private_data;

	(void)inode;
	ipc_shutdown_service(service);
	ipc_service_put(service);
	return 0;
}

static const struct file_operations ipc_server_file_operations = {
	.owner = THIS_MODULE,
	.release = ipc_server_release,
	.unlocked_ioctl = ipc_server_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ipc_server_ioctl,
#endif
	.llseek = noop_llseek,
};

static int ipc_open(struct inode *inode, struct file *file)
{
	struct ipc_file *context;

	(void)inode;
	context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (!context)
		return -ENOMEM;
	file->private_data = context;
	return 0;
}

static int ipc_release(struct inode *inode, struct file *file)
{
	struct ipc_file *context = file->private_data;
	struct ipc_service *service = context->service;

	(void)inode;
	if (!service) {
		kfree(context);
		return 0;
	}

	ipc_shutdown_service(service);
	ipc_service_put(service);
	kfree(context);
	return 0;
}

static const struct file_operations ipc_file_operations = {
	.owner = THIS_MODULE,
	.open = ipc_open,
	.release = ipc_release,
	.unlocked_ioctl = ipc_control_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl = ipc_control_ioctl,
#endif
	.llseek = noop_llseek,
};

static struct miscdevice ipc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "ipc",
	.fops = &ipc_file_operations,
	.mode = 0666,
};

static int __init ipc_init(void)
{
	return misc_register(&ipc_device);
}

static void __exit ipc_exit(void)
{
	misc_deregister(&ipc_device);
}

module_init(ipc_init);
module_exit(ipc_exit);

MODULE_DESCRIPTION("Synchronous IPC transport");
MODULE_AUTHOR("WerWolv");
MODULE_LICENSE("GPL");
