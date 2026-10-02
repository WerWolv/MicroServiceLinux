#include <spi/spi.hpp>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace {

    using namespace std::chrono_literals;

    [[noreturn]] void throwSystemError(std::string_view operation) {
        throw std::system_error(errno, std::generic_category(), std::string(operation));
    }

    void writeSysfs(const std::filesystem::path &path, std::string_view value) {
        std::ofstream stream(path);
        stream << value;
        stream.close();
        if (!stream)
            throw std::runtime_error("failed to write " + path.string());
    }

    std::string readDeviceTreeString(const std::filesystem::path &path) {
        std::ifstream stream(path, std::ios::binary);
        if (!stream)
            throw std::runtime_error("failed to read " + path.string());
        std::string value{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()
        };
        while (!value.empty() && value.back() == '\0')
            value.pop_back();
        return value;
    }

    std::uint32_t readDeviceTreeU32(const std::filesystem::path &path) {
        std::ifstream stream(path, std::ios::binary);
        std::array<unsigned char, 4> bytes = {};
        if (!stream.read(reinterpret_cast<char *>(bytes.data()), bytes.size()))
            throw std::runtime_error("failed to read " + path.string());
        return static_cast<std::uint32_t>(bytes[0]) << 24 |
               static_cast<std::uint32_t>(bytes[1]) << 16 |
               static_cast<std::uint32_t>(bytes[2]) << 8 |
               static_cast<std::uint32_t>(bytes[3]);
    }

    std::optional<std::string> findSpiDevice(
        const std::filesystem::path &aliasNode,
        std::uint32_t chipSelect,
        std::optional<std::string> &expectedDevice
    ) {
        constexpr auto SpiDevices = "/sys/bus/spi/devices";
        for (const auto &entry : std::filesystem::directory_iterator(SpiDevices)) {
            const auto deviceName = entry.path().filename().string();
            if (!deviceName.starts_with("spi") || !deviceName.contains('.'))
                continue;
            const auto ofNode = entry.path() / "of_node";
            if (std::filesystem::exists(ofNode) &&
                std::filesystem::canonical(ofNode) == aliasNode)
                return deviceName;
        }

        constexpr auto SpiMasters = "/sys/class/spi_master";
        if (!std::filesystem::exists(SpiMasters))
            return std::nullopt;
        const auto controllerNode = aliasNode.parent_path();
        for (const auto &entry : std::filesystem::directory_iterator(SpiMasters)) {
            const auto masterName = entry.path().filename().string();
            if (!masterName.starts_with("spi"))
                continue;
            const auto ofNode = entry.path() / "device/of_node";
            if (!std::filesystem::exists(ofNode) ||
                std::filesystem::canonical(ofNode) != controllerNode)
                continue;

            expectedDevice = masterName + "." + std::to_string(chipSelect);
            if (std::filesystem::exists(
                    std::filesystem::path(SpiDevices) / *expectedDevice
                ))
                return expectedDevice;
        }
        return std::nullopt;
    }

    std::string resolveSpiDevice(std::string_view name) {
        if (name.empty() || name.find('/') != std::string_view::npos)
            throw std::invalid_argument("invalid SPI device name");

        constexpr auto DeviceTree = "/sys/firmware/devicetree/base";
        const auto nodePath = readDeviceTreeString(
            std::filesystem::path(DeviceTree) / "aliases" / name
        );
        if (nodePath.empty() || nodePath.front() != '/')
            throw std::runtime_error("invalid device-tree alias: " + std::string(name));
        const auto aliasNode = std::filesystem::canonical(
            std::filesystem::path(DeviceTree) / nodePath.substr(1)
        );
        const auto chipSelect = readDeviceTreeU32(aliasNode / "reg");

        std::optional<std::string> expectedDevice;
        for (unsigned attempt = 0; attempt < 200; ++attempt) {
            if (auto device = findSpiDevice(aliasNode, chipSelect, expectedDevice))
                return *device;
            std::this_thread::sleep_for(10ms);
        }

        if (expectedDevice.has_value())
            throw std::runtime_error(
                "SPI child for alias " + std::string(name) +
                " was not registered as " + *expectedDevice
            );
        throw std::runtime_error(
            "SPI controller for alias " + std::string(name) + " was not registered"
        );
    }

    ipc::FileDescriptor openSpiDevice(std::string_view name) {
        const auto deviceName = resolveSpiDevice(name);
        const auto suffix = deviceName.substr(3);

        const auto devicePath = "/dev/spidev" + suffix;

        if (!std::filesystem::exists(devicePath)) {
            const auto sysfsDevice = std::filesystem::path("/sys/bus/spi/devices") / deviceName;
            writeSysfs(sysfsDevice / "driver_override", "spidev");
            writeSysfs("/sys/bus/spi/drivers/spidev/bind", deviceName);

            for (unsigned attempt = 0;
                 attempt < 100 && !std::filesystem::exists(devicePath);
                 ++attempt)
                std::this_thread::sleep_for(10ms);
        }

        ipc::FileDescriptor descriptor(::open(devicePath.c_str(), O_WRONLY | O_CLOEXEC));
        if (!descriptor.valid())
            throwSystemError("open " + devicePath);
        return descriptor;
    }

}

template <>
struct ipc::Service<wolv::srv::spi::ISpiDevice>::Server : Definition {
    Server(const std::string &name, wolv::srv::spi::Mode mode,
           std::uint8_t bitsPerWord, std::uint32_t speedHz)
        : descriptor(openSpiDevice(name)) {
        if (mode < wolv::srv::spi::Mode::Mode0 || mode > wolv::srv::spi::Mode::Mode3)
            throw std::invalid_argument("invalid SPI mode");
        if (bitsPerWord == 0 || speedHz == 0)
            throw std::invalid_argument("invalid SPI configuration");

        auto rawMode = static_cast<std::uint32_t>(mode);
        if (ioctl(descriptor.get(), SPI_IOC_WR_MODE32, &rawMode) < 0)
            throwSystemError("set SPI mode");
        if (ioctl(descriptor.get(), SPI_IOC_WR_BITS_PER_WORD, &bitsPerWord) < 0)
            throwSystemError("set SPI bits per word");
        if (ioctl(descriptor.get(), SPI_IOC_WR_MAX_SPEED_HZ, &speedHz) < 0)
            throwSystemError("set SPI speed");
    }

    void writeBytes(const std::vector<std::uint8_t> &data) override {
        writeAll(std::as_bytes(std::span(data)));
    }

    void writeRegion(const ipc::MemoryRegion &data) override {
        if ((data.prot() & PROT_READ) == 0)
            throw std::invalid_argument("SPI memory region is not readable");
        if (data.size() > std::numeric_limits<std::size_t>::max())
            throw std::length_error("SPI memory region is too large");

        auto mapping = data.map();
        try {
            writeAll(mapping);
        } catch (...) {
            munmap(mapping.data(), mapping.size());
            throw;
        }
        if (munmap(mapping.data(), mapping.size()) < 0)
            throwSystemError("unmap SPI memory region");
    }

    void writeAll(std::span<const std::byte> data) {
        while (!data.empty()) {
            const auto chunkSize = std::min<std::size_t>(data.size(), 4096);
            const auto written = ::write(descriptor.get(), data.data(), chunkSize);
            if (written < 0) {
                if (errno == EINTR)
                    continue;
                throwSystemError("write SPI data");
            }
            if (written == 0)
                throw std::runtime_error("SPI write made no progress");
            data = data.subspan(static_cast<std::size_t>(written));
        }
    }

    ipc::FileDescriptor descriptor;
};

template <>
struct ipc::Service<wolv::srv::spi::Spi>::Server : Definition {
    wolv::srv::spi::ISpiDevice::Handle openDevice(const std::string &name,
                                                  wolv::srv::spi::Mode mode,
                                                  std::uint8_t bitsPerWord,
                                                  std::uint32_t speedHz) override {
        auto device = std::make_unique<wolv::srv::spi::ISpiDevice::Server>(
            name, mode, bitsPerWord, speedHz
        );
        auto client = device->createUnnamed();
        std::scoped_lock lock(devicesMutex);
        devices.push_back(std::move(device));
        return client;
    }

    std::mutex devicesMutex;
    std::vector<std::unique_ptr<wolv::srv::spi::ISpiDevice::Server>> devices;
};

int main() {
    wolv::srv::spi::Spi::Server server;
    server.process(4);
}
