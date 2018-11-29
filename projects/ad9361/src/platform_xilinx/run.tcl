connect arm hw
rst
fpga -f hw_platform_0/system_top.bit
source hw_platform_0/ps7_init.tcl
ps7_init
init_user
dow [lindex $argv 0]
con
