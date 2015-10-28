## vc707 xmd tcl file

fpga -f hw/system_top.bit

connect mb mdm
stop


dow vc707.elf
run
disconnect 0

