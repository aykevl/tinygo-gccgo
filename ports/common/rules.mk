
GCC=$(TOOLCHAIN)gcc
GCCGO=$(TOOLCHAIN)gccgo

clean:
	@echo "RMRF $(BUILD)"
	$(Q)rm -rf $(BUILD)

# Make build directories. Must always run as the first command.
mkdir:
	@mkdir -p $(DIRS)

# Build a single runtime.o for all Go sources.
$(BUILD)/pkg/runtime.o: $(SRC_GO_RUNTIME)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -fgo-pkgpath=runtime -c -o $@ $^

# Build the 'main' package (with program name as filename)
$(BUILD)/pkg/$(PKG).o: $(TOP)/src/$(PKG)/*.go
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg

# Below here, you can find various packages that may or may not function.
# Package unicode and strconv are functional, while fmt is unusable (but you
# can reference symbols in there so it's not completely broken).

$(BUILD)/pkg/fmt.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/fmt/{{ . }} {{end}}' $(PKGROOT)/fmt)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=fmt

$(BUILD)/pkg/math.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/math/{{ . }} {{end}}' $(PKGROOT)/math)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=math

$(BUILD)/pkg/strconv.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/strconv/{{ . }} {{end}}' $(PKGROOT)/strconv)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=strconv

$(BUILD)/pkg/errors.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/errors/{{ . }} {{end}}' $(PKGROOT)/errors)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=errors

$(BUILD)/pkg/unicode.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/unicode/{{ . }} {{end}}' $(PKGROOT)/unicode)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=unicode

$(BUILD)/pkg/unicode/utf8.o: $(shell go list -f '{{ range .GoFiles }}$(PKGROOT)/unicode/utf8/{{ . }} {{end}}' $(PKGROOT)/unicode/utf8)
	@echo "Go   $@"
	$(Q)$(GCCGO) $(GOFLAGS) -c -o $@ $^ -fgo-relative-import-path=/home/ayke/src/tinygo -I $(BUILD)/pkg -fgo-pkgpath=unicode/utf8

# Build libgo C sources.
# TODO: this uses gnu99 to work around undefined stack_t
$(BUILD)/libgo/%.o: $(GOFRONTEND)/libgo/runtime/%.c
	@echo "CC   $^"
	$(Q)$(GCC) $(CFLAGS) -std=gnu99 -c -o $@ $^

# Build tinygo C sources.
$(BUILD)/tinygo/%.o: $(TOP)/src/runtime/%.c
	@echo "CC   $^"
	$(Q)$(GCC) $(CFLAGS) -std=c99 -fexceptions -c -o $@ $^

# Build port-specific C sources.
$(BUILD)/port/%.o: %.c
	@echo "CC   $^"
	$(Q)$(GCC) $(CFLAGS) -std=c99 -fexceptions -c -o $@ $^
