
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
after 5000
stop

puts "Moving data into rx.csv file..."

set startAddress 8388608
set readData [mrd $startAddress 32768]

set fp [ open rx.csv a ]
for {set index 1} {$index < 65536} {incr index 4} {
	set data [lindex $readData $index]
	set intData [expr 0x$data]
	
	set sampleQ1 [expr {$intData & 0xFFFF}]
	set sampleI1 [expr {($intData >> 16) & 0xFFFF}]
	
	set data [lindex $readData [expr {$index + 2}]]
	set intData [expr 0x$data]
	
	set sampleQ2 [expr {$intData & 0xFFFF}]
	set sampleI2 [expr {($intData >> 16) & 0xFFFF}]
	
	set line $sampleI1,$sampleQ1,$sampleI2,$sampleQ2
	
	puts $fp $line
}
close $fp

puts "Done."

disconnect 64
exit
