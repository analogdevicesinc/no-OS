# AD9088 upstream PR split — 6 PRs

Supersedes the single-PR shape in `UPSTREAM_PR_PLAN.md` (that doc stays valid
for commit-message text, the pre-submit checklist and the decision log).

**Instruction received:** one PR with only the vendor API (code that must not be
modified and is merged as-is), then cmake, then CRC, then adf4030 and adf4382
as two separate PRs, then finally ad9088 (FSM + driver + project).

**Baseline:** `feature/FCE-112-add-ad9088-support` at `070dedbe10`, 7 commits
over `origin/main` `dc629751f2`, 445 files / +103435 / −156. The 7 commits are
path-disjoint except for the three splits called out in §2, so every branch
below is built by checking paths out of the existing branch — no `rebase -i`.

---

## 1. Mapping

| PR | Subject | Source | Files | Lines | Needs | Status |
|---|---|---|---|---|---|---|
| 1 | `drivers: rf-transceiver: ad9088: add Apollo API` | part of `86ebbefae1` | 405 | +94692 + 8 bin | — | **DONE** `abf0d124a8` |
| 2 | `cmake: add firmware blob embedding support` | `ddbf10e842` | 3 | +166 | — |
| 3 | `util: no_os_crc32: add CRC-32 support` | `d12a878e4b` | 4 | +124 | — |
| 4 | `drivers: frequency: adf4030: add JESD204 support` | `142bfbc772` + 1 line of `b0583c8b89` | 3 | +607/−48 | — |
| 5 | `drivers: frequency: adf4382: add phase alignment` | `51e51d051f` | 2+ | +501/−108 | — |
| 6 | `drivers: rf-transceiver: ad9088: add driver` + `projects: ad9088: add VCU118 project` | rest of `86ebbefae1`, 3 lines of `b0583c8b89`, `070dedbe10` | 32 | +7367 | 1,2,3,4,5 |

PRs 1–5 are mutually independent and all apply to `origin/main` today. Only
PR 6 is stacked, and it needs all five. 417 files of `86ebbefae1` = 393 vendor
text + 9 firmware binaries + 8 own-code files + 7 glue files; the split of
those 417 is exact and given per PR below.

---

## 2. Three things the naive split gets wrong

### 2.1 adf4030 does not compile without a jesd204.h enum (blocker)

`drivers/frequency/adf4030/adf4030.c:2349` registers
`[JESD204_OP_CLK_SYNC_STAGE4]`, and that enumerator is added by
`b0583c8b89 include: jesd204: add AD9088 FSM stages` — which the instruction
puts in the *last* PR. PR 4 would not build.

**Fix: split `b0583c8b89` along its users rather than shipping it whole.**

- `JESD204_OP_CLK_SYNC_STAGE4` → **PR 4**. Its only user in the whole series is
  `adf4030.c`; grep confirms the AD9088 driver never references it.
- `JESD204_OP_OPT_POST_SETUP_STAGE1/2/3` → **PR 6**. Used only by
  `ad9088_jesd204_fsm.c:1211,1215,1219`.

This is strictly better than keeping the 4 lines together: each PR then adds
exactly the enumerators it uses, which is what a reviewer asks for anyway.

It is safe to add enumerators piecemeal because the FSM dispatch is generic —
`jesd204/jesd204-fsm.c:21` walks `for (op = 0; op < __JESD204_MAX_OPS; op++)`
and indexes `dev_data->state_ops[op]`. No transition table needs a new row, and
existing drivers leave the slot NULL. Inserting `CLK_SYNC_STAGE4` renumbers the
later enumerators, which is a recompile-only change in a static build.

The two hunks land ~9 lines apart in the same enum, so PR 4 and PR 6 merge in
either order without a textual conflict.

### 2.2 The CI guards must travel with the vendor tree, not with the driver (blocker)

`86ebbefae1` adds four guard entries that exist *because of* the vendor tree.
If they stay with the driver in PR 6, CI on PR 1 runs the full style and
compile sweep over 393 vendor files and fails:

| File | Lines | Why it is PR 1 |
|---|---|---|
| `.astyleignore` | +4 | lists `ad9088/{public,private,adi_inc,adi_utils}/` |
| `.cppcheckignore` | +4 of 7 | same four paths |
| `doc/doxygen/CMakeLists.txt` | +4 | excludes the same four dirs from doxygen |
| `drivers/Makefile` | +1 | `-path ./rf-transceiver/ad9088 -prune` in `SKIPDIR`, the `-Wall -Wextra` driver sweep — same treatment as navassa/madura/koror/palma |

`.astyleignore`, the doxygen file and `drivers/Makefile` contain *only*
vendor-related additions, so those three can be checked out whole into PR 1.
`.cppcheckignore` is the one file that must be hand-split: its remaining 3
lines (the comment plus
`comparePointers:drivers/rf-transceiver/ad9088/ad9088_dt.c`) belong to PR 6,
because they suppress a finding in own code.

### 2.3 adf4382 changes the unit of an existing public API and leaves two callers behind

`51e51d051f` converts `adf4382_set_phase_adjust()` from picoseconds to
femtoseconds and touches only `adf4382.c`/`adf4382.h`. Two in-tree callers pass
a value that now means something 1000× different, and both still compile
silently:

- `drivers/frequency/adf4382/iio_adf4382.c:525` — `adf4382_set_phase_adjust(adf4382, val)`, an IIO attribute whose documented unit changes under it.
- `projects/adf4382/src/examples/basic/basic_example.c:67` — `adf4382_set_phase_adjust(dev, 1)`.

Inside a 7-commit AD9088 PR this was invisible. As a standalone
`drivers: frequency: adf4382` PR it is the first thing a frequency-domain
reviewer will find. **PR 5 must either update both callers or keep a
ps-taking wrapper.** Decide before pushing; it changes the file list and the
commit message.

---

## 3. Per-PR detail

Branch names are suggestions; PR 6 keeps the existing branch name so the
existing branch/PR identity survives.

### PR 1 — Apollo vendor API, merged as-is

Branch: `feature/FCE-112-ad9088-api`

Contents — 405 files, +94692 lines plus 8 binaries:

```
drivers/rf-transceiver/ad9088/adi_inc/        2 files
drivers/rf-transceiver/ad9088/adi_utils/      2
drivers/rf-transceiver/ad9088/public/       163   (inc 98, src 65)
drivers/rf-transceiver/ad9088/private/      226   (inc 222, src 4)
drivers/rf-transceiver/ad9088/firmware/       8   binary (device images only)
.astyleignore                                +4
.cppcheckignore                              +4   (vendor paths only)
doc/doxygen/CMakeLists.txt                   +4
drivers/Makefile                             +1
```

Build:

```bash
git checkout -b feature/FCE-112-ad9088-api origin/main
git checkout feature/FCE-112-add-ad9088-support -- \
    drivers/rf-transceiver/ad9088/adi_inc \
    drivers/rf-transceiver/ad9088/adi_utils \
    drivers/rf-transceiver/ad9088/public \
    drivers/rf-transceiver/ad9088/private \
    drivers/rf-transceiver/ad9088/firmware \
    .astyleignore doc/doxygen/CMakeLists.txt drivers/Makefile
git checkout feature/FCE-112-add-ad9088-support -- .cppcheckignore
# then hand-remove the 2 own-code lines + comment from .cppcheckignore
```

Nothing registers these files with Kconfig or CMake, so **PR 1 compiles
nothing**. That is intentional and must be stated in the PR body, or the first
review comment will be "dead code". Suggested wording: *"First of six. Vendor
Apollo API drop, imported unmodified; it is not referenced by any build until
the driver lands in PR #6. Please review for import fidelity, not style — the
tree is excluded from astyle, cppcheck, doxygen and the driver -Wall sweep by
the four guard entries in this PR."*

Verified: `grep -rl no_os_` across the four vendor directories returns **0**
files, so the "unmodified, merge as-is" claim holds — nothing in the tree was
adapted to no-OS.

CI gate: astyle / cppcheck / doxygen / `drivers/Makefile` sweep must all pass
purely on the guards. Run them before pushing; they are the only checks this PR
can fail.

**Firmware images — decided (2026-09-24).** The 8 signed device images
(`app_signed_encrypted_B/`, `app_signed_encrypted_prod_B/`) ship in PR 1 with
the rest of the vendor drop. The JESD profile
`204C_M4_L8_NP16_20p0_4x2.bin` is deliberately **excluded** — it moves to the
project folder in PR 6. See §3 PR 6.

### PR 2 — cmake firmware blob embedding

Branch: `feature/FCE-112-cmake-firmware-blob`. Cherry-pick `ddbf10e842` as-is.

```
CMakeLists.txt                          +1    include(FirmwareBlob)
cmake/FirmwareBlob.cmake              +144
drivers/platform/xilinx/toolchain.cmake +21   objcopy plumbing
```

Confirmed clean of the SDT contamination described in `UPSTREAM_PR_PLAN.md`
§1.1 — `toolchain.cmake` carries only the objcopy additions.

No in-tree user until PR 6, so say so in the body. CI gate: any Xilinx project
must still configure and build; `include(FirmwareBlob)` at top level runs for
every platform, so also configure one non-Xilinx project (stm32 or maxim) to
prove the include is inert there.

### PR 3 — util: no_os_crc32

Branch: `feature/FCE-112-crc32`. Cherry-pick `d12a878e4b` as-is.

```
include/no_os_crc.h    +1
include/no_os_crc32.h +44
util/CMakeLists.txt    +1
util/no_os_crc32.c    +78
```

Fourth CRC width beside the existing 8/16/24, same table-generation pattern.
The only consumer is `ad9088_dt.c` in PR 6. Self-contained and the easiest of
the six to get merged.

### PR 4 — adf4030 JESD204 support

Branch: `feature/FCE-112-adf4030-jesd204`.

```
drivers/frequency/adf4030/adf4030.c  +~560/−~40
drivers/frequency/adf4030/adf4030.h  +~46/−~8
include/jesd204.h                    +1     JESD204_OP_CLK_SYNC_STAGE4 only
```

Build: cherry-pick `142bfbc772`, then add the single enumerator line by hand
(do **not** cherry-pick `b0583c8b89`).

CI gate: `projects/adf4030` (stm32 sdp-ck1z, `basic.conf` and `iio.conf`) must
still build — it is the only in-tree consumer.

Commit-message note: the message already carries `Assisted-by:` before
`Signed-off-by:` and the duplicated `adf4030_set_alignment_threshold()`
paragraph noted in earlier review is gone (now appears once). It must gain a
line explaining the `include/jesd204.h` enumerator, since that file is now part
of this commit — e.g. *"Add the CLK_SYNC_STAGE4 enumerator the realignment
callback hooks; the FSM walks every op index, so existing drivers leaving the
slot NULL are unaffected."*

Behaviour change to keep in the message: `adf4030_set_alignment_threshold()`
no longer enables iteration (that splits into
`adf4030_set_alignment_threshold_en()`), so a threshold of 0 now programs 0
instead of disabling.

### PR 5 — adf4382 phase alignment

Branch: `feature/FCE-112-adf4382-phase`. Cherry-pick `51e51d051f`, **plus the
§2.3 caller fix**.

```
drivers/frequency/adf4382/adf4382.c  +349/−~85
drivers/frequency/adf4382/adf4382.h  +260/−~23
+ iio_adf4382.c and/or projects/adf4382/.../basic_example.c   (see §2.3)
```

Public API delta a reviewer will focus on:

```
-#define NS_TO_PS                 KHZ_PER_MHZ
+#define FS_PER_NS / NS_PER_MS / MS_PER_NS / NS_PER_FS / PERIOD_IN_DEG / PERIOD_IN_DEG_MS
-int adf4382_set_phase_adjust(struct adf4382_dev *dev, uint32_t phase_ps);
+int adf4382_set_phase_adjust(struct adf4382_dev *dev, uint32_t phase_fs);
+int adf4382_get_phase_adjust(struct adf4382_dev *dev, uint32_t *phase_fs);
+int adf4382_set_auto_align(struct adf4382_dev *dev, bool en);
```

CI gate: `projects/adf4382` (stm32 sdp-ck1z, `basic.conf` and `iio.conf`).

Still excluded by earlier decision, do not fold in: the ADF4382 VTUNE fix
(`ADF4382_VCO_CAL_VTUNE 640` truncating to 128 in the `uint8_t` argument of
`adf4382_spi_write()`). It is a live bug but it is not this PR's subject.

### PR 6 — ad9088 driver + VCU118 project

Branch: `feature/FCE-112-add-ad9088-support` (existing).

```
drivers/rf-transceiver/ad9088/ad9088.c                 1503
drivers/rf-transceiver/ad9088/ad9088.h                  344
drivers/rf-transceiver/ad9088/ad9088_dt.c               354
drivers/rf-transceiver/ad9088/ad9088_jesd204_fsm.c     1232
drivers/rf-transceiver/ad9088/ad9088_mcs.c              448
drivers/rf-transceiver/ad9088/ad9088_runtime.c          435
drivers/rf-transceiver/ad9088/ad9088_firmware.cmake      67
drivers/rf-transceiver/ad9088/README.rst                236
drivers/rf-transceiver/CMakeLists.txt                   +20
drivers/rf-transceiver/Kconfig                           +6
doc/sphinx/source/drivers/rf-transceiver/ad9088.rst      +1
.cppcheckignore                                          +3   (own-code lines)
include/jesd204.h                                        +3   (OPT_POST_SETUP_STAGE1/2/3)
+ all 19 files of 070dedbe10 (the project)
+ 204C_M4_L8_NP16_20p0_4x2.bin, relocated under projects/ad9088/
```

**The JESD profile moves out of the driver.** It was left out of PR 1 on
purpose: it is a board/link configuration, not a device artifact, and it is
selected per project variant. The move is cheaper than it looks:
`no_os_add_firmware_blob()` takes `SOURCE` and `SYMBOL_PATH` as *separate*
arguments, by design, "so the image can be swapped"
(`cmake/FirmwareBlob.cmake:37`). objcopy runs from a staging root and is handed
`SYMBOL_PATH` verbatim, so the exported `_binary_..._start/_end` names follow
`SYMBOL_PATH` and not the file's location on disk. Change only `SOURCE` in
`ad9088_firmware.cmake` (and the `CONFIG_AD9088_PROFILE` selection) to a path
under `projects/ad9088/`, leave `SYMBOL_PATH` alone, and the `extern` symbol
names in the driver need no change at all.

Two commits, in this order: driver, then project. Rebase onto whatever of
PRs 1–5 has merged; while they are still open, base the branch on the merge of
all five so it builds.

Do not re-litigate, from the earlier review: `set(NOOS_XILINX_SDT ON)` is
present at `projects/ad9088/CMakeLists.txt:48` and must stay **before**
`config_platform_sdk()`; both sphinx stubs exist; the driver commit message no
longer claims a `private/inc/bitfields/README`.

CI gate: full `basic_example` and `dma_example` builds for vcu118 from scratch,
astyle over own code, and — because five PRs will have moved underneath it —
a hardware re-run before the final push.

---

## 4. Carried over, still open

1. **astyle over own code.** Earlier review found ~10 files / ~108 lines
   reformatted, with `adf4030.c` and `adf4382.c` passing on `main` and failing
   at branch HEAD — i.e. PRs 4 and 5 introduce them. The in-tree astyle build
   (`build/astyle/build/gcc/bin/astyle`) is **gone** as of this writing, so
   this could not be re-measured; rebuild it and run per-branch before pushing.
2. **Two `-Wformat` warnings in own code**, both still present and both PR 6:
   `ad9088.c:1284` (`%X`) and `ad9088_dt.c:264` (`%u`) — `uint32_t` is
   `long unsigned int` on MicroBlaze.
3. **Per-commit buildability** was never verified for the old series. The
   split makes this cheap: each of PRs 1–5 is one or two commits, so build each
   branch head once.
4. Commit messages must not mention Linux, the kernel, device trees or sysfs.

## 4b. Deferred on PR 1 (`abf0d124a8`, by decision 2026-09-24)

Both reviewed, both judged not worth an amend now. Fold into a later push of
the branch if it gets rewritten for any other reason:

1. `public/inc/adi_apollo_jtx_types.h` is committed mode `100755`. Inherited
   from the original branch, and the only non-`100644` file in the tree —
   `navassa`, `madura`, `koror` and `palma` have none between them.
   `git update-index --chmod=-x <path>`.
2. The commit message no longer explains the four CI-guard files it touches
   (`.astyleignore`, `.cppcheckignore`, `doc/doxygen/CMakeLists.txt`,
   `drivers/Makefile`).

## 5. Decisions needed before any branch is cut

1. ~~Firmware `.bin` images in PR 1 or PR 6?~~ **Closed 2026-09-24:** device
   images in PR 1, JESD profile relocated to the project in PR 6.
2. PR 5: update the two ps callers, or keep a ps-taking compatibility wrapper?
3. Open all six at once as a declared stack, or push PRs 1–5 first and open
   PR 6 only once they have merged?

---

## 6. PR descriptions

Follow `.github/PULL_REQUEST_TEMPLATE.md` — keep its three headings
(`## Pull Request Description`, `## PR Type`, `## PR Checklist`) and its
checkbox lists exactly as the template writes them, and add the `#### Testing`
and `#### Related` subsections the AD9088 series already used. Do not delete
checklist entries that do not apply; tick what is true and annotate the rest on
the same line, so a reviewer can see the item was considered rather than
dropped.

Every PR in the series carries the same **Series** block, so a reviewer landing
on any one of the six can see where it sits and what it waits on. Update the
bold marker to whichever PR is being opened, and add the PR numbers once they
exist.

```markdown
**Series** — each of these applies to `main` on its own; only the last is stacked:

1. drivers: rf-transceiver: ad9088 — vendor Apollo API
2. cmake — firmware blob embedding
3. util — no_os_crc32
4. drivers: frequency: adf4030 — JESD204 support
5. drivers: frequency: adf4382 — phase alignment
6. drivers: rf-transceiver: ad9088 — driver + VCU118 project (needs 1-5)
```

The **Testing** block is shared too, because the hardware result belongs to the
series rather than to any one PR. State plainly what the individual PR builds —
for PRs 1, 2 and 3 that is "nothing in this PR is compiled by it", which is the
honest answer and is better said outright than implied:

```markdown
#### Testing

Vitis 2025.1, MicroBlaze, AD9084-EVAL on VCU118.

<what this PR alone builds / does not build>

The series as a whole: both examples build clean from scratch with the SDT ABI
active, astyle passes over every file CI checks, and sphinx-html builds with
both README.rst rendering through their stubs. On hardware, the JESD204C links
reach LINK_RUNNING through the full FSM, subclass 1 runs with the ADF4030 as
SYSREF provider (SYSREF phase locked and captured on both AXI cores),
multi-chip sync calibration passes init-cal validation, and the DMA example's
cabled loopback recovers the transmitted tone.
```

`#### Related` carries `FCE-112` on all six.

### PR 1 — as opened

```markdown
## Pull Request Description

First of six PRs adding support for the AD9088 (Apollo) RF transceiver. This
one imports the vendor Apollo device API the driver is built on, so the driver
itself can be reviewed on its own later.

The tree is imported unmodified and is meant to be merged that way: no symbol
from this repository is referenced anywhere in it, and none should be added to
it. Please review it for import fidelity rather than style.

Contents, 405 files:

- `drivers/rf-transceiver/ad9088/public/` and `private/` — the device API and
  its register bitfields. `private/inc/bitfields` ships one set per silicon
  revision, `a0` and `b0`; the two hold 108 headers each and share 106 of the
  names, which are included unqualified, so the consumer puts exactly one of
  them on the include path. That selection lands with the driver.
- `adi_inc/` — the common ADI API types; `adi_utils/` — helpers both rely on.
- `firmware/` — the signed device images the driver loads at start-up.
- `.astyleignore`, `.cppcheckignore`, `doc/doxygen/CMakeLists.txt` and
  `drivers/Makefile` — exclude the tree from the checks that only apply to code
  written for this repository, the same way the other vendor API trees under
  `drivers/` are excluded.

Nothing in the build references any of this yet: there is no Kconfig symbol and
no CMake entry until the driver lands in PR 6. That is deliberate, and it is
what keeps this PR reviewable as an import.

**Series** — each of these applies to `main` on its own; only the last is stacked:

1. **this PR** — vendor Apollo API
2. cmake — firmware blob embedding
3. util — no_os_crc32
4. drivers: frequency: adf4030 — JESD204 support
5. drivers: frequency: adf4382 — phase alignment
6. drivers: rf-transceiver: ad9088 — driver + VCU118 project (needs 1-5)

#### Testing

Vitis 2025.1, MicroBlaze, AD9084-EVAL on VCU118.

No code in this PR is compiled by it — the tree has no build entry until PR 6,
so the only checks it can exercise are astyle, cppcheck, doxygen and the driver
compile sweep, all of which are satisfied by the four exclusion entries above.
The tree is built and exercised as part of PR 6.

The series as a whole: both examples build clean from scratch with the SDT ABI
active, astyle passes over every file CI checks, and sphinx-html builds with
both README.rst rendering through their stubs. On hardware, the JESD204C links
reach LINK_RUNNING through the full FSM, subclass 1 runs with the ADF4030 as
SYSREF provider (SYSREF phase locked and captured on both AXI cores),
multi-chip sync calibration passes init-cal validation, and the DMA example's
cabled loopback recovers the transmitted tone.

#### Related

FCE-112

## PR Type
- [ ] Bug fix (change that fixes an issue)
- [x] New feature (change that adds new functionality)
- [ ] Breaking change (has dependencies in other repos or will cause CI to fail)

## PR Checklist
- [x] I have followed the [Coding style guidelines](http://analogdevicesinc.github.io/no-OS/drivers_guide.html#coding-style) — for the files written for this repository; the imported tree is excluded from astyle by design
- [x] I have complied with the [Submission Checklist](http://analogdevicesinc.github.io/no-OS/contributing.html#submission-checklist)
- [x] I have performed a self-review of the changes
- [ ] I have commented my code, at least hard-to-understand parts — not applicable, nothing here is written for this repository
- [ ] I have build all projects affected by the changes in this PR — no project is affected; nothing references these files yet
- [ ] I have tested in hardware affected projects, at the relevant boards — not testable in isolation; the tree is exercised on VCU118 as part of PR 6
- [x] I have signed off all commits from this PR
- [ ] I have updated the documentation (wiki pages, ReadMe etc), if applies — the driver README.rst and its sphinx stub land with the driver in PR 6
```

### Per-PR notes on the checklist

- **PRs 2 and 3** (cmake, crc32): "build all projects affected" is the one to be
  careful with. PR 2 adds a top-level `include(FirmwareBlob)` that runs for
  every platform, so configure at least one Xilinx and one non-Xilinx project
  and say which. PR 3 has no consumer until PR 6; say so.
- **PRs 4 and 5** (adf4030, adf4382): both have an in-tree project
  (`projects/adf4030`, `projects/adf4382`, stm32 sdp-ck1z, `basic.conf` and
  `iio.conf`). Build both variants and name them, and state whether hardware
  was available — these are existing drivers, so a behaviour change lands on
  users who are not the AD9088. PR 5 must also state what happened to the
  picosecond callers (§2.3).
- **PR 6**: the only one that can tick hardware honestly. It is also the only
  one that updates documentation.

---

## 7. Final state (2026-09-29)

All six commits exist, each a single commit on an `origin/main` ancestor. The
branches were renamed onto per-PR tickets late in the process; FCE-1058 is
unassigned and PR 6 still carries the old FCE-112.

| PR | Branch | sha | Files |
|---|---|---|---|
| 1 | `feature/FCE-1057-ad9088-api` | `aae166352` | 406 |
| 2 | `feature/FCE-1054-cmake-firmware-blob` | `b2b10608c` | 3 |
| 3 | `feature/FCE-1055-crc32` | `b075a6269` | 4 |
| 4 | `feature/FCE-1056-adf4030-jesd204` | `e9865a73a` | 3 |
| 5 | `feature/FCE-1059-adf4382-phase` | `0ccd5fbc0` | 4 |
| 6 | `feature/FCE-112-add-ad9088-support` | `86acf5216` | 33 |

Shas move on every amend; re-read them rather than trusting this table. Each
`#### Related` should carry that PR's own ticket — the drafted descriptions
predate the rename and all say FCE-112.

**Gates run.** astyle 3.1, built and invoked the way `.github/scripts/astyle.sh`
does (`--options=.github/config/astyle_config`), over every `.c`/`.h` in each
commit after `.astyleignore` filtering: 0/3/3/4/17 files checked, **none
reformatted**. This retires the older finding that `adf4030.c` and `adf4382.c`
fail astyle at HEAD — measured with the pinned version against these commits,
they do not. Commit-message format (50/72, `Assisted-by` before
`Signed-off-by`), file sets and base commits verified for all six. No added
line in PRs 2-6 mentions Linux, the kernel, device trees or sysfs; PR 1's two
`#include <linux/kernel.h>` hits are vendor code inside `#ifdef __KERNEL__`
and must stay.

**PR 1 gained a licence** late: `license/2026-05-25-ad9084-sdk-2.1.0-ga-
click-thru-SLA.pdf`, 19.7 KiB. It is the first `.pdf` tracked in the repo —
other vendor trees ship plain-text `LICENSE`/`LICENSE.txt`. No tooling impact.

**Deferred by explicit decision** (2026-09-29, "no other changes"), each
reviewed and judged not worth an amend:

1. `public/inc/adi_apollo_jtx_types.h` is mode `100755`, the only non-`100644`
   file in the vendor tree.
2. PR 1's message does not explain its four CI-guard files, nor the licence
   unless the §7 amendment was applied.
3. PR 6 is one combined commit; upstream convention is a driver commit plus a
   project commit, and the paths are already disjoint if that is ever wanted.
4. PR 5's commit message names the AD9088 — the only forward reference left in
   PRs 1-5.

### 7.1 PR descriptions

One file per PR in `projects/ad9088/dev_doc/PR_descriptions/`, named after the
branch. They follow `.github/PULL_REQUEST_TEMPLATE.md` plus `#### Testing` and
`#### Related`, and each carries a Series block naming the other five.

Reviewed 2026-09-29 against the commits; part numbers, file count and the
paste truncations were corrected. Known and left as-is:

- **`#### Related` says FCE-112 in all six** — correct only for PR 6. Each
  should carry its own ticket.
- **FCE-1057 lists 406 files but the bullets account for 405** — the licence
  PDF is not in the contents list.
- **FCE-112 no longer introduces** the three `include/jesd204.h` enums, the
  `b0` include-path selection, the cppcheck suppression or the SDT opt-in,
  though its Testing section still refers to "the new post-setup stages". A
  reviewer sees a shared-header change with no explanation.
- **FCE-112's Testing lost its build/astyle/sphinx paragraph**, so the build
  and documentation checkboxes are ticked with nothing behind them.
- **FCE-1054 has no `#### Testing`** and **FCE-1055's says only that the last
  PR exercises it**, while both tick the build checkbox.

PRs were not yet opened as of 2026-09-29; the user opens all six.
