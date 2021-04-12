# Data acquisition trough system-console

# This script has two modes of geting info of the data to read from memory and store
# in local files:
#      1. Receive al info from the command line
#      2. Receive the start address for the captured data in memory, the rest
#         of the info will be automatically read out of the memory (NOT YET IMPLEMENTED);

set start_addr [lindex $argv 0]      ;# hex (0x...)
set num_of_samples [lindex $argv 1]  ;# decinal
set num_of_channels [lindex $argv 2] ;# for RF projects consider 2x nr of channels(I and Q)
set storage_bits [lindex $argv 3]    ;# for 8-16bit rezolution data is stored on 16bit (max 32)

set nios_proc [ lindex [ get_service_paths master ] 0 ]
open_service master $nios_proc

after 1000

# geting parameters from memory if not all parameters are received
if { $storage_bits == "" } {
	set format_address          [expr $start_addr - 0x02]
	set real_bits_address       [expr $start_addr - 0x04]
	set storage_bits_address    [expr $start_addr - 0x06]
	set shift_address           [expr $start_addr - 0x08]
	set endianness_address      [expr $start_addr - 0x10]
	set num_of_samples_address  [expr $start_addr - 0x12]
	set num_of_channels_address [expr $start_addr - 0x20]

	# Required information

	# set format [master_read_32 $nios_proc $format_address]
	# set real_bits [master_read_32 $nios_proc $real_bits_address]
	set storage_bits [master_read_32 $nios_proc $storage_bits_address 1]
	# set shift [master_read_32 $nios_proc $shift_address]
	# set endianness [master_read_32 $nios_proc $endianness_address]
	set num_of_samples [master_read_32 $nios_proc $num_of_samples_address 1]
	set num_of_channels [master_read_32 $nios_proc $num_of_channels_address 1]
}

# Throw warning messages for empty values
if { $storage_bits  == "" } {
	puts "WRONG PARAMETER VALUE storage_bits  = $storage_bits"
}
if { $num_of_samples == ""} {
	puts "WRONG PARAMETER VALUE num_of_samples = $num_of_samples"
}
if { $num_of_channels == ""} {
	puts "WRONG PARAMETER VALUE num_of_channels = $num_of_channels"
}

# Debug messages
puts "Script parameters:"
puts "Start address = $start_addr"
puts "total num of samples: $num_of_samples"
puts "num of channels = $num_of_channels"
puts "bytes per sample = $storage_bits\n"

puts "Moving data into .csv files..."

# Read data from memory
set num_sample_per_32bit_mm [expr $num_of_samples / $num_of_channels]
set readData [master_read_32 $nios_proc $start_addr $num_sample_per_32bit_mm]

set f_name "capture_ch"
set f_type ".csv"

# open .csv files to write in
for {set index 1} {$index <= $num_of_channels} {incr index 1} {
	set file_name $f_name$index$f_type
	file delete -force $file_name
	set f($index) [ open $file_name a ]
}

# Arrange channel data into individual files (considering hdl system has a cpack core)
set ch_x_index 1
for {set index 1} {$index < $num_sample_per_32bit_mm} {incr index 1} {
	set data [lindex $readData $index] ; # 32 bit, 16 bit/channel
	set data [expr $data]
	if { $storage_bits <= 16 } {
		set sample1 [expr {$data & 0xFFFF}]
		set sample2 [expr {($data >> 16) & 0xFFFF}]
		if { $num_of_channels == 1 } {
			puts $f($ch_x_index) $sample1
			puts $f($ch_x_index) $sample2
		} else {
			for {set n 1} {$n < $num_of_channels } {incr n 1} {
				puts $f($ch_x_index) $sample1
				if {$ch_x_index < $num_of_channels} {
					set ch_x_index [expr $ch_x_index + 1]
				} else {
					set ch_x_index 1
				}
				puts $f($ch_x_index) $sample2
				if {$ch_x_index < $num_of_channels} {
					set ch_x_index [expr $ch_x_index + 1]
				} else {
					set ch_x_index 1
				}
				if { [expr $n + 2] < $num_of_channels} {
					set index [expr $index + 1]
					set data [lindex $readData $index]
					set data [expr $data]
					set sample1 [expr {$data & 0xFFFF}]
					set sample2 [expr {($data >> 16) & 0xFFFF}]
				}
			}
		}
	} elseif { $storage_bits <= 32 } { # 32 bit channel
		set sample [expr {$data & 0xFFFFFF}]
		if { $num_of_channels == 1 } {
			puts $f($ch_x_index) $sample
		} else
			for {set n 1} {$n < $num_of_channels } {incr n 1} {
				if {$ch_x_index < $num_of_channels} {
					set ch_x_index [expr $ch_x_index + 1]
				} else {
					set ch_x_index 1
				}
				puts $f($ch_x_index) $sample
				if { [expr $n + 1] < $num_of_channels} {
					set index [expr $index + 1]
					set data [lindex $readData $index]
					set data [expr $data]
					set sample [expr {$data & 0xFFFFFF}]
				}
			}
		}
	} else {
		puts "ERROR: Unsupported number of bits/sample: $storage_bits"
	}
}

# close .csv files
for {set index 1} {$index <= $num_of_channels} {incr index 1} {
	close $f($index)
}

close_service master $nios_proc
puts "Done."
