proc _file_is_xsa {} {
	if {[string first .hdf $::hw ] != -1} {
		return 0
	} else {
		return 1
	}
}

proc _get_processor {} {
	set proc_list [hsi::get_cells * -hier -filter {IP_TYPE==PROCESSOR}]

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
		if {[string first "_HEAP_SIZE = DEFINED(_HEAP_SIZE)" $line ] != -1} {
			puts $temp "_HEAP_SIZE = DEFINED(_HEAP_SIZE) ? _HEAP_SIZE : 0x100000;"
		} else {
			puts $temp $line
		}
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

# HSI-only project creation (Vitis 2025+ / Embedded Kit compatible)
proc _vitis_hsi_project {} {
	openhw $::hw
	set cpu [_get_processor]

	# Create bsp
	hsi::generate_bsp						\
		-dir bsp						\
		-proc $cpu						\
		-os standalone						\
		-compile

	# Create app directory structure
	file mkdir app/src

	# Generate linker script using the HSI linker generator API
	# (lg_init/lg_get_sections/lg_set_section_memory/lg_generate from
	#  $XILINX_VITIS/scripts/xsct/hsi/hsiutils.tcl)
	set mss_path [file normalize bsp/system.mss]
	set hw_path [file normalize $::hw]
	::hsi::utils::lg_init $hw_path $mss_path

	# Assign each section to an appropriate memory region
	# (replicates what 'lscript generate' does internally)
	set sections [::hsi::utils::lg_get_sections]
	dict for {sec_type value} $sections {
		foreach sec [split $value "."] {
			set parts [split $sec :]
			set name [lindex $parts 0]
			if {$name == ""} continue
			set size [lindex $parts 1]
			set assigned_mem [lindex $parts 2]
			if {$assigned_mem != ""} continue

			# Find the best memory for this section type
			set best_mem ""
			set best_size 0
			set mem_json [::hsi::utils::lg_get_memories -json]
			set mem_dict [::json::json2dict $mem_json]
			dict for {mkey mval} $mem_dict {
				# Check if this memory is valid for this section
				set mem_valid [lsearch $parts "*$mkey*"]
				if {$mem_valid != -1} {
					set msize [dict get $mval size]
					if {$best_mem == "" || $msize > $best_size} {
						set best_mem $mkey
						set best_size $msize
					}
				}
			}

			if {$best_mem != ""} {
				if {$sec_type == "stack_section" || $sec_type == "heap_section"} {
					::hsi::utils::lg_set_section_memory -sec $name -mem $best_mem -size $size
				} else {
					::hsi::utils::lg_set_section_memory -sec .$name -mem $best_mem -size $size
				}
			}
		}
	}

	::hsi::utils::lg_generate [file normalize app/src/lscript.ld]
	::hsi::utils::lg_delete

	# For Zynq (cortexa9), copy Xilinx.spec needed by the linker
	if {[string first "cortexa9" $cpu] != -1} {
		set vitis_dir $::env(XILINX_VITIS)
		# Try multiple possible locations for Xilinx.spec
		set spec_paths [list \
			[file normalize $vitis_dir/data/embeddedsw/scripts/specs/arm/Xilinx.spec] \
			[file normalize $vitis_dir/gnu/aarch32/lin/gcc-arm-none-eabi/arm-none-eabi/lib/Xilinx.spec] \
			[file normalize $vitis_dir/../data/embeddedsw/scripts/specs/arm/Xilinx.spec] \
		]
		set spec_src ""
		foreach path $spec_paths {
			if {[file exists $path]} {
				set spec_src $path
				break
			}
		}
		if {$spec_src != ""} {
			file copy -force $spec_src app/src/Xilinx.spec
		} else {
			puts "Warning: Xilinx.spec not found in any expected location, linker may fail"
			puts "Searched: $spec_paths"
		}
	}

	closehw $::hw

	# Increase heap size
	_replace_heap
}

# Classic Eclipse-based project creation (Vitis 2019-2024)
proc _vitis_classic_project {} {
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
		-template  $::template

	closehw $::hw

	# Increase heap size
	_replace_heap

	# Configure the project
	_project_config "app" "config"
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
		-app [string trimright $::template (C)]			\
		-bsp bsp

	closehw $::hw

	# Increase heap size
	_replace_heap

	# Configure the project
	_project_config "sdk" "configapp"
}

proc get_arch {} {
	openhw $::hw
	set cpu [_get_processor]
	set file [open [lindex "$::hw_path/arch.txt"] w+]
	puts $file $cpu
	close $file
	closehw $::hw
}

proc _get_vitis_year {} {
	if {[info exists ::env(XILINX_VITIS)]} {
		# Extract year from path like /tools/Xilinx/Vitis/2025.1
		regexp {(\d{4})\.\d+} $::env(XILINX_VITIS) match year
		if {[info exists year]} {
			return $year
		}
	}
	return 0
}

proc create_project {} {
	cd $::ws

	if {[_file_is_xsa] == 1} {
		set vitis_year [_get_vitis_year]
		# Use HSI-only mode for Vitis 2023+ to avoid Eclipse GUI issues in CI
		# The classic Eclipse-based approach fails silently in headless environments
		if {$vitis_year >= 2023} {
			_vitis_hsi_project
		} else {
			setws ./
			_vitis_classic_project
		}
	} else {
		setws ./
		_xsdk_project
	}
}

proc clean_build {} {
	cd $::ws

	set vitis_year [_get_vitis_year]
	if {$vitis_year >= 2025} {
		# Vitis 2025+: direct file cleanup (no Eclipse dependency)
		file delete -force app/src
		file mkdir app/src
	} else {
		# Vitis 2019-2024: Eclipse-based cleanup
		setws ./
		app clean -name app
	}
}

set pl_dict [dict create					\
	"ps7_cortexa9_0" "xc7z*"				\
	"psu_cortexa53_0" "PSU"					\
	"sys_cips_pspmc_0_psv_cortexa72_0" "Versal*"		\
	"sys_mb" "xc*"]

set ps_dict [dict create					\
	"ps7_cortexa9_0" "*Cortex-A9 MPCore #0*"		\
	"psu_cortexa53_0" "*Cortex-A53 #0*"			\
	"sys_cips_pspmc_0_psv_cortexa72_0" "*Cortex-A72 #0*"	\
	"sys_mb" "*MicroBlaze #*"]

proc _cpu_reset {cpu} {
	if {$cpu == "sys_mb"} {
		return
	}
	if {$cpu == "sys_cips_pspmc_0_psv_cortexa72_0"} {
		# Versal: PLM handles reset; just select the A72 core
		return
	}
	targets -set -filter {name =~ "APU*" && jtag_cable_name =~ "*$::jtagtarget*"}
	stop
	if {$cpu == "ps7_cortexa9_0"} {
		rst
	} elseif {$cpu == "psu_cortexa53_0"} {
		rst -system
	}
}

proc _write_pl_versal {pdi_path} {
	# Versal PDI programming requires Vivado hardware manager (cs_server).
	# xsct + hw_server alone cannot program Versal devices.
	set pdi [file normalize $pdi_path]
	set tmp_script "/tmp/_noos_versal_pdi_[pid].tcl"
	set fd [open $tmp_script w]
	puts $fd "open_hw_manager"
	puts $fd "connect_hw_server -url TCP:localhost:3121"
	puts $fd "open_hw_target"
	puts $fd "current_hw_device \[lindex \[get_hw_devices xcv*\] 0\]"
	puts $fd "set_property PROGRAM.FILE \{$pdi\} \[current_hw_device\]"
	puts $fd "program_hw_devices \[current_hw_device\]"
	puts $fd "close_hw_manager"
	puts $fd "exit"
	close $fd

	puts "INFO: Programming Versal PDI via Vivado..."
	set result ""
	if {[catch {set result [exec vivado -mode tcl -nolog -nojournal -source $tmp_script 2>@1]} err]} {
		set result $err
	}
	file delete -force $tmp_script
	if {[string first "Successfully programmed PDI" $result] == -1} {
		error "ERROR: Vivado PDI programming failed:\n$result"
	}
	puts "INFO: PDI programmed successfully. Waiting for PLM..."
	after 5000
}

proc _write_pl {cpu {bitstream}} {
	if {$cpu == "sys_cips_pspmc_0_psv_cortexa72_0"} {
		# Versal: handled separately in upload via _write_pl_versal
		return
	}
	set name [dict get $::pl_dict $cpu]
	targets -set -filter {name =~ "$name" && jtag_cable_name =~ "*$::jtagtarget*"}
	fpga -file [file normalize $bitstream]
}

proc _init_ps {cpu} {
	openhw $::hw
	switch $cpu {
		"ps7_cortexa9_0" {
			targets -set -filter {name =~ "APU*" && jtag_cable_name =~ "*$::jtagtarget*"}
			ps7_init
			ps7_post_config
		}

		"psu_cortexa53_0" {
			targets -set -nocase -filter {name =~ "*A53*#0" && jtag_cable_name =~ "*$::jtagtarget*"}
			rst -processor
			dow $::fsbl_file
			set bp_16_41_fsbl_bp [bpadd -addr &XFsbl_Exit]
			con -block -timeout 60
			bpremove $bp_16_41_fsbl_bp
			targets -set -filter {name =~ "Cortex-A53 #0" && jtag_cable_name =~ "*$::jtagtarget*"}
		}
		"psu_cortexr5_0" {
			targets -set -nocase -filter {name =~ "PSU" && jtag_cable_name =~ "*$::jtagtarget*"}
			psu_init
			after 1000
			psu_ps_pl_isolation_removal
			after 1000
			psu_ps_pl_reset_config
			targets -set -filter {name =~ "Cortex-R5 #0" && jtag_cable_name =~ "*$::jtagtarget*"}
		}
		"sys_cips_pspmc_0_psv_cortexa72_0" {
			# Versal: release the A72 from reset after PDI programming.
			targets -set -nocase -filter {name =~ "*A72*#0" && jtag_cable_name =~ "*$::jtagtarget*"}
			rst -processor -clear-registers
			after 2000
			# Versal requires forced memory access for dow since
			# the XSA memory map may not reflect the PDI runtime state.
			configparams force-mem-accesses 1
		}
		"sys_mb" {
			set name [dict get $::pl_dict $cpu]
			targets -set -filter {name =~ "$name" && jtag_cable_name =~ "*$::jtagtarget*"}
		}
	}
}

proc _write_ps {cpu} {
	set name [dict get $::ps_dict $cpu]
	targets -set -filter {name =~  "$name" && jtag_cable_name =~ "*$::jtagtarget*"}
	after 2000
	dow "[file normalize $::binary]"
	after 2000
	con
	disconnect
}

proc upload {} {
	openhw $::hw
	set cpu [_get_processor]

	if {$cpu == "sys_cips_pspmc_0_psv_cortexa72_0"} {
		# Versal flow: PDI via Vivado (requires cs_server), then
		# ELF download via xsct.
		set xsa_dir [file dirname [file normalize $::hw]]
		set pdi_path [file join $xsa_dir "system_top.pdi"]
		if {![file exists $pdi_path]} {
			exec unzip -o -q [file normalize $::hw] "*.pdi" -d $xsa_dir
		}

		# 1. Start hw_server (Vivado and xsct will both connect to it)
		catch {exec pkill -x hw_server}
		catch {exec pkill -x cs_server}
		after 2000
		exec hw_server &
		after 3000

		# 2. Program PDI via Vivado (auto-launches cs_server)
		_write_pl_versal $pdi_path

		# 3. Ensure cs_server is running for debug target enumeration
		if {[catch {exec pgrep -x cs_server}]} {
			puts "INFO: Starting cs_server..."
			exec cs_server &
			after 3000
		}

		# 4. Connect xsct to the running hw_server
		if { [info exists ::env(XSCT_REMOTE_HOST)] && [info exists ::env(XSCT_REMOTE_PORT)] } {
			connect -host "$::env(XSCT_REMOTE_HOST)" -port "$::env(XSCT_REMOTE_PORT)"
		} else {
			connect -host localhost -port 3121
		}
		after 5000

		# 5. Init PS and download ELF
		_init_ps $cpu
		_write_ps $cpu
	} else {
		set bitstream [file rootname $::hw]
		append bitstream ".bit"

		# Connect to the fpga
		if { [info exists ::env(XSCT_REMOTE_HOST)] && [info exists ::env(XSCT_REMOTE_PORT)] } {
			connect -host "$::env(XSCT_REMOTE_HOST)" -port "$::env(XSCT_REMOTE_PORT)"
		} else {
			connect
		}

		# Reset and stop the ARM CPU before we re-program the FPGA
		_cpu_reset $cpu
		_write_pl $cpu $bitstream
		_init_ps $cpu
		_write_ps $cpu
	}
}

set function	[lindex $argv 0]
set ws		[lindex $argv 1]
set hw_path	[lindex $argv 2]
set hw		$::hw_path/[lindex $argv 3]
set binary      [lindex $argv 4]
set target	[lindex $argv 5]
set template	[lindex $argv 6]
set fsbl_file   [lindex $argv 7]
set jtagtarget	[lindex $argv 8]

if {$target == 0} {set $target ""}

if {[file exists $::hw_path/ps7_init.tcl]} {
	source "[file normalize $::hw_path/ps7_init.tcl]"
} elseif {[file exists $::hw_path/psu_init.tcl]} {
	source "[file normalize $::hw_path/psu_init.tcl]"
}

$function
