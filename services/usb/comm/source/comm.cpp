#include <usb/comm.hpp>

#include <filesystem>
#include <memory>
#include <utility>

#include <fs/fs.hpp>

#include "log/log.hpp"

template <>
struct ipc::Service<wolv::srv::usb::comm::IUSBChannel>::Server : Definition {
    explicit Server(std::string channelName, wolv::srv::fs::IFile::Client *usbInterface) : m_channelName(std::move(channelName)), m_usbInterface(usbInterface) {
    }

    void send(const std::vector<uint8_t> &data) override {
        m_usbInterface->writeBytes(0x00, data);
    }

    std::vector<uint8_t> receive(size_t size) override {
        return m_usbInterface->readBytes(0x00, size);
    }

private:
    std::string m_channelName;
    wolv::srv::fs::IFile::Client *m_usbInterface;
};

template <>
struct ipc::Service<wolv::srv::usb::comm::USBCommunication>::Server : Definition {
    Server() {
        usbInterface = setupUSBDevice();
    }

    wolv::srv::usb::comm::IUSBChannel::Handle openChannel(const std::string &name) override {
        auto channel = std::make_shared<wolv::srv::usb::comm::IUSBChannel::Server>(name, &usbInterface);
        return channel->createUnnamed();
    }

private:
    wolv::srv::fs::IFile::Client setupUSBDevice() {
        wolv::srv::fs::FileSystem::Client fs;
        wolv::srv::log::Logging::Client log;
        auto console = log.kernelConsole();

        try
        {
            const std::filesystem::path g = "/sys/kernel/config/usb_gadget/g1";
            fs.createDirectory((g / "strings/0x409").string());
            fs.createDirectory((g / "configs/c.1").string());
            fs.createDirectory((g / "functions/acm.usb0").string());

            {
                auto file = fs.openFile(g / "idVendor");
                file.writeString(0x00, "0x1337");
            }
            {
                auto file = fs.openFile(g / "idProduct");
                file.writeString(0x00, "0xDEAD");
            }

            {
                auto file = fs.openFile(g / "strings/0x409/serialnumber");
                file.writeString(0x00, "1234567890");
            }
            {
                auto file = fs.openFile(g / "strings/0x409/manufacturer");
                file.writeString(0x00, "WolvOS");
            }
            {
                auto file = fs.openFile(g / "strings/0x409/product");
                file.writeString(0x00, "WolvOS USB Gadget");
            }

            fs.createSymlink(g / "functions/acm.usb0", g / "configs/c.1/acm.usb0");

            {
                auto directories = fs.listDirectory("/sys/class/udc");
                if (directories.empty())
                {
                    console.fatal("No UDC device found. USB gadget will not be enabled.");
                    return Empty;
                }


                console.debug("Using UDC device: " + directories[0].name);

                auto file = fs.openFile(g / "UDC");
                file.writeString(0x00, directories[0].name);
            }

            return fs.openFile("/dev/ttyGS0");
        } catch (std::exception &e)
        {
            console.fatal(e.what());
            return Empty;
        }
    }

    wolv::srv::fs::IFile::Client usbInterface = ipc::Empty;
};

int main() {
    wolv::srv::usb::comm::USBCommunication::Server server;
    server.process(4);
}
