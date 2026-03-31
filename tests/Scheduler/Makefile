include ../../extra/nanopb.mk

# Compiler flags to enable all warnings & debug info
CFLAGS = -Wall -g -O0
CFLAGS += -I$(NANOPB_DIR)

# CAPI library
CAPI_DIR = /home/ruben/ADI/core-sw-common-hal
CAPI_SRC = $(CAPI_DIR)/src/capi_i2c.c
CFLAGS += -I$(CAPI_DIR)/api

all: main

.SUFFIXES:

clean:
	rm -f main capi.pb.c capi.pb.h

%: %.c $(CAPI_SRC) Queue.c encoders.c capi.pb.c
	$(CC) $(CFLAGS) -o $@ $^ $(NANOPB_CORE)
