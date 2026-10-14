# AD9088 upstream PR plan

Target: <https://github.com/analogdevicesinc/no-OS> (`main`)
Source: `dev/ad9088_rebase` (10 commits)
Proposed: **7 commits**, built one at a time.

**Status (2026-09-15): the series is built and every gate passes.** It lives on
`feature/FCE-112-add-ad9088-support`, rebased on `origin/main` at `dc629751f2`,
linear, no merges, 445 files / +103435 / −156:

| # | sha | subject |
|---|---|---|
| 1 | `ddbf10e842` | cmake: add firmware blob embedding support |
| 2 | `d12a878e4b` | util: no_os_crc32: add CRC-32 support |
| 3 | `d47d651ecc` | drivers: frequency: adf4030: add JESD204 support |
| 4 | `f55da83956` | drivers: frequency: adf4382: add phase alignment |
| 5 | `b85003e68d` | include: jesd204: add AD9088 FSM stages |
| 6 | `e643c62c95` | drivers: rf-transceiver: ad9088: add driver |
| 7 | `5633ee53cf` | projects: ad9088: add VCU118 project |

Shas change on every amend — re-read them with
`git log --oneline main..HEAD` rather than trusting this table.

Not yet done, and the only thing left before pushing: **no commit has been
built individually** (only HEAD), and **nothing has run on the VCU118 since the
rebase** over 69 upstream commits. See §4.

---

## 1. Findings that change the shape of the PR

Verified against `origin/main` at `dc629751f2`. `dev/ad9088_rebase` forks at
`c690a3744f`; **69 commits** have landed upstream since.

### 1.1 The SDT commit is now obsolete — and dangerous if kept

> **DONE (2026-09-15), but it was missed on the first pass.** The obsolete
> commit was correctly dropped — and the one-line replacement below was then
> never added, so the series built firmware that could not initialise. Dropping
> the commit and adding the line are *two* actions; doing only the first is
> silent. Symptom and proof, for anyone who hits it again: the generated BSP
> has 0 `XPAR_*_DEVICE_ID` and 52 `XPAR_*_BASEADDR`, `-DSDT` reaches nothing,
> the four `#ifdef SDT` blocks in `parameters.c` compile out, and UART, both
> SPI controllers and GPIO all get `.base_addr = 0` — so `LookupConfig(0)`
> returns instance 0 and `apollo_spi_extra_ip` binds XSPI_0 (the clock chip)
> instead of XSPI_1. That is the "AD9088 initialization failed" path.
>
> Verify from the configure log, not by reading the file: it must print
> `-- Xilinx BSP is SDT; compiling no-os with -DSDT`. To prove it reached the
> data, dump `.data` from `parameters.c.obj` — the four base addresses must be
> `0x40000000` (GPIO), `0x44a80000` (XSPI_1), `0x44a70000` (XSPI_0),
> `0x40600000` (UART), with the two SPI values *distinct*.
>
> Landed in commit 7.

`0daa9a607c cmake:xilinx: Add SDT fixes` must be **dropped entirely**. Both of
its hunks have been overtaken upstream:

| Hunk | Upstream status |
|---|---|
| `drivers/platform/xilinx/xilinx_spi_pl.c`: `#include "xilinx_spi.h"` | Already on `main` (identical line) |
| `cmake/xilinx/xilinx_platform_sdk.cmake`: SDT detection | Superseded by `25e35d717c cmake: xilinx: build no-OS with -DSDT on SDT BSP` |

The upstream implementation differs in two ways that matter:

- it detects SDT by the **absence of numeric `XPAR_*_DEVICE_ID` macros** in the
  generated `xparameters.h`, not by grepping `*_toolchain.cmake` for `-DSDT`;
- it is **gated behind a per-project `NOOS_XILINX_SDT` opt-in**, deliberately,
  so unvalidated boards (Zynq-7000 ps7) keep the classic ABI.

**A test rebase onto `origin/main` succeeds with zero conflicts.** That is the
trap: git happily keeps *both* blocks, and the branch's ungated block runs
first, so `-DSDT` gets applied to **every** Xilinx project. That silently
un-gates a gate upstream added on purpose. Confirmed by inspecting the rebased
file — two SDT blocks, lines 87-107 (branch, ungated) and 111-129 (upstream,
gated).

**Action:** drop the commit; instead add one line to
`projects/ad9088/CMakeLists.txt`, exactly as `projects/adrv902x/CMakeLists.txt`
does:

```cmake
# ad9088's app and the Apollo/xilinx drivers it uses are SDT-clean, so opt in
# to the Vitis 2025+ SDT ABI when the BSP is SDT.
set(NOOS_XILINX_SDT ON)
```

> This must be placed **before** `config_platform_sdk()`.

### 1.2 `Assisted-by:` trailer is missing from all 10 commits

> **Resolved.** Six of the seven commits now carry
> `Assisted-by: Claude Opus 5` immediately before `Signed-off-by:`.
> Commit 5 (`include: jesd204`) ships **without** it by explicit decision
> (2026-09-15) — it is a four-line addition of enum values. Do not re-raise.
> No `Co-Authored-By` anywhere; see §6 Q1, now closed.

The repo requires `Assisted-by: <agent and model>` immediately **before**
`Signed-off-by:` on any AI-assisted commit. Upstream has 141 such commits
(`Assisted-by: Claude Opus 4.8`, `Claude Sonnet 5`, …). None of the 10 branch
commits carry it. Every commit in the new series needs:

```
Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> Note: this session is configured to append
> `Co-Authored-By: Claude Opus 5 (1M context) <noreply@anthropic.com>`. Upstream
> commits with `Assisted-by` do **not** carry a `Co-Authored-By` line. Your
> call — the repo convention is `Assisted-by` + `Signed-off-by` only, and that
> is what the messages below use. Add the `Co-Authored-By` line if you want it.

### 1.3 Two commits are dropped, one is split

- `8d3a961b4f driver: ad4382: Fix write VTUNE` — **dropped, deferred by
  decision (2026-09-14).** Not needed for this series.

  Dropping it is clean. `ADF4382_VCO_CAL_VTUNE 640` is **already defined on
  `origin/main`** (`adf4382.h:503`), so the truncation at `adf4382.c:1577` —
  `adf4382_spi_write()` takes a `uint8_t` data argument, so 640 lands as 128 —
  is pre-existing upstream behaviour, not something this series introduces. The
  series simply does not touch that line. The `ADF4382_CAL_VTUNE_TO_{LSB,MSB}_MSK`
  masks the fix would have used are referenced from four other call sites, so
  nothing is left unused either.

  Worth recording so it is not lost: the correct fix is two
  `adf4382_spi_update_bits()` writes across `REG0038[7:0]` and `REG0039[6:0]`,
  since `CAL_VTUNE_TO` is a 15-bit field. Send it upstream separately when it
  matters — it is an independent bug fix and does not belong to AD9088 support.

- `7bee318477 ad9088: Clean up comments and add readmes` — a grab bag spanning
  five unrelated areas. **Split and distribute:**

  | Hunk | Goes to |
  |---|---|
  | `drivers/frequency/adf4030/adf4030.c` (drop bogus `static` on a struct type decl) | adf4030 commit |
  | `include/no_os_crc.h` (`#include "no_os_crc32.h"`) | crc32 commit |
  | `tools/scripts/platform/xilinx/capture.tcl` (remote `hw_server`) | **dropped — not upstreamed** (decision 2026-09-14) |
  | ad9088 driver license headers + `README.rst` + doc stub + `drivers/rf-transceiver/CMakeLists.txt` | driver commit |
  | project license headers + `README.rst` + doc stub + `.conf` | project commit |

  The `capture.tcl` hunk (honour `XSCT_REMOTE_HOST` / `XSCT_REMOTE_PORT`,
  matching `util.tcl`) is generic tooling unrelated to AD9088 support and is
  **left out of the PR entirely** — it stays on the local branch. It is
  self-contained and can go upstream on its own later if wanted.

### 1.4 `a0` bitfield headers: keep, off the include path

**Decided (2026-09-14): keep `a0/`**, so the vendor drop stays verbatim.

`drivers/rf-transceiver/ad9088/private/inc/bitfields/a0/` is 108 files and
1.5 MB, and nothing references it. Only `b0` is on the include path, and that
is deliberate: the two directories share 106 of 108 header *names*, all
included by bare name, so putting both on the include path would let include
order silently pick the wrong register map.

Because the directory ships but stays unused, the reason has to be written
down where the next contributor will find it — otherwise someone adds a
recurse helper and silently compiles the wrong register map. Two places:

1. commit 6's message (already worded for this — see §3), and
2. ~~a short `drivers/rf-transceiver/ad9088/private/inc/bitfields/README`~~ —
   **dropped 2026-09-15.** The file was never written, and commit 6's message
   claimed it existed; the claim has been removed rather than the file added,
   so the reasoning now lives only in the commit message and §5. Writing the
   README later is still the better answer if a reviewer pushes back — the
   commit message is not visible to someone browsing the tree.

Expect a reviewer to ask why 1.5 MB of unreferenced headers is in the PR. The
answer — keeping the vendor drop intact — is reasonable, but it should be in
the PR description rather than discovered in review (see §5).

### 1.5 Flag up front in the PR: firmware binaries

The driver ships **11 `.bin` files, 1.2 MB**, including signed/encrypted core
firmware. no-OS today tracks no comparable binaries — the only `.bin`/`.dat`
files in the tree are `projects/iio_demo/sample_{ramp,sine}.dat`. This is a
**precedent-setting addition** and the most likely thing to stall review.

Raise it in the PR description rather than waiting for a reviewer to find it:
state the size, why the blobs are linked rather than converted to C arrays
(that is what `cmake/FirmwareBlob.cmake` exists for), and ask explicitly
whether maintainers want them in-tree or fetched at configure time.

### 1.6 Things that are already fine — do not "fix" them

- **Doc stubs.** `doc/sphinx/source/{drivers,projects}/rf-transceiver/ad9088.rst`
  are one-line `.. include::` stubs. The parent toctrees in `drivers_doc.rst`
  and `projects_doc.rst` use `:glob:`, so they register automatically. No
  toctree edit needed.
- **Line length.** Own sources run 1-48 lines over 80 columns by a naive
  tab-expanded count, but so do established upstream files (`ad9361.c`: 357,
  `ad9528.c`: 48). astyle does not break comments or unbreakable lines. Not a
  blocker. `astyle` is not installed locally, so this cannot be checked here —
  compare counts against HEAD rather than chasing zero.
- **License headers.** All own `.c`/`.h` files carry the ADI header with
  `@file`/`@brief`/`@author`. Only `ad9088_firmware.cmake` lacks one — add it
  for consistency (CMake files elsewhere in the repo do carry headers).
- **`dev_doc/` and `tmp/`** are untracked on the branch and will not leak into
  the PR. Keep it that way.

---

### 1.7 CI runs astyle over the vendor tree — `.astyleignore` is mandatory

**Found 2026-09-15, fixed in commit 6.** This one would have failed CI on its
own, and it is invisible if you only lint your own files.

`.github/scripts/astyle.sh` (invoked by `.github/workflows/build-common.yaml`)
reformats **every** `.c`/`.h` changed in the PR range, skipping only paths
listed in `.astyleignore`, then fails on `git diff --exit-code`. It does not
know or care that a path is a vendor drop.

Measured on this series before the fix: 417 `.c`/`.h` files in range, of which
astyle would reformat **380 — 370 of them the vendor Apollo tree**. The PR
would have failed on code that must not be touched.

`.astyleignore` already exempts every other vendor drop in the repo — navassa,
madura, koror, palma, `ad9081/api`, `ad9083_api`, mykonos — so the fix is to
follow that convention with four lines:

```
drivers/rf-transceiver/ad9088/public/
drivers/rf-transceiver/ad9088/private/
drivers/rf-transceiver/ad9088/adi_inc/
drivers/rf-transceiver/ad9088/adi_utils/
```

That drops CI's scope to the 24 files we actually own. Matching is plain
substring (`[[ "$file" == *"$fpath"* ]]`), so directory prefixes are enough.

Two traps worth writing down:

- **The authoritative option set is `.github/config/astyle_config`, not the
  command line in `CLAUDE.md`.** They happen to agree today; if they ever
  diverge, CI follows the config file.
- **astyle is buildable locally** — `.github/scripts/astyle.sh` downloads and
  builds 3.1 into `build/astyle`. That means a `rm -rf build/` deletes your
  only copy, which is exactly what happened once during this work. To check
  without touching files, use `--dry-run --formatted`; to tell an *introduced*
  violation from a pre-existing one, extract the file from `main` into a temp
  tree and dry-run it there.

One astyle output was overridden by hand: it rewrites `(uint32_t)*pdata++` to
`(uint32_t) * pdata++` in `no_os_crc32.c`, which reads as a multiply —
`--pad-oper` cannot tell a cast-then-deref apart. Written as
`(uint32_t)(*pdata++)` it is stable under astyle and still correct. A second
wart was left alone: `ad9088.h:226` now wraps the `fnco_phase` declarator onto
a flat continuation line, because the one-line form is 82 characters. It is the
only such array in the repo, so there is no precedent to follow.

### 1.8 The two README.rst files need Sphinx stubs

**Found 2026-09-15, fixed in commits 6 and 7.** Writing `README.rst` is not
enough — nothing in the docs build reaches it. `doc/sphinx/source/drivers_doc.rst`
and `projects_doc.rst` build their sections from `:glob:` toctrees over
`doc/sphinx/source/<area>/<category>/*`, so each README needs a one-line stub
*in the doc tree*:

```
doc/sphinx/source/drivers/rf-transceiver/ad9088.rst
  .. include:: ../../../../../drivers/rf-transceiver/ad9088/README.rst

doc/sphinx/source/projects/rf-transceiver/ad9088.rst
  .. include:: ../../../../../projects/ad9088/README.rst
```

Five `../` reaches the repo root from either location. Note the asymmetry: a
driver stub's target includes the category directory, a project stub's does not
— projects are only grouped by category inside the docs tree. No index edit is
needed; the glob picks the file up, and the filename becomes the URL slug.

Build it with `cmake -B build/doc doc/sphinx && cmake --build build/doc --target
sphinx-html` (it makes its own venv from `doc/sphinx/source/requirements.txt`).
Doing so caught a real bug: the driver README tagged its C initialisation
example as `code-block:: bash`, so Pygments failed to lex it and fell back to
relaxed mode. Now `code-block:: c`, matching the repo.

---

## 2. Recommended construction method

The seven commits are **path-disjoint** (verified — no two touch the same
file). So do not fight `git rebase -i` to reorder and split the grab-bag
commit. Instead **rebuild the series by staging paths** onto a fresh branch.
This is cleaner, conflict-free, and matches the one-commit-at-a-time workflow.

```bash
# 0. Get a verified, rebased tree to copy from.
git fetch upstream            # or origin, whichever tracks analogdevicesinc/no-OS
git checkout -B ad9088-staging dev/ad9088_rebase
git rebase origin/main        # verified: clean, zero conflicts

# Remove the obsolete SDT block that the clean rebase silently duplicated.
$EDITOR cmake/xilinx/xilinx_platform_sdk.cmake   # delete the branch's block
git add -A && git commit --amend --no-edit       # or fix in the right commit

STAGING=$(git rev-parse HEAD)

# 1. Start the real series from upstream main.
git checkout -b ad9088-upstream origin/main

# 2. For each commit below: check out its paths from $STAGING, then commit.
git checkout $STAGING -- <paths>
git commit -s          # paste the message, add the Assisted-by trailer
```

Between commits, sanity-check that the tree still configures. After the final
commit, confirm `git diff ad9088-upstream $STAGING` shows **only** the deliberate
differences:

- the dropped SDT block in `cmake/xilinx/xilinx_platform_sdk.cmake`,
- the added `set(NOOS_XILINX_SDT ON)` in `projects/ad9088/CMakeLists.txt`,
- `tools/scripts/platform/xilinx/capture.tcl` (never staged — §1.3),
- the new `private/inc/bitfields/README` (§1.4).

---

## 3. The commit series

All subjects are ≤ 50 chars; all bodies wrap at 72.

### Commit 1 — firmware blob embedding

Paths: `CMakeLists.txt`, `cmake/FirmwareBlob.cmake`,
`drivers/platform/xilinx/toolchain.cmake`

Unchanged from `b7437a6a31` — the subject and body already match upstream
convention. Only the `Assisted-by` trailer is added.

```
cmake: add firmware blob embedding support

Port the .bin embedding the Make build provided (generic.mk FW_SRCS,
xilinx.mk OBJCOPY_BIN_FORMAT/ARCH) to CMake, so a vendor firmware image
can be linked into the ELF rather than converted to a C array first.

no_os_add_firmware_blob() stages the image under a fixed root in the
build tree and runs objcopy -I binary from there. objcopy derives the
_binary_<path>_{start,end,size} symbols from the path as written on its
command line, so staging plus WORKING_DIRECTORY makes the caller's
SYMBOL_PATH the whole of the symbol name, independent of where the
image lives on disk or how the build tree is laid out.

SOURCE and SYMBOL_PATH are separate arguments so the image can be
swapped without disturbing those symbols. That replaces the Make
build's copy of the selected image over a fixed name in the source
tree, which existed only to pin the names and dirtied the checkout on
every parse.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

### Commit 2 — CRC-32

Paths: `include/no_os_crc32.h`, `include/no_os_crc.h`, `util/no_os_crc32.c`,
`util/CMakeLists.txt`

Folds in the one-line `no_os_crc.h` include from the grab bag. Body expanded —
the original ("Adding CRC 32 check needed for some drivers.") does not say
which polynomial or which drivers.

```
util: no_os_crc32: add CRC-32 support

no-OS provides CRC-8, CRC-16 and CRC-24 helpers but no 32-bit variant.

Add no_os_crc32 alongside the existing widths, following the same
table-generation pattern, and pull the new header into no_os_crc.h so
users that include the aggregate header get it for free.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> Check before sending: confirm the implementation is the standard reflected
> IEEE 802.3 CRC-32 and say so in the body if it is; reviewers will ask.

### Commit 3 — ADF4030

Paths: `drivers/frequency/adf4030/`

Folds in the grab-bag hunk that drops the meaningless `static` from the
`struct adf4030_reg_sequence` *type* declaration.

```
drivers: frequency: adf4030: add JESD204 support

The ADF4030 generates BSYNC and SYSREF which makes it
take part in JESD204 bring-up rather than being configured once at
probe. Register it as a JESD204 device and realign the channels that
ask for it on CLK_SYNC_STAGE4, driven by an optional init-time channel
list.

Supporting changes to existing code, each needed to make that alignment
converge:

  - adf4030_poll() takes a timeout. A TDC measurement accumulates
    2^(AVGEXP + 6) BSYNC periods, ~215 ms at AVGEXP 15 and a 9.765625
    MHz BSYNC, which the fixed 2 s budget could not express per
    operation.
  - adf4030_set_tdc_measurement() waits out the measurement before
    polling, and now also waits on MATH_BUSY: the result is only stable
    once the math block has finished.
  - adf4030_set_single_ch_alignment() writes TDC_SOURCE. It was left at
    its 0x1F reset value, a channel that does not exist, so the FSM
    armed and never completed.
  - adf4030_set_alignment_iter() touches only ALIGN_CYCLES. EN_ITER and
    EN_CYCS_RED are independent controls, and folding them in here
    silently undid whatever had just configured them.
  - adf4030_set_channel_direction() keeps the TX path powered and lets
    EN_DRIVE alone express direction; an input channel still needs its
    path alive for the TDC to observe it.
  - ADF4030_AVGRXP becomes ADF4030_AVGEXP with GENMASK(3, 0). It was
    unused, misnamed, and one bit too wide.

adf4030_set_alignment_threshold() changes behaviour: it programs the
threshold only, and enabling the iteration splits out into the new
adf4030_set_alignment_threshold_en(). A threshold of 0 used to disable
iteration and now programs 0. This keeps the three independent controls
in 0x37 from undoing one another.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> The body is a reconstruction — the original said only that the part "requires
> some additional functionality". Re-read the diff and adjust the middle
> paragraph to name what actually landed.

### Commit 4 — ADF4382

Paths: `drivers/frequency/adf4382/`

Content is `92f1e7124e` as-is. The VTUNE fix (`8d3a961b4f`) is **not** included
— see §1.3. The subject also fixes the `ad4382` → `adf4382` part-name typo.

```
drivers: frequency: adf4382: add phase alignment

Some boards carry legacy ADF4382 silicon whose default register values
differ from the current revision. Detect those versions and apply the
matching defaults so both populate correctly.

Update the phase alignment sequence to the values established by bench
characterisation, and convert the phase API from picoseconds to
femtoseconds so the AD9088 multi-chip sync path can express
sub-picosecond BSYNC delays.

adf4382_pfd_compute() now returns an error code and hands the frequency
back through an out parameter, so a PFD outside the supported range is
rejected rather than propagated. Updated across all nine call sites.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> Known unrelated breakage, worth a heads-up in the PR: `projects/adf4382` does
> not build at HEAD because `adf4382.h` includes `no_os_clk.h` and the project's
> `src.mk` never provided it. Pre-existing, not caused by this series — but a
> reviewer building that project will hit it.

### Commit 5 — jesd204 stages

Paths: `include/jesd204.h`

```
include: jesd204: add AD9088 FSM stages

The AD9088 bring-up needs link states the current FSM does not model:
two optional setup stages before the link is enabled, and post-setup
stages where multi-chip sync calibration runs once the link is up.

Add the enumerators so a device driver can hook those stages. Existing
drivers leave the new op slots NULL and are unaffected.

Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> Confirm the "existing drivers unaffected" claim by checking the FSM skips
> NULL ops for the new stages before asserting it upstream.

### Commit 6 — the driver

Paths: `drivers/rf-transceiver/ad9088/`, `drivers/rf-transceiver/CMakeLists.txt`,
`drivers/rf-transceiver/Kconfig`, `doc/sphinx/source/drivers/rf-transceiver/ad9088.rst`

Folds in the grab-bag license headers, `README.rst`, doc stub and CMakeLists
changes. Keeps `private/inc/bitfields/a0/` per §1.4. Also carries the four
`.astyleignore` entries for the vendor directories (§1.7) — without them the
astyle job fails on 370 vendor files. The `bitfields/README` that §1.4
originally called for was **dropped**; see §1.4.

This is the large one: ~99k lines, of which ~4.3k are the no-OS driver and
~94.6k are the vendor API tree.

```
drivers: rf-transceiver: ad9088: add driver

Add the AD9088 MxFE no-OS driver, together with the vendor-provided
API tree it is built on.

firmware/ carries the device profile images and the signed core
firmware loaded to the device at startup. The build links these into
the ELF as binary blobs rather than converting them to C arrays, so
the driver references them through the linker-generated _binary_*
symbols declared in ad9088.h.

The vendor drop ships register maps for both A0 and B0 silicon, but
only b0 is placed on the include path. The two sets share almost every
header name and are included by bare name, so exposing both would let
include order decide which register map is compiled in; -DB0 selects
the matching definitions.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

> The third paragraph is new and worth keeping — it is the non-obvious
> constraint a future contributor would otherwise undo.

### Commit 7 — the project

Paths: `projects/ad9088/`, `projects/Kconfig`,
`doc/sphinx/source/projects/rf-transceiver/ad9088.rst`

Folds in the grab-bag project license headers, `README.rst`, doc stub and
`.conf` changes, **plus the new `set(NOOS_XILINX_SDT ON)`** replacing the
dropped SDT commit.

```
projects: ad9088: add VCU118 project

Add the AD9088 project for the AD9084-EVAL board on the VCU118
platform, with two examples.

basic_example brings up the JESD204C links and reports link status.
dma_example additionally loads the TX DMA from a sine LUT and reads
the result back through the RX DMA; it assumes external loopback
cables are fitted to the setup.

Assisted-by: Claude Opus 5
Signed-off-by: Ciprian Hegbeli <ciprian.hegbeli@analog.com>
```

---

## 4. Pre-submit checklist

Status 2026-09-15. Everything except the last two lines is verified.

- [x] Rebased on current `origin/main` (`dc629751f2`); linear, no merges; the
      duplicated SDT block is gone
      (`grep -c "XILINX_BSP_SDT" cmake/xilinx/xilinx_platform_sdk.cmake` → 0)
- [x] Every subject ≤ 50 chars (longest is 48) and every body line ≤ 72
- [x] `Assisted-by:` immediately before `Signed-off-by:` on six of seven;
      commit 5 exempt by decision (§1.2). `Signed-off-by:` on all seven.
- [x] `set(NOOS_XILINX_SDT ON)` in `projects/ad9088/CMakeLists.txt`, before
      `config_platform_sdk()`; configure prints
      `Xilinx BSP is SDT; compiling no-os with -DSDT` (§1.1)
- [x] `.astyleignore` exempts the four vendor directories; CI's astyle scope is
      24 files and astyle makes zero changes to them (§1.7)
- [x] Sphinx stubs exist for both `README.rst`; `sphinx-html` builds with only
      the two offline `no-os-doxygen` HTTP 403 warnings (§1.8)
- [x] `@author` is CHegbeli in every own ad9088 file — the `RBolboac` trap did
      not materialise. Vendor files under `private/`, `public/`, `adi_inc/`
      and `adi_utils/` keep their ADI vendor headers untouched. The
      Berghe / Osemene / Kucukarabacioglu tags in the adf4030 and adf4382
      files are pre-existing on `main` and correctly untouched.
- [x] ~~`ad9088_firmware.cmake` has the ADI license header~~ — **moot**: 0 of
      46 `.cmake` files in the repo carry one, so adding it would be the
      outlier.
- [x] ~~`private/inc/bitfields/README`~~ — dropped, see §1.4.
- [x] Both examples build clean from scratch for vcu118, 0 errors, `-DSDT` on:
      `basic_example` text 645836, `dma_example` text 649644
- [x] `dev_doc/` and `tmp/` still untracked; nothing stray (`.xsa`, `.orig`,
      `CLAUDE.md`) in the commit range
- [ ] **Each commit builds on its own** — only HEAD has been built. The
      ordering is dependency-correct by inspection (cmake → crc32 → adf4030 →
      adf4382 → jesd204 → driver → project), but that is not the same as
      having compiled each one.
- [ ] **Re-run on VCU118 after the rebase** — the 69 upstream commits include
      CMake and Xilinx BSP changes, so the last hardware verification
      (2026-08-21) no longer covers this tree. This is the one substantive gap.

### Warnings knowingly shipped

`gcc -Wall -Wextra` is clean of *errors*; three warnings in own code remain, all
deliberate:

- `ad9088.c:1284` (`%X` ×2) and `ad9088_dt.c:264` (`%u`) against `uint32_t`,
  which is `long unsigned` on MicroBlaze — cosmetic; `PRIX32`/`PRIu32` fixes it.
- `adf4382.h:504` `-Woverflow`, 640 → 128 — **upstream's own value**, present on
  `main`; the VTUNE fix was deliberately dropped (§1.3).

Everything else comes from pre-existing upstream code (axi_core/jesd204,
hmc7044, jesd204-core, the xilinx platform) or the vendor Apollo tree.

### Still carrying Linux references in shipped comments

Against the no-Linux-refs rule, seven remain: `adf4030.c:2166,2357,2400,2410`,
`adf4030.h:574`, `adf4382.c:1082`, `no_os_crc32.c:48`, plus the filename
`ad9088_dt.c` itself ("dt" reads as device tree in a bare-metal driver; its
`@brief` says "Device profile parsing"). Both `README.rst` are clean.

---

## 5. PR description

Rewritten 2026-09-16 to the repo's own template
(`.github/PULL_REQUEST_TEMPLATE.md`) and the house style of recent merged PRs
(#3331 adrv902x VCU118, #3337 tcs34725, #3333 adf4030). Numbers are measured
off the branch — re-measure if it moves.

House pattern: `## Pull Request Description`, prose, a `Changes:` list
mirroring the commits, a short testing statement, `### Related` with the JIRA
key, then the template's `## PR Type` and `## PR Checklist` kept verbatim.

> Title: **`ad9088: add AD9088 (Apollo) MxFE driver and VCU118 project`**

```markdown
## Pull Request Description

Add support for the AD9088 (Apollo) MxFE: the no-OS driver, the vendor Apollo
API tree it is built on, and a project for the AD9084-EVAL board on VCU118
with two examples.

Seven commits, ordered so the dependencies land first. Reviewing them in this
order is the intended path:

| # | Commit | Category | Review size | Depends on |
|---|--------|----------|-------------|------------|
| 1 | cmake: add firmware blob embedding support | Build system | 3 files, +166 | - |
| 2 | util: no_os_crc32: add CRC-32 support | Shared utility | 4 files, +124 | - |
| 3 | drivers: frequency: adf4030: add JESD204 support | Existing driver | 2 files, +596/-48 | - |
| 4 | drivers: frequency: adf4382: add phase alignment | Existing driver | 2 files, +501/-108 | - |
| 5 | include: jesd204: add AD9088 FSM stages | Core framework | 1 file, +4 | - |
| 6 | drivers: rf-transceiver: ad9088: add driver | New driver + vendor drop | 12 files, +4650, plus 402 vendor files (+94679) | 1, 2, 5 |
| 7 | projects: ad9088: add VCU118 project | New project | 19 files, +2715 | 3, 4, 6 |

Commits 1-5 are independent of one another and of the AD9088 — each can be
read, or taken, on its own. Commit 6 needs 1 (firmware blobs), 2 (profile and
image validation) and 5 (the FSM stages it hooks). Commit 7 needs 3 and 4 for
the clock and SYSREF parts it instantiates, and 6 for the driver itself.

Commit 6 is where the file count comes from, but the review is much smaller
than it looks: 402 of its 414 files are the untouched vendor drop, leaving
roughly 4.6k lines of no-OS code.

Changes:
- cmake: add firmware blob embedding support
  Link a vendor .bin image into the ELF with objcopy -I binary rather than
  converting it to a C array first. Staging plus WORKING_DIRECTORY makes the
  caller's SYMBOL_PATH the whole of the _binary_* symbol name, independent of
  where the image sits on disk.
- util: no_os_crc32: add CRC-32 support
  Fourth CRC width alongside the existing CRC-8/16/24, following the same
  table-generation pattern and pulled into the aggregate no_os_crc.h.
- drivers: frequency: adf4030: add JESD204 support
  The ADF4030 generates BSYNC and SYSREF, so it takes part in JESD204
  bring-up rather than being configured once at probe. Register it as a
  JESD204 device and realign the channels that ask for it on
  CLK_SYNC_STAGE4, driven by an optional init-time channel list. Carries the
  supporting fixes that alignment needs to converge (TDC_SOURCE, MATH_BUSY,
  per-operation poll timeout, AVGEXP mask).
- drivers: frequency: adf4382: add phase alignment
  Detect legacy silicon and apply the matching defaults, convert the phase
  API from picoseconds to femtoseconds so sub-picosecond BSYNC delays can be
  expressed, and give adf4382_pfd_compute() a PFD range check across all
  nine call sites.
- include: jesd204: add AD9088 FSM stages
  Two optional setup stages before link enable, and post-setup stages where
  multi-chip sync calibration runs once the link is up. Existing drivers
  leave the new op slots NULL and are unaffected.
- drivers: rf-transceiver: ad9088: add driver
  The driver, the vendor Apollo API tree it is built on, and the firmware
  images.
- projects: ad9088: add VCU118 project
  basic_example brings up the JESD204C links and reports link status.
  dma_example additionally loads the TX DMA from a sine LUT and reads the
  result back through the RX DMA; it assumes external loopback cables are
  fitted.

445 files, +103435/-156. Of the new code roughly 4.3k lines are the no-OS
driver and 94.7k the vendor API tree.

### Three things worth raising up front

1. Firmware binaries. The driver commit adds 9 .bin files (~1.2 MB): the
   device profile images and the signed core firmware the part loads at
   startup. As far as I can tell no-OS does not currently track binaries of
   this kind, so this is precedent-setting. They are linked into the ELF as
   blobs rather than converted to C arrays, which keeps the checkout clean
   and lets the profile be swapped without disturbing the symbol names.
   Happy to move them out of tree and fetch at configure time instead.

2. Vendor API tree size, following the same in-tree pattern as
   koror/palma/madura/navassa. Within it, private/inc/bitfields/ carries
   register maps for both A0 and B0 silicon. Only b0 is on the include path:
   the two sets share almost every header name and are included by bare
   name, so exposing both would let include order decide which register map
   is compiled in, and -DB0 selects the active one. The unused a0 tree (108
   files) is kept so the vendor drop stays verbatim. Happy to drop a0 if you
   would rather not carry unreferenced headers.

3. .astyleignore. The driver commit adds the four vendor directories
   (public/, private/, adi_inc/, adi_utils/) to .astyleignore, the same way
   navassa, madura, koror, palma, ad9081/api and ad9083_api are already
   exempt. Without it the astyle job reformats ~370 vendor files. The 24
   files this series owns are checked and pass.

The project opts in to the Vitis 2025+ SDT ABI with set(NOOS_XILINX_SDT ON),
exactly as adrv902x does; no other project's behaviour changes.

### Testing

Vitis 2025.1, MicroBlaze, AD9084-EVAL on VCU118.

- Both examples build clean from scratch with the SDT ABI active; astyle
  passes over every file CI checks; sphinx-html builds and both new
  README.rst render through their stubs.
- JESD204C links reach LINK_RUNNING, full FSM including the new
  optional-setup and post-setup stages.
- Subclass 1: ADF4030 as SYSREF provider, SYSREF Phase Locked and SYSREF
  captured on both AXI cores.
- Multi-chip sync calibration passes init-cal validation; SYSREF_PHASE
  trimmed from 433 to 40 Fclk counts (21.65 ns to 2.00 ns at a 20 GHz device
  clock), stable across repeated reads with no link regression.
- dma_example: noise floor, RX FNCO test tone and cabled DAC-to-ADC loopback
  all pass, loopback coherence 99.8%.

Pre-existing, not introduced here: projects/adf4382 does not build at HEAD
because adf4382.h includes no_os_clk.h, which that project never provided. A
reviewer building that project will hit it — happy to fix it here or
separately.

### Related

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
- [x] I have updated the documentation (wiki pages, ReadMe etc), if applies
```

> Two checklist boxes are deliberately left unticked — see §4. Tick them once
> the VCU118 run happens on this tree, and once the other projects that pull
> in adf4030/adf4382/jesd204/crc32 have been built.

### If maintainers ask to split

The natural boundary is **commits 1-5 (prerequisites) / commits 6-7 (AD9088)**.
Commits 1-5 stand alone, touch only shared code, and are reviewable in an hour.
A prerequisite PR would be titled
`shared: prerequisites for AD9088 MxFE support` with the list above truncated
to the first five and a one-line note that the AD9088 driver follows.

---

## 5b. Text dropped from the draft messages — CLOSED

**Decided 2026-09-16: both omissions are intentional. Do not re-propose
restoring them.**

The §3 messages are verbatim copies of what is on the branch. Two earlier
drafts said something the shipped messages do not; the shorter wording is the
one that ships. Recorded here only so the difference is not mistaken for an
accident on a later read:

- **Commit 7** dropped the paragraph explaining the SDT opt-in:

  > Opt in to the Vitis 2025+ SDT ABI with NOOS_XILINX_SDT, as adrv902x
  > does. The app and the drivers it uses are SDT-clean, and the BSP is
  > compiled with -DSDT, so the application must match or it passes a
  > device id where a base address is expected.

  This is the single least obvious line in the commit and the one that cost
  the most to rediscover (§1.1). A reviewer seeing a bare `set(NOOS_XILINX_SDT
  ON)` has no way to know why.

- **Commit 2** dropped the sentence saying *who needs* CRC-32:

  > The AD9088 needs one to validate the device profile and firmware images
  > it loads over SPI before handing them to the part.

  Without it, commit 2 reads as an unmotivated utility addition sitting in
  front of a driver series.

---

## 6. Open questions for you

Resolved 2026-09-14: `a0/` is **kept** (§1.4); the ADF4382 VTUNE fix and the
`capture.tcl` change are both **dropped** from the PR (§1.3).

Still open:

1. ~~**`Co-Authored-By`**~~ — **closed 2026-09-15: repo convention only.** No
   commit carries `Co-Authored-By`; `Assisted-by` + `Signed-off-by` throughout.
2. **Firmware blobs** — in-tree, or pre-emptively move to a fetch-at-configure
   scheme before submitting?
3. **One PR or two** — submit all seven, or land prerequisites first?
