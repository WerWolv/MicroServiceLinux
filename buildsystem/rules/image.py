from pathlib import Path, PurePosixPath

from SCons.Script import Action

from ..process import process_environment, run


DEFAULT_INITRAMFS_DIRECTORIES = {
    "/bin": 0o755,
    "/dev": 0o755,
    "/lib": 0o755,
    "/lib/modules": 0o755,
    "/proc": 0o555,
    "/root": 0o700,
    "/sys": 0o555,
    "/tmp": 0o1777,
    "/usr": 0o755,
    "/usr/bin": 0o755,
}


def run_command(target, source, env):
    for node in target:
        Path(node.abspath).parent.mkdir(parents=True, exist_ok=True)
    return run(env["ACTION_ARGV"], env=process_environment(env))


def describe_command(target, source, env):
    return env["ACTION_DESCRIPTION"]


def build_initramfs(target, source, env):
    manifest = Path(target[0].abspath)
    output = Path(target[1].abspath)
    manifest.parent.mkdir(parents=True, exist_ok=True)
    output.parent.mkdir(parents=True, exist_ok=True)

    directories = dict(DEFAULT_INITRAMFS_DIRECTORIES)
    for destination, _, _, _, _ in env["INITRAMFS_FILES"]:
        parent = PurePosixPath(destination).parent
        while parent != PurePosixPath("/"):
            directories.setdefault(str(parent), 0o755)
            parent = parent.parent

    lines = [
        f"dir {path} {mode:04o} 0 0\n"
        for path, mode in sorted(
            directories.items(),
            key=lambda item: (PurePosixPath(item[0]).parts, item[0]),
        )
    ]
    lines.append("nod /dev/console 0600 0 0 c 5 1\n")
    lines.extend(
        f"file {destination} {source_path} {mode:04o} {uid} {gid}\n"
        for destination, source_path, mode, uid, gid in env["INITRAMFS_FILES"]
    )
    manifest.write_text("".join(lines))

    return run([
        env["INITRAMFS_GENERATOR"],
        "-t",
        "0",
        "-o",
        str(output),
        str(manifest),
    ])


def describe_initramfs(target, source, env):
    return "INITRAMFS"


class ImageRules:
    def initramfs(self, *, name, generator, targets=None):
        if targets is None:
            targets = tuple(self.targets.values())
        files = tuple(
            entry
            for target in targets
            for entry in target.initramfs_files
        )
        destinations = set()
        for entry in files:
            destination = PurePosixPath(entry.destination)
            if not destination.is_absolute() or ".." in destination.parts:
                raise ValueError(
                    f"initramfs destination must be an absolute normalized path: "
                    f"{entry.destination}"
                )
            if destination == PurePosixPath("/"):
                raise ValueError("cannot install a file at the initramfs root directory")
            if any(character.isspace() for character in str(destination)):
                raise ValueError(f"initramfs destination cannot contain whitespace: {destination}")
            if str(destination) in destinations:
                raise ValueError(f"duplicate initramfs destination: {destination}")
            destinations.add(str(destination))

        entries = tuple(
            (
                str(PurePosixPath(entry.destination)),
                entry.source.abspath,
                entry.mode,
                entry.uid,
                entry.gid,
            )
            for entry in files
        )
        for _, source_path, _, _, _ in entries:
            if any(character.isspace() for character in source_path):
                raise ValueError(f"initramfs source cannot contain whitespace: {source_path}")

        outputs = self._nodes(["initramfs.list", "initramfs.cpio"], output=True)
        environment = self.environment.Clone(
            INITRAMFS_GENERATOR=generator.abspath,
            INITRAMFS_FILES=entries,
        )
        built = environment.Command(
            outputs,
            [generator, *(entry.source for entry in files)],
            Action(
                build_initramfs,
                describe_initramfs,
                varlist=["INITRAMFS_GENERATOR", "INITRAMFS_FILES"],
            ),
        )
        return self._register(name, built)

    def action(self, *, name, outputs, inputs, command):
        output_nodes = self._nodes(outputs, output=True)
        input_nodes = tuple(
            value if hasattr(value, "abspath") else self.environment.File(str(self.source(value)))
            for value in inputs
        )
        argv = tuple(
            value.abspath if hasattr(value, "abspath") else str(value)
            for value in command
        )
        environment = self.environment.Clone(
            ACTION_ARGV=argv,
            ACTION_DESCRIPTION=name,
        )
        built = environment.Command(
            output_nodes,
            input_nodes,
            Action(
                run_command,
                describe_command,
                varlist=["ACTION_ARGV"],
            ),
        )
        return self._register(name, built)
