
set currentWorkingDirectory [pwd]
set bitFileName "/system.bit"
set elfFileName "/evaluate.elf"

set bitFilePath $currentWorkingDirectory$bitFileName
set elfFilePath $currentWorkingDirectory$elfFileName

#fpga -f $bitFilePath
connect mb mdm
rst
stop
dow $elfFilePath
run
disconnect 0
exit
