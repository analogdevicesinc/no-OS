# nios-ii

ifeq ($(SOPCINFO-FILE),)
  SOPCINFO-FILE := $(M_SOPCINFO_FILE)
endif

PRJ_DIR := $(dir $(SOPCINFO-FILE))
M_SOF_FILE := $(PRJ_DIR)/$(M_SOF_FILE)
ifeq ($(SOF-FILE),)
  SOF-FILE := $(M_SOF_FILE)
endif

APP_LOG := nios_ii_app.log
BSP_LOG := nios_ii_bsp.log
ELF_FILE := sw.elf
CPU_NAME := sys_cpu

HDR_FILES := $(M_HDR_FILES)
HDR_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.h))

SRC_FILES := $(M_SRC_FILES)
SRC_FILES += $(foreach i_dir, $(M_INC_DIRS), $(wildcard $(i_dir)/*.c))

BSP_CMD := nios2-bsp hal bsp
BSP_CMD += $(SOPCINFO-FILE)
BSP_CMD += --cpu-name $(CPU_NAME)

APP_CMD := nios2-app-generate-makefile
APP_CMD += --bsp-dir bsp
APP_CMD += --app-dir sw
APP_CMD += $(addprefix --inc-dir , $(M_INC_DIRS))
APP_CMD += --inc-dir bsp/HAL/inc/sys
APP_CMD += --elf-name $(ELF_FILE)
APP_CMD += --set OBJDUMP_INCLUDE_SOURCE 1
APP_CMD += --set APP_CFLAGS_DEFINED_SYMBOLS -DALTERA -DNIOS_II
APP_CMD += --set APP_CFLAGS_USER_FLAGS -Wno-format
APP_CMD += --src-files $(SRC_FILES)

.PHONY: all
all: sw/$(ELF_FILE)

sw/$(ELF_FILE): bsp/system.h sw/Makefile hw/system_top.sof $(SRC_FILES) $(HDR_FILES)
	make -C sw


sw/Makefile: $(SOPCINFO-FILE)
	rm -fr sw
	$(APP_CMD) > $(APP_LOG) 2>&1
	mv $(APP_LOG) sw/


bsp/system.h: $(SOPCINFO-FILE)
	rm -fr bsp
	$(BSP_CMD) > $(BSP_LOG) 2>&1
	mv $(BSP_LOG) bsp/


hw/system_top.sof: $(SOF-FILE) $(SOPCINFO-FILE)
	rm -fr hw
	mkdir hw
	cp $(SOF-FILE) hw/system_top.sof
	cp $(SOPCINFO-FILE) hw/


.PHONY: clean
clean:
	rm -fr bsp
	rm -fr sw


.PHONY: clean-all
clean-all:
	rm -fr bsp
	rm -fr sw
	rm -fr hw


.PHONY: run
run: sw/$(ELF_FILE)
	nios2-configure-sof hw/system_top.sof
	nios2-download -g sw/sw.elf
	nios2-terminal

