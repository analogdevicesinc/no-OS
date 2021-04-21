proc _file_is_xsa {} {
	if {[string first .hdf $::hw ] != -1} {
		return 0
	} else {
		return 1
	}
}

proc _get_processor {} {
	set proc_list [hsi::get_cells * -filter {IP_TYPE==PROCESSOR}]

	if {[llength $proc_list] == 0} {
		return 0
	}
	if {[llength $proc_list] > 1} {
		if { $::target eq ""} {
		return [lindex $proc_list 0]
		} else {
			foreach proc $proc_list {
				if {[string first $::target $proc] != -1} {
					return $proc
				}
			}
			puts "Warning: Target cpu unavailable"
			return [lindex $proc_list 0]
		}
	} else {
		return $proc_list
	}
}

proc _replace_heap {} {
	set file_name "$::ws/app/src/lscript.ld"
	set temp_name "$file_name.tmp"
	set file [open [lindex $file_name] r]
	set temp [open [lindex $temp_name] w+]

	while {[gets $file line] >= 0} {
		if {[string first _HEAP_SIZE $line ] != -1} {
			puts $temp "_HEAP_SIZE = 0x100000;"
		}
		puts $temp $line
	}

	close $file
	close $temp

	file rename -force $temp_name $file_name
}

proc _project_config {cmd {arg}} {
	if {$cmd == "sdk"} {
		set arg2 "app"
	} else {
		set arg2 "name"
	}

	set paths [lsort -unique  $::env(EXTRA_INC_PATHS)]

	foreach path $paths {
		set new_path [string map [list $::env(PROJECT_BUILD)	\
				  \${ProjDirPath}] $path]
		$cmd $arg -$arg2 app include-path $new_path
	}
	foreach symbol $::env(FLAGS_WITHOUT_D) {
		$cmd $arg -$arg2 app define-compiler-symbols $symbol
	}
	foreach lib $::env(EXTRA_LIBS_NAMES) {
		$cmd $arg -$arg2 app libraries $lib
	}
	foreach lib_path $::env(EXTRA_LIBS_PATHS) {
		$cmd $arg -$arg2 app library-search-path $lib_path
	}
}

proc _vitis_project {} {
	openhw $::hw
	set cpu [_get_processor]
	
	# Create bsp
	hsi::generate_bsp						\
		-dir bsp						\
		-proc $cpu						\
		-os standalone						\
		-compile
	
	# Create app
	app create							\
		-name app						\
		-hw $::hw						\
		-proc $cpu						\
		-os standalone						\
		-template  {Empty Application}

	closehw $::hw

	# Increase heap size
	_replace_heap

	# Configure the project
	_project_config "app" "config"

	# Build the project
	app build -name app
}

proc _xsdk_project {} {
	openhw $::hw
	set cpu [_get_processor]

	# # Create hwspec
	sdk createhw							\
		-name hw						\
		-hwspec $::hw
	# Create bsp
	sdk createbsp							\
		-name bsp						\
		-hwproject hw						\
		-proc $cpu						\
		-os standalone  
	# Create app
	sdk createapp							\
		-name app						\
		-hwproject hw						\
		-proc $cpu						\
		-os standalone						\
		-lang C							\
		-app {Empty Application}				\
		-bsp bsp
	
	closehw $::hw

	# Increase heap size
	_replace_heap

	# Configure the project
	_project_config "sdk" "configapp"

	# Build the project via SDK
	clean -type all
	build -type all

	projects -build -name app -type app
}

proc get_arch {} {
	openhw $::hw
	set cpu [_get_processor]
	set file [open [lindex "$::hw_path/arch.txt"] w+]
	puts $file $cpu
	close $file
	closehw $::hw
}

proc create_project {} {
	cd $::ws
	setws ./

	if {[_file_is_xsa] == 1} {
		_vitis_project
	} else {
		_xsdk_project
	}
}

set pl_dict [dict create					\
	"ps7_cortexa9_0" "xc7z*"				\
	"psu_cortexa53_0" "PSU"					\
	"sys_mb" "xc*"]

set ps_dict [dict create					\
	"ps7_cortexa9_0" "*Cortex-A9 MPCore #0*"		\
	"psu_cortexa53_0" "*Cortex-A53 #0*"			\
	"sys_mb" "*MicroBlaze #*"]

proc _cpu_reset {cpu} {
	targets -set -filter {name =~ "APU*"}
	stop
	if {$cpu == "ps7_cortexa9_0"} {
		rst 
	} elseif {$cpu == "psu_cortexa53_0"} {
		rst -system
	}
}

proc _write_pl {cpu {bitstream}} {
	set name [dict get $::pl_dict $cpu]
	targets -set -filter {name =~ "$name"}
	fpga -file [file normalize $bitstream]
}

proc _init_ps {cpu} {
	openhw $::hw
	switch $cpu {
		"ps7_cortexa9_0" {
			targets -set -filter {name =~ "APU*"}
			ps7_init
			ps7_post_config
		}

		"psu_cortexa53_0" {
			# Configure PSU
			targets -set -nocase -filter {name =~ "PSU"}
			psu_init
			psu_post_config
			psu_ps_pl_reset_config
			psu_ps_pl_isolation_removal
			# write bootloop and release A53-0 reset
			mwr 0xffff0000 0x14000000
			mwr 0xFD1A0104 0x380E
			targets -set -filter {name =~ "Cortex-A53 #0"}
		}
		"psu_cortexr5_0" {
			targets -set -nocase -filter {name =~ "PSU"}
			psu_init
			after 1000
			psu_ps_pl_isolation_removal
			after 1000
			psu_ps_pl_reset_config
			targets -set -filter {name =~ "Cortex-R5 #0"}
		}
		"sys_mb" {
			targets -set -filter {[dict get $::pl_dict $cpu]}

		}
	}
}

proc _write_ps {cpu} {
	set name [dict get $::ps_dict $cpu]
	targets -set -filter {name =~  "$name"}
	dow "[file normalize $::binary]"
	con
	disconnect
}

proc upload {} {
	openhw $::hw
	set bitstream $::hw_path/system_top.bit
	set cpu [_get_processor]

	# Connect to the fpga
	connect

	# Reset and stop the ARM CPU before we re-program the FPGA if we are on a ZYNQ.
	# Otherwise undefined behavior can occur.
	_cpu_reset $cpu

	_write_pl $cpu $bitstream

	_init_ps $cpu

	_write_ps $cpu
}

set function	[lindex $argv 0]
set ws		[lindex $argv 1]
set hw_path	[lindex $argv 2]
set hw		$::hw_path/[lindex $argv 3]
set binary      [lindex $argv 4]
set target	[lindex $argv 5]

if {[file exists $::hw_path/ps7_init.tcl]} {
	source "[file normalize $::hw_path/ps7_init.tcl]"
} elseif {[file exists $::hw_path/psu_init.tcl]} {
	source "[file normalize $::hw_path/psu_init.tcl]"
}

$function
