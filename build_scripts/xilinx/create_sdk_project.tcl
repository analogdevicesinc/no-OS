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
if { [catch {set proj_dir $parameter(1)} fid] } {
	puts "The tcl script was called with less parameters than required"
	exit 1
}
if { [catch {set system_hdf_location $parameter(2)} fid] } {
	puts "The tcl script was called with less parameters than required"
	exit 1
}

# define, create, set, project directories
puts "The build directory is: $proj_dir"
if {[file exists $proj_dir]} {
	puts "$proj_dir already exists"
} else { 
	file mkdir $proj_dir
	puts "$proj_dir was created"
}
  
set input "input_files"
file mkdir $proj_dir/$input/sources
set project_destination $proj_dir/$input/sources
set workspace "workspace"
set source_directory [pwd]

# acknowledge the board processor
cd $proj_dir/$input
file copy -force -- $system_hdf_location $proj_dir/$input
hsi open_hw_design system_top.hdf
set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0] 
puts "FPGA CPU: $cpu_name"
 
# create project
cd $proj_dir
sdk set_workspace $proj_dir/$workspace
sdk create_hw_project -name hw -hwspec $system_hdf_location
sdk create_bsp_project -name bsp -hwproject hw -proc $cpu_name -os standalone 
sdk create_app_project -name sw -hwproject hw -proc $cpu_name -bsp bsp -os standalone -lang C -app {Empty Application} 

# import sources
source $source_directory/../build_scripts/xilinx/parse_readme_copy_sources.tcl; 
file copy -force -- $proj_dir/$workspace/sw/src/lscript.ld $project_destination
sdk import_sources -name sw -path $project_destination
sdk build_project -type all

# delete the copy of source files
file delete -force -- $proj_dir/$input
puts "Done."
exit