## Pull Request Description

Adding support for the AD9088 (Apollo) RF transceiver, and independent of the
rest: it adds a CMake helper for linking a vendor firmware image into the ELF.

It can replace the way firmware binaries are linked into the ELF today, a
common need for RF transceivers. The current method runs
`xxd -i <source.bin> > <dest.h>` to turn the binary into a hex string, copies
that into a `.h` file and loads it from there at run time, as see here:
[the ADRV902x use-case guide][adrv902x-usecase] for an example.

[adrv902x-usecase]: https://developer.analog.com/docs/no-os/projects/rf-transceiver/adrv902x/#adding-or-updating-a-use-case

`no_os_add_firmware_blob()` runs `objcopy -I binary` over the image and links
the resulting object, rather than converting the image to a C array first.
objcopy derives the `_binary_<path>_{start,end,size}` symbol names from the
path as written on its command line, so the function stages the image under a
fixed root in the build tree and runs objcopy from there — the caller's
`SYMBOL_PATH` then *is* the symbol name, whatever the image is called on disk
and however the build tree is laid out. `SOURCE` and `SYMBOL_PATH` are separate
arguments for that reason: the image can be swapped per board or per profile
without disturbing the `extern` declarations that match those symbols.

The Xilinx toolchain file publishes the BFD target and architecture objcopy
needs as `NO_OS_OBJCOPY_BIN_FORMAT` and `NO_OS_OBJCOPY_BIN_ARCH`, cached the
same way `XILINX_ARCH` is so they survive CMake's `try_compile` probe.
MicroBlaze defaults to the big-endian `elf32-microblaze`, so the format is
always passed explicitly.

Nothing in the tree calls the helper yet — the first caller will be the AD9088
driver.

**Series** — each of these applies to `main` on its own; only the last is stacked:

1. FCE-1057-ad9088-api - ad9084/ad9088 vendor Apollo API
2. **this PR** - firmware blob embedding
3. FCE-1055-crc32 - no_os_crc32
4. FCE-1056-adf4030-jesd204 - adf4030 JESD204 support
5. FCE-1059-adf4382-phase - adf4382 phase alignment
6. FCE-112-add-ad9088-support - ad9084/ad9088 driver + VCU118 project (needs 1-5)

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
- [ ] I have build all projects affected by the changes in this PR
- [ ] I have tested in hardware affected projects, at the relevant boards
- [x] I have signed off all commits from this PR
- [ ] I have updated the documentation (wiki pages, ReadMe etc), if applies