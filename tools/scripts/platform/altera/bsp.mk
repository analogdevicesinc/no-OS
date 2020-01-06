CC ?= nios2-elf-gcc
AR = nios2-elf-ar

LIB_NAME = hal_bsp

OBJ_DIR = obj

INCS = -I./drivers/inc			\
	-I./HAL/inc		\
	-I.		\
	-I./drivers/inc

C_FLAGS ?= -xc				\
	-MP				\
	-MMD				\
	-pipe				\
	-D__hal__				\
	-DALT_NO_INSTRUCTION_EMULATION				\
	-DALTERA_TRIPLE_SPEED_MAC				\
	-DALT_SINGLE_THREADED				\
	-DALTERA_AUTONEG_TIMEOUT_THRESHOLD=2500				\
	-DALTERA_CHECKLINK_TIMEOUT_THRESHOLD=10000				\
	-DALTERA_NOMDIO_TIMEOUT_THRESHOLD=1000000   				\
	-O0				\
	-g				\
	-Wall 				\
	-mno-hw-div				\
	-mhw-mul				\
	-mno-hw-mulx				\
	-mgpopt=global  

S_FLAGS ?= -MP				\
	-MMD				\
	-O0				\
	-g				\
	-Wall 				\
	-mno-hw-div				\
	-mhw-mul				\
	-mno-hw-mulx				\
	-mgpopt=global   				\
	-pipe				\
	-D__hal__				\
	-DALT_NO_INSTRUCTION_EMULATION				\
	-DALTERA_TRIPLE_SPEED_MAC				\
	-DALT_SINGLE_THREADED				\
	-DALTERA_AUTONEG_TIMEOUT_THRESHOLD=2500				\
	-DALTERA_CHECKLINK_TIMEOUT_THRESHOLD=10000				\
	-DALTERA_NOMDIO_TIMEOUT_THRESHOLD=1000000 				\
	-Wa,-gdwarf2 			

SRCS_C = $(shell find . -name "*.c")
SRCS_S = $(shell find . -name "*.S")

OBJS_C = $(SRCS_C:.c=.o)
OBJS_S = $(SRCS_S:.S=.o)

%.o:%.c
	$(CC) $(C_FLAGS) $(INCS) -o $@ -c $<

%.o:%.S
	$(CC) $(S_FLAGS) $(INCS) -o $@ -c $<

test:
	echo $(SRCS_C)

.DEFAULT_GOAL := all
all: $(OBJS_C) $(OBJS_S)
	$(AR) -src lib$(LIB_NAME).a $(OBJS_C) $(OBJS_S)

clean:
	rm $(OBJS_C) $(OBJS_S) lib$(LIB_NAME).a
