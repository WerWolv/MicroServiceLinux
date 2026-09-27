#include <fs/fs.hpp>

#include <mutex>
#include <fstream>

template <>
struct ipc::Service<wolv::srv::fs::IFile>::Server : Definition {
    explicit Server(std::string path) : path(std::move(path)) {}

    std::string read() override {
        std::ifstream stream(path);
        return { std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>() };
    }

    std::string path;
};

template <>
struct ipc::Service<wolv::srv::fs::FileSystem>::Server : Definition {
    wolv::srv::fs::IFile::Handle openFile(const std::string &path) override {
        auto file = std::make_unique<wolv::srv::fs::IFile::Server>(path);
        auto client = file->createUnnamed();
        std::scoped_lock lock(filesMutex);
        files.push_back(std::move(file));
        return client;
    }

    std::mutex filesMutex;
    std::vector<std::unique_ptr<wolv::srv::fs::IFile::Server>> files;
};

int main() {
    wolv::srv::fs::FileSystem::Server server;
    server.process(4);
}