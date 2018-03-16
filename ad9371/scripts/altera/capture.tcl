puts "Acquiring data"
set mm [ lindex [ get_service_paths master ] 0 ]
close_service master $mm
open_service master $mm

set startAddress 0x800000

set fp1 [ open iq_rx1.csv a ]
set fp2 [ open iq_rx2.csv a ]
set readData [ master_read_32 $mm $startAddress 32768]

for {set index 0} {$index < 32768} {incr index 2} {
	set data [lindex $readData $index]

	set sampleI1 [expr {$data & 0xFFFF}]
	set sampleQ1 [expr {($data >> 16) & 0xFFFF}]

	set data [lindex $readData [expr {$index + 1}]]

	set sampleI2 [expr {$data & 0xFFFF}]
	set sampleQ2 [expr {($data >> 16) & 0xFFFF}]

	puts $fp1 $sampleI1
	puts $fp1 $sampleQ1
	puts $fp2 $sampleI2
	puts $fp2 $sampleQ2
}

close $fp1
close $fp2

puts "Iteration complete"
close_service master $mm
