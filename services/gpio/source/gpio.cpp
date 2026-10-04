#include <gpio/gpio.hpp>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

    [[noreturn]] void throwSystemError(std::string_view operation) {
        throw std::system_error(errno, std::generic_category(), std::string(operation));
    }

    struct LineLocation {
        std::filesystem::path chipPath;
        std::uint32_t offset;
    };

    LineLocation findLine(std::string_view name) {
        if (name.empty())
            throw std::invalid_argument("GPIO line name is empty");

        std::vector<std::filesystem::path> chipPaths;
        for (const auto &entry : std::filesystem::directory_iterator("/dev")) {
            const auto filename = entry.path().filename().string();
            if (filename.starts_with("gpiochip"))
                chipPaths.push_back(entry.path());
        }
        std::ranges::sort(chipPaths);

        std::optional<LineLocation> match;
        for (const auto &path : chipPaths) {
            ipc::FileDescriptor chip(::open(path.c_str(), O_RDONLY | O_CLOEXEC));
            if (!chip.valid())
                continue;

            gpiochip_info chipInfo = {};
            if (ioctl(chip.get(), GPIO_GET_CHIPINFO_IOCTL, &chipInfo) < 0)
                continue;

            for (std::uint32_t offset = 0; offset < chipInfo.lines; ++offset) {
                gpio_v2_line_info lineInfo = {};
                lineInfo.offset = offset;
                if (ioctl(chip.get(), GPIO_V2_GET_LINEINFO_IOCTL, &lineInfo) < 0)
                    throwSystemError("get GPIO line information");
                if (name != lineInfo.name)
                    continue;
                if (match.has_value())
                    throw std::runtime_error("GPIO line name is not unique: " + std::string(name));
                match = LineLocation(path, offset);
            }
        }

        if (!match.has_value())
            throw std::runtime_error("GPIO line was not found: " + std::string(name));
        return *match;
    }

    ipc::FileDescriptor requestLine(const std::string &lineName, bool output, bool initialValue) {
        const auto [chipPath, offset] = findLine(lineName);
        ipc::FileDescriptor chip(::open(chipPath.c_str(), O_RDONLY | O_CLOEXEC));
        if (!chip.valid())
            throwSystemError("open " + chipPath.string());

        gpio_v2_line_request request = {};
        request.offsets[0] = offset;
        request.num_lines = 1;
        std::strncpy(request.consumer, "gpio-service", sizeof(request.consumer) - 1);
        request.config.flags = output ? GPIO_V2_LINE_FLAG_OUTPUT : GPIO_V2_LINE_FLAG_INPUT;

        if (output) {
            request.config.num_attrs = 1;
            request.config.attrs[0].attr.id = GPIO_V2_LINE_ATTR_ID_OUTPUT_VALUES;
            request.config.attrs[0].attr.values = initialValue ? 1 : 0;
            request.config.attrs[0].mask = 1;
        }

        if (ioctl(chip.get(), GPIO_V2_GET_LINE_IOCTL, &request) < 0)
            throwSystemError("request GPIO line " + lineName);
        return ipc::FileDescriptor(request.fd);
    }

}

template <>
struct ipc::Service<wolv::srv::gpio::IGpioInput>::Server : Definition {
    explicit Server(const std::string &lineName)
        : descriptor(requestLine(lineName, false, false)) {}

    bool getValue() override {
        gpio_v2_line_values values = {
            .bits = 0,
            .mask = 1,
        };
        if (ioctl(descriptor.get(), GPIO_V2_LINE_GET_VALUES_IOCTL, &values) < 0)
            throwSystemError("get GPIO value");
        return (values.bits & 1) != 0;
    }

    ipc::FileDescriptor descriptor;
};

template <>
struct ipc::Service<wolv::srv::gpio::IGpioOutput>::Server : Definition {
    Server(const std::string &lineName, bool initialValue)
        : descriptor(requestLine(lineName, true, initialValue)) {}

    void setValue(bool value) override {
        gpio_v2_line_values values = {
            .bits = value ? 1UL : 0UL,
            .mask = 1,
        };
        if (ioctl(descriptor.get(), GPIO_V2_LINE_SET_VALUES_IOCTL, &values) < 0)
            throwSystemError("set GPIO value");
    }

    ipc::FileDescriptor descriptor;
};

template <>
struct ipc::Service<wolv::srv::gpio::Gpio>::Server : Definition {
    wolv::srv::gpio::IGpioInput::Handle requestInput(const std::string &lineName) override {
        auto line = std::make_shared<wolv::srv::gpio::IGpioInput::Server>(lineName);
        return line->createUnnamed();
    }

    wolv::srv::gpio::IGpioOutput::Handle requestOutput(const std::string &lineName,
                                                       bool initialValue) override {
        auto line = std::make_shared<wolv::srv::gpio::IGpioOutput::Server>(
            lineName, initialValue
        );
        return line->createUnnamed();
    }
};

int main() {
    wolv::srv::gpio::Gpio::Server server;
    server.process(4);
}
