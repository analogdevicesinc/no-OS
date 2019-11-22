#!/bin/bash

if [ "$#" -eq 2 ]
then
export PATH="$PATH							\
:$$(1)/$$(2)/quartus/bin						\
:$$(1)/$$(2)/quartus/sop_builder/bin					\
:$$(1)/$$(2)/nios2eds							\
:$$(1)/$$(2)/nios2edsbin						\
:$$(1)/$$(2)/nios2eds/sdk/bin						\
:$$(1)/$$(2)/embedded							\
:$$(1)/$$(2)/nios2edsbin/gnu/H-x86_64-pc-linux-gnu/bin			\
:$$(1)/$$(2)/modelsim_ase/linuxaloem					\
:$$(1)/$$(2)/embedded/host_tools/menor/gnu/arm/baremetal/bin		\
:$$(1)/$$(2)/embedded/host_tools/altera/preloadergen			\
:$$(1)/$$(2)/embedded/host_tools/altera/mkimage				\
:$$(1)/$$(2)/embedded/host_tools/altera/mkpimag				\
:$$(1)/$$(2)/embedded/host_tools/altera/device_tee			\
:$$(1)/$$(2)/embedded/host_tools/altera/diskutils			\
:$$(1)/$$(2)/embedded/host_tools/altera/imagecat			\
:$$(1)/$$(2)/embedded/host_tools/altera/securebot			\
:$$(1)/$$(2)/embedded/host_tools/gnu/dtc				\
:$$(1)/$$(2)/embedded/ds-5/sw/gcc/bin					\
:$$(1)/$$(2)/embedded/ds-5/sw/ARMCompler5.06u1/bin			\
:$$(1)/$$(2)/embedded/ds-5/bi"
else
echo "Usage : source intel_env.sh [/path/to/intel/directory] [version]"
fi