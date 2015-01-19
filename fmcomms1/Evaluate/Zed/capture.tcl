
set currentWorkingDirectory [pwd]
set bitFileName "/system.bit"
set tclFileName "/ps7_init.tcl"
set elfFileName "/capture.elf"

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

puts "Capturing data..."

run
after 30000
stop

puts "Moving data into rx.csv file..."

set startAddress 134217728
set readData [mrd $startAddress 16384]

set fp [ open rx.csv a ]
for {set index 1} {$index < 32768} {incr index 2} {
	set data [lindex $readData $index]
	set intData [expr 0x$data]
	
	set sampleQ [expr {$intData & 0xFFFF}]
	set sampleI [expr {($intData >> 16) & 0xFFFF}]
	
	set line $sampleI,$sampleQ
	
	puts $fp $line
}
close $fp

puts "Done."

disconnect 64
exit
