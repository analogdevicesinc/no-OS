
connect arm hw

puts "Moving data into rx.csv file..."

set startAddress 8388608
set readData [mrd $startAddress 16384]

set fp [ open rx.csv a ]
for {set index 1} {$index < 32768} {incr index 2} {
	set data [lindex $readData $index]
	set intData [expr 0x$data]
	
	set byte0 [expr {$intData & 0xFF}]
	set byte1 [expr {($intData >> 8) & 0xFF}]
	set byte2 [expr {($intData >> 16) & 0xFF}]
	set byte3 [expr {($intData >> 24) & 0xFF}]

	set sample0 [expr {($byte0 << 8) | $byte2}]
	set sample1 [expr {($byte1 << 8) | $byte3}]

        # samples are in two's complement
        if { $sample0 > 0x7FFF } {
          set sample0 [expr [expr ~$sample0] & 0x7FFF]
          set sample0 [expr {int ([expr $sample0 + 1] * -1) }]
        }
        if { $sample1 > 0x7FFF } {
          set sample1 [expr [expr ~$sample1] & 0x7FFF]
          set sample1 [expr {int ([expr $sample1 + 1] * -1) }]
        }

	set line $sample0,$sample1
	puts $fp $line
}
close $fp

puts "Done."

disconnect 64
exit
