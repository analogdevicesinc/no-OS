if { $argc != 3 } {
	puts "create_project: Invalid arguments"
	exit
}
# Set the workspace
sdk setws [lindex $argv 0]
# Create hwspec
sdk createhw -name hw -hwspec [lindex $argv 1]
# Create bsp
sdk createbsp -name bsp -hwproject hw -proc [lindex $argv 2] -os standalone  
# Create app
sdk createapp -name app -hwproject hw -proc [lindex $argv 2] -os standalone \
	-lang C -app {Empty Application} -bsp bsp
# Build the project via SDK
clean -type all
build -type all

exit