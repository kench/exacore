common_OBJECTS = \
        common/clocks.o \
        common/xmalloc.o \
        common/posix_util.o \
        common/cpu_dispatch.o \
        common/rational.o \
        common/backtrace.o \
        common/new_delete_malloc.o \
        common/hex_dump.o \
        common/instrument.o \
        common/block_set.o \
        common/buffer.o \
        common/serialize.o

common_LIBS = -lrt
