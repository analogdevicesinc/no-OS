
set currentWorkingDirectory [pwd]
set bitFileName "/system.bit"
set elfFileName "/capture.elf"

set bitFilePath $currentWorkingDirectory$bitFileName
set elfFilePath $currentWorkingDirectory$elfFileName

fpga -f $bitFilePath
connect mb mdm
after 500
rst
after 500
stop
after 500
rst
after 500
dow $elfFilePath

puts "Capturing data..."

run
after 15000
stop

puts "Moving data into rx.csv file..."

set startAddress 2281701376
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

disconnect 0
exit
