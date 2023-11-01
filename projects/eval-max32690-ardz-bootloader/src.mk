SRC_DIRS = $(PROJECT)/src
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/core/musbhsfc
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/enumerate
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/src/devclass
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/musbhsfc
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/enumerate
SRC_DIRS += $(MAXIM_LIBRARIES)/MAXUSB/include/devclass

INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb.h
INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_protocol.h
INCS += $(MAXIM_LIBRARIES)/MAXUSB/include/core/usb_event.h

SRCS += $(MAXIM_LIBRARIES)/MAXUSB/src/core/usb_event.c