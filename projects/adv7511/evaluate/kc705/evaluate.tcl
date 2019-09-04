
set currentWorkingDirectory [pwd]
set bitFileName "/system.bit"
set elfFileName "/sw.elf"
set elfFilePath $currentWorkingDirectory$elfFileName
set bitFilePath $currentWorkingDirectory$bitFileName

fpga -f $bitFilePath
connect mb mdm
rst
stop
dow $elfFilePath
run
disconnect 0
exit
