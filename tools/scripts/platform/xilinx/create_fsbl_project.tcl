set build_dir [lindex $argv 0]
set hardware [lindex $argv 1]
hsi open_hw_design $::hardware
set cpu_name [lindex [hsi get_cells -hier -filter {IP_TYPE==PROCESSOR}] 0]
platform create -name hw0 -hw $::hardware -os standalone -out $::build_dir/tmp/output -proc $cpu_name
platform generate
