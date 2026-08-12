---
name: no-os-source-header
description: Add the standard no-OS license/doxygen header to a new C source or header file, or fix a non-conforming one. Use when creating a new .c/.h/.cpp/.hpp/.cc file under no-OS, or when a file is missing the SPDX tag, doxygen @file banner, copyright line, or (for headers) include guards.
---

# no-OS Source File Header

Give every new no-OS C-family file the house header: a doxygen `@file` banner,
a current-year ADI copyright line, and a single
`SPDX-License-Identifier: BSD-3-Clause` tag. Headers additionally get include
guards. This matches the policy in `doc/sphinx/source/license_rules.rst` and the
example in `doc/sphinx/source/drivers_guide.rst`.

## When to use

- Creating a new `.c/.h/.cpp/.hpp/.cc` file under no-OS.
- A file is missing or has a malformed license header (no SPDX tag, full-text
  BSD boilerplate instead of the SPDX tag, missing `@file` banner, or — for a
  header — missing include guards).

## Do NOT use for

- Files under `libraries/**` (bundled third-party; keep their own headers).
- The vendor trees: `drivers/rf-transceiver/**`,
  `projects/adrv*/**`, `projects/ad9371/**`, and the proprietary vendor-API
  `*_api/` subtrees. These keep their existing vendor/proprietary headers.
- Files under a non-BSD-3 license (e.g. BSD-2-Clause `drivers/adc/ltc2312/`);
  do not overwrite their SPDX tag with `BSD-3-Clause`.

## Inputs to gather first

Determine from the repo where possible; ask only for what you cannot infer:

1. **File path and type** — `.h`/`.hpp` get include guards; `.c`/`.cpp`/`.cc`
   do not.
2. **Part / module name** — for the `@brief` line and (headers) the include
   guard macro. For a driver, this is the part number (e.g. `AD405X`).
3. **Author name and ADI email** — for the `@author` line. Use the committer's
   git `user.name`/`user.email` if the user does not specify. One `@author`
   line per author; add more lines for multiple authors.
4. **Copyright year** — the current year for a brand-new file. Use a range
   (e.g. `2024~2026`) only when editing a file that already had an earlier
   year.

## How to write it

1. Start from the template pair at the repo root:
   `doc/source_header_template.c` and `doc/source_header_template.h`.
2. Replace every `<...>` placeholder:
   - `<part>` — lowercase file base name (e.g. `ad405x`).
   - `<PART>` — uppercase part/module name (e.g. `AD405X`); also forms the
     include-guard macro `__<PART>_H__`.
   - `<Your Name>` / `<your.name>` — author and ADI email.
   - Copyright year — current year for new files.
3. Keep the banner byte-exact: the opening `/**...//**` doxygen marker, the
   `****` separator line, and the closing `***/` fence must match the template
   (astyle and the docs build rely on this shape).
4. **Exactly one** `SPDX-License-Identifier: BSD-3-Clause` line, on its own
   comment line after the copyright line(s). Never emit the full-text BSD
   boilerplate — the SPDX tag replaces it.
5. For headers, wrap the body in `#ifndef __<PART>_H__ / #define __<PART>_H__`
   ... `#endif /* __<PART>_H__ */` include guards.
6. Adjust the `#include` lines in the `.c` template to what the file actually
   needs; they are only a starting point.

## After writing

- Verify exactly one SPDX tag and zero `Redistribution and use` /
  `POSSIBILITY OF SUCH DAMAGE` lines remain.
- Verify the include-guard macro matches the file (`__<PART>_H__`) and is
  closed.
- Run astyle if the file has real code:
  `astyle --style=linux --indent=force-tab=8 --max-code-length=80 --suffix=none --pad-oper --pad-header --unpad-paren --pad-comma <file>`
- Do not run a git commit unless the user asks. When committing, follow the
  **no-os-contribute** skill for the required trailers.

See `doc/sphinx/source/license_rules.rst` for the full license policy and
`doc/sphinx/source/drivers_guide.rst` for the rendered header example.
