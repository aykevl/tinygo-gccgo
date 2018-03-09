
# Specify the build output directory. All object code will be stored in this
# directory.
BUILD=build

# Define which package/cmd to build. It will be stored in $(BUILD).
PKG ?= hello

# Define where to put the output binary
OUT = $(BUILD)/$(PKG)

.PHONY: all clean run mkdir test test-pkg

# Various common flags for GCC.
FLAGS=-Os -g -Wall -Werror -fomit-frame-pointer -Igofrontend/libgo/runtime -Isrc/runtime

CFLAGS = $(FLAGS)
GOFLAGS = $(FLAGS) -fno-split-stack
LDFLAGS = $(CFLAGS)

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

all: mkdir $(OUT)

clean:
	rm -rf $(BUILD)

run: all
	@echo RUN $(OUT)
	@$(OUT)

# Find all the dependencies of the main package.
PKGDEPS = $(shell go list -f '{{ join .Imports " " }}' ./src/$(PKG))

# Which sources should be used for Go dependencies.
#PKGROOT = /usr/share/go-1.7/src
PKGROOT = gofrontend/libgo/go

# Directories required during the build. Will be created by mkdir at the start.
DIRS = \
	$(BUILD)/tinygo \
	$(BUILD)/libgo \
	$(BUILD)/pkg \
	$(BUILD)/test

# libgo C sources used in the runtime. These live in gofrontend/libgo/runtime.
# More should be added in the future, but this already works quite well.
SRC_C_LIBGO += \
	print.c \
	go-print.c \
	go-append.c \
	go-copy.c \
	go-memcmp.c \
	go-strslice.c \
	go-strplus.c \
	go-strcmp.c \
	go-make-slice.c \
	go-new-map.c \
	go-map-index.c \
	go-map-len.c \
	go-map-delete.c \
	go-map-range.c \
	go-byte-array-to-string.c \
	go-int-to-string.c \
	go-type-identity.c \
	go-type-interface.c \
	go-type-eface.c \
	go-type-error.c \
	go-type-string.c \
	go-type-float.c \
	go-type-complex.c \
	go-typedesc-equal.c \
	go-check-interface.c \
	go-assert-interface.c \
	go-convert-interface.c \
	go-can-convert-interface.c \
	go-eface-compare.c \
	go-unsafe-pointer.c \
	go-assert.c \
	go-runtime-error.c

# Runtime C sources from tinygo. These live in src/runtime.
SRC_C_TINYGO = \
	tinygo.c \
	tinygo_arm.c \
	channel.c \
	panic.c \
	libgo-go-iface.c \
	libgo-map.c \
	libgo-runtime1.c \
	libgo-string.c

# Runtime Go sources (both from tinygo and golang itself). These have to be
# built all at once, or there will be undefined variables and such.
SRC_GO_RUNTIME += \
	src/runtime/runtime.go \
	gofrontend/libgo/go/runtime/error.go

# Build a single runtime.o for all Go sources.
$(BUILD)/pkg/runtime.o: $(SRC_GO_RUNTIME)
	gccgo $(GOFLAGS) -fgo-pkgpath=runtime -c -o $@ $^

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

# Make build directories. Must always run as the first command.
mkdir:
	@mkdir -p $(DIRS)

# Build the runtime package.
$(BUILD)/pkg/$(PKG).o: src/$(PKG)/*.go
	gccgo $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg

# Below here, you can find various packages that may or may not function.
# Package unicode and strconv are functional, while fmt is unusable (but you
# can reference symbols in there so it's not completely broken).

$(BUILD)/pkg/fmt.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/fmt/{{ . }} {{end}}' ./$(PKGROOT)/fmt)
	gccgo $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=fmt

$(BUILD)/pkg/strconv.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/strconv/{{ . }} {{end}}' ./$(PKGROOT)/strconv)
	gccgo $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=strconv

$(BUILD)/pkg/unicode.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/unicode/{{ . }} {{end}}' ./$(PKGROOT)/unicode)
	gccgo $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=unicode

# Build libgo C sources.
# TODO: this uses gnu99 to work around undefined stack_t
$(BUILD)/libgo/%.o: gofrontend/libgo/runtime/%.c
	gcc $(CFLAGS) -std=gnu99 -c -o $@ $^

# Build tinygo C sources.
$(BUILD)/tinygo/%.o: src/runtime/%.c
	gcc $(CFLAGS) -std=c99 -fexceptions -c -o $@ $^

# Link the final executable.
$(OUT): $(OBJ)
	gcc $(LDFLAGS) -o $@ $^


######################
#    Testing code    #
######################

TEST_OUTPUT_FILE = $(BUILD)/test/$(PKG).txt

$(BUILD)/test/%.txt: $(BUILD)/%
	@$< 2>&1 > $@

# Test the current package.
test-pkg: mkdir tests/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo testing package $(PKG)
	@diff -u tests/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo OK

test:
	./tests/run-tests
