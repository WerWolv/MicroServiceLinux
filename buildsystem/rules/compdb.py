import json
import os
from pathlib import Path

from SCons.Script import Action

from ..process import process_environment, run


def generate_kbuild_database(target, source, env):
    output = Path(target[0].abspath)
    output.parent.mkdir(parents=True, exist_ok=True)
    return run(
        [
            "python3",
            env["KBUILD_COMPDB_GENERATOR"],
            "--directory",
            env["KBUILD_COMPDB_DIRECTORY"],
            "--output",
            str(output),
            *env["KBUILD_COMPDB_PATHS"],
        ],
        env=process_environment(env),
    )


def merge_compilation_databases(target, source, env):
    entries = []
    seen = set()
    for database in env["COMPILATION_DATABASES"]:
        path = Path(database)
        if not path.is_file():
            raise RuntimeError(f"compilation database fragment does not exist: {path}")
        for entry in json.loads(path.read_text()):
            directory = entry.get("directory", str(path.parent))
            file = entry["file"]
            if not os.path.isabs(file):
                file = os.path.abspath(os.path.join(directory, file))
            entry["directory"] = os.path.abspath(directory)
            entry["file"] = file
            command = entry.get("command") or json.dumps(entry.get("arguments", ()))
            key = (file, command)
            if key not in seen:
                seen.add(key)
                entries.append(entry)

    entries.sort(key=lambda entry: (entry["file"], entry.get("output", "")))
    output = Path(target[0].abspath)
    output.write_text(json.dumps(entries, indent=2, sort_keys=True) + "\n")
    return 0


def describe_kbuild_database(target, source, env):
    return f"COMPDB  {env['KBUILD_COMPDB_NAME']}"


def describe_merged_database(target, source, env):
    return "COMPDB  compile_commands.json"


class CompilationDatabaseRules:
    def compilation_database(self, *, name="compdb", output="compile_commands.json"):
        work = self.output("compdb")
        scons_database = self.environment.CompilationDatabase(
            str(work / "scons.json")
        )
        databases = [scons_database[0].abspath]
        dependencies = [*scons_database]

        for target in tuple(self.targets.values()):
            for index, provider in enumerate(target.kbuild_compilation_databases):
                fragment = self.environment.File(
                    str(work / f"{target.name}-{index}.json")
                )
                fragment_environment = self.environment.Clone(
                    KBUILD_COMPDB_NAME=target.name,
                    KBUILD_COMPDB_GENERATOR=provider.generator.abspath,
                    KBUILD_COMPDB_DIRECTORY=str(provider.directory),
                    KBUILD_COMPDB_PATHS=[str(path) for path in provider.paths],
                )
                generated = fragment_environment.Command(
                    fragment,
                    [provider.generator, *target.nodes],
                    Action(
                        generate_kbuild_database,
                        describe_kbuild_database,
                        varlist=[
                            "KBUILD_COMPDB_GENERATOR",
                            "KBUILD_COMPDB_DIRECTORY",
                            "KBUILD_COMPDB_PATHS",
                        ],
                    ),
                )
                fragment_environment.AlwaysBuild(generated)
                databases.append(generated[0].abspath)
                dependencies.extend(generated)

        output_node = self.environment.File(str(self.root / output))
        merge_environment = self.environment.Clone(
            COMPILATION_DATABASES=tuple(databases),
        )
        merged = merge_environment.Command(
            output_node,
            dependencies,
            Action(
                merge_compilation_databases,
                describe_merged_database,
                varlist=["COMPILATION_DATABASES"],
            ),
        )
        merge_environment.AlwaysBuild(merged)
        self.environment.Alias(name, merged)
        return tuple(merged)
