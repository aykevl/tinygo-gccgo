
BUILD ?= build

ifneq ($(V),1)
Q=@
endif

# Default arch, should be overridden (if there is an implementation).
ARCH ?= otherarch

# Root of the project.
TOP ?= ../..

# Various common flags for GCC.
# Add not only -Wall and -Werror, but also some extra errors that could be
# useful (from -Wextra). Don't add -Wextra directly as it produces a lot of
# noise and some code doesn't compile with it.
FLAGS += -Os -g -Wall -Werror -Wtype-limits

# Contains the standard library and parts of the runtime.
GCCREPO = $(TOP)/gcc

INC += -I.
INC += -I$(TOP)/ports/$(PORT)
INC += -I$(TOP)
CFLAGS += $(FLAGS) $(INC)
CXXFLAGS += $(FLAGS) $(INC)
GOFLAGS += $(FLAGS) -L$(BUILD) -fno-split-stack
LDFLAGS += $(CFLAGS) -Wl,--gc-sections

$(BUILD)/tinygo/%.o: CFLAGS += -I$(GCCREPO)/libgo/runtime
$(BUILD)/libgo/%.o: CFLAGS += -I$(GCCREPO)/libgo/runtime
$(BUILD)/port/%.o: CFLAGS += -I$(GCCREPO)/libgo/runtime

# Currently we rely on --gc-sections for garbage collection, to remove
# unreferenced symbols. If this isn't done, there will be lots of linker
# errors.
FLAGS += -ffunction-sections -fdata-sections

# Add possibility of enabling LTO. This greatly reduces binary size and stack
# usage (due to inlining), but is less tested and may give problems.
LTO ?= 1
ifeq ($(LTO),1)
FLAGS += -flto -Wno-lto-type-mismatch -ffat-lto-objects # -fuse-linker-plugin
endif

# gofrontend/libgo/runtime uses typedef'd anonymous structures, which are not
# allowed in the C11 standard.
# Reference:
# https://stackoverflow.com/questions/9142163/c11-anonymous-structs-via-typedefs
CFLAGS += -fplan9-extensions

ifeq ("$(wildcard $(GCCREPO)/libgo)","")
$(info $(GCCREPO) does not exist - try running: git submodule update --init)
$(error no gofrontend available)
endif

# Which standard library should be used.
PKGROOT = $(GCCREPO)/libgo/go

# Directories required during the build. Will be created by mkdir at the start.
DIRS += \
	$(BUILD)/tinygo \
	$(BUILD)/libgo \
	$(BUILD)/port \
	$(BUILD)/pkg \
	$(BUILD)/test

# libgo C sources used in the runtime. These live in gofrontend/libgo/runtime.
# More should be added in the future, but this already works quite well.
SRC_C_LIBGO += \
	go-append.c \
	go-assert.c \
	go-assert-interface.c \
	go-byte-array-to-string.c \
	go-can-convert-interface.c \
	go-check-interface.c \
	go-construct-map.c \
	go-convert-interface.c \
	go-copy.c \
	go-eface-compare.c \
	go-int-array-to-string.c \
	go-int-to-string.c \
	go-interface-compare.c \
	go-make-slice.c \
	go-map-delete.c \
	go-map-index.c \
	go-map-len.c \
	go-map-range.c \
	go-memcmp.c \
	go-new-map.c \
	go-print.c \
	go-reflect-call.c \
	go-reflect-map.c \
	go-rune.c \
	go-runtime-error.c \
	go-strcmp.c \
	go-string-to-byte-array.c \
	go-string-to-int-array.c \
	go-strplus.c \
	go-strslice.c \
	go-type-complex.c \
	go-type-eface.c \
	go-type-error.c \
	go-type-float.c \
	go-type-identity.c \
	go-type-interface.c \
	go-type-string.c \
	go-typedesc-equal.c \
	go-unsafe-new.c \
	go-unsafe-newarray.c \
	go-unsafe-pointer.c

# Runtime C sources from tinygo. These live in src/runtime.
SRC_C_TINYGO = \
	tinygo.c \
	channel.c \
	heap.c \
	panic.c \
	print.c \
	libgo-go-iface.c \
	libgo-map.c \
	libgo-reflect.c \
	libgo-runtime1.c \
	libgo-string.c

# Runtime Go sources (both from tinygo and golang itself). These have to be
# built all at once, or there will be undefined variables and such.
SRC_GO_RUNTIME += \
	$(TOP)/src/runtime/runtime.go \
	$(GCCREPO)/libgo/go/runtime/error.go

SRC_GO_SYSCALL += \
	$(TOP)/src/syscall/syscall.go

SRC_GO_OS += \
	$(TOP)/src/os/os.go

LIBS += \
	$(BUILD)/libruntime.a \
	$(BUILD)/syscall.o \
	$(BUILD)/os.o

# Architecture-specific files.
SRC_C_TINYGO += tinygo_$(ARCH).c
SRC_GO_RUNTIME += $(TOP)/src/runtime/runtime_$(ARCH).go

# Create a list of all object files to be linked in the final executable.
OBJ += build/pkg/runtime.a
OBJ += $(addprefix $(BUILD)/tinygo/,$(SRC_C_TINYGO:.c=.o))
OBJ += $(addprefix $(BUILD)/libgo/,$(SRC_C_LIBGO:.c=.o))
OBJ += $(addprefix $(BUILD)/port/,$(SRC_C_PORT:%.c=%.o))
