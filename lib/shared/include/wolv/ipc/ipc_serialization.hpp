#pragma once

#include "ipc.h"

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <meta>
#include <span>
#include <stdexcept>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

namespace ipc {

    class FileDescriptor {
    public:
        FileDescriptor() = default;
        explicit FileDescriptor(int fd) : fd(fd) {}

        FileDescriptor(const FileDescriptor &) = delete;
        FileDescriptor &operator=(const FileDescriptor &) = delete;

        FileDescriptor(FileDescriptor &&other) noexcept : fd(other.release()) {}

        FileDescriptor &operator=(FileDescriptor &&other) noexcept {
            if (this != &other)
                reset(other.release());
            return *this;
        }

        ~FileDescriptor() {
            reset();
        }

        [[nodiscard]] int get() const { return fd; }
        [[nodiscard]] bool valid() const { return fd >= 0; }

        int release() {
            return std::exchange(fd, -1);
        }

        void reset(int replacement = -1) {
            if (fd >= 0)
                close(fd);
            fd = replacement;
        }

    private:
        int fd = -1;
    };

    class MemoryRegion {
    public:
        MemoryRegion(FileDescriptor descriptor, std::uint32_t protection,
                     std::uint64_t offset, std::uint64_t size)
            : descriptor(std::move(descriptor)), protection(protection),
              regionOffset(offset), regionSize(size) {}

        MemoryRegion(const MemoryRegion &) = delete;
        MemoryRegion &operator=(const MemoryRegion &) = delete;
        MemoryRegion(MemoryRegion &&) noexcept = default;
        MemoryRegion &operator=(MemoryRegion &&) noexcept = default;

        [[nodiscard]] const FileDescriptor &fileDescriptor() const { return descriptor; }
        [[nodiscard]] FileDescriptor &fileDescriptor() { return descriptor; }
        [[nodiscard]] std::uint32_t prot() const { return protection; }
        [[nodiscard]] std::uint64_t offset() const { return regionOffset; }
        [[nodiscard]] std::uint64_t size() const { return regionSize; }
        [[nodiscard]] std::span<std::byte> map() const {
            void *mapping = mmap(nullptr, regionSize, protection, MAP_SHARED,
                                 descriptor.get(), regionOffset);
            if (mapping == MAP_FAILED)
                throw std::system_error(errno, std::generic_category(), "map IPC memory region");
            return std::span(static_cast<std::byte *>(mapping), regionSize);
        }

    private:
        FileDescriptor descriptor;
        std::uint32_t protection;
        std::uint64_t regionOffset;
        std::uint64_t regionSize;
    };

    class SerializerWriter;
    class SerializerReader;

    template<typename T>
    struct Serializer;

    class SerializerWriter {
    public:
        SerializerWriter(std::byte *data, std::size_t capacity) : data(data), capacity(capacity) {}

        SerializerWriter(std::byte *data, std::size_t capacity,
                         std::span<int> handles, std::size_t &handleCount,
                         std::span<ipc_region> regions, std::size_t &regionCount)
            : data(data), capacity(capacity), handles(handles), regions(regions),
              handleCount(&handleCount), regionCount(&regionCount) {}

        template<typename T>
        void write(const T &value);

        void writeBytes(std::span<const std::byte> bytes) {
            if (bytes.size() > capacity - offset)
                throw std::length_error("IPC payload exceeds ipc_PAYLOAD_MAX");

            if (!bytes.empty())
                std::memcpy(data + offset, bytes.data(), bytes.size());
            offset += bytes.size();
        }

        void writeFileDescriptor(int fd) {
            if (handleCount == nullptr)
                throw std::logic_error("serializer has no file-descriptor attachment storage");
            if (*handleCount == handles.size())
                throw std::length_error("IPC message exceeds ipc_HANDLES_MAX");
            if (fd < 0)
                throw std::invalid_argument("cannot serialize an invalid file descriptor");

            const int duplicate = fcntl(fd, F_DUPFD_CLOEXEC, 0);
            if (duplicate < 0)
                throw std::system_error(errno, std::generic_category(), "duplicate IPC file descriptor");
            handles[(*handleCount)++] = duplicate;
        }

        void writeMemoryRegion(const ipc_region &region) {
            if (regionCount == nullptr)
                throw std::logic_error("serializer has no memory-region attachment storage");
            if (*regionCount == regions.size())
                throw std::length_error("IPC message exceeds ipc_REGIONS_MAX");
            if (region.fd < 0)
                throw std::invalid_argument("cannot serialize a memory region with an invalid file descriptor");

            const int duplicate = fcntl(region.fd, F_DUPFD_CLOEXEC, 0);
            if (duplicate < 0)
                throw std::system_error(errno, std::generic_category(), "duplicate IPC memory region");

            auto copy = region;
            copy.fd = duplicate;
            regions[(*regionCount)++] = copy;
        }

        [[nodiscard]] std::size_t size() const { return offset; }

    private:
        std::byte *data;
        std::size_t capacity;
        std::size_t offset = 0;
        std::span<int> handles;
        std::span<ipc_region> regions;
        std::size_t *handleCount = nullptr;
        std::size_t *regionCount = nullptr;
    };

    class SerializerReader {
    public:
        SerializerReader(const std::byte *data, std::size_t size) : data(data), size(size) {}

        SerializerReader(const std::byte *data, std::size_t size,
                         std::span<int> handles, std::span<ipc_region> regions)
            : data(data), size(size), handles(handles), regions(regions) {}

        template<typename T>
        std::remove_cv_t<std::remove_reference_t<T>> read();

        std::span<const std::byte> readBytes(std::size_t count) {
            if (count > size - offset)
                throw std::runtime_error("IPC payload is truncated");

            auto bytes = std::span(data + offset, count);
            offset += count;
            return bytes;
        }

        int readFileDescriptor() {
            if (handleIndex == handles.size())
                throw std::runtime_error("IPC message is missing a file descriptor");
            return std::exchange(handles[handleIndex++], -1);
        }

        ipc_region readMemoryRegion() {
            if (regionIndex == regions.size())
                throw std::runtime_error("IPC message is missing a memory region");
            auto region = regions[regionIndex++];
            regions[regionIndex - 1].fd = -1;
            return region;
        }

        [[nodiscard]] std::size_t remaining() const { return size - offset; }

        void finish() const {
            if (remaining() != 0)
                throw std::runtime_error("IPC payload has trailing data");
            if (handleIndex != handles.size())
                throw std::runtime_error("IPC message has trailing file descriptors");
            if (regionIndex != regions.size())
                throw std::runtime_error("IPC message has trailing memory regions");
        }

    private:
        const std::byte *data;
        std::size_t size;
        std::size_t offset = 0;
        std::span<int> handles;
        std::span<ipc_region> regions;
        std::size_t handleIndex = 0;
        std::size_t regionIndex = 0;
    };

    template<typename>
    struct Serializer {};

    template<typename T>
    void SerializerWriter::write(const T &value) {
        using Value = std::remove_cv_t<std::remove_reference_t<T>>;
        Serializer<Value>::serialize(*this, value);
    }

    template<typename T>
    std::remove_cv_t<std::remove_reference_t<T>> SerializerReader::read() {
        using Value = std::remove_cv_t<std::remove_reference_t<T>>;
        return Serializer<Value>::deserialize(*this);
    }

    namespace impl {

        template<typename T>
        using WireValue = std::remove_cv_t<std::remove_reference_t<T>>;

        template<typename T>
        consteval void validateWireArgument() {
            static_assert(!std::is_rvalue_reference_v<T>, "IPC arguments cannot be rvalue references");
        }

        template<typename T>
        consteval void validateWireReturn() {
            static_assert(!std::is_reference_v<T>, "IPC methods cannot return references");
        }

        struct SerializedMessage {
            std::array<std::byte, IPC_PAYLOAD_MAX> payload {};
            std::array<int, IPC_HANDLES_MAX> handles {};
            std::array<ipc_region, IPC_REGIONS_MAX> regions {};
            std::size_t payloadSize = 0;
            std::size_t handleCount = 0;
            std::size_t regionCount = 0;

            SerializedMessage() = default;
            SerializedMessage(const SerializedMessage &) = delete;
            SerializedMessage &operator=(const SerializedMessage &) = delete;

            SerializedMessage(SerializedMessage &&other) noexcept
                : payload(other.payload), handles(other.handles),
                  regions(other.regions), payloadSize(other.payloadSize),
                  handleCount(std::exchange(other.handleCount, 0)),
                  regionCount(std::exchange(other.regionCount, 0)) {
                other.payloadSize = 0;
            }

            SerializedMessage &operator=(SerializedMessage &&other) noexcept {
                if (this != &other) {
                    clear();
                    payload = other.payload;
                    handles = other.handles;
                    regions = other.regions;
                    payloadSize = std::exchange(other.payloadSize, 0);
                    handleCount = std::exchange(other.handleCount, 0);
                    regionCount = std::exchange(other.regionCount, 0);
                }
                return *this;
            }

            ~SerializedMessage() { clear(); }

            void clear() {
                for (std::size_t i = 0; i < handleCount; ++i) {
                    if (handles[i] >= 0)
                        close(handles[i]);
                    handles[i] = -1;
                }
                for (std::size_t i = 0; i < regionCount; ++i) {
                    if (regions[i].fd >= 0)
                        close(regions[i].fd);
                    regions[i].fd = -1;
                }
                payloadSize = 0;
                handleCount = 0;
                regionCount = 0;
            }

            [[nodiscard]] SerializerWriter writer() {
                return {
                    payload.data(), payload.size(),
                    handles, handleCount,
                    regions, regionCount
                };
            }

            [[nodiscard]] SerializerReader reader() {
                return {
                    payload.data(), payloadSize,
                    std::span(handles).first(handleCount),
                    std::span(regions).first(regionCount)
                };
            }
        };

        template<int Id, typename ... Args>
        struct SyncRequest {
            static_assert(Id >= 0, "IPC method IDs cannot be negative");

            std::uint32_t command = Id;
            std::tuple<Args...> arguments;

            [[nodiscard]] SerializedMessage serialize() const {
                SerializedMessage message;
                auto writer = message.writer();
                writer.write(command);
                std::apply([&](const auto &... values) {
                    (writer.write(values), ...);
                }, arguments);
                message.payloadSize = writer.size();
                return message;
            }
        };

        template<typename Ret>
        struct SyncResponse {
            using Value = WireValue<Ret>;

            static Value deserialize(SerializedMessage &message) {
                auto reader = message.reader();
                auto value = reader.read<Value>();
                reader.finish();
                return value;
            }
        };

        template<>
        struct SyncResponse<void> {
            static void deserialize(SerializedMessage &message) {
                auto reader = message.reader();
                reader.finish();
            }
        };

        template<typename ... Args>
        struct ArgumentReader;

        template<>
        struct ArgumentReader<> {
            static std::tuple<> read(SerializerReader &) { return {}; }
        };

        template<typename First, typename ... Rest>
        struct ArgumentReader<First, Rest...> {
            static auto read(SerializerReader &reader) {
                auto first = reader.read<WireValue<First>>();
                auto rest = ArgumentReader<Rest...>::read(reader);
                return std::tuple_cat(std::tuple<WireValue<First>>(std::move(first)), std::move(rest));
            }
        };

        template<typename Ret, typename ... Args>
        struct ServerInvoker {
            template<typename Callable>
            static void invoke(Callable &&callable, SerializedMessage &request,
                               SerializedMessage &response) {
                (validateWireArgument<Args>(), ...);
                if constexpr (!std::is_void_v<Ret>)
                    validateWireReturn<Ret>();

                auto reader = request.reader();
                reader.read<std::uint32_t>();
                auto arguments = ArgumentReader<Args...>::read(reader);
                reader.finish();

                if constexpr (std::is_void_v<Ret>) {
                    std::apply(std::forward<Callable>(callable), std::move(arguments));
                } else {
                    auto result = std::apply(std::forward<Callable>(callable), std::move(arguments));
                    auto writer = response.writer();
                    writer.write(result);
                    response.payloadSize = writer.size();
                }
            }
        };

        consteval std::meta::info serverInvokerType(std::meta::info method) {
            auto functionType = std::meta::type_of(method);
            auto templateParameters = std::meta::parameters_of(functionType);
            templateParameters.insert(templateParameters.begin(), std::meta::return_type_of(functionType));
            return std::meta::substitute(^^ServerInvoker, templateParameters);
        }

    }

}
