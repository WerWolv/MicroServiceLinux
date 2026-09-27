savedcmd_source/ipc.o := /home/werwolv/Development/ipc/.build/output/toolchain/bin/arm-ipc-linux-gnueabihf-gcc -Wp,-MMD,source/.ipc.o.d -nostdinc -I/home/werwolv/Development/ipc/os/linux/arch/arm/include -I/home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated -I/home/werwolv/Development/ipc/os/linux/include -I/home/werwolv/Development/ipc/.build/output/linux/include -I/home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi -I/home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi -I/home/werwolv/Development/ipc/os/linux/include/uapi -I/home/werwolv/Development/ipc/.build/output/linux/include/generated/uapi -include /home/werwolv/Development/ipc/os/linux/include/linux/compiler-version.h -include /home/werwolv/Development/ipc/os/linux/include/linux/kconfig.h -include /home/werwolv/Development/ipc/os/linux/include/linux/compiler_types.h -D__KERNEL__ -mlittle-endian -D__LINUX_ARM_ARCH__=7 -fmacro-prefix-map=/home/werwolv/Development/ipc/os/modules/ipc/= -fshort-wchar -funsigned-char -fno-common -fno-PIE -fno-strict-aliasing -std=gnu11 -fms-extensions -fno-dwarf2-cfi-asm -mno-fdpic -fno-ipa-sra -mtp=cp15 -mabi=aapcs-linux -mfpu=vfp -funwind-tables -marm -Wa,-mno-warn-deprecated -march=armv7-a -msoft-float -Uarm -fno-delete-null-pointer-checks -O2 -fno-allow-store-data-races -fstack-protector-strong -fomit-frame-pointer -ftrivial-auto-var-init=zero -fzero-init-padding-bits=all -fno-stack-clash-protection -fdiagnostics-show-context=2 -fstrict-flex-arrays=3 -fno-strict-overflow -fno-stack-check -fconserve-stack -fno-builtin-wcslen -Wall -Wextra -Wundef -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Werror=strict-prototypes -Wno-format-security -Wno-trigraphs -Wno-frame-address -Wno-address-of-packed-member -Wmissing-declarations -Wmissing-prototypes -Wframe-larger-than=1280 -Wno-main -Wno-type-limits -Wno-dangling-pointer -Wvla-larger-than=1 -Wno-pointer-sign -Wcast-function-type -Wno-unterminated-string-initialization -Wno-array-bounds -Wno-stringop-overflow -Wno-alloc-size-larger-than -Wimplicit-fallthrough=5 -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -Wenum-conversion -Wunused -Wno-unused-but-set-variable -Wno-unused-const-variable -Wno-packed-not-aligned -Wno-format-overflow -Wno-format-truncation -Wno-stringop-truncation -Wno-override-init -Wno-missing-field-initializers -Wno-shift-negative-value -Wno-maybe-uninitialized -Wno-sign-compare -Wno-unused-parameter -mstack-protector-guard=tls -mstack-protector-guard-offset=1408 -I/home/werwolv/Development/ipc/os/modules/ipc/./include -I/home/werwolv/Development/ipc/os/modules/ipc/. -I.  -DMODULE  -DKBUILD_BASENAME='"ipc"' -DKBUILD_MODNAME='"ipc"' -D__KBUILD_MODNAME=ipc -c -o source/ipc.o /home/werwolv/Development/ipc/os/modules/ipc/source/ipc.c  

source_source/ipc.o := /home/werwolv/Development/ipc/os/modules/ipc/source/ipc.c

deps_source/ipc.o := \
    $(wildcard include/config/COMPAT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/FUNCTION_ALIGNMENT) \
    $(wildcard include/config/CC_HAS_SANE_FUNCTION_ALIGNMENT) \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/LD_DEAD_CODE_DATA_ELIMINATION) \
    $(wildcard include/config/LTO_CLANG) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/KCSAN) \
    $(wildcard include/config/CC_HAS_ASSUME) \
    $(wildcard include/config/CC_HAS_COUNTED_BY) \
    $(wildcard include/config/FORTIFY_SOURCE) \
    $(wildcard include/config/UBSAN_BOUNDS) \
    $(wildcard include/config/CC_HAS_COUNTED_BY_PTR) \
    $(wildcard include/config/CC_HAS_MULTIDIMENSIONAL_NONSTRING) \
    $(wildcard include/config/CFI) \
    $(wildcard include/config/ARCH_USES_CFI_GENERIC_LLVM_PASS) \
    $(wildcard include/config/CC_HAS_BROKEN_COUNTED_BY_REF) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler-context-analysis.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler_attributes.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
    $(wildcard include/config/SHADOW_CALL_STACK) \
    $(wildcard include/config/KCOV) \
    $(wildcard include/config/CC_HAS_TYPEOF_UNQUAL) \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/percpu_types.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/percpu_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/atomic.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
    $(wildcard include/config/KVFREE_RCU_BATCHED) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/types.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/types.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/int-ll64.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/int-ll64.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/bitsperlong.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitsperlong.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/bitsperlong.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/posix_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/stddef.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/stddef.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/posix_types.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/posix_types.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/atomic.h \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/GENERIC_ATOMIC64) \
    $(wildcard include/config/ARM_LPAE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/OBJTOOL) \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/rwonce.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/rwonce.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kasan-checks.h \
    $(wildcard include/config/KASAN_GENERIC) \
    $(wildcard include/config/KASAN_SW_TAGS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kcsan-checks.h \
    $(wildcard include/config/KCSAN_WEAK_MEMORY) \
    $(wildcard include/config/KCSAN_IGNORE_ATOMICS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/prefetch.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/processor.h \
    $(wildcard include/config/HAVE_HW_BREAKPOINT) \
    $(wildcard include/config/BINFMT_ELF_FDPIC) \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/CPU_ENDIAN_BE8) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/hw_breakpoint.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/ARM_THUMB) \
    $(wildcard include/config/CPU_V7M) \
    $(wildcard include/config/THUMB2_KERNEL) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/ptrace.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/hwcap.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/hwcap.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/bitfield.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/build_bug.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/typecheck.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/byteorder.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/byteorder/little_endian.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/byteorder/little_endian.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/swab.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/swab.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/swab.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/swab.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/byteorder/generic.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/unified.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/vdso/processor.h \
    $(wildcard include/config/ARM_ERRATA_754327) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cache.h \
    $(wildcard include/config/ARM_L1_CACHE_SHIFT) \
    $(wildcard include/config/AEABI) \
    $(wildcard include/config/ARCH_HAS_CACHE_LINE_SIZE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqflags.h \
    $(wildcard include/config/PROVE_LOCKING) \
    $(wildcard include/config/TRACE_IRQFLAGS) \
    $(wildcard include/config/PREEMPT_RT) \
    $(wildcard include/config/IRQSOFF_TRACER) \
    $(wildcard include/config/PREEMPT_TRACER) \
    $(wildcard include/config/DEBUG_IRQFLAGS) \
    $(wildcard include/config/TRACE_IRQFLAGS_SUPPORT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqflags_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cleanup.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/err.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/errno.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/errno.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/errno-base.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/args.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/irqflags.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/irqflags.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/percpu.h \
    $(wildcard include/config/CPU_V6) \
    $(wildcard include/config/ARM_HAS_GROUP_RELOCS) \
    $(wildcard include/config/ARM_MODULE_PLTS) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/insn.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/percpu.h \
    $(wildcard include/config/DEBUG_PREEMPT) \
    $(wildcard include/config/HAVE_SETUP_PER_CPU_AREA) \
  /home/werwolv/Development/ipc/os/linux/include/linux/threads.h \
    $(wildcard include/config/NR_CPUS) \
    $(wildcard include/config/BASE_SMALL) \
  /home/werwolv/Development/ipc/os/linux/include/linux/percpu-defs.h \
    $(wildcard include/config/ARCH_MODULE_NEEDS_WEAK_PER_CPU) \
    $(wildcard include/config/AMD_MEM_ENCRYPT) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/barrier.h \
    $(wildcard include/config/CPU_32v6K) \
    $(wildcard include/config/CPU_XSC3) \
    $(wildcard include/config/CPU_FA526) \
    $(wildcard include/config/ARM_HEAVY_MB) \
    $(wildcard include/config/ARM_DMA_MEM_BUFFERABLE) \
    $(wildcard include/config/CPU_SPECTRE) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/barrier.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cmpxchg.h \
    $(wildcard include/config/CPU_SA1100) \
    $(wildcard include/config/CPU_SA110) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cmpxchg-emu.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/cmpxchg-local.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/atomic/atomic-arch-fallback.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/atomic/atomic-long.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/atomic/atomic-instrumented.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/instrumented.h \
    $(wildcard include/config/DEBUG_ATOMIC) \
    $(wildcard include/config/DEBUG_ATOMIC_LARGEST_ALIGN) \
  /home/werwolv/Development/ipc/os/linux/include/linux/bug.h \
    $(wildcard include/config/GENERIC_BUG) \
    $(wildcard include/config/PRINTK) \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/linkage.h \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/stringify.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/GENDWARFKSYMS) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/linkage.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/CPU_ENDIAN_BE32) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bug.h \
    $(wildcard include/config/DEBUG_BUGVERBOSE_DETAILED) \
    $(wildcard include/config/BUG) \
    $(wildcard include/config/GENERIC_BUG_RELATIVE_POINTERS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/instrumentation.h \
    $(wildcard include/config/NOINSTR_VALIDATION) \
  /home/werwolv/Development/ipc/os/linux/include/linux/once_lite.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/panic.h \
    $(wildcard include/config/PANIC_TIMEOUT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/stdarg.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/printk.h \
    $(wildcard include/config/MESSAGE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_DEFAULT) \
    $(wildcard include/config/CONSOLE_LOGLEVEL_QUIET) \
    $(wildcard include/config/EARLY_PRINTK) \
    $(wildcard include/config/PRINTK_INDEX) \
    $(wildcard include/config/DYNAMIC_DEBUG) \
    $(wildcard include/config/DYNAMIC_DEBUG_CORE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/init.h \
    $(wildcard include/config/MEMORY_HOTPLUG) \
    $(wildcard include/config/HAVE_ARCH_PREL32_RELOCATIONS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kern_levels.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ratelimit_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/bits.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/bits.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/const.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/const.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/bits.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/overflow.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/limits.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/limits.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/limits.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/const.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/param.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/param.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/param.h \
    $(wildcard include/config/HZ) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/param.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/spinlock_types_raw.h \
    $(wildcard include/config/DEBUG_SPINLOCK) \
    $(wildcard include/config/DEBUG_LOCK_ALLOC) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/spinlock_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/lockdep_types.h \
    $(wildcard include/config/PROVE_RAW_LOCK_NESTING) \
    $(wildcard include/config/LOCKDEP) \
    $(wildcard include/config/LOCK_STAT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kmsan-checks.h \
    $(wildcard include/config/KMSAN) \
  /home/werwolv/Development/ipc/os/linux/include/linux/anon_inodes.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/completion.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/swait.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/list.h \
    $(wildcard include/config/LIST_HARDENED) \
    $(wildcard include/config/DEBUG_LIST) \
  /home/werwolv/Development/ipc/os/linux/include/linux/container_of.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/spinlock.h \
    $(wildcard include/config/PREEMPTION) \
  /home/werwolv/Development/ipc/os/linux/include/linux/preempt.h \
    $(wildcard include/config/HAS_SEPARATE_PREEMPT_RESCHED_BITS) \
    $(wildcard include/config/PREEMPT_COUNT) \
    $(wildcard include/config/TRACE_PREEMPT_TOGGLE) \
    $(wildcard include/config/PREEMPT_NOTIFIERS) \
    $(wildcard include/config/PREEMPT_DYNAMIC) \
    $(wildcard include/config/PREEMPT_NONE) \
    $(wildcard include/config/PREEMPT_VOLUNTARY) \
    $(wildcard include/config/PREEMPT) \
    $(wildcard include/config/PREEMPT_LAZY) \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/preempt.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/preempt.h \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_KEY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/thread_info.h \
    $(wildcard include/config/THREAD_INFO_IN_TASK) \
    $(wildcard include/config/GENERIC_ENTRY) \
    $(wildcard include/config/ARCH_HAS_PREEMPT_LAZY) \
    $(wildcard include/config/HAVE_ARCH_WITHIN_STACK_FRAMES) \
    $(wildcard include/config/SH) \
  /home/werwolv/Development/ipc/os/linux/include/linux/restart_block.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/time64.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  /home/werwolv/Development/ipc/os/linux/include/linux/math.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/div64.h \
    $(wildcard include/config/CC_OPTIMIZE_FOR_PERFORMANCE) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/compiler.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/div64.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  /home/werwolv/Development/ipc/os/linux/include/linux/bitops.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/kernel.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/sysinfo.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/generic-non-atomic.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/bitops.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/non-atomic.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/non-instrumented-non-atomic.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/builtin-__fls.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/builtin-__ffs.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/builtin-fls.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/builtin-ffs.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/ffz.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/fls64.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/sched.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/hweight.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/arch_hweight.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/const_hweight.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/lock.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/instrumented-lock.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/le.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/math64.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/time64.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/time.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/time_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/errno.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/errno.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/current.h \
    $(wildcard include/config/CURRENT_POINTER_IN_TPIDRURO) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/KASAN) \
    $(wildcard include/config/VMAP_STACK) \
    $(wildcard include/config/ARM_THUMBEE) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/VFPv3) \
    $(wildcard include/config/IWMMXT) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/page.h \
    $(wildcard include/config/CPU_COPY_V4WT) \
    $(wildcard include/config/CPU_COPY_V4WB) \
    $(wildcard include/config/CPU_COPY_FEROCEON) \
    $(wildcard include/config/CPU_COPY_FA) \
    $(wildcard include/config/CPU_XSCALE) \
    $(wildcard include/config/CPU_COPY_V6) \
    $(wildcard include/config/KUSER_HELPERS) \
    $(wildcard include/config/HAVE_ARCH_PFN_VALID) \
  /home/werwolv/Development/ipc/os/linux/include/vdso/page.h \
    $(wildcard include/config/PAGE_SHIFT) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/glue.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/pgtable-2level-types.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/memory.h \
    $(wildcard include/config/NEED_MACH_MEMORY_H) \
    $(wildcard include/config/PAGE_OFFSET) \
    $(wildcard include/config/HIGHMEM) \
    $(wildcard include/config/DRAM_BASE) \
    $(wildcard include/config/DRAM_SIZE) \
    $(wildcard include/config/XIP_KERNEL) \
    $(wildcard include/config/HAVE_TCM) \
    $(wildcard include/config/ARM_PATCH_PHYS_VIRT) \
    $(wildcard include/config/PHYS_OFFSET) \
    $(wildcard include/config/DEBUG_VIRTUAL) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sizes.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/kasan_def.h \
    $(wildcard include/config/KASAN_SHADOW_OFFSET) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/getorder.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/memory_model.h \
    $(wildcard include/config/FLATMEM) \
    $(wildcard include/config/SPARSEMEM_VMEMMAP) \
    $(wildcard include/config/SPARSEMEM) \
  /home/werwolv/Development/ipc/os/linux/include/linux/pfn.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/traps.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/interrupt_rc.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/processor.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/smp.h \
    $(wildcard include/config/UP_LATE_INIT) \
    $(wildcard include/config/CSD_LOCK_WAIT_DEBUG) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpumask.h \
    $(wildcard include/config/FORCE_NR_CPUS) \
    $(wildcard include/config/HOTPLUG_CPU) \
    $(wildcard include/config/DEBUG_PER_CPU_MAPS) \
    $(wildcard include/config/CPUMASK_OFFSTACK) \
  /home/werwolv/Development/ipc/os/linux/include/linux/bitmap.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/align.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/align.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/find.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/string.h \
    $(wildcard include/config/BINARY_PRINTF) \
  /home/werwolv/Development/ipc/os/linux/include/linux/array_size.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/string.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/string.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/bitmap-str.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpumask_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/gfp_types.h \
    $(wildcard include/config/KASAN_HW_TAGS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/numa.h \
    $(wildcard include/config/NUMA_KEEP_MEMINFO) \
    $(wildcard include/config/NUMA) \
    $(wildcard include/config/HAVE_ARCH_NODE_DEV_GROUP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/nodemask.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/minmax.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/nodemask_types.h \
    $(wildcard include/config/NODES_SHIFT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/random.h \
    $(wildcard include/config/VMGENID) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kernel.h \
    $(wildcard include/config/PREEMPT_VOLUNTARY_BUILD) \
    $(wildcard include/config/HAVE_PREEMPT_DYNAMIC_CALL) \
    $(wildcard include/config/PREEMPT_) \
    $(wildcard include/config/DEBUG_ATOMIC_SLEEP) \
    $(wildcard include/config/DYNAMIC_FTRACE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kstrtox.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sprintf.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/static_call_types.h \
    $(wildcard include/config/HAVE_STATIC_CALL) \
    $(wildcard include/config/HAVE_STATIC_CALL_INLINE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/trace_printk.h \
    $(wildcard include/config/TRACING) \
  /home/werwolv/Development/ipc/os/linux/include/linux/instruction_pointer.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/util_macros.h \
    $(wildcard include/config/FOO_SUSPEND) \
  /home/werwolv/Development/ipc/os/linux/include/linux/wordpart.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/random.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/ioctl.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/ioctl.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/ioctl.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/ioctl.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqnr.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/irqnr.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/smp_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/llist.h \
    $(wildcard include/config/ARCH_HAVE_NMI_SAFE_CMPXCHG) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/smp.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/bottom_half.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/lockdep.h \
    $(wildcard include/config/DEBUG_LOCKING_API_SELFTESTS) \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/mmiowb.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/mmiowb.h \
    $(wildcard include/config/MMIOWB) \
  /home/werwolv/Development/ipc/os/linux/include/linux/spinlock_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rwlock_types.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/spinlock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rwlock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/INLINE_SPIN_LOCK) \
    $(wildcard include/config/INLINE_SPIN_LOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK) \
    $(wildcard include/config/INLINE_SPIN_TRYLOCK_BH) \
    $(wildcard include/config/UNINLINE_SPIN_UNLOCK) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_BH) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_SPIN_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/GENERIC_LOCKBREAK) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/INLINE_READ_LOCK) \
    $(wildcard include/config/INLINE_WRITE_LOCK) \
    $(wildcard include/config/INLINE_READ_LOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_LOCK_BH) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_WRITE_LOCK_IRQSAVE) \
    $(wildcard include/config/INLINE_READ_TRYLOCK) \
    $(wildcard include/config/INLINE_WRITE_TRYLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK) \
    $(wildcard include/config/INLINE_READ_UNLOCK_BH) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_BH) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQ) \
    $(wildcard include/config/INLINE_READ_UNLOCK_IRQRESTORE) \
    $(wildcard include/config/INLINE_WRITE_UNLOCK_IRQRESTORE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/wait.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cred.h \
    $(wildcard include/config/MULTIUSER) \
    $(wildcard include/config/KEYS) \
    $(wildcard include/config/SECURITY) \
    $(wildcard include/config/USER_NS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/capability.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/capability.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/uidgid.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/uidgid_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/highuid.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/key.h \
    $(wildcard include/config/KEY_NOTIFICATIONS) \
    $(wildcard include/config/NET) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rbtree.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rbtree_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcupdate.h \
    $(wildcard include/config/PREEMPT_RCU) \
    $(wildcard include/config/TINY_RCU) \
    $(wildcard include/config/RCU_STRICT_GRACE_PERIOD) \
    $(wildcard include/config/RCU_LAZY) \
    $(wildcard include/config/RCU_STALL_COMMON) \
    $(wildcard include/config/NO_HZ_FULL) \
    $(wildcard include/config/VIRT_XFER_TO_GUEST_WORK) \
    $(wildcard include/config/RCU_NOCB_CPU) \
    $(wildcard include/config/TASKS_RCU_GENERIC) \
    $(wildcard include/config/TASKS_RCU) \
    $(wildcard include/config/TASKS_RUDE_RCU) \
    $(wildcard include/config/TREE_RCU) \
    $(wildcard include/config/DEBUG_OBJECTS_RCU_HEAD) \
    $(wildcard include/config/PROVE_RCU) \
    $(wildcard include/config/ARCH_WEAK_RELEASE_ACQUIRE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_NATIVE) \
    $(wildcard include/config/SCHED_INFO) \
    $(wildcard include/config/SCHEDSTATS) \
    $(wildcard include/config/SCHED_CORE) \
    $(wildcard include/config/FAIR_GROUP_SCHED) \
    $(wildcard include/config/RT_GROUP_SCHED) \
    $(wildcard include/config/RT_MUTEXES) \
    $(wildcard include/config/UCLAMP_TASK) \
    $(wildcard include/config/UCLAMP_BUCKETS_COUNT) \
    $(wildcard include/config/KMAP_LOCAL) \
    $(wildcard include/config/MEM_ALLOC_PROFILING) \
    $(wildcard include/config/SCHED_CLASS_EXT) \
    $(wildcard include/config/CGROUP_SCHED) \
    $(wildcard include/config/CFS_BANDWIDTH) \
    $(wildcard include/config/BLK_DEV_IO_TRACE) \
    $(wildcard include/config/TASKS_TRACE_RCU) \
    $(wildcard include/config/TRIVIAL_PREEMPT_RCU) \
    $(wildcard include/config/MEMCG_V1) \
    $(wildcard include/config/LRU_GEN) \
    $(wildcard include/config/COMPAT_BRK) \
    $(wildcard include/config/CGROUPS) \
    $(wildcard include/config/BLK_CGROUP) \
    $(wildcard include/config/PSI) \
    $(wildcard include/config/PAGE_OWNER) \
    $(wildcard include/config/EVENTFD) \
    $(wildcard include/config/ARCH_HAS_CPU_PASID) \
    $(wildcard include/config/X86_BUS_LOCK_DETECT) \
    $(wildcard include/config/TASK_DELAY_ACCT) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ARCH_HAS_SCALED_CPUTIME) \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING_GEN) \
    $(wildcard include/config/POSIX_CPUTIMERS) \
    $(wildcard include/config/POSIX_CPU_TIMERS_TASK_WORK) \
    $(wildcard include/config/SYSVIPC) \
    $(wildcard include/config/DETECT_HUNG_TASK) \
    $(wildcard include/config/IO_URING) \
    $(wildcard include/config/AUDIT) \
    $(wildcard include/config/AUDITSYSCALL) \
    $(wildcard include/config/DETECT_HUNG_TASK_BLOCKER) \
    $(wildcard include/config/UBSAN) \
    $(wildcard include/config/UBSAN_TRAP) \
    $(wildcard include/config/COMPACTION) \
    $(wildcard include/config/TASK_XACCT) \
    $(wildcard include/config/CPUSETS) \
    $(wildcard include/config/X86_CPU_RESCTRL) \
    $(wildcard include/config/PERF_EVENTS) \
    $(wildcard include/config/NUMA_BALANCING) \
    $(wildcard include/config/SCHED_CACHE) \
    $(wildcard include/config/ARCH_HAS_LAZY_MMU_MODE) \
    $(wildcard include/config/FAULT_INJECTION) \
    $(wildcard include/config/LATENCYTOP) \
    $(wildcard include/config/KUNIT) \
    $(wildcard include/config/FUNCTION_GRAPH_TRACER) \
    $(wildcard include/config/MEMCG) \
    $(wildcard include/config/UPROBES) \
    $(wildcard include/config/BCACHE) \
    $(wildcard include/config/LIVEPATCH) \
    $(wildcard include/config/BPF_SYSCALL) \
    $(wildcard include/config/KSTACK_ERASE) \
    $(wildcard include/config/KSTACK_ERASE_METRICS) \
    $(wildcard include/config/X86_MCE) \
    $(wildcard include/config/KRETPROBES) \
    $(wildcard include/config/RETHOOK) \
    $(wildcard include/config/ARCH_HAS_PARANOID_L1D_FLUSH) \
    $(wildcard include/config/RV) \
    $(wildcard include/config/RV_PER_TASK_MONITORS) \
    $(wildcard include/config/USER_EVENTS) \
    $(wildcard include/config/UNWIND_USER) \
    $(wildcard include/config/SCHED_PROXY_EXEC) \
    $(wildcard include/config/MEM_ALLOC_PROFILING_DEBUG) \
    $(wildcard include/config/SCHED_MM_CID) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/sched.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cache.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/cache.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/futex_types.h \
    $(wildcard include/config/FUTEX) \
    $(wildcard include/config/FUTEX_PRIVATE_HASH) \
    $(wildcard include/config/FUTEX_ROBUST_UNLOCK) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mutex_types.h \
    $(wildcard include/config/MUTEX_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_MUTEXES) \
  /home/werwolv/Development/ipc/os/linux/include/linux/osq_lock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/pid_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sem_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/shm.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/shmparam.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kmsan_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/plist_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/hrtimer_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/timerqueue_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/timer_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/seccomp_types.h \
    $(wildcard include/config/SECCOMP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/refcount_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/resource.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/resource.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/resource.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/resource.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/resource.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/latencytop.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/prio.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/signal_types.h \
    $(wildcard include/config/OLD_SIGACTION) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/signal.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/signal.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/signal.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/signal-defs.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/sigcontext.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/uapi/asm/siginfo.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/siginfo.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/syscall_user_dispatch_types.h \
    $(wildcard include/config/SYSCALL_USER_DISPATCH) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mm_types_task.h \
    $(wildcard include/config/ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH) \
  /home/werwolv/Development/ipc/os/linux/include/linux/netdevice_xmit.h \
    $(wildcard include/config/NET_ACT_MIRRED) \
    $(wildcard include/config/NET_EGRESS) \
    $(wildcard include/config/NF_DUP_NETDEV) \
  /home/werwolv/Development/ipc/os/linux/include/linux/task_io_accounting.h \
    $(wildcard include/config/TASK_IO_ACCOUNTING) \
  /home/werwolv/Development/ipc/os/linux/include/linux/posix-timers_types.h \
    $(wildcard include/config/POSIX_TIMERS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rseq_types.h \
    $(wildcard include/config/RSEQ) \
    $(wildcard include/config/RSEQ_SLICE_EXTENSION) \
  /home/werwolv/Development/ipc/os/linux/include/linux/irq_work_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/workqueue_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/seqlock_types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kcsan.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rv.h \
    $(wildcard include/config/RV_LTL_MONITOR) \
    $(wildcard include/config/RV_HA_MONITOR) \
    $(wildcard include/config/RV_REACTORS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/tracepoint-defs.h \
    $(wildcard include/config/TRACEPOINTS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/static_key.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/unwind_deferred_types.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/kmap_size.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/kmap_size.h \
    $(wildcard include/config/DEBUG_KMAP_LOCAL) \
  /home/werwolv/Development/ipc/.build/output/linux/include/generated/rq-offsets.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/ext.h \
    $(wildcard include/config/EXT_SUB_SCHED) \
    $(wildcard include/config/EXT_GROUP_SCHED) \
  /home/werwolv/Development/ipc/os/linux/include/linux/context_tracking_irq.h \
    $(wildcard include/config/CONTEXT_TRACKING_IDLE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcutree.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sysctl.h \
    $(wildcard include/config/SYSCTL) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/sysctl.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rwsem.h \
    $(wildcard include/config/RWSEM_SPIN_ON_OWNER) \
    $(wildcard include/config/DEBUG_RWSEMS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/assoc_array.h \
    $(wildcard include/config/ASSOCIATIVE_ARRAY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/refcount.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/user.h \
    $(wildcard include/config/EPOLL) \
    $(wildcard include/config/VFIO_PCI_ZDEV_KVM) \
    $(wildcard include/config/IOMMUFD) \
    $(wildcard include/config/SECRETMEM) \
    $(wildcard include/config/WATCH_QUEUE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/percpu_counter.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/percpu.h \
    $(wildcard include/config/MODULES) \
    $(wildcard include/config/KMALLOC_PARTITION_CACHES) \
    $(wildcard include/config/PAGE_SIZE_4KB) \
    $(wildcard include/config/NEED_PER_CPU_PAGE_FIRST_CHUNK) \
  /home/werwolv/Development/ipc/os/linux/include/linux/alloc_tag.h \
    $(wildcard include/config/MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/codetag.h \
    $(wildcard include/config/CODE_TAGGING) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mmdebug.h \
    $(wildcard include/config/DEBUG_VM) \
    $(wildcard include/config/DEBUG_VM_IRQSOFF) \
    $(wildcard include/config/DEBUG_VM_PGFLAGS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/ratelimit.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/fcntl.h \
    $(wildcard include/config/ARCH_32BIT_OFF_T) \
  /home/werwolv/Development/ipc/os/linux/include/linux/stat.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/stat.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/stat.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/time.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/time32.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/timex.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/timex.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/timex.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/time32.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/time.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/fcntl.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/fcntl.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/fcntl.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/openat2.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/file.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/fs.h \
    $(wildcard include/config/FANOTIFY_ACCESS_PERMISSIONS) \
    $(wildcard include/config/FS_POSIX_ACL) \
    $(wildcard include/config/CGROUP_WRITEBACK) \
    $(wildcard include/config/IMA) \
    $(wildcard include/config/FILE_LOCKING) \
    $(wildcard include/config/FSNOTIFY) \
    $(wildcard include/config/FS_DAX) \
    $(wildcard include/config/SWAP) \
    $(wildcard include/config/BLOCK) \
    $(wildcard include/config/UNICODE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/fs/super.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/fs/super_types.h \
    $(wildcard include/config/QUOTA) \
    $(wildcard include/config/FS_ENCRYPTION) \
    $(wildcard include/config/FS_VERITY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/fs_dirent.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/errseq.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/list_lru.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/shrinker.h \
    $(wildcard include/config/SHRINKER_DEBUG) \
  /home/werwolv/Development/ipc/os/linux/include/linux/xarray.h \
    $(wildcard include/config/XARRAY_MULTI) \
  /home/werwolv/Development/ipc/os/linux/include/linux/gfp.h \
    $(wildcard include/config/ZONE_DMA) \
    $(wildcard include/config/ZONE_DMA32) \
    $(wildcard include/config/ZONE_DEVICE) \
    $(wildcard include/config/CONTIG_ALLOC) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mmzone.h \
    $(wildcard include/config/ARCH_FORCE_MAX_ORDER) \
    $(wildcard include/config/PAGE_BLOCK_MAX_ORDER) \
    $(wildcard include/config/HAVE_GIGANTIC_FOLIOS) \
    $(wildcard include/config/HUGETLB_PAGE) \
    $(wildcard include/config/HUGETLB_PAGE_OPTIMIZE_VMEMMAP) \
    $(wildcard include/config/CMA) \
    $(wildcard include/config/MEMORY_ISOLATION) \
    $(wildcard include/config/ZSMALLOC) \
    $(wildcard include/config/UNACCEPTED_MEMORY) \
    $(wildcard include/config/IOMMU_SUPPORT) \
    $(wildcard include/config/TRANSPARENT_HUGEPAGE) \
    $(wildcard include/config/LRU_GEN_STATS) \
    $(wildcard include/config/LRU_GEN_WALKS_MMU) \
    $(wildcard include/config/MEMORY_FAILURE) \
    $(wildcard include/config/PAGE_EXTENSION) \
    $(wildcard include/config/DEFERRED_STRUCT_PAGE_INIT) \
    $(wildcard include/config/HAVE_MEMORYLESS_NODES) \
    $(wildcard include/config/SPARSEMEM_EXTREME) \
    $(wildcard include/config/SPARSEMEM_VMEMMAP_PREINIT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/list_nulls.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/seqlock.h \
    $(wildcard include/config/CC_IS_GCC) \
    $(wildcard include/config/GCC_VERSION) \
    $(wildcard include/config/UBSAN_ALIGNMENT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mutex.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/debug_locks.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/pageblock-flags.h \
    $(wildcard include/config/HUGETLB_PAGE_SIZE_VARIABLE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/page-flags-layout.h \
  /home/werwolv/Development/ipc/.build/output/linux/include/generated/bounds.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/mm_types.h \
    $(wildcard include/config/HAVE_ALIGNED_STRUCT_PAGE) \
    $(wildcard include/config/SLAB_OBJ_EXT) \
    $(wildcard include/config/HUGETLB_PMD_PAGE_TABLE_SHARING) \
    $(wildcard include/config/SLAB_FREELIST_HARDENED) \
    $(wildcard include/config/USERFAULTFD) \
    $(wildcard include/config/ANON_VMA_NAME) \
    $(wildcard include/config/PER_VMA_LOCK) \
    $(wildcard include/config/HAVE_ARCH_COMPAT_MMAP_BASES) \
    $(wildcard include/config/MEMBARRIER) \
    $(wildcard include/config/ARCH_HAS_ELF_CORE_EFLAGS) \
    $(wildcard include/config/AIO) \
    $(wildcard include/config/MMU_NOTIFIER) \
    $(wildcard include/config/SPLIT_PMD_PTLOCKS) \
    $(wildcard include/config/IOMMU_MM_DATA) \
    $(wildcard include/config/KSM) \
    $(wildcard include/config/MM_ID) \
    $(wildcard include/config/CORE_DUMP_DEFAULT_ELF_HEADERS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/auxvec.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/auxvec.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/auxvec.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/auxvec.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kref.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/maple_tree.h \
    $(wildcard include/config/DEBUG_MAPLE_TREE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/uprobes.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/timer.h \
    $(wildcard include/config/DEBUG_OBJECTS_TIMERS) \
    $(wildcard include/config/NO_HZ_COMMON) \
  /home/werwolv/Development/ipc/os/linux/include/linux/ktime.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/jiffies.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/jiffies.h \
  /home/werwolv/Development/ipc/.build/output/linux/include/generated/timeconst.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/ktime.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/timekeeping.h \
    $(wildcard include/config/POSIX_AUX_CLOCKS) \
    $(wildcard include/config/GENERIC_CMOS_UPDATE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/clocksource_ids.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/debugobjects.h \
    $(wildcard include/config/DEBUG_OBJECTS) \
    $(wildcard include/config/DEBUG_OBJECTS_FREE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/workqueue.h \
    $(wildcard include/config/DEBUG_OBJECTS_WORK) \
    $(wildcard include/config/FREEZER) \
    $(wildcard include/config/SYSFS) \
    $(wildcard include/config/WQ_WATCHDOG) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/CPU_HAS_ASID) \
    $(wildcard include/config/VDSO) \
  /home/werwolv/Development/ipc/os/linux/include/linux/page-flags.h \
    $(wildcard include/config/PAGE_IDLE_FLAG) \
    $(wildcard include/config/ARCH_USES_PG_ARCH_2) \
    $(wildcard include/config/ARCH_USES_PG_ARCH_3) \
    $(wildcard include/config/MIGRATION) \
    $(wildcard include/config/DEBUG_KMAP_LOCAL_FORCE_MAP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/local_lock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/local_lock_internal.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/zswap.h \
    $(wildcard include/config/ZSWAP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/memory_hotplug.h \
    $(wildcard include/config/ARCH_HAS_ADD_PAGES) \
    $(wildcard include/config/MEMORY_HOTREMOVE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/notifier.h \
    $(wildcard include/config/TREE_SRCU) \
  /home/werwolv/Development/ipc/os/linux/include/linux/srcu.h \
    $(wildcard include/config/TINY_SRCU) \
    $(wildcard include/config/NEED_SRCU_NMI_SAFE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcu_segcblist.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/srcutree.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcu_node_tree.h \
    $(wildcard include/config/RCU_FANOUT) \
    $(wildcard include/config/RCU_FANOUT_LEAF) \
  /home/werwolv/Development/ipc/os/linux/include/linux/topology.h \
    $(wildcard include/config/USE_PERCPU_NUMA_NODE_ID) \
    $(wildcard include/config/SCHED_SMT) \
    $(wildcard include/config/GENERIC_ARCH_TOPOLOGY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/arch_topology.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/topology.h \
    $(wildcard include/config/ARM_CPU_TOPOLOGY) \
    $(wildcard include/config/BL_SWITCHER) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/topology.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/mm.h \
    $(wildcard include/config/MMU_LAZY_TLB_REFCOUNT) \
    $(wildcard include/config/ARCH_HAS_MEMBARRIER_CALLBACKS) \
    $(wildcard include/config/ARCH_HAS_SYNC_CORE_BEFORE_USERMODE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sync_core.h \
    $(wildcard include/config/ARCH_HAS_PREPARE_SYNC_CORE_CMD) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/coredump.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/list_bl.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/bit_spinlock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/uuid.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/percpu-rwsem.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcuwait.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/signal.h \
    $(wildcard include/config/SCHED_AUTOGROUP) \
    $(wildcard include/config/BSD_PROCESS_ACCT) \
    $(wildcard include/config/TASKSTATS) \
    $(wildcard include/config/STACK_GROWSUP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rculist.h \
    $(wildcard include/config/PROVE_RCU_LIST) \
  /home/werwolv/Development/ipc/os/linux/include/linux/signal.h \
    $(wildcard include/config/DYNAMIC_SIGFRAME) \
    $(wildcard include/config/PROC_FS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/jobctl.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/task.h \
    $(wildcard include/config/HAVE_EXIT_THREAD) \
    $(wildcard include/config/ARCH_WANTS_DYNAMIC_TASK_STRUCT) \
    $(wildcard include/config/HAVE_ARCH_THREAD_STRUCT_WHITELIST) \
  /home/werwolv/Development/ipc/os/linux/include/linux/uaccess.h \
    $(wildcard include/config/ARCH_HAS_SUBPAGE_FAULTS) \
    $(wildcard include/config/ARCH_MEMORY_ORDER_TSO) \
    $(wildcard include/config/HARDENED_USERCOPY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/fault-inject-usercopy.h \
    $(wildcard include/config/FAULT_INJECTION_USERCOPY) \
  /home/werwolv/Development/ipc/os/linux/include/linux/nospec.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ucopysize.h \
    $(wildcard include/config/HARDENED_USERCOPY_DEFAULT_ON) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/uaccess.h \
    $(wildcard include/config/CPU_SW_DOMAIN_PAN) \
    $(wildcard include/config/CPU_TTBR0_PAN) \
    $(wildcard include/config/HAVE_EFFICIENT_UNALIGNED_ACCESS) \
    $(wildcard include/config/UACCESS_WITH_MEMCPY) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/domain.h \
    $(wildcard include/config/IO_36) \
    $(wildcard include/config/CPU_USE_DOMAINS) \
    $(wildcard include/config/CPU_CP15_MMU) \
  /home/werwolv/Development/ipc/os/linux/include/linux/unaligned.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/unaligned/packed_struct.h \
  /home/werwolv/Development/ipc/os/linux/include/vdso/unaligned.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/pgtable.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/proc-fns.h \
    $(wildcard include/config/BIG_LITTLE) \
    $(wildcard include/config/HARDEN_BRANCH_PREDICTOR) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/glue-proc.h \
    $(wildcard include/config/CPU_ARM7TDMI) \
    $(wildcard include/config/CPU_ARM720T) \
    $(wildcard include/config/CPU_ARM740T) \
    $(wildcard include/config/CPU_ARM9TDMI) \
    $(wildcard include/config/CPU_ARM920T) \
    $(wildcard include/config/CPU_ARM922T) \
    $(wildcard include/config/CPU_ARM925T) \
    $(wildcard include/config/CPU_ARM926T) \
    $(wildcard include/config/CPU_ARM940T) \
    $(wildcard include/config/CPU_ARM946E) \
    $(wildcard include/config/CPU_ARM1020) \
    $(wildcard include/config/CPU_ARM1020E) \
    $(wildcard include/config/CPU_ARM1022) \
    $(wildcard include/config/CPU_ARM1026) \
    $(wildcard include/config/CPU_MOHAWK) \
    $(wildcard include/config/CPU_FEROCEON) \
    $(wildcard include/config/CPU_V6K) \
    $(wildcard include/config/CPU_PJ4B) \
    $(wildcard include/config/CPU_V7) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/pgtable-nopud.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/pgtable-nop4d.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/pgtable-hwdef.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/pgtable-2level-hwdef.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/tlbflush.h \
    $(wildcard include/config/SMP_ON_UP) \
    $(wildcard include/config/CPU_TLB_V4WT) \
    $(wildcard include/config/CPU_TLB_FA) \
    $(wildcard include/config/CPU_TLB_V4WBI) \
    $(wildcard include/config/CPU_TLB_FEROCEON) \
    $(wildcard include/config/CPU_TLB_V4WB) \
    $(wildcard include/config/CPU_TLB_V6) \
    $(wildcard include/config/CPU_TLB_V7) \
    $(wildcard include/config/ARM_ERRATA_720789) \
    $(wildcard include/config/ARM_ERRATA_798181) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/pgtable-2level.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/extable.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/extable.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/access_ok.h \
    $(wildcard include/config/ALTERNATE_USER_ADDRESS_SPACE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/pid.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rhashtable-types.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/posix-timers.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/alarmtimer.h \
    $(wildcard include/config/RTC_CLASS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/hrtimer.h \
    $(wildcard include/config/HIGH_RES_TIMERS) \
    $(wildcard include/config/TIME_LOW_RES) \
    $(wildcard include/config/TIMERFD) \
  /home/werwolv/Development/ipc/os/linux/include/linux/hrtimer_rearm.h \
    $(wildcard include/config/HRTIMER_REARM_DEFERRED) \
  /home/werwolv/Development/ipc/os/linux/include/linux/timerqueue.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcuref.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rcu_sync.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/quota.h \
    $(wildcard include/config/QUOTA_NETLINK_INTERFACE) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/dqblk_xfs.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dqblk_v1.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dqblk_v2.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dqblk_qtree.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/projid.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/quota.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/unicode.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dcache.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rculist_bl.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/lockref.h \
    $(wildcard include/config/ARCH_USE_CMPXCHG_LOCKREF) \
  /home/werwolv/Development/ipc/os/linux/include/linux/stringhash.h \
    $(wildcard include/config/DCACHE_WORD_ACCESS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/hash.h \
    $(wildcard include/config/HAVE_ARCH_HASH) \
  /home/werwolv/Development/ipc/os/linux/include/linux/vfsdebug.h \
    $(wildcard include/config/DEBUG_VFS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/wait_bit.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kdev_t.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/kdev_t.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/path.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/radix-tree.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/semaphore.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/migrate_mode.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/delayed_call.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ioprio.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/rt.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/iocontext.h \
    $(wildcard include/config/BLK_ICQ) \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/ioprio.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/mount.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/mnt_idmapping.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/slab.h \
    $(wildcard include/config/FAILSLAB) \
    $(wildcard include/config/KFENCE) \
    $(wildcard include/config/SLUB_TINY) \
    $(wildcard include/config/SLUB_DEBUG) \
    $(wildcard include/config/KMALLOC_PARTITION_RANDOM) \
    $(wildcard include/config/KMALLOC_PARTITION_TYPED) \
    $(wildcard include/config/SLAB_BUCKETS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/percpu-refcount.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kasan.h \
    $(wildcard include/config/KASAN_STACK) \
    $(wildcard include/config/KASAN_VMALLOC) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kasan-enabled.h \
    $(wildcard include/config/ARCH_DEFER_KASAN) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kasan-tags.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rw_hint.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/file_ref.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/fs.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/miscdevice.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/major.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/device.h \
    $(wildcard include/config/GENERIC_MSI_IRQ) \
    $(wildcard include/config/ENERGY_MODEL) \
    $(wildcard include/config/PINCTRL) \
    $(wildcard include/config/ARCH_HAS_DMA_OPS) \
    $(wildcard include/config/DMA_DECLARE_COHERENT) \
    $(wildcard include/config/DMA_CMA) \
    $(wildcard include/config/SWIOTLB) \
    $(wildcard include/config/SWIOTLB_DYNAMIC) \
    $(wildcard include/config/PM) \
    $(wildcard include/config/PM_SLEEP) \
    $(wildcard include/config/OF) \
    $(wildcard include/config/DEVTMPFS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/dev_printk.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/energy_model.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kobject.h \
    $(wildcard include/config/UEVENT_HELPER) \
    $(wildcard include/config/DEBUG_KOBJECT_RELEASE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sysfs.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kernfs.h \
    $(wildcard include/config/KERNFS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/idr.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/kobject_ns.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/cpufreq.h \
    $(wildcard include/config/CPU_FREQ) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/topology.h \
    $(wildcard include/config/SCHED_CLUSTER) \
    $(wildcard include/config/SCHED_MC) \
    $(wildcard include/config/CPU_FREQ_GOV_SCHEDUTIL) \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/idle.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/sched/sd_flags.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ioport.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/klist.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/pm.h \
    $(wildcard include/config/VT_CONSOLE_SLEEP) \
    $(wildcard include/config/CXL_SUSPEND) \
    $(wildcard include/config/PM_CLK) \
    $(wildcard include/config/PM_GENERIC_DOMAINS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/device/bus.h \
    $(wildcard include/config/ACPI) \
  /home/werwolv/Development/ipc/os/linux/include/linux/device/class.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/device/devres.h \
    $(wildcard include/config/HAS_IOMEM) \
  /home/werwolv/Development/ipc/os/linux/include/linux/device/driver.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/module.h \
    $(wildcard include/config/MODULES_TREE_LOOKUP) \
    $(wildcard include/config/STACKTRACE_BUILD_ID) \
    $(wildcard include/config/ARCH_USES_CFI_TRAPS) \
    $(wildcard include/config/MODULE_SIG) \
    $(wildcard include/config/KALLSYMS) \
    $(wildcard include/config/BPF_EVENTS) \
    $(wildcard include/config/DEBUG_INFO_BTF_MODULES) \
    $(wildcard include/config/EVENT_TRACING) \
    $(wildcard include/config/KPROBES) \
    $(wildcard include/config/MODULE_UNLOAD) \
    $(wildcard include/config/CONSTRUCTORS) \
    $(wildcard include/config/FUNCTION_ERROR_INJECTION) \
    $(wildcard include/config/MITIGATION_RETPOLINE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/buildid.h \
    $(wildcard include/config/VMCORE_INFO) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kmod.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/umh.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/elf.h \
    $(wildcard include/config/ARCH_HAVE_EXTRA_ELF_NOTES) \
    $(wildcard include/config/ARCH_USE_GNU_PROPERTY) \
    $(wildcard include/config/ARCH_HAVE_ELF_PROT) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/elf.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/user.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/elf.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/elf-em.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/moduleparam.h \
    $(wildcard include/config/ALPHA) \
    $(wildcard include/config/PPC64) \
  /home/werwolv/Development/ipc/os/linux/include/linux/rbtree_latch.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/error-injection.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/error-injection.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dynamic_debug.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/module.h \
    $(wildcard include/config/ARM_UNWIND) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/module.h \
    $(wildcard include/config/HAVE_MOD_ARCH_SPECIFIC) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/unwind.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/device-id/acpi.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/device-id/of.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/device.h \
    $(wildcard include/config/ARM_DMA_USE_IOMMU) \
    $(wildcard include/config/ARCH_OMAP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/pm_wakeup.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/mm.h \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_BITS) \
    $(wildcard include/config/HAVE_ARCH_MMAP_RND_COMPAT_BITS) \
    $(wildcard include/config/PPC32) \
    $(wildcard include/config/X86_USER_SHADOW_STACK) \
    $(wildcard include/config/RISCV_USER_CFI) \
    $(wildcard include/config/ARM64_GCS) \
    $(wildcard include/config/MEM_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_HAS_PKEYS) \
    $(wildcard include/config/ARCH_PKEY_BITS) \
    $(wildcard include/config/PARISC) \
    $(wildcard include/config/SPARC64) \
    $(wildcard include/config/ARM64_MTE) \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_MINOR) \
    $(wildcard include/config/USERFAULTFD_RWP) \
    $(wildcard include/config/MSEAL_SYSTEM_MAPPINGS) \
    $(wildcard include/config/FIND_NORMAL_PAGE) \
    $(wildcard include/config/SHMEM) \
    $(wildcard include/config/HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD) \
    $(wildcard include/config/ARCH_HAS_PTE_SPECIAL) \
    $(wildcard include/config/ARCH_SUPPORTS_PMD_PFNMAP) \
    $(wildcard include/config/ARCH_SUPPORTS_PUD_PFNMAP) \
    $(wildcard include/config/ASYNC_KERNEL_PGTABLE_FREE) \
    $(wildcard include/config/SPLIT_PTE_PTLOCKS) \
    $(wildcard include/config/HIGHPTE) \
    $(wildcard include/config/DEBUG_VM_RB) \
    $(wildcard include/config/PAGE_POISONING) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
    $(wildcard include/config/DEBUG_PAGEALLOC) \
    $(wildcard include/config/ARCH_WANT_OPTIMIZE_DAX_VMEMMAP) \
    $(wildcard include/config/HUGETLBFS) \
    $(wildcard include/config/MAPPING_DIRTY_HELPERS) \
    $(wildcard include/config/PAGE_POOL) \
  /home/werwolv/Development/ipc/os/linux/include/linux/pgalloc_tag.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/mmap_lock.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/range.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/page_ext.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/stacktrace.h \
    $(wildcard include/config/ARCH_STACKWALK) \
    $(wildcard include/config/STACKTRACE) \
    $(wildcard include/config/HAVE_RELIABLE_STACKTRACE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/page_ref.h \
    $(wildcard include/config/DEBUG_PAGE_REF) \
  /home/werwolv/Development/ipc/os/linux/include/linux/pgtable.h \
    $(wildcard include/config/PGTABLE_LEVELS) \
    $(wildcard include/config/ARCH_HAS_NONLEAF_PMD_YOUNG) \
    $(wildcard include/config/ARCH_HAS_HW_PTE_YOUNG) \
    $(wildcard include/config/GUP_GET_PXX_LOW_HIGH) \
    $(wildcard include/config/ARCH_WANT_PMD_MKWRITE) \
    $(wildcard include/config/HAVE_ARCH_SOFT_DIRTY) \
    $(wildcard include/config/ARCH_HAS_PMD_SOFTLEAVES) \
    $(wildcard include/config/ARCH_HAS_PTE_PROTNONE) \
    $(wildcard include/config/HAVE_ARCH_HUGE_VMAP) \
    $(wildcard include/config/X86_ESPFIX64) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/pgtable_uffd.h \
    $(wildcard include/config/HAVE_ARCH_USERFAULTFD_WP) \
    $(wildcard include/config/PTE_MARKER_UFFD_WP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/page_table_check.h \
    $(wildcard include/config/PAGE_TABLE_CHECK) \
  /home/werwolv/Development/ipc/os/linux/include/linux/memremap.h \
    $(wildcard include/config/DEVICE_PRIVATE) \
    $(wildcard include/config/PCI_P2PDMA) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cacheinfo.h \
    $(wildcard include/config/ACPI_PPTT) \
    $(wildcard include/config/ARM) \
    $(wildcard include/config/ARCH_HAS_CPU_CACHE_ALIASING) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpuhplock.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cachetype.h \
    $(wildcard include/config/CPU_CACHE_VIVT) \
    $(wildcard include/config/CPU_CACHE_VIPT) \
  /home/werwolv/Development/ipc/os/linux/include/linux/iommu-debug-pagealloc.h \
    $(wildcard include/config/IOMMU_DEBUG_PAGEALLOC) \
  /home/werwolv/Development/ipc/os/linux/include/linux/huge_mm.h \
    $(wildcard include/config/PGTABLE_HAS_HUGE_LEAVES) \
    $(wildcard include/config/PERSISTENT_HUGE_ZERO_FOLIO) \
  /home/werwolv/Development/ipc/os/linux/include/linux/vmstat.h \
    $(wildcard include/config/VM_EVENT_COUNTERS) \
    $(wildcard include/config/DEBUG_TLBFLUSH) \
    $(wildcard include/config/PER_VMA_LOCK_STATS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/vm_event_item.h \
    $(wildcard include/config/BALLOON) \
    $(wildcard include/config/BALLOON_MIGRATION) \
    $(wildcard include/config/X86) \
    $(wildcard include/config/DEBUG_STACK_USAGE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/mman.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/mman.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/mman.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/system_info.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/uapi/asm/mman.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/mman.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/mman-common.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/asm-generic/hugetlb_encode.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/pagemap.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/highmem.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cacheflush.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cacheflush.h \
    $(wildcard include/config/ARM_ERRATA_411920) \
    $(wildcard include/config/OUTER_CACHE) \
    $(wildcard include/config/CPU_ICACHE_MISMATCH_WORKAROUND) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/glue-cache.h \
    $(wildcard include/config/CPU_CACHE_V4) \
    $(wildcard include/config/CPU_CACHE_V4WB) \
    $(wildcard include/config/CACHE_B15_RAC) \
    $(wildcard include/config/CPU_CACHE_NOP) \
    $(wildcard include/config/CPU_CACHE_V7) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/OUTER_CACHE_SYNC) \
  /home/werwolv/Development/ipc/os/linux/include/linux/kmsan.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/dma-direction.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/hardirq.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/context_tracking_state.h \
    $(wildcard include/config/CONTEXT_TRACKING_USER) \
    $(wildcard include/config/CONTEXT_TRACKING) \
    $(wildcard include/config/RCU_DYNTICKS_TORTURE) \
  /home/werwolv/Development/ipc/os/linux/include/linux/ftrace_irq.h \
    $(wildcard include/config/HWLAT_TRACER) \
    $(wildcard include/config/OSNOISE_TRACER) \
  /home/werwolv/Development/ipc/os/linux/include/linux/vtime.h \
    $(wildcard include/config/VIRT_CPU_ACCOUNTING) \
    $(wildcard include/config/HAVE_VIRT_CPU_ACCOUNTING_IDLE) \
    $(wildcard include/config/IRQ_TIME_ACCOUNTING) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/hardirq.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/irq.h \
    $(wildcard include/config/SPARSE_IRQ) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/hardirq.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/irq.h \
    $(wildcard include/config/GENERIC_IRQ_EFFECTIVE_AFF_MASK) \
    $(wildcard include/config/GENERIC_IRQ_IPI) \
    $(wildcard include/config/IRQ_DOMAIN_HIERARCHY) \
    $(wildcard include/config/DEPRECATED_IRQ_CPU_ONOFFLINE) \
    $(wildcard include/config/GENERIC_IRQ_MIGRATION) \
    $(wildcard include/config/GENERIC_PENDING_IRQ) \
    $(wildcard include/config/HARDIRQS_SW_RESEND) \
    $(wildcard include/config/GENERIC_IRQ_CHIP) \
    $(wildcard include/config/GENERIC_IRQ_MULTI_HANDLER) \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqhandler.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqreturn.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/io.h \
    $(wildcard include/config/HAS_IOPORT_MAP) \
    $(wildcard include/config/PCI) \
    $(wildcard include/config/STRICT_DEVMEM) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/io.h \
    $(wildcard include/config/PCMCIA) \
    $(wildcard include/config/NEED_MACH_IO_H) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/pci_iomap.h \
    $(wildcard include/config/NO_GENERIC_PCI_IOPORT_MAP) \
    $(wildcard include/config/GENERIC_PCI_IOMAP) \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/io.h \
    $(wildcard include/config/GENERIC_IOMAP) \
    $(wildcard include/config/TRACE_MMIO_ACCESS) \
    $(wildcard include/config/HAS_IOPORT) \
    $(wildcard include/config/GENERIC_IOREMAP) \
  /home/werwolv/Development/ipc/os/linux/include/linux/logic_pio.h \
    $(wildcard include/config/INDIRECT_PIO) \
  /home/werwolv/Development/ipc/os/linux/include/linux/fwnode.h \
  /home/werwolv/Development/ipc/.build/output/linux/arch/arm/include/generated/asm/irq_regs.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/irq_regs.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/irqdesc.h \
    $(wildcard include/config/GENERIC_IRQ_STAT_SNAPSHOT) \
    $(wildcard include/config/GENERIC_IRQ_DEBUGFS) \
    $(wildcard include/config/IRQ_DOMAIN) \
  /home/werwolv/Development/ipc/os/linux/include/linux/irq_work.h \
    $(wildcard include/config/IRQ_WORK) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/irq_work.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/smp_plat.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cpu.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpu.h \
    $(wildcard include/config/GENERIC_CPU_DEVICES) \
    $(wildcard include/config/PM_SLEEP_SMP) \
    $(wildcard include/config/PM_SLEEP_SMP_NONZERO_CPU) \
    $(wildcard include/config/ARCH_HAS_CPU_FINALIZE_INIT) \
    $(wildcard include/config/CPU_MITIGATIONS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/node.h \
    $(wildcard include/config/HMEM_REPORTING) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpuhotplug.h \
    $(wildcard include/config/HOTPLUG_CORE_SYNC_DEAD) \
  /home/werwolv/Development/ipc/os/linux/include/linux/cpu_smt.h \
    $(wildcard include/config/HOTPLUG_SMT) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/cputype.h \
    $(wildcard include/config/CPU_CP15) \
    $(wildcard include/config/CPU_PJ4) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/hw_irq.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/highmem-internal.h \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/highmem.h \
    $(wildcard include/config/DEBUG_HIGHMEM) \
  /home/werwolv/Development/ipc/os/linux/arch/arm/include/asm/fixmap.h \
  /home/werwolv/Development/ipc/os/linux/include/asm-generic/fixmap.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/hugetlb_inline.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/user_namespace.h \
    $(wildcard include/config/INOTIFY_USER) \
    $(wildcard include/config/FANOTIFY) \
    $(wildcard include/config/BINFMT_MISC) \
    $(wildcard include/config/PERSISTENT_KEYRINGS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/nsproxy.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ns_common.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/ns/ns_common_types.h \
    $(wildcard include/config/IPC_NS) \
    $(wildcard include/config/NET_NS) \
    $(wildcard include/config/PID_NS) \
    $(wildcard include/config/TIME_NS) \
    $(wildcard include/config/UTS_NS) \
  /home/werwolv/Development/ipc/os/linux/include/linux/ns/nstree_types.h \
  /home/werwolv/Development/ipc/os/linux/include/uapi/linux/nsfs.h \
  /home/werwolv/Development/ipc/os/linux/include/linux/rculist_nulls.h \
  /home/werwolv/Development/ipc/os/modules/ipc/./include/ipc.h \

source/ipc.o: $(deps_source/ipc.o)

$(deps_source/ipc.o):
