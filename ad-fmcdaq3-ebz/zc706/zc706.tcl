## zc706 xmd tcl file

fpga -f hw/system_top.bit

connect arm hw
stop

source hw/ps7_init.tcl
ps7_init
ps7_post_config

dow zc706.elf
run
disconnect 64

