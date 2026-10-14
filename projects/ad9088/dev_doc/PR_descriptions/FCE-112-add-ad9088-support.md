## Pull Request Description

Add support for the AD9088 and AD9084 Apollo MxFE: wideband RF DACs and ADCs
with a configurable digital datapath, organised as two independent sides, each
owning its converters, its coarse and fine up/down converters and its JESD204C
link to the host. This adds the no-OS driver and a project for the AD9084-EVAL
board on VCU118.

The driver builds on the Apollo device API and covers bring-up and firmware
load, the JESD204 state machine callbacks, multi-chip sync calibration, and
runtime control of the coarse and fine NCOs. The datapath geometry is not
fixed — a device profile image loaded over SPI describes it, which is why the
firmware images ship with the driver.

`basic_example` brings the links up and reports link status. `dma_example`
additionally loads the transmit DMA from a sine table and reads the result back
through the receive DMA; it expects external loopback cables to be fitted.

**Series** — each of these applies to `main` on its own; only the last is stacked:

1. FCE-1057-ad9088-api - ad9084/ad9088 vendor Apollo API
2. FCE-1054-cmake-firmware-blob - firmware blob embedding
3. FCE-1055-crc32 - no_os_crc32
4. FCE-1056-adf4030-jesd204 - adf4030 JESD204 support
5. FCE-1059-adf4382-phase - adf4382 phase alignment
6. **this PR** - ad9084/ad9088 driver + VCU118 project (needs 1-5)

**Depends on**, and does not build without:

| | |
|---|---|
| Apollo vendor API | the tree this driver wraps |
| `cmake`: firmware blob embedding | `no_os_add_firmware_blob()` |
| `util`: `no_os_crc32` | profile validation in `ad9088_dt.c` |
| `adf4030`: JESD204 support | the project's SYSREF/BSYNC provider |
| `adf4382`: phase alignment | the project's device clock PLL |

#### Testing

Vitis 2025.1, MicroBlaze, AD9084-EVAL on VCU118.

On hardware: the JESD204C links reach LINK_RUNNING through the full FSM
including the new post-setup stages; subclass 1 runs with the ADF4030 as SYSREF
provider, SYSREF phase locked and SYSREF captured on both AXI cores; multi-chip
sync passes init-cal validation; and the DMA example's cabled loopback recovers
the transmitted tone.

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
- [x] I have tested in hardware affected projects, at the relevant boards
- [x] I have signed off all commits from this PR
- [x] I have updated the documentation (wiki pages, ReadMe etc), if applies