#pragma once

#include <wolv/ipc/ipc_serialization.hpp>

namespace ipc {

    template<typename T> requires (
        (!std::is_pointer_v<T> &&
        !std::is_member_pointer_v<T> &&
        std::is_trivially_copyable_v<T> &&
        std::is_default_constructible_v<T>) ||
        std::is_enum_v<T>
    )
    struct Serializer<T> {
        static void serialize(SerializerWriter &writer, const T &value) {
            writer.writeBytes(std::as_bytes(std::span(&value, 1)));
        }

        static T deserialize(SerializerReader &reader) {
            T value {};
            const auto bytes = reader.readBytes(sizeof(value));
            std::memcpy(&value, bytes.data(), bytes.size());
            return value;
        }
    };

    template<typename T>
    struct Serializer<std::optional<T>> {
        static void serialize(SerializerWriter &writer, const std::optional<T> &value) {
            writer.write(static_cast<std::uint8_t>(value.has_value() ? 1 : 0));
            if (value.has_value())
                writer.write(value.value());
        }

        static std::optional<T> deserialize(SerializerReader &reader) {
            const auto hasValue = reader.read<std::uint8_t>();
            if (hasValue)
                return reader.read<T>();
            return std::nullopt;
        }
    };

    template<>
    struct Serializer<FileDescriptor> {
        static void serialize(SerializerWriter &writer, const FileDescriptor &descriptor) {
            writer.writeFileDescriptor(descriptor.get());
        }

        static FileDescriptor deserialize(SerializerReader &reader) {
            return FileDescriptor(reader.readFileDescriptor());
        }
    };

    template<>
    struct Serializer<MemoryRegion> {
        static void serialize(SerializerWriter &writer, const MemoryRegion &region) {
            writer.writeMemoryRegion({
                .fd = region.fileDescriptor().get(),
                .prot = region.prot(),
                .offset = region.offset(),
                .size = region.size(),
            });
        }

        static MemoryRegion deserialize(SerializerReader &reader) {
            const auto [fd, prot, offset, size] = reader.readMemoryRegion();
            return {
                FileDescriptor(fd),
                prot,
                offset, size
            };
        }
    };

    template<typename T> requires requires(T t) { t.begin(); t.end(); }
    struct Serializer<T> {
        static void serialize(SerializerWriter &writer, const T &value) {
            writer.write(static_cast<std::uint32_t>(std::size(value)));
            for (const auto &element : value)
                writer.write(element);
        }

        static T deserialize(SerializerReader &reader) {
            T value;
            value.resize(reader.read<std::uint32_t>());
            for (auto &element : value)
                element = reader.read<std::remove_cv_t<std::remove_reference_t<decltype(element)>>>();
            return value;
        }
    };

    constexpr static struct {} Serializable;

    template<typename T> requires (!std::meta::annotations_of_with_type(^^T, ^^decltype(Serializable)).empty())
    struct Serializer<T> {
        static void serialize(SerializerWriter &writer, const T &value) {
            template for (constexpr auto member : std::define_static_array(std::meta::members_of(^^T, std::meta::access_context::unprivileged()))) {
                if constexpr (std::meta::is_nonstatic_data_member(member)) {
                    writer.write(value.[: member :]);
                }
            }
        }

        static T deserialize(SerializerReader &reader) {
            T value;
            template for (constexpr auto member : std::define_static_array(std::meta::members_of(^^T, std::meta::access_context::unprivileged()))) {
                if constexpr (std::meta::is_nonstatic_data_member(member)) {
                    using MemberType = std::remove_cv_t<std::remove_reference_t<typename[: std::meta::type_of(member) :]>>;
                    auto memberValue = reader.read<MemberType>();
                    value.[: member :] = std::move(memberValue);
                }
            }
            return value;
        }
    };

}