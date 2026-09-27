#include <log/log.hpp>

int main() {
    wolv::srv::log::Logging::Client client;
    auto console = client.kernelConsole();

    console.info("Hello World");

    while (true) {
        sleep(1);
    }
}