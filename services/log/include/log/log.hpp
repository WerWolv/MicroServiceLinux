#pragma once

#include <wolv/ipc/ipc.hpp>

#include <filesystem>

namespace wolv::srv::log {

    struct ILogger : ipc::Service<ILogger> {
        [[=ipc::MethodId(1)]]
        virtual void debug(const std::string &message) = 0;

        [[=ipc::MethodId(2)]]
        virtual void info(const std::string &message) = 0;

        [[=ipc::MethodId(3)]]
        virtual void warn(const std::string &message) = 0;

        [[=ipc::MethodId(4)]]
        virtual void error(const std::string &message) = 0;

        [[=ipc::MethodId(5)]]
        virtual void fatal(const std::string &message) = 0;
    };

    struct Logging : ipc::Service<Logging> {
        constexpr static auto Identifier = "log";

        [[=ipc::MethodId(1)]]
        virtual ILogger::Handle kernelConsole() = 0;

        [[=ipc::MethodId(2)]]
        virtual ILogger::Handle file(const std::string &path) = 0;
    };
}