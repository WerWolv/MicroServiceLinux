from dataclasses import replace

from ..model import CcInfo, InitramfsFile, merge_cc_info, unique


class CcRules:
    def interface_library(
        self,
        *,
        name,
        headers=(),
        include_directories=(),
        compile_options=(),
        link_options=(),
        dependencies=(),
    ):
        inherited = merge_cc_info(tuple(dependencies))
        header_nodes = self._nodes(headers)
        cc_info = CcInfo(
            include_directories=unique((
                *inherited.include_directories,
                *(self.source(path) for path in include_directories),
            )),
            compile_options=unique((*inherited.compile_options, *compile_options)),
            link_options=unique((*inherited.link_options, *link_options)),
            libraries=inherited.libraries,
            build_inputs=unique((*inherited.build_inputs, *header_nodes)),
        )
        return self._register(name, header_nodes, cc_info)

    def static_library(
        self,
        *,
        name,
        sources,
        toolchain,
        headers=(),
        include_directories=(),
        compile_options=(),
        link_options=(),
        dependencies=(),
    ):
        inherited = merge_cc_info(tuple(dependencies))
        own_includes = tuple(self.source(path) for path in include_directories)
        header_nodes = self._nodes(headers)
        environment = self._cc_environment(
            toolchain,
            "c++",
            unique((*inherited.include_directories, *own_includes)),
            unique((*inherited.compile_options, *compile_options)),
        )
        objects = self._objects(environment, name, sources, toolchain, inherited.build_inputs)
        archive = environment.StaticLibrary(
            target=str(self.output(f"lib/lib{name}.a")),
            source=objects,
        )
        environment.Depends(archive, toolchain.outputs)
        target = self._register(name, archive)
        cc_info = CcInfo(
            include_directories=unique((*inherited.include_directories, *own_includes)),
            compile_options=unique((*inherited.compile_options, *compile_options)),
            link_options=unique((*inherited.link_options, *link_options)),
            libraries=unique((*archive, *inherited.libraries)),
            build_inputs=unique((*inherited.build_inputs, *header_nodes)),
        )
        result = replace(target, cc_info=cc_info)
        self.targets[name] = result
        return result

    def executable(
        self,
        *,
        name,
        sources,
        toolchain,
        language="c++",
        output=None,
        headers=(),
        include_directories=(),
        compile_options=(),
        link_options=(),
        dependencies=(),
        initramfs=None,
    ):
        inherited = merge_cc_info(tuple(dependencies))
        includes = unique((
            *inherited.include_directories,
            *(self.source(path) for path in include_directories),
        ))
        environment = self._cc_environment(
            toolchain,
            language,
            includes,
            unique((*inherited.compile_options, *compile_options)),
        )
        inputs = unique((*inherited.build_inputs, *self._nodes(headers)))
        objects = self._objects(environment, name, sources, toolchain, inputs)
        output = self.output(output or f"bin/{name}")
        program = environment.Program(
            target=str(output),
            source=[*objects, *inherited.libraries],
            LINKFLAGS=[*inherited.link_options, *link_options],
        )
        environment.Depends(program, [*toolchain.outputs, *inputs])
        initramfs_files = ()
        if initramfs is not None:
            initramfs_files = (InitramfsFile(program[0], initramfs, 0o755),)
        return self._register(name, program, initramfs_files=initramfs_files)

    def _cc_environment(self, toolchain, language, includes, compile_options):
        linker = toolchain.c_compiler if language == "c" else toolchain.cxx_compiler
        environment = self.environment.Clone(
            CC=toolchain.c_compiler.abspath,
            CXX=toolchain.cxx_compiler.abspath,
            AR=toolchain.archiver.abspath,
            RANLIB=toolchain.ranlib.abspath,
            LINK=linker.abspath,
        )
        environment.AppendUnique(
            CPPPATH=[str(path) for path in includes],
            CCFLAGS=list(compile_options),
        )
        return environment

    def _objects(self, environment, name, sources, toolchain, inputs):
        objects = []
        for source in sources:
            source_path = self.source(source)
            relative = source_path.relative_to(self.root)
            object_path = self.output("obj") / name / relative.with_suffix(".o")
            node = environment.Object(target=str(object_path), source=str(source_path))
            environment.Depends(node, [*toolchain.outputs, *inputs])
            objects.extend(node)
        return objects
