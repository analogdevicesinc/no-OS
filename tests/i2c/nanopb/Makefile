# Include the nanopb provided Makefile rules
include ../../extra/nanopb.mk

# Build directory
BUILD = build
UTILS = utils
TRANSPORT = transport

# Compiler flags - aggressive size optimization for embedded
CFLAGS = -Wall -Os -g -flto -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables
CFLAGS += -I$(NANOPB_DIR)
CFLAGS += -I.
CFLAGS += -I$(BUILD)

# Linker flags - remove unused sections and strip symbols
LDFLAGS = -Wl,--gc-sections#,--strip-all -flto    # <<--- CHANGE HERE FOR OPTIMIZATION AND REMOVE -g FLAG!!!!

# CAPI library
CAPI_DIR = /home/ruben/ADI/core-sw-common-hal
CAPI_SRC = $(CAPI_DIR)/src/capi_i2c.c
CFLAGS += -I$(CAPI_DIR)/api

# Source files
TRANSPORT_SRC = $(TRANSPORT)/transport.c
PROTO_SRC = $(BUILD)/capi.pb.c
RPC_SRC = $(BUILD)/capi_rpc.c
DRIVER_SRC = platform/nanopb/capi_i2c_nanopbClient_driver.c
#PLATFORM_DRIVER_SRC = platform/nanopb/capi_i2c_dummy_driver.c
PLATFORM_DRIVER_SRC = platform/linux/capi_i2c_linux_driver.c

all: $(BUILD)/client $(BUILD)/server

.SUFFIXES:

# Create build directory
$(BUILD):
	mkdir -p $(BUILD)

clean:
	rm -rf $(BUILD)

# Generate proto with options file
$(BUILD)/capi.pb.c $(BUILD)/capi.pb.h: capi.proto capi.options | $(BUILD)
	$(PROTOC) $(PROTOC_OPTS) --nanopb_out=$(BUILD) capi.proto

# Generate RPC stubs
$(BUILD)/capi_rpc.c $(BUILD)/capi_rpc.h: capi.proto $(UTILS)/nanopb_rpc_generator.py | $(BUILD)
	python3 $(UTILS)/nanopb_rpc_generator.py capi.proto --output $(BUILD)/capi_rpc

$(BUILD)/client: main_client.c $(DRIVER_SRC) $(TRANSPORT_SRC) $(RPC_SRC) $(PROTO_SRC) $(CAPI_SRC) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(NANOPB_CORE)

$(BUILD)/server: server.c $(PLATFORM_DRIVER_SRC) $(TRANSPORT_SRC) $(RPC_SRC) $(PROTO_SRC) $(CAPI_SRC) | $(BUILD)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(NANOPB_CORE)
