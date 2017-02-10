
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

set readData [mrd -force $start_addr [expr 4 * $num_of_sample]]

set fp1 [ open channel_0.csv w ]
set fp2 [ open channel_1.csv w ]
for {set index 1} {$index < 65536} {incr index 2} {
	set data [lindex $readData $index]
	set intData [expr 0x$data]

	set sampleI1 [expr {$intData & 0xFFFF}]
	set sampleQ1 [expr {($intData >> 16) & 0xFFFF}]

	puts $fp1 $sampleI1
	puts $fp2 $sampleQ1

}
close $fp1
close $fp2

puts "Done."

disconnect

exit
