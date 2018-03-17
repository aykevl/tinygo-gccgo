
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

DIRS += $(BUILD)/bin

all: mkdir $(BUILD)/libruntime.a

# For anything other than build/libruntime.a
$(BUILD)/%.a: build/libruntime.a
	$(Q)../../tinygo build -o $@ $(patsubst $(BUILD)/%.a,%,$@)

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
