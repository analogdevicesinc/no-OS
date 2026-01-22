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
CFLAGS += -DNO_OS_W5500_NETWORKING
CFLAGS += -DNO_OS_IP=\"$(NO_OS_IP)\"
CFLAGS += -DNO_OS_NETMASK=\"$(NO_OS_NETMASK)\"
CFLAGS += -DNO_OS_GATEWAY=\"$(NO_OS_GATEWAY)\"
NO_OS_IP=192.168.1.110
NO_OS_NETMASK=255.255.255.0
NO_OS_GATEWAY=192.168.1.1
endif

ifeq ($(INTERFACE), ethernet_t1l)
$(info Using ETHERNET T1L as interface for IIO communication)
CFLAGS += -DPQM_CONN_T1L
CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1
NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=0.0.0.0
endif

ifndef PQLIB_PATH
$(error 'No PQM library path defined')
endif

include ../../tools/scripts/generic_variables.mk

CFLAGS_MFLOAT_TYPE=soft
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR

include src.mk
include ../../tools/scripts/generic.mk
