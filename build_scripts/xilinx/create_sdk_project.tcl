#
# Create SDK project
#

# geting parsed arguments
set index 1
	foreach argument $argv {
		set parameter($index) $argument
		incr index 1
	}

# setting the parsed arguments
if { [catch {set system_hdf_location_arg $parameter(1)} fid] } {
	puts "The tcl script was called with less parameters than required"
	exit 1
}
if { [catch {set project_location_arg $parameter(2)} fid] } {
	puts "The tcl script was called with less parameters than required"
	exit 1
}

# check if the hdf exists
if {[file exists $system_hdf_location_arg]} {
	set system_hdf_location [file join [pwd] $system_hdf_location_arg]
	puts "hdf file used: $system_hdf_location"
} else {
	puts "No such hdf file: $system_hdf_location_arg"
	exit
}

# define, create, set, project directories
puts "The build directory is: $project_location_arg"
if {[file exists $project_location_arg]} {
	puts "$project_location_arg already exists"
	set project_location [file join [pwd] $project_location_arg]
	puts $project_location
} else {
	file mkdir $project_location_arg
	set project_location [file join [pwd] $project_location_arg]
	puts "$project_location was created"
}

set source_directory [pwd]
set input "input_files"
file mkdir $project_location/$input/sources
set project_destination $project_location/$input/sources

# acknowledge the board processor
cd $project_location/$input
file copy -force -- $system_hdf_location $project_location/$input
hsi open_hw_design system_top.hdf
set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]
puts "FPGA CPU: $cpu_name"

# create project
cd $project_location
if {[file exists ./workspace/hw]} {
  puts "Delete old workspace"
	file delete -force -- $project_location/workspace
}
sdk setws $project_location/workspace

sdk createhw -name hw -hwspec $system_hdf_location
sdk createbsp -name bsp -hwproject hw -proc $cpu_name -os standalone
sdk createapp -name sw -hwproject hw -proc $cpu_name -bsp bsp -os standalone -lang C -app {Empty Application}

# import sources
source $source_directory/../build_scripts/xilinx/parse_readme_copy_sources.tcl;
file copy -force -- $project_location/workspace/sw/src/lscript.ld $project_destination

if { $cpu_name == "sys_mb" } {
set fp1 [open $project_location/workspace/sw/src/lscript.ld r+]
set file_data [read $fp1]
		if { [regsub -all {_HEAP_SIZE : 0x800;} $file_data {_HEAP_SIZE : 0x10000;} file_data] } {
			puts "successfully  increase the heap size to 0x10000"
		} else { puts "faild to increase the heap size"
			exit -1
		}
chan truncate $fp1 0 ;#delete the content
seek $fp1 0 start
puts $fp1 $file_data
close $fp1
}

sdk importsources -name sw -path $project_destination/
sdk projects -build -type all

# delete the copy of source files
file delete -force -- $project_location/$input
puts "Done."
exit
