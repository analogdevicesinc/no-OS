# tcl script to init arm and run sdk

fpga -debugdevice devicenr 2 -f ADIEvalBoard.bit
connect arm hw
source ps7_init.tcl
ps7_init
init_user
dow ADIEvalBoard.elf

	# Setting startTest to test_ptr[]
	set startTest [mrd_var admp441DataPointer]
	set startTest [format "0x%08x" $startTest]

puts "Running program for 25 seconds..."	
	con
	after 25000
	stop

puts "Recorded data successfully stored in RAM"

puts "Generating EVAL_ADMP441Z_Demo.wav"	
puts "Reading data from RAM"	
puts "Moving data into file EVAL_ADMP441Z_Demo.wav "
		
puts -nonewline "Writing WAVE Header..."		
	# Write WAVE Header
	set data {}
	set micData [mrd $startTest 11]
	for {set i 1} {$i < 22} {incr i 2} {
		set val [lindex $micData $i]
		if { ![string length $val] } {
			set val "0"
		} 
		set val [expr 0x$val] 
		lappend data $val 
		lappend data "\n" 
		set addr [expr $startTest + 4]
		set startTest [format "0x%08x" $addr]
	}		 
	set fp [ open EVAL_ADMP441Z_Demo.wav a ]
	fconfigure $fp -translation binary
	for { set index 0 } { $index <= 21 } { incr index 2} {
		set byte_1 [ lindex $data $index ]
		set binary_first_16_bits [binary format S1 [expr {($byte_1 >> 16)}]]
		set binary_second_16_bits [binary format S1 [expr {(($byte_1 << 16) >> 16)}]]
		puts -nonewline $fp $binary_first_16_bits
		puts -nonewline $fp $binary_second_16_bits
	}
	close $fp

puts "Done!"

puts "Writing Audio Data"
puts "Operation will take a few minutes"
        
	 #Write audio data	
	 for {set j 0} {$j < 5000} {incr j 1} {		
		set data {}
        puts $j
		set micData [mrd $startTest 100]
		for {set i 1} {$i < 200} {incr i 2} {
			set val [lindex $micData $i]
			if { ![string length $val] } {
				set val "0"
			} 
			set val [expr 0x$val]  
			lappend data $val 
			lappend data "\n" 
			set addr [expr $startTest + 4]
			set startTest [format "0x%08x" $addr]
		}		 
		set fp [ open EVAL_ADMP441Z_Demo.wav a ]
		fconfigure $fp -translation binary
		for { set index 0 } { $index <= 199 } { incr index 2} {
			set byte_1 [ lindex $data $index ]
			set binary_first_16_bits [binary format S1 [expr {($byte_1 >> 16)}]]
			set binary_second_16_bits [binary format S1 [expr {(($byte_1 << 16) >> 16)}]]
			puts -nonewline $fp $binary_first_16_bits
			puts -nonewline $fp $binary_second_16_bits
		}
		close $fp
	  }	

puts "Data acquisition is complete. Data is stored in the file EVAL_ADMP441Z_Demo.wav."	

puts "Disconnecting from the processor..." 
	stop
	
puts ">>>"
puts "Press CTRL-C and type Y"
	after 20000

run
disconnect 64
exit
