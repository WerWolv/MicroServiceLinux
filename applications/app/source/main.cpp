#include <log/log.hpp>
#include <display/display.hpp>
#include <usb/comm.hpp>

#include <cstdint>
#include <mdspan>

namespace {

    void drawRect(wolv::srv::display::Framebuffer &framebuffer, std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height, wolv::srv::display::Color color) {
        for (std::uint32_t row = 0; row < height; ++row) {
            for (std::uint32_t col = 0; col < width; ++col) {
                const auto px = x + col;
                const auto py = y + row;
                if (px < framebuffer.width && py < framebuffer.height) {
                    framebuffer.setPixel(px, py, color);
                }
            }
        }
    }

}

int main() {
    wolv::srv::usb::comm::USBCommunication::Client comm;

    auto channel = comm.openChannel("test");
    channel.send({ 0x41, 0x42, 0x43, 0x44, '\r', '\n' });

    while (true) {

    }
}
