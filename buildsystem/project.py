from __future__ import annotations

import os
from pathlib import Path

from SCons.Script import Default, Environment, SConsignFile

from .model import Target
from .rules.cc import CcRules
from .rules.compdb import CompilationDatabaseRules
from .rules.image import ImageRules
from .rules.kbuild import KBuildRules
from .rules.toolchain import ToolchainRules


class Project(ToolchainRules, CcRules, KBuildRules, ImageRules, CompilationDatabaseRules):
    def __init__(self, build_directory=".build"):
        self.root = Path.cwd().resolve()
        build_directory = Path(build_directory)
        self.build_directory = (
            build_directory if build_directory.is_absolute() else self.root / build_directory
        ).resolve()
        self.output_directory = self.build_directory / "output"
        self.jobs = int(os.environ.get("BUILD_JOBS", os.cpu_count() or 1))
        self.environment = Environment(ENV=os.environ.copy())
        self.environment.Tool("compilation_db")
        self.environment["COMPILATIONDB_USE_ABSPATH"] = True
        self.kbuild_lock = self.environment.File(str(self.build_directory / ".kbuild.lock"))
        self.targets = {}
        SConsignFile(str(self.build_directory.parent / ".sconsign.dblite"))

    def source(self, path):
        path = Path(path)
        return (path if path.is_absolute() else Path.cwd() / path).resolve()

    def output(self, path):
        return (self.output_directory / path).resolve()

    def _nodes(self, paths, *, output=False):
        resolve = self.output if output else self.source
        return tuple(self.environment.File(str(resolve(path))) for path in paths)

    def _register(
        self,
        name,
        nodes,
        cc_info=None,
        initramfs_files=(),
        kbuild_compilation_databases=(),
    ):
        if name in self.targets:
            raise ValueError(f"duplicate target name: {name}")
        nodes = tuple(nodes)
        self.environment.Alias(name, list(nodes))
        target = Target(
            name=name,
            nodes=nodes,
            cc_info=cc_info,
            initramfs_files=tuple(initramfs_files),
            kbuild_compilation_databases=tuple(kbuild_compilation_databases),
        )
        self.targets[name] = target
        return target

    def default(self, target):
        Default(list(target.nodes))
