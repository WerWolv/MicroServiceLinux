#pragma once

#include <wolv/ipc/ipc.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace wolv::srv::spi {

    enum class Mode : std::uint32_t {
        Mode0 = 0,
        Mode1 = 1,
        Mode2 = 2,
        Mode3 = 3,
    };

    struct ISpiDevice : ipc::Service<ISpiDevice> {
        [[=ipc::MethodId(1)]]
        virtual void writeBytes(const std::vector<std::uint8_t> &data) = 0;

        [[=ipc::MethodId(2)]]
        virtual void writeRegion(const ipc::MemoryRegion &data) = 0;
    };

    struct Spi : ipc::Service<Spi> {
        constexpr static auto Identifier = "spi";

        [[=ipc::MethodId(1)]]
        virtual ISpiDevice::Handle openDevice(const std::string &name, Mode mode, std::uint8_t bitsPerWord, std::uint32_t speedHz) = 0;
    };

}
