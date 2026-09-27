#pragma once

#include <wolv/ipc/ipc.hpp>

namespace wolv::srv::fs {

    struct IFile : ipc::Service<IFile> {
        [[=ipc::MethodId(1)]]
        virtual std::string read() = 0;
    };

    struct FileSystem : ipc::Service<FileSystem> {
        constexpr static auto Identifier = "fs";

        [[=ipc::MethodId(1)]]
        virtual IFile::Handle openFile(const std::string &) = 0;
    };
}