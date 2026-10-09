# ADP20086 Driver — Code Audit

**Scope:** `drivers/power/adp20086/{adp20086.c,adp20086.h,iio_adp20086.c,iio_adp20086.h,README.rst}`, `projects/adp20086/**`, build wiring, Sphinx wiring
**Branch:** `feature/FCE-791-add-adp20086-driver`
**Method:** 13 independent audit lenses + 8 targeted follow-ups, each finding adversarially verified by 1–3 independent skeptics (188 agents, 107 unique findings, 17 refuted). Every macro claim below was proven by compiling against the repo's real `include/no_os_util.h`; every register/LSB claim was checked against `exports/_adp20086_datasheet_text.txt`.

---

## Bottom line

**Not ready to merge.** Two defects will mis-program overcurrent protection on an ASIL-B part: the IIO `current_limit` attribute is off by one LSB in the write direction (**C-1**), and the ILIM getters accept `ALL_CHANNELS` and read the interrupt-mask register as if it were a current limit (**C-2**). A third, **C-3**, makes `adp20086_reset_interrupt_masks()` silently clear `OVTST`/`IIR_EN`/`DISCH_EN` while the README states in plain text that it does not. Beyond correctness, the build is not CI-clean (`-Wunused-label`, astyle failures in both examples), `compile_commands.json` is committed, and the driver links `no_os_irq` without selecting `CONFIG_IRQ`. The device layer itself is in good shape — the table-driven refactor landed, all 76 public functions are documented, and the four driver files are astyle-clean; the damage is concentrated in the IIO layer, the examples, and the README.

Prior-audit regression check: `ILIM_MASK` (old C-1), the IIO double-shift (H-1), `get_ilim_ua` (H-2), and doxygen coverage (A-6) are **genuinely fixed**. Old M-1 and M-4 are **still present**; old L-1, L-3 and L-5 are **still present**, and the L-1 fix **introduced two new stale API names**.

---

## Severity summary

| ID | Severity | Finding | Location |
|----|----------|---------|----------|
| C-1 | **Critical** | IIO `current_limit` write is off by one LSB — 52 mA programs 104 mA | [iio_adp20086.c:700](drivers/power/adp20086/iio_adp20086.c#L700) |
| C-2 | **Critical** | ILIM getters accept `ALL_CHANNELS` (−1) and read MASK3 (0x0B) as a current limit | [adp20086.c:1402](drivers/power/adp20086/adp20086.c#L1402) |
| C-3 | **High** | `reset_interrupt_masks()` clobbers `OVTST`/`IIR_EN`/`DISCH_EN`; README claims the opposite | [adp20086.c:972](drivers/power/adp20086/adp20086.c#L972) |
| H-1 | High | Kconfig selects GPIO but not IRQ, while the driver calls `no_os_irq_*` unconditionally | [Kconfig:50](drivers/power/Kconfig#L50) |
| H-2 | High | IIO `int_mask`/`int_mask3` writes clobber the same three feature bits | [iio_adp20086.c:953](drivers/power/adp20086/iio_adp20086.c#L953) |
| H-3 | High | Every IIO store discards `iio_parse_value()`'s result — bad input is silently 0 | [iio_adp20086.c:692](drivers/power/adp20086/iio_adp20086.c#L692) |
| H-4 | High | IIO threshold stores overflow `uint32_t`, wrapping absurd input into a valid threshold | [iio_adp20086.c:709](drivers/power/adp20086/iio_adp20086.c#L709) |
| H-5 | High | `debug_reg_read/write` truncate the address to 8 bits with no bounds check | [iio_adp20086.c:1071](drivers/power/adp20086/iio_adp20086.c#L1071) |
| H-6 | High | Unused `error_dev:` label — real `-Wunused-label` warning on every build | [adp20086.c:276](drivers/power/adp20086/adp20086.c#L276) |
| H-7 | High | astyle violations in both examples will fail the mandatory CI gate | [basic_example.c:60](projects/adp20086/src/examples/basic_example/basic_example.c#L60) |
| H-8 | High | `compile_commands.json` (machine-local build artifact) committed to the repo root | `compile_commands.json` |
| M-1 | Medium | `remove()` early-returns on first error, leaking the I2C descriptor and the allocation | [adp20086.c:305](drivers/power/adp20086/adp20086.c#L305) |
| M-2 | Medium | Threshold setters reject code 0, so the reset value cannot be written back | [adp20086.c:1989](drivers/power/adp20086/adp20086.c#L1989) |
| M-3 | Medium | `clear_device_status()` flushes only STAT1/STAT5, never STAT2/3/4 | [adp20086.c:1299](drivers/power/adp20086/adp20086.c#L1299) |
| M-4 | Medium | ID register is read but never validated — a dead device reading 0x00 passes init | [adp20086.c:249](drivers/power/adp20086/adp20086.c#L249) |
| M-5 | Medium | `init()` unconditionally drives EN low, dropping all four rails on a running system | [adp20086.c:204](drivers/power/adp20086/adp20086.c#L204) |
| M-6 | Medium | IIO `iopen`/`load_detect` use integer mA against a 3.5 mA LSB — lossy round-trip | [iio_adp20086.c:597](drivers/power/adp20086/iio_adp20086.c#L597) |
| M-7 | Medium | Public out-pointers dereferenced without a NULL guard | [adp20086.c:538](drivers/power/adp20086/adp20086.c#L538) |
| M-8 | Medium | `basic_example` leaks the GPIO IRQ controller when `adp20086_init()` fails | [basic_example.c:100](projects/adp20086/src/examples/basic_example/basic_example.c#L100) |
| M-9 | Medium | IIO example's `pr_info` output goes nowhere — stdio UART is never bound | [iio_example.c:39](projects/adp20086/src/examples/iio_example/iio_example.c#L39) |
| M-10 | Medium | `example_main()` called with no visible prototype | [main.c:38](projects/adp20086/src/platform/maxim/main.c#L38) |
| L-1 | Low | Seven dead `enum < first_enumerator` checks — always false on unsigned enums | [adp20086.c:636](drivers/power/adp20086/adp20086.c#L636) |
| L-2 | Low | `IIO_MAX_GLOBAL_ATTRS` has exactly zero headroom, enforced only by a comment | [iio_adp20086.h:21](drivers/power/adp20086/iio_adp20086.h#L21) |
| L-3 | Low | `set_ovtst()` always fails at reset because CONFIG resets to 0x1F | [adp20086.c:997](drivers/power/adp20086/adp20086.c#L997) |
| API-1 | High | `enum adp20086_channel` is an enum, array index, register addend **and** shift count | [adp20086.h:161](drivers/power/adp20086/adp20086.h#L161) |
| API-2 | High | `enum adp20086_parameters` fuses two namespaces; declaration *order* is load-bearing | [adp20086.h:198](drivers/power/adp20086/adp20086.h#L198) |
| API-3 | High | `set_ilim()` takes a raw code while every sibling setter takes micro-units | [adp20086.h:537](drivers/power/adp20086/adp20086.h#L537) |
| API-4 | Medium | `struct adp20086_channel_status` is six parallel nibble-masks | [adp20086.h:264](drivers/power/adp20086/adp20086.h#L264) |
| D-1 | High | README calls two functions that do not exist | [README.rst:178](drivers/power/adp20086/README.rst#L178) |
| D-2 | High | README invents two global IIO attribute names; real ones are `uvin`/`ovin` on `vin` | [README.rst:350](drivers/power/adp20086/README.rst#L350) |
| D-3 | Medium | Project README documents INTB on P1_9; the code uses P1_8 | [README.rst:34](projects/adp20086/README.rst#L34) |
| D-4 | Medium | Project README missing mandatory sections and absent from the Sphinx toctree | [README.rst:1](projects/adp20086/README.rst#L1) |
| D-5 | Medium | `intbsts` documented as "BIST in progress"; datasheet says "INT Stuck High BIST" | [adp20086.h:297](drivers/power/adp20086/adp20086.h#L297) |
| D-6 | Medium | README documents `uvout` on the `iout` channels; it lives on the `vout` channels | [README.rst:335](drivers/power/adp20086/README.rst#L335) |
| X-1 | Medium | Dead `#ifdef ADP20086_EVKIT_LOAD` — never defined anywhere | [basic_example.c:162](projects/adp20086/src/examples/basic_example/basic_example.c#L162) |
| X-2 | Medium | ~500 lines of triplicated `set`/`get_code`/`get_<unit>` wrappers | [adp20086.c:1436](drivers/power/adp20086/adp20086.c#L1436) |
| X-3 | Low | Eight dead register macros; the code open-codes the same masks instead | [adp20086.h:119](drivers/power/adp20086/adp20086.h#L119) |
| X-4 | Low | Dead `if (ret)` after a `printf` that cannot set `ret` | [basic_example.c:205](projects/adp20086/src/examples/basic_example/basic_example.c#L205) |
| X-5 | Low | Two unused locals in `basic_example`; unused `<stdlib.h>` in the IIO layer | [basic_example.c:76](projects/adp20086/src/examples/basic_example/basic_example.c#L76) |
| X-6 | Low | Malformed doxygen block; two `//` comments restating adjacent code | [adp20086.c:961](drivers/power/adp20086/adp20086.c#L961) |

---

## Critical

### C-1. IIO `current_limit` write is off by one LSB

**Location:** [iio_adp20086.c:700](drivers/power/adp20086/iio_adp20086.c#L700)
**Impact:** Every current-limit setting except 832 mA programs one step too high. On an ASIL-B camera-power protector this is a safety-relevant misconfiguration, and it is silent.

```c
i = no_os_clamp(((uint32_t)val * 1000) / ADP20086_ILIM_LSB_UA, 0,
                ADP20086_ILIM_832MA);
return adp20086_set_ilim(adp20086, ch, (enum adp20086_ilim_available)i);
```

The device layer defines code *n* as `(n+1) × 52 mA` — `adp20086_get_ilim_ua()` correctly computes `(code + 1) * ADP20086_ILIM_LSB_UA`. The store direction divides without the matching `−1`. Proven by direct computation:

| write | code | reads back |
|------:|-----:|-----------:|
| 52 mA | 1 | **104 mA** |
| 104 mA | 2 | **156 mA** |
| 416 mA | 8 | **468 mA** |
| 832 mA | 15 | 832 mA |

A user who reads `current_limit` and writes the same value back walks the limit up one step per round trip. This is the un-mirrored half of the old H-2 fix.

**Fix** — invert the getter's transfer function exactly, and reject sub-LSB input rather than clamping it to code 0:

```c
uint32_t ua = (uint32_t)val * 1000;

if (ua < ADP20086_ILIM_LSB_UA || ua > ADP20086_MAX_ILIM_UA)
        return -EINVAL;

i = (ua / ADP20086_ILIM_LSB_UA) - 1;
```

Note this is also the natural place to retire `ADP20086_MAX_ILIM_UA` from the dead-macro list (X-3) by actually using it.

### C-2. ILIM getters accept `ALL_CHANNELS` and read the interrupt-mask register

**Location:** [adp20086.c:1402](drivers/power/adp20086/adp20086.c#L1402)
**Impact:** `adp20086_get_ilim_code(dev, ADP20086_ALL_CHANNELS, &c)` returns success with a value read from MASK3. Callers cannot distinguish it from a real current limit.

```c
if (!dev || ch < ADP20086_ALL_CHANNELS || ch > ADP20086_CHANNEL4 || !code)
        return -EINVAL;

return adp20086_get_register_field(dev, ADP20086_REG_ILIM(ch),
                                   ADP20086_ILIM_MASK(ch), code);
```

The guard uses `ch < ADP20086_ALL_CHANNELS`, which *admits* −1. Compiled against the real `no_os_util.h`, `ADP20086_REG_ILIM(-1)` expands to `0x0C + (-1 >> 1)` = **0x0B** = `ADP20086_REG_MASK3`, and `ADP20086_ILIM_MASK(-1)` yields `0x0F`. The header's own comment says getters "require a single channel" — the code does not enforce it. `adp20086_get_ilim_ua()` then feeds that garbage through `(code + 1) * 52000`.

**Fix** — getters must reject the broadcast sentinel:

```c
if (!dev || !code || ch < ADP20086_CHANNEL1 || ch > ADP20086_CHANNEL4)
        return -EINVAL;
```

Apply the same correction to every getter that takes a channel; see API-1 for the structural fix that prevents the whole class.

### C-3. `reset_interrupt_masks()` destroys three feature bits — and the README promises it does not

**Location:** [adp20086.c:972](drivers/power/adp20086/adp20086.c#L972)
**Impact:** Silently disables the OV comparator self-test arm, the ADC IIR filter and the output discharge resistors. The README states the opposite in plain text, so a reader has no reason to suspect it.

```c
ret = adp20086_write(dev, ADP20086_REG_MASK,  ADP20086_REG_MASK_DEFAULT);
ret = adp20086_write(dev, ADP20086_REG_MASK2, ADP20086_REG_MASK2_DEFAULT);
ret = adp20086_write(dev, ADP20086_REG_MASK3, ADP20086_REG_MASK3_DEFAULT);
```

`MASK` bit 7 is `OVTST`; `MASK3` bits 5 and 4 are `IIR_EN` and `DISCH_EN`. These are whole-byte writes. [README.rst:141](drivers/power/adp20086/README.rst#L141) says: *"It writes the mask bits alone, so the feature bits sharing those registers are preserved."*

**Fix** — read-modify-write the interrupt bits only:

```c
ret = adp20086_update_register(dev, ADP20086_REG_MASK,
                               NO_OS_GENMASK(6, 0),
                               ADP20086_REG_MASK_DEFAULT);
/* MASK2 is all interrupt bits — a plain write is fine. */
ret = adp20086_update_register(dev, ADP20086_REG_MASK3,
                               ADP20086_MASK3_FCALM | NO_OS_GENMASK(3, 0),
                               ADP20086_REG_MASK3_DEFAULT);
```

---

## High

### H-1. Kconfig selects GPIO but not IRQ

**Location:** [Kconfig:50](drivers/power/Kconfig#L50)
**Impact:** Link failure (undefined `no_os_irq_*`) for any configuration that does not enable `CONFIG_IRQ` through some other driver.

```kconfig
config POWER_ADP20086
	depends on I2C
	select GPIO
```

`drivers/api/CMakeLists.txt:6` gates `no_os_irq.c` on `CONFIG_IRQ`, and `adp20086.c` makes nine unconditional `no_os_irq_*` calls. It links today only because the example also pulls in IRQ.

**Fix:** add `select IRQ` beside `select GPIO`.

### H-2. IIO interrupt-mask writes clobber the same three feature bits as C-3

**Location:** [iio_adp20086.c:953](drivers/power/adp20086/iio_adp20086.c#L953)

```c
return adp20086_write(adp20086, reg, (uint8_t)val);
```

Writing `int_mask` or `int_mask3` from userspace destroys `OVTST`, `IIR_EN` and `DISCH_EN`. This is the old M-1, unfixed. Fix with `adp20086_update_register()` and the same masks as C-3 — or better, route the attribute through `adp20086_set_interrupt_mask()` instead of touching registers from the IIO layer.

### H-3. Every IIO store discards the parse result

**Location:** [iio_adp20086.c:692](drivers/power/adp20086/iio_adp20086.c#L692)
**Impact:** Unparseable input is silently treated as a valid 0 rather than rejected.

```c
iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
if (val < 0)
        return -EINVAL;
```

`iio_parse_value()` returns a negative errno on failure, which is never checked; `val` may be left untouched or zero. Writing `abc` to `current_limit` is accepted.

**Fix:**

```c
ret = iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
if (ret)
        return ret;
```

### H-4. IIO threshold stores overflow `uint32_t`

**Location:** [iio_adp20086.c:709](drivers/power/adp20086/iio_adp20086.c#L709)
**Impact:** A large input wraps into the accepted range and programs a *tiny* threshold — the opposite of what the user asked for, with no error.

```c
return adp20086_set_uvout(adp20086, ch, (uint32_t)val * 1000);
```

Writing `4300000` to `uvout` gives `4300000 × 1000 mod 2³²` = `5032704` µV ≈ 5.03 V, which passes the `≤ 18360000` clamp and is silently programmed.

**Fix** — bound before scaling:

```c
if (val < 0 || (uint32_t)val > ADP20086_MAX_VOUT_UV / 1000)
        return -EINVAL;
```

### H-5. `debug_reg_read`/`debug_reg_write` truncate instead of validating

**Location:** [iio_adp20086.c:1071](drivers/power/adp20086/iio_adp20086.c#L1071)

```c
return adp20086_write(iio_adp20086->adp20086_dev, (uint8_t)reg,
                      (uint8_t)writeval);
```

The register map ends at 0x2A. `reg = 0x12A` silently aliases onto 0x2A. Old M-4, unfixed.

**Fix:** `if (reg > ADP20086_REG_LOAD_INT_EN || writeval > 0xFF) return -EINVAL;`

### H-6. Unused `error_dev:` label breaks the clean-build requirement

**Location:** [adp20086.c:276](drivers/power/adp20086/adp20086.c#L276)

Verified by compiling:

```
drivers/power/adp20086/adp20086.c:276:1: warning: label 'error_dev' defined but not used [-Wunused-label]
```

Left behind by the init rewrite — the SRS called this out explicitly and it is still here. The `no_os_free(device)` below it is reached by fallthrough, so simply delete the label.

### H-7. astyle violations in both examples will fail CI

**Location:** [basic_example.c:60](projects/adp20086/src/examples/basic_example/basic_example.c#L60), [iio_example.c:44](projects/adp20086/src/examples/iio_example/iio_example.c#L44)

`.github/workflows/build-common.yaml` runs `.github/scripts/astyle.sh`, which reformats in place and `exit 1`s on any diff. Running the repo's own `.github/config/astyle_config` against the files:

- `basic_example.c` — continuation-line indent at :60, **trailing whitespace on blank line :180**
- `iio_example.c` — continuation-line indent at :44

The four `drivers/power/adp20086/*.{c,h}` files are **clean**; only the two examples fail.

### H-8. `compile_commands.json` committed to the repo root

A 290-line machine-local clangd artifact with absolute paths from this workstation, tracked in git as of `b1b19dc60` and not in `.gitignore`. Delete it and add it to `.gitignore`.

---

## Medium

### M-1. `remove()` leaks on the first sub-remove error

**Location:** [adp20086.c:305](drivers/power/adp20086/adp20086.c#L305)

```c
ret = no_os_gpio_remove(dev->intb_gpio_desc);
if (ret)
        return ret;
```

If any removal fails, the remaining descriptors and `dev` itself leak. Accumulate the error and always complete the teardown:

```c
int err = 0, ret;

ret = no_os_gpio_remove(dev->intb_gpio_desc);  if (ret) err = ret;
ret = no_os_gpio_remove(dev->en_gpio_desc);    if (ret && !err) err = ret;
ret = no_os_i2c_remove(dev->i2c_desc);         if (ret && !err) err = ret;
no_os_free(dev);
return err;
```

> The init-path sibling of this claim — that the unwind double-frees the I2C descriptor — was **refuted**: `no_os_calloc` zeroes the struct and `no_os_i2c_remove(NULL)` returns `-EINVAL` safely.

### M-2. Threshold setters reject code 0

**Location:** [adp20086.c:1989](drivers/power/adp20086/adp20086.c#L1989)

```c
if (!dev || value < map->lsb || value > map->max)
        return -EINVAL;
```

`LDET_SET`, `IOPEN_SET`, `UVOUT_SET`, `OVIN_SET` and `UVIN_SET` all reset to **0x00**, and code 0 is a valid setting (threshold disabled). The driver cannot write the device's own reset value. Change the lower bound to `value > map->max` only, or document 0 as reserved.

### M-3. `clear_device_status()` leaves the per-channel latches set

**Location:** [adp20086.c:1299](drivers/power/adp20086/adp20086.c#L1299)

Reads only STAT1 and STAT5. The genuinely latched per-channel faults — OC/OV/UV/TS in STAT2/STAT3 and OPEN/LOAD_DET in STAT4 — survive the "flush". The doxygen, the header and [README.rst:220](drivers/power/adp20086/README.rst#L220) all describe it as flushing "the latched device status registers" without the STAT1/STAT5 qualifier. Either read all six registers or rename to `adp20086_clear_device_faults()` and say so.

### M-4. ID register is read but never validated

**Location:** [adp20086.c:249](drivers/power/adp20086/adp20086.c#L249)

```c
ret = adp20086_read(device, ADP20086_REG_ID, &id);
device->pece    = no_os_field_get(ADP20086_ID_PECE, id);
device->part_id = no_os_field_get(ADP20086_ID_ID, id);
```

Nothing is compared. A dead or absent device whose bus reads back 0x00 passes `adp20086_init()` cleanly. `FR-INIT-3`/`OI-3` in `exports/ADP20086_no-OS_driver_SRS.md` require *"a caller-selectable expected-ID check"* precisely because the magic values are unpublished — add an optional `expected_id` to `struct adp20086_init_param` and check it when non-zero.

### M-5. `init()` unconditionally drives EN low

**Location:** [adp20086.c:204](drivers/power/adp20086/adp20086.c#L204)

```c
ret = no_os_gpio_direction_output(device->en_gpio_desc, NO_OS_GPIO_LOW);
```

Re-initializing the driver on a running system drops all four camera rails. There is no way to ask `init()` to adopt the current state. Add an `en_initial_state` (or leave the pin untouched and let the caller drive it).

### M-6. IIO `iopen`/`load_detect` use integer mA against a 3.5 mA LSB

**Location:** [iio_adp20086.c:597](drivers/power/adp20086/iio_adp20086.c#L597)

With `LSB = 3500 µA`, integer mA cannot represent odd codes: code 1 = 3.5 mA reads back as `3`, and writing `3` gives code 0. Round-tripping is lossy for half the range. Expose µA, or use `IIO_VAL_INT_PLUS_MICRO`.

### M-7. Public out-pointers dereferenced without a NULL guard

**Location:** [adp20086.c:538](drivers/power/adp20086/adp20086.c#L538)

`adp20086_read()`, `adp20086_get_register_field()` and the three per-channel enable getters check `!dev` but then write through `data`/`status` unchecked, while their siblings (`get_ilim_code`, `read_adc_microunits`, …) *do* check. Make the idiom uniform.

### M-8. `basic_example` leaks the IRQ controller on init failure

**Location:** [basic_example.c:100](projects/adp20086/src/examples/basic_example/basic_example.c#L100)

```c
ret = adp20086_init(&adp20086_dev, &adp20086_ip);
if (ret)
        goto remove_uart;        /* skips no_os_irq_ctrl_remove() */
```

`remove_uart:` does not release `adp20086_gpio_irq_desc`, which was acquired just above. Needs an intermediate `remove_irq:` label.

### M-9. IIO example's fault output goes nowhere

**Location:** [iio_example.c:39](projects/adp20086/src/examples/iio_example/iio_example.c#L39)

The callback calls `pr_info()`, but `iio_app` never calls `no_os_uart_stdio()` (confirmed by grep over `iio/iio_app/iio_app.c`), and `projects/adp20086/src/platform/maxim/main.c` does not either. Sibling projects that print from an IIO example (`ad5460`, `admt4000`, `swiot1l`, `adin1110`) all bind stdio in `main.c`. As written the fault messages are discarded — and if stdio *were* bound to the same UART, they would corrupt the IIOD protocol stream.

### M-10. `example_main()` called with no prototype

**Location:** [main.c:38](projects/adp20086/src/platform/maxim/main.c#L38)

Implicit declaration — removed in C99, an error under `-Werror=implicit-function-declaration`. Siblings declare `extern int example_main();` in `main.c` (e.g. `projects/lt3074/src/platform/maxim/main.c:36`). Add the declaration.

---

## Low

### L-1. Seven dead lower-bound range checks

**Location:** [adp20086.c:636](drivers/power/adp20086/adp20086.c#L636)

```c
|| ilim < ADP20086_ILIM_52MA || ilim > ADP20086_ILIM_832MA)
```

`enum adp20086_ilim_available` has no negative enumerator, so the compiler gives it an unsigned type and `ilim < 0` is always false (verified). Harmless but misleading — it reads as validation that is not happening. Note the contrast with `enum adp20086_channel`, which *is* signed because of the `−1` sentinel; that asymmetry is exactly what makes C-2 possible.

### L-2. `ADP20086_MAX_GLOBAL_ATTRS` has zero headroom

**Location:** [iio_adp20086.h:21](drivers/power/adp20086/iio_adp20086.h#L21)

The template has exactly 9 entries and the bound is 10 (9 + NULL terminator) — correct today, but adding a tenth attribute overflows `global_attrs[]` with no diagnostic. The only protection is a comment saying "keep in step". Derive it instead:

```c
#define ADP20086_MAX_GLOBAL_ATTRS (NO_OS_ARRAY_SIZE(adp20086_global_attrs) + 1)
```

or add a `static_assert`.

### L-3. `set_ovtst()` is unusable at reset

**Location:** [adp20086.c:997](drivers/power/adp20086/adp20086.c#L997)

The guard is datasheet-correct (*"The channel EN[x] bits must be low to run the diagnostics"*), but `CONFIG` resets to **0x1F** — all four EN bits set — so `adp20086_set_ovtst(dev, true)` returns `-EINVAL` on a fresh part until the caller explicitly disables all channels. Nothing documents this. Worth a doxygen note, and arguably the function should disable/restore the channels itself.

---

## API & data structures

### API-1. `enum adp20086_channel` carries four incompatible roles

**Location:** [adp20086.h:161](drivers/power/adp20086/adp20086.h#L161)

The same type is an enum, an **array index**, a **register-offset addend** and a **shift count** — while also carrying a `−1` sentinel. That combination is the root cause of C-2 and makes four header macros produce garbage for `ALL_CHANNELS`:

| macro | `ch = -1` expands to |
|---|---|
| `ADP20086_REG_ILIM(-1)` | `0x0B` (MASK3) |
| `ADP20086_REG_LDET_SET(-1)` | `0x0D` (ILIM34) |
| `ADP20086_REG_VOUT_READ(-1)` | `0x1E` (VIN_READ) |
| `NO_OS_BIT(-1)` | undefined behaviour |

**Fix** — separate the sentinel from the index. Keep `enum adp20086_channel` strictly `0..3`, and give broadcast its own entry point:

```c
enum adp20086_channel { ADP20086_CHANNEL1, ADP20086_CHANNEL2,
                        ADP20086_CHANNEL3, ADP20086_CHANNEL4 };
#define ADP20086_CHANNEL_COUNT 4

int adp20086_set_ilim(struct adp20086_dev *, enum adp20086_channel, uint32_t ua);
int adp20086_set_ilim_all(struct adp20086_dev *, uint32_t ua);
```

Every getter then takes a type that *cannot* hold `−1`, and C-2 becomes unrepresentable rather than merely unguarded.

### API-2. `enum adp20086_parameters` fuses two namespaces with load-bearing order

**Location:** [adp20086.h:198](drivers/power/adp20086/adp20086.h#L198)

The header admits it: *"The order of the two groups is relied upon for range checking."* `ADP20086_PARAM_LDET..UVOUT` index `adp20086_param_maps`; `ADP20086_PARAM_CHANNELS..OPEN_PROTECTION` index `adp20086_chan_feature_maps`. Inserting an enumerator in the wrong place silently routes threshold writes at the interrupt-mask register. Split into two enums — the compiler then enforces what a comment currently requests. This enum is also purely internal (no public prototype uses it), so it should move to the `.c` file entirely.

### API-3. `set_ilim()` is the only setter taking a raw code

**Location:** [adp20086.h:537](drivers/power/adp20086/adp20086.h#L537)

Every sibling setter takes micro-units (`set_uvout(…, uint32_t uvout_uv)`), and there is a `get_ilim_ua()` with **no** `set_ilim_ua()`. The IIO layer therefore had to invent the µA→code conversion itself — and got it wrong (C-1). Add `adp20086_set_ilim_ua()` and let the IIO layer call it; that fixes C-1 structurally rather than patching one call site.

### API-4. `struct adp20086_channel_status` is six parallel nibble-masks

**Location:** [adp20086.h:264](drivers/power/adp20086/adp20086.h#L264)

Six `uint8_t` bitmask fields force every consumer to loop over channels on the outside and re-index six fields, and require a free function (`adp20086_channel_asserted`) to exist purely to hide the layout. Both examples show the cost — [basic_example.c:55-62](projects/adp20086/src/examples/basic_example/basic_example.c#L55) calls it six times per channel. A per-channel array reads better at every call site:

```c
struct adp20086_channel_faults {
        bool thermal_shutdown, overcurrent, overvoltage;
        bool undervoltage, load_detected, open_load;
};
struct adp20086_channel_status {
        struct adp20086_channel_faults ch[ADP20086_CHANNEL_COUNT];
};
```

---

## Documentation

### D-1. README calls two functions that do not exist

**Location:** [README.rst:178](drivers/power/adp20086/README.rst#L178), [:184](drivers/power/adp20086/README.rst#L184)

| README says | actual symbol |
|---|---|
| `adp20086_channel_flagged()` | `adp20086_channel_asserted()` |
| `adp20086_int_enable()` | `adp20086_enable_intb()` |

Both appear in the prose **and** in the `.. code-block:: c` snippet, so the documented example does not compile. `adp20086_int_enable()` is also wrong in the doxygen at [adp20086.c:174](drivers/power/adp20086/adp20086.c#L174). These were introduced by the fix for the old L-1.

### D-2. README invents two global IIO attribute names

**Location:** [README.rst:350](drivers/power/adp20086/README.rst#L350)

README lists `vin_uv_threshold` and `vin_ov_threshold` under "Global Attributes". Neither string exists in `iio_adp20086.c` (grep: zero hits). The real attributes are **`uvin`** and **`ovin`**, and they live on the `vin` *channel*, not in the global list. The actual nine global attributes are `gpio_en`, `gpio_en_available`, `intb`, `pece`, `part_id`, `revision`, `int_mask`, `int_mask2`, `int_mask3`.

### D-3. Project README documents the wrong INTB pin

**Location:** [README.rst:34](projects/adp20086/README.rst#L34) — says **P1_9**; `parameters.h:38-39` defines `INTB_PORT 1`, `INTB_PIN 8` → **P1_8**. Anyone wiring the board from the README gets no interrupts.

### D-4. Project README is incomplete and unpublished

**Location:** [README.rst:1](projects/adp20086/README.rst#L1)

Missing the "No-OS Supported Examples", "No-OS Supported Platforms" and "Build Command" sections that every sibling project README carries. It is also the only power project with no `doc/sphinx/source/projects/power/adp20086.rst` stub, so it never renders in the docs. (The *driver* README is correctly wired at `doc/sphinx/source/drivers/power/adp20086.rst`.)

### D-5. `intbsts` is documented backwards

**Location:** [adp20086.h:297](drivers/power/adp20086/adp20086.h#L297)

```c
/** Internal BIST in progress. */
bool intbsts;
```

The datasheet defines STAT5 bit 4 as *"INTBSTS. INT Stuck High Built-in Self-Test"* — a **fault**, not a progress flag. `ADP20086_SELFTEST_FAULT_MASK` already treats it as fatal (`GENMASK(5,2)` covers bit 4), so the comment contradicts the code immediately below it. Given the known BIST-vs-power-sequencing behaviour on this part, a misleading comment here is expensive: `init()` returns `-ENODEV` and the comment sends the reader looking for a transient.

### D-6. README places `uvout` on the wrong channels

**Location:** [README.rst:335](drivers/power/adp20086/README.rst#L335) — documents `uvout` under "Output Channel Attributes" (the four `iout` channels); it is actually an attribute of the `vout` channels.

---

## Dead code & comment noise

### X-1. Dead `#ifdef ADP20086_EVKIT_LOAD`

**Location:** [basic_example.c:162](projects/adp20086/src/examples/basic_example/basic_example.c#L162)

`ADP20086_EVKIT_LOAD` is defined nowhere in the repo (verified across all `.c/.h/.conf/Kconfig/CMakeLists`). The block containing `adp20086_set_iopen()` and `adp20086_enable_open_protection()` is **never compiled**, so the example silently does not demonstrate open-load protection. Old L-3, still present. Either define it in `basic_example.conf` or drop the guard.

### X-2. ~500 lines of triplicated wrappers

**Location:** [adp20086.c:1436](drivers/power/adp20086/adp20086.c#L1436) onward

The `set_X` / `get_X_code` / `get_X_<unit>` pattern is repeated for LDET, OVIN, UVIN, IOPEN and UVOUT — fifteen near-identical functions that each forward to the already-table-driven `adp20086_set_param`/`get_param_code`/`get_param_microunits`. The table-driven core is the right design; the wrappers around it are the bulk. Worth collapsing, though this is a readability investment rather than a defect.

### X-3. Eight dead register macros, with the same masks open-coded

**Location:** [adp20086.h:119](drivers/power/adp20086/adp20086.h#L119)

`ADP20086_LDET_EN_MASK`, `ADP20086_OPEN_PROT_MASK`, `ADP20086_LDET_EN()`, `ADP20086_OPEN_PROT()`, `ADP20086_CONFIG_EN()`, `ADP20086_CHAN_STATUS_LOAD_DET()`, `ADP20086_CHAN_STATUS_OPEN()` and `ADP20086_MAX_ILIM_UA` have exactly one occurrence each — their own definition. Meanwhile [adp20086.c:1198](drivers/power/adp20086/adp20086.c#L1198) open-codes the very masks two of them name:

```c
status->load_detected = no_os_field_get(NO_OS_GENMASK(7, 4), stat4);
status->open_load     = no_os_field_get(NO_OS_GENMASK(3, 0), stat4);
```

Use the named macros and delete the rest. (The seven `*_ADDR` I²C constants are *not* dead code — they are legitimate public API even with one in-tree user.)

### X-4–X-6. Smaller cleanups

- **X-4** [basic_example.c:205](projects/adp20086/src/examples/basic_example/basic_example.c#L205) — `if (ret) goto remove_dev;` directly after a `printf` that cannot set `ret`; copy-paste leftover.
- **X-5** [basic_example.c:76](projects/adp20086/src/examples/basic_example/basic_example.c#L76) — two unused locals (`ch_status`, `dev_status`); unused `#include <stdlib.h>` at [iio_adp20086.c:10](drivers/power/adp20086/iio_adp20086.c#L10). *(The `no_os_util.h` include in `iio_example.c` is **used** — `NO_OS_ARRAY_SIZE` — and should stay.)*
- **X-6** [adp20086.c:961](drivers/power/adp20086/adp20086.c#L961) — malformed doxygen block (bare blank line breaks the comment); two `//` comments at [:524](drivers/power/adp20086/adp20086.c#L524) in an otherwise exclusively `/* */` file, each merely restating the next line. Also [iio_adp20086.c:1144](drivers/power/adp20086/iio_adp20086.c#L1144), whose doxygen makes a gratuitous and inaccurate claim about the adp5055 driver.

---

## Suggested fix order

1. **C-2, API-1 together.** Make `enum adp20086_channel` unsigned-safe and add explicit `_all()` entry points. Doing this first means the guard fixes in step 2 are written once against the final signature.
2. **C-1 + API-3.** Add `adp20086_set_ilim_ua()` with the correct `(ua / LSB) − 1` transfer function, then make the IIO layer call it. Fixes the critical off-by-one at the root instead of at the call site.
3. **C-3 + H-2 together.** Convert both mask-write paths to `adp20086_update_register()`; they share the same masks, so fix them in one commit and correct the README sentence at the same time.
4. **H-1, H-6, H-7, H-8.** Build and CI hygiene — `select IRQ`, delete the label, run the repo's astyle over the two examples, remove `compile_commands.json`. Cheap, and makes CI green so later changes get real signal.
5. **H-3, H-4, H-5.** IIO input validation. All three are in the same two functions; do them in one pass.
6. **M-1 … M-10.** Correctness and lifecycle. M-4 and M-5 touch `struct adp20086_init_param`, so land them together.
7. **D-1 … D-6.** Documentation, *after* the API changes above — otherwise the README gets rewritten twice.
8. **API-2, API-4, X-1 … X-6.** Structural and cosmetic cleanup last; none of it blocks a merge, and all of it conflicts with earlier steps if done first.

## What is already good

The table-driven core is the right shape: `adp20086_int_maps`, `adp20086_chan_feature_maps` and `adp20086_param_maps` replaced what the previous audit measured as ~290 lines of switch ladders, and they make the register layout auditable in one place. All **76** public functions carry doxygen (verified by script) — the old "67 of 68 undocumented" finding is fully resolved. `ADP20086_ILIM_MASK` is now correct for all four channels, proven by compiling against the real `NO_OS_GENMASK`. The four `drivers/power/adp20086/*.{c,h}` files pass the repo's own astyle config unmodified. The INTB design is genuinely careful: the ISR does no bus traffic, and `adp20086_process_interrupt()` re-samples the pin after re-arming specifically to catch a fault latched during the status read — with a comment explaining why. `clear_device_status()` saves and restores `CONFIG.CLR` so the flush has no lasting side effect. Several comments explain non-obvious datasheet behaviour rather than restating code, which is the right instinct; the stale ones flagged above are the exception, not the pattern.
