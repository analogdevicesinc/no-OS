SRCS += $(PROJECT)/loader.c
INCS += $(PROJECT)/loader.h

# Select linker script based on target
ifeq ($(TARGET),max32672)
LSCRIPT=DualImageLoader_max32672.ld
else ifeq ($(TARGET),max32690)
LSCRIPT=DualImageLoader_max32690.ld
endif

# Add our loader function table
ASM_SRCS=$(PROJECT)/DualImageLoader.S

# The CMSIS startup will be included automatically 
# by the build system through the platform makefiles
