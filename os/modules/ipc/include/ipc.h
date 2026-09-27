#ifndef HORIZON_IPC_H
#define HORIZON_IPC_H

#include <linux/ioctl.h>
#include <linux/types.h>

#define IPC_DEVICE_PATH "/dev/ipc"
#define IPC_SERVICE_NAME_MAX 32
#define IPC_PAYLOAD_MAX 4096
#define IPC_HANDLES_MAX 8
#define IPC_REGIONS_MAX 4

/* A region is a shared-memory FD plus the range the receiver may map. */
struct ipc_region {
	__s32 fd;
	__u32 prot;
	__u64 offset;
	__u64 size;
};

struct ipc_register_service {
	char name[IPC_SERVICE_NAME_MAX];
};

struct ipc_get_service {
	char name[IPC_SERVICE_NAME_MAX];
	__s32 handle;
	__u32 reserved;
};

struct ipc_create_service {
	__s32 server_handle;
	__s32 client_handle;
};

struct ipc_send_sync {
	__u64 request;
	__u64 response;
	__u64 handles;
	__u64 regions;
	__u64 response_handles;
	__u64 response_regions;

	__u32 request_size;
	__u32 response_capacity;
	__u16 handle_count;
	__u16 region_count;
	__u16 response_handle_capacity;
	__u16 response_region_capacity;

	/* Outputs. result belongs to the service; ioctl errors are transport errors. */
	__u32 response_size;
	__u16 response_handle_count;
	__u16 response_region_count;
	__s32 result;
	__u32 reserved;
};

/*
 * On entry, reply_token identifies the previous request to reply to. Set it to
 * zero on the first call. The reply is committed before waiting for a request.
 * On success, request_token and the request fields describe the next request.
 */
struct ipc_receive_and_reply {
	__u64 reply_token;
	__s32 reply_result;
	__u32 reply_size;
	__u64 reply;
	__u64 reply_handles;
	__u64 reply_regions;
	__u16 reply_handle_count;
	__u16 reply_region_count;
	__u32 reserved0;

	__u64 request_token;
	__u64 request;
	__u64 request_handles;
	__u64 request_regions;
	__u32 request_capacity;
	__u32 request_size;
	__u16 request_handle_capacity;
	__u16 request_region_capacity;
	__u16 request_handle_count;
	__u16 request_region_count;
	__u32 peer_pid;
	__u32 peer_uid;
};

#define IPC_IOC_MAGIC 0xB7
#define IPC_IOC_REGISTER_SERVICE \
	_IOW(IPC_IOC_MAGIC, 0x00, struct ipc_register_service)
#define IPC_IOC_CREATE_SERVICE \
	_IOR(IPC_IOC_MAGIC, 0x01, struct ipc_create_service)
#define IPC_IOC_GET_SERVICE \
	_IOWR(IPC_IOC_MAGIC, 0x02, struct ipc_get_service)
#define IPC_IOC_SEND_SYNC_REQUEST \
	_IOWR(IPC_IOC_MAGIC, 0x03, struct ipc_send_sync)
#define IPC_IOC_RECEIVE_AND_REPLY \
	_IOWR(IPC_IOC_MAGIC, 0x04, struct ipc_receive_and_reply)

#endif
