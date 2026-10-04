#include <log/log.hpp>

#include <filesystem>
#include <fcntl.h>
#include <memory>
#include <mutex>

template <>
struct ipc::Service<wolv::srv::log::ILogger>::Server : Definition {
    explicit Server(const std::string &logPath) {
        fd = open(logPath.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    }

    ~Server() override {
        if (fd >= 0) {
            close(fd);
        }
    }

    void debug(const std::string &message) override {
        std::scoped_lock lock(logMutex);
        dprintf(fd, "[DEBUG] %s\n", message.c_str());
    }

    void info(const std::string &message) override {
        std::scoped_lock lock(logMutex);
        dprintf(fd, "[INFO] %s\n", message.c_str());
    }

    void warn(const std::string &message) override {
        std::scoped_lock lock(logMutex);
        dprintf(fd, "[WARN] %s\n", message.c_str());
    }

    void error(const std::string &message) override {
        std::scoped_lock lock(logMutex);
        dprintf(fd, "[ERROR] %s\n", message.c_str());
    }

    void fatal(const std::string &message) override {
        std::scoped_lock lock(logMutex);
        dprintf(fd, "[FATAL] %s\n", message.c_str());
    }

    int fd;
    std::mutex logMutex;
};

template <>
struct ipc::Service<wolv::srv::log::Logging>::Server : Definition
{
    wolv::srv::log::ILogger::Handle kernelConsole() override {
        return file("/dev/kmsg");
    }

    wolv::srv::log::ILogger::Handle file(const std::string &path) override {
        auto logger = std::make_shared<wolv::srv::log::ILogger::Server>(path);
        return logger->createUnnamed();
    }
};

int main() {
    wolv::srv::log::Logging::Server server;
    server.process(4);
}
