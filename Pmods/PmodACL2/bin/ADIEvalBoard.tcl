# tcl script to init arm and run sdk

fpga -debugdevice devicenr 2 -f ADIEvalBoard.bit
connect arm hw
source ps7_init.tcl
ps7_init
init_user
dow ADIEvalBoard.elf
run
disconnect 64
exit
