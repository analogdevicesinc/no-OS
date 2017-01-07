# xsct tcl file

hsi open_hw_design [lindex $argv 0]
set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]

sdk setws .
sdk createhw -name hw -hwspec [lindex $argv 0]
sdk createapp -name sw -hwproject hw -proc $cpu_name -bsp bsp -app {Hello World}
sdk build_project -type all

exit
