import shutil
from pathlib import Path

from SCons.Script import Action, AlwaysBuild

from ..model import Bootloader, InitramfsFile, KBuildCompilationDatabase, Kernel
from ..process import process_environment, run


def make_command(environment, targets, *, extra_variables=(), parallel=False):
    command = ["make"]
    if parallel:
        command.append(f"-j{environment['BUILD_JOBS']}")
    command.extend(["-C", environment["KBUILD_SOURCE"]])
    command.append(f"O={environment['KBUILD_OUTPUT']}")
    command.extend(environment["KBUILD_VARIABLES"])
    command.extend(extra_variables)
    command.extend(targets)
    return command


def configure_linux(target, source, env):
    build_directory = Path(env["KBUILD_OUTPUT"])
    build_directory.mkdir(parents=True, exist_ok=True)
    shutil.copy2(env["KCONFIG_FILE"], build_directory / ".config")
    return run(make_command(env, ["olddefconfig"]), env=process_environment(env))


def configure_uboot(target, source, env):
    Path(env["KBUILD_OUTPUT"]).mkdir(parents=True, exist_ok=True)
    return run(
        make_command(env, [env["KCONFIG_TARGET"]]),
        env=process_environment(env),
    )


def run_kbuild(target, source, env):
    Path(env["KBUILD_OUTPUT"]).mkdir(parents=True, exist_ok=True)
    return run(
        make_command(env, env["KBUILD_TARGETS"], parallel=True),
        env=process_environment(env),
    )


def run_kernel_module(target, source, env):
    module_output = Path(env["MODULE_OUTPUT"])
    module_output.mkdir(parents=True, exist_ok=True)
    variables = [
        f"M={env['MODULE_SOURCE']}",
        f"MO={module_output}",
    ]
    return run(
        make_command(env, ["modules"], extra_variables=variables, parallel=True),
        env=process_environment(env),
    )


def describe_linux_configuration(target, source, env):
    return "KCONFIG Linux for STM32MP157-DK2"


def describe_uboot_configuration(target, source, env):
    return "KCONFIG U-Boot for STM32MP157-DK2"


def describe_kbuild(target, source, env):
    return f"KBUILD  {env['KBUILD_DESCRIPTION']}"


def describe_module(target, source, env):
    return f"KBUILD  {env['MODULE_NAME']}.ko"


class KBuildRules:
    def linux_kernel(
        self,
        *,
        name,
        source,
        config,
        architecture,
        device_tree,
        toolchain,
    ):
        source_directory = self.source(source)
        build_directory = self.output("linux")
        config_node = self.environment.File(str(build_directory / ".config"))
        variables = [f"ARCH={architecture}", f"CROSS_COMPILE={toolchain.cross_compile}"]
        configure_environment = self.environment.Clone(
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KCONFIG_FILE=str(self.source(config)),
        )
        configured = configure_environment.Command(
            config_node,
            [
                self.environment.File(str(self.source(config))),
                self.environment.File(str(source_directory / "Kconfig")),
                *toolchain.outputs,
            ],
            Action(
                configure_linux,
                describe_linux_configuration,
                varlist=["KBUILD_SOURCE", "KBUILD_OUTPUT", "KBUILD_VARIABLES", "KCONFIG_FILE"],
            ),
        )

        outputs = (
            self.environment.File(str(build_directory / "arch/arm/boot/zImage")),
            self.environment.File(str(build_directory / f"arch/arm/boot/dts/{device_tree}.dtb")),
            self.environment.File(str(build_directory / "Module.symvers")),
            self.environment.File(str(build_directory / "usr/gen_init_cpio")),
        )
        build_environment = self.environment.Clone(
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KBUILD_TARGETS=[
                "zImage",
                f"{device_tree}.dtb",
                "modules",
                "usr_gen_init_cpio",
            ],
            KBUILD_DESCRIPTION="Linux for STM32MP157-DK2",
            BUILD_JOBS=self.jobs,
        )
        built = build_environment.Command(
            outputs,
            [configured, *toolchain.outputs],
            Action(
                run_kbuild,
                describe_kbuild,
                varlist=[
                    "KBUILD_SOURCE",
                    "KBUILD_OUTPUT",
                    "KBUILD_VARIABLES",
                    "KBUILD_TARGETS",
                    "BUILD_JOBS",
                ],
            ),
        )
        AlwaysBuild(built)
        build_environment.SideEffect(self.kbuild_lock, built)
        compilation_generator = self.environment.File(
            str(source_directory / "scripts/clang-tools/gen_compile_commands.py")
        )
        target = self._register(
            name,
            built,
            kbuild_compilation_databases=(
                KBuildCompilationDatabase(
                    compilation_generator,
                    build_directory,
                    (build_directory,),
                ),
            ),
        )
        return Kernel(
            target=target,
            source_directory=source_directory,
            build_directory=build_directory,
            image=outputs[0],
            device_tree=outputs[1],
            module_symbols=outputs[2],
            gen_init_cpio=outputs[3],
        )

    def uboot(
        self,
        *,
        name,
        source,
        configuration,
        architecture,
        device_tree,
        toolchain,
    ):
        source_directory = self.source(source)
        build_directory = self.output("u-boot")
        config_node = self.environment.File(str(build_directory / ".config"))
        variables = [
            f"ARCH={architecture}",
            f"DEVICE_TREE={device_tree}",
            f"CROSS_COMPILE={toolchain.cross_compile}",
        ]
        configure_environment = self.environment.Clone(
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KCONFIG_TARGET=configuration,
        )
        configured = configure_environment.Command(
            config_node,
            [
                self.environment.File(str(source_directory / "Kconfig")),
                self.environment.File(str(source_directory / "configs" / configuration)),
                *toolchain.outputs,
            ],
            Action(
                configure_uboot,
                describe_uboot_configuration,
                varlist=["KBUILD_SOURCE", "KBUILD_OUTPUT", "KBUILD_VARIABLES", "KCONFIG_TARGET"],
            ),
        )

        outputs = (
            self.environment.File(str(build_directory / "u-boot-spl.stm32")),
            self.environment.File(str(build_directory / "u-boot.img")),
        )
        build_environment = self.environment.Clone(
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KBUILD_TARGETS=[],
            KBUILD_DESCRIPTION="U-Boot for STM32MP157-DK2",
            BUILD_JOBS=self.jobs,
        )
        built = build_environment.Command(
            outputs,
            [configured, *toolchain.outputs],
            Action(
                run_kbuild,
                describe_kbuild,
                varlist=[
                    "KBUILD_SOURCE",
                    "KBUILD_OUTPUT",
                    "KBUILD_VARIABLES",
                    "KBUILD_TARGETS",
                    "BUILD_JOBS",
                ],
            ),
        )
        AlwaysBuild(built)
        build_environment.SideEffect(self.kbuild_lock, built)
        compilation_generator = self.environment.File(
            str(source_directory / "scripts/gen_compile_commands.py")
        )
        target = self._register(
            name,
            built,
            kbuild_compilation_databases=(
                KBuildCompilationDatabase(
                    compilation_generator,
                    build_directory,
                    (build_directory,),
                ),
            ),
        )
        return Bootloader(target=target, spl=outputs[0], image=outputs[1])

    def kernel_module(
        self,
        *,
        name,
        module_name,
        source,
        kernel,
        toolchain,
        inputs=(),
        initramfs=None,
    ):
        source_directory = self.source(source)
        output_directory = self.output(f"modules/{module_name}")
        output = self.environment.File(str(output_directory / f"{module_name}.ko"))
        environment = self.environment.Clone(
            KBUILD_SOURCE=str(kernel.source_directory),
            KBUILD_OUTPUT=str(kernel.build_directory),
            KBUILD_VARIABLES=["ARCH=arm", f"CROSS_COMPILE={toolchain.cross_compile}"],
            MODULE_SOURCE=str(source_directory),
            MODULE_OUTPUT=str(output_directory),
            MODULE_NAME=module_name,
            BUILD_JOBS=self.jobs,
        )
        built = environment.Command(
            output,
            [kernel.module_symbols, *toolchain.outputs, *self._nodes(inputs)],
            Action(
                run_kernel_module,
                describe_module,
                varlist=[
                    "KBUILD_SOURCE",
                    "KBUILD_OUTPUT",
                    "KBUILD_VARIABLES",
                    "MODULE_SOURCE",
                    "MODULE_OUTPUT",
                    "MODULE_NAME",
                    "BUILD_JOBS",
                ],
            ),
        )
        AlwaysBuild(built)
        environment.SideEffect(self.kbuild_lock, built)
        initramfs_files = ()
        if initramfs is not None:
            initramfs_files = (InitramfsFile(built[0], initramfs, 0o644),)
        compilation_generator = self.environment.File(
            str(kernel.source_directory / "scripts/clang-tools/gen_compile_commands.py")
        )
        return self._register(
            name,
            built,
            initramfs_files=initramfs_files,
            kbuild_compilation_databases=(
                KBuildCompilationDatabase(
                    compilation_generator,
                    source_directory,
                    (source_directory, output_directory),
                ),
            ),
        )
