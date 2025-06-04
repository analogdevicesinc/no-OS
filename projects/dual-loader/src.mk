SRCS += $(PROJECT)/loader.c
INCS += $(PROJECT)/loader.h
LSCRIPT=DualImageLoader.ld

# Add our loader function table
ASM_SRCS=$(PROJECT)/DualImageLoader.S

# The CMSIS startup (startup_max32672.S) will be included automatically 
# by the build system through the platform makefiles
