# tcl script to init arm and run sdk

fpga -debugdevice devicenr 2 -f cf_adv7511_zed.bit
connect arm hw
source ps7_init.tcl
ps7_init
dow cf_adv7511_zed.elf
run
disconnect 64
