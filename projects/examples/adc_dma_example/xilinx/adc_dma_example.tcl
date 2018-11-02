
set start_addr 8388608

set num_samples 16384

set num_bytes_per_sample 2

set num_channels 4

#Default mrd access size is word
set mrd_size [expr {$num_samples * $num_channels * $num_bytes_per_sample / 4}]

connect

target 2

puts "Transferring data from target to iq_rx.csv files..."

set read_data [mrd $start_addr $mrd_size]

set fp(0) [open iq_rx1.csv a]
set fp(2) [open iq_rx2.csv a]

set index 1
while {$index < [expr {$mrd_size * 2}]} {
	for {set ch 0} {$ch < $num_channels} {incr ch 2} {
		set data [lindex $read_data [expr {$index + $ch}]]
		set data [expr 0x$data]
		set sample_i [expr {$data & 0xFFFF}]
		set sample_q [expr {($data >> 16) & 0xFFFF}]
		puts $fp($ch) $sample_i
		puts $fp($ch) $sample_q
	}
	incr index $num_channels
}
close $fp(0)
close $fp(2)

puts "Done."

disconnect

exit
