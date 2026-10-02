import gzip
import os
import shutil
import stat
from pathlib import Path, PurePosixPath

from SCons.Script import Action

from ..process import process_environment, run


def run_command(target, source, env):
    for node in target:
        Path(node.abspath).parent.mkdir(parents=True, exist_ok=True)
    return run(env["ACTION_ARGV"], env=process_environment(env))


def describe_command(target, source, env):
    return env["ACTION_DESCRIPTION"]


def build_initramfs(target, source, env):
    manifest = Path(target[0].abspath)
    output = Path(target[1].abspath)
    temporary = output.with_name(f".{output.name}.tmp")
    manifest.parent.mkdir(parents=True, exist_ok=True)
    output.parent.mkdir(parents=True, exist_ok=True)

    base = Path(env["INITRAMFS_BASE"])
    base_contents = base.read_text()
    if base_contents and not base_contents.endswith("\n"):
        base_contents += "\n"

    paths = {}
    for line in base_contents.splitlines():
        fields = line.split()
        if not fields or fields[0].startswith("#"):
            continue
        if len(fields) >= 2:
            if fields[1] in paths:
                raise RuntimeError(f"initramfs path is already defined: {fields[1]}")
            paths[fields[1]] = fields[0]

    directories = {}
    sysroot_entries = []

    def add_directory(destination, mode):
        entry_type = paths.get(destination)
        if entry_type is not None and entry_type != "dir":
            raise RuntimeError(f"initramfs path is already defined: {destination}")
        if entry_type is None:
            paths[destination] = "dir"
            directories[destination] = mode

    def add_entry(destination, entry_type, line):
        if destination in paths:
            raise RuntimeError(f"initramfs path is already defined: {destination}")
        paths[destination] = entry_type
        sysroot_entries.append((destination, line))

    for sysroot_path in env["INITRAMFS_SYSROOTS"]:
        sysroot = Path(sysroot_path)
        if not sysroot.is_dir():
            raise RuntimeError(f"toolchain sysroot does not exist: {sysroot}")

        for root, directory_names, file_names in os.walk(sysroot, followlinks=False):
            directory_names.sort()
            file_names.sort()
            root_path = Path(root)

            for name in tuple(directory_names):
                path = root_path / name
                relative = path.relative_to(sysroot)
                destination = f"/{relative.as_posix()}"
                metadata = path.lstat()
                if stat.S_ISLNK(metadata.st_mode):
                    directory_names.remove(name)
                    link_target = os.readlink(path)
                    add_entry(
                        destination,
                        "slink",
                        f"slink {destination} {link_target} "
                        f"{stat.S_IMODE(metadata.st_mode):04o} 0 0\n",
                    )
                else:
                    add_directory(destination, stat.S_IMODE(metadata.st_mode))

            for name in file_names:
                path = root_path / name
                relative = path.relative_to(sysroot)
                destination = f"/{relative.as_posix()}"
                metadata = path.lstat()
                mode = stat.S_IMODE(metadata.st_mode)
                if stat.S_ISLNK(metadata.st_mode):
                    link_target = os.readlink(path)
                    add_entry(
                        destination,
                        "slink",
                        f"slink {destination} {link_target} {mode:04o} 0 0\n",
                    )
                elif stat.S_ISREG(metadata.st_mode):
                    add_entry(
                        destination,
                        "file",
                        f"file {destination} {path} {mode:04o} 0 0\n",
                    )
                else:
                    raise RuntimeError(f"unsupported sysroot file type: {path}")

    for destination, _, _, _, _ in env["INITRAMFS_FILES"]:
        parent = PurePosixPath(destination).parent
        while parent != PurePosixPath("/"):
            add_directory(str(parent), 0o755)
            parent = parent.parent
        if destination in paths:
            raise RuntimeError(f"initramfs path is already defined: {destination}")
        paths[destination] = "file"

    lines = [base_contents]
    lines.extend(
        f"dir {path} {mode:04o} 0 0\n"
        for path, mode in sorted(
            directories.items(),
            key=lambda item: (PurePosixPath(item[0]).parts, item[0]),
        )
    )
    lines.extend(line for _, line in sorted(sysroot_entries))
    lines.extend(
        f"file {destination} {source_path} {mode:04o} {uid} {gid}\n"
        for destination, source_path, mode, uid, gid in env["INITRAMFS_FILES"]
    )
    manifest.write_text("".join(lines))

    output.unlink(missing_ok=True)
    try:
        result = run([
            env["INITRAMFS_GENERATOR"],
            "-t",
            "0",
            "-o",
            str(temporary),
            str(manifest),
        ])
        if result != 0:
            return result

        with temporary.open("rb") as source_file, output.open("wb") as output_file:
            with gzip.GzipFile(
                filename="",
                mode="wb",
                compresslevel=9,
                fileobj=output_file,
                mtime=0,
            ) as compressed:
                shutil.copyfileobj(source_file, compressed)
    finally:
        temporary.unlink(missing_ok=True)

    return 0


def describe_initramfs(target, source, env):
    return "INITRAMFS"


class ImageRules:
    def initramfs(self, *, name, generator, base, sysroots=(), targets=None):
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

        base_node = self.environment.File(str(self.source(base)))
        sysroot_paths = tuple(str(toolchain.sysroot) for toolchain in sysroots)
        sysroot_dependencies = tuple(
            node
            for toolchain in sysroots
            for node in toolchain.outputs
        )
        outputs = self._nodes(["initramfs.list", "initramfs.cpio.gz"], output=True)
        environment = self.environment.Clone(
            INITRAMFS_GENERATOR=generator.abspath,
            INITRAMFS_BASE=base_node.abspath,
            INITRAMFS_FILES=entries,
            INITRAMFS_SYSROOTS=sysroot_paths,
        )
        built = environment.Command(
            outputs,
            [
                generator,
                base_node,
                *sysroot_dependencies,
                *(entry.source for entry in files),
            ],
            Action(
                build_initramfs,
                describe_initramfs,
                varlist=[
                    "INITRAMFS_GENERATOR",
                    "INITRAMFS_BASE",
                    "INITRAMFS_FILES",
                    "INITRAMFS_SYSROOTS",
                ],
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
