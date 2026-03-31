# ADXL366 Development & Strategy Notes

_⭐ indicates current milestone_

## Project Context

This document captures the development strategy, technical decisions, and upstream contribution plan for the ADXL366 driver project.

**Primary Goal:** Contribute ADXL366 driver to upstream no-OS repository
**Deadline:** March 31st, 2026 ⭐

---

## Multi-Platform Strategy: A→B→C

**All phases must be completed sequentially to finish the project.**

### Why This Order?

**Initial approach:** Started with nRF54L15 (future target platform)
- Hit power domain initialization issues
- SPIM peripheral not accessible
- Weeks spent on platform-specific debugging
- Deadline approaching with no tested driver

**Strategic pivot (March 27th):** Test on mature platform first ⭐
- Separates driver concerns from platform concerns
- Gets driver validated and upstream before deadline
- Can refine nRF integration without pressure

**Key insight:** Driver and platform layer are separate concerns. Driver can be upstreamed with one tested platform, others can follow. However, all three phases are required for project completion.

---

## Phase Details

### Phase A: MAX32655 Testing ✅ (March 27-30, 2026)
**Status:** COMPLETE

**Objectives:**
- Create MAX32655 eval project
- Test all 14 driver functions on hardware
- Validate driver is platform-agnostic
- Prepare for upstream PR

**Results:**
- Basic example: continuous accelerometer reading ✅
- Comprehensive test: all features validated ✅
- Hardware tested: March 30th, 2026
- Ready for PR submission

**Why MAX32655 first:**
- Platform already mature in no-OS
- No power domain issues
- Unblocks driver contribution immediately

### Phase B: nRF54L15 with Nordic SDK (NEXT - after PR)
**Status:** Pending Phase A completion

**Objectives:**
- Get practical nRF testing environment working
- Use Nordic SDK initialization (like working UART example)
- Keep ADXL366 driver platform-agnostic
- Eval project handles Nordic-specific setup

**Approach:**
- Accept Nordic SDK dependency for eval project
- Focus on driver functionality, not bare-metal platform
- Demonstrate driver portability

### Phase C: Bare-metal nRF Platform (Required for project completion)
**Status:** Required after Phase B - all phases must complete sequentially

**Objectives:**
- Proper bare-metal no-OS platform layer for nRF54L15
- Implement power domain/clock initialization
- Make nRF platform adapters work without Nordic SDK
- Critical for BioLinq and other projects

**Why after Phase B:**
- Requires significant platform development
- Not blocking driver contribution (Phase A handles that)
- Phase B provides working reference while developing bare-metal layer
- All three phases required to mark project as complete

---

## Upstream Contribution Plan

### Commit Structure

We'll submit the driver in a single PR with clean commits:

**Commit 1: Add ADXL366 driver** ⭐ (CRITICAL)
```
drivers: accel: adxl366: Add ADXL366 accelerometer driver

Add driver for ADXL366 ultra-low-power 3-axis accelerometer:
- Device initialization and ID verification
- Range configuration (±2g, ±4g, ±8g)
- ODR configuration (12.5Hz - 400Hz)
- 14-bit accelerometer data acquisition
- Temperature sensor reading
- Status register access
- Power mode control

Tested on MAX32655 FTHR + ADXL366Z eval board.

Signed-off-by: Marco Ramirez <marco.ramirez@analog.com>
```

Files:
- `drivers/accel/adxl366/adxl366.c` (new)
- `drivers/accel/adxl366/adxl366.h` (new)

**Commit 2: Add ADXL366 evaluation project**
```
projects: eval-adxl366z: Add evaluation project for ADXL366

Add evaluation project demonstrating ADXL366 driver usage:
- Basic example: continuous accelerometer reading
- Comprehensive test: validates all 14 driver functions
- MAX32655 platform support

Hardware: MAX32655 FTHR + ADXL366Z eval board

Signed-off-by: Marco Ramirez <marco.ramirez@analog.com>
```

Files:
- All files in `projects/eval-adxl366z/`

### Code Style Requirements

All code must pass astyle formatting:

```bash
astyle --style=linux --indent=force-tab=8 --max-code-length=80 \
       --suffix=none --pad-oper --pad-header --unpad-paren --pad-comma \
       drivers/accel/adxl366/*.[ch] \
       projects/eval-adxl366z/src/**/*.[ch]
```

### Commit Message Rules

- Follow 50/72 rule (subject ≤50 chars, body wrapped at 72)
- Use imperative mood ("Add driver" not "Added driver")
- Include detailed description in body
- Must have `Signed-off-by` trailer
- Reference hardware tested

### Pre-submission Checklist

- [ ] All code passes astyle formatting
- [ ] Each commit builds independently
- [ ] Driver tested on hardware (documented in commit message)
- [ ] No compiler warnings
- [ ] Commit messages follow guidelines
- [ ] All commits signed off (`git commit -s`)

---

## Development Timeline

- **2026-03-06:** Initial ADXL366 driver implementation (basic features)
- **2026-03-11:** nRF platform debugging (power domain issues)
- **2026-03-14:** Realized upstream contribution strategy
- **2026-03-27:** Pivoted to MAX32655 first (A→B→C sequence) ⭐
- **2026-03-24:** Complete driver implementation (14 functions) ✅
- **2026-03-30:** Hardware testing passed on MAX32655 ✅
- **2026-03-31:** DEADLINE - Driver PR submission target

---

## Technical Decisions

### Why 14 Functions in Phase 1?

Initially planned minimal driver (init + read accel). Extended to include:
- Range/ODR configuration: Essential for different use cases
- Temperature reading: Low-hanging fruit, useful for monitoring
- Status register: Critical for debugging and data ready polling

Deferred to Phase 2:
- FIFO: More complex, not needed for basic operation
- Interrupts: Requires interrupt infrastructure
- Activity detection: Application-specific feature

**Rationale:** Ship complete but focused driver. Phase 1 covers 90% of use cases.

### Platform-Agnostic Design

Driver uses only no-OS abstractions:
- `no_os_spi_*` for communication
- `no_os_delay_*` for timing
- No platform-specific includes

Benefits:
- Works on any platform with SPI support
- Easy to port to new platforms
- Matches no-OS architecture

### MAX32655 vs nRF54L15 First

**MAX32655 advantages:**
- Mature platform layer in no-OS
- No initialization issues
- Fast validation cycle

**nRF54L15 challenges:**
- Power domains require complex init
- Platform layer incomplete
- Would block entire driver contribution

**Decision:** Start with working platform, add nRF later.

---

## Hardware Testing

### Test Setup
- **Board:** MAX32655 FTHR
- **Sensor:** ADXL366Z evaluation board
- **Interface:** SPI (4MHz, Mode 0)
- **Power:** 3.3V
- **Date:** March 30th, 2026

### Test Results

**Basic Example:**
- Device ID verification: PASS (0xAD 0x1D 0xF7)
- Continuous accelerometer reading: PASS
- ~1g magnitude measured (expected for stationary board)

**Comprehensive Test:**
- All 14 driver functions tested
- All configurations validated (range, ODR)
- Temperature sensor: 25°C (expected)
- Status register: DATA_RDY and AWAKE bits correct
- Result: ALL TESTS PASSED ✅

---

## Future Work

### Phase 2 Features (Post-Upstream)
- FIFO buffer management
- Activity/inactivity detection
- Interrupt configuration
- Self-test functionality

### Additional Platforms
- nRF54L15 (Phase B)
- Other platforms as needed

### BioLinq Integration
- Once nRF platform is stable
- Driver ready for integration

---

## References

- [ADXL366 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/adxl366.pdf)
- [no-OS Repository](https://github.com/analogdevicesinc/no-OS)
- [no-OS Contributing Guide](https://github.com/analogdevicesinc/no-OS/blob/main/doc/sphinx/source/contributing.rst)
- [no-OS Code Style Guidelines](https://github.com/analogdevicesinc/no-OS/wiki/Code-Style-guidelines)
