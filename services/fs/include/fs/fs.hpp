#pragma once

#include <wolv/ipc/ipc.hpp>

namespace wolv::srv::fs {

    struct IFile : ipc::Service<IFile> {
        [[=ipc::MethodId(1)]]
        virtual std::vector<std::uint8_t> readBytes(std::uint64_t offset, std::uint64_t size) = 0;

        [[=ipc::MethodId(2)]]
        virtual void writeBytes(std::uint64_t offset, const std::vector<std::uint8_t> &data) = 0;

        [[=ipc::MethodId(3)]]
        virtual void writeString(std::uint64_t offset, const std::string &string) = 0;

        [[=ipc::MethodId(4)]]
        virtual std::uint64_t size() = 0;
    };

    struct [[=ipc::Serializable]] DirectoryEntry {
        std::string name;
        enum class Type : std::uint8_t {
            Unknown,
            NamedPipe,
            CharacterDevice,
            Directory,
            BlockDevice,
            RegularFile,
            Symlink,
            Socket
        } type;
    };

    struct FileSystem : ipc::Service<FileSystem> {
        constexpr static auto Identifier = "fs";

        [[=ipc::MethodId(1)]]
        virtual IFile::Handle openFile(const std::string &) = 0;

        [[=ipc::MethodId(2)]]
        virtual bool createDirectory(const std::string &path) = 0;

        [[=ipc::MethodId(3)]]
        virtual bool createSymlink(const std::string &target, const std::string &linkPath) = 0;

        [[=ipc::MethodId(4)]]
        virtual std::vector<DirectoryEntry> listDirectory(const std::string &path) = 0;
    };
}