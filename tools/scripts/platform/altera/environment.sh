#!/bin/bash

if [ "$#" -eq 2 ]
then
export PATH="$1/$2/quartus/bin\
:$1/$2/nios2eds/bin\
:$1/$2/nios2eds/bin/gnu/H-x86_64-pc-linux-gnu/bin\
:$1/$2/nios2eds/sdk2/bin\
:$PATH"
else
echo "Usage : source intel_env.sh [/path/to/intel/directory] [version]"
fi