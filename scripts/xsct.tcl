# xsct tcl file

set m_cpu [lindex $argv 0]
set m_mode [lindex $argv 1]

sdk setws .

if {$m_mode eq "init"} {

  hsi open_hw_design [lindex $argv 2]
  sdk createhw -name hw -hwspec [lindex $argv 2]

  if {$m_cpu eq "ZYNQ_PS7"} {
    sdk createapp -name fsbl -hwproject hw -proc ps7_cortexa9_0 -app {Zynq FSBL}
    sdk createapp -name xilsw -hwproject hw -proc ps7_cortexa9_0 -bsp fsbl_bsp -app {Hello World}
    sdk createapp -name sw -hwproject hw -proc ps7_cortexa9_0 -bsp fsbl_bsp -app {Empty Application}
  }

  if {$m_cpu eq "ZYNQ_PSU"} {
    sdk createapp -name fsbl -hwproject hw -proc psu_cortexa53_0 -app {Zynq MP FSBL}
    sdk createapp -name xilsw -hwproject hw -proc psu_cortexa53_0 -bsp fsbl_bsp -app {Hello World}
    sdk createapp -name sw -hwproject hw -proc psu_cortexa53_0 -bsp fsbl_bsp -app {Empty Application}
  }

  if {$m_cpu eq "MICROBLAZE"} {
    sdk createapp -name xilsw -hwproject hw -proc sys_mb -bsp bsp -app {Hello World}
    sdk createapp -name sw -hwproject hw -proc sys_mb -bsp bsp -app {Empty Application}
  }

  exit
}

if {$m_mode eq "sources"} {

  for {set i 2} {$i < $argc} {incr i} {
    file copy -force [lindex $argv $i] sw/src/
  }

  exit
}

if {$m_mode eq "defines"} {

  sdk configapp -app sw build-config debug
  for {set i 2} {$i < $argc} {incr i} {
    sdk configapp -app sw define-compiler-symbols [lindex $argv $i]
  }

  sdk configapp -app sw build-config release
  for {set i 2} {$i < $argc} {incr i} {
    sdk configapp -app sw define-compiler-symbols [lindex $argv $i]
  }

  exit
}

if {$m_mode eq "make-bsp-xilsw"} {

  if {$m_cpu eq "ZYNQ_PS7"} {
    sdk configapp -app fsbl build-config release
    sdk configapp -app xilsw build-config release
    sdk projects -build -type bsp -name fsbl_bsp
    sdk projects -build -type app -name fsbl
    sdk projects -build -type app -name xilsw
    sdk configapp -app fsbl build-config debug
    sdk configapp -app xilsw build-config debug
    sdk projects -build -type bsp -name fsbl_bsp
    sdk projects -build -type app -name fsbl
    sdk projects -build -type app -name xilsw
  }

  if {$m_cpu eq "ZYNQ_PSU"} {
    sdk configapp -app fsbl build-config release
    sdk configapp -app xilsw build-config release
    sdk projects -build -type bsp -name fsbl_bsp
    sdk projects -build -type app -name fsbl
    sdk projects -build -type app -name xilsw
    sdk configapp -app fsbl build-config debug
    sdk configapp -app xilsw build-config debug
    sdk projects -build -type bsp -name fsbl_bsp
    sdk projects -build -type app -name fsbl
    sdk projects -build -type app -name xilsw
  }

  if {$m_cpu eq "MICROBLAZE"} {
    sdk configapp -app xilsw build-config release
    sdk projects -build -type bsp -name bsp
    sdk projects -build -type app -name xilsw
    sdk configapp -app xilsw build-config debug
    sdk projects -build -type bsp -name bsp
    sdk projects -build -type app -name xilsw
  }

  exit
}

sdk configapp -app sw build-config release
sdk projects -build -type app -name sw
sdk configapp -app sw build-config debug
sdk projects -build -type app -name sw

exit

