FILE=README.md
VARIABLE := $(shell cat $(FILE))
files := $(subst ],,$(subst [,,$(strip $(foreach dir,$(VARIABLE),$(if $(findstring [.,$(dir)),$(dir))))))

project := sdk_project
hdf_file:= system_top.hdf
elf_file := $(project)/workspace/sw/Debug/sw.elf
libxil := $(project)/workspace/bsp/sys_mb/lib/libxil.a

src := $(foreach dir,$(files),$(wildcard $(dir)*.c))
src += $(foreach dir,$(files),$(wildcard $(dir)*.h))


all:$(elf_file)

$(elf_file): $(libxil) $(src)
	@echo 'creating project'
	xsct -s ../build_scripts/xilinx/create_sdk_project.tcl ./$(hdf_file) $(project)

$(libxil): $(hdf_file)
	rm -rf $(project)
