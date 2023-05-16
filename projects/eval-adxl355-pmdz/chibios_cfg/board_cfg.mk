# Target settings.
MCU  = cortex-m4

# Startup files.
include $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk/startup_stm32f4xx.mk

include $(CHIBIOS)/os/hal/ports/STM32/STM32F4xx/platform.mk
include $(CHIBIOS)/os/hal/boards/ADI_EVAL_SDP_CK1Z/board.mk

include $(CHIBIOS)/os/common/ports/ARMv7-M/compilers/GCC/mk/port.mk

# Define linker script file here
LDSCRIPT= $(STARTUPLD)/STM32F469xI.ld

RULESPATH = $(CHIBIOS)/os/common/startup/ARMCMx/compilers/GCC/mk
