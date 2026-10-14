## Pull Request Description

Three changes to the ADF4382 driver, all needed before the part can be used as
the device clock PLL in a synchronised multi-chip setup.

**Legacy silicon.** Some boards carry older ADF4382 revisions whose default
register values differ from the current one. Detect those versions and apply
the matching defaults so both populate correctly.

**Phase API in femtoseconds.** The phase alignment sequence is updated to the
values established by bench characterisation, and `adf4382_set_phase_adjust()`
now takes femtoseconds rather than picoseconds, so sub-picosecond delays can be
expressed at all — the register LSB is a few hundred femtoseconds, which the
old unit could only address in steps of three or four. `adf4382_get_phase_adjust()`
and `adf4382_set_auto_align()` are new.

The IIO `phase` attribute keeps its picosecond interface: both the read and the
write path convert at the call site, so nothing user-visible changes unit. The
basic example does the same. Without that the attribute would have silently
shifted by 1000× in both directions.

**PFD range checking.** `adf4382_pfd_compute()` now returns an error code and
hands the frequency back through an out parameter, so a PFD outside the
supported range is rejected instead of propagating into the divider maths.
Updated across all nine call sites; the function is static, so nothing outside
the driver is affected.

**Series** — each of these applies to `main` on its own; only the last is stacked:

1. FCE-1057-ad9088-api - ad9084/ad9088 vendor Apollo API
2. FCE-1054-cmake-firmware-blob - firmware blob embedding
3. FCE-1055-crc32 - no_os_crc32
4. FCE-1056-adf4030-jesd204 - adf4030 JESD204 support
5. **this PR** - adf4382 phase alignment
6. FCE-112-add-ad9088-support - ad9084/ad9088 driver + VCU118 project (needs 1-5)

#### Testing

`projects/adf4382` (stm32, sdp-ck1z) builds clean in both `basic.conf` and
`iio.conf`.

astyle 3.1 with `.github/config/astyle_config` over the four changed files:
no reformatting.

#### Related

FCE-112

## PR Type
- [ ] Bug fix (change that fixes an issue)
- [x] New feature (change that adds new functionality)
- [ ] Breaking change (has dependencies in other repos or will cause CI to fail)

## PR Checklist
- [x] I have followed the [Coding style guidelines](http://analogdevicesinc.github.io/no-OS/drivers_guide.html#coding-style)
- [x] I have complied with the [Submission Checklist](http://analogdevicesinc.github.io/no-OS/contributing.html#submission-checklist)
- [x] I have performed a self-review of the changes
- [x] I have commented my code, at least hard-to-understand parts
- [x] I have build all projects affected by the changes in this PR
- [ ] I have tested in hardware affected projects, at the relevant boards
- [x] I have signed off all commits from this PR
- [ ] I have updated the documentation (wiki pages, ReadMe etc), if applies 