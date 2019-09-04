
set currentWorkingDirectory [pwd]
set bitFileName "/system.bit"
set tclFileName "/ps7_init.tcl"
set elfFileName "/sw.elf"

set bitFilePath $currentWorkingDirectory$bitFileName
set tclFilePath $currentWorkingDirectory$tclFileName
set elfFilePath $currentWorkingDirectory$elfFileName

fpga -debugdevice devicenr 2 -f $bitFilePath
connect arm hw
rst -processor
source $tclFilePath
ps7_init
init_user
dow $elfFilePath
run
disconnect 64
exit
