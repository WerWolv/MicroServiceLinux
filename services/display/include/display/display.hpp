#pragma once

#include <wolv/ipc/ipc.hpp>

#include <cstdint>
#include <type_traits>

namespace wolv::srv::display {

    enum class PixelFormat : std::uint32_t {
        Spectra6 = 1,
    };

    enum class Color : std::uint8_t {
        Black   = 0x00,
        White   = 0x01,
        Yellow  = 0x02,
        Red     = 0x03,
        Blue    = 0x05,
        Green   = 0x06,
        Brown   = 0x07
    };

    // Row-major native portrait layout; even x is the high nibble.
    struct FramebufferInfo {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t stride;
        std::uint32_t size;
        PixelFormat format;
    };

    struct [[=ipc::Serializable]] Framebuffer {
        void setPixel(std::uint32_t x, std::uint32_t y, Color color) {
            const auto index = y * stride + x / 2;
            auto &packedPixels = region.map()[index];
            const auto colorValue = static_cast<std::uint8_t>(color);
            if (x % 2 == 0)
                packedPixels = static_cast<std::byte>((std::to_integer<std::uint8_t>(packedPixels) & 0x0F) | (colorValue << 4));
            else
                packedPixels = static_cast<std::byte>((std::to_integer<std::uint8_t>(packedPixels) & 0xF0) | colorValue);
        }

        ipc::MemoryRegion region;
        std::uint32_t width, height, stride;
    };

    static_assert(sizeof(FramebufferInfo) == 20);
    static_assert(std::is_trivially_copyable_v<FramebufferInfo>);

    struct Display : ipc::Service<Display> {
        constexpr static auto Identifier = "display";

        [[=ipc::MethodId(1)]]
        virtual FramebufferInfo info() const = 0;

        [[=ipc::MethodId(2)]]
        virtual Framebuffer framebuffer() = 0;

        [[=ipc::MethodId(3)]]
        virtual void update() = 0;

        [[=ipc::MethodId(4)]]
        virtual void updatePartial(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) = 0;
    };

}
