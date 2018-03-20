
CC=$(TOOLCHAIN)gcc
GXX=$(TOOLCHAIN)g++
GCCGO=$(TOOLCHAIN)gccgo
LD=$(TOOLCHAIN)ld
AR=$(TOOLCHAIN)gcc-ar

# Make build directories. Must always run as the first command.
mkdir:
	@mkdir -p $(DIRS)

clean:
	@echo "RMRF $(BUILD)"
	$(Q)rm -rf $(BUILD)
	@echo "RMRF $(TOP)/libgo/pkg"
	$(Q)rm -rf $(TOP)/libgo/pkg

# Build a single runtime.a for all Go sources.
$(BUILD)/pkg/runtime.a: $(SRC_GO_RUNTIME)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=runtime -c -o $@ $^

$(BUILD)/syscall.o: $(SRC_GO_SYSCALL)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=syscall -c -o $@ $^

$(BUILD)/os.o: $(SRC_GO_OS) $(BUILD)/syscall.o
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=os -c -o $@ $(SRC_GO_OS)

$(BUILD)/machine.o: $(SRC_GO_MACHINE)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=machine -c -o $@ $(SRC_GO_MACHINE)

# Build libgo C sources.
# TODO: this uses gnu99 to work around undefined stack_t
$(BUILD)/libgo/%.o: $(GCCREPO)/libgo/runtime/%.c
	@echo "CC   $^"
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -c -o $@ $^

# Build tinygo C sources.
$(BUILD)/tinygo/%.o: $(TOP)/src/runtime/%.c
	@echo "CC   $^"
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -fexceptions -c -o $@ $^

# Build port-specific C sources.
$(BUILD)/port/%.o: $(TOP)/ports/$(PORT)/%.c
	@echo "CC   $^"
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -fexceptions -c -o $@ $^

# Create runtime library
$(BUILD)/libruntime.a: $(OBJ)
	@echo "AR   $@"
	$(Q)rm -f $@
	$(Q)$(AR) rcs $@ $^
