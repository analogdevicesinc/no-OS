---
name: no-os-driver-readme
description: Write or update a no-OS device driver README.rst following the repo's standard driver-documentation structure. Use when creating documentation for a driver under drivers/<category>/<part>/, updating an existing driver README, or wiring a driver doc into the Sphinx toctree.
---

# no-OS Driver README

Generate or update `drivers/<category>/<part>/README.rst` files for the no-OS
repository so they match the house driver-documentation structure. These READMEs
are rendered on the docs site via a per-driver `.. include::` stub and the
category glob toctree.

## When to use

- Creating a `README.rst` for a driver that lacks one.
- Rewriting an old driver README to the current structure.
- Wiring a new driver doc into the Sphinx build (stub + toctree check).

## Inputs to gather first

Before writing, collect this. Ask the user only for what you cannot determine
from the repo or the device data sheet:

1. **Driver directory** — `drivers/<category>/<part>/`. Confirm it exists and
   note the category (e.g. `temperature`, `adc-dac`, `power`, `digital-io`).
2. **Device(s)** — part number(s) the driver supports and the ADI product
   page(s). Multiple parts share one README when one driver covers them.
3. **Overview facts** — resolution, channels, interface (SPI/I2C), supply
   rails, key on-chip features, typical applications. Pull from the data sheet.
4. **Driver API surface** — inspect the driver's public header
   (`<part>.h`) to list the functional groups and the `<part>_*` functions in
   each (init, register access, measurement/conversion, channel config, GPIO,
   diagnostics, ...). Group the docs the same way.
5. **Operation modes** — if the device is software-configurable into distinct
   modes, capture them for the optional Operation Modes grid table.
6. **IIO layer** — check for `iio_<part>.c`. If present, document the IIO
   attributes (channel / global / debug) and the IIO init example; if absent,
   drop the IIO sections entirely.

Do not invent specs. If a value is unknown, leave a clearly-marked `<...>`
placeholder rather than guessing.

## How to write it

1. Start from the template: `doc/driver_readme_template.rst` (at the repo
   root). Copy its structure verbatim, then fill every `<...>` placeholder and
   delete the `..` guidance comment lines.
2. Keep the exact section order and RST heading underline convention:
   - `=` title (level 1), `-` section (level 2), `~` subsection (level 3),
     `^` sub-sub (level 4). Underlines must be **at least** as long as the text.
3. Standard section order (matching existing driver READMEs):
   - `<PART> no-OS driver` title, then `.. no-os-doxygen::`
   - **Supported Devices** — one `:adi:` role link per part.
   - **Overview** — a few paragraphs from the data sheet.
   - **Applications** — bullet list; optional **Operation Modes** grid table.
   - **<PART> Device Configuration** — `Driver Initialization` subsection plus
     one `~~~` subsection per functional group of the API.
   - **<PART> Driver Initialization Example** — a `.. code-block:: bash` snippet.
   - IIO sections (only if an IIO layer exists): **no-OS IIO support**,
     **IIO Device Configuration** (attribute subsections), **IIO Driver
     Initialization Example**.
4. Link products with the **`:adi:`** role (e.g. `` :adi:`MAX31827` ``), the
   current convention, rather than a raw analog.com URL.

## Wiring into the Sphinx build

- Ensure a stub exists at
  `doc/sphinx/source/drivers/<category>/<part>.rst` containing only:

  ```rst
  .. include:: ../../../../../drivers/<category>/<part>/README.rst
  ```

- The category's glob toctree in `doc/sphinx/source/drivers_doc.rst`
  (`drivers/<category>/*`) then picks the stub up automatically — no manual
  toctree edit is needed as long as the category block already exists. If the
  category is new, add a section with a glob toctree for it.

## After writing

- Verify heading underlines are long enough (short underlines break the RST build).
- Verify each documented `<part>_*` function actually exists in the driver header.
- Verify the include stub exists and points at the right README path.
- Do not run a git commit unless the user asks.
