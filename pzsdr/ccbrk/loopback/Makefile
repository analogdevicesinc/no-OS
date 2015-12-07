# The related system_top.hdf file must be symlinked or copied over to build.
#
# Also, this requires at least vivado-2015.2 since it uses xsct to run the SDK
# from the command line.

SOURCES := $(wildcard *.c)
EXEC = loopback

all: $(EXEC)

$(EXEC): system_top.hdf
	@mkdir -p sdk
	@echo 'sdk set_workspace sdk' > sdk/sdk.tcl
	@echo 'sdk create_hw_project -name hw -hwspec system_top.hdf' >> sdk/sdk.tcl
	@echo 'sdk create_bsp_project -name bsp -hwproject hw -proc ps7_cortexa9_0 -os standalone' >> sdk/sdk.tcl
	@echo 'sdk create_app_project -name $(EXEC) -hwproject hw -proc ps7_cortexa9_0 -os standalone -lang C -app "Empty Application" -bsp bsp' >> sdk/sdk.tcl
	@echo 'exit' >> sdk/sdk.tcl
	@xsct -n -q -s sdk/sdk.tcl
	@patch -p0 < ../../u-boot-elf-2015.2.patch
	@cp -R $(SOURCES) "sdk/$(EXEC)/src/"
	@echo 'sdk set_workspace sdk' > sdk/sdk.tcl
	@echo 'sdk build_project -name $(EXEC)' >> sdk/sdk.tcl
	@echo 'exit' >> sdk/sdk.tcl
	@xsct -n -q -s sdk/sdk.tcl
	@rm -rf .Xil *.log

clean:
	@rm -rf sdk
