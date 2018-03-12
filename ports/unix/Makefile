
# Specify the build output directory. All object code will be stored in this
# directory.
BUILD=build

# Define where to put the output binary
OUT = $(BUILD)/$(PKG)

FLAGS += -fomit-frame-pointer

include ../common/vars.mk

.PHONY: all clean run mkdir test test-pkg

# Find the current architecture.
# TODO: cross compilation
ARCH_TUPLE = $(shell gcc -dumpmachine)
ifneq (,$(findstring arm-,"$(ARCH_TUPLE)"))
ARCH = arm
else
ARCH = other
endif

SRC_C_PORT = main.c

all: mkdir $(OUT)

# Link the final executable.
$(OUT): $(OBJ)
	@echo "LINK $@"
	$(Q)gcc $(LDFLAGS) -o $@ $^

run: all
	@echo "RUN  $(OUT)"
	@$(OUT)

######################
#    Testing code    #
######################

TEST_OUTPUT_FILE = $(BUILD)/test/$(PKG).txt

$(BUILD)/test/%.txt: $(BUILD)/%
	@$< 2>&1 > $@

# Test the current package.
test-pkg: mkdir $(BUILD)/test/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo testing package $(PKG)
	@diff -u $(TOP)/tests/$(PKG).txt $(TEST_OUTPUT_FILE)
	@echo OK

test:
	$(TOP)/tests/run-tests

include ../common/rules.mk
