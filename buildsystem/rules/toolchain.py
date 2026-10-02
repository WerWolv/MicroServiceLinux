from pathlib import Path

from SCons.Script import Action

from ..model import Toolchain
from ..process import process_environment, run


def build_crosstool_ng_host(target, source, env):
    source_directory = Path(env["CTNG_SOURCE"])
    build_directory = Path(env["CTNG_HOST_BUILD"])
    prefix = Path(env["CTNG_HOST_PREFIX"])
    result = run(["./bootstrap"], cwd=source_directory)
    if result:
        return result
    configure = source_directory / "configure"

    build_directory.mkdir(parents=True, exist_ok=True)
    result = run(
        [str(configure), f"--prefix={prefix}"],
        cwd=build_directory,
        env=process_environment(env),
    )
    if result:
        return result
    result = run(
        ["make", f"-j{env['BUILD_JOBS']}"],
        cwd=build_directory,
        env=process_environment(env),
    )
    if result:
        return result
    return run(
        ["make", "install"],
        cwd=build_directory,
        env=process_environment(env),
    )


def build_cross_toolchain(target, source, env):
    build_directory = Path(env["CT_BUILD"])
    prefix = Path(env["CT_PREFIX"])
    downloads = Path(env["CT_DOWNLOADS"])
    configuration = Path(env["CT_CONFIG"])
    ct_ng = Path(env["CT_NG"])

    build_directory.mkdir(parents=True, exist_ok=True)
    prefix.mkdir(parents=True, exist_ok=True)
    downloads.mkdir(parents=True, exist_ok=True)

    contents = configuration.read_text()
    contents = contents.replace("@PREFIX@", str(prefix))
    contents = contents.replace("@DOWNLOADS@", str(downloads))
    (build_directory / ".config").write_text(contents)

    action_env = process_environment(env)
    result = run([str(ct_ng), "olddefconfig"], cwd=build_directory, env=action_env)
    if result:
        return result
    result = run([str(ct_ng), "build"], cwd=build_directory, env=action_env)
    if result:
        return result
    for tool in env["CT_TOOLS"]:
        if not Path(tool).is_file():
            raise RuntimeError(f"crosstool-NG did not produce {tool}")
    Path(target[0].abspath).touch()
    return 0


def describe_toolchain_host(target, source, env):
    return "BUILD   crosstool-NG"


def describe_cross_toolchain(target, source, env):
    return f"BUILD   {env['CT_DESCRIPTION']}"


class ToolchainRules:
    def crosstool_ng(self, *, name, source, config, triplet, description, build_name):
        source_directory = self.source(source)
        configuration = self.source(config)
        shared_output = self.build_directory.parent / build_name
        work = shared_output / "crosstool-ng"
        host_build = work / "host-build"
        host_prefix = work / "host"
        ct_ng = self.environment.File(str(host_prefix / "bin/ct-ng"))

        host_environment = self.environment.Clone(
            CTNG_SOURCE=str(source_directory),
            CTNG_HOST_BUILD=str(host_build),
            CTNG_HOST_PREFIX=str(host_prefix),
            BUILD_JOBS=self.jobs,
        )
        host = host_environment.Command(
            ct_ng,
            self._nodes([
                Path(source) / "bootstrap",
                Path(source) / "configure.ac",
                Path(source) / "Makefile.am",
            ]),
            Action(
                build_crosstool_ng_host,
                describe_toolchain_host,
                varlist=["CTNG_SOURCE", "CTNG_HOST_BUILD", "CTNG_HOST_PREFIX", "BUILD_JOBS"],
            ),
        )
        host_environment.NoClean(host)

        prefix = shared_output / "toolchain"
        binary_prefix = prefix / "bin" / f"{triplet}-"
        compiler_outputs = tuple(
            self.environment.File(str(binary_prefix) + suffix)
            for suffix in ("gcc", "g++", "ar", "ranlib", "strip")
        )
        toolchain_stamp = self.environment.File(str(work / "toolchain.stamp"))
        cross_environment = self.environment.Clone(
            CT_BUILD=str(work / "toolchain-build"),
            CT_PREFIX=str(prefix),
            CT_DOWNLOADS=str(shared_output / "downloads"),
            CT_CONFIG=str(configuration),
            CT_NG=ct_ng.abspath,
            CT_TOOLS=[tool.abspath for tool in compiler_outputs],
            CT_DESCRIPTION=description,
        )
        built = cross_environment.Command(
            toolchain_stamp,
            (self.environment.File(str(configuration)), *host),
            Action(
                build_cross_toolchain,
                describe_cross_toolchain,
                varlist=[
                    "CT_BUILD",
                    "CT_PREFIX",
                    "CT_DOWNLOADS",
                    "CT_CONFIG",
                    "CT_NG",
                    "CT_TOOLS",
                    "CT_DESCRIPTION",
                ],
            ),
        )
        cross_environment.NoClean(built)
        target = self._register(name, built)
        return Toolchain(
            target=target,
            c_compiler=compiler_outputs[0],
            cxx_compiler=compiler_outputs[1],
            archiver=compiler_outputs[2],
            ranlib=compiler_outputs[3],
            stripper=compiler_outputs[4],
            cross_compile=str(binary_prefix),
            sysroot=prefix / triplet / "sysroot",
        )
