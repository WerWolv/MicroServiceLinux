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

    void log(std::string message) {
        static int kernelConsole = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
        if (kernelConsole >= 0) {
            dprintf(kernelConsole, "%s\n", message.c_str());
        }
    }

    void mountFileSystem(const char* source, const char* target, const char* type) {
        if (mount(source, target, type, 0, nullptr) < 0 && errno != EBUSY) {
            log("Failed to mount filesystem: " + std::string(target));
        }
    }

    void loadKernelModules() {
        log("Loading kernel modules...");
        for (const auto &modulePath : std::filesystem::directory_iterator("/modules")) {
            const int module = open(modulePath.path().c_str(), O_RDONLY | O_CLOEXEC);
            if (module < 0) {
                log("Failed to open kernel module " + modulePath.path().string() + ": " + std::string(strerror(errno)));
                continue;
            }
            if (syscall(SYS_finit_module, module, "", 0) < 0 && errno != EEXIST) {
                log("Failed to load kernel module " + modulePath.path().string() + ": " + std::string(strerror(errno)));
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
            log("Failed to execute program: " + programPath.string());
            std::exit(127);
        }
        close(service);

        log("Started program " + programPath.string() + " with PID " + std::to_string(child));
    }

    void loadServices() {
        log("Starting services...");
        for (const auto &service : std::filesystem::recursive_directory_iterator("/services")) {
            if (service.is_regular_file())
                executeProgram(service.path());
        }
    }

    void loadApplication() {
        log("Starting application...");
        for (const auto &application : std::filesystem::directory_iterator("/app")) {
            if (application.is_regular_file())
                executeProgram(application.path());
        }
    }

}

int main() {
    mountFileSystem("devtmpfs", "/dev", "devtmpfs");
    mountFileSystem("proc", "/proc", "proc");
    mountFileSystem("sysfs", "/sys", "sysfs");
    mountFileSystem("configfs", "/sys/kernel/config", "configfs");

    loadKernelModules();
    loadServices();
    loadApplication();

    for (;;) {
        int status = 0;
        const pid_t pid = waitpid(-1, &status, 0);
        if (pid < 0) {
            if (errno == EINTR)
                continue;
            if (errno == ECHILD) {
                pause();
                continue;
            }
            log("waitpid failed: " + std::string(strerror(errno)));
            continue;
        }

        if (WIFEXITED(status)) {
            log("Process " + std::to_string(pid) + " exited with status " + std::to_string(WEXITSTATUS(status)));
        } else if (WIFSIGNALED(status)) {
            log("Process " + std::to_string(pid) + " terminated by signal " + std::to_string(WTERMSIG(status)));
        }
    }
}
