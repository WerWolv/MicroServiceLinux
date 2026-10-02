import os
import shutil
import subprocess
from datetime import datetime, timezone
from pathlib import Path

from SCons.Script import Action, AlwaysBuild

from ..model import Bootloader, InitramfsFile, KBuildCompilationDatabase, Kernel
from ..process import process_environment, run


def reproducible_environment(environment, source_directory):
    result = dict(environment.get("ENV", {}))
    epoch = result.get("SOURCE_DATE_EPOCH") or os.environ.get("SOURCE_DATE_EPOCH")
    if epoch is None:
        completed = subprocess.run(
            ["git", "-C", str(source_directory), "log", "-1", "--format=%ct"],
            check=False,
            capture_output=True,
            text=True,
        )
        if completed.returncode != 0 or not completed.stdout.strip():
            raise RuntimeError(
                f"could not determine source timestamp for {source_directory}"
            )
        epoch = completed.stdout.strip()

    try:
        timestamp = datetime.fromtimestamp(int(epoch), timezone.utc)
    except (OverflowError, ValueError) as error:
        raise ValueError(f"invalid SOURCE_DATE_EPOCH: {epoch!r}") from error

    result["SOURCE_DATE_EPOCH"] = str(epoch)
    result.setdefault("KBUILD_BUILD_VERSION", "1")
    result.setdefault(
        "KBUILD_BUILD_TIMESTAMP",
        timestamp.strftime("%Y-%m-%d %H:%M:%S +0000"),
    )
    return result


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
    if env["KCONFIG_BASE"]:
        result = run(
            make_command(env, [env["KCONFIG_BASE"]]),
            env=process_environment(env),
        )
        if result:
            return result
        result = run(
            [
                str(Path(env["KBUILD_SOURCE"]) / "scripts/kconfig/merge_config.sh"),
                "-m",
                "-O",
                str(build_directory),
                str(build_directory / ".config"),
                env["KCONFIG_FILE"],
            ],
            env=process_environment(env),
        )
        if result:
            return result
    else:
        shutil.copy2(env["KCONFIG_FILE"], build_directory / ".config")
    return run(make_command(env, ["olddefconfig"]), env=process_environment(env))


def configure_uboot(target, source, env):
    build_directory = Path(env["KBUILD_OUTPUT"])
    build_directory.mkdir(parents=True, exist_ok=True)
    result = run(
        make_command(env, [env["KCONFIG_BASE"]]),
        env=process_environment(env),
    )
    if result:
        return result
    result = run(
        [
            str(Path(env["KBUILD_SOURCE"]) / "scripts/kconfig/merge_config.sh"),
            "-m",
            "-O",
            str(build_directory),
            str(build_directory / ".config"),
            env["KCONFIG_FILE"],
        ],
        env=process_environment(env),
    )
    if result:
        return result
    return run(make_command(env, ["olddefconfig"]), env=process_environment(env))


def run_kbuild(target, source, env):
    Path(env["KBUILD_OUTPUT"]).mkdir(parents=True, exist_ok=True)
    return run(
        make_command(env, env["KBUILD_TARGETS"], parallel=True),
        env=process_environment(env),
    )


def build_device_tree(target, source, env):
    output = Path(target[0].abspath)
    preprocessed = output.with_suffix(".dts.tmp")
    output.parent.mkdir(parents=True, exist_ok=True)

    try:
        result = run(
            [
                env["HOST_CC"],
                "-E",
                "-nostdinc",
                "-I",
                env["DTC_INCLUDE"],
                "-undef",
                "-D__DTS__",
                "-x",
                "assembler-with-cpp",
                "-o",
                str(preprocessed),
                env["DEVICE_TREE_SOURCE"],
            ],
            env=process_environment(env),
        )
        if result != 0:
            return result

        return run(
            [
                env["DTC"],
                "-o",
                str(output),
                "-b",
                "0",
                "-i",
                str(Path(env["DEVICE_TREE_SOURCE"]).parent),
                "-i",
                env["DTC_INCLUDE"],
                "-Wno-unique_unit_address",
                "-Wno-unit_address_vs_reg",
                "-Wno-avoid_unnecessary_addr_size",
                "-Wno-alias_paths",
                "-Wno-interrupt_map",
                "-Wno-simple_bus_reg",
                str(preprocessed),
            ],
            env=process_environment(env),
        )
    finally:
        preprocessed.unlink(missing_ok=True)


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
    return f"KCONFIG {env['KBUILD_DESCRIPTION']}"


def describe_uboot_configuration(target, source, env):
    return f"KCONFIG {env['KBUILD_DESCRIPTION']}"


def describe_kbuild(target, source, env):
    return f"KBUILD  {env['KBUILD_DESCRIPTION']}"


def describe_device_tree(target, source, env):
    return f"DTC     {Path(env['DEVICE_TREE_SOURCE']).name}"


def describe_module(target, source, env):
    return f"KBUILD  {env['MODULE_NAME']}.ko"


class KBuildRules:
    def linux_kernel(
        self,
        *,
        name,
        source,
        config,
        base_configuration,
        architecture,
        image_target,
        image_path,
        description,
        device_tree,
        toolchain,
    ):
        source_directory = self.source(source)
        build_directory = self.output("linux")
        config_node = self.environment.File(str(build_directory / ".config"))
        variables = [f"ARCH={architecture}", f"CROSS_COMPILE={toolchain.cross_compile}"]
        environment_variables = reproducible_environment(
            self.environment, source_directory
        )
        configure_environment = self.environment.Clone(
            ENV=environment_variables,
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KCONFIG_FILE=str(self.source(config)),
            KCONFIG_BASE=base_configuration or "",
            KBUILD_DESCRIPTION=description,
        )
        configuration_sources = [
            self.environment.File(str(self.source(config))),
            self.environment.File(str(source_directory / "Kconfig")),
            *toolchain.outputs,
        ]
        if base_configuration:
            configuration_sources.append(
                self.environment.File(
                    str(
                        source_directory
                        / "arch"
                        / architecture
                        / "configs"
                        / base_configuration
                    )
                )
            )
        configured = configure_environment.Command(
            config_node,
            configuration_sources,
            Action(
                configure_linux,
                describe_linux_configuration,
                varlist=[
                    "KBUILD_SOURCE",
                    "KBUILD_OUTPUT",
                    "KBUILD_VARIABLES",
                    "KCONFIG_FILE",
                    "KCONFIG_BASE",
                    "KBUILD_DESCRIPTION",
                ],
            ),
        )

        outputs = (
            self.environment.File(str(build_directory / image_path)),
            self.environment.File(str(build_directory / "Module.symvers")),
            self.environment.File(str(build_directory / "usr/gen_init_cpio")),
            self.environment.File(str(build_directory / "scripts/dtc/dtc")),
        )
        build_environment = self.environment.Clone(
            ENV=environment_variables,
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KBUILD_TARGETS=[
                image_target,
                "modules",
            ],
            KBUILD_DESCRIPTION=description,
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
        build_environment.Precious(built)
        AlwaysBuild(built)
        build_environment.SideEffect(self.kbuild_lock, built)

        device_tree_source = self.environment.File(str(self.source(device_tree)))
        device_tree_output = self.environment.File(
            str(self.output(f"devicetrees/{Path(device_tree).stem}.dtb"))
        )
        device_tree_environment = self.environment.Clone(
            DEVICE_TREE_SOURCE=device_tree_source.abspath,
            DTC=outputs[3].abspath,
            DTC_INCLUDE=str(source_directory / "scripts/dtc/include-prefixes"),
            HOST_CC="gcc",
        )
        device_tree_built = device_tree_environment.Command(
            device_tree_output,
            [device_tree_source, *built],
            Action(
                build_device_tree,
                describe_device_tree,
                varlist=["DEVICE_TREE_SOURCE", "DTC", "DTC_INCLUDE", "HOST_CC"],
            ),
        )
        AlwaysBuild(device_tree_built)

        compilation_generator = self.environment.File(
            str(source_directory / "scripts/clang-tools/gen_compile_commands.py")
        )
        target = self._register(
            name,
            (*built, *device_tree_built),
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
            architecture=architecture,
            image=outputs[0],
            device_tree=device_tree_built[0],
            module_symbols=outputs[1],
            gen_init_cpio=outputs[2],
        )

    def uboot(
        self,
        *,
        name,
        source,
        config,
        base_configuration,
        architecture,
        device_tree,
        outputs,
        description,
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
        environment_variables = reproducible_environment(
            self.environment, source_directory
        )
        configure_environment = self.environment.Clone(
            ENV=environment_variables,
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KCONFIG_FILE=str(self.source(config)),
            KCONFIG_BASE=base_configuration,
            KBUILD_DESCRIPTION=description,
        )
        configured = configure_environment.Command(
            config_node,
            [
                self.environment.File(str(source_directory / "Kconfig")),
                self.environment.File(
                    str(source_directory / "configs" / base_configuration)
                ),
                self.environment.File(str(self.source(config))),
                *toolchain.outputs,
            ],
            Action(
                configure_uboot,
                describe_uboot_configuration,
                varlist=[
                    "KBUILD_SOURCE",
                    "KBUILD_OUTPUT",
                    "KBUILD_VARIABLES",
                    "KCONFIG_FILE",
                    "KCONFIG_BASE",
                    "KBUILD_DESCRIPTION",
                ],
            ),
        )

        output_nodes = tuple(
            self.environment.File(str(build_directory / output))
            for output in outputs
        )
        build_environment = self.environment.Clone(
            ENV=environment_variables,
            KBUILD_SOURCE=str(source_directory),
            KBUILD_OUTPUT=str(build_directory),
            KBUILD_VARIABLES=variables,
            KBUILD_TARGETS=[],
            KBUILD_DESCRIPTION=description,
            BUILD_JOBS=self.jobs,
        )
        built = build_environment.Command(
            output_nodes,
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
        build_environment.Precious(built)
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
        return Bootloader(target=target, outputs=tuple(built))

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
            ENV=reproducible_environment(
                self.environment, kernel.source_directory
            ),
            KBUILD_SOURCE=str(kernel.source_directory),
            KBUILD_OUTPUT=str(kernel.build_directory),
            KBUILD_VARIABLES=[
                f"ARCH={kernel.architecture}",
                f"CROSS_COMPILE={toolchain.cross_compile}",
            ],
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
        environment.Precious(built)
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
