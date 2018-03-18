
TOOLCHAIN = xtensa-esp32-elf-
PORT = esp32
BUILD = build

include ../common/vars.mk

SRC_C_PORT += main.c
SRC_GO_RUNTIME += runtime.go
SRC_GO_OS += os.go
SRC_GO_MACHINE += machine.go

LIBS += $(BUILD)/machine.o

all: mkdir $(LIBS) $(BUILD)/esp32_out.ld #$(BUILD)/bootloader.bin

$(BUILD)/esp32_out.ld: $(IDF_PATH)/components/esp32/ld/esp32.ld
	@echo "CPP  $^"
	$(Q)$(CC) -I$(TOP)/ports/$(PORT) -C -P -x c -E $< -o $@

# Build assembly sources from the port SDK.
$(BUILD)/sdk/%.o: $(IDF_PATH)/%.S
	@echo "AS   $^"
	@mkdir -p "$(basename $@)"
	$(Q)$(CC) $(CFLAGS) -std=c99 -fexceptions -c -o $@ $^

# Build C sources from the port SDK.
$(BUILD)/sdk/%.o: $(IDF_PATH)/%.c
	@echo "CC   $^"
	@mkdir -p "$(basename $@)"
	$(Q)$(CC) $(CFLAGS) -std=gnu99 -fexceptions -c -o $@ $^

# Build C++ sources from the port SDK.
$(BUILD)/sdk/%.o: $(IDF_PATH)/%.cpp
	@echo "CXX  $^"
	@mkdir -p "$(basename $@)"
	$(Q)$(GXX) $(CXXFLAGS) -std=c++11 -fexceptions -c -o $@ $^

ESPCOMP = $(IDF_PATH)/components

SDK_BIN = sdk/components

############################
##    MicroPython code    ##
############################
#
# Note: the following code has been taken from ports/esp32/Makefile of the
# MicroPython project, and has been modified to work with TinyGo.
# URL:
#
#   https://github.com/micropython/micropython/blob/master/ports/esp32/Makefile
#
# Original license, taken from the root of the project (the Makefile doesn't
# have a license header):
#
#   The MIT License (MIT)
#
#   Copyright (c) 2013, 2014 Damien P. George
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.

INC_ESPCOMP += -I$(ESPCOMP)/bootloader_support/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include
INC_ESPCOMP += -I$(ESPCOMP)/driver/include/driver
INC_ESPCOMP += -I$(ESPCOMP)/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/include
INC_ESPCOMP += -I$(ESPCOMP)/soc/esp32/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/expat/include/expat
INC_ESPCOMP += -I$(ESPCOMP)/expat/port/include
INC_ESPCOMP += -I$(ESPCOMP)/heap/include
INC_ESPCOMP += -I$(ESPCOMP)/json/include
INC_ESPCOMP += -I$(ESPCOMP)/json/port/include
INC_ESPCOMP += -I$(ESPCOMP)/log/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/include
INC_ESPCOMP += -I$(ESPCOMP)/nvs_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/freertos/include
INC_ESPCOMP += -I$(ESPCOMP)/tcpip_adapter/include
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip/port
INC_ESPCOMP += -I$(ESPCOMP)/lwip/include/lwip/posix
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/include
INC_ESPCOMP += -I$(ESPCOMP)/mbedtls/port/include
INC_ESPCOMP += -I$(ESPCOMP)/spi_flash/include
INC_ESPCOMP += -I$(ESPCOMP)/vfs/include
INC_ESPCOMP += -I$(ESPCOMP)/newlib/platform_include
INC_ESPCOMP += -I$(ESPCOMP)/xtensa-debug-module/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/include
INC_ESPCOMP += -I$(ESPCOMP)/wpa_supplicant/port/include
INC_ESPCOMP += -I$(ESPCOMP)/ethernet/include
INC_ESPCOMP += -I$(ESPCOMP)/app_trace/include
INC_ESPCOMP += -I$(ESPCOMP)/app_update/include

INC += $(INC_ESPCOMP)

ESPIDF_DRIVER_O = $(addprefix $(SDK_BIN)/driver/,\
	uart.o \
	periph_ctrl.o \
	ledc.o \
	gpio.o \
	timer.o \
	spi_master.o \
	spi_common.o \
	rtc_module.o \
	)

$(BUILD)/$(SDK_BIN)/esp32/dport_access.o: CFLAGS += -Wno-array-bounds
ESPIDF_ESP32_O = $(addprefix $(SDK_BIN)/esp32/,\
	brownout.o \
	panic.o \
	esp_timer.o \
	esp_timer_esp32.o \
	ets_timer_legacy.o \
	event_default_handlers.o \
	fast_crypto_ops.o \
	task_wdt.o \
	cache_err_int.o \
	clk.o \
	core_dump.o \
	cpu_start.o \
	gdbstub.o \
	crosscore_int.o \
	ipc.o \
	int_wdt.o \
	event_loop.o \
	hwcrypto/sha.o \
	hwcrypto/aes.o \
	lib_printf.o \
	freertos_hooks.o \
	system_api.o \
	hw_random.o \
	phy_init.o \
	intr_alloc.o \
	dport_access.o \
	wifi_init.o \
	wifi_internal.o \
	sleep_modes.o \
	)

ESPIDF_HEAP_O = $(addprefix $(SDK_BIN)/heap/,\
	heap_caps.o \
	heap_caps_init.o \
	multi_heap.o \
	)

ESPIDF_SOC_O = $(addprefix $(SDK_BIN)/soc/,\
	esp32/cpu_util.o \
	esp32/rtc_clk.o \
	esp32/rtc_init.o \
	esp32/rtc_pm.o \
	esp32/rtc_sleep.o \
	esp32/rtc_time.o \
	esp32/soc_memory_layout.o \
	)

ESPIDF_CXX_O = $(addprefix $(SDK_BIN)/cxx/,\
	cxx_guards.o \
	)

ESPIDF_ETHERNET_O = $(addprefix $(SDK_BIN)/ethernet/,\
	emac_dev.o \
	emac_main.o \
	eth_phy/phy_tlk110.o \
	eth_phy/phy_lan8720.o \
	eth_phy/phy_common.o \
	)

$(BUILD)/$(SDK_BIN)/expat/%.o: CFLAGS += -Wno-unused-function
ESPIDF_EXPAT_O = $(addprefix $(SDK_BIN)/expat/,\
	library/xmltok_ns.o \
	library/xmltok.o \
	library/xmlparse.o \
	library/xmlrole.o \
	library/xmltok_impl.o \
	port/minicheck.o \
	port/expat_element.o \
	port/chardata.o \
	)

ESPIDF_PTHREAD_O = $(addprefix $(SDK_BIN)/pthread/,\
	pthread.o \
	pthread_local_storage.o \
	)

# Assembler .S files need only basic flags, and in particular should not have
# -Os because that generates subtly different code.
# We also need custom CFLAGS for .c files because FreeRTOS has headers with
# generic names (eg queue.h) which can clash with other files in the port.
CFLAGS_ASM = -I$(ESPCOMP)/esp32/include -I$(ESPCOMP)/soc/esp32/include -I$(ESPCOMP)/freertos/include/freertos -I. -Iports/$(PORT)
$(BUILD)/$(SDK_BIN)/freertos/portasm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(SDK_BIN)/freertos/xtensa_context.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(SDK_BIN)/freertos/xtensa_intr_asm.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(SDK_BIN)/freertos/xtensa_vectors.o: CFLAGS = $(CFLAGS_ASM)
$(BUILD)/$(SDK_BIN)/freertos/%.o: CFLAGS += $(INC_ESPCOMP) -I$(ESPCOMP)/freertos/include/freertos
ESPIDF_FREERTOS_O = $(addprefix $(SDK_BIN)/freertos/,\
	croutine.o \
	event_groups.o \
	FreeRTOS-openocd.o \
	list.o \
	portasm.o \
	port.o \
	queue.o \
	ringbuf.o \
	tasks.o \
	timers.o \
	xtensa_context.o \
	xtensa_init.o \
	xtensa_intr_asm.o \
	xtensa_intr.o \
	xtensa_overlay_os_hook.o \
	xtensa_vector_defaults.o \
	xtensa_vectors.o \
	)

ESPIDF_VFS_O = $(addprefix $(SDK_BIN)/vfs/,\
	vfs_uart.o \
	vfs.o \
	)

ESPIDF_JSON_O = $(addprefix $(SDK_BIN)/json/cJSON/,\
	cJSON.o \
	cJSON_Utils.o \
	)

ESPIDF_LOG_O = $(addprefix $(SDK_BIN)/log/,\
	log.o \
	)

ESPIDF_XTENSA_DEBUG_MODULE_O = $(addprefix $(SDK_BIN)/xtensa-debug-module/,\
	eri.o \
	trax.o \
	)

ESPIDF_TCPIP_ADAPTER_O = $(addprefix $(SDK_BIN)/tcpip_adapter/,\
	tcpip_adapter_lwip.o \
	)

ESPIDF_APP_TRACE_O = $(addprefix $(SDK_BIN)/app_trace/,\
	app_trace.o \
	)

ESPIDF_APP_UPDATE_O = $(addprefix $(SDK_BIN)/app_update/,\
	esp_ota_ops.o \
	)

ESPIDF_NEWLIB_O = $(addprefix $(SDK_BIN)/newlib/,\
	time.o \
	syscalls.o \
	syscall_table.o \
	reent_init.o \
	locks.o \
	)

ESPIDF_NVS_FLASH_O = $(addprefix $(SDK_BIN)/nvs_flash/,\
	src/nvs_types.o \
	src/nvs_page.o \
	src/nvs_item_hash_list.o \
	src/nvs_pagemanager.o \
	src/nvs_storage.o \
	src/nvs_api.o \
	)

ESPIDF_OPENSSL_O = $(addprefix $(SDK_BIN)/openssl/,\
	)

ESPIDF_SPI_FLASH_O = $(addprefix $(SDK_BIN)/spi_flash/,\
	flash_mmap.o \
	partition.o \
	spi_flash_rom_patch.o \
	cache_utils.o \
	flash_ops.o \
	)

$(BUILD)/$(SDK_BIN)/lwip/%.o: CFLAGS += -Wno-address -Wno-unused-variable -Wno-unused-but-set-variable
ESPIDF_LWIP_O = $(addprefix $(SDK_BIN)/lwip/,\
	api/pppapi.o \
	api/netbuf.o \
	api/api_lib.o \
	api/netifapi.o \
	api/tcpip.o \
	api/netdb.o \
	api/err.o \
	api/api_msg.o \
	api/sockets.o \
	apps/sntp/sntp.o \
	apps/dhcpserver.o \
	core/ipv4/ip_frag.o \
	core/ipv4/dhcp.o \
	core/ipv4/ip4_addr.o \
	core/ipv4/igmp.o \
	core/ipv4/ip4.o \
	core/ipv4/autoip.o \
	core/ipv4/icmp.o \
	core/ipv6/ip6_frag.o \
	core/ipv6/dhcp6.o \
	core/ipv6/inet6.o \
	core/ipv6/ip6_addr.o \
	core/ipv6/ip6.o \
	core/ipv6/nd6.o \
	core/ipv6/mld6.o \
	core/ipv6/ethip6.o \
	core/ipv6/icmp6.o \
	core/mem.o \
	core/init.o \
	core/memp.o \
	core/sys.o \
	core/tcp_in.o \
	core/dns.o \
	core/ip.o \
	core/pbuf.o \
	core/raw.o \
	core/tcp.o \
	core/def.o \
	core/netif.o \
	core/stats.o \
	core/timers.o \
	core/inet_chksum.o \
	core/udp.o \
	core/tcp_out.o \
	netif/slipif.o \
	netif/etharp.o \
	netif/ethernet.o \
	netif/lowpan6.o \
	netif/ethernetif.o \
	port/freertos/sys_arch.o \
	port/netif/wlanif.o \
	port/netif/ethernetif.o \
	port/vfs_lwip.o \
	)

ESPIDF_MBEDTLS_O = $(addprefix $(SDK_BIN)/mbedtls/,\
	library/entropy.o \
	library/pkcs12.o \
	library/ccm.o \
	library/pk.o \
	library/sha1.o \
	library/x509_csr.o \
	library/ssl_cli.o \
	library/ecp.o \
	library/blowfish.o \
	library/x509.o \
	library/ecp_curves.o \
	library/error.o \
	library/ssl_ticket.o \
	library/entropy_poll.o \
	library/cipher.o \
	library/version_features.o \
	library/ripemd160.o \
	library/rsa.o \
	library/md.o \
	library/md_wrap.o \
	library/sha256.o \
	library/dhm.o \
	library/ssl_cache.o \
	library/pkwrite.o \
	library/base64.o \
	library/asn1parse.o \
	library/ssl_tls.o \
	library/hmac_drbg.o \
	library/pem.o \
	library/version.o \
	library/gcm.o \
	library/memory_buffer_alloc.o \
	library/md2.o \
	library/ecdsa.o \
	library/ssl_srv.o \
	library/x509_crt.o \
	library/ecdh.o \
	library/asn1write.o \
	library/md4.o \
	library/debug.o \
	library/x509_create.o \
	library/ecjpake.o \
	library/oid.o \
	library/md5.o \
	library/ssl_ciphersuites.o \
	library/sha512.o \
	library/xtea.o \
	library/aes.o \
	library/cipher_wrap.o \
	library/arc4.o \
	library/bignum.o \
	library/pkparse.o \
	library/padlock.o \
	library/threading.o \
	library/x509_crl.o \
	library/pkcs11.o \
	library/aesni.o \
	library/timing.o \
	library/certs.o \
	library/pkcs5.o \
	library/ssl_cookie.o \
	library/camellia.o \
	library/havege.o \
	library/des.o \
	library/x509write_csr.o \
	library/platform.o \
	library/ctr_drbg.o \
	library/x509write_crt.o \
	library/pk_wrap.o \
	port/esp_bignum.o \
	port/esp_hardware.o \
	port/esp_sha1.o \
	port/esp_sha256.o \
	port/esp_sha512.o \
	)

$(BUILD)/$(SDK_BIN)/wpa_supplicant/%.o: CFLAGS += -DEMBEDDED_SUPP -D__ets__ -Wno-strict-aliasing
ESPIDF_WPA_SUPPLICANT_O = $(addprefix $(SDK_BIN)/wpa_supplicant/,\
	src/crypto/aes-internal-enc.o \
	src/crypto/sha256-internal.o \
	src/crypto/md5-internal.o \
	src/crypto/aes-internal.o \
	src/crypto/sha1.o \
	src/crypto/aes-internal-dec.o \
	src/crypto/aes-unwrap.o \
	src/crypto/crypto_internal-rsa.o \
	src/crypto/dh_groups.o \
	src/crypto/crypto_internal.o \
	src/crypto/aes-wrap.o \
	src/crypto/sha1-internal.o \
	src/crypto/dh_group5.o \
	src/crypto/sha256.o \
	src/crypto/rc4.o \
	src/crypto/md5.o \
	src/crypto/aes-cbc.o \
	src/crypto/sha1-pbkdf2.o \
	src/crypto/bignum.o \
	src/crypto/crypto_internal-modexp.o \
	src/crypto/crypto_internal-cipher.o \
	src/fast_crypto/fast_aes-unwrap.o \
	src/fast_crypto/fast_aes-wrap.o \
	src/fast_crypto/fast_sha256.o \
	src/fast_crypto/fast_sha256-internal.o \
	port/os_xtensa.o \
	)

OBJ_ESPIDF =
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NEWLIB_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_DRIVER_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ESP32_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_HEAP_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SOC_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_CXX_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_ETHERNET_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_EXPAT_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_PTHREAD_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_FREERTOS_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_VFS_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_JSON_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_LOG_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_LWIP_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_MBEDTLS_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_XTENSA_DEBUG_MODULE_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_TCPIP_ADAPTER_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_APP_TRACE_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_APP_UPDATE_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_NVS_FLASH_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_OPENSSL_O))
OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_SPI_FLASH_O))
#OBJ_ESPIDF += $(addprefix $(BUILD)/, $(ESPIDF_WPA_SUPPLICANT_O))

OBJ += $(OBJ_ESPIDF)

IDF_VER := $(shell cd ${IDF_PATH} && git describe --always --tags --dirty)
CFLAGS += -DIDF_VER='"$(IDF_VER)"'
CFLAGS += -DMBEDTLS_CONFIG_FILE='"mbedtls/esp_config.h"'
FLAGS += -DESP_PLATFORM
FLAGS += -mlongcalls

include ../common/rules.mk
