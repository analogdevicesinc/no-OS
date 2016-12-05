# The related system_top.hdf file must be symlinked or copied over to build.
#
# Also, this requires at least vivado-2016.2 for the xsct options to work.
#
# See the follow reference guide for xsct as needed:
# http://www.xilinx.com/support/documentation/sw_manuals/xilinx2016_2/ug1208-xsct-reference-guide.pdf

SOURCES := $(wildcard *.c)
EXEC = loopback
WORKDIR = $(shell pwd)

all: $(EXEC)

$(EXEC): system_top.hdf
	@mkdir -p sdk
	@echo 'sdk setws sdk' > sdk/sdk.tcl
	@echo 'sdk createhw -name hw -hwspec system_top.hdf' >> sdk/sdk.tcl
	@echo 'sdk createbsp -name bsp -hwproject hw -proc ps7_cortexa9_0 -os standalone' >> sdk/sdk.tcl
	@echo 'sdk createapp -name $(EXEC) -hwproject hw -proc ps7_cortexa9_0 -os standalone -lang C -app "Empty Application" -bsp bsp' >> sdk/sdk.tcl
	@echo 'exit' >> sdk/sdk.tcl
	@xsct -no-ini -quiet sdk/sdk.tcl
	@cd sdk/bsp/ps7_cortexa9_0/libsrc/standalone_v*/src && patch -p6 < "${WORKDIR}"/../../u-boot-elf-2015.2.patch
	@cp -R $(SOURCES) "sdk/$(EXEC)/src/"
	@echo 'sdk setws sdk' > sdk/sdk.tcl
	@echo 'sdk projects -build' >> sdk/sdk.tcl
	@echo 'exit' >> sdk/sdk.tcl
	@xsct -no-ini -quiet sdk/sdk.tcl
	@rm -rf .Xil *.log

clean:
	@rm -rf sdk
