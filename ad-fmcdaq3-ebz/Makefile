FILE=README.md
VARIABLE:=$(shell cat $(FILE))
files:=$(subst ],,$(subst [,,$(strip $(foreach dir,$(VARIABLE),$(if $(findstring [.,$(dir)),$(dir))))))

project:=sdk_project
hdf_file:=system_top.hdf
elf_file:= $(project)/workspace/sw/Debug/sw.elf
fpga_processor:=sys_mb
fpga_processor+=ps7_cortexa9_0
fpga_processor+=ps7_cortexa9_1
libxil:=$(foreach name,$(fpga_processor),$(if $(wildcard $(project)/workspace/bsp/$(name)/lib/libxil.a),$(project)/workspace/bsp/$(name)/lib/libxil.a))


src:=$(foreach dir,$(files),$(wildcard $(dir)*.c))
src+=$(foreach dir,$(files),$(wildcard $(dir)*.h))


all:$(elf_file)

$(elf_file): $(libxil) $(src)
	@echo 'creating project'
	xsct -s ../build_scripts/xilinx/create_sdk_project.tcl $(hdf_file) $(project)

$(libxil): $(hdf_file)
	rm -rf $(project)
