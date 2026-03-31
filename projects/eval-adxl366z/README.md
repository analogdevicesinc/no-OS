# EVAL-ADXL366Z Project

_⭐ indicates current milestone_

## Overview
Evaluation project for the ADXL366 ultra-low-power 3-axis accelerometer, demonstrating driver integration across multiple platforms.

**Primary Goal:** Contribute ADXL366 driver to upstream no-OS repository (deadline: March 31st, 2026)

## Multi-Platform Strategy: A→B→C

### Phase A: MAX32655 Testing (✅ COMPLETE - March 30th, 2026)
**Goal:** Test driver on working platform and submit upstream PR before deadline

**Hardware:**
- MAX32655 FTHR board
- ADXL366Z evaluation board

**Why first:**
- MAX32655 platform already mature in no-OS
- Unblocks driver contribution immediately
- No platform initialization issues

**Status:** ✅ COMPLETE - All tests passed on hardware
- Basic example: accelerometer data streaming ✅
- Comprehensive test: all 14 driver functions validated ✅
- Ready for upstream PR submission

---

### Phase B: nRF54L15 with Nordic SDK (NEXT - after PR submitted)
**Goal:** Get practical nRF testing environment working

**Hardware:**
- nRF54L15 Development Kit
- ADXL366Z evaluation board

**Approach:**
- Use Nordic SDK initialization (like working UART example)
- Keep ADXL366 driver platform-agnostic
- Eval project handles Nordic-specific setup

**Status:** Pending MAX32655 PR submission

---

### Phase C: Bare-metal nRF Platform (Required for project completion)
**Goal:** Proper bare-metal no-OS platform layer for nRF54L15

**Tasks:**
- Implement power domain/clock initialization
- Make nRF platform adapters work without Nordic SDK
- Critical for BioLinq and other projects

**Status:** Required after Phase B - all phases must complete sequentially

---

## ADXL366 Driver Features

### ✅ Phase 1: Core Features (COMPLETE - Hardware Validated)
All features tested on MAX32655 FTHR + ADXL366Z hardware (March 30th, 2026):

**Configuration:**
- Range configuration (±2g, ±4g, ±8g) ✅
- Output Data Rate (ODR) configuration (12.5Hz - 400Hz) ✅
- Power mode control (standby/measurement) ✅
- Software reset ✅

**Data Acquisition:**
- 14-bit accelerometer data (X, Y, Z axes) ✅
- Temperature sensor reading ✅
- Status register reading ✅

**Low-Level Operations:**
- Device initialization and ID verification ✅
- SPI register read/write/multi-byte operations ✅

**Total: 14 driver functions implemented and tested**

### 📋 Phase 2: Advanced Features (Future)
- FIFO buffer management (read, configure, watermark)
- Activity/inactivity detection
- Interrupt configuration and mapping
- Self-test functionality

---

## Project Structure

```
eval-adxl366z/
├── Makefile                      # Build configuration
├── src.mk                        # Source file definitions
├── README.md                     # This file
├── src/
│   ├── common/                   # Platform-agnostic initialization
│   │   ├── common_data.c         # ADXL366 parameters
│   │   └── common_data.h
│   ├── examples/                 # Example applications
│   │   ├── basic/                # Basic accelerometer reading
│   │   │   ├── basic_example.c
│   │   │   └── basic_example.h
│   │   ├── comprehensive/        # Full driver feature test
│   │   │   ├── comprehensive_example.c
│   │   │   └── comprehensive_example.h
│   │   └── examples_src.mk
│   └── platform/
│       └── maxim/                # MAX32655 platform (TESTED ✅)
│           ├── main.c
│           ├── parameters.c
│           ├── parameters.h
│           └── platform_src.mk
```

---

## Building

### Prerequisites
- Set `MAXIM_LIBRARIES` environment variable to SDK path
- For MAX32655: Analog Devices CFS 2.1.0 or later

### Build Commands

**Basic Example** (continuous accelerometer reading):
```bash
export MAXIM_LIBRARIES=/path/to/cfs/x.x.x/SDK/MAX/Libraries
cd projects/eval-adxl366z
make PLATFORM=maxim TARGET=max32655 BASIC_EXAMPLE=y
```

**Comprehensive Test** (validates all 14 driver functions):
```bash
export MAXIM_LIBRARIES=/path/to/cfs/x.x.x/SDK/MAX/Libraries
cd projects/eval-adxl366z
make PLATFORM=maxim TARGET=max32655 COMPREHENSIVE_EXAMPLE=y
```

The example selection is also configured in the Makefile.

---

## Flashing and Running

### MAX32655 FTHR

1. Connect MAX32655 FTHR to PC via USB (powers board and provides debug/UART)
2. Build the project (see Building section above)
3. Flash using OpenOCD:
```bash
openocd -f interface/cmsis-dap.cfg -f target/max32655.cfg \
  -c "program build/eval-adxl366z.elf verify reset exit"
```
4. Connect to serial console (115200 baud, 8N1):
```bash
screen /dev/ttyACM0 115200
# or
minicom -D /dev/ttyACM0 -b 115200
```

---

## Hardware Connections

### MAX32655 FTHR ↔ ADXL366Z
```
MAX32655 FTHR          ADXL366Z Eval Board
─────────────────      ────────────────────
3.3V          ────────> VS
3.3V          ────────> VDDIO
GND           ────────> GND
P0.20 (SS0)   ────────> CS
P0.23 (SCK)   ────────> SCLK
P0.21 (MOSI)  ────────> MOSI
P0.22 (MISO)  <───────  MISO
```

**Note**: Using QSPI1 peripheral (device_id=0 maps to MXC_SPI1 in SDK)

### nRF54L15 DK ↔ ADXL366Z (Future)
```
nRF54L15 DK            ADXL366Z Eval Board
─────────────────      ────────────────────
3.3V          ────────> VS
3.3V          ────────> VDDIO
GND           ────────> GND
P2.05 (CS)    ────────> CS
P2.01 (CLK)   ────────> SCLK
P2.02 (MOSI)  ────────> MOSI
P2.04 (MISO)  <───────  MISO
```

---

## Expected Output

### Basic Example
```
ADXL366 Basic Example
=====================

Initializing ADXL366...
Device IDs: 0xAD 0x1D 0xF7
ADXL366 initialized successfully!

Reading accelerometer data...
(Press Ctrl+C to stop)

X: -0.5680 g  Y: -0.2586 g  Z: +0.7276 g
X: -0.5685 g  Y: -0.2589 g  Z: +0.7259 g
X: -0.5702 g  Y: -0.2630 g  Z: +0.7296 g
[Updates every 500ms...]
```

### Comprehensive Test (All 14 Functions)
```
===================================
ADXL366 Comprehensive Test Example
Testing All 14 Driver Functions
===================================

Function 1/14: adxl366_init()
PASS: ADXL366 initialized successfully!

Function 2/14: adxl366_reg_read() - tested in Test 1

=== Test 1: Device ID Verification ===
Device IDs: 0xAD 0x1D 0xF7
PASS: Device ID verification

Functions 3-4/14: adxl366_set_range(), adxl366_get_range()

=== Test 2: Range Configuration ===
Set ±2g, Read back: 0
Set ±4g, Read back: 1
Set ±8g, Read back: 2
PASS: Range configuration

Functions 5-6/14: adxl366_set_odr(), adxl366_get_odr()

=== Test 3: ODR Configuration ===
Set 12.5Hz, Read back: 0
Set 50Hz, Read back: 2
Set 100Hz, Read back: 3
Set 400Hz, Read back: 5
PASS: ODR configuration

Functions 7-8/14: adxl366_read_temperature(), adxl366_reg_read_multiple()

=== Test 4: Temperature Reading ===
Temperature: 25.00 °C
PASS: Temperature reading

Function 9/14: adxl366_read_accel()

=== Test 5: Accelerometer Data Reading ===
Sample 1: X=-1.0175 g, Y=-0.5671 g, Z=+1.4279 g
Sample 2: X=-1.1092 g, Y=-0.4934 g, Z=+1.4528 g
Sample 3: X=-1.1019 g, Y=-0.5231 g, Z=+1.4367 g
Sample 4: X=-1.0819 g, Y=-0.5168 g, Z=+1.4406 g
Sample 5: X=-1.0965 g, Y=-0.5022 g, Z=+1.4435 g
PASS: Accelerometer data reading

Function 10/14: adxl366_get_status()

=== Test 6: Status Register ===
Status: 0x41
  DATA_RDY:     1
  FIFO_RDY:     0
  FIFO_WM:      0
  FIFO_OVR:     0
  ACT:          0
  INACT:        0
  AWAKE:        1
PASS: Status register reading

Function 11/14: adxl366_reg_write()

=== Test 7: Register Write ===
Write: 0x52, Read back: 0x00 (expected 0x00 after reset)
PASS: Register write

Function 12/14: adxl366_reg_read_multiple() - explicit test

=== Test 8: Multi-byte Register Read ===
Read 6 bytes: 0x00 0x00 0x00 0x00 0x00 0x00
PASS: Multi-byte register read

Functions 13-14/14: adxl366_set_power_mode(), adxl366_get_power_mode()

=== Test 9: Power Mode Configuration ===
Set STANDBY, Read back: 0
Set MEASURE, Read back: 2
PASS: Power mode configuration

===================================
ALL 14 FUNCTIONS TESTED!
ALL TESTS PASSED!
===================================

Cleaning up: adxl366_remove()
```

---

## Project Status

All phases must be completed sequentially to finish the project.

**Phase A (MAX32655):** ✅ Complete - All tests passed (March 30th, 2026) ⭐

**Phase B (nRF54L15):** Next - After Phase A completion

**Phase C (Bare-metal nRF):** Required - After Phase B completion

See [DEVELOPMENT.md](DEVELOPMENT.md) for detailed strategy and technical decisions.

---

## Contributing

This project follows the [no-OS coding style guidelines](https://github.com/analogdevicesinc/no-OS/wiki/Code-Style-guidelines).

All commits must be signed off: `git commit -s`

For contribution strategy and upstream PR details, see [DEVELOPMENT.md](DEVELOPMENT.md).

---

## References
- [ADXL366 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl366.pdf)
- [no-OS Repository](https://github.com/analogdevicesinc/no-OS)
- [no-OS Contributing Guide](https://github.com/analogdevicesinc/no-OS/blob/main/doc/sphinx/source/contributing.rst)
