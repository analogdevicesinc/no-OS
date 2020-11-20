if { $argc != 1 } {
	puts "create_project: Invalid arguments"
	exit
}

sdk setws [lindex $argv 0]

projects -build -name app -type app

exit
