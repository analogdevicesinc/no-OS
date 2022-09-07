if { $argc != 1 } {
	puts "create_project: Invalid arguments"
	exit
}

sdk setws [lindex $argv 0]

app build -all

exit
