
# Define which package/cmd to build. It will be stored in $(BUILD).
PKG ?= hello

ifneq ($(V),1)
Q=@
endif

ifeq ($(ARCH),)
ARCH = other
endif

TOP=../..

# Various common flags for GCC.
FLAGS += -Os -g -Wall -Werror -I$(GOFRONTEND)/libgo/runtime -I$(TOP)/src/runtime

INC += -I.
CFLAGS += $(FLAGS) $(INC)
GOFLAGS += $(FLAGS) -fno-split-stack
LDFLAGS += $(CFLAGS)

# Add possibility of enabling LTO. This greatly reduces binary size and stack
# usage (due to inlining), but is less tested and may give problems.
LTO ?= 0
ifeq ($(LTO),1)
# currenty gives problems
FLAGS += -flto -fno-strict-aliasing -Wno-lto-type-mismatch
else
# Currently we rely on --gc-sections for garbage collection, to remove
# unreferenced symbols. If this isn't done, there will be lots of linker
# errors.
FLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections
endif

# gofrontend/libgo/runtime uses typedef'd anonymous structures, which are not
# allowed in the C11 standard.
# Reference:
# https://stackoverflow.com/questions/9142163/c11-anonymous-structs-via-typedefs
CFLAGS += -fplan9-extensions

# Find all the dependencies of the main package.
PKGDEPS = $(shell go list -f '{{ join .Imports " " }}' $(TOP)/src/$(PKG))

# Where the gofrontend is located (usually in the git submodule).
GOFRONTEND = $(TOP)/gofrontend

ifeq ("$(wildcard $(GOFRONTEND)/libgo)","")
$(info $(GOFRONTEND) does not exist - try running: git submodule update --init)
$(error no gofrontend available)
endif

# Which sources should be used for Go dependencies.
#PKGROOT = /usr/share/go-1.7/src
PKGROOT = $(GOFRONTEND)/libgo/go

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
	print.c \
	go-append.c \
	go-assert.c \
	go-assert-interface.c \
	go-byte-array-to-string.c \
	go-can-convert-interface.c \
	go-check-interface.c \
	go-convert-interface.c \
	go-copy.c \
	go-eface-compare.c \
	go-int-to-string.c \
	go-make-slice.c \
	go-map-delete.c \
	go-map-index.c \
	go-map-len.c \
	go-map-range.c \
	go-memcmp.c \
	go-new-map.c \
	go-print.c \
	go-runtime-error.c \
	go-strcmp.c \
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
	go-unsafe-pointer.c

# Runtime C sources from tinygo. These live in src/runtime.
SRC_C_TINYGO = \
	tinygo.c \
	channel.c \
	panic.c \
	libgo-go-iface.c \
	libgo-map.c \
	libgo-runtime1.c \
	libgo-string.c

# Runtime Go sources (both from tinygo and golang itself). These have to be
# built all at once, or there will be undefined variables and such.
SRC_GO_RUNTIME += \
	$(TOP)/src/runtime/runtime.go \
	$(GOFRONTEND)/libgo/go/runtime/error.go

# Architecture-specific flags.
SRC_C_TINYGO += tinygo_$(ARCH).c
SRC_GO_RUNTIME += $(TOP)/src/runtime/runtime_$(ARCH).go

# Create a lit of all Go package dependencies, stored as $(BUILD)/pkg/*.o
# files.
# Always include the runtime and the main package as a dependency.
OBJ_GO += build/pkg/$(PKG).o
OBJ_GO += $(addsuffix .o,$(addprefix $(BUILD)/pkg/,$(PKGDEPS)))
OBJ_GO += build/pkg/runtime.o

# Create a list of all object files to be linked in the final executable.
OBJ += $(OBJ_GO)
OBJ += $(addprefix $(BUILD)/tinygo/,$(SRC_C_TINYGO:.c=.o))
OBJ += $(addprefix $(BUILD)/libgo/,$(SRC_C_LIBGO:.c=.o))
OBJ += $(addprefix $(BUILD)/port/,$(SRC_C_PORT:.c=.o))
