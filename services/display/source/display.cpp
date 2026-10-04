#include <display/display.hpp>
#include <gpio/gpio.hpp>
#include <log/log.hpp>
#include <spi/spi.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <vector>

#include <fcntl.h>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <unistd.h>

namespace {

    using namespace std::chrono_literals;

    constexpr std::uint32_t DisplayWidth = 400;
    constexpr std::uint32_t DisplayHeight = 600;
    constexpr std::uint32_t FramebufferStride = DisplayWidth / 2;
    constexpr std::uint32_t FramebufferSize = FramebufferStride * DisplayHeight;
    constexpr std::uint32_t SpiSpeed = 4'000'000;

    enum class Command : std::uint8_t {
        PanelSetting                  = 0x00,
        PowerSetting                  = 0x01,
        PowerOff                      = 0x02,
        PowerOffSequenceSetting       = 0x03,
        PowerOn                       = 0x04,
        BoosterSoftStart1             = 0x05,
        BoosterSoftStart2             = 0x06,
        BoosterSoftStart3             = 0x08,
        DataTransmission              = 0x10,
        DisplayRefresh                = 0x12,
        PllControl                    = 0x30,
        VcomAndDataIntervalSetting    = 0x50,
        TconSetting                   = 0x60,
        ResolutionSetting             = 0x61,
        PartialWindow                 = 0x83,
        TemperatureVcomDcSetting      = 0x84,
        CommandHeader                 = 0xAA,
        PowerSaving                   = 0xE3,
    };

    struct UpdateRegion {
        std::uint32_t x;
        std::uint32_t y;
        std::uint32_t width;
        std::uint32_t height;

        [[nodiscard]] bool fullScreen() const {
            return x == 0 && y == 0 && width == DisplayWidth && height == DisplayHeight;
        }
    };

    enum class RefreshMode {
        Full,
        Partial,
    };

    constexpr std::array<std::uint8_t, 4> encodeResolution(std::uint32_t width, std::uint32_t height) {
        return {
            static_cast<std::uint8_t>(width >> 8),
            static_cast<std::uint8_t>(width),
            static_cast<std::uint8_t>(height >> 8),
            static_cast<std::uint8_t>(height),
        };
    }

    constexpr std::array<std::uint8_t, 9> encodePartialWindow(const UpdateRegion &region) {
        const auto xLast = region.x + region.width - 1;
        const auto yBoundary = region.y + region.height;

        return {
            static_cast<std::uint8_t>(region.x >> 8),
            static_cast<std::uint8_t>(region.x),
            static_cast<std::uint8_t>(xLast >> 8),
            static_cast<std::uint8_t>(xLast),
            static_cast<std::uint8_t>(region.y >> 8),
            static_cast<std::uint8_t>(region.y),
            static_cast<std::uint8_t>(yBoundary >> 8),
            static_cast<std::uint8_t>(yBoundary),
            0x01,
        };
    }

    constexpr std::array<std::uint8_t, 1> encodeVcomAndDataInterval(RefreshMode mode) {
        return { mode == RefreshMode::Full ? std::uint8_t(0x3F) : std::uint8_t(0xFF) };
    }

    [[noreturn]] void throwSystemError(std::string_view operation) {
        throw std::system_error(errno, std::generic_category(), std::string(operation));
    }

    class EpaperDisplay {
    public:
        EpaperDisplay(wolv::srv::gpio::Gpio::Client &gpio, wolv::srv::spi::Spi::Client &spiService)
            : power(gpio.requestOutput("epaper-power", false)),
              reset(gpio.requestOutput("epaper-reset", false)),
              dataCommand(gpio.requestOutput("epaper-dc", false)),
              busy(gpio.requestInput("epaper-busy")),
              spi(spiService.openDevice(
                  "epaper0", wolv::srv::spi::Mode::Mode0, 8, SpiSpeed
            )
        ) {
            power.setValue(true);
            std::this_thread::sleep_for(10ms);
            initialize();
        }

        void update(const wolv::srv::display::Framebuffer &framebuffer, const UpdateRegion &region) {
            if (region.width == 0 || region.height == 0 || region.x >= DisplayWidth ||
                region.y >= DisplayHeight || region.width > DisplayWidth - region.x ||
                region.height > DisplayHeight - region.y || region.x % 4 != 0 ||
                region.width % 4 != 0)
                throw std::invalid_argument("invalid e-paper update region");
            if (framebuffer.region.size() != region.width / 2 * region.height)
                throw std::invalid_argument("invalid e-paper framebuffer size");

            setUpdateRegion(region);
            sendCommand(Command::DataTransmission);
            // Two horizontal pixels per byte: the left pixel is in the high nibble.
            sendData(framebuffer.region);

            sendCommand(Command::PowerOn);
            waitUntilReady(40s, "power-on");
            std::this_thread::sleep_for(200ms);
            sendCommand(Command::BoosterSoftStart2, { 0x6F, 0x1F, 0x17, 0x27 });
            std::this_thread::sleep_for(200ms);
            setUpdateRegion(region);
            sendCommand(
                Command::VcomAndDataIntervalSetting,
                encodeVcomAndDataInterval(region.fullScreen() ? RefreshMode::Full : RefreshMode::Partial)
            );
            sendCommand(Command::DisplayRefresh, { 0x00 });
            waitUntilReady(40s, "refresh");
            sendCommand(Command::PowerOff, { 0x00 });
            waitUntilReady(40s, "power-off");
        }

    private:
        void initialize() {
            reset.setValue(true);
            std::this_thread::sleep_for(20ms);
            reset.setValue(false);
            std::this_thread::sleep_for(2ms);
            reset.setValue(true);
            std::this_thread::sleep_for(20ms);
            waitUntilReady(10s, "reset");
            std::this_thread::sleep_for(30ms);

            // The public panel documentation does not expose these payload layouts.
            sendCommand(Command::CommandHeader, { 0x49, 0x55, 0x20, 0x08, 0x09, 0x18 });
            sendCommand(Command::PowerSetting, { 0x3F });
            sendCommand(Command::PanelSetting, { 0x5F, 0x69 });
            sendCommand(Command::BoosterSoftStart1, { 0x40, 0x1F, 0x1F, 0x2C });
            sendCommand(Command::BoosterSoftStart3, { 0x6F, 0x1F, 0x1F, 0x22 });
            sendCommand(Command::BoosterSoftStart2, { 0x6F, 0x1F, 0x17, 0x17 });
            sendCommand(Command::PowerOffSequenceSetting, { 0x00, 0x54, 0x00, 0x44 });
            sendCommand(Command::TconSetting, { 0x02, 0x00 });
            sendCommand(Command::PllControl, { 0x08 });
            sendCommand(
                Command::VcomAndDataIntervalSetting,
                encodeVcomAndDataInterval(RefreshMode::Full)
            );
            sendCommand(
                Command::ResolutionSetting,
                encodeResolution(DisplayWidth, DisplayHeight)
            );
            sendCommand(Command::PowerSaving, { 0x2F });
            sendCommand(Command::TemperatureVcomDcSetting, { 0x01 });
            waitUntilReady(10s, "initialization");
        }

        void setUpdateRegion(const UpdateRegion &region) {
            sendCommand(Command::PartialWindow, encodePartialWindow(region));
        }

        void sendCommand(Command command) {
            dataCommand.setValue(false);
            writeBytes({ static_cast<std::uint8_t>(command) });
        }

        void sendCommand(Command command, std::initializer_list<std::uint8_t> data) {
            sendCommand(command);
            dataCommand.setValue(true);
            writeBytes(data);
        }

        template <std::size_t Size>
        void sendCommand(Command command, const std::array<std::uint8_t, Size> &data) {
            sendCommand(command);
            dataCommand.setValue(true);
            writeBytes(std::vector<std::uint8_t>(data.begin(), data.end()));
        }

        void sendData(const ipc::MemoryRegion &data) {
            dataCommand.setValue(true);
            spi.writeRegion(data);
        }

        void writeBytes(const std::vector<std::uint8_t> &data) {
            spi.writeBytes(data);
        }

        void waitUntilReady(std::chrono::steady_clock::duration timeout, std::string_view phase) {
            const auto deadline = std::chrono::steady_clock::now() + timeout;
            while (!busy.getValue()) {
                if (std::chrono::steady_clock::now() >= deadline)
                    throw std::runtime_error(
                        "timed out waiting for e-paper display during " + std::string(phase)
                    );
                std::this_thread::sleep_for(10ms);
            }
            std::this_thread::sleep_for(100ms);
        }

        wolv::srv::gpio::IGpioOutput::Handle power;
        wolv::srv::gpio::IGpioOutput::Handle reset;
        wolv::srv::gpio::IGpioOutput::Handle dataCommand;
        wolv::srv::gpio::IGpioInput::Handle busy;
        wolv::srv::spi::ISpiDevice::Handle spi;
    };

    ipc::FileDescriptor createBuffer(const char *name) {
        ipc::FileDescriptor descriptor(memfd_create(name, MFD_CLOEXEC));
        if (!descriptor.valid())
            throwSystemError("create e-paper framebuffer");
        if (ftruncate(descriptor.get(), FramebufferSize) < 0)
            throwSystemError("size e-paper framebuffer");
        return descriptor;
    }

    std::byte *mapFramebuffer(int descriptor) {
        void *mapping = mmap(
            nullptr,
            FramebufferSize,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            descriptor,
            0x00
        );

        if (mapping == MAP_FAILED)
            throwSystemError("map e-paper framebuffer");

        return static_cast<std::byte *>(mapping);
    }

}

template <>
struct ipc::Service<wolv::srv::display::Display>::Server : Definition {
    Server(wolv::srv::log::ILogger::Handle &logger,
           wolv::srv::gpio::Gpio::Client &gpio,
           wolv::srv::spi::Spi::Client &spi)
        : logger(logger),
          framebufferDescriptor(createBuffer("epaper-framebuffer")),
          framebufferMapping(mapFramebuffer(framebufferDescriptor.get())),
          transferDescriptor(createBuffer("epaper-transfer")),
          transferMapping(mapFramebuffer(transferDescriptor.get())),
          display(gpio, spi) {

        std::memset(framebufferMapping, 0x11, FramebufferSize);
        display.update(shareFramebuffer(PROT_READ), {
            .x = 0,
            .y = 0,
            .width = DisplayWidth,
            .height = DisplayHeight,
        });
        std::this_thread::sleep_for(1s);
    }

    ~Server() override {
        munmap(transferMapping, FramebufferSize);
        munmap(framebufferMapping, FramebufferSize);
    }

    wolv::srv::display::FramebufferInfo info() const override {
        return {
            .width = DisplayWidth,
            .height = DisplayHeight,
            .stride = FramebufferStride,
            .size = FramebufferSize,
            .format = wolv::srv::display::PixelFormat::Spectra6,
        };
    }

    wolv::srv::display::Framebuffer framebuffer() override {
        return shareFramebuffer(PROT_READ | PROT_WRITE);
    }

    wolv::srv::display::Framebuffer shareFramebuffer(std::uint32_t protection) const {
        return shareBuffer(framebufferDescriptor, protection, FramebufferSize);
    }

    wolv::srv::display::Framebuffer shareBuffer(const ipc::FileDescriptor &descriptor, std::uint32_t protection, std::uint32_t size) const {
        const int duplicate = fcntl(descriptor.get(), F_DUPFD_CLOEXEC, 0);
        if (duplicate < 0)
            throwSystemError("duplicate e-paper framebuffer");
        auto fbInfo = info();
        return wolv::srv::display::Framebuffer{
            .region = ipc::MemoryRegion(
                ipc::FileDescriptor(duplicate),
                protection,
                0,
                size
            ),
            .width = fbInfo.width,
            .height = fbInfo.height,
            .stride = fbInfo.stride,
        };
    }

    void update() override {
        updatePartial(0, 0, DisplayWidth, DisplayHeight);
    }

    void updatePartial(std::uint32_t x, std::uint32_t y,
                       std::uint32_t width, std::uint32_t height) override {
        std::scoped_lock lock(updateMutex);
        if (width == 0 || height == 0 || x >= DisplayWidth || y >= DisplayHeight ||
            width > DisplayWidth - x || height > DisplayHeight - y)
            throw std::invalid_argument("invalid e-paper update region");

        const auto right = x + width;
        const auto alignedX = x - x % 4;
        const auto alignedRight = std::min(DisplayWidth, (right + 3) / 4 * 4);
        const UpdateRegion region {
            .x = alignedX,
            .y = y,
            .width = alignedRight - alignedX,
            .height = height,
        };

        const bool partial = !region.fullScreen();
        if (partial) {
            const auto sourceX = region.x / 2;
            const auto rowSize = region.width / 2;
            for (std::uint32_t row = 0; row < region.height; ++row) {
                std::memcpy(
                    transferMapping + row * rowSize,
                    framebufferMapping + (region.y + row) * FramebufferStride + sourceX,
                    rowSize
                );
            }
        }

        try {
            if (partial) {
                display.update(
                    shareBuffer(transferDescriptor, PROT_READ, region.width / 2 * region.height),
                    region
                );
            } else {
                display.update(shareFramebuffer(PROT_READ), region);
            }
        } catch (const std::exception &error) {
            logger.error("display: refresh failed: " + std::string(error.what()));
            throw;
        }
    }

    wolv::srv::log::ILogger::Handle &logger;
    ipc::FileDescriptor framebufferDescriptor;
    std::byte *framebufferMapping;
    ipc::FileDescriptor transferDescriptor;
    std::byte *transferMapping;
    EpaperDisplay display;
    std::mutex updateMutex;
};

int main() {
    wolv::srv::log::Logging::Client logging;
    auto logger = logging.kernelConsole();

    try {
        wolv::srv::gpio::Gpio::Client gpio;
        wolv::srv::spi::Spi::Client spi;
        wolv::srv::display::Display::Server server(logger, gpio, spi);
        server.process(4);
    } catch (const std::exception &error) {
        logger.fatal("display: startup failed: " + std::string(error.what()));
        return 1;
    }
}
