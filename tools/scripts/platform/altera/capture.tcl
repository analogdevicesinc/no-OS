# Data acquisition through System Console, the Nios V counterpart of
# platform/xilinx/capture.tcl.
#
# Reads the capture buffer over the JTAG-to-Avalon master (fpga_m) rather than
# through the CPU: the buffer lives in on-chip memory, which that master sees at
# the same address the CPU and the DMAs do (hdl projects/common/nios_a5e). The
# CPU is left running, so the application stays parked in its while(1) and the
# buffer is not disturbed.
#
# Run as:  system-console --cli --script=capture.tcl <start_addr> <num_of_samples> \
#                                                    <num_of_channels> [storage_bits]
#
# start_addr is the address the application prints
# ("DMA_EXAMPLE Rx: address=0x..."). num_of_samples is per channel.

set start_addr      [lindex $argv 0]
set num_of_samples  [lindex $argv 1]
set num_of_channels [lindex $argv 2]
set storage_bits    [lindex $argv 3]

if {$storage_bits == ""} {
    set storage_bits 16
}

foreach {name value} [list start_addr $start_addr num_of_samples $num_of_samples \
                           num_of_channels $num_of_channels] {
    if {$value == ""} {
        puts "MISSING PARAMETER $name"
        exit 1
    }
}
if {$storage_bits != 16} {
    puts "ERROR: only 16 bit samples are supported"
    exit 1
}

# The JTAG master appears as a "master" service. Anything but exactly one claim
# is ambiguous, so bail rather than guess which path is the right one.
set master_paths [get_service_paths master]
if {[llength $master_paths] == 0} {
    puts "ERROR: no master service. Is the FPGA configured and the cable visible\
          to jtagconfig?"
    exit 1
}
if {[llength $master_paths] > 1} {
    puts "Multiple master services, using the first of:"
    foreach path $master_paths { puts "  $path" }
}
set master [lindex $master_paths 0]
set claim [claim_service master $master ""]

puts "Script parameters:"
puts "Start address = $start_addr"
puts "num of samples = $num_of_samples"
puts "num of channels = $num_of_channels"
puts "bits per sample = $storage_bits\n"

puts "Moving data into .csv files..."

set f_name "capture_ch"
set f_type ".csv"
for {set index 1} {$index <= $num_of_channels} {incr index} {
    set file_name $f_name$index$f_type
    file delete -force $file_name
    set f($index) [open $file_name a]
}

# Samples interleave across channels, two 16-bit samples per 32-bit word, so one
# word carries two consecutive channels. Read in blocks: master_read_32 on the
# whole capture at once is slow to marshal and easy to time out on.
set total_words [expr {$num_of_samples * $num_of_channels / 2}]
set words_per_block 4096
set ch_index 1
set word 0

while {$word < $total_words} {
    set count [expr {min($words_per_block, $total_words - $word)}]
    set data [master_read_32 $claim [expr {$start_addr + $word * 4}] $count]

    foreach value $data {
        set value [expr {$value}]
        # Sign-extend: the converters produce two's complement, and plot.py
        # expects the raw unsigned value it folds itself.
        puts $f($ch_index) [expr {$value & 0xFFFF}]
        set ch_index [expr {$ch_index % $num_of_channels + 1}]
        puts $f($ch_index) [expr {($value >> 16) & 0xFFFF}]
        set ch_index [expr {$ch_index % $num_of_channels + 1}]
    }

    incr word $count
}

for {set index 1} {$index <= $num_of_channels} {incr index} {
    close $f($index)
}

close_service master $claim

puts "Done."
