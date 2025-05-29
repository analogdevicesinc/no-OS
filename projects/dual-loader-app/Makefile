PLATFORM=maxim
TARGET=max32690

# Handle BLINKING parameter (default to normal)
BLINKING ?= normal

# Add BLINKING_SPEED define based on parameter
ifeq ($(BLINKING),fast)
    CFLAGS += -DBLINKING_FAST
else ifeq ($(BLINKING),slow)
    CFLAGS += -DBLINKING_SLOW
else
    CFLAGS += -DBLINKING_NORMAL
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ARTIFACT=$(basename $(BINARY))-$(basename $(notdir $(LSCRIPT)))

bin: all
	cp $(BINARY) $(ARTIFACT).elf
	$(OC) $(ARTIFACT).elf -O binary -R .app_footer $(ARTIFACT).bin
	echo ffffffffffffffff | xxd -r -p > $(BUILD_DIR)/Footer.bin
	crc=$$(crc32 $(ARTIFACT).bin); echo $${crc:6:2}$${crc:4:2}$${crc:2:2}$${crc:0:2} | xxd -r -p >> $(BUILD_DIR)/Footer.bin
	echo crc $$(crc32 $(ARTIFACT).bin)
	len=$$(stat --format=%s $(ARTIFACT).bin | awk '{printf "%08x", $$1}'); echo $${len:6:2}$${len:4:2}$${len:2:2}$${len:0:2} | xxd -r -p >> ${BUILD_DIR}/Footer.bin
	echo len $$(stat --format=%s $(ARTIFACT).bin)
	$(OC) --update-section .app_footer=${BUILD_DIR}/Footer.bin $(BINARY)
	$(OC) --update-section .app_footer=${BUILD_DIR}/Footer.bin $(ARTIFACT).elf
	$(OC) $(ARTIFACT).elf -O binary --gap-fill 0xFF $(ARTIFACT).bin
