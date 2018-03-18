
# Specify the build output directory. All object code will be stored in this
# directory.
BUILD = build
PORT = unix

include ../common/vars.mk

.PHONY: all clean run mkdir test

# Find the current architecture.
# TODO: cross compilation
ARCH_TUPLE = $(shell gcc -dumpmachine)
ifneq (,$(findstring arm-,"$(ARCH_TUPLE)"))
ARCH = arm
else
ARCH = otherarch
endif

FLAGS += -fomit-frame-pointer

SRC_C_PORT = main.c
SRC_GO_RUNTIME += runtime.go
SRC_GO_SYSCALL += syscall.go
SRC_GO_OS += os.go

DIRS += $(BUILD)/bin

all: mkdir $(LIBS)

# For anything other than build/libruntime.a
$(BUILD)/%.a:
	@echo "Go   $@"
	$(Q)../../tinygo build -o $@ $(patsubst $(BUILD)/%.a,%,$@)


######################
#    Testing code    #
######################

# Build the main package
$(BUILD)/pkg/%.o: $(TOP)/src/%/*.go
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^

TEST_OBJS = \
	$(BUILD)/errors.a \
	$(BUILD)/math.a \
	$(BUILD)/strconv.a \
	$(OBJ)

# Link the executable.
$(BUILD)/bin/%: $(BUILD)/pkg/%.o $(TEST_OBJS)
	@echo "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/test/%.txt: $(BUILD)/bin/%
	@echo "RUN  $^"
	@$< 2>&1 > $@

# Keep Make from removing intermediary files
.SECONDARY: $(TEST_OBJS)

test: all
	$(TOP)/tests/run-tests

include ../common/rules.mk
