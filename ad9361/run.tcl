fpga -f sdk_project/workspace/hw/system_top.bit

connect arm hw
stop

source sdk_project/workspace/hw/ps7_init.tcl
ps7_init
ps7_post_config

dow sdk_project/workspace/sw/Debug/sw.elf
run
disconnect 64
