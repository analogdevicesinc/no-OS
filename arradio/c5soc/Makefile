## Makefile

include ../../scripts/noos.mk

M_SOPCINFO_FILE := $(HDL-DIR)/projects/arradio/c5soc/system_bd.sopcinfo
M_SOPCINFO_DIR := $(HDL-DIR)/projects/arradio/c5soc/hps_isw_handoff/system_bd_sys_hps
M_SOF_FILE := $(HDL-DIR)/projects/arradio/c5soc/arradio_c5soc.sof

M_SRC_DIRS := $(NOOS-DIR)/ad9361/sw
M_SRC_DIRS += $(NOOS-DIR)/ad9361/sw/platform_xilinx

M_DEFINES := CONFIG_H_
M_DEFINES += HAVE_VERBOSE_MESSAGES
M_DEFINES += HAVE_SPLIT_GAIN_TABLE=1
M_DEFINES += HAVE_TDD_SYNTH_TABLE=1
M_DEFINES += AD9361_DEVICE=1
M_DEFINES += AD9364_DEVICE=0
M_DEFINES += AD9363A_DEVICE=0
M_DEFINES += XILINX_PLATFORM
M_DEFINES += PICOZED_SDR

SYS_UBOOT_IMAGE := bsp/arradio_c5soc_uboot.bin
SYS_FPGA_IMAGE := hw/arradio_c5soc.rbf

SRC_FILES := $(M_SRC_FILES)
SRC_FILES += $(foreach i_dir, $(M_SRC_DIRS), $(wildcard $(i_dir)/*.c))
HPS_FLAGS := $(addprefix -D, $(M_DEFINES))
HPS_FLAGS += $(addprefix -I, $(M_INC_DIRS))
HPS_FLAGS += $(addprefix -I, $(M_SRC_DIRS))


.PHONY: all
all: $(SYS_UBOOT_IMAGE) $(SYS_FPGA_IMAGE)


.PHONY: clean
clean:
	rm -fr sw hw bsp


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



