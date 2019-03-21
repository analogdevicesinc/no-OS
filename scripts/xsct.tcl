# xsct tcl file

set m_mode [lindex $argv 0]
sdk setws .

if {$m_mode == "init"} {

  set hdf_file [lindex $argv 1]
  hsi open_hw_design $hdf_file
  set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]

  sdk createhw -name hw -hwspec $hdf_file
  sdk createapp -name xilsw -hwproject hw -proc $cpu_name -bsp bsp -app {Hello World}
  sdk createapp -name sw -hwproject hw -proc $cpu_name -bsp bsp -app {Empty Application}
  exit
}

if {$m_mode == "sources"} {

  for {set i 1} {$i < $argc} {incr i} {
    file copy -force [lindex $argv $i] sw/src/
  }
  exit
}
if {$m_mode == "library"} {

  for {set i 1} {$i < $argc} {incr i} {
    setlib -lib [lindex $argv $i]
  }
  exit
}

if {$m_mode == "defines"} {

  sdk configapp -app sw build-config debug
  for {set i 2} {$i < $argc} {incr i} {
    set compiler_define [lindex $argv $i]
    if { [regexp [ 0-9] $compiler_define ] } {
	regsub -all { } $compiler_define {=} compiler_define
    }
    sdk configapp -app sw define-compiler-symbols $compiler_define
  }
  sdk configapp -app sw build-config release
  for {set i 2} {$i < $argc} {incr i} {
    set compiler_define [lindex $argv $i]
    if { [regexp [ 0-9] $compiler_define ] } {
	regsub -all { } $compiler_define {=} compiler_define
    }
    sdk configapp -app sw define-compiler-symbols $compiler_define
  }

  set hdf_file [lindex $argv 1]
  hsi open_hw_design $hdf_file
  set cpu_name [lindex [hsi get_cells -filter {IP_TYPE==PROCESSOR}] 0]
  if { $cpu_name == "sys_mb" } {	  
	  sdk configapp -app sw -set linker-misc {-Xlinker --defsym=_HEAP_SIZE=0x2000 -Xlinker --defsym=_STACK_SIZE=0x2000}
  }
  exit
}

if {$m_mode == "make-bsp-xilsw"} {

  sdk projects -build -type bsp -name bsp
  sdk projects -build -type app -name xilsw
  exit
}

sdk projects -build -type app -name sw
exit

