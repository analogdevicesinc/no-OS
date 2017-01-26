# xsdb tcl file

set m_type [lindex $argv 0]

connect

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "PSU"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "xc7z*"}
}

if {$m_type == "MICROBLAZE"} {
}

fpga -file hw/system_top.bit

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "APU*"}
  source hw/psu_init.tcl
  psu_init
  psu_post_config
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "APU*"}
  source hw/ps7_init.tcl
  ps7_init
  ps7_post_config
}

if {$m_type == "ZYNQ_PSU"} {
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "*Cortex-A9 MPCore #0*"}
}

if {$m_type == "MICROBLAZE"} {
}

dow sw/Release/sw.elf
con
disconnect
exit

