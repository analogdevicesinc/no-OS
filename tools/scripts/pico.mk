ifeq ($(PICO_SDK_PATH),)
$(error $(ENDL)PICO_SDK_PATH not defined\
		$(ENDL)Please run command "export PICO_SDK_PATH=/path/to/your/PICO_SDK_PATH"$(ENDL))
endif # PICO_SDK_PATH check

ifeq ($(JLINK_SERVER_PATH),)
$(warning $(ENDL)JLINK_SERVER_PATH not defined\
		$(ENDL)Please run command "export JLINK_SERVER_PATH=/path/to/your/JLINK_SERVER_PATH"$(ENDL)\
		$(ENDL)Example "export JLINK_SERVER_PATH=/opt/SEGGER/JLink/JLinkGDBServerCLExe"$(ENDL))
endif # JLINK_SERVER_PATH check

#----------------------------------------------------------------------
# Tools & Helpers
#----------------------------------------------------------------------
CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-gcc
GDB=gdb-multiarch
OC=arm-none-eabi-objcopy	
SIZE=arm-none-eabi-size

# ELF2UF2 is used to convert elf binary in uf2 format
# For flashing UF2 file can be dragged onto USB Mass Storage Device 
ELF2UF2  :=  $(PICO_SDK_PATH)/tools/elf2uf2/build/elf2uf2
ELF2UF2_CMAKE_DIR  :=  $(PICO_SDK_PATH)/tools/elf2uf2/
ELF2UF2_BUILD  :=  $(shell (if [ -f $(ELF2UF2) ] ; then echo "ELF2UF2 already built" ; else echo "ELF2UF2 not built, proceed building" ; cd $(ELF2UF2_CMAKE_DIR) ; mkdir build ; cd build ; cmake .. ; make ; fi ;))

# Generated hex file
HEX = $(basename $(BINARY)).hex
# Generated uf2 file
UF2 = $(basename $(BINARY)).uf2

#----------------------------------------------------------------------
# Project Structure
#----------------------------------------------------------------------
TARGET = rp2040
PROJECT_BUILD = $(BUILD_DIR)/app

LSCRIPT = $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/memmap_default.ld

#----------------------------------------------------------------------
# Platform Source Files
#----------------------------------------------------------------------
# These are the platform c files that have to be compiled
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_sync/critical_section.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_sync/lock_core.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_sync/mutex.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_sync/sem.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_time/time.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_time/timeout_helper.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_util/datetime.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_util/pheap.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/common/pico_util/queue.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_claim/claim.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_clocks/clocks.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_gpio/gpio.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_i2c/i2c.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_irq/irq.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_pll/pll.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_spi/spi.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_sync/sync.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_timer/timer.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_uart/uart.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_vreg/vreg.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_watchdog/watchdog.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_xosc/xosc.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_bootrom/bootrom.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_double/double_math.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_float/float_init_rom.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_float/float_math.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_malloc/pico_malloc.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_platform/platform.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_printf/printf.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_runtime/runtime.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/binary_info.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_stdio/stdio.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_stdio_uart/stdio_uart.c
PLATFORM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_stdlib/stdlib.c

#----------------------------------------------------------------------
# Assembly Source Files
#----------------------------------------------------------------------
# These are the platform assembly files that have to be compiled
ASM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/hardware_irq/irq_handler_chain.S
ASM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_double/double_v1_rom_shim.S
ASM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_float/float_aeabi.S
ASM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_float/float_v1_rom_shim.S
ASM_SRCS += $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/crt0.S

#----------------------------------------------------------------------
# Platform Include Paths
#----------------------------------------------------------------------
# These are the platform include paths that contain needed header files

PLATFORM_PICO_INCS_PATH += $(NO-OS)/drivers/platform/pico/
PLATFORM_INCS_PATH += $(PICO_SDK_PATH)/src/boards/include

PLATFORM_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/boot_stage2/include
PLATFORM_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/cmsis/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_base/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_claim/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_clocks/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_divider/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_gpio/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_i2c/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_irq/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_pll/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_resets/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_spi/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_sync/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_timer/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_uart/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_vreg/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_watchdog/include
PLATFORM_HARDWARE_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/hardware_xosc/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_bootrom/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_double/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_float/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_int64_ops/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_malloc/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_platform/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_printf/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_runtime/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_stdio/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/rp2_common/pico_stdio_uart/include
PLATFORM_INCS_PATH += $(PICO_SDK_PATH)/src/rp2040/hardware_regs/include
PLATFORM_INCS_PATH += $(PICO_SDK_PATH)/src/rp2040/hardware_structs/include

PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_base/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_binary_info/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_bit_ops/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_divider/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_stdlib/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_sync/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_time/include
PLATFORM_PICO_INCS_PATH += $(PICO_SDK_PATH)/src/common/pico_util/include


PLATFORM_INCS_PATH += $(PLATFORM_PICO_INCS_PATH)
PLATFORM_INCS_PATH += $(PLATFORM_HARDWARE_INCS_PATH)
#----------------------------------------------------------------------
# Platform Include Flags
#----------------------------------------------------------------------
PLATFORM_INCS += $(addprefix -I,$(PLATFORM_INCS_PATH))
ASM_INCLUDE += $(PLATFORM_INCS)

#----------------------------------------------------------------------
# Platform Directories needed for source links
#----------------------------------------------------------------------
PLATFORM_DIRS += $(sort $(foreach dir, $(PLATFORM_SRCS),$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PLATFORM_INCS_PATH),$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PLATFORM_HARDWARE_INCS_PATH),$(addsuffix hardware/, $(dir)/)))
PLATFORM_DIRS += $(sort $(foreach dir, $(PLATFORM_PICO_INCS_PATH),$(addsuffix pico/, $(dir)/)))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/boards/include/boards/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/common/pico_binary_info/include/pico/binary_info/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/common//pico_util/include/pico/util/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2_common/pico_stdio/include/pico/stdio/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2_common/boot_stage2/include/boot_stage2/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2_common/cmsis/include/cmsis/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2_common/pico_bootrom/include/pico/bootrom/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2040/hardware_structs/include/hardware/structs/,$(dir $(dir))))
PLATFORM_DIRS += $(sort $(foreach dir, $(PICO_SDK_PATH)/src/rp2040/hardware_regs/include/hardware/regs/,$(dir $(dir))))

#----------------------------------------------------------------------
# Platform header files needed for source links
#----------------------------------------------------------------------
PLATFORM_INCLUDE_FILES += $(foreach dir, $(PLATFORM_INCS_PATH), $(call rwildcard, $(dir),*.h))
PLATFORM_INCLUDE_FILES += $(foreach dir, $(PLATFORM_INCS_PATH), $(call rwildcard, $(dir)/pico,*.h))

#----------------------------------------------------------------------
# Defined Symbols
#----------------------------------------------------------------------
DEFINES += -DLIB_PICO_BIT_OPS=1
DEFINES += -DLIB_PICO_BIT_OPS_PICO=1
DEFINES += -DLIB_PICO_DIVIDER=1
DEFINES += -DLIB_PICO_DIVIDER_HARDWARE=1
DEFINES += -DLIB_PICO_DOUBLE=1
DEFINES += -DLIB_PICO_DOUBLE_PICO=1
DEFINES += -DLIB_PICO_FLOAT=1
DEFINES += -DLIB_PICO_FLOAT_PICO=1
DEFINES += -DLIB_PICO_INT64_OPS=1
DEFINES += -DLIB_PICO_INT64_OPS_PICO=1
DEFINES += -DLIB_PICO_MALLOC=1
DEFINES += -DLIB_PICO_MEM_OPS=1
DEFINES += -DLIB_PICO_MEM_OPS_PICO=1
DEFINES += -DLIB_PICO_PLATFORM=1
DEFINES += -DLIB_PICO_PRINTF=1
DEFINES += -DLIB_PICO_PRINTF_PICO=1
DEFINES += -DLIB_PICO_RUNTIME=1
DEFINES += -DLIB_PICO_STANDARD_LINK=1
DEFINES += -DLIB_PICO_STDIO=1
DEFINES += -DLIB_PICO_STDIO_UART=1
DEFINES += -DLIB_PICO_STDLIB=1
DEFINES += -DLIB_PICO_SYNC=1
DEFINES += -DLIB_PICO_SYNC_CORE=1
DEFINES += -DLIB_PICO_SYNC_CRITICAL_SECTION=1
DEFINES += -DLIB_PICO_SYNC_MUTEX=1
DEFINES += -DLIB_PICO_SYNC_SEM=1
DEFINES += -DLIB_PICO_TIME=1
DEFINES += -DLIB_PICO_UTIL=1
DEFINES += -DPICO_BOARD=\"pico\"
DEFINES += -DPICO_BUILD=1
DEFINES += -DPICO_CMAKE_BUILD_TYPE=\"Debug\"
DEFINES += -DPICO_COPY_TO_RAM=0
DEFINES += -DPICO_CXX_ENABLE_EXCEPTIONS=0
DEFINES += -DPICO_NO_FLASH=0
DEFINES += -DPICO_NO_HARDWARE=0
DEFINES += -DPICO_ON_DEVICE=1
DEFINES += -DPICO_USE_BLOCKED_RAM=0

DEFINES += -DDEBUG
DEFINES += -DPICO_HEAP_SIZE=0x1000
DEFINES += -DCFG_TUSB_MCU=OPT_MCU_RP2040
DEFINES += -DENABLE_LOGGING
DEFINES += -DPICO_PLATFORM
DEFINES += -DPICO_DEFAULT_IRQ_PRIORITY=0

#----------------------------------------------------------------------
# Bootloader specific
#----------------------------------------------------------------------
BOOT_STAGE2 = $(PICO_SDK_PATH)/src/rp2_common/boot_stage2
BOOTSRCS := $(BOOT_STAGE2)/compile_time_choice.S
BOOTELF := $(subst :,__COLON__, $(addprefix $(PROJECT_BUILD)/, $(BOOT_STAGE2)/boot.elf))
BOOTBIN := $(subst :,__COLON__, $(addprefix $(PROJECT_BUILD)/, $(BOOT_STAGE2)/boot.bin))
BOOTLD := $(BOOT_STAGE2)/boot_stage2.ld

BOOTSRC := $(subst :,__COLON__, $(addprefix $(PROJECT_BUILD)/, $(BOOT_STAGE2)/boot.S))
BOOTOBJ := $(addsuffix .o, $(BOOTSRC))

BOOT_DEFINES := -DPICO_BOARD=\"pico\"
BOOT_DEFINES += -DPICO_BUILD=1
BOOT_DEFINES += -DPICO_NO_HARDWARE=0
BOOT_DEFINES += -DPICO_ON_DEVICE=1

BOOT_INCLUDE := -I$(NO-OS)/drivers/platform/pico/
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2_common/cmsis/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2_common/boot_stage2/asminclude
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2040/hardware_regs/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2_common/hardware_base/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/common/pico_base/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/boards/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2_common/pico_platform/include
BOOT_INCLUDE += -I$(PICO_SDK_PATH)/src/rp2_common/boot_stage2/include

BOOT_ASFLAGS := $(BOOT_INCLUDE) $(BOOT_DEFINES)

BOOT_CFLAGS := -mcpu=cortex-m0plus
BOOT_CFLAGS += -mthumb
BOOT_CFLAGS += -Wl,--build-id=none
BOOT_CFLAGS += --specs=nosys.specs
BOOT_CFLAGS += -nostartfiles
BOOT_CFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT_NAME)_bs2_default.elf.map
BOOT_CFLAGS += $(BOOT_INCLUDE)

DEPFLAGS += -MT$@
DEPFLAGS += -MMD
DEPFLAGS += -MP
DEPFLAGS += -MF$@.d

COMPILE.s=$(AS) $(BOOT_ASFLAGS) $(DEPFLAGS) -c $< -o $@

#----------------------------------------------------------------------
# Compiler & Linker Flags
#----------------------------------------------------------------------
CFLAGS += -mcpu=cortex-m0plus
CFLAGS += -mthumb
CFLAGS += -O0
CFLAGS += -g3
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -std=gnu11
CFLAGS += $(DEFINES)

ASFLAGS += $(ASM_INCLUDE)
ASFLAGS += -mcpu=cortex-m0plus
ASFLAGS += -mthumb
ASFLAGS += -O0
ASFLAGS += -g3
ASFLAGS += $(DEFINES)
ASFLAGS += -xassembler-with-cpp

LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(PROJECT_NAME).map
LDFLAGS += -mcpu=cortex-m0plus
LDFLAGS += -mthumb
LDFLAGS += -O0
LDFLAGS += -DDEBUG
LDFLAGS += -Wl,--build-id=none
LDFLAGS += --specs=nosys.specs
LDFLAGS += -Wl,--wrap=sprintf
LDFLAGS += -Wl,--wrap=snprintf
LDFLAGS += -Wl,--wrap=vsnprintf
LDFLAGS += -Wl,--wrap=__ctzsi2
LDFLAGS += -Wl,--wrap=__ctzdi2
LDFLAGS += -Wl,--wrap=__popcountsi2
LDFLAGS += -Wl,--wrap=__popcountdi2
LDFLAGS += -Wl,--wrap=__clz
LDFLAGS += -Wl,--wrap=__clzl
LDFLAGS += -Wl,--wrap=__clzll
LDFLAGS += -Wl,--wrap=sqrt
LDFLAGS += -Wl,--wrap=cos
LDFLAGS += -Wl,--wrap=sin
LDFLAGS += -Wl,--wrap=tan
LDFLAGS += -Wl,--wrap=atan2
LDFLAGS += -Wl,--wrap=exp
LDFLAGS += -Wl,--wrap=log
LDFLAGS += -Wl,--wrap=ldexp
LDFLAGS += -Wl,--wrap=copysign
LDFLAGS += -Wl,--wrap=trunc
LDFLAGS += -Wl,--wrap=floor
LDFLAGS += -Wl,--wrap=ceil
LDFLAGS += -Wl,--wrap=round
LDFLAGS += -Wl,--wrap=sincos
LDFLAGS += -Wl,--wrap=asin
LDFLAGS += -Wl,--wrap=acos
LDFLAGS += -Wl,--wrap=atan
LDFLAGS += -Wl,--wrap=sinh
LDFLAGS += -Wl,--wrap=cosh
LDFLAGS += -Wl,--wrap=tanh
LDFLAGS += -Wl,--wrap=asinh
LDFLAGS += -Wl,--wrap=acosh
LDFLAGS += -Wl,--wrap=atanh
LDFLAGS += -Wl,--wrap=exp2
LDFLAGS += -Wl,--wrap=log2
LDFLAGS += -Wl,--wrap=exp10
LDFLAGS += -Wl,--wrap=log10
LDFLAGS += -Wl,--wrap=pow
LDFLAGS += -Wl,--wrap=powint
LDFLAGS += -Wl,--wrap=hypot
LDFLAGS += -Wl,--wrap=cbrt
LDFLAGS += -Wl,--wrap=fmod
LDFLAGS += -Wl,--wrap=drem
LDFLAGS += -Wl,--wrap=remainder
LDFLAGS += -Wl,--wrap=remquo
LDFLAGS += -Wl,--wrap=expm1
LDFLAGS += -Wl,--wrap=log1p
LDFLAGS += -Wl,--wrap=fma
LDFLAGS += -Wl,--wrap=__aeabi_fadd
LDFLAGS += -Wl,--wrap=__aeabi_fdiv
LDFLAGS += -Wl,--wrap=__aeabi_fmul
LDFLAGS += -Wl,--wrap=__aeabi_frsub
LDFLAGS += -Wl,--wrap=__aeabi_fsub
LDFLAGS += -Wl,--wrap=__aeabi_cfcmpeq
LDFLAGS += -Wl,--wrap=__aeabi_cfrcmple
LDFLAGS += -Wl,--wrap=__aeabi_cfcmple
LDFLAGS += -Wl,--wrap=__aeabi_fcmpeq
LDFLAGS += -Wl,--wrap=__aeabi_fcmplt
LDFLAGS += -Wl,--wrap=__aeabi_fcmple
LDFLAGS += -Wl,--wrap=__aeabi_fcmpge
LDFLAGS += -Wl,--wrap=__aeabi_fcmpgt
LDFLAGS += -Wl,--wrap=__aeabi_fcmpun
LDFLAGS += -Wl,--wrap=__aeabi_i2f
LDFLAGS += -Wl,--wrap=__aeabi_l2f
LDFLAGS += -Wl,--wrap=__aeabi_ui2f
LDFLAGS += -Wl,--wrap=__aeabi_ul2f
LDFLAGS += -Wl,--wrap=__aeabi_f2iz
LDFLAGS += -Wl,--wrap=__aeabi_f2lz
LDFLAGS += -Wl,--wrap=__aeabi_f2uiz
LDFLAGS += -Wl,--wrap=__aeabi_f2ulz
LDFLAGS += -Wl,--wrap=__aeabi_f2d
LDFLAGS += -Wl,--wrap=sqrtf
LDFLAGS += -Wl,--wrap=cosf
LDFLAGS += -Wl,--wrap=sinf
LDFLAGS += -Wl,--wrap=tanf
LDFLAGS += -Wl,--wrap=atan2f
LDFLAGS += -Wl,--wrap=expf
LDFLAGS += -Wl,--wrap=logf
LDFLAGS += -Wl,--wrap=ldexpf
LDFLAGS += -Wl,--wrap=copysignf
LDFLAGS += -Wl,--wrap=truncf
LDFLAGS += -Wl,--wrap=floorf
LDFLAGS += -Wl,--wrap=ceilf
LDFLAGS += -Wl,--wrap=roundf
LDFLAGS += -Wl,--wrap=sincosf
LDFLAGS += -Wl,--wrap=asinf
LDFLAGS += -Wl,--wrap=acosf
LDFLAGS += -Wl,--wrap=atanf
LDFLAGS += -Wl,--wrap=sinhf
LDFLAGS += -Wl,--wrap=coshf
LDFLAGS += -Wl,--wrap=tanhf
LDFLAGS += -Wl,--wrap=asinhf
LDFLAGS += -Wl,--wrap=acoshf
LDFLAGS += -Wl,--wrap=atanhf
LDFLAGS += -Wl,--wrap=exp2f
LDFLAGS += -Wl,--wrap=log2f
LDFLAGS += -Wl,--wrap=exp10f
LDFLAGS += -Wl,--wrap=log10f
LDFLAGS += -Wl,--wrap=powf
LDFLAGS += -Wl,--wrap=powintf
LDFLAGS += -Wl,--wrap=hypotf
LDFLAGS += -Wl,--wrap=cbrtf
LDFLAGS += -Wl,--wrap=fmodf
LDFLAGS += -Wl,--wrap=dremf
LDFLAGS += -Wl,--wrap=remainderf
LDFLAGS += -Wl,--wrap=remquof
LDFLAGS += -Wl,--wrap=expm1f
LDFLAGS += -Wl,--wrap=log1pf
LDFLAGS += -Wl,--wrap=fmaf
LDFLAGS += -Wl,--wrap=malloc
LDFLAGS += -Wl,--wrap=calloc
LDFLAGS += -Wl,--wrap=free
LDFLAGS += -Wl,--wrap=_memcpy
LDFLAGS += -Wl,--wrap=_memset
LDFLAGS += -Wl,--wrap=__aeabi_memcpy
LDFLAGS += -Wl,--wrap=__aeabi_memset
LDFLAGS += -Wl,--wrap=__aeabi_memcpy4
LDFLAGS += -Wl,--wrap=__aeabi_memset4
LDFLAGS += -Wl,--wrap=__aeabi_memcpy8
LDFLAGS += -Wl,--wrap=__aeabi_memset8
LDFLAGS += -Wl,-z,max-page-size=4096
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--wrap=printf
LDFLAGS += -Wl,--wrap=vprintf
LDFLAGS += -Wl,--wrap=puts
LDFLAGS += -Wl,--wrap=putchar
LDFLAGS += -Wl,--wrap=getchar

$(PLATFORM)_project:
	$(call print, Buildingfortarget $(TARGET))
	$(call print, CreatingIDEproject)
	$(call mk_dir, $(BUILD_DIR)) $(HIDE)

$(PLATFORM)_sdkopen:
	$(shell python $(PLATFORM_TOOLS)/run_config.py $(BINARY) $(PROJECT) $(PICO_SDK_PATH) $(TARGET) $(JLINK_SERVER_PATH))
	code $(PROJECT)

$(PLATFORM)_sdkclean: clean

$(PLATFORM)_sdkbuild: build

$(HEX): $(BINARY)
	$(call print,[HEX] $(notdir $@))
	$(OC) -O ihex $(BINARY) $(HEX)
	$(call print, $(notdir $@) isready)

$(UF2): $(BINARY)
	$(call print,[UF2] $(notdir $@))
	$(ELF2UF2) $(BINARY) $(UF2)
	$(call print,$(notdir $@) isready)

.SECONDEXPANSION:
$(BOOTSRC): $(BOOTSRCS)
	@mkdir -p $(@D)
	$(CC) $(BOOT_CFLAGS) -T $(BOOTLD) $^ -o $(BOOTELF)
	$(OC) -Obinary $(BOOTELF) $(BOOTBIN)
	python $(BOOT_STAGE2)/pad_checksum -s 0xffffffff $(BOOTBIN) $@

.SECONDEXPANSION:
$(BOOTOBJ): $(BOOTSRC)
	@mkdir -p $(@D)
	$(COMPILE.s)

.PHONY: $(BINARY).gdb
$(BINARY).gdb:
	@echo target remote localhost:3333 > $(BINARY).gdb	
	@echo load $(BINARY) >> $(BINARY).gdb	
	@echo file $(BINARY) >> $(BINARY).gdb
	@echo b main >> $(BINARY).gdb
	@echo monitor reset halt >> $(BINARY).gdb	
	@echo tui enable >> $(BINARY).gdb
	@echo c >> $(BINARY).gdb	

.PHONY: pico_run
$(PLATFORM)_run:all
	openocd -f interface/jlink.cfg -c "transport select swd" -c "adapter_khz 6000" \
	-f target/rp2040.cfg -c "program $(BINARY) reset exit"

$(PLATFORM)_reset:

.PHONY:debug
debug: all $(BINARY).gdb start_openocd
	$(GDB) --command=$(BINARY).gdb


LINK_SRCS = y
link_srcs: 
	$(foreach file,$(sort $(PLATFORM_SRCS)),\
		$(call update_file,$(file),$(call relative_to_project,$(file))) $(HIDE);) echo . $(HIDE)
	$(foreach file,$(sort $(PLATFORM_INCLUDE_FILES)),\
		$(call update_file,$(file),$(call relative_to_project,$(file))) $(HIDE);) echo . $(HIDE)

$(PLATFORM)_post_build: $(HEX) $(UF2) link_srcs

clean_hex:
	@$(call print, [Delete] $(HEX))
	$(call remove_file, $(HEX)) $(HIDE)

clean_uf2:
	@$(call print, [Delete] $(UF2))
	$(call remove_file, $(UF2)) $(HIDE)

clean:clean_hex clean_uf2

start_openocd:
	openocd -f interface/jlink.cfg -c "transport select swd" -c "adapter_khz 6000" \
	-f target/rp2040.cfg -c "init" &
