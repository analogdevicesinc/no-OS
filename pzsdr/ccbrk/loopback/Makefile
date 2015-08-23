# Symlink or copy over the related system_top.hdf to build

SOURCES := loopback.c
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
	# apply fixes to make the elfs run in u-boot properly
	@patch -p0 < ../../u-boot-elf-xil-crt0.S.patch
	@sed -i 's/^ENTRY(_vector_table)/ENTRY(_start)/' "sdk/$(EXEC)/src/lscript.ld"
	@cp -R $(SOURCES) "sdk/$(EXEC)/src/"
	@echo 'sdk set_workspace sdk' > sdk/sdk.tcl
	@echo 'sdk build_project -name $(EXEC)' >> sdk/sdk.tcl
	@echo 'exit' >> sdk/sdk.tcl
	@xsct -n -q -s sdk/sdk.tcl

clean:
	@rm -rf sdk .Xil
