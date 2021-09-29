#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src

SRCS += $(PROJECT)/src/app.c			\
	$(DRIVERS)/spi/spi.c			\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c

INCS += $(INCLUDE)/error.h			\
	$(INCLUDE)/spi.h			\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h
