
# xsdb tcl file

set m_type [lindex $argv 0]
set start_addr [lindex $argv 1]
set num_of_sample [lindex $argv 2]

connect

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "ARM*#0*"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "ARM*#0*"}
}

if {$m_type == "MICROBLAZE"} {
  targets -set -filter {name =~ "*MicroBlaze*"}
}

puts "Moving data into .csv files..."

after 1000

set start_addr [expr 0x$start_addr]
set num_of_word [expr 2 * $num_of_sample]
set readData [mrd -force $start_addr $num_of_word]

set fp [ open capture.csv w ]
for {set index 1} {$index < $num_of_word} {incr index 4} {

	set data [lindex $readData $index]

	puts $fp $data

}
close $fp

puts "Done."

disconnect

exit
