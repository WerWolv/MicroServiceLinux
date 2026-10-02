#pragma once

#include <wolv/ipc/ipc.hpp>

#include <cstdint>
#include <type_traits>

namespace wolv::srv::display {

    enum class PixelFormat : std::uint32_t {
        Spectra6 = 1,
    };

    enum class Color : std::uint8_t {
        Black = 0x0,
        White = 0x1,
        Yellow = 0x2,
        Red = 0x3,
        Blue = 0x5,
        Green = 0x6,
    };

    // Row-major native portrait layout; even x is the high nibble.
    struct FramebufferInfo {
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t stride;
        std::uint32_t size;
        PixelFormat format;
    };

    static_assert(sizeof(FramebufferInfo) == 20);
    static_assert(std::is_trivially_copyable_v<FramebufferInfo>);

    struct Display : ipc::Service<Display> {
        constexpr static auto Identifier = "display";

        [[=ipc::MethodId(1)]]
        virtual FramebufferInfo info() = 0;

        [[=ipc::MethodId(2)]]
        virtual ipc::MemoryRegion framebuffer() = 0;

        [[=ipc::MethodId(3)]]
        virtual void update() = 0;

        [[=ipc::MethodId(4)]]
        virtual void updatePartial(std::uint32_t x, std::uint32_t y, std::uint32_t width, std::uint32_t height) = 0;
    };

}
