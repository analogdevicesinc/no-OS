set arch [lindex $argv 0]
set bitstream [lindex $argv 1]
set elf [lindex $argv 2]
set hw_path [lindex $argv 3]

# Connect to the fpga
connect

# Reset and stop the ARM CPU before we re-program the FPGA if we are on a ZYNQ.
# Otherwise undefined behavior can occur.
if {$arch == "ps7_cortexa9_0"} {
	targets -set -filter {name =~ "APU*"}
	stop
	rst
}

# Write the bitstream
if {$arch == "ps7_cortexa9_0"} {
	targets -set -filter {name =~ "xc7z*"}
} elseif {$arch == "psu_cortexa53_0"} {
	targets -set -filter {name =~ "PSU"}
} elseif {$arch == "sys_mb"} {
	targets -set -filter {name =~ "xc7*" || name =~ "xck*" }
}
fpga -file "[file normalize $bitstream]"

# Initialize the board
if {$arch == "ps7_cortexa9_0"} {
	targets -set -filter {name =~ "APU*"}
	source "[file normalize $hw_path/ps7_init.tcl]"
	ps7_init
	ps7_post_config
} elseif {$arch == "psu_cortexa53_0"} {
	targets -set -filter {name =~ "APU*"}
	source "[file normalize $hw_path/psu_init.tcl]"
	psu_init
	psu_post_config
	psu_ps_pl_isolation_removal
	psu_ps_pl_reset_config
	mwr 0xffff0000 0x14000000
	mwr 0xFD1A0104 0x380E
} elseif {$arch == "sys_mb"} {
	targets -set -filter {name =~ "xc7*" || name =~ "xck*" }
}

# Upload the elf file
if {$arch == "ps7_cortexa9_0"} {
	targets -set -filter {name =~ "*Cortex-A9 MPCore #0*"}
} elseif {$arch == "psu_cortexa53_0"} {
	targets -set -filter {name =~ "*Cortex-A53 #0*"}
} elseif {$arch == "sys_mb"} {
	after 1000
	targets -set -filter {name =~ "*MicroBlaze #*"}
}
dow "[file normalize $elf]"
con
disconnect
exit