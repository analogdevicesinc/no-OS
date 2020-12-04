#!/bin/bash

if [ "$#" -eq 2 ]
then
export PATH="$1/SDK/$2/bin\
:$1/SDK/$2/gnu/microblaze/lin/bin\
:$1/SDK/$2/gnu/arm/lin/bin\
:$1/SDK/$2/gnu/microblaze/linux_toolchain/lin64_le/bin\
:$1/SDK/$2/gnu/aarch32/lin/gcc-arm-linux-gnueabi/bin\
:$1/SDK/$2/gnu/aarch32/lin/gcc-arm-none-eabi/bin\
:$1/SDK/$2/gnu/aarch64/lin/aarch64-linux/bin\
:$1/SDK/$2/gnu/aarch64/lin/aarch64-none/bin\
:$1/SDK/$2/gnu/armr5/lin/gcc-arm-none-eabi/bin\
:$1/SDK/$2/tps/lnx64/cmake-3.3.2/bin:\
:$PATH"
else
echo "Usage : source xilinx_env.sh [/path/to/xilinx/directory] [version]"
fi
