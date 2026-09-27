# Horizon-style Linux IPC

This project provides a small Linux kernel module with a synchronous,
service-oriented IPC interface inspired by Horizon's named ports and
`svcSendSyncRequest`/`svcReplyAndReceive` flow. It is an educational transport,
not an ABI-compatible implementation of Nintendo Horizon IPC.

## Interface

Opening `/dev/horizon-ipc` creates a control endpoint. The five ioctls are:

- `HIPC_IOC_REGISTER_SERVICE`: register one globally unique service name on the
  current open file. Closing that file unregisters the service and wakes clients.
- `HIPC_IOC_CREATE_SERVICE`: create an unnamed service and return separate server
  and client file descriptors. Unnamed services are never visible to name lookup.
- `HIPC_IOC_GET_SERVICE`: resolve a service name to a service file descriptor.
  The call waits interruptibly until that service is registered.
- `HIPC_IOC_SEND_SYNC_REQUEST`: send a request through a service file descriptor
  and block until its reply arrives. The request does not contain a service name.
- `HIPC_IOC_RECEIVE_AND_REPLY`: optionally reply to a previously received token,
  then block until the next request arrives.

`REGISTER_SERVICE`, `CREATE_SERVICE`, `GET_SERVICE`, and named-service
`RECEIVE_AND_REPLY` operate on the control endpoint. `SEND_SYNC_REQUEST` operates
on a client descriptor returned by `GET_SERVICE` or `CREATE_SERVICE`. For an
unnamed service, `RECEIVE_AND_REPLY` operates on the returned server descriptor.
Closing a client descriptor releases its reference without shutting down the
service.

## Unnamed services

Unnamed services let a named service return typed child interfaces without
publishing every object globally. Define the child exactly like another service,
but omit `Identifier`:

```cpp
struct IFile : ipc::Service<IFile> {
    [[=ipc::MethodId(1)]]
    virtual std::string read() = 0;
};

struct FileSystem : ipc::Service<FileSystem> {
    constexpr static auto Identifier = "filesystem";

    [[=ipc::MethodId(1)]]
    virtual IFile::Client openFile(const std::string &path) = 0;
};
```

An `IFile::Server` implementation calls `createUnnamed()` to start receiving on
the private server endpoint and obtain the typed client endpoint to return:

```cpp
IFile::Client FileSystemServer::openFile(const std::string &path) {
    auto file = std::make_unique<IFile::Server>(path);
    auto client = file->createUnnamed();
    files.push_back(std::move(file)); // Keep the implementation alive.
    return client;
}
```

`IFile::Client` is serialized as a descriptor attachment rather than payload
bytes. The receiving process owns that descriptor and can invoke `read()` using
the same generated proxy as a named service. See the C++ example for the full
implementation.

Each message can carry up to 4096 payload bytes, eight ordinary file descriptors,
and four shared-memory regions. Descriptor transfer duplicates the sender's open
file reference into the receiver with `FD_CLOEXEC`; it does not consume the
sender's descriptor.

## C++ serialization

The C++ frontend generates synchronous request and response handling directly
from each annotated virtual function. Arithmetic values and other trivially
copyable types use their object representation by default.

Non-trivial or variable-sized types can specialize `ipc::Serializer<T>`:

```cpp
template<>
struct ipc::Serializer<Message> {
    static void serialize(ipc::SerializerWriter &writer, const Message &message) {
        writer.write(static_cast<std::uint32_t>(message.text.size()));
        writer.writeBytes(std::as_bytes(std::span(message.text)));
    }

    static Message deserialize(ipc::SerializerReader &reader) {
        const auto size = reader.read<std::uint32_t>();
        const auto bytes = reader.readBytes(size);
        return { std::string(reinterpret_cast<const char *>(bytes.data()), bytes.size()) };
    }
};
```

Custom serializers can compose other serializers through `write` and `read`.
All reads and writes are bounded by the transport's 4096-byte payload limit, and
the framework rejects truncated or trailing data. See
`examples/cpp/service_test.hpp` for a complete custom type used as both an
argument and return value.

File descriptors and shared memory use the built-in `ipc::FileDescriptor` and
`ipc::MemoryRegion` types. Their serializers place descriptors in the ioctl's
attachment arrays instead of the byte payload:

```cpp
[[=ipc::MethodId(4)]]
virtual ipc::FileDescriptor echoFile(ipc::FileDescriptor) = 0;

[[=ipc::MethodId(5)]]
virtual ipc::MemoryRegion echoRegion(ipc::MemoryRegion) = 0;
```

Both types are move-only and own their descriptor. Sending one does not consume
it unless the interface takes it by value; the transport duplicates outgoing
descriptors and the receiving wrapper owns the descriptor installed by the
kernel. `MemoryRegion` additionally carries its mapping protection, offset, and
size. Unconsumed or malformed attachments are closed automatically.

A memory region carries an FD, page-aligned offset, length, and `PROT_*` flags.
Use a `memfd`, shared file, or other mmap-capable FD and map the received FD with
`MAP_SHARED`. Arbitrary anonymous virtual addresses are deliberately not mapped
across processes: FD-backed mappings let Linux enforce access and lifetime rules.

## Build and run

The build requires Python 3 and SCons. The image build also requires the
crosstool-NG host dependencies, `fakeroot`, `e2fsprogs`, and `gptfdisk`.

Install the pinned SCons version globally or in a virtual environment:

```sh
python3 -m venv .build/venv
.build/venv/bin/pip install -r requirements.txt
```

Build the complete image with either the virtual-environment executable or a
system installation:

```sh
.build/venv/bin/scons -j"$(nproc)" images
# Or: scons -j"$(nproc)" images
```

The first build compiles the Cortex-A7 hard-float GCC 16 toolchain and can take a
while. The final `.build/output/images/` directory contains the SPL, U-Boot, Linux
kernel, device tree, kernel-generated initramfs, ext4 boot filesystem, and the
flashable `sdcard.img`.

`SConstruct` wires together the top-level domain definitions. Each domain
`SConscript` owns its nested targets, and source paths are relative to the file
that declares them. Reusable C/C++ usage requirements, crosstool-NG, KBuild, and
generic action abstractions live under `buildsystem/`. Generated tools are
ordinary dependencies, so SCons builds the cross compiler before any target that
consumes it. Set `BUILD_DIR` to change the build root and `BUILD_JOBS` to control
parallel nested KBuild jobs:

```sh
scons BUILD_DIR=/tmp/ipc-build BUILD_JOBS=8 images
```

Generate a merged compilation database for SCons, Linux, U-Boot, and external
kernel-module sources with:

```sh
scons compdb
```

The KBuild targets are built first so their recorded compiler commands are
available, then all fragments are merged into root-level `compile_commands.json`.

Executables and kernel modules can install themselves into the generated
initramfs by declaring their destination:

```python
service = project.executable(
    # ...
    initramfs="/usr/bin/service",
)

module = project.kernel_module(
    # ...
    initramfs="/lib/modules/module.ko",
)
```

The initramfs rule collects these declarations automatically and creates missing
parent directories. Executables are installed with mode `0755`; kernel modules
use `0644`.

## Behavioral notes

- Service names are process-independent and limited to 31 bytes plus NUL.
- Service lookup waits until registration and can be interrupted by a signal.
- A service descriptor remains bound to the service instance it resolved. If
  that server exits, subsequent sends fail with `ESHUTDOWN`; resolve the name
  again to connect to a replacement instance.
- Payload and attachment limits are compile-time ABI constants.
- Requests and waits are interruptible. If a client is interrupted after the
  server receives its request, a later reply is accepted and discarded.
- Reply payload fields are ignored when `reply_token` is zero. Once a reply is
  committed, the ioctl clears `reply_token` in userspace before waiting again.
- If a receive buffer is too small, `EMSGSIZE` is returned with required counts
  populated and that pending request remains queued.
- If a response buffer is too small, required counts are populated, but the
  synchronous transaction is complete and cannot be retried.
- The module only transports messages. Authentication and per-service policy are
  left to device permissions and the service protocol.
