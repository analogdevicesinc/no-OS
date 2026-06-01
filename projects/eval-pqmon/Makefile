INTERFACE ?= usb

BASIC_EXAMPLE = y
IIOD=y

HEAP_SIZE=0x80000
STACK_SIZE=0x20000

ifeq ($(INTERFACE), usb)
$(info Using USB as interface for IIO communication)
CFLAGS += -DPQM_CONN_USB
CFLAGS += -DNO_OS_USB_UART
endif

ifeq ($(INTERFACE), serial)
$(info Using SERIAL as interface for IIO communication)
CFLAGS += -DPQM_CONN_SERIAL
endif

ifeq ($(INTERFACE), ethernet)
$(info Using ETHERNET as interface for IIO communication)
CFLAGS += -DPQM_CONN_USB
CFLAGS += -DNO_OS_USB_UART
CFLAGS += -DPQM_CONN_ETH
NETWORKING = y
endif

ifeq ($(INTERFACE), ethernet_t1l)
$(info Using ETHERNET T1L as interface for IIO communication)
CFLAGS += -DPQM_CONN_T1L
NETWORKING = y
endif

ifndef PQLIB_PATH
$(error 'No PQM library path defined')
endif

include ../../tools/scripts/generic_variables.mk

CFLAGS_MFLOAT_TYPE=soft
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR

include src.mk
include ../../tools/scripts/generic.mk
