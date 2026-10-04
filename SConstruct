from pathlib import Path

from SCons.Errors import UserError
from SCons.Script import ARGUMENTS, SConscript

from buildsystem import Project


DEFAULT_BOARD = "stm32mp157c-dk2"
SUPPORTED_BOARDS = (
    "raspberrypi-4b",
    "stm32mp157c-dk2",
)

board_name = ARGUMENTS.get("BOARD", DEFAULT_BOARD)
if board_name not in SUPPORTED_BOARDS:
    raise UserError(
        f"unsupported BOARD={board_name!r}; expected one of: {', '.join(SUPPORTED_BOARDS)}"
    )

board = SConscript(f"boards/{board_name}/SConscript")
build_root = Path(ARGUMENTS.get("BUILD_DIR", ".build"))
project = Project(build_directory=build_root / board.name)

toolchain = SConscript(
    "toolchain/SConscript",
    exports={"project": project, "board": board}
)

lib = SConscript(
    "lib/SConscript",
    exports={"project": project}
)

os = SConscript(
    "os/SConscript",
    exports={"project": project, "toolchain": toolchain, "board": board},
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
    exports={
        "project": project,
        "board": board,
        "toolchain": toolchain,
        "os": os,
        "services": services,
        "applications": applications,
    },
)
project.compilation_database(output="compile_commands.json")

project.default(image)
