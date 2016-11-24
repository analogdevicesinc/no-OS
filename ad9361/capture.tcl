proc capture {name offset} {
	set startAddress [expr 8388608 + $offset]
	set readData [mrd $startAddress 32768]
	set ch1 ch1-${name}
	set ch2 ch2-${name}
	
	set fp1 [ open ${ch1} w ]
	set fp2 [ open ${ch2} w ]
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

}


connect arm hw

puts "Storing data into .csv files..."

capture rx 0
capture tx-mon 16384

puts "Done."

disconnect 64
exit