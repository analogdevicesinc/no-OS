# Xilinx Build Troubleshooting

## Error: lscript.ld not found / No rule to make target lscript.ld

### Symptoms

Build fails with one of these errors:

```
couldn't open "/path/to/build_xilinx_project/app/src/lscript.ld": no such file or directory
```

Or:

```
make[1]: *** No rule to make target '/path/to/build_xilinx_project/app/src/lscript.ld', needed by '*.elf'.  Stop.
```

The `app/src/` directory may exist but be empty, or the project creation step is skipped entirely.

### Root Cause

This issue occurs when the build system state is corrupted. Common causes:

1. **OOM kill** - The CI agent was killed due to memory exhaustion, leaving orphaned processes and corrupted workspace state
2. **Mixed file ownership** - Build directories have mixed ownership (e.g., some `root:root`, some `user:group`) after agent configuration changes
3. **Stale Eclipse metadata** - Corrupted `.metadata` directories from interrupted builds
4. **Build lock file present** - `.project.target` file exists, causing make to skip project creation even though project files are missing

### Diagnosis

1. Check for mixed ownership:
   ```bash
   ls -la /no-OS_builds/builds_main/build_xilinx_*/ | head -30
   ```

2. Check for stale lock files:
   ```bash
   ls /no-OS_builds/builds_main/build_xilinx_*/.project.target 2>/dev/null
   ```

3. Check for corrupted directories:
   ```bash
   ls -d /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil} 2>/dev/null
   ```

4. Verify the build shows "Creating and configuring the IDE project" in output. If missing, project creation is being skipped.

### Solution

#### Full cleanup procedure:

```bash
# 1. Fix ownership (use the user the CI agent runs as)
sudo chown -R root:root /no-OS_builds/builds_main/

# 2. Remove build lock files (forces project recreation)
rm -f /no-OS_builds/builds_main/build_xilinx_*/.project.target

# 3. Clean corrupted build directories
rm -rf /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil}

# 4. Verify everything is clean
ls /no-OS_builds/builds_main/build_xilinx_*/.project.target 2>/dev/null || echo "Lock files clean"
ls -d /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil} 2>/dev/null || echo "Dirs clean"
```

#### Understanding the build flow:

1. `.project.target` is a lock file in `$(BUILD_DIR)/`
2. If it exists, make skips the `project` step and goes directly to `build`
3. The `project` step creates `app/`, `bsp/`, `tmp/` directories and generates `lscript.ld`
4. If lock exists but project files are missing/corrupted, build fails

### Prevention

For CI pipelines, add a pre-build cleanup step:

```bash
# Clean lock files and stale metadata
rm -f $BUILD_DIR/.project.target
rm -rf $BUILD_DIR/.metadata
```

---

## Error: Xilinx.spec not found

### Symptom

```
arm-xilinx-eabi-gcc.real: fatal error: cannot read spec file '.../app/src/Xilinx.spec': No such file or directory
```

### Root Cause

For Zynq (cortexa9) targets, `Xilinx.spec` is required by the linker. Eclipse/Vitis should copy this file during project creation, but it may fail silently.

### Solution

The file should be at: `$XILINX_VITIS/../data/embeddedsw/scripts/specs/arm/Xilinx.spec`

If project creation failed to copy it, manually copy or force a full project rebuild by removing `.project.target`.

---

## Error: The project given does not exist in workspace

### Symptom

```
The project given does not exist in workspace.
    while executing
"xsdk_eval ..."
    (procedure "_project_config" ...)
```

### Root Cause

Eclipse's `app create` command failed silently (often due to corrupted `.metadata` directory), but the build continued. When `_project_config` tries to configure the Eclipse project, it fails because no valid project exists.

### Solution

Clean the `.metadata` directories and force project recreation:

```bash
rm -rf /no-OS_builds/builds_main/build_xilinx_*/.metadata
rm -f /no-OS_builds/builds_main/build_xilinx_*/.project.target
```

---

## CI Agent Architecture

The no-OS CI uses two self-hosted agents:

| Agent | Platforms | Service Name |
|-------|-----------|--------------|
| `no-OS_xilinx` | Xilinx | `vsts.agent.AnalogDevices.Default.no\x2dOS_xilinx.service` |
| `no-OS` | STM32, Maxim, Mbed, Pico, ADuCM3029 | `vsts.agent.AnalogDevices.Default.no\x2dOS.service` |

Both agents run on the same server and can be affected by the same issues (OOM, orphaned processes, ownership changes).

---

## CI Agent Recovery

### OOM Kill Recovery

If the CI agent was OOM killed:

1. Check for orphaned processes:
   ```bash
   ps aux | grep -E 'eclipse|java|xsct' | grep -v grep
   ```

2. Kill any orphans:
   ```bash
   sudo pkill -9 -f eclipse
   sudo pkill -9 -f 'java.*eclipse'
   ```

3. Clean build directories (see above)

4. Restart the agent service if needed

### Memory Prevention

Add memory limits to the agent systemd service:

```ini
[Service]
MemoryMax=80%
MemoryHigh=70%
```

---

## May 2026 Incident Summary

### What Happened

On May 18, 2026, the CI server experienced memory exhaustion causing OOM kills:

1. **OOM Kill** - Both CI agents were killed due to memory exhaustion
2. **Orphaned Processes** - Eclipse/Java processes from Xilinx builds were left running
3. **Ownership Change** - The agent restarted as `root` instead of original user (`aandrisa`)
4. **Corrupted State** - Build directories had mixed ownership and corrupted Eclipse metadata
5. **Build Failures** - All Xilinx builds failed with `lscript.ld not found` errors

### Root Cause

Memory-leaked GUI processes (mate-indicator-applet, arctica-greeter from X2GO sessions) consumed ~22GB of RAM, leaving insufficient memory for builds.

### Resolution

1. Killed memory-hogging GUI processes
2. Fixed ownership: `sudo chown -R root:root /no-OS_builds/builds_main/`
3. Cleaned corrupted build state:
   ```bash
   rm -f /no-OS_builds/builds_main/build_xilinx_*/.project.target
   rm -rf /no-OS_builds/builds_main/build_xilinx_*/{app,bsp,tmp,.metadata,.Xil}
   ```
4. Retriggered CI builds

### Lessons Learned

- Monitor server memory usage proactively
- Set memory limits on systemd services
- Encourage proper logout from remote desktop sessions
- The `.project.target` lock file is critical - if present but project is corrupted, builds fail silently

---

## Key Files and Directories

| Path | Purpose |
|------|---------|
| `$BUILD_DIR/.project.target` | Lock file - if exists, project creation is skipped |
| `$BUILD_DIR/tmp/arch.txt` | Architecture info - triggers hardware evaluation |
| `$BUILD_DIR/app/` | Application project directory |
| `$BUILD_DIR/app/src/lscript.ld` | Linker script (required for build) |
| `$BUILD_DIR/app/src/Xilinx.spec` | Linker spec file (Zynq only) |
| `$BUILD_DIR/bsp/` | Board Support Package |
| `$BUILD_DIR/.metadata/` | Eclipse workspace state |
| `$BUILD_DIR/.Xil/` | Xilinx tool temporary files |
