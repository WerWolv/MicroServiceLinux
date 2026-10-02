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
        printf("Loading kernel modules...\n");
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
        printf("Executing program: %s\n", programPath.c_str());
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
            perror(programPath.c_str());
            std::exit(127);
        }
        close(service);
    }

    void loadServices() {
        printf("Starting services...\n");
        for (const auto &servicePath : std::filesystem::directory_iterator("/services")) {
            executeProgram(servicePath);
        }
    }

    void loadApplication() {
        printf("Starting application...\n");
        for (const auto &servicePath : std::filesystem::directory_iterator("/app")) {
            executeProgram(servicePath);
        }
    }

}

int main() {
    {
        int kernelConsole = open("/dev/kmsg", O_RDWR | O_CLOEXEC);
        dup2(kernelConsole, STDOUT_FILENO);
        dup2(kernelConsole, STDERR_FILENO);
        close(kernelConsole);

        std::setvbuf(stdout, nullptr, _IONBF, 0);
        std::setvbuf(stderr, nullptr, _IONBF, 0);
    }

    mountFileSystem("devtmpfs", "/dev", "devtmpfs");
    mountFileSystem("proc", "/proc", "proc");
    mountFileSystem("sysfs", "/sys", "sysfs");

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
            perror("waitpid");
            continue;
        }

        if (WIFEXITED(status)) {
            fprintf(stderr, "Process %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "Process %d terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }
}
