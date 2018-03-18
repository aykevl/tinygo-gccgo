
# Specify the build output directory. All object code will be stored in this
# directory.
BUILD = build

PORT = unix

FLAGS += -fomit-frame-pointer

include ../common/vars.mk

.PHONY: all clean run mkdir test test-pkg

# Find the current architecture.
# TODO: cross compilation
ARCH_TUPLE = $(shell gcc -dumpmachine)
ifneq (,$(findstring arm-,"$(ARCH_TUPLE)"))
ARCH = arm
else
ARCH = otherarch
endif

SRC_C_PORT = main.c
SRC_GO_RUNTIME += runtime.go

OBJ += $(BUILD)/port/os.o

DIRS += $(BUILD)/bin

all: mkdir $(BUILD)/libruntime.a $(BUILD)/syscall.o

# For anything other than build/libruntime.a
$(BUILD)/%.a: build/libruntime.a
	@echo "Go   $@"
	$(Q)../../tinygo build -o $@ $(patsubst $(BUILD)/%.a,%,$@)

SRC_GO_SYSCALL += syscall.go

# Per-port packages
$(BUILD)/port/os.o: os/os.go
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=os -c -o $@ $^

######################
#    Testing code    #
######################

# Build the main package
$(BUILD)/pkg/%.o: $(TOP)/src/%/*.go
	@echo "Go   $@"
	$(Q)gccgo $(GOFLAGS) -c -o $@ $^

# Link the executable.
$(BUILD)/bin/%: $(OBJ) $(BUILD)/pkg/%.o $(BUILD)/strconv.a
	@echo "LINK $@"
	$(Q)gcc $(LDFLAGS) -o $@ $^

TEST_OUTPUT_FILE = $(BUILD)/test/$(PKG).txt

$(BUILD)/test/%.txt: $(BUILD)/bin/%
	@$< 2>&1 > $@

# Test the current package.
test-pkg: mkdir $(BUILD)/test/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo testing package $(PKG)
	@diff -u $(TOP)/tests/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo OK

test: all
	$(TOP)/tests/run-tests

include ../common/rules.mk
