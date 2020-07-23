ifeq (aducm3029,$(strip $(PLATFORM)))
#Aducm3029 makefile
include ../../tools/scripts/aducm.mk

else
#Xilnx and altera makefiles
ifeq ($(OS), Windows_NT)
include ../../tools/scripts/windows.mk
else
include ../../tools/scripts/linux.mk
endif

endif
