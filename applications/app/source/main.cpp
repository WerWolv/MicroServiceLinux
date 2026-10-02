#include <log/log.hpp>
#include <display/display.hpp>

#include <cstddef>
#include <cstdint>
#include <mdspan>
#include <stdexcept>

namespace {

    using Framebuffer = std::mdspan<std::byte, std::dextents<std::size_t, 2>>;

    void drawRect(Framebuffer framebuffer, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, wolv::srv::display::Color color) {
        const auto framebufferWidth = framebuffer.extent(1) * 2;
        const auto framebufferHeight = framebuffer.extent(0);
        if (x > framebufferWidth || width > framebufferWidth - x ||
            y > framebufferHeight || height > framebufferHeight - y)
            throw std::invalid_argument("rectangle is outside the framebuffer");

        const auto colorValue = static_cast<std::uint8_t>(color);
        for (std::uint32_t row = y; row < y + height; ++row) {
            for (std::uint32_t column = x; column < x + width; ++column) {
                auto &packedPixels = framebuffer[row, column / 2];
                auto value = std::to_integer<std::uint8_t>(packedPixels);
                if (column % 2 == 0)
                    value = static_cast<std::uint8_t>((value & 0x0F) | (colorValue << 4));
                else
                    value = static_cast<std::uint8_t>((value & 0xF0) | colorValue);
                packedPixels = static_cast<std::byte>(value);
            }
        }
    }

}

int main() {
    wolv::srv::display::Display::Client display;
    auto fb = display.framebuffer();
    auto info = display.info();
    auto mapping = fb.map();
    Framebuffer map(mapping.data(), info.height, info.stride);

    drawRect(map, 0, 0, info.width, info.height, wolv::srv::display::Color::Black);
    drawRect(map, 5, 5, info.width - 10, info.height - 10, wolv::srv::display::Color::Blue);
    display.update();

    while (true) {

    }
}
