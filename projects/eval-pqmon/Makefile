BASIC_EXAMPLE = y
IIOD=y

HEAP_SIZE=0x80000
STACK_SIZE=0x20000

ifndef PQLIB_PATH
$(error 'No PQLIB_PATH library path defined')
endif

include ../../tools/scripts/generic_variables.mk

CFLAGS_MFLOAT_TYPE=soft
CFLAGS += -DNATIVE_SDHC
CFLAGS += -DNO_OS_USB_UART
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1

include src.mk
include ../../tools/scripts/generic.mk
