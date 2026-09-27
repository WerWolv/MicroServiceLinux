from SCons.Script import ARGUMENTS, SConscript

from buildsystem import Project


project = Project(build_directory=ARGUMENTS.get("BUILD_DIR", ".build"))

toolchain = SConscript(
    "toolchain/SConscript",
    exports={"project": project}
)

lib = SConscript(
    "lib/SConscript",
    exports={"project": project}
)

os = SConscript(
    "os/SConscript",
    exports={"project": project, "toolchain": toolchain},
)
services = SConscript(
    "services/SConscript",
    exports={"project": project, "toolchain": toolchain, "lib": lib},
)
applications = SConscript(
    "applications/SConscript",
    exports={"project": project, "toolchain": toolchain, "services": services},
)

image = SConscript(
    "image/SConscript",
    exports={"project": project, "os": os, "services": services, "applications": applications},
)
project.compilation_database()

project.default(image)
