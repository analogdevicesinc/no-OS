#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/instr-amplif/ada4250

SRCS += $(PLATFORM_DRIVERS)/spi.c					\
	$(NO-OS)/util/util.c						\

INCS += $(INCLUDE)/util.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/spi.h						\
	$(PLATFORM_DRIVERS)/spi_extra.h
