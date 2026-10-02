#pragma once

#include <wolv/ipc/ipc.hpp>

#include <string>

namespace wolv::srv::gpio {

    struct IGpioInput : ipc::Service<IGpioInput> {
        [[=ipc::MethodId(1)]]
        virtual bool getValue() = 0;
    };

    struct IGpioOutput : ipc::Service<IGpioOutput> {
        [[=ipc::MethodId(1)]]
        virtual void setValue(bool value) = 0;
    };

    struct Gpio : ipc::Service<Gpio> {
        constexpr static auto Identifier = "gpio";

        [[=ipc::MethodId(1)]]
        virtual IGpioInput::Handle requestInput(const std::string &lineName) = 0;

        [[=ipc::MethodId(2)]]
        virtual IGpioOutput::Handle requestOutput(const std::string &lineName,
                                                  bool initialValue) = 0;
    };

}
