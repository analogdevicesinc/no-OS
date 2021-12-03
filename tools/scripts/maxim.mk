ifndef MAXIM_LIBRARIES
$(error MAXIM_LIBRARIES not defined.$(ENDL))
endif

PLATFORM_RELATIVE_PATH = $(patsubst $(MAXIM_LIBRARIES)%,Maxim%,$1)
PLATFORM_FULL_PATH = $(patsubst Maxim%,$(MAXIM_LIBRARIES)%,$1)
CREATED_DIRECTORIES += Maxim

ifneq "$(HEAP_SIZE)" ""
CFLAGS+=-D__HEAP_SIZE=$(HEAP_SIZE)
endif
ifneq "$(STACK_SIZE)" ""
CFLAGS+=-D_STACK_SIZE=$(STACK_SIZE)
endif

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as

LDFLAGS = -mcpu=cortex-m4 	\
	-Wl,--gc-sections 	\
	--specs=nosys.specs	\
	-mfloat-abi=hard 	\
	-mfpu=fpv4-sp-d16 	\
	--entry=Reset_Handler		
	

CFLAGS=-mthumb                                                                 \
        -mcpu=cortex-m4                                                         \
        -mfloat-abi=hard                                                        \
        -mfpu=fpv4-sp-d16                                                       \
        -Wa,-mimplicit-it=thumb                                                 \
        -fsingle-precision-constant                                             \
        -ffunction-sections                                                     \
        -fdata-sections                                                         \
        -MD                                                                     \
        -Wall                                                                   \
        -Wdouble-promotion                                                      \
        -Wno-format                                                      \
	-g									\
	-c	

CFLAGS += -I$(MAXIM_LIBRARIES)/CMSIS/Include	\
	-I$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include	\
	-I$(MAXIM_LIBRARIES)/MAX32660PeriphDriver/Include	\
	-I$(MAXIM_LIBRARIES)/Boards/EvKit_V1/Include		

CFLAGS += -DTARGET_REV=$(TARGET_REV) \
	-DTARGET=$(TARGET)		\
	-DMAXIM_PLATFORM

LSCRIPT = $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/GCC/max32660.ld

#.PHONY $(BINARY).gdb
#$(BINARY).gdb:
#	@echo target remote localhost:3333 > $(BINARY).gdb
#	@echo load $(BINARY) >> $(BINARY).gdb
#	@echo file $(BINARY) >> $(BINARY).gdb
#	@echo monitor reset halt >> $(BINARY).gdb
#	@echo tb main >> $(BINARY).gdb
#	@echo tb HardFault_Handler >> $(BINARY).gdb
#	@echo tui enable > $(BINARY).gdb
#	@echo c >> $(BINARY).gdb



