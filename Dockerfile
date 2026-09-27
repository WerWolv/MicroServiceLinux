FROM gcc:16

RUN apt update && apt install -y cmake gdb ninja-build

ENTRYPOINT [ "" ]