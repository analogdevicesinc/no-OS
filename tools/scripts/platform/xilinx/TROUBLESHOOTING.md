# Xilinx Build Troubleshooting

## Error: lscript.ld not found

### Symptom

Build fails with an error similar to:

```
couldn't open "/path/to/build_xilinx_project/app/src/lscript.ld": no such file or directory
    while executing
"open [lindex $file_name] r"
    (procedure "_replace_heap" line 4)
    invoked from within
"_replace_heap"
```

The `app/src/` directory exists but is empty - no linker script was generated.

### Root Cause

The Vitis/Eclipse `app create` command fails silently when there is a stale or corrupted
`.metadata` directory in the build workspace. Eclipse uses this directory to store workspace
state, and issues can occur when:

- A previous build was interrupted or failed mid-execution
- The `.metadata/.lock` file was not properly released
- The build directory ownership changed (e.g., switching between users or CI agents)
- Eclipse/GTK environment issues on headless servers

When `app create` fails, it does not generate the linker script (`lscript.ld`), causing
the subsequent `_replace_heap` procedure to fail.

### Diagnosis

1. Check if the `.metadata` directory exists in the build directory:
   ```bash
   ls -la $BUILD_DIR/.metadata/
   ```

2. Look for a stale `.lock` file or old timestamps:
   ```bash
   ls -la $BUILD_DIR/.metadata/.lock
   cat $BUILD_DIR/.metadata/.log | tail -50
   ```

3. Check directory ownership - mixed ownership can cause issues:
   ```bash
   ls -la $BUILD_DIR/
   ```

### Solution

Remove the stale `.metadata` directory and rebuild:

```bash
rm -rf $BUILD_DIR/.metadata
```

For bulk cleanup on CI/build servers:

```bash
cd /path/to/builds/
for dir in build_xilinx_*/; do
    if [ -d "$dir/.metadata" ]; then
        echo "Removing $dir/.metadata"
        rm -rf "$dir/.metadata"
    fi
done
```

### Prevention

For CI pipelines, consider adding a cleanup step before Xilinx builds:

```bash
rm -rf $BUILD_DIR/.metadata
```

This ensures each build starts with a fresh Eclipse workspace state.
