## Makefile

include ../../scripts/noos.mk

M_SOPCINFO_FILE := $(HDL-DIR)/projects/arradio/c5soc/system_bd.sopcinfo
M_SOPCINFO_DIR := $(HDL-DIR)/projects/arradio/c5soc/hps_isw_handoff/system_bd_sys_hps
M_SOF_FILE := $(HDL-DIR)/projects/arradio/c5soc/arradio_c5soc.sof
M_SRC_FILES := $(NOOS-DIR)/arradio/c5soc/alt_retarget_gnu.c

M_SRC_DIRS := $(NOOS-DIR)/ad9361/sw
M_SRC_DIRS += $(NOOS-DIR)/ad9361/sw/platform_altera

M_DEFINES := CONFIG_H_
M_DEFINES += HAVE_VERBOSE_MESSAGES
M_DEFINES += HAVE_SPLIT_GAIN_TABLE=1
M_DEFINES += HAVE_TDD_SYNTH_TABLE=1
M_DEFINES += AD9361_DEVICE=1
M_DEFINES += AD9364_DEVICE=0
M_DEFINES += AD9363A_DEVICE=0

SYS_UBOOT_IMAGE := bsp/arradio_c5soc_uboot.bin
SYS_FPGA_IMAGE := hw/arradio_c5soc.rbf
SYS_APP_IMAGE := sw/arradio_c5soc.bin
SYS_APP_SCRIPT := u-boot.scr

SRC_FILES := $(M_SRC_FILES)
SRC_FILES += $(foreach i_dir, $(M_SRC_DIRS), $(wildcard $(i_dir)/*.c))
HPS_FLAGS := $(addprefix -D, $(M_DEFINES))
HPS_FLAGS += $(addprefix -I, $(M_INC_DIRS))
HPS_FLAGS += $(addprefix -I, $(M_SRC_DIRS))


.PHONY: all
all: $(SYS_UBOOT_IMAGE) $(SYS_FPGA_IMAGE) $(SYS_APP_IMAGE) $(SYS_APP_SCRIPT)
ifneq ($(sd-dir),)
	rm -fr $(sd-dir)/arradio_c5soc.bin
	rm -fr $(sd-dir)/arradio_c5soc.rbf
	rm -fr $(sd-dir)/u-boot.scr
	cp $(SYS_FPGA_IMAGE) $(sd-dir)/
	cp $(SYS_APP_IMAGE) $(sd-dir)/
	cp $(SYS_APP_SCRIPT) $(sd-dir)/
endif


.PHONY: clean
clean:
	rm -fr sw hw bsp mkdefines.log


$(SYS_UBOOT_IMAGE): $(M_SOPCINFO_DIR) $(M_SOPCINFO_FILE)
	rm -fr bsp
	mkdir -p bsp
	bsp-create-settings --bsp-dir bsp --type spl --preloader-settings-dir $(M_SOPCINFO_DIR) \
		--settings bsp/arradio_c5soc.bsp --set spl.boot.WATCHDOG_ENABLE false
	make -C bsp
	make -C bsp uboot
	cat bsp/preloader-mkpimage.bin bsp/uboot-socfpga/u-boot.img > $(SYS_UBOOT_IMAGE)
	rm -fr sw
	mkdir -p sw
	sopc-create-header-files $(M_SOPCINFO_FILE) --output-dir sw


$(SYS_FPGA_IMAGE): $(M_SOF_FILE)
	rm -fr hw
	mkdir -p hw
	quartus_cpf -c -o bitstream_compression=off $(M_SOF_FILE) $(SYS_FPGA_IMAGE)


$(SYS_APP_IMAGE): $(SRC_FILES)
	arm-altera-eabi-gcc -g -O0 -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon \
		-Wall -Werror -std=c99 -o sw/arradio_c5soc.axf -DHPS -Dsoc_cv_av \
		-T$(SOCEDS_DEST_ROOT)/host_tools/mentor/gnu/arm/baremetal/arm-altera-eabi/lib/cycloneV-dk-ram.ld \
		-I$(SOCEDS_DEST_ROOT)/ip/altera/hps/altera_hps/hwlib/include \
		-I$(SOCEDS_DEST_ROOT)/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av \
		-I$(SOCEDS_DEST_ROOT)/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av/socal \
		-Isw -Ibsp/generated $(HPS_FLAGS) $(SRC_FILES)
	arm-altera-eabi-objcopy -O binary sw/arradio_c5soc.axf $(SYS_APP_IMAGE)


$(SYS_APP_SCRIPT): $(SYS_UBOOT_IMAGE) $(SYS_FPGA_IMAGE)
	$(file >u-boot.scr.txt,fatload mmc 0:1 $$fpgadata arradio_c5soc.rbf)
	$(file >>u-boot.scr.txt,fpga load 0 $$fpgadata $$filesize)
	$(file >>u-boot.scr.txt,run bridge_enable_handoff)
	$(file >>u-boot.scr.txt,fatload mmc 0:1 0x100000 arradio_c5soc.bin)
	$(file >>u-boot.scr.txt,go 0x100000)
	mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "U-Boot Script" -d u-boot.scr.txt $(SYS_APP_SCRIPT)


