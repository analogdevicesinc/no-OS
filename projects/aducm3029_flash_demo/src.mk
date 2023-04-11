#See No-OS/tool/scripts/src_model.mk for variable description

CFLAGS += -DENABLE_UART_STDIO

SRC_DIRS += $(PROJECT)/src

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(DRIVERS)/api
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
