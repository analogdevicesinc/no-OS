# Data acquisition trough xsdb

# This script has two modes of geting info of the data to read from memory and store
# in local files:
#      1. Receive al info from the command line
#      2. Receive the start address for the captured data in memory, the rest
#         of the info will be automatically read out of the memory (NOT YET IMPLEMENTED);

set m_type [lindex $argv 0]
set start_addr [lindex $argv 1]      ;# hex (0x...)
set num_of_samples [lindex $argv 2]  ;# decinal
set num_of_channels [lindex $argv 3] ;# for RF projects consider 2x nr of channels(I and Q)
set storage_bits  [lindex $argv 4]   ;# for 8-16bit rezolution data is stored on 16bit (max 32)

connect

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "*A53*#0*"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "ARM*#0*"}
}

if {$m_type == "MICROBLAZE"} {
  targets -set -filter {name =~ "*MicroBlaze #0*"}
}

if {$m_type == "VERSAL"} {
  targets -set -filter {name =~ "*A72*#0*"}
}

after 1000

if { $storage_bits  == "" } {
	puts "Read parameters from memory"
	set format_address          [expr $start_addr - 0x02 1]
	set real_bits_address       [expr $start_addr - 0x04 1]
	set storage_bits_address    [expr $start_addr - 0x06 1] ;# bytes_per_sample
	set shift_address           [expr $start_addr - 0x08 1]
	set endianness_address      [expr $start_addr - 0x10 1]
	set num_of_samples_address  [expr $start_addr - 0x12 1]
	set num_of_channels_address [expr $start_addr - 0x20 1]

	# Used information
	set storage_bits [mrd -force $storage_bits_address]
	set num_of_samples [mrd -force $num_of_samples_address]
	set num_of_channels [mrd -force $num_of_channels_address]
}

if { $storage_bits  == "" } {
	puts "WRONG PARAMETER VALUE storage_bits  = $storage_bits "
	exit
}
if { $num_of_samples == ""} {
	puts "WRONG PARAMETER VALUE num_of_samples = $num_of_samples"
	exit
}
if { $num_of_channels == ""} {
	puts "WRONG PARAMETER VALUE num_of_channels = $num_of_channels"
	exit
}

puts "Script parameters:"
puts "Start address = $start_addr"
puts "num of samples = $num_of_samples"
puts "num of channels = $num_of_channels"
puts "bytes per sample = $storage_bits\n"

puts "Moving data into .csv files..."

set start_addr [expr $start_addr]
set num_of_word [expr 2 * $num_of_samples]
set readData [mrd -force $start_addr $num_of_word]

set f_name "capture_ch"
set f_type ".csv"

for {set index 1} {$index <= $num_of_channels} {incr index 1} {
	set file_name $f_name$index$f_type
	file delete -force $file_name
	set f($index) [ open $file_name a ]
}

# mrd returns:
# index     x       x+1
# info   address: data(32b)

# Arrange channel data into individual files (considering hdl system has a cpack core)
set ch_x_index 1
for {set index 1} {$index < $num_of_samples} {incr index 2} {
	set data [lindex $readData $index] ; # 32 bit, 16 bit/channal
	set data [expr 0x$data]
	if { $storage_bits <= 16 } {
		set sample1 [expr {$data & 0xFFFF}]
		set sample2 [expr {($data >> 16) & 0xFFFF}]
		if { $num_of_channels == 1 } {
			puts $f($ch_x_index) $sample1
			puts $f($ch_x_index) $sample2
		} else {
			for {set n 1} {$n < $num_of_channels } {incr n 2} {
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
					set index [expr $index + 2]
					set data [lindex $readData $index]
					set data [expr 0x$data]
					set sample1 [expr {$data & 0xFFFF}]
					set sample2 [expr {($data >> 16) & 0xFFFF}]
				}
			}
		}
	} elseif { $storage_bits <= 32 } {
		set sample [expr {$data & 0xFFFFFF}]
		if { $num_of_channels == 1 } {
			puts $f($ch_x_index) $sample
		} else {
			for {set n 1} {$n < $num_of_channels } {incr n 1} {
				if {$ch_x_index < $num_of_channels} {
					set ch_x_index [expr $ch_x_index + 1]
				} else {
					set ch_x_index 1
				}
				puts $f($ch_x_index) $sample
				if { [expr $n + 1] < $num_of_channels} {
					set index [expr $index + 2]
					set data [lindex $readData $index]
					set data [expr 0x$data]
					set sample [expr {$data & 0xFFFFFF}]
				}
			}
		}
	} else {
		puts "ERROR: Unsupported number of bits/sample"
		exit
	}
}

for {set index 1} {$index <= $num_of_channels} {incr index 1} {
	close $f($index)
}

puts "Done."

disconnect

exit
