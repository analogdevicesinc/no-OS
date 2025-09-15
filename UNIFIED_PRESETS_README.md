# Unified Preset Workflow

This branch implements a unified preset system that works for both in-tree and out-of-tree builds with the same commands and directory structure.

## Quick Start

### In-Tree Builds (Current Repository)

```bash
# Build any project for any supported board
./build_project.sh max78000_fthr iio_demo
./build_project.sh max32650_fthr eval-adxl355-pmdz
./build_project.sh apard32690 ftc_workshop

# Or manually:
cmake -B build --preset max78000_fthr -DPROJECT_NAME=iio_demo
cmake --build build --preset iio_demo
```

### Out-of-Tree Builds

#### Option 1: With FetchContent (Automatic)
```bash
# In your project directory with CMakeLists.txt that uses FetchContent
cmake -B build --preset max78000_fthr-fetchcontent
cmake --build build --preset my_project
```

#### Option 2: With Local no-OS
```bash
# Set NO_OS_PATH to point to your local no-OS repository
export NO_OS_PATH=../no-os
cmake -B build --preset max78000_fthr-local
cmake --build build --preset my_project
```

## Key Features

### 🎯 Unified Commands
- Same preset names work for both in-tree and out-of-tree builds
- Auto-detection of build context (in-tree vs out-of-tree)
- Simplified command line interface

### 🏗️ Build Structure
- **Board presets**: Define hardware configuration (`max78000_fthr`, `max32650_fthr`, etc.)
- **Project presets**: Define what to build (`iio_demo`, `eval-adxl355-pmdz`, etc.)
- **Auto-detection**: CMake automatically sets correct paths based on context

### 🔧 Supported Boards
- `max78000_fthr` - MAX78000 feather board
- `max32650_fthr` - MAX32650 feather board
- `apard32690` - APARD32690 board
- `stm32f756_nucleo` - STM32F756 Nucleo board

### 📦 Supported Projects
- `iio_demo` - IIO Demo project
- `eval-adxl355-pmdz` - ADXL355 PMDZ project
- `ftc_workshop` - FTC Workshop project

## Migration Guide

### From Old Workflow
**Before:**
```bash
cmake --preset max78000_fthr -DPROJECT_DEFCONFIG=iio_demo/project.conf
cmake --build build --target iio_demo
```

**After:**
```bash
cmake --preset max78000_fthr -DPROJECT_NAME=iio_demo
cmake --build build --preset iio_demo
```

### For Out-of-Tree Projects
1. Copy `example-project-presets.json` to your project directory
2. Rename to `CMakePresets.json`
3. Modify preset names and targets as needed
4. Use appropriate preset based on your no-OS setup (FetchContent vs local)

## How It Works

The system auto-detects the build context:

1. **In-tree**: Detects `drivers/` directory in source root
   - Uses relative paths to toolchain and defconfig files
   - Projects located in `projects/` subdirectory

2. **Out-of-tree with FetchContent**: Detects `_deps/no-os-core-src/`
   - Uses paths relative to fetched content
   - Project files in current directory

3. **Out-of-tree with local**: Uses `NO_OS_PATH` environment variable
   - Uses absolute paths to local no-OS repository
   - Project files in current directory

## Files Added/Modified

- `CMakePresets.json` - Updated with unified presets
- `CMakeLists.txt` - Added auto-detection logic
- `build_project.sh` - Helper script for easy building
- `example-project-presets.json` - Template for out-of-tree projects
- `UNIFIED_PRESETS_README.md` - This documentation

## Testing

Test the workflow with:

```bash
# Test in-tree build
./build_project.sh max78000_fthr iio_demo

# Test preset listing
cmake --list-presets=configure
cmake --list-presets=build

# Test manual configuration
cmake -B test-build --preset max78000_fthr -DPROJECT_NAME=iio_demo
```