#pragma once

#include <meta>

#include "ipc.h"
#include "ipc_serialization.hpp"
#include "serializers.hpp"

#include <array>
#include <cerrno>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <type_traits>
#include <vector>

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace ipc {

    struct MethodId {
        constexpr MethodId(int id) : id(id) {}
        int id;
    };

    namespace impl {

        template<int Id, typename Ret, typename ... Args>
        struct IdCaller {
            Ret operator()(Args ... args) {
                (validateWireArgument<Args>(), ...);
                if constexpr (!std::is_void_v<Ret>)
                    validateWireReturn<Ret>();

                SyncRequest<Id, Args...> request {
                    .arguments = std::tuple<Args...>(std::forward<Args>(args)...)
                };

                auto requestPayload = request.serialize();
                SerializedMessage responseMessage;
                ipc_send_sync send = {};

                send.request = reinterpret_cast<std::uintptr_t>(requestPayload.payload.data());
                send.response = reinterpret_cast<std::uintptr_t>(responseMessage.payload.data());
                send.handles = reinterpret_cast<std::uintptr_t>(requestPayload.handles.data());
                send.regions = reinterpret_cast<std::uintptr_t>(requestPayload.regions.data());
                send.response_handles = reinterpret_cast<std::uintptr_t>(responseMessage.handles.data());
                send.response_regions = reinterpret_cast<std::uintptr_t>(responseMessage.regions.data());
                send.request_size = requestPayload.payloadSize;
                send.response_capacity = responseMessage.payload.size();
                send.handle_count = requestPayload.handleCount;
                send.region_count = requestPayload.regionCount;
                send.response_handle_capacity = responseMessage.handles.size();
                send.response_region_capacity = responseMessage.regions.size();

                if (ioctl(fd, IPC_IOC_SEND_SYNC_REQUEST, &send) < 0)
                    throw std::system_error(errno, std::generic_category(), "send IPC request");

                responseMessage.payloadSize = send.response_size;
                responseMessage.handleCount = send.response_handle_count;
                responseMessage.regionCount = send.response_region_count;
                if (send.result != 0)
                    throw std::runtime_error("IPC service call (" + std::to_string(Id) + ") returned error " + std::to_string(send.result));

                return SyncResponse<Ret>::deserialize(responseMessage);
            }

            int fd = -1;
        };

        template<typename Service>
        struct ClientCreator {
            struct Type : Service {};

            consteval {
                std::vector<std::meta::info> functionTypes;
                for (auto method : std::meta::members_of(^^Service, std::meta::access_context::unprivileged())) {
                    if (!std::meta::is_function(method))
                        continue;
                    if (std::meta::is_special_member_function(method))
                        continue;

                    if (!std::meta::is_pure_virtual(method))
                        throw std::runtime_error("Method " + std::string(std::meta::identifier_of(method)) + " is not pure-virtual!");

                    auto methodIdAttributes = std::meta::annotations_of_with_type(method, ^^MethodId);
                    if (methodIdAttributes.empty())
                        continue;
                    auto firstAttribute = methodIdAttributes.front();

                    auto functionType = std::meta::type_of(method);
                    auto templateParameters = std::meta::parameters_of(functionType);
                    templateParameters.insert(templateParameters.begin(), std::meta::return_type_of(functionType));
                    templateParameters.insert(templateParameters.begin(), std::meta::reflect_constant(std::meta::extract<MethodId>(firstAttribute).id));

                    auto type = std::meta::substitute(^^IdCaller, templateParameters);

                    functionTypes.push_back(std::meta::data_member_spec(type, std::meta::data_member_options { .name = std::meta::identifier_of(method)} ));
                }

                std::meta::define_aggregate(^^Type, functionTypes);
            }
        };

        template<typename Service>
        FileDescriptor getNamedService() {
            static_assert(requires { Service::Identifier; }, "Named IPC clients require a Service Identifier");
            static_assert(!std::string_view(Service::Identifier).empty(), "Service Identifier is empty!");
            static_assert(std::string_view(Service::Identifier).size() < IPC_SERVICE_NAME_MAX,
                          "Service Identifier is too long! Must be less than 32 characters!");

            FileDescriptor control(open(IPC_DEVICE_PATH, O_RDWR | O_CLOEXEC));
            if (!control.valid())
                throw std::system_error(errno, std::generic_category(), "open " IPC_DEVICE_PATH);

            ipc_get_service lookup = {};
            constexpr std::string_view identifier = Service::Identifier;
            std::memcpy(lookup.name, identifier.data(), identifier.size());
            if (ioctl(control.get(), IPC_IOC_GET_SERVICE, &lookup) < 0)
                throw std::system_error(errno, std::generic_category(), "get IPC service");
            return FileDescriptor(lookup.handle);
        }

    }

    constexpr static struct EmptyMarker {} Empty;

    template<typename T>
    class ServiceHandle : public impl::ClientCreator<T>::Type {
    public:
        ServiceHandle(EmptyMarker) {}
        ServiceHandle() : ServiceHandle(impl::getNamedService<T>()) {}

        explicit ServiceHandle(FileDescriptor descriptor) : descriptor(std::move(descriptor)) {
            if (!this->descriptor.valid())
                throw std::invalid_argument("cannot create an IPC client from an invalid service handle");

            using Callers = impl::ClientCreator<T>::Type;
            template for (constexpr auto member : std::define_static_array(std::meta::nonstatic_data_members_of(^^Callers, std::meta::access_context::unprivileged()))) {
                this->[:member:] = { this->descriptor.get() };
            }
        }

        ServiceHandle(const ServiceHandle &) = delete;
        ServiceHandle &operator=(const ServiceHandle &) = delete;
        ServiceHandle(ServiceHandle &&) noexcept = default;
        ServiceHandle &operator=(ServiceHandle &&) noexcept = default;

        [[nodiscard]] int handle() const { return descriptor.get(); }

    private:
        FileDescriptor descriptor;
    };

    template<typename T>
    struct Serializer<ServiceHandle<T>> {
        static void serialize(SerializerWriter &writer, const ServiceHandle<T> &service) {
            writer.writeFileDescriptor(service.handle());
        }

        static ServiceHandle<T> deserialize(SerializerReader &reader) {
            return ServiceHandle<T>(FileDescriptor(reader.readFileDescriptor()));
        }
    };

    template<typename T>
    struct Service {
        virtual ~Service() = default;

        struct Definition : T, std::enable_shared_from_this<Definition> {
            void process(int threadCount) {
                static_assert(requires { T::Identifier; }, "Missing Service Identifier! Add `constexpr static auto Identifier = \"service_name\";` to your named service definition!");
                static_assert(!std::string_view(T::Identifier).empty(), "Service Identifier is empty!");
                static_assert(std::string_view(T::Identifier).size() < IPC_SERVICE_NAME_MAX, "Service Identifier is too long! Must be less than 32 characters!");

                int deviceFd = open(IPC_DEVICE_PATH, O_RDWR | O_CLOEXEC);
                if (deviceFd < 0)
                    throw std::system_error(errno, std::generic_category(), "open " IPC_DEVICE_PATH);

                ipc_register_service registration {};
                constexpr std::string_view identifier = T::Identifier;
                std::memcpy(registration.name, identifier.data(), identifier.size());
                if (ioctl(deviceFd, IPC_IOC_REGISTER_SERVICE, &registration) < 0) {
                    const auto error = errno;
                    close(deviceFd);
                    throw std::system_error(error, std::generic_category(), "register IPC service");
                }

                startProcessing(deviceFd, threadCount);
                for (;;)
                    pause();
            }

            ServiceHandle<T> createUnnamed(int threadCount = 1) {
                auto lifetime = this->shared_from_this();
                FileDescriptor control(open(IPC_DEVICE_PATH, O_RDWR | O_CLOEXEC));
                if (!control.valid())
                    throw std::system_error(errno, std::generic_category(), "open " IPC_DEVICE_PATH);

                ipc_create_service creation = {};
                if (ioctl(control.get(), IPC_IOC_CREATE_SERVICE, &creation) < 0)
                    throw std::system_error(errno, std::generic_category(), "create unnamed IPC service");

                FileDescriptor server(creation.server_handle);
                FileDescriptor client(creation.client_handle);
                startProcessing(server.release(), threadCount, std::move(lifetime));
                return ServiceHandle<T>(std::move(client));
            }

        private:
            void startProcessing(int serverFd, int threadCount,
                                 std::shared_ptr<Definition> lifetime = {}) {
                FileDescriptor server(serverFd);
                if (threadCount <= 0)
                    throw std::invalid_argument("IPC server thread count must be positive");

                std::vector<FileDescriptor> workers;
                workers.reserve(threadCount);
                for (int i = 0; i < threadCount; ++i) {
                    const int workerFd = fcntl(server.get(), F_DUPFD_CLOEXEC, 0);
                    if (workerFd < 0)
                        throw std::system_error(errno, std::generic_category(), "duplicate IPC server handle");
                    workers.emplace_back(workerFd);
                }

                for (auto &worker : workers) {
                    std::thread([this, worker = std::move(worker), lifetime]() mutable {
                        this->processThread(worker.release());
                    }).detach();
                }
            }

            void processThread(int deviceFd) {
                impl::SerializedMessage request;
                impl::SerializedMessage response;
                ipc_receive_and_reply call {};
                call.request = reinterpret_cast<std::uintptr_t>(request.payload.data());
                call.request_handles = reinterpret_cast<std::uintptr_t>(request.handles.data());
                call.request_regions = reinterpret_cast<std::uintptr_t>(request.regions.data());
                call.request_capacity = request.payload.size();
                call.request_handle_capacity = request.handles.size();
                call.request_region_capacity = request.regions.size();

                for (;;) {
                    const bool submittedReply = call.reply_token != 0;
                    call.reply = reinterpret_cast<std::uintptr_t>(response.payload.data());
                    call.reply_handles = reinterpret_cast<std::uintptr_t>(response.handles.data());
                    call.reply_regions = reinterpret_cast<std::uintptr_t>(response.regions.data());
                    call.reply_size = response.payloadSize;
                    call.reply_handle_count = response.handleCount;
                    call.reply_region_count = response.regionCount;

                    if (ioctl(deviceFd, IPC_IOC_RECEIVE_AND_REPLY, &call) < 0) {
                        if (errno == EINTR) {
                            response.clear();
                            continue;
                        }
                        const auto error = errno;
                        close(deviceFd);
                        if (error != ESHUTDOWN && error != EBADF)
                            fprintf(stderr, "receive IPC request failed: %s\n", std::strerror(error));
                        return;
                    }

                    response.clear();
                    if (submittedReply)
                        continue;

                    request.payloadSize = call.request_size;
                    request.handleCount = call.request_handle_count;
                    request.regionCount = call.request_region_count;

                    call.reply_token = call.request_token;
                    call.reply_result = -ENOSYS;

                    if (call.request_size < sizeof(std::uint32_t)) {
                        call.reply_result = -EINVAL;
                        request.clear();
                        continue;
                    }

                    std::uint32_t command;
                    std::memcpy(&command, request.payload.data(), sizeof(command));
                    bool dispatched = false;

                    template for (constexpr auto method : std::define_static_array(std::meta::members_of(^^T, std::meta::access_context::unprivileged()))) {
                        if constexpr (std::meta::is_function(method) && !std::meta::is_special_member_function(method)) {
                            constexpr auto attributes = std::define_static_array(std::meta::annotations_of_with_type(method, ^^MethodId));
                            if constexpr (!attributes.empty()) {
                                constexpr auto methodId = std::meta::extract<MethodId>(attributes.front()).id;
                                if (!dispatched && command == methodId) {
                                    using Invoker = [:impl::serverInvokerType(method):];

                                    try {
                                        Invoker::invoke(
                                            [this]<typename... Args>(Args &&... args) -> decltype(auto) {
                                                return this->[:method:](std::forward<Args>(args)...);
                                            },
                                            request,
                                            response);
                                        call.reply_result = 0;
                                    } catch (const std::exception &e) {
                                        int kernelConsole = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
                                        dprintf(kernelConsole, "IPC service method %s threw an exception: %s\n", std::meta::identifier_of(method).data(), e.what());
                                        close(kernelConsole);
                                        response.clear();
                                        call.reply_result = -EINVAL;
                                    }
                                    dispatched = true;
                                }
                            }
                        }
                    }
                    request.clear();
                }
            }
        };

        using Client = ServiceHandle<T>;
        using Handle = Client;

        struct Server;

    protected:
        Service() = default;
    };

}
