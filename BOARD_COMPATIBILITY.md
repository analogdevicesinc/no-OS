# Board Compatibility Matrix

This document describes which boards (configure presets) are supported by each project in the no-OS repository.

## 🎯 Automatic Enforcement

Board compatibility is **automatically enforced** by CMake during configuration. If you try to build a project with an unsupported board, you'll get a clear error message with the compatibility matrix.

## 📋 Compatibility Matrix

| Project | Maxim Boards | STM32 Boards | Other Platforms |
|---------|-------------|-------------|-----------------|
| **iio_demo** | ✅ max78000_fthr<br/>✅ max32650_fthr<br/>✅ apard32690 | ✅ stm32f756_nucleo | ✅ aducm3029<br/>✅ pico<br/>✅ linux<br/>✅ xilinx |
| **eval-adxl355-pmdz** | ✅ max78000_fthr<br/>✅ max32650_fthr<br/>✅ apard32690 | ✅ stm32f756_nucleo | ✅ aducm3029<br/>✅ pico |
| **ftc_workshop** | ✅ max78000_fthr<br/>✅ max32650_fthr<br/>✅ apard32690 | ❌ stm32f756_nucleo | ❌ Not supported |

### Legend
- ✅ = Supported (has platform implementation)
- ❌ = Not supported (no platform implementation)

## 🚀 Usage Examples

### Valid Combinations
```bash
# IIO Demo (most compatible)
cmake --preset max78000_fthr -DPROJECT_NAME=iio_demo    ✅
cmake --preset stm32f756_nucleo -DPROJECT_NAME=iio_demo ✅

# ADXL355 PMDZ (Maxim + STM32 only)
cmake --preset max32650_fthr -DPROJECT_NAME=eval-adxl355-pmdz    ✅
cmake --preset stm32f756_nucleo -DPROJECT_NAME=eval-adxl355-pmdz ✅

# FTC Workshop (Maxim only)
cmake --preset apard32690 -DPROJECT_NAME=ftc_workshop ✅
```

### Invalid Combinations (Will Fail)
```bash
# FTC Workshop doesn't support STM32
cmake --preset stm32f756_nucleo -DPROJECT_NAME=ftc_workshop ❌

# Error message you'll see:
# ❌ Board compatibility error!
#    Project: ftc_workshop
#    Platform: stm32
#    Board: sdp-ck1z
#
# Project 'ftc_workshop' does not support platform 'stm32'.
# Supported platforms: maxim
```

## 🔍 How It Works

### 1. Preset Descriptions
Each build preset in `CMakePresets.json` shows supported platforms in its description:
```json
{
  "name": "ftc_workshop",
  "displayName": "FTC Workshop Project",
  "description": "Build FTC Workshop project (supports: maxim ONLY)"
}
```

### 2. CMake Enforcement
The root `CMakeLists.txt` contains a compatibility matrix that's checked during configuration:
```cmake
# Define compatibility matrix
set(PROJECT_SUPPORTED_PLATFORMS_iio_demo "maxim;stm32;aducm3029;pico;linux;xilinx")
set(PROJECT_SUPPORTED_PLATFORMS_eval-adxl355-pmdz "maxim;stm32;aducm3029;pico")
set(PROJECT_SUPPORTED_PLATFORMS_ftc_workshop "maxim")
```

### 3. Project Structure
Compatibility is determined by which platform directories exist in each project:
```
projects/
├── iio_demo/src/platform/
│   ├── maxim/     ✅
│   ├── stm32/     ✅
│   ├── aducm3029/ ✅
│   ├── pico/      ✅
│   ├── linux/     ✅
│   └── xilinx/    ✅
├── eval-adxl355-pmdz/src/platform/
│   ├── maxim/     ✅
│   ├── stm32/     ✅
│   ├── aducm3029/ ✅
│   └── pico/      ✅
└── ftc_workshop/src/platform/
    └── maxim/     ✅ (only)
```

## 🎨 Board Details

### Maxim Boards (Platform: maxim)
- **max78000_fthr**: MAX78000 feather board
- **max32650_fthr**: MAX32650 feather board
- **apard32690**: APARD32690 board

### STM32 Boards (Platform: stm32)
- **stm32f756_nucleo**: STM32F756 Nucleo board

### Other Platforms
- **aducm3029**: ADuCM3029 development board
- **pico**: Raspberry Pi Pico
- **linux**: Linux host builds
- **xilinx**: Xilinx FPGA platforms

## 🔧 For Project Developers

When adding a new project:

1. **Create platform directories** for each supported platform in `src/platform/`
2. **Update the compatibility matrix** in root `CMakeLists.txt`
3. **Update preset descriptions** to show supported platforms
4. **Test invalid combinations** to ensure proper error messages

When adding board support to existing projects:

1. **Create the platform directory** (e.g., `src/platform/stm32/`)
2. **Implement required files** (`main.c`, `parameters.c`, etc.)
3. **Update compatibility matrix** in `CMakeLists.txt`
4. **Test the new combination**

This ensures users get clear feedback about board support and prevents build failures due to missing platform implementations.