if { $argc != 2 } {
	puts "read_hdf: Invalid arguments"
	exit
}
cd [lindex $argv 0]
# Open the design
hsi open_hw_design [lindex $argv 1]
# Save the cpu name in a variable
set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]
# Open a file
set fp [open "arch.txt" w+]
# Write the cpu name in the file
puts $fp $cpu_name
close $fp