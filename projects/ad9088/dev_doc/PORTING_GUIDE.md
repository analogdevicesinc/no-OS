# AD9088 Linux Kernel to no-OS Porting Guide

This guide walks through porting the AD9088 (Apollo) RF transceiver driver from
the ADI Linux kernel (`~/Documents/GitHub/ADI_kernel`) to the no-OS bare-metal
framework (`ADI_no-os`). Each step builds on the previous one.

---

## Progress Tracker

- [x] Step 0: Orientation
- [ ] Step 1: Structs and init_param (add fields incrementally per step)
- [x] Step 2: SPI HAL wiring (verified on VCU118 — all 3 register tests pass)
- [x] Step 3: Firmware / profile loading (TyE path verified on HW — all 4 flash images load)
- [x] Step 4: Device setup (hw_open through startup — AD9084 Rev. 4 Grade 3 probed)
- [x] Step 5: JESD204 FSM callbacks (done: scaffolding, LINK_INIT, ADXCVR init, LINK_SETUP, CLOCKS_ENABLE, LINK_ENABLE, OPT_SETUP_STAGE1/2, lane-crossbar fix — **link up, WAIT_BS cleared on HW 2026-07-24**; **LINK_RUNNING HW-verified on VCU118 2026-07-27** — all 8 lanes "Link is good", User status Ready; **POST_SETUP_STAGE1/2/3 + POST_RUNNING HW-verified on VCU118 2026-07-27** — full bring-up sequence, `is_initialized` set, `adc_bgcal_unfreeze` runs, reaches "Project configured")
- [x] Step 6: NCO control functions (calc FTW/freq + CNCO/FNCO set/get wrappers ported, HW-verified on VCU118 2026-07-28)
- [ ] Step 7: Calibration (clock conditioning + ADC init cal)
- [x] Step 8: MCS (multi-chip sync) — **DONE and HW-verified on VCU118. Subclass 1 came up 2026-08-13; MCS calibration passed on its first hardware run 2026-08-14.** `Total BSYNC path delay 2175424 fs`, `MCS Init Cal Validation: Passed`, and **`SYSREF_PHASE` trimmed 433 → 40** — that unit is Fclk (20 GHz, 50 ps/count), so 21.65 ns → 2.00 ns — stable across all five reads, with no link regression (8 lanes good, `Subclass=1`, NCO round-trips passing). The cal layer is `ad9088_mcs.c` + a real `post_setup_stage1` body + an `ad9088_bsync_ops`/`ad9088_clk_ops` function-pointer shim replacing the dead `iio_adf4030`/`iio_adf4382` handles; it also converted the shared ADF4382 phase API from ps to fs. Two follow-ups, neither a failure: the 2.00 ns residual is close to the 2.175 ns path delay just compensated and the init-cal status data needed to attribute it is printed only on failure; and latency repeatability across FSM restarts is not yet exercised. See `SUBCLASS1_ENABLEMENT.md` Stage D
- [ ] Step 9: Runtime control (loopback, test tones, CFIR) — RX FNCO test tone + DMA capture HW-verified on VCU118 (**2026-08-13**: noise floor and test-tone coherence pass, JTAG readback of the capture buffer working via `capture.tcl`); loopback modes and CFIR still to port
- [ ] Step 10: IIO integration
- [x] Step 11: ad9088_remove and cleanup
- [x] Step 12: Project integration (basic_example, common_data, src.mk — partial)

---

## Subclass 1 enablement — DONE (2026-08-13)

Subclass-0 bring-up is **complete and HW-verified**: the full FSM reaches
"Project configured" with all 8 lanes "Link is good", User status Ready and
`is_initialized` set (2026-07-27), and the end-to-end datapath passes the DMA
example's three tests with 99.8% loopback coherence (2026-08-10).

**Subclass 1 — SYSREF-based deterministic latency — is now up and HW-verified**,
as groundwork for multi-device phase alignment. The detailed staging, hardware
results and verification criteria live in **`SUBCLASS1_ENABLEMENT.md`** — read
that first when resuming. Stages A, B, B-fix and C are all done; the subclass-1
link came up on the first attempt with `Subclass=1` on all enabled links,
`SYSREF Phase: Locked`, `SYSREF captured: Yes` / `SYSREF alignment error: No` on
both AXI cores, `Link status: DATA` with 8 lanes good, `SYSREF_PHASE = 433`
identical across five reads, and all four NCO round-trips passing. The bring-up
harness is `basic_example`.

**Stage D (MCS calibration) passed on its first hardware run, 2026-08-14.** It
trimmed the `SYSREF_PHASE = 433` above to **40** — that unit is Fclk (20 GHz), so
21.65 ns → 2.00 ns — with `Total BSYNC path delay 2175424 fs` and `MCS Init Cal
Validation: Passed`. The riskiest moment, the BSYNC round-trip handing the SYSREF
line to the Apollo and back *while the link is up*, **did not disturb the link**.
Stage D also converted the shared ADF4382 phase API from picoseconds to
femtoseconds and gave `adf4382_pfd_compute()` a PFD range check across all 9 of its
call sites, so `projects/adf4382` is affected too. Two follow-ups remain, neither a
failure: attributing the 2.00 ns residual (needs the init-cal status printed on the
success path) and exercising latency repeatability across FSM restarts.

Stage C was five changes: `AD9088_RX/TX_JESD_SUBCLASS = 1`; the `sysref_present`
profile fixup in `ad9088_dt.c` (mandatory — the hardware dump showed all three
flags 0); a profile version check plus `profile_checksum` recompute, which
retired the long-standing `WARNING: profile crc is invalid.` line and needed a new
`util/no_os_crc32.c`; seeding `sysref.lmfc_offset` to
`JESD204_LMFC_OFFSET_UNINITIALIZED` in the **framework**
(`jesd204_dev_alloc_links()`), matching the kernel; and folding the ADF4030's
sticky `TDC_ERR`/`TMP_ALIGN_ERR` bits into its per-channel alignment log so
"aligned" no longer conflates *finished* with *converged*.

Two things the earlier plan for Stage C got wrong, both corrected in
`SUBCLASS1_ENABLEMENT.md`: `lnk->sysref.mode` should **not** be set to
`JESD204_SYSREF_CONTINUOUS` (the kernel never touches `lnk->sysref` at all, and
the HMC7044 pulse-gen mode it would force is already configured), and the
convergence gap was closed with a sticky-error read rather than a second TDC
probe (the probe costs ~215 ms per channel and perturbs the TDC registers).

**One live regression:** `iio_example.c` will no longer bring up a link — it still
marks the HMC7044 as `is_sysref_provider` and has no ADF4030, so it inherits the
shared subclass flip with nothing driving the Apollo's SYSREF pin. The fix is
mechanical; see the deviations list in `SUBCLASS1_ENABLEMENT.md`.

Getting the alignment working required an unrelated-looking root cause worth
remembering: `TDC_SOURCE` (`0x11[4:0]`) was never programmed, because
`clks_sync4` guarded the write on a `calloc`-zeroed driver shadow while the
register resets to `0x1F`. For a reference channel of 0 the write was skipped and
the TDC measured against a channel that does not exist. Two hypotheses recorded
earlier — that iterative alignment (`EN_ITER`) was disabled, and that AVGEXP
should be 13 — were both **wrong**; see the Stage B-fix section of
`SUBCLASS1_ENABLEMENT.md` for the corrections.

This section records only what the investigation corrected about the assumptions
previously written here.

Reading the kernel devicetree (`vcu118_ad9084.dts`) disproved three claims that
earlier revisions of this guide made:

1. **`JESD_SUBCLASS` is defined inside `#if WITH_AION`** (`dts:38-48`), i.e.
   subclass 1 and the ADF4030 are *one feature*, not two independent ones. The
   ADF4030 work is not optional groundwork preceding a subclass flip — it is the
   flip.
2. **The board has an ADF4030 and it is the `jesd204-sysref-provider`**
   (`dts:617,639`, `adf4030@4`) — not the HMC7044, as the topology arrays
   assumed. It drives both the Apollo's SYSREF pin (ch5, `APOLLO_SYSREF`) and the
   FPGA's `sysref_in` (ch8, `SYSREF_IN_F`). The earlier statement that the kernel
   "skips `post_setup_stage1` on a board with no adf clock chips, which matches
   our config exactly" was wrong: it matched our *config*, which simply never
   instantiated the chip that is physically present.
3. **The adf4030/adf4382 drivers did not need porting** — both already exist in
   no-OS (`drivers/frequency/adf4030/`, `drivers/frequency/adf4382/`) with the
   API the kernel MCS code reaches through IIO. Only JESD204 registration was
   missing from the ADF4030, now added.

Also corrected: **HMC7044 ch3 is `ADF4030_BSYNC0`, not `FPGA_SYSREF`** (LVPECL,
`adi,jesd204-sysref-chan`), and **ch1 `ADF4030_REFIN` /20 → 125 MHz was missing
from `chan_spec` entirely**, so the ADF4030 had no reference clock. Both fixed
2026-08-13.

SYSREF being "cosmetic under subclass 0" was true, and is why the pass mask
excluded bit `0x40` there. **We now run subclass 1**, so the mask is `0x60` and an
unlocked SYSREF phase *fails* the link — the `SYSREF Phase:` line is load-bearing
and reads `Locked` on hardware.

---

## Prerequisite: HAL Sync

Before any porting work, ensure the Apollo HAL in no-OS matches the kernel
version. The HAL (`public/src/` and `public/inc/`) is a vendor-provided library
shared between both trees — if they diverge, you'll get subtle failures during
calibration or startup.

```bash
# Check for differences
diff <(ls ~/Documents/GitHub/ADI_kernel/drivers/iio/trx-rf/ad9088/public/src/*.c \
       | xargs -I{} basename {} | sort) \
     <(ls drivers/rf-transceiver/apollo/public/src/*.c \
       | xargs -I{} basename {} | sort)

# If files differ, copy from kernel to no-OS
cp ~/Documents/GitHub/ADI_kernel/drivers/iio/trx-rf/ad9088/public/src/*.c \
   drivers/rf-transceiver/apollo/public/src/
cp ~/Documents/GitHub/ADI_kernel/drivers/iio/trx-rf/ad9088/public/inc/*.h \
   drivers/rf-transceiver/apollo/public/inc/
```

Also sync `private/` bitfield headers and `adi_utils/` if present. The headers
tend to stay in sync but the source files can diverge significantly (we found
31/48 files differing before syncing).

---

## Architecture Overview

### What the kernel driver does

The Linux kernel driver lives at:
```
~/Documents/GitHub/ADI_kernel/drivers/iio/trx-rf/ad9088/
```

Key files:
| File | Lines | Role |
|------|-------|------|
| `ad9088.c` | 5757 | Main driver: probe, IIO integration, NCO, attribute handlers |
| `ad9088.h` | 436 | Core structs: `ad9088_phy`, `ad9088_chan_map`, enums |
| `ad9088_jesd204_fsm.c` | 1059 | JESD204 FSM callbacks (link_init, setup, enable, status) |
| `ad9088_dt.c` | 296 | Device tree parsing |
| `ad9088_cal.c` | 713 | Calibration save/restore with CRC |
| `ad9088_mcs.c` | 505 | Multi-chip sync init and tracking |
| `ad9088_bmem.c` | 911 | Block memory delay calibration |
| `ad9088_fft.c` | 768 | FFT sniffer (spectrum analysis) |
| `ad9088_ffh.c` | 441 | Fast frequency hopping |
| `ad9088_debugfs.c` | 893 | Debugfs entries (optional, skip for no-OS) |

Under the hood, all of these call into a vendor HAL library — the "Apollo public
API" — which consists of ~48 source files and ~116 headers. This HAL handles
register-level operations, ARM firmware communication, and signal chain control.

### What no-OS already has

The Apollo HAL is already fully ported:
```
drivers/rf-transceiver/apollo/
  public/src/  -> 48 C files (adi_apollo_*.c)
  public/inc/  -> 116 headers (adi_apollo_*.h)
  private/     -> bitfield definitions
  firmware/    -> usecase.bin (device profile binary)
    app_signed_encrypted_B/      -> TyE flash images (non-prod)
    app_signed_encrypted_prod_B/ -> TyE flash images (prod)
```

The **driver wrapper** (`ad9088.c`, `ad9088.h`, `ad9088_dt.c`) exists but is
only ~40% complete. The basic init flow works (SPI, reset, register test), but
the JESD204 FSM, calibration, NCO control, and IIO integration are missing.

### The porting pattern

Every port follows the same translation:
1. **Linux API** -> **no-OS API**: `devm_kzalloc` -> `no_os_calloc`, `dev_err`
   -> `pr_err`, `spi_device` -> `no_os_spi_desc`, etc.
2. **Device tree** -> **init_param struct**: DT properties become fields in
   `ad9088_init_param` filled at compile time in `common_data.c`
3. **IIO subsystem** -> **no-OS IIO**: `iio_dev` -> `iio_device`, channels
   and attributes registered via `iio_app`
4. **Kernel frameworks** -> **no-OS equivalents**: `struct clk` -> manual rate
   tracking, `regulator` -> GPIO control, `mutex` -> nothing (bare metal,
   single thread)
5. **Apollo HAL calls are identical** — the vendor API (`adi_apollo_*`) is the
   same in both worlds. This is the key insight: most of the "porting" is
   wiring, not logic.

---

## Step 0: Orientation — Understanding the Files

**Goal**: Understand what maps to what before touching any code.

### Kernel -> no-OS file mapping

| Kernel file | no-OS equivalent | Status |
|-------------|------------------|--------|
| `ad9088.h` | `drivers/rf-transceiver/apollo/ad9088.h` | Partial — structs exist, some commented out |
| `ad9088.c` (probe) | `ad9088.c` (`ad9088_init`) | Partial — SPI+reset works, rest missing |
| `ad9088.c` (NCO, attrs) | `ad9088.c` (new functions needed) | Missing |
| `ad9088_dt.c` | `ad9088_dt.c` (`ad9088_parse_struct`) | Done — parses init_param instead of DT; board lane crossbars wired (jrx=physical, jtx=logical) |
| `ad9088_jesd204_fsm.c` | `ad9088_jesd204_fsm.c` | Full FSM — LINK_INIT..LINK_ENABLE + OPT_SETUP_STAGE1/2 + LINK_RUNNING (HW-verified 2026-07-27) + POST_SETUP_STAGE1/2/3 + POST_RUNNING (ported 2026-07-27) |
| `ad9088_cal.c` | Not needed initially | Skip — cal save/restore is a nice-to-have |
| `ad9088_mcs.c` | New functions in ad9088.c | Missing |
| `ad9088_bmem.c` | `ad9088.c` or new file | Missing |
| `ad9088_fft.c` | `ad9088_fft.c` | Exists but mostly commented out |
| `ad9088_ffh.c` | Not needed initially | Skip |
| `ad9088_debugfs.c` | Not applicable | Skip — no debugfs in bare metal |

### Linux API -> no-OS translation table

| Linux | no-OS | Notes |
|-------|-------|-------|
| `devm_kzalloc(dev, size, GFP_KERNEL)` | `no_os_calloc(1, size)` | Must manually free in remove |
| `dev_err(&spi->dev, ...)` | `pr_err(...)` | no_os_print_log.h |
| `dev_info(...)` | `pr_info(...)` | |
| `dev_dbg(...)` | `pr_debug(...)` | |
| `struct spi_device *spi` | `struct no_os_spi_desc *spi` | |
| `spi_write_then_read(...)` | `no_os_spi_write_and_read(...)` | |
| `struct gpio_desc` | `struct no_os_gpio_desc` | |
| `devm_gpiod_get(...)` | `no_os_gpio_get(...)` | |
| `struct mutex lock` | Remove (single-threaded) | |
| `struct clk *clk` | Track rate in a `uint64_t` field | |
| `clk_get_rate(clk)` | Read the stored rate value | |
| `struct regulator` | GPIO or remove | |
| `request_firmware(...)` | Link binary via linker script (`_binary_*` symbols) | Already done |
| `jesd204_dev_of_get(...)` | Skip — not needed in no-OS | |
| `jesd204_device_create(...)` | `jesd204_dev_register(...)` | Different API |
| `iio_device_alloc(...)` | `iio_app_init(...)` | no-OS IIO framework |
| `container_of(...)` | Direct struct access | |
| `EXPORT_SYMBOL(...)` | Remove | |
| `module_spi_driver(...)` | Remove — init called from main | |
| `do_div(a, b)` | `a / b` (64-bit division) | |
| `clamp_t(type, val, lo, hi)` | Write a simple inline or macro | |
| `BIT(n)` | `(1 << (n))` or `NO_OS_BIT(n)` | |

### Exercise

Before moving to Step 1, read these files side by side:
1. Kernel `ad9088.h` struct `ad9088_phy` vs no-OS `ad9088.h` struct `ad9088_phy`
2. Kernel `ad9088_probe()` (in `ad9088.c`) vs no-OS `ad9088_init()`
3. Kernel `ad9088_jesd204_fsm.c` — this is what we need to port

**Key question to answer**: For each field in the kernel's `ad9088_phy`, is it
already in the no-OS version, commented out, or missing entirely?

---

## Step 1: Complete the Structs (ad9088.h)

**Goal**: Make `ad9088_init_param` and `ad9088_phy` complete enough to support
all subsequent steps.

### What to do

Compare kernel `ad9088.h` (`struct ad9088_phy`, lines 215-346) against the
no-OS version. Add missing fields needed for JESD204, NCO control, and
calibration.

### Fields to add to `ad9088_init_param`

These come from device tree properties in the kernel. In no-OS, the user sets
them directly in `common_data.c`:

```c
// Already present: spi_init, gpio_reset, gpio_tri_req, rx/tx enable GPIOs,
//   spi_3wire_en, rx/tx_real_channel_en, side_b_use_own_tpl_en,
//   multidevice_instance_count, trig_sync_en, standalone_en, nyquist_zone,
//   subclass, jtx/jrx lane mappings, serializer amplitude/emphasis

// Missing - add these:
bool sniffer_en;                    // Enable FFT sniffer
uint32_t cddc_sample_delay;         // Block memory CDDC delay
uint32_t fddc_sample_delay;         // Block memory FDDC delay
```

### Fields to add to `ad9088_phy`

```c
// Missing - needed for JESD204 FSM and calibration:
bool cal_data_loaded_from_fw;       // Whether cal data was loaded from file
bool cddc_sample_delay_en;          // Enable CDDC delay cal
bool fddc_sample_delay_en;          // Enable FDDC delay cal
uint8_t *nvm_adc_cal;              // Calibration data buffer
uint8_t *adc_cal;                  // ADC cal data buffer
bool cnco_dual_modulus_mode_en;     // CNCO dual modulus mode
bool fnco_dual_modulus_mode_en;     // FNCO dual modulus mode
```

### Key pattern: init_param vs dev

- `init_param` = what the user configures at compile time (replaces device tree)
- `ad9088_phy` = runtime state allocated by `ad9088_init()`
- `ad9088_init()` copies relevant fields from `init_param` to `phy`

### How kernel DT parsing maps to no-OS

Kernel (`ad9088_dt.c`):
```c
phy->spi_3wire_en = device_property_read_bool(dev, "adi,spi-3wire-enable");
```

no-OS (`ad9088_dt.c`):
```c
phy->spi_3wire_en = init_param->spi_3wire_en;
```

no-OS (`common_data.c`):
```c
struct ad9088_init_param ad9088_ip = {
    .spi_3wire_en = false,
    ...
};
```

---

## Step 2: SPI HAL Wiring

**Goal**: Understand how SPI communication works between the driver and the
Apollo HAL.

### Already done

This step is **complete** in the current no-OS driver. The three SPI callbacks
are implemented in `ad9088.c`:

```c
static int ad9088_spi_xfer(void *dev_obj, uint8_t *wbuf, uint8_t *rbuf, uint32_t len);
static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[], ...);
static int ad9088_spi_write(void *dev_obj, const uint8_t tx_data[], ...);
```

These are registered in `ad9088_init()`:
```c
phy->ad9088.hal_info.spi0_desc.read = ad9088_spi_read;
phy->ad9088.hal_info.spi0_desc.write = ad9088_spi_write;
phy->ad9088.hal_info.spi0_desc.xfer = ad9088_spi_xfer;
```

Also registered: `delay_us`, `reset_pin_ctrl`, `log_write`.

### How it works

The Apollo HAL calls `adi_apollo_hal_reg_set/get()` which internally calls the
registered `read`/`write` callbacks. The no-OS callbacks use
`no_os_spi_write_and_read()` to talk to the actual SPI peripheral.

### Confirmed complete

All three SPI callbacks (`xfer`, `read`, `write`) plus `reset_pin_ctrl`,
`delay_us`, and `log_write` are implemented and wired up in `ad9088_init()`.

The kernel also has an `ad9088_check_apollo_error()` helper that translates
Apollo `API_CMS_ERROR_*` codes into human-readable strings and Linux errno
values. This is **not** part of the HAL — it's a driver-level convenience
function. Add it when porting the JESD204 FSM callbacks (Step 5) if you want
better error messages, or just use `pr_err` with the raw error code.

### VCU118 hardware verification (2026-07-10)

SPI verified working on VCU118 after fixing `GPIO_OFFSET`. All three
`ad9088_reg_test()` checks pass: direct register, indirect register, ARM
memory.

**Bug fixed: device stuck in reset** — The AXI GPIO controller has two 32-bit
channels. The DTS maps the reset pin to GPIO 62 (`<&axi_gpio 62 0>`), which
is channel 1, pin 30. The no-OS `GPIO_OFFSET` was 0, so pin 30 (channel 0)
was being driven instead of pin 62 (channel 1). The device never left reset
and all SPI reads returned 0x00. Fix: `GPIO_OFFSET = 32` in `parameters.h`.

| Parameter | Linux DTS | no-OS | Match? |
|-----------|-----------|-------|--------|
| SPI controller | `axi_spi_2 @ 0x44A80000` | `XPAR_SPI_1_DEVICE_ID` (SPI_PL) | Yes |
| SPI mode | Mode 0 | `NO_OS_SPI_MODE_0` | Yes |
| Chip select | reg = 0 | `SPI_CS_APOLLO = 0` | Yes |
| SPI clock | 5 MHz | 13 MHz | No (works, may need lowering) |
| Reset GPIO | pin 62 | `GPIO_OFFSET(32) + GPIO_RESET_N(30) = 62` | Yes (after fix) |

**Fixed**: `ad9088_reg_test()` now uses `pr_info` for pass messages and
`pr_err` only for failures (previously used `pr_err` for both).

---

## Step 3: Firmware / Profile Loading

**Goal**: Load the device profile (use-case binary) and apply it to hardware.

### What's done

**Binary embedding** — The profile binary (`usecase.bin`) is linked into the ELF
via `objcopy` in the build system. The Makefile copies the selected firmware to
`usecase.bin` before building:

```makefile
# projects/ad9088/Makefile
FIRMWARE ?= $(NO-OS)/drivers/rf-transceiver/apollo/firmware/id01_uc42_ce_vck190.bin
NEW_FIRMWARE = $(dir $(FIRMWARE))usecase.bin
$(shell cp $(FIRMWARE) $(NEW_FIRMWARE))
```

**Profile loading** — `ad9088_get_profile()` in `ad9088_dt.c` validates the
binary size against `sizeof(adi_apollo_top_t)` (22,130 bytes) and copies it into
`phy->profile`:

```c
int ad9088_get_profile(struct ad9088_phy *phy, const struct ad9088_init_param *init_param)
{
    size_t firmware_size = _binary_..._end - _binary_..._start;
    if (sizeof(*p) != firmware_size) {
        pr_err("Invalid size of profile structure ...\n");
        return -EINVAL;
    }
    memcpy(p, firmware_ptr, firmware_size);
    return 0;
}
```

**Lane setup** — `ad9088_jesd_lane_setup()` configures JTX/JRX lane mappings
and serializer amplitude/emphasis from `init_param` into the loaded profile.

**Call chain** — `ad9088_parse_struct()` calls both `ad9088_get_profile()` and
`ad9088_jesd_lane_setup()` during init.

### COMPLETE

The profile is loaded into memory and applied via `adi_apollo_startup_execute()`
in Step 4. The startup call subsumes firmware loading, profile application, and
datapath configuration in a single unified API.

### Kernel equivalent

In the kernel, `request_firmware()` loads the binary from the filesystem at
runtime. The no-OS approach of linking it into the ELF is simpler and correct
for bare-metal.

### TyE firmware provider

The startup sequence checks TyE bypass status at runtime via
`adi_apollo_arm_tye_bypassed_get()`. When TyE is active (bypass = 0), it loads
4 flash images per variant via a firmware provider interface:

| FW ID | File | Description |
|-------|------|-------------|
| `SECR_BOOT_HDR` | `flash_image_0x01030000.bin` | SecureBoot header |
| `CORE_0_TYE_FW` | `flash_image_0x20000000.bin` | Core 0 TyE firmware |
| `CORE_1_TYE_FW` | `flash_image_0x02000000.bin` | Core 1 TyE firmware |
| `TYE_OPER_FW` | `flash_image_0x21000000.bin` | TyE operational firmware |

The binaries are organized in two subdirectories under
`drivers/rf-transceiver/apollo/firmware/`:
- `app_signed_encrypted_B/` — non-prod variant
- `app_signed_encrypted_prod_B/` — prod variant

The `PROD_*` FW IDs point to the `prod_B` directory; the non-prod IDs point to
the non-prod directory. The build system's `rwildcard` in `generic.mk`
automatically picks up all `.bin` files from subdirectories in `SRC_DIRS`.

The firmware provider is implemented in `ad9088.c`:
- `ad9088_fw_provider_get()` — returns a pointer to the linked binary
- `ad9088_fw_provider_close()` — no-op (binaries are statically linked)
- `fw_table[]` — maps `adi_apollo_startup_fw_id_e` enum to `_binary_*` symbols

The old standard FW path (`APOLLO_FW_CPU0_B.bin`, `APOLLO_FW_CPU1_B.bin`) is
no longer used and has been removed.

### Hardware verification (2026-07-17)

TyE firmware loading verified on VCU118. All 4 flash images load successfully
(scenario 4, TyE bypass = 0). SecureBoot stage 0 and stage 1 both pass.
Full startup sequence completes: FW_LOAD, PROFILE_LOAD, TMU_EN, DP_CONFIG,
LINK_EN, TXRX_ACTIVATE, SYNC — all OK.

---

## Step 4: Device Setup (hw_open through startup)

**Goal**: Complete the initialization sequence from SPI setup through device
startup.

### Current state

`ad9088_init()` currently does:
1. Parse init_param -> allocate phy, init SPI, init GPIOs
2. Wire up HAL callbacks
3. Call `adi_apollo_device_hw_open()` (reset + init)
4. Set active protocol to SPI
5. Run register test

### COMPLETE

After the register test, `ad9088_init()` now calls:

```c
// Run unified startup (firmware load, profile apply, datapath config)
adi_apollo_startup_execute(&phy->ad9088, &phy->profile,
                           ADI_APOLLO_STARTUP_SEQ_DEFAULT);

// Version and firmware info (mirrors kernel's ad9088_version_info)
ad9088_version_info(phy);

// Chip identification + final probe message
adi_apollo_device_chip_id_get(...)
pr_info("AD%X Rev. %u Grade %u (API %u.%u.%u) probed\n", ...);
```

`ad9088_version_info()` is a static helper matching the kernel's function of
the same name. It queries: API revision, mailbox ready check, ping core 1,
ping core 0, FW version. Error messages match the kernel's wording.

**Note**: The kernel's `ad9088_setup()` calls `adi_apollo_startup_execute()`
(not `adi_apollo_device_cfg_set` + `adi_apollo_device_startup` — those functions
do not exist in the HAL). `ADI_APOLLO_STARTUP_SEQ_DEFAULT` runs all 7 startup
sub-sequences: FW_LOAD, PROFILE_LOAD, TMU_EN, DP_CONFIG, LINK_EN,
TXRX_ACTIVATE, and SYNC.

**Error cleanup**: `ad9088_init()` uses `goto error` / `goto error_hw_close`
labels to properly free resources on failure. `ad9088_remove()` is implemented.

### Hardware verification (2026-07-17)

Output on VCU118:
```
Test direct register Passed
Test indirect register Passed
Test ARM memory Passed
API ver: 2.0.10
Ping (core 1) cmd/res: 0x00000000/0x00000001
Ping (core 0) cmd/res: 0x12345678/0x1234567a
FW ver: 2.0.6
AD9084 Rev. 4 Grade 3 (API 2.0.10) probed
```

### Architecture decision

The kernel probe delegates JESD204 link bring-up to FSM callbacks (Step 5).
The no-OS JESD204 framework supports the same callback structure, so we use
**Option A**: implement the FSM callbacks to mirror the kernel.

---

## Step 5: JESD204 FSM Callbacks

**Goal**: Port the JESD204 state machine callbacks from the kernel.

This is the largest and most critical step. The JESD204 FSM orchestrates the
entire link bring-up sequence between the FPGA (AXI JESD204 core) and the
AD9088.

### Kernel source

`ad9088_jesd204_fsm.c` — 1059 lines, defines these callbacks:

```c
jesd204_dev_data = {
    .state_ops = {
        [JESD204_OP_LINK_INIT]        = { .per_link = ad9088_jesd204_link_init },
        [JESD204_OP_LINK_SETUP]       = { .per_device = ad9088_jesd204_link_setup },
        [JESD204_OP_LINK_SUPPORTED]   = { .per_link = ... },
        [JESD204_OP_OPT_SETUP_STAGE1] = { .per_device = ad9088_jesd204_setup_stage1 },
        [JESD204_OP_OPT_SETUP_STAGE2] = { .per_device = ad9088_jesd204_setup_stage2 },
        [JESD204_OP_CLKS_ENABLE]      = { .per_link = ad9088_jesd204_clks_enable },
        [JESD204_OP_LINK_ENABLE]      = { .per_link = ad9088_jesd204_link_enable },
        [JESD204_OP_OPT_POST_RUNNING_STAGE] = { .per_device = ad9088_jesd204_post_setup_stage1/2/3 },
    },
};
```

### What each callback does

1. **link_init**: Fills in `jesd204_link` struct from the profile (lane count,
   converters, frame params, version, subclass, sample rate). Pure data
   translation, no hardware access.

2. **link_setup**: Enables JTX/JRX links, configures RX/TX enable blocks,
   resets datapaths, sets MCS subclass and SYSREF. First real hardware
   interaction.

3. **setup_stage1**: Sets Nyquist zone, runs clock conditioning cal, runs ADC
   init cal, inspects JRX/JTX links, sets phase adjust. Long-running
   calibrations happen here.

4. **setup_stage2**: Completes ADC cal, re-syncs links, starts block memory
   delay calibration.

5. **clks_enable**: SERDES calibration per link — freezes/unfreezes background
   cal, runs init cal for high lane rates, enables JTX links.

6. **link_enable**: Enables JRX links, triggers SYSREF.

7. **post_setup stages**: Final link status checks, IRQ setup, sets
   `is_initialized` flag.

### How to port

The no-OS JESD204 framework uses the same callback structure. You need to:

1. Create a `jesd204_dev_data` struct with the callbacks
2. Register it during `ad9088_init()` via `jesd204_dev_register()`
3. Port each callback, replacing:
   - `struct device *dev = jesd204_dev_to_device(jdev)` -> remove (use pr_err)
   - `dev_err(dev, ...)` -> `pr_err(...)`
   - `struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev)` -> same pattern
     works in no-OS

### Example: porting link_init

Kernel:
```c
static int ad9088_jesd204_link_init(struct jesd204_dev *jdev,
                                     enum jesd204_state_op_reason reason,
                                     struct jesd204_link *lnk) {
    struct device *dev = jesd204_dev_to_device(jdev);
    struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
    struct ad9088_phy *phy = priv->phy;
    // ... fill lnk fields from phy->profile ...
}
```

no-OS:
```c
static int ad9088_jesd204_link_init(struct jesd204_dev *jdev,
                                     enum jesd204_state_op_reason reason,
                                     struct jesd204_link *lnk) {
    struct ad9088_jesd204_priv *priv = jesd204_dev_priv(jdev);
    struct ad9088_phy *phy = priv->phy;
    // ... identical logic, just remove dev_dbg/dev_err -> pr_debug/pr_err ...
}
```

### The Apollo HAL calls are identical

All the `adi_apollo_*` function calls are the same between kernel and no-OS.
That's the whole point of the HAL — you just need to translate the Linux
framework glue.

### Progress: Step 5 sub-steps

The FSM port is being done incrementally, one callback at a time, testing on
hardware after each sub-step.

#### Sub-step 1: Registration scaffolding + LINK_INIT — DONE (2026-07-20)

**Framework prerequisite**: Added `JESD204_OP_OPT_POST_SETUP_STAGE1/2/3` to the
no-OS JESD204 framework enum in `include/jesd204.h`. The kernel has these but
no-OS was missing them. No changes to `jesd204-fsm.c` needed — the FSM loops
over `__JESD204_MAX_OPS` automatically.

**Files created/modified**:
- `drivers/rf-transceiver/apollo/ad9088_jesd204_fsm.c` (new) — contains:
  - `ad9088_jesd204_link_init()` — per-link callback, fills `jesd204_link`
    from Apollo profile (JRX for deframer/TX, JTX for framer/RX)
  - `ad9088_jesd204_uninit()` — no-op cleanup callback for DEVICE_INIT
  - `jesd204_ad9088_init` struct — `max_num_links=4`, `num_retries=0`
  - Debug prints showing full link params after LINK_INIT
- `drivers/rf-transceiver/apollo/ad9088.h` — added:
  - Include guard (`#ifndef __AD9088_H__`)
  - 8 link ID defines (`DEFRAMER_LINK_A0_TX` through `FRAMER_LINK_B1_RX`)
  - `extern const struct jesd204_dev_data jesd204_ad9088_init`
- `drivers/rf-transceiver/apollo/ad9088.c` — added:
  - `jesd204_dev_register()` in `ad9088_init()` after device probe
  - `jesd204_dev_unregister()` in `ad9088_remove()` before hw_close
  - Newlines in `ad9088_log_write()` HAL callback (was missing `\n`)
- `projects/ad9088/src/examples/basic/basic_example.c` — added:
  - JESD204 topology with HMC7044 (sysref), AXI JESD RX, AXI JESD TX,
    AD9088 (top device), using 2 links (Side A Link 0)
  - `jesd204_topology_init()` + `jesd204_fsm_start()`
- `projects/ad9088/src.mk` — added `ad9088_jesd204_fsm.c`

**Hardware verification (VCU118)**:
```
Link4: RX(framer) JESD204C L=8 M=4 F=1 K=256 N=16 NP=16 S=1 HD=1 E=1
Link4: lane_rate=10312500 kHz sample_rate=20000000000 Hz sample_rate_div=16 subclass=0 encoder=64b66b
Link0: TX(deframer) JESD204C L=8 M=4 F=1 K=256 N=16 NP=16 S=1 HD=1 E=1
Link0: lane_rate=10312500 kHz sample_rate=20000000000 Hz sample_rate_div=16 subclass=0 encoder=64b66b
```

> **Note (2026-08-13):** this `lane_rate=10312500 kHz` was captured with an
> earlier usecase profile and does **not** describe the profile now built
> (`204C_M4_L8_NP16_20p0_4x2.bin`). The current link runs at
> **20 625 000 kHz**, matching `AD9088_LANE_RATE_KHZ` in `common_data.h` — which
> is therefore correct and must not be "fixed" to the figure above. The
> cross-check: for 204C/64b66b with E=1 the LEMC rate is
> `lane_rate / (66 * 32) = 20.625e9 / 2112 = 9 765 625 Hz`, exactly
> `SYSREF_CLK_MHz` in the kernel devicetree, and exactly the BSYNC frequency the
> ADF4030 is programmed to. At 10.3125 Gbps the arithmetic would give half that
> and the SYSREF tree would not line up.

FSM runs through all stages. LINK_INIT fills link params correctly. AXI JESD
cores report "set lane rate failed (-22)" because ADXCVR is not yet initialized
(no `adxcvr_init` structs or calls in the project). ADXCVR IP exists in HDL at:
- RX: `XPAR_AXI_APOLLO_RX_XCVR_BASEADDR = 0x44a60000`
- TX: `XPAR_AXI_APOLLO_TX_XCVR_BASEADDR = 0x44b60000`

**Next**: Add ADXCVR initialization (`adxcvr_init` structs in common_data,
`adxcvr_init()` calls in basic_example, `jesd204_clk` wiring for lane clocks).
Then continue with sub-step 2 (LINK_SETUP callback).

#### Sub-step 1b: ADXCVR initialization — DONE (2026-07-22)

The AXI JESD cores need the GTY transceivers (ADXCVR) configured before they
can lock a lane rate. The HDL has ADXCVR IP but the software never wired it up.
Pattern followed: `projects/ad9081/src/app_jesd.c`.

**Files modified**:
- `src/platform/xilinx/parameters.h` — added:
  ```c
  #define RX_XCVR_BASEADDR	XPAR_AXI_APOLLO_RX_XCVR_BASEADDR  /* 0x44a60000 */
  #define TX_XCVR_BASEADDR	XPAR_AXI_APOLLO_TX_XCVR_BASEADDR  /* 0x44b60000 */
  ```
- `src/common/common_data.h` — added `axi_adxcvr.h` / `jesd204_clk.h`
  includes and `rx_adxcvr_ip` / `tx_adxcvr_ip` externs.
- `src/common/common_data.c` — added `tx_adxcvr_ip` and `rx_adxcvr_ip`
  `adxcvr_init` structs. **Both use `ADXCVR_SYS_CLK_QPLL0`** (see gotcha
  below), `out_clk_sel = ADXCVR_PROGDIV_CLK`, `lane_rate_khz =
  AD9088_LANE_RATE_KHZ` (20625000), `ref_rate_khz = 312500`. RX has
  `lpm_enable = 1`, TX has `lpm_enable = 0`.
- `src/examples/basic/basic_example.c` — added `adxcvr_init()` calls for
  TX and RX (between DMAC init and JESD init), `jesd204_clk` structs, and
  `no_os_clk_desc` lane clocks wired via `jesd204_clk_ops`:
  ```c
  tx_jesd_clk.xcvr = tx_adxcvr;          /* after adxcvr_init */
  rx_lane_clk.platform_ops = &jesd204_clk_ops;
  rx_lane_clk.dev_desc = &rx_jesd_clk;
  rx_jesd204_ip.lane_clk = &rx_lane_clk; /* before axi_jesd204_rx_init */
  rx_jesd_clk.jesd_rx = rx_jesd;         /* after axi_jesd204_rx_init */
  ```
  Plus new cleanup labels `error_rx_adxcvr` / `error_tx_adxcvr`.

No `src.mk` changes needed — `axi_adxcvr.c`, `jesd204_clk.c`, and
`xilinx_transceiver.c` were already listed.

**GOTCHA — CPLL cannot reach 20.625 Gbps**: The AD9081 reference uses CPLL for
RX, so the first attempt copied that. On the AD9088 (lane rate 20.625 Gbps) the
RX ADXCVR failed with:
```
DEBUG: CPLL: failed to find setting for lane rate 20625000 kHz with reference clock 312500 kHz
RX ADXCVR initialization failed
```
The UltraScale+ GTY CPLL VCO tops out ~6.25 GHz (~12.5 Gbps max line rate).
20.625 Gbps requires QPLL0 (VCO 9.8–16.375 GHz → ×2). Fix: set RX
`sys_clk_sel = ADXCVR_SYS_CLK_QPLL0` (TX already used QPLL0). Both directions
now use QPLL0.

**Clock rate reference (from working kernel link status, UC that runs 20.625
Gbps lanes)**:
| Field | Value |
|---|---|
| Lane rate | 20625.000 MHz (`AD9088_LANE_RATE_KHZ` = 20625000) |
| Lane rate / 66 = link clock | 312.500 MHz |
| Device clock (reported/desired) | 312.500 MHz |
| ADXCVR ref clock (HMC ch10 FPGA_REFCLK) | 312.500 MHz (`ref_rate_khz`) |
| LEMC rate | 9.765 MHz |

Note: there is a second, slower UC (10.3125 Gbps lanes, 156.25 MHz device/ref).
The current config targets the 20.625 Gbps UC.

**Debugging tip — enabling `pr_debug`**: `pr_debug` in no-OS is gated by
`NO_OS_LOG_LEVEL`. It only compiles in when the level is **exactly** DEBUG (7):
```makefile
# projects/ad9088/Makefile, after generic_variables.mk include
CFLAGS += -DNO_OS_LOG_LEVEL=7
```
Because the macro is baked into every object, changing it requires a full
`make reset && make` (a plain rebuild reuses stale objects). Remove the flag
once done — it produces very verbose DRP-write spam. This is how the CPLL
failure above was diagnosed.

**Status**: ADXCVR builds clean and configures the TX path; RX QPLL0 fix
applied and builds clean — pending on-hardware verification that the link
reaches Link Status = DATA.

#### Sub-step 1c: Clock-plan consistency fix — DONE (2026-07-23)

The QPLL locked but `adxcvr_clk_enable()` spun its 100-iteration buffer-error
retry loop (each retry re-prints `rx_adxcvr: OK`) and ended with:
```
QPLL RX buffer underflow error, status: 0x61
QPLL RX buffer overflow error, status: 0x61
```
Status `0x61` = STATUS(bit0) + UNDERFLOW(bit5) + OVERFLOW(bit6). **Both** under-
and overflow set at once is the signature of a device/link clock at the wrong
rate relative to the incoming lane data — a 2:1 mismatch, not "no data".

**Root cause**: inconsistent clock plan. The loaded firmware was
`204C_M4_L8_NP16_20p0_4x4.bin` (L8 → 10.3125 Gbps/lane, needs 156.25 MHz link
clock), but the software constants and HMC dividers all described the L4 plan
(`AD9088_LANE_RATE_KHZ = 20625000`, HMC core-clock dividers = 8 → 312.5 MHz).
Tellingly, every HMC channel comment already said `// 156.25` while the divider
value produced 312.5 — the values, not the intent, were wrong.

**Fix applied**: switched firmware to `204C_M4_L8_NP16_20p0_4x2.bin` (still L8,
but half the decimation → 20.625 Gbps/lane, `sample_rate_div=8`). This makes
firmware + `AD9088_LANE_RATE_KHZ=20625000` + the 312.5 MHz HMC tree all
internally consistent at 20.625 Gbps.

**Verified on HW**: ADXCVR buffer status improved **0x61 → 0x21** — the overflow
bit cleared, confirming the gross rate mismatch is gone. Remaining `0x21`
(underflow + ready) is a *different, expected* problem (see sub-step 2): the
buffer starves because the AD9088 framer isn't streaming synced data yet.

**Clock-plan rule of thumb**: halving the lane count halves the device/link
clock. L4 @ 20.625 Gbps → 312.5 MHz core clock (HMC div 8); L8 @ 10.3125 Gbps
→ 156.25 MHz (HMC div 16). Keep firmware, `AD9088_LANE_RATE_KHZ`, and the HMC
dividers describing the *same* use-case.

The link previously failed at `WAIT_BS` (FPGA deframer state 1/4: RESET →
**WAIT_BS** → BLOCK_SYNC → DATA) with PHY buffer underflow `0x21`. Root cause is
structural, not a rate bug: the no-OS AD9088 FSM only implemented `LINK_INIT` +
`DEVICE_INIT`, while the kernel has 12 callbacks. The device-side stages that
make the link actually run — `LINK_SETUP`, `CLOCKS_ENABLE`, `LINK_ENABLE` — were
all missing from the `jesd204_ad9088_init` op table. All required
`JESD204_OP_*` enums already exist in `include/jesd204.h`. These are now being
ported one op at a time (sub-steps 2–4 below).

#### Sub-step 2: LINK_SETUP — DONE (2026-07-23)

Ported `ad9088_jesd204_link_setup` (per_device, `JESD204_STATE_OP_MODE_PER_DEVICE`)
and registered `[JESD204_OP_LINK_SETUP]`. The callback:
- enables Apollo JTx/JRx links A0/B0, disables A1/B1;
- powers up the Rx (ADC) and Tx (DAC) blocks via SPI;
- pulses the Rx/Tx datapath reset;
- selects subclass from the profile and enables the MCS SYSREF receiver when
  subclass 1;
- freezes ADC background cal and runs the MCS dynamic-sync sequences.

Deviations from kernel: no-OS `ad9088_check_apollo_error(ret, name)` has no
`dev` arg; the per-device `pr_debug` drops `link_id` (no `lnk` in a per_device
callback).

#### Sub-step 3: CLOCKS_ENABLE — DONE (2026-07-23)

Ported `ad9088_jesd204_clks_enable` (per_link) and registered
`[JESD204_OP_CLOCKS_ENABLE]`. Per link it:
- maps the link id to its SERDES 12-pack (A/B);
- on init > 8 Gbps runs `adi_apollo_serdes_jrx_init_cal` (**mandatory high-rate
  SERDES cal**), and freezes/unfreezes the JRx background cal above 16 Gbps;
- enables the framer (JTx) link on init, disables it on teardown.

Also added the `ad9088_to_link()` helper (link id → Apollo link-select bitmask)
in `ad9088.c`/`ad9088.h`, used here and by `LINK_ENABLE`.

Deviations from kernel: warmboot-from-FW cal path is not ported
(`cal_data_loaded_from_fw` field absent) — init-cal is hardcoded to
`ADI_APOLLO_INIT_CAL_ENABLED`; log lines use numeric `link_id` (no
`ad9088_fsm_links_to_str[]`).

#### Sub-step 4: LINK_ENABLE — DONE (2026-07-23)

Ported `ad9088_jesd204_link_enable` (per_link) and registered
`[JESD204_OP_LINK_ENABLE]` with **`.post_state_sysref = true`**. For deframer
(JRx) links it enables the Apollo JRx link on init / disables on teardown.

The `post_state_sysref` flag is the key piece: with the top device (AD9088) now
setting it, the framework calls `jesd204_sysref_async()` after LINK_ENABLE,
pulsing SYSREF through the HMC7044 `sysref_cb` — the alignment event the
deframer needs to leave `WAIT_BS`. (Previously no no-OS driver set this flag, so
no automatic SYSREF fired during bring-up.)

**Status**: `LINK_SETUP` + `CLOCKS_ENABLE` + `LINK_ENABLE` are the minimum
device-side set for link bring-up. Not yet verified on HW.

#### Sub-step 5: OPT_SETUP_STAGE1 + OPT_SETUP_STAGE2 — DONE (2026-07-24)

**Why this came before LINK_RUNNING**: an on-HW run of the sub-step 1-4 build
failed with the SERDES JRx init-cal (in `CLOCKS_ENABLE`) returning
`SERDES cal error (-85)` (`init_err_codes[0]=0x207`), then
`QPLL RX buffer underflow status 0x21` and `Link4 status failed (WAIT_BS)`. The
op table jumped straight `LINK_SETUP -> CLOCKS_ENABLE`, skipping the two
calibration stages the kernel runs in between — and the SERDES cal depends on
`sysclk_cond_cal()` from stage1. Missing stages, not a rate bug.

Ported `ad9088_jesd204_setup_stage1` and `ad9088_jesd204_setup_stage2` (both
per_device, `JESD204_STATE_OP_MODE_PER_DEVICE`) and registered
`[JESD204_OP_OPT_SETUP_STAGE1]` / `[JESD204_OP_OPT_SETUP_STAGE2]` between
`LINK_SETUP` and `CLOCKS_ENABLE`.

**stage1**: records rx/tx en masks; sets ADC Nyquist zone; configures + runs
`adi_apollo_sysclk_cond_cal` (clock conditioning — the SERDES cal prereq);
inspects JRx/JTx links (populates `phy->jrx_lanes[]`); calc+set JRx phase
adjust, set JTx phase adjust 0; starts ADC init-cal.

**stage2**: completes ADC init-cal; re-sets Nyquist zone (cal can clobber it);
re-runs `adi_apollo_clk_mcs_dyn_sync_rxtxlinks_sequence_run`.

Also ported the two helpers `ad9088_inspect_jrx_link_all()` /
`ad9088_inspect_jtx_link_all()` into `ad9088.c`/`ad9088.h` (swap `dev_info` ->
`pr_info`, drop the `dev` arg on `ad9088_check_apollo_error`).

Deviations from kernel:
- **Nyquist zone value**: no-OS `ad9088_dt.c` stores `rx_nyquist_zone[side][dp]`
  as `zone - 1` (0/1) in a 2D array, while the kernel keeps the raw scalar
  (1/2) and passes it straight to the HAL. The HAL expects 1 or 2, so the
  callbacks pass `phy->rx_nyquist_zone[0][0] + 1`.
- **Warmboot/NVM cal paths not ported** (`cal_data_loaded_from_fw`,
  `is_adc_nvm_fused`): hardcoded to `ADI_APOLLO_SYSCLKCONDITIONING_ENABLED` and
  `ADI_APOLLO_INIT_CAL_ENABLED` (run-from-scratch).
- **Block-memory delay cal skipped** (`cddc/fddc_sample_delay_en` fields
  absent) — stage2 steps 4-5 left as a comment stub.

Added `#include "no_os_util.h"` to `ad9088.c` (for `NO_OS_ARRAY_SIZE`) and
`adi_apollo_cfg.h` / `adi_apollo_sysclk_cond.h` to `ad9088.h`.

**Status**: builds clean on VCU118 (MicroBlaze). Verified on HW: the stages
run (clock-conditioning cal executes, JRx/JTx inspect prints the correct
`L=8 M=4 F=1 S=1 Np=16` config, ADC init-cal runs the full ~29 s instead of
bailing in 1.2 s). **Partial result** — the stages fixed the rx_adxcvr retry
storm and let the real ADC cal run, but the SERDES JRx init-cal *still*
failed with the same `-85` / `init_err_codes[0]=0x207` / `init_err_cals[0]=0x04`
and the link still stopped at `WAIT_BS`. Root cause turned out to be the lane
crossbar, not the calibration stages — see sub-step 5b.

#### Sub-step 5b: Lane crossbar mapping fix — DONE (2026-07-24), WAIT_BS CLEARED

**Symptom**: after sub-step 5, SERDES JRx init-cal failed with
`init_err_codes[0]=0x207` = `APOLLO_CPU_SERDES_RX_LANE00_ERROR` ("Lane 0
Error, Unexpected HW behavior") and `init_err_cals[0]=0x04` =
`APOLLO_INIT_CAL_MSK_IC_SERDES_RX`. A *per-lane* failure on lane 0 points at
the crossbar, not the cal recipe: the AD9088 deframer's logical lane 0 was
being told to read a physical SERDES lane the FPGA wasn't driving.

**Root cause (two compounding bugs):**

1. **Identity lane map instead of the board's crossbar.** no-OS hardcoded
   `AD9088_LOGICAL_LANE_MAPPING = {0,1,...,11}` for all four maps. The kernel
   loads board-specific non-identity crossbars from the devicetree. For our
   firmware (`204C_M4_L8_NP16_20p0_4x2`, matched by
   `vcu118_ad9084_204C_M4_L8_NP16_20p0_4x2_CLL.dts` which `#include`s
   `vcu118_ad9084.dts`) the correct maps are (dts lines 792-795):
   ```
   adi,jrx0-physical-lane-mapping = <10 8 9 11 5 1 3 7 4 6 2 0>;
   adi,jtx0-logical-lane-mapping  = <11 2 3 5 10 1 9 0 6 7 8 4>;
   adi,jrx1-physical-lane-mapping = <4 6 2 0 1 7 10 3 5 8 9 11>;
   adi,jtx1-logical-lane-mapping  = <3 9 5 4 2 6 1 7 8 11 0 10>;
   ```
   Note the asymmetry (mirrors kernel `ad9088_dt.c`): the **JTx (framer)**
   crossbar comes from the *logical*-lane-mapping property, the **JRx
   (deframer)** crossbar comes from the *physical*-lane-mapping property.
   Renamed the `ad9088_init_param` fields to `jrx0/1_physical_lane_mapping` +
   `jtx0/1_logical_lane_mapping` and added the four board maps as
   `AD9088_{TX0,TX1,RX0,RX1}_*_LANE_MAPPING` macros in `common_data.h`.

2. **`&phy` passed where `phy` expected.** `ad9088_dt.c` called
   `ad9088_jesd_lane_setup(&phy, init_param)` — `&phy` is `ad9088_phy **`
   but the function takes `ad9088_phy *`. Every `phy->profile...lane_xbar[]`
   and `lane_enables` write landed in the caller's stack pointer instead of
   the device profile, so **neither** the identity map nor the corrected map
   ever reached `phy->profile`; the device silently ran on whatever crossbar
   was baked into `usecase.bin`. Fixed to pass `phy`. (Also dropped a stray
   `return 0;` from the `void` function.)

**Status**: VERIFIED on HW. SERDES JRx init-cal now completes with no error,
`tx_adxcvr` and `rx_adxcvr` both report OK @ 20625000 kHz, and bring-up
reaches "Project configured" — `WAIT_BS` cleared, link is up.

#### Sub-step 6: LINK_RUNNING — DONE (2026-07-27), HW-verified on VCU118

Ported `ad9088_jesd204_link_running` (per_link) and registered
`[JESD204_OP_LINK_RUNNING]` (no `.mode`, no `.post_state_sysref`). The callback
validates and prints per-link JESD status once the link is running:
- on teardown (`reason != INIT`) it clears `phy->is_initialized` and returns;
- for deframer (JRx, `is_transmit`) links it prints the JRx phase difference and
  the JRx (device-RX) link/lane status;
- for framer (JTx) links it prints the JTx (device-TX) link status.
  (The kernel calls the RX status helper on the `is_transmit` branch and the TX
  helper on the framer branch — preserved verbatim to avoid drift.)
Returns `JESD204_STATE_CHANGE_ERROR` if a status-print helper returns < 0.

**Collateral ported into `ad9088.c` / `ad9088.h`** (all mirror the kernel):
- `ad9088_fsm_links_to_str[]` — link-id → human name table (8 entries).
- `ad9088_jrx_204c_states[]` — 204C lane-state strings (static).
- `ad9088_print_link_phase()` — `adi_apollo_jrx_phase_diff_get` + print.
- `ad9088_jesd_tx_link_status_print()` — polls `adi_apollo_jtx_link_status_get`;
  204C checks `(stat & 0x60)==0x60`, 204B checks `(stat & 0xF0)==0x70`;
  retry loop with `no_os_mdelay(20)`.
- `ad9088_jesd_rx_link_status_print()` — polls `adi_apollo_jrx_link_status_get`;
  subclass-dependent mask (0x60 / 0x20); per-lane loop via
  `adi_apollo_jrx_j204c/b_lane_status_get` using the physical lane from
  `profile.jrx[...].rx_link_cfg[...].lane_xbar[i]`.

Deviations from kernel (translation glue only):
- `dev_info/dev_err(&phy->spi->dev, ...)` → `pr_info/pr_err`;
  `msleep(20)` → `no_os_mdelay(20)`; `BIT(n)` → `NO_OS_BIT(n)`.
- no-OS `jesd204_link.link_id` is `uint32_t`, so printed link ids are cast to
  `(int)` for `%d` (avoids `-Wformat`). Array indexing stays uncast.
- `ad9088_status_show` (debugfs-only) was **not** ported.

**Status**: builds clean on VCU118 (MicroBlaze) with no new warnings, and
**HW-verified on VCU118 (2026-07-27)**. Bring-up log shows, on Link A0:
- JTx framer: `204C PLL locked, PHASE established, MODE valid`;
- JRx deframer: `Phase Difference 23`, all 8 lanes `204C Lane-N@M status: Link
  is good` (lane state `0x6`), and `User status: Ready`.
- FSM proceeds past LINK_RUNNING to "Project configured" with no
  `STATE_CHANGE_ERROR`.

Note: the final line reads `SYSREF Phase: Unlocked`. That was **cosmetic and
expected** at the time — this profile ran **subclass 0**, so the pass/fail mask
was `0x20` (User status only) and the SYSREF-phase bit `0x40` was deliberately
excluded (`/* Ignore SYSREF Phase */` in `ad9088_jesd_rx_link_status_print`).
Behavior matched the kernel exactly.

**Superseded 2026-08-13:** `AD9088_RX/TX_JESD_SUBCLASS` is now **1**, so the mask
is `0x60` and an unlocked SYSREF phase *fails* the link. On hardware the line now
reads `SYSREF Phase: Locked`. The lock did **not** require MCS cal, so it was
never gated on the deferred Step 8 work as this paragraph originally assumed — see
`SUBCLASS1_ENABLEMENT.md` Stage C.

#### Sub-step 7: POST_SETUP + POST_RUNNING stages — DONE (2026-07-27), HW-verified on VCU118

Ported the four kernel POST callbacks into `ad9088_jesd204_fsm.c` and registered
`[JESD204_OP_OPT_POST_SETUP_STAGE1/2/3]` + `[JESD204_OP_OPT_POST_RUNNING_STAGE]`
(all `.mode = JESD204_STATE_OP_MODE_PER_DEVICE`, matching the kernel). This
completes the full kernel bring-up sequence in no-OS.

- **`ad9088_jesd204_post_setup_stage1`** — effective stub *as of this step*. Our
  config wired no clock chips (`phy->iio_adf4030`/`iio_adf4382` always NULL), so
  on INIT it printed `"Skipping MCS calibration"` and returned DONE. The kernel
  MCS calibration body was deferred to **Step 8** — and **implemented there on
  2026-08-14**, with the NULL IIO handles replaced by the `ad9088_bsync_ops` /
  `ad9088_clk_ops` shim. Examples that supply no ops still take the skip path.
- **`ad9088_jesd204_post_setup_stage2`** — ports the `if (phy->trig_sync_en)`
  HAL sequence (`adi_apollo_clk_mcs_sync_trig_map` → `trig_sync_enable(0)` →
  `trig_reset_disable` → `trig_reset_dsp_enable` → `trig_sync_enable(1)`), with
  `pr_err` on each error; teardown does `trig_sync_enable(0)` +
  `trig_reset_disable`. The adf4030 `aion_*` alignment block was not ported at
  this step; **added in Step 8 on 2026-08-14**, gated on
  `aion_background_serial_alignment_en`, which is false by default so the
  behaviour above is unchanged.
- **`ad9088_jesd204_post_setup_stage3`** — ports the
  `triq_req_gpio && trig_sync_en` GPIO pulse via `no_os_gpio_set_value(1)` /
  `no_os_udelay(1)` / `no_os_gpio_set_value(0)`.
- **`ad9088_jesd204_post_setup_stage4`** (POST_RUNNING) — teardown clears
  `phy->is_initialized`. INIT ports the `if (phy->trig_sync_en)` phase-check
  block (`adi_apollo_hal_bf_wait_to_set` with
  `BF_TRIGGER_SYNC_DONE_A0_INFO(MCS_SYNC_MCSTOP0)`,
  `adi_apollo_clk_mcs_trig_phase_get`, `period_fs` via `no_os_div64_u64_rem`,
  25%–75% margin `pr_warning`, then `trig_sync_enable(0)` + `trig_reset_disable`),
  then unconditionally: `ad9088_print_sysref_phase(phy)`,
  **`adi_apollo_adc_bgcal_unfreeze()`** (background ADC cal resumes), and
  **`phy->is_initialized = true`**.

**Collateral ported into `ad9088.c` / `ad9088.h`**:
- `ad9088_print_sysref_phase()` — loops 5×, `adi_apollo_clk_mcs_sysref_phase_get`,
  prints `no_os_sign_extend32(sysref_phase, 9)`.

Deviations from kernel (translation glue only):
- `dev_info/dev_err/dev_dbg` → `pr_info/pr_err/pr_debug`; `udelay` →
  `no_os_udelay`; `sign_extend32` → `no_os_sign_extend32`; `div64_u64` →
  `no_os_div64_u64_rem` (uint64_t divisor `dev_clk_freq_Hz`, needs a real
  remainder pointer — the helper writes `*remainder` unconditionally).
- `period_fs` is `uint32_t` (= `long unsigned int` on MicroBlaze), so it is cast
  to `(unsigned int)` for `%u` (avoids `-Wformat`).
- **Not ported at this step** (un-ported infrastructure / deferred): MCS
  calibration and adf4030 background serial alignment — both **since ported in
  Step 8, 2026-08-14**; `hsci_disable_after_initial_configuration` (hsci fields
  commented out in the no-OS phy struct) and DEBUG-only sync_input_count / mcs
  status prints, which remain un-ported.

**Status**: builds clean on VCU118 (MicroBlaze) with no new `-Wall -Wextra`
warnings, and **HW-verified on VCU118 (2026-07-27)**. The trig-sync paths in
stages 2/4 compile but are inert for this profile (`trig_sync_en = false`); they
are ported for fidelity. Bring-up log confirms:
- stage1 prints `Skipping MCS calibration` (adf chips NULL);
- stage2/3 silently skipped (`trig_sync_en = false`);
- stage4 runs `ad9088_print_sysref_phase()` (`SYSREF_PHASE = 0 (TRY0..4)` —
  reads 0 with no MCS/trig-sync active, becomes meaningful after Step 8),
  `adi_apollo_adc_bgcal_unfreeze()` returns OK, and `is_initialized = true`;
- FSM reaches "Project configured" with no `STATE_CHANGE_ERROR`, LINK_RUNNING
  output (8 lanes good, User status Ready) unchanged.

---

## Step 6: NCO Control Functions

**Goal**: Port the NCO (Numerically Controlled Oscillator) frequency tuning
functions.

### What NCOs do

The AD9088 has two levels of NCOs:
- **CNCO** (Coarse NCO): 4 per side, 32-bit FTW, shifts the main datapath
- **FNCO** (Fine NCO): 8 per side, 48-bit FTW, shifts individual channelizers

### Functions to port from kernel ad9088.c

```c
int adi_ad9088_calc_nco_ftw(...)   // Calculate frequency tuning word from Hz
int adi_ad9088_calc_nco_freq(...)  // Calculate Hz from FTW (reverse)
```

Plus the higher-level wrappers that set/get NCO frequencies via the Apollo HAL:
```c
// These call adi_apollo_cnco_freq_set() / adi_apollo_fnco_freq_set()
// internally after calculating FTW
```

### Translation notes

- `do_div(a, b)` -> use regular C division (no-OS has 64-bit support)
- `clamp_t(int64_t, val, lo, hi)` -> write a simple macro or inline
- `adi_api_utils_*` functions are in the Apollo HAL, already available

### When you need this

NCO functions are needed for IIO channel attributes (Step 10) and for runtime
frequency tuning. They can be ported independently.

### Progress (2026-07-28) — ported, HW-verified on VCU118

Ported into `ad9088.c` / `ad9088.h`:

- **`adi_ad9088_calc_nco_ftw()`** — verbatim from kernel `ad9088.c:25`. Glue
  translations only: `do_div(f_clamp, div)` -> `f_clamp /= div`; `clamp_t` ->
  `no_os_clamp` (computes `half = f_clamp >> 1` first to avoid a macro
  double-eval); `dev_dbg` -> `pr_debug`. `adi_api_utils_ratio_decomposition`
  is the same HAL call (declared in `adi_utils.h`).
- **`adi_ad9088_calc_nco_freq()`** — verbatim from kernel `ad9088.c:76`. Uses
  `adi_api_utils_mult_128/add_128/div_128` (all in `adi_utils.h`, matching
  signatures). The kernel passes `nco_shift` (an `s64*`) straight into
  `div_128`'s `uint64_t *` out-param — preserved with a `(uint64_t *)` cast.
- **Standalone phy-level wrappers** (chosen over inlining in IIO, since IIO is
  Step 10): `ad9088_{set,get}_cnco_freq(phy, terminal, side, cddc_num, freq)`
  and `ad9088_{set,get}_fnco_freq(...)`. These extract the exact logic from the
  kernel IIO `CDDC_NCO_FREQ` / `FDDC_NCO_FREQ` read/write cases (kernel
  `ad9088.c:1564-1602`, `1715-1807`) minus the sysfs/`kstrtoll` glue. The IIO
  layer in Step 10 becomes a thin caller of these.

Mask derivation without a `chan_map` (no-OS has none yet): added static
`cnco_masks[side][4]` (`ADI_APOLLO_CNCO_{A,B}{0-3}`) and `fnco_masks[side][8]`
(`ADI_APOLLO_FDDC_{A,B}{0-7}`) tables in `ad9088.h`, mirroring the kernel's
`cddc_masks_all` / `fddc_masks`. Profile indices follow the kernel:
`cddc_pi = cddc_num % ADI_APOLLO_CDUCS_PER_SIDE`,
`fddc_pi = fddc_num % ADI_APOLLO_FDUCS_PER_SIDE`, and for the FNCO's parent CDDC
`cddc_pi = (fddc_num / 2) % ADI_APOLLO_CDUCS_PER_SIDE`.

Struct fields added to `ad9088_phy` (kernel `ad9088.h:298-299`), needed by
`calc_nco_ftw`: `cnco_dual_modulus_mode_en`, `fnco_dual_modulus_mode_en`.
Header includes added: `adi_apollo_cnco.h`, `adi_apollo_fnco.h`,
`adi_apollo_cddc.h`, `adi_apollo_cduc.h`, `adi_utils.h`.

Deviations from kernel (translation glue only):
- `div_s64` -> `no_os_div_s64`; `do_div` -> `/`; `clamp_t` -> `no_os_clamp`.
- FNCO phase offset kept as kernel: `main_phase_offset =
  no_os_div_s64(fnco_phase * 14073748835533, 18000LL)`.
- **Dual-modulus mode is off** by default (both `*_dual_modulus_mode_en` fields
  init to 0 in `phy`), so `calc_nco_ftw` forces `frac_a=0, frac_b=1` — matches
  the current profile.

**HW test hooks** (temporary, in `basic_example.c` after "Project configured",
to be removed/folded into IIO at Step 10): four set->get->print round-trips on
side-A datapath 0 — RX CNCO (125 MHz), RX FNCO (40.2 MHz), TX CNCO (125 MHz),
TX FNCO (40.2 MHz). Each verifies the round-trip FTW<->Hz math on hardware for
both terminals and both NCO stages.

Bug found + fixed via HW test (2026-07-28): TX FNCO read back 60.3 MHz for a
40.2 MHz set (exactly 1.5x). Cause: `ad9088_set_fnco_freq` computed `cddc_dcm`
with `adi_apollo_cddc_dcm_bf_to_val` for *both* terminals, while the getter
(and the kernel write path, `ad9088.c:1764`) uses `adi_apollo_cduc_interp_bf_to_val`
for TX and `adi_apollo_cddc_dcm_bf_to_val` for RX. TX interpolation vs RX
decimation map the same `drc_ratio` enum to different numeric ratios, so set/get
divided `f` differently on the TX fine path. Fixed the setter to branch on
terminal like the getter.

**Status**: builds clean on VCU118 (MicroBlaze) — no new `-Wall -Wextra`
warnings from any NCO function or the test hook. **HW-verified on VCU118
(2026-07-28)**: all four round-trips (RX/TX CNCO/FNCO) correct — 125 MHz CNCO
reads 124999997 Hz (expected 32-bit FTW quantization), FNCO reads 40.2 MHz
exactly. TX FNCO passed after the `bf_to_val` fix above.

---

## Step 7: Calibration

**Goal**: Port the essential calibration sequences.

### Clock conditioning calibration

Called from `setup_stage1` in the JESD204 FSM:
```c
adi_apollo_cfg_clk_cond_cal_cfg_set(device, cfg);
adi_apollo_sysclk_cond_cal(device);
```

### ADC init calibration

Also in `setup_stage1`:
```c
adi_apollo_adc_init_cal_start(device, chans, init_cal_cfg);
// ... wait ...
adi_apollo_adc_init_cal_complete(device, chans);  // in setup_stage2
```

### SERDES calibration

In `clks_enable`:
```c
adi_apollo_serdes_jrx_init_cal(&phy->ad9088, serdes, init_cal);
adi_apollo_serdes_jrx_bgcal_freeze/unfreeze(...);
```

### Calibration save/restore (optional)

`ad9088_cal.c` in the kernel implements cal save/restore to/from binary blobs.
This is nice-to-have for reducing boot time but not required for initial
bring-up. Skip for now.

### All cal functions are Apollo HAL calls — no porting of logic needed, just
the orchestration in the JESD204 callbacks.

---

## Step 8: MCS (Multi-Chip Sync)

**Goal**: Port multi-chip synchronization support.

### What MCS does

When multiple AD9088 devices share a system, their clocks and data paths must
be synchronized. MCS uses SYSREF pulses to align internal dividers.

### Kernel source

`ad9088_mcs.c` — functions called from IIO attributes and JESD204 callbacks:
```c
ad9088_mcs_init_cal_setup()
ad9088_mcs_init_cal_validate()
ad9088_mcs_tracking_cal_setup()
ad9088_mcs_tracking_cal_validate()
ad9088_delta_t_measurement_set/get()
```

### When you need this

MCS is needed for multi-device systems. For single-device bring-up, the basic
MCS calls in the JESD204 FSM (`adi_apollo_clk_mcs_dyn_sync_*`) are sufficient.
Full MCS porting can be deferred.

### Status: DONE — HW-verified on VCU118, 2026-08-14, first attempt

MCS *calibration* is **ported and HW-verified**:

```
Total BSYNC path delay 2175424 fs
MCS Init Cal Validation: Passed
SYSREF_PHASE = 40 (TRY0..TRY4)      <- was 433 before MCS cal
```

`SYSREF_PHASE` is in units of **Fclk** (20 GHz device clock, 50 ps/count), so the
cal removed 19.65 ns of external-to-internal SYSREF offset (21.65 ns → 2.00 ns),
repeatably, with no link regression. `ad9088_jesd204_post_setup_stage1` is no
longer a stub;
`drivers/rf-transceiver/apollo/ad9088_mcs.c` exists; and the dead
`iio_adf4030`/`iio_adf4382` handles were replaced by an `ad9088_bsync_ops` /
`ad9088_clk_ops` function-pointer shim in `ad9088_init_param`, wired up by
`ad9088_mcs_ops_bind()` in `common_data.c`. `dma_example` and `iio_example` supply
no ops and so still print `Skipping MCS calibration`, unchanged.

**The empirical question below was answered 2026-08-13: subclass 1 locks SYSREF
phase *without* MCS cal.** `SYSREF Phase: Locked`, `SYSREF captured: Yes` on both
AXI cores, and `SYSREF_PHASE` stable at 433 across five consecutive reads — with
`Skipping MCS calibration` still in the log. So the cal layer was never on the
critical path; it was ported to trim that 433 toward 0, i.e. to make latency
*accurate* rather than merely *deterministic*, and for a second device.

**The full change list, the traps found during the port, and the ordered
first-run checklist live in `SUBCLASS1_ENABLEMENT.md` Stage D.** Two things worth
carrying here because they bite outside Step 8:

- **`no_os_div_u64()` truncates silently above a `uint32_t` divisor**, so the
  validate step's `1e12` divide must use `no_os_div64_u64_rem()` — which in turn
  dereferences its remainder pointer unconditionally, so NULL faults.
- **The shared ADF4382 phase API moved from picoseconds to femtoseconds**, gained
  `adf4382_get_phase_adjust()` and `adf4382_set_auto_align()`, and
  `adf4382_pfd_compute()` took the kernel's `int (dev, uint64_t *)` signature with
  a PFD range check across all 9 call sites. `projects/adf4382` is affected and
  was updated; note that project does not build at HEAD for an unrelated reason
  (`adf4382.h` includes `no_os_clk.h`, which its `src.mk` never provides).

**What holds.** Subclass 0 genuinely does not use MCS cal: it makes no
deterministic-latency guarantee, so there is no SYSREF timing contract for the
delta-t correction to trim. `LINK_SETUP` only arms the SYSREF receiver when
subclass == 1.

**What was wrong.**

- The claim that porting MCS cal "requires porting the adf4030/adf4382
  clock-chip drivers" — **both drivers already exist in no-OS**, with exactly the
  calls the kernel reaches through IIO: `adf4030_get_bsync_freq`,
  `adf4030_set_channel_delay`/`_get_channel_delay` (femtoseconds),
  `adf4030_set_channel_direction`, `adf4030_set_background_serial_alignment`,
  `adf4030_set_single_ch_alignment`, `adf4030_set_tdc_measurement`, and
  `adf4382_set_phase_adjust`. What was missing was JESD204 registration in the
  ADF4030, added 2026-08-13.
- The claim that we are "a board with no adf chips wired, so the kernel itself
  returns early" — the board **has** an ADF4030 (`vcu118_ad9084.dts:617`,
  `adf4030@4`), and on the kernel platform `post_setup_stage1` therefore *runs*.
  Only our config lacked the chip. `phy->iio_adf4030`/`iio_adf4382` are NULL in
  no-OS because nothing ever assigns them, not because the hardware is absent.

**Two further gaps found in the port.**

- **`JESD204_OP_CLK_SYNC_STAGE4` was missing from the no-OS framework**
  (`include/jesd204.h` stopped at `CLK_SYNC_STAGE3`). The kernel's ADF4030
  registers its BSYNC alignment on that op, so without it the alignment step had
  nowhere to run. Added 2026-08-13.
- **The `center_sysref.sysref_present` fixup was dropped from `ad9088_dt.c`.**
  Kernel `ad9088_dt.c:237-248` force-sets it when subclass is non-zero; no-OS
  ported only the four subclass assignments. Since we keep the existing
  subclass-0-era profile binary, this is mandatory for subclass 1 — without it
  the on-chip firmware is never told to expect an external SYSREF. **Ported in
  Stage C, 2026-08-13**, together with a profile version check and a
  `profile_checksum` recompute (new `util/no_os_crc32.c`) that retired the
  `WARNING: profile crc is invalid.` line.
- **`sysref.lmfc_offset` was never seeded to
  `JESD204_LMFC_OFFSET_UNINITIALIZED`.** Offset 0 is a legal value rather than
  "unset", so both AXI cores unconditionally programmed 0. Fixed in the framework
  (`jesd204_dev_alloc_links()`), matching the kernel's
  `__jesd204_dev_init_link_data()`. **Ported in Stage C, 2026-08-13.**

**Answered: the ADF4030's own alignment is sufficient on a single device.** Its
auto-align (`align_on_sync_en` on channels 0/5/8, reference channel 0, 8
iterations — the dts asks for 6 but the kernel misspells the property name, so the
reference runs 8) does set the JRX `0x40` SYSREF-received bit, and `SYSREF_PHASE`
is repeatable. MCS cal trims the *residual* skew between the external BSYNC edge
and the Apollo's internal SYSREF counter, and tracks ADF4382 drift — required for
multi-chip, functionally optional for this single-device link. It was ported
anyway and **HW-verified on 2026-08-14**, cutting the residual from 21.65 ns to
2.00 ns; see `SUBCLASS1_ENABLEMENT.md` Stage D for the change list and results.

---

## Step 9: Runtime Control

**Goal**: Port loopback, test tones, and CFIR filter control.

### Loopback modes

Kernel exposes via IIO device attributes:
```c
// loopback_mode_side_a, loopback_mode_side_b (0-3)
adi_apollo_loopback_mode_set(device, side, mode);
```

### Test tones

Kernel exposes via IIO channel extended info:
```c
adi_apollo_cddc_test_tone_enable(device, ...);
adi_apollo_fddc_test_tone_enable(device, ...);
```

### CFIR (Configurable FIR)

```c
adi_apollo_cfir_profile_set(device, ...);
adi_apollo_cfir_enable(device, ...);
```

### All of these are straightforward HAL calls wrapped in IIO attribute
handlers. Port them as part of Step 10 (IIO integration).

### Progress (2026-08-13) — test tone + capture verified, JTAG readback working

The RX FNCO test tone and the DMA capture path are HW-verified on VCU118. Latest
run of `dma_example` with the cabled loopback section commented out:

```
NCO tx c/f 2500000/0 kHz  rx c/f 2500000/0 kHz  net 0 kHz
--- Noise floor, datapath idle ---   coh 0.0%    PASS
--- RX FNCO test tone ---            coh 99.9%   PASS
    spread 1 (max 250)  amp 11519 LSB
    99.4% of the 11583 LSB transmitted, 35.1% of full scale
RX buffer address: 0x80190c00 samples=65536 channels=4 bits=16
```

The cabled DAC→ADC loopback (previously passing at 99.8% coherence) is currently
commented out in `dma_example.c`, not removed. Note that disabling it leaves
`loopback_pass` uninitialized, so the example's return value is meaningless while
it stays that way.

Still to port for this step: loopback mode control, CFIR.

#### Pulling the capture buffer off the board over JTAG

`tools/scripts/platform/xilinx/capture.tcl` reads the buffer out of DDR and
de-interleaves it into one CSV per channel; `plot.py` renders them.

**The firmware must be parked before capturing.** The example ends with a
`while (1);` immediately after the buffer-address `pr_info`. Without it every CSV
comes out all zeros: `adc_buffer_dma` is `static`, so it lives in `.bss`, and when
the example finishes and the program restarts (or the ELF is re-downloaded) crt0
re-zeroes `.bss` — the capture is gone before `mrd` reads it. Never reflash between
the test run and the capture. Parking also keeps the JESD link and DMA up, which
the read needs.

The script takes the board's `hw_server` from `XSCT_REMOTE_HOST`/`XSCT_REMOTE_PORT`
(same convention as `util.tcl`), since the board is remote — see
`UART_FORWARDING.md` for the tunnel:

```bash
cd projects/ad9088/captures
export XSCT_REMOTE_HOST=localhost XSCT_REMOTE_PORT=3122
xsct ../../../tools/scripts/platform/xilinx/capture.tcl \
     MICROBLAZE 0x80190c00 65536 4 16
python3 ../../../tools/scripts/platform/xilinx/plot.py 4
```

Argument and output notes, none of them obvious from the script:

- `num_of_samples` is the **total across channels**, not per channel: `16384 4`
  yields 4096 samples/channel. `dma_example.c`'s `pr_info` prints the total so all
  four values copy straight to the command line.
- The address comes from the `pr_info`, or from
  `mb-nm -S build/ad9088.elf | grep adc_buffer_dma` — that is the authority if the
  two ever disagree.
- CSV values are unsigned `0..65535`; `plot.py` maps them back to signed itself.
- The script reads 4x more words than the 16-bit de-interleave consumes
  (`num_of_word = 2 * num_of_samples` vs. `num_of_samples / 2`). Harmless, but it
  is the first thing to fix if a full-depth capture feels slow.
- Sanity-check before plotting: `sort -u capture_ch1.csv | wc -l` should be
  thousands, not 1. A flat plot and an all-zero capture look identical.

Causes ruled out while diagnosing the all-zero captures, recorded so they are not
re-investigated: it is **not** a debug-module limitation (`sys_mb_debug` does have
`C_DBG_MEM_ACCESS = 0`, but `mrd -force` against a running MicroBlaze returns valid
data anyway — a `stop`/`con` pair around the read was tried and reverted, and would
be actively harmful to the Zynq/Versal projects sharing the script); **not** cache
(`dma_example_capture()` already invalidates the range); and **not** the address.

---

## Step 10: IIO Integration

**Goal**: Expose device control through the no-OS IIO framework.

### Overview

The kernel driver registers as an IIO device with:
- **Channels**: One per FDDC/FDUC (RX/TX channelizer), complex I/Q
- **Channel attributes**: NCO freq/phase, gain, test tone enable
- **Device attributes**: JESD204 FSM status, MCS control, loopback mode

### no-OS IIO pattern

Look at existing no-OS IIO drivers for reference (e.g.,
`drivers/rf-transceiver/navassa/adrv9002_iio.c` or
`drivers/adc/ad9081/ad9081_iio.c`).

The pattern is:
```c
// 1. Define channel specs
static struct iio_channel ad9088_channels[] = { ... };

// 2. Define attributes
static struct iio_attribute ad9088_dev_attrs[] = { ... };
static struct iio_attribute ad9088_ch_attrs[] = { ... };

// 3. Implement read/write callbacks
static int ad9088_iio_read_attr(void *dev, char *buf, ...);
static int ad9088_iio_write_attr(void *dev, char *buf, ...);

// 4. Define iio_device
static struct iio_device ad9088_iio_dev = {
    .channels = ad9088_channels,
    .attrs = ad9088_dev_attrs,
    .read_attr = ad9088_iio_read_attr,
    .write_attr = ad9088_iio_write_attr,
};
```

### Minimum viable IIO

For initial bring-up, expose:
1. Device attrs: JESD204 FSM state (read-only)
2. Channel attrs: CNCO/FNCO frequency (read/write), converter rate (read-only)
3. Loopback mode (device attr, read/write)

Advanced attributes (test tones, CFIR, FFH, MCS) can be added later.

---

## Step 11: ad9088_remove and Cleanup

### COMPLETE

`ad9088_remove()` is implemented in `ad9088.c`. It closes the HAL, frees all
GPIOs, SPI, and the phy struct in reverse allocation order. Per no-OS convention,
`_remove()` does NOT set the freed pointer to NULL — the caller is responsible.

`ad9088_init()` also has proper `goto error` cleanup labels for failure paths.

---

## Step 12: Project Integration

**Goal**: Wire everything into the project build and example code.

### Partially complete

**Done**:
- `basic_example.c` — fixed error cleanup labels (numeric -> descriptive),
  added `return 0` before error labels so success path doesn't fall through,
  fixed "JESD RX" typo in TX init error message
- `src.mk` — firmware directory (`SRC_DIRS += .../apollo/firmware`) already
  uses `rwildcard` to pick up all `.bin` files from subdirectories
- Build verified on VCU118 with Vitis 2025.1

### Remaining

1. **`projects/ad9088/src/examples/basic/basic_example.c`** — Add JESD204
   topology construction and `jesd204_fsm_start()` after Step 5

2. **`projects/ad9088/src/common/common_data.c`** — Add any new init_param
   fields as they are introduced in future steps

### Build and test

```bash
source /xilinx/2025.1/Vitis/settings64.sh
cd projects/ad9088
make
```

---

## Reference: How the topology and FSM work (no-OS)

This section describes the actual no-OS mechanism (grounded in
`jesd204/jesd204-core.c`, `jesd204/jesd204-fsm.c`, and `include/jesd204.h`), so
you know exactly where each callback runs and why a missing one causes
`WAIT_BS`.

### Part 1: Per-device registration

Before any topology exists, each participating driver calls
`jesd204_dev_register(&jdev, &dev_data)` *inside its own init*. This allocates a
`struct jesd204_dev` and attaches:
- a **`dev_data`** table — `state_ops[]`, the FSM callbacks
- **private storage** (`sizeof_priv` bytes), reached inside callbacks via
  `jesd204_dev_priv(jdev)`

By the time `basic_example` builds the topology, five `jdev`s exist (the ADF4030
was added 2026-08-13):

| Device | Source of its `dev_data` | Callbacks implemented |
|---|---|---|
| `adf4030_dev->jdev` | adf4030 driver | **sysref provider** (`sysref_cb`, LINK_SUPPORTED, CLK_SYNC_STAGE4) |
| `hmc7044_dev->jdev` | hmc7044 driver | LINK_SUPPORTED, CLK_SYNC_STAGE1-3, LINK_PRE_SETUP |
| `rx_jesd->jdev` | `axi_jesd204_rx` | pre_setup, link_setup, clks_enable, link_enable, link_running |
| `tx_jesd->jdev` | `axi_jesd204_tx` | TX equivalents |
| `ad9088_phy->jdev` | `jesd204_ad9088_init` (`ad9088_jesd204_fsm.c`) | **only LINK_INIT + DEVICE_INIT so far** |

The AD9088's `dev_data` declares `sizeof_priv = sizeof(struct
ad9088_jesd204_priv)` — that's the `priv->phy` back-pointer every callback uses.

### Part 2: Topology setup (`basic_example.c`)

`jesd204_topology_init()` takes an array of `jesd204_topology_dev`, each binding
a `jdev` to the link IDs it participates in plus two role flags:

```c
struct jesd204_topology_dev devs[] = {
  { .jdev = adf4030_dev->jdev,
    .link_ids = {FRAMER_LINK_A0_RX, DEFRAMER_LINK_A0_TX},
    .links_number = 2, .is_sysref_provider = true },   // role flag
  { .jdev = hmc7044_dev->jdev,
    .link_ids = {FRAMER_LINK_A0_RX, DEFRAMER_LINK_A0_TX},
    .links_number = 2 },
  { .jdev = rx_jesd->jdev,
    .link_ids = {FRAMER_LINK_A0_RX}, .links_number = 1 },
  { .jdev = tx_jesd->jdev,
    .link_ids = {DEFRAMER_LINK_A0_TX}, .links_number = 1 },
  { .jdev = ad9088_phy->jdev,
    .link_ids = {FRAMER_LINK_A0_RX, DEFRAMER_LINK_A0_TX},
    .links_number = 2, .is_top_device = true },         // role flag
};
```

What `topology_init` does with this (`jesd204-core.c:100-119`):
- The **top device** (`is_top_device`, the AD9088) is pulled into
  `top->dev_top`. Its `link_ids` become the **master link list** the FSM
  iterates. `jesd204_dev_alloc_links()` allocates `active_links[]` — the
  `struct jesd204_link` objects that `LINK_INIT` fills.
- Every **other** device goes into `top->devs[]` (note `devs_number - 1`).
- The **sysref provider** (ADF4030) is recorded as `dev_top->jdev_sysref` so
  the FSM can fire SYSREF pulses through it. Note the provider entry must come
  *before* the top device in this array: `topology_init` reads
  `is_sysref_provider` from the caller's array but takes the `jdev` pointer from
  its own top-device-filtered copy, so the two indices only agree while the
  provider precedes the top device.

Two links are active: **`FRAMER_LINK_A0_RX`** (AD9088 ADC → FPGA, receive/framer
path) and **`DEFRAMER_LINK_A0_TX`** (FPGA → AD9088 DAC, transmit/deframer path).

### Part 3: How `jesd204_fsm_start` executes (`jesd204-fsm.c:21-54`)

A triple-nested loop. The **outer loop is over operations** — it walks the whole
bring-up sequence one stage at a time, visiting every device per stage:

```
for each OP (DEVICE_INIT -> LINK_INIT -> LINK_SETUP -> ... -> POST_*):
    for each link_id in the top device's link list:
        for each participant device that lists this link_id:
            if per_device callback (and not yet run this op) -> call once
            if per_link  callback                            -> call for this link
        # top device (AD9088) runs LAST for this link:
        if top device per_link for this op -> call it
            if op marked post_state_sysref -> fire SYSREF via provider
    # finally the top device per_device callback for this op:
    if top device per_device for this op -> call it
        if post_state_sysref -> fire SYSREF
```

Key properties:
1. **Stage-synchronized across devices** — every device finishes op *N* before
   anyone starts op *N+1*. That is what makes bring-up coordinated (FPGA
   `rx_jesd` and AD9088 framer both do `LINK_SETUP` before either does
   `CLOCKS_ENABLE`).
2. **`per_device` runs once per op** (guarded by `per_device_op_done[dev]`).
3. **The top device (AD9088) always runs last** within each op.
4. **SYSREF is fired by the framework**, not the driver, whenever an op sets
   `post_state_sysref = true` (kernel: `LINK_ENABLE`). Routed through
   `dev_top->jdev_sysref` → HMC7044.
5. `jesd204_fsm_stop` is the mirror image: same loops in reverse op order with
   `reason = UNINIT`.

The `reason` argument threaded through every callback is
`JESD204_STATE_OP_REASON_INIT` on start / `UNINIT` on stop — that is why every
kernel callback starts with `if (reason != ...INIT) return DONE;`.

### Part 4: Why the current build stalls at WAIT_BS

The AD9088 `dev_data` only populates `state_ops[DEVICE_INIT]` and
`state_ops[LINK_INIT]`. When the outer loop reaches `LINK_SETUP`,
`CLOCKS_ENABLE`, `LINK_ENABLE`, the AD9088 slots are **NULL** and the framework
skips them. The FPGA `rx_jesd`/`tx_jesd` *do* run their versions, but with no
matching device-side link-enable, SERDES cal, or `post_state_sysref` pulse, the
AD9088 framer never streams synced data → link layer stuck at `WAIT_BS`, PHY
elastic buffer underflows (`0x21`). The loop structure is correct and the FPGA
side participates; the AD9088 half of the handshake is simply missing from the
op table. Filling it in is sub-step 2 onward.

### Layer map (which struct is which layer)

| Struct | Layer | Role |
|---|---|---|
| `adf4382` | clocking | 20 GHz Apollo device clock |
| `hmc7044` | clocking | FPGA core clks + GT refclk, ADF4030 refin (ch1) and BSYNC0 (ch3) |
| `adf4030` | clocking | BSYNC/SYSREF to the Apollo (ch5) and the FPGA (ch8) |
| `rx_adxcvr` / `tx_adxcvr` (`adxcvr`) | PHY | GTY SERDES + QPLL, elastic buffer |
| `rx_jesd` / `tx_jesd` (`axi_jesd204_rx/tx`) | link | 64b66b block sync, alignment |
| `ad9088_phy` | transport + link (device side) | framer/deframer, datapath |
| `rx_dmac` / `tx_dmac` | application | moves samples to/from memory |

`buffer under/overflow` errors are **PHY-layer** (`adxcvr`); `WAIT_BS` /
`CGS` / `DATA` states are **link-layer** (`axi_jesd204_rx`). A JESD link only
runs when both ends (FPGA `rx_jesd` **and** the AD9088 framer) are enabled and
SYSREF-aligned together — the reason the coordinated FSM exists.

---

## Reference: JESD204 FSM State Sequence

For reference, this is the order the FSM callbacks fire:

```
1. LINK_INIT       (per-link)  -> Fill jesd204_link struct from profile
2. LINK_SETUP      (per-device)-> Enable links, reset datapaths, MCS setup
3. LINK_SUPPORTED  (per-link)  -> Verify link config is valid
4. SETUP_STAGE1    (per-device)-> Nyquist, clock cal, ADC cal start, JESD inspect
5. SETUP_STAGE2    (per-device)-> ADC cal complete, re-sync, bmem delay
6. CLKS_ENABLE     (per-link)  -> SERDES cal, JTX enable
7. LINK_ENABLE     (per-link)  -> JRX enable, SYSREF trigger
8. POST_RUNNING_1  (per-device)-> Link status check
9. POST_RUNNING_2  (per-device)-> IRQ setup
10. POST_RUNNING_3 (per-device)-> Set is_initialized, print chip info
```

---

## Tips

- **Build early, build often**: After each step, try to compile. Fix errors
  before moving on.
- **The HAL is your friend**: If you're unsure what a function does, look at
  the Apollo HAL source in `public/src/`. The function names are descriptive.
- **Compare side by side**: Keep the kernel `ad9088_jesd204_fsm.c` open next to
  your no-OS code. The logic is the same — only the Linux framework glue
  changes.
- **Start with compile, then runtime**: Get it compiling first, even with stubs.
  Then fill in the stubs one at a time, testing on hardware after each.
- **Single-device first**: Skip multi-device sync, FFH, and advanced features
  until the basic single-device flow works end to end.
