# xsdb tcl file

set m_type [lindex $argv 0]

connect

# Reset and stop the ARM CPU before we re-program the FPGA if we are on a ZYNQ.
# Otherwise undefined behavior can occur.
if {[string match "ZYNQ_*" $m_type]} {
  targets -set -filter {name =~ "APU*"}
  stop
  rst
}

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "PSU"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "xc7z*"}
}

if {$m_type == "MICROBLAZE"} {
  targets -set -filter {name =~ "xc7*" || name =~ "xck*" }
}

fpga -file hw/system_top.bit

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "APU*"}
  source hw/psu_init.tcl
  psu_init
  psu_post_config
  psu_ps_pl_isolation_removal
  psu_ps_pl_reset_config
  mwr 0xffff0000 0x14000000                                                                                         
  mwr 0xFD1A0104 0x380E                                                                                             
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "APU*"}
  source hw/ps7_init.tcl
  ps7_init
  ps7_post_config
}

if {$m_type == "ZYNQ_PSU"} {
  targets -set -filter {name =~ "*Cortex-A53 #0*"}
}

if {$m_type == "ZYNQ_PS7"} {
  targets -set -filter {name =~ "*Cortex-A9 MPCore #0*"}
}

if {$m_type == "MICROBLAZE"} {
  after 1000
  targets -set -filter {name =~ "*MicroBlaze #*"}
}

dow sw/Release/sw.elf
con
disconnect
exit

