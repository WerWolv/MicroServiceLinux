#pragma once

#include <wolv/ipc/ipc.hpp>

#include <filesystem>

namespace wolv::srv::usb::comm {

    struct IUSBChannel : ipc::Service<IUSBChannel> {
        [[=ipc::MethodId(1)]]
        virtual void send(const std::vector<uint8_t> &data) = 0;

        [[=ipc::MethodId(2)]]
        virtual std::vector<uint8_t> receive(size_t size) = 0;
    };

    struct USBCommunication : ipc::Service<USBCommunication> {
        constexpr static auto Identifier = "usb:comm";

        [[=ipc::MethodId(1)]]
        virtual IUSBChannel::Handle openChannel(const std::string &name) = 0;
    };
}