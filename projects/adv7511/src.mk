################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

# Uncomment to select the profile

SRCS += $(PROJECT)/src/main.c \
	$(PROJECT)/src/cf_hdmi.c \
	$(PROJECT)/src/edid.c \
	$(PROJECT)/src/transmitter.c \
	$(PROJECT)/src/wrapper.c
SRCS += $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
SRCS +=	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(PLATFORM_DRIVERS)/xilinx_i2c.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_timer.c
SRCS +=$(PROJECT)/TX/HAL/COMMON/tx_hal.c \
	$(PROJECT)/TX/HAL/WIRED/wrd_hal.c \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/7511_hal.c \
	$(PROJECT)/TX/LIB/tx_cec.c \
	$(PROJECT)/TX/LIB/tx_isr.c \
	$(PROJECT)/TX/LIB/tx_lib.c \
	$(PROJECT)/TX/LIB/tx_multi.c
INCS +=	$(PROJECT)/src/app_config.h \
	$(PROJECT)/src/cf_hdmi.h \
	$(PROJECT)/src/cf_hdmi_demo.h \
	$(PROJECT)/src/edid.h \
	$(PROJECT)/src/transmitter.h \
	$(PROJECT)/src/transmitter_defs.h \
	$(PROJECT)/src/wrapper.h
INCS += $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/xilinx_i2c.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/xilinx_timer.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h
INCS +=	$(PROJECT)/TX/tx_lib.h \
	$(PROJECT)/TX/HAL/COMMON/tx_cfg.h \
	$(PROJECT)/TX/HAL/COMMON/tx_hal.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/7511_cfg.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/7511_hal.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_cec_map_adr.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_cec_map_def.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_cec_map_fct.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_cfg.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_edid_map_adr.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_edid_map_def.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_edid_map_fct.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_lib.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_main_map_adr.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_main_map_def.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_main_map_fct.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_packet_map_adr.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_packet_map_def.h \
	$(PROJECT)/TX/HAL/WIRED/ADV7511/MACROS/ADV7511_packet_map_fct.h \
	$(PROJECT)/TX/LIB/tx_isr.h \
	$(PROJECT)/TX/LIB/tx_multi.h
