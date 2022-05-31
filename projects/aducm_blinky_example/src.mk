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

#In aducm projects the name of the source dir should be different from src
#Direcory where app srcs are stored

SRC_DIRS +=	$(PROJECT)/app_src \
		$(PLATFORM_DRIVERS) \
		$(INCLUDE) \
		$(NO-OS)/util

#Include makefiles from each source directory if they exist
SUB_MAKES= $(wildcard $(addsuffix /src.mk, $(SRC_DIRS)))
include $(SUB_MAKES)

DIRECORIES_WITH_MAKEFILES = $(patsubst %/src.mk, %,$(SUB_MAKES))
REMAINING_DIRECORIES = $(filter-out $(DIRECORIES_WITH_MAKEFILES), $(SRC_DIRS))

# recursive wildcard
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS += $(DRIVERS)/api/no_os_gpio.c \
        $(DRIVERS)/api/no_os_i2c.c  \
        $(DRIVERS)/api/no_os_irq.c  \
        $(DRIVERS)/api/no_os_spi.c  \
		$(DRIVERS)/api/no_os_timer.c

SRCS += $(foreach dir, $(REMAINING_DIRECORIES), $(call rwildcard, $(dir),*.c))
INCS += $(foreach dir, $(REMAINING_DIRECORIES), $(call rwildcard, $(dir),*.h))
