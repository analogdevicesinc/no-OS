# CI Incident Report: May 2026 OOM Kill and Recovery

## Executive Summary

On May 18, 2026, the no-OS CI server experienced memory exhaustion causing OOM kills on both self-hosted agents. This resulted in corrupted build state, orphaned processes, and failed Xilinx builds with `lscript.ld not found` errors. Recovery required server-side cleanup and code changes to make builds more resilient.

---

## Timeline

| Date | Event |
|------|-------|
| May 18 | OOM kill affects both CI agents |
| May 18 | GitHub runner `romlxvm2` goes offline |
| May 21 | Investigation begins, root cause identified |
| May 21 | Server cleanup performed, code fixes implemented |

---

## Affected Systems

### CI Agents

| Agent | Platform | Service Name | Impact |
|-------|----------|--------------|--------|
| `no-OS_xilinx` | Xilinx (Vitis/Vivado) | `vsts.agent.AnalogDevices.Default.no\x2dOS_xilinx.service` | Corrupted Eclipse workspace state |
| `no-OS` | STM32, Maxim, Mbed, Pico, ADuCM3029 | `vsts.agent.AnalogDevices.Default.no\x2dOS.service` | OOM killed, orphaned processes |
| `romlxvm2` | GitHub Actions | `actions.runner.adi-innersource-no-OS.romlxvm2.service` | Auto-deleted after 14 days offline |

### Build Infrastructure

- Build cache directory: `/no-OS_builds/builds_main/`
- Mixed ownership: some files `root:root`, some `aandrisa:no-OS`
- Stale lock files: `.project.target` present in build directories
- Corrupted metadata: `.metadata` directories with invalid Eclipse state

---

## Root Cause Analysis

### Primary Cause: Memory Exhaustion

Memory-leaked GUI processes from long-running X2GO remote desktop sessions consumed ~22GB of RAM:

| Process | Memory | Cause |
|---------|--------|-------|
| `mate-indicator-applet-complete` | 5-6 GB each | X2GO session memory leak |
| `arctica-greeter` | 6+ GB | Login screen memory leak |
| `x2goagent` | Variable | Stale remote sessions |

When builds started, insufficient memory remained, triggering OOM kills.

### Secondary Cause: Corrupted Build State

After OOM kill:
1. Eclipse/Java processes were killed mid-operation
2. `.metadata` directories left in inconsistent state
3. `.project.target` lock files remained, indicating "project exists"
4. Actual project files (`app/`, `bsp/`, `lscript.ld`) were missing or corrupted

### Tertiary Cause: Ownership Mismatch

Agent restarted as `root` instead of original user (`aandrisa`), creating mixed ownership that caused permission issues.

---

## Why Eclipse Failed Silently

The Xilinx build system uses Eclipse/Vitis for project creation. The failure mode is insidious:

```
Build Flow:
1. Check if .project.target exists
   └─ YES → Skip project creation, go directly to build
   └─ NO  → Create project (generates lscript.ld, Xilinx.spec, etc.)

2. Build the project
   └─ Requires lscript.ld to exist
```

**The Problem:**
- `.project.target` existed (lock file from previous build)
- Make skipped project creation
- But `app/src/lscript.ld` was missing (corrupted/deleted during OOM)
- Build failed with `lscript.ld not found`

**Why Eclipse Can't Recover:**
- Eclipse's `app create` relies on `.metadata` workspace state
- Corrupted `.metadata` makes Eclipse think project exists when it doesn't
- `_project_config` procedure fails: "The project given does not exist in workspace"
- Error is not always fatal, build continues, then fails later

---

## GitHub Runner Recovery (STM32/romlxvm2)

The GitHub runner required full re-registration because:
1. OOM kill stopped the runner service
2. After 14 days offline, GitHub auto-deleted the runner registration
3. Local config files still existed but were invalid

### Recovery Steps

```bash
# 1. Kill orphaned processes
ps aux | grep Xvfb
sudo kill <PID>

# 2. Uninstall old service
cd /no-OS-github-agent
sudo systemctl stop actions.runner.adi-innersource-no-OS.romlxvm2.service
sudo ./svc.sh uninstall
rm -f .runner .credentials .credentials_rsaparams

# 3. Re-register (get new token from GitHub UI)
./config.sh --url https://github.com/adi-innersource/no-OS \
            --token <NEW_TOKEN> \
            --labels romlxvm2 \
            --name romlxvm2

# 4. Reinstall and start
sudo ./svc.sh install
# Edit service file: ExecStart=/no-OS-github-agent/run_locally_with_display.sh, User=root
sudo systemctl daemon-reload
sudo systemctl start actions.runner.adi-innersource-no-OS.romlxvm2.service
```

---

## Azure DevOps Agent Recovery (Xilinx/no-OS)

```bash
# 1. Check service status
sudo systemctl status 'vsts.agent.AnalogDevices.Default.no\x2dOS.service'

# 2. Kill ALL orphaned processes (critical - old sessions block new ones)
sudo pkill -9 -f no-OS_agent
ps aux | grep Xvfb
sudo kill -9 <old_xvfb_pids>

# 3. Restart service
sudo systemctl start 'vsts.agent.AnalogDevices.Default.no\x2dOS.service'
```

**Note:** The hyphen in `no-OS` is escaped as `\x2d` in systemd service names.

---

## Xilinx Build State Recovery

```bash
# 1. Fix ownership
sudo chown -R root:root /no-OS_builds/builds_main/

# 2. Remove lock files (forces project recreation)
rm -f /no-OS_builds/builds_main/build_xilinx_*/.project.target

# 3. Clean corrupted directories
rm -rf /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil}

# 4. Verify cleanup
ls /no-OS_builds/builds_main/build_xilinx_*/.project.target 2>/dev/null || echo "Lock files clean"
```

---

## Code Changes Implemented

### 1. Pipeline Pre-Build Cleanup (`build_projects.yml`)

Added automatic cleanup before every Xilinx build to prevent corrupted state from affecting builds:

```yaml
# Clean stale Eclipse state and build locks
for dir in /no-OS_builds/builds_main/build_xilinx_*/; do
  if [ -d "$dir/.metadata" ]; then
    rm -rf "$dir/.metadata"
  fi
  if [ -f "$dir/.project.target" ]; then
    rm -f "$dir/.project.target"
  fi
done
```

**Important: Light Cleanup vs Full Cleanup**

This pipeline cleanup is intentionally **light** - it only removes:
- `.metadata` - Eclipse workspace state (prone to corruption)
- `.project.target` - Lock file that skips project creation

It does **NOT** remove:
- `app/` - Application project directory
- `bsp/` - Board Support Package (expensive to regenerate)
- `tmp/` - Temporary files including `arch.txt` (tracks hardware version)

This means BSP caching still works. The build script (`build_projects.py`) compares the current hardware (XSA) with what was used before:

```
-> Same hardware from last build, use existing bsp
-> make update
-> make clean
-> make all
```

Removing `.project.target` causes the project target to re-run, but the TCL scripts check if artifacts already exist before recreating them. The BSP regeneration (which takes significant time) is only triggered when hardware actually changes.

**Full cleanup** (for truly corrupted state) removes everything:
```bash
rm -rf /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil}
```

Use full cleanup only when the build cache is completely corrupted and needs to be rebuilt from scratch.

### 2. HSI Fallback for Linker Script (`util.tcl`)

Eclipse project creation can fail silently. Added HSI (Hardware Software Interface) fallback to generate `lscript.ld` directly:

```tcl
# HSI bypasses Eclipse entirely - more reliable for CI
proc generate_linker_script_hsi {xsa_file proc_name output_dir} {
    hsi::open_hw_design $xsa_file
    set proc [hsi::get_cells -filter "IP_TYPE==PROCESSOR" $proc_name]
    hsi::generate_target -force {sw bsp linker} $proc -dir $output_dir
    hsi::close_hw_design [hsi::current_hw_design]
}
```

**Why HSI over Eclipse:**
- No workspace state dependency
- No `.metadata` corruption risk
- Direct hardware-to-linker-script generation
- More predictable in automated environments

### 2.1. Implementation suggestion for HSI fallback

```tcl

# HSI fallback: generate lscript.ld when Eclipse app create fails
# in tools/scripts/platform/xilinx/util.tcl
proc _generate_lscript_hsi {cpu} {
	puts ">>> HSI FALLBACK: Generating lscript.ld using HSI API <<<"

	# Create app directory structure
	file mkdir app/src

	# Generate linker script using the HSI linker generator API
	set mss_path [file normalize bsp/system.mss]
	set hw_path [file normalize $::hw]
	puts ">>> HSI: Initializing linker generator with hw=$hw_path mss=$mss_path"
	::hsi::utils::lg_init $hw_path $mss_path

	# Assign each section to an appropriate memory region
	set sections [::hsi::utils::lg_get_sections]
	dict for {sec_type value} $sections {
		foreach sec [split $value "."] {
			set parts [split $sec :]
			set name [lindex $parts 0]
			if {$name == ""} continue
			set size [lindex $parts 1]
			set assigned_mem [lindex $parts 2]
			if {$assigned_mem != ""} continue

			set best_mem ""
			set best_size 0
			set mem_json [::hsi::utils::lg_get_memories -json]
			set mem_dict [::json::json2dict $mem_json]
			dict for {mkey mval} $mem_dict {
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
	puts ">>> HSI: Generated lscript.ld successfully"

	# For Zynq (cortexa9), copy Xilinx.spec needed by the linker
	if {[string first "cortexa9" $cpu] != -1} {
		set vitis_dir $::env(XILINX_VITIS)
		set spec_src [file normalize $vitis_dir/data/embeddedsw/scripts/specs/arm/Xilinx.spec]
		puts ">>> HSI: Copying Xilinx.spec from $spec_src"
		if {[file exists $spec_src]} {
			file copy -force $spec_src app/src/Xilinx.spec
			puts ">>> HSI: Xilinx.spec copied successfully"
		} else {
			puts ">>> HSI WARNING: Xilinx.spec not found at $spec_src"
		}
	}
}

```

### 3. Multi-Path Xilinx.spec Search (`util.tcl`)

Zynq (cortexa9) builds require `Xilinx.spec` for linking. Added search across multiple possible locations:

```tcl
set spec_paths [list \
    "$vitis_dir/data/embeddedsw/scripts/specs/arm/Xilinx.spec" \
    "$vitis_dir/../data/embeddedsw/scripts/specs/arm/Xilinx.spec" \
    "$vitis_dir/gnu/aarch32/lin/gcc-arm-none-eabi/arm-xilinx-eabi/lib/Xilinx.spec" \
]
```

### 4. Debug Output

Added debug lines to trace file locations in CI logs:

```tcl
puts "DEBUG: XILINX_VITIS = $vitis_dir"
puts "DEBUG: Checking $spec_norm"
puts "DEBUG: Found Xilinx.spec at $spec_norm"
```

---

## Key Files Reference

| Path | Purpose |
|------|---------|
| `$BUILD_DIR/.project.target` | Lock file - if exists, project creation is skipped |
| `$BUILD_DIR/tmp/arch.txt` | Architecture info - triggers hardware evaluation |
| `$BUILD_DIR/app/src/lscript.ld` | Linker script - required for build |
| `$BUILD_DIR/app/src/Xilinx.spec` | GCC spec file - required for Zynq only |
| `$BUILD_DIR/.metadata/` | Eclipse workspace state - prone to corruption |
| `$BUILD_DIR/bsp/` | Board Support Package |

---

## Prevention Measures

### 1. Memory Management

Add memory limits to CI agent systemd services:

```ini
[Service]
MemoryMax=80%
MemoryHigh=70%
```

### 2. Session Management

- Encourage proper logout from remote desktop sessions
- Set idle timeouts for X2GO connections
- Schedule periodic cleanup of stale sessions

### 3. Build Resilience

- Pre-build cleanup of `.metadata` and `.project.target`
- HSI fallback for linker script generation
- Debug output for troubleshooting

### 4. Monitoring

- Monitor server memory usage proactively
- Alert before OOM threshold is reached
- Track orphaned processes

---

## Lessons Learned

1. **Lock files are dangerous** - `.project.target` assumes valid state without verification
2. **Eclipse is fragile in CI** - Workspace state corruption causes silent failures
3. **OOM kills leave orphans** - Always check for and kill orphaned processes
4. **Mixed ownership breaks builds** - Agent restart as different user causes permission issues
5. **GUI processes leak memory** - Long-running remote desktop sessions are a liability on CI servers
6. **HSI is more reliable** - Lower-level Xilinx APIs are more suitable for automation than Eclipse

---

## Resolution

**The server-side cleanup alone was sufficient to fix the issue.**

After performing the cleanup steps below, builds from other branches (without any code changes) started working immediately:

```bash
# 1. Fix ownership
sudo chown -R root:root /no-OS_builds/builds_main/

# 2. Remove lock files (forces project recreation)
rm -f /no-OS_builds/builds_main/build_xilinx_*/.project.target

# 3. Clean corrupted directories
rm -rf /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil}
```

This confirms the root cause was **purely server state corruption** from the OOM kill - not a code issue in the build scripts.

### What Was Required vs Preventive

| Change | Status | Purpose |
|--------|--------|---------|
| Server cleanup (ownership + lock files) | **Required** | Fixed the immediate issue |
| Pipeline pre-build cleanup | Preventive | Guards against future corruption |
| HSI fallback for linker script | Preventive | Not triggered - Eclipse worked after cleanup |
| Multi-path Xilinx.spec search | Not needed | File exists in default Vitis location |

The code changes developed during investigation are **preventive measures** that can help avoid similar issues in the future, but were not required to resolve this incident.

---

## Related Documentation

- [TROUBLESHOOTING.md](./TROUBLESHOOTING.md) - Detailed troubleshooting steps
- [how-to-fix-runner.md](../../../how-to-fix-runner.md) - GitHub runner recovery guide
