from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class CcInfo:
    include_directories: tuple[Path, ...] = ()
    compile_options: tuple[str, ...] = ()
    link_options: tuple[str, ...] = ()
    libraries: tuple[Any, ...] = ()
    build_inputs: tuple[Any, ...] = ()


@dataclass(frozen=True)
class InitramfsFile:
    source: Any
    destination: str
    mode: int
    uid: int = 0
    gid: int = 0


@dataclass(frozen=True)
class KBuildCompilationDatabase:
    generator: Any
    directory: Path
    paths: tuple[Path, ...]


@dataclass(frozen=True)
class Target:
    name: str
    nodes: tuple[Any, ...]
    cc_info: CcInfo | None = None
    initramfs_files: tuple[InitramfsFile, ...] = ()
    kbuild_compilation_databases: tuple[KBuildCompilationDatabase, ...] = ()


@dataclass(frozen=True)
class Toolchain:
    target: Target
    c_compiler: Any
    cxx_compiler: Any
    archiver: Any
    ranlib: Any
    stripper: Any
    cross_compile: str

    @property
    def outputs(self) -> tuple[Any, ...]:
        return self.target.nodes


@dataclass(frozen=True)
class Kernel:
    target: Target
    source_directory: Path
    build_directory: Path
    image: Any
    device_tree: Any
    module_symbols: Any
    gen_init_cpio: Any


@dataclass(frozen=True)
class Bootloader:
    target: Target
    spl: Any
    image: Any


def unique(values):
    result = []
    for value in values:
        if value not in result:
            result.append(value)
    return tuple(result)


def merge_cc_info(dependencies: tuple[Target, ...] | list[Target]) -> CcInfo:
    infos = [dependency.cc_info for dependency in dependencies if dependency.cc_info is not None]
    return CcInfo(
        include_directories=unique(
            path for info in infos for path in info.include_directories
        ),
        compile_options=unique(
            option for info in infos for option in info.compile_options
        ),
        link_options=unique(
            option for info in infos for option in info.link_options
        ),
        libraries=unique(
            library for info in infos for library in info.libraries
        ),
        build_inputs=unique(
            node for info in infos for node in info.build_inputs
        ),
    )
