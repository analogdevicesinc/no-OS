# Uncomment to use the desired platform
#PLATFORM = xilinx
PLATFORM = linux
# PLATFORM = altera
# PLATFORM = aducm3029
TINYIIOD=y
HARDWARE ?= ad3552r_qspi_pwm.hdf
VERBOSE=0
CFLAGS += -DDEBUG
CFLAGS += -DLOG_LEVEL=LOG_DEBUG
LDFLAGS += -pthread
LIB_FLAGS += -lm
BUILD_DIR=/root/no-OS/projects/ad3552rfmcz/build

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
