## xsct tcl file

hsi open_hw_design [lindex $argv 0]
set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]

sdk set_workspace .
sdk create_hw_project -name hw -hwspec [lindex $argv 0]
sdk create_bsp_project -name bsp -hwproject hw -proc $cpu_name -os standalone
sdk create_app_project -name sw -hwproject hw -proc $cpu_name -bsp bsp -app {Hello World}
sdk build_project -type all

exit

