
# xsdb tcl file

set m_type [lindex $argv 0]

connect

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "PSU"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "xc7z*"}
}

if {$m_type == "MICROBLAZE"} {
  targets -set -filter {name =~ "xc*"}
}

puts "Moving data into .csv files..."

after 1000

set readData [list]

set startAddress 8388608

for {set i 0} {$i < 32768} {incr i 1} {
  set readData [lappend $readData [mrd -force [expr $startAddress + $i]]]
}

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

disconnect 64
exit
