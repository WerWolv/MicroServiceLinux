#define _GNU_SOURCE

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

namespace {

    void mountFileSystem(const char* source, const char* target, const char* type) {
        if (mount(source, target, type, 0, nullptr) < 0 && errno != EBUSY) {
            perror(target);
        }
    }

    void loadKernelModules() {
        for (const auto &modulePath : std::filesystem::directory_iterator("/modules")) {
            const int module = open(modulePath.path().c_str(), O_RDONLY | O_CLOEXEC);
            if (module < 0) {
                continue;
            }
            if (syscall(SYS_finit_module, module, "", 0) < 0 && errno != EEXIST) {
                perror("Failed to load kernel module");
            }
            close(module);
        }
    }

    void executeProgram(const std::filesystem::path &programPath) {
        const int service = open(programPath.c_str(), O_RDONLY | O_CLOEXEC);
        if (service < 0) {
            return;
        }

        const pid_t child = fork();
        if (child < 0) {
            throw std::runtime_error("Failed to load service " + programPath.string());
        }
        if (child == 0) {
            execl(programPath.c_str(), programPath.filename().c_str(), nullptr);
            throw std::runtime_error("Failed to execute service " + programPath.string());
        }
        close(service);
    }

    void loadServices() {
        for (const auto &servicePath : std::filesystem::directory_iterator("/services")) {
            executeProgram(servicePath);
        }
    }

    void loadApplication() {
        for (const auto &servicePath : std::filesystem::directory_iterator("/app")) {
            executeProgram(servicePath);
        }
    }

}

int main() {
    mountFileSystem("devtmpfs", "/dev", "devtmpfs");
    mountFileSystem("proc", "/proc", "proc");
    mountFileSystem("sysfs", "/sys", "sysfs");

    loadKernelModules();
    loadServices();
    loadApplication();

    int status = 0;
    pid_t pid = waitpid(-1, &status, 0);
    throw std::runtime_error("Service with PID " + std::to_string(pid) + " exited with status " + strerror(status));
}
