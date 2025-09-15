# Project-Specific Presets

Each project now includes its own `CMakePresets.json` file for out-of-tree builds. This allows projects to be easily extracted and used independently from the main no-OS repository.

## How It Works

### Auto-Detection
The project presets automatically detect the build context:

1. **FetchContent builds**: Downloads no-OS automatically
2. **Local no-OS builds**: Uses `NO_OS_PATH` environment variable
3. **In-tree builds**: Detects when project is in `projects/` subdirectory

### Project Files

Each project contains:
- `CMakePresets.json` - Preset definitions for out-of-tree builds
- `CMakeLists_standalone.txt` - Top-level CMake file for out-of-tree builds
- `project.conf` - Default project configuration

## Usage Examples

### In-Tree Builds (Current Workflow)
```bash
# From no-OS root directory
cmake --preset max78000_fthr -DPROJECT_NAME=iio_demo
cmake --build build --preset iio_demo

# With multiple targets (ADXL355)
cmake --preset max78000_fthr -DPROJECT_NAME=eval-adxl355-pmdz
cmake --build build --preset eval_adxl355_pmdz_iio
```

### Out-of-Tree Builds with FetchContent
```bash
# Copy project to separate directory
cp -r projects/iio_demo /path/to/my_project
cd /path/to/my_project

# Rename standalone CMakeLists.txt
mv CMakeLists_standalone.txt CMakeLists.txt

# Build (no-OS downloaded automatically)
cmake --preset max78000_fthr
cmake --build build --preset iio_demo
```

### Out-of-Tree Builds with Local no-OS
```bash
# Copy project to separate directory
cp -r projects/iio_demo /path/to/my_project
cd /path/to/my_project

# Rename standalone CMakeLists.txt
mv CMakeLists_standalone.txt CMakeLists.txt

# Point to local no-OS
export NO_OS_PATH=/path/to/no-os
cmake --preset max78000_fthr
cmake --build build --preset iio_demo
```

## Project-Specific Features

### IIO Demo
- Single target: `iio_demo`
- Build preset: `iio_demo`

### ADXL355 PMDZ
- Multiple targets:
  - `eval-adxl355-pmdz-dummy` → preset `dummy`
  - `eval-adxl355-pmdz-iio` → preset `iio`
  - `eval-adxl355-pmdz-iio-trigger` → preset `iio_trigger`
  - `eval-adxl355-pmdz` (all) → preset `all`

## Helper Script

The root `build_project.sh` script supports multiple targets:

```bash
# Build specific ADXL355 target
./build_project.sh max78000_fthr eval-adxl355-pmdz iio

# Use custom project config
./build_project.sh max78000_fthr eval-adxl355-pmdz iio custom.conf

# Build all ADXL355 targets
./build_project.sh max78000_fthr eval-adxl355-pmdz all
```

## Migration Guide

To extract a project for out-of-tree development:

1. **Copy project directory**:
   ```bash
   cp -r projects/my_project /separate/location/
   cd /separate/location/my_project
   ```

2. **Set up CMake**:
   ```bash
   mv CMakeLists_standalone.txt CMakeLists.txt
   ```

3. **Use presets directly**:
   ```bash
   cmake --list-presets=configure  # See available boards
   cmake --list-presets=build      # See available targets
   cmake --preset <board>
   cmake --build build --preset <target>
   ```

The presets handle all path configuration automatically - no manual setup required!