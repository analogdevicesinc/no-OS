# no-os-sample-project

Sample external project demonstrating out-of-tree no-OS builds with PROJECT_DEFCONFIG support.

## Usage

```bash
# Configure and build with default project.conf
cmake --preset apard32690
cmake --build build --target sample

# Configure with custom config file
cmake --preset apard32690 -DPROJECT_DEFCONFIG=custom.conf --fresh
cmake --build build --target sample
```

The project automatically uses `project.conf` if no `PROJECT_DEFCONFIG` is specified.