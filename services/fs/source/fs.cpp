#include <fs/fs.hpp>

#include <filesystem>
#include <fstream>
#include <memory>

template <>
struct ipc::Service<wolv::srv::fs::IFile>::Server : Definition {
    explicit Server(const std::string &path) {
        m_fd = open(path.c_str(), O_RDWR | O_CLOEXEC);
        if (m_fd < 0)
            m_fd = open(path.c_str(), O_CREAT | O_CLOEXEC | O_RDWR, S_IRWXU);

        m_seekable = isSeekable(m_fd);
    }

    ~Server() override {
        if (m_fd >= 0) {
            close(m_fd);
        }
    }

    std::vector<std::uint8_t> readBytes(std::uint64_t offset, std::uint64_t size) override {
        std::vector<std::uint8_t> buffer(size);
        ssize_t bytesRead = m_seekable ? pread64(m_fd, buffer.data(), size, offset) : read(m_fd, buffer.data(), size);
        if (bytesRead < 0) {
            throw std::system_error(errno, std::generic_category(), "read file");
        }
        buffer.resize(bytesRead);
        return buffer;
    }

    void writeBytes(std::uint64_t offset, const std::vector<std::uint8_t>& data) override {
        ssize_t bytesWritten = m_seekable ? pwrite64(m_fd, data.data(), data.size(), offset) : write(m_fd, data.data(), data.size());
        if (bytesWritten < 0) {
            throw std::system_error(errno, std::generic_category(), "write file");
        }
        if (static_cast<std::size_t>(bytesWritten) != data.size()) {
            throw std::runtime_error("incomplete write to file");
        }
    }

    void writeString(std::uint64_t offset, const std::string& string) override {
       writeBytes(offset, { string.begin(), string.end() });
    }

    std::uint64_t size() override {
        off_t fileSize = lseek(m_fd, 0, SEEK_END);
        if (fileSize < 0) {
            throw std::system_error(errno, std::generic_category(), "get file size");
        }
        return static_cast<std::uint64_t>(fileSize);
    }

private:
    [[nodiscard]] static bool isSeekable(int fd) {
        if (fd < 0)
            return false;

        const auto result = lseek(fd, 0, SEEK_CUR);
        if (result == off_t(-1) && errno == ESPIPE)
            return false;
        return (result != off_t(-1));
    }

    int m_fd = -1;
    bool m_seekable = true;
};

template <>
struct ipc::Service<wolv::srv::fs::FileSystem>::Server : Definition {

    wolv::srv::fs::IFile::Handle openFile(const std::string &path) override {
        auto file = std::make_shared<wolv::srv::fs::IFile::Server>(path);
        return file->createUnnamed();
    }

    bool createDirectory(const std::string &path) override {
        std::error_code ec;
        std::filesystem::create_directories(path, ec);
        return !ec;
    }

    bool createSymlink(const std::string &target, const std::string &linkPath) override {
        std::error_code ec;
        std::filesystem::create_symlink(target, linkPath, ec);
        return !ec;
    }

    std::vector<wolv::srv::fs::DirectoryEntry> listDirectory(const std::string& path) override {
        std::vector<wolv::srv::fs::DirectoryEntry> entries;
        std::error_code ec;
        for (const auto& entry : std::filesystem::directory_iterator(path, ec)) {
            if (ec) {
                throw std::system_error(ec.value(), std::generic_category(), "list directory");
            }

            if (entry.path().filename() == "." || entry.path().filename() == "..") {
                continue; // Skip current and parent directory entries
            }

            wolv::srv::fs::DirectoryEntry dirEntry;
            dirEntry.name = entry.path().filename().string();
            switch (entry.status().type()) {
                case std::filesystem::file_type::regular:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::RegularFile;
                    break;
                case std::filesystem::file_type::directory:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::Directory;
                    break;
                case std::filesystem::file_type::symlink:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::Symlink;
                    break;
                case std::filesystem::file_type::block:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::BlockDevice;
                    break;
                case std::filesystem::file_type::character:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::CharacterDevice;
                    break;
                case std::filesystem::file_type::fifo:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::NamedPipe;
                    break;
                case std::filesystem::file_type::socket:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::Socket;
                    break;
                default:
                    dirEntry.type = wolv::srv::fs::DirectoryEntry::Type::Unknown;
                    break;
            }
            entries.push_back(dirEntry);
        }
        return entries;

    }
};

int main() {
    wolv::srv::fs::FileSystem::Server server;
    server.process(4);
}
