connect arm hw

puts "Transfering data..."

set startAddress 8388608
set readData [mrd $startAddress 32768]

set fp1 [ open iq_rx1.csv a ]
set fp2 [ open iq_rx2.csv a ]
for {set index 1} {$index < 65536} {incr index 4} {
	set data [lindex $readData $index]
	set intData [expr 0x$data]
	
	set sampleI1 [expr {$intData & 0xFFFF}]
	set sampleQ1 [expr {($intData >> 16) & 0xFFFF}]
	
	set data [lindex $readData [expr {$index + 2}]]
	set intData [expr 0x$data]
	
	set sampleI2 [expr {$intData & 0xFFFF}]
	set sampleQ2 [expr {($intData >> 16) & 0xFFFF}]
	
	puts $fp1 $sampleI1
	puts $fp1 $sampleQ1
	puts $fp2 $sampleI2
	puts $fp2 $sampleQ2
	
}
close $fp1
close $fp2

puts "Done."

disconnect 64
exit
