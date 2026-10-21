*******************
no-OS license rules
*******************

no-OS is, by default, permissively licensed under **BSD-3-Clause**.
Individual files may be under different terms, always stated in the file's
own header. This page explains how licenses are declared and organized so
that the license of any file is clear and, where possible, machine
readable.

The top-level ``COPYING`` file is a short pointer to this default and to
this page. The license of any individual file is stated by its own SPDX
tag or header; that tag is authoritative.

Declaring a license in a source file
====================================

Every source file should declare its license with an SPDX identifier in
its header comment, in a form appropriate to the file type (some older
files, mostly build files and scripts, are still being converted). For C
sources and headers::

    /* SPDX-License-Identifier: BSD-3-Clause */

For scripts and other ``#``-comment files::

    # SPDX-License-Identifier: BSD-3-Clause

The SPDX tag is authoritative for that file, and is required for new
no-OS code because it is precise and machine parsable. Analog Devices'
own no-OS sources carry this tag (usually alongside a copyright line)
rather than the full BSD boilerplate.

The ``LICENSES/`` directory
===========================

The full text of every license used by Analog Devices' own no-OS code is
collected under ``LICENSES/``, one file per license, grouped by role:

``LICENSES/preferred/``
  ``BSD-3-Clause`` (the default and the only license for new no-OS code).

``LICENSES/permissive/``
  Permissive licenses that are allowed but are not the default:

  - ``Apache-2.0`` — the ADRV903x (palma) and ADRV904x (koror) vendor
    APIs, the generated firmware/profile files of the adrv903x and
    adrv904x projects, and the Maxim SDK lwIP port header
    ``libraries/lwip-glue/arch/cc.h``.
  - ``MIT`` — third-party code kept under its original license, e.g. the
    Xilinx-derived ``drivers/platform/xilinx/xilinx_uart.c``, the LVGL
    ``lv_conf.h`` files, the jsmn parser in the navassa tree, the Azure
    IoT sample code in eval-ade9430 and the ad5293 driver.
  - ``OFL-1.1`` — font data generated from OFL-licensed fonts.

``LICENSES/proprietary/``
  Non-permissive ADI vendor device-API licenses, each confined to its
  driver subtree and the matching example project (detailed below):
  ``LicenseRef-ADI-API-License``,
  ``LicenseRef-ADI-ADRV9025-SLA``, ``LicenseRef-ADI-ADRV9001-SLA``,
  ``LicenseRef-ADI-AD9084-SLA`` and
  ``LicenseRef-ADI-AD937X-Firmware-CTSLA``.

Each license file begins with a ``Valid-License-Identifier:`` line (the
SPDX or repository-local ``LicenseRef-`` identifier), an optional
``SPDX-URL:`` for standard licenses, a ``Usage-Guide:`` describing when
the license may be used, and the full ``License-Text:``.

Repository-local ``LicenseRef-`` identifiers
============================================

Some licenses used by no-OS are not standard SPDX licenses, so they use
REUSE-style repository-local identifiers:

- ``LicenseRef-ADI-API-License`` — the ADI "Source Code Software License
  Agreement" ``20130524-CISC-CTSLA`` carried by the talise, ad9083,
  ad9208 and ad917x vendor API files and the ad9371 and adrv9009 project
  trees.
- ``LicenseRef-ADI-ADRV9025-SLA`` — the ADI "Software License Agreement"
  ``20180813-ADI-N3PT-CTSLA`` carried by the ADRV9025 (madura) vendor
  API files and the generated profiles and platform glue of the
  adrv902x project; the same non-permissive ADI "API license" family. It
  expressly forbids subjecting the software to source-disclosure terms.
- ``LicenseRef-ADI-AD937X-Firmware-CTSLA`` — the ADI "Software License
  Agreement" ``20180301-AD937X-CTSLA`` carried by the Talise (AD937x)
  ARM and stream firmware binaries under
  ``drivers/rf-transceiver/talise/firmware`` and the AD9371 (Mykonos)
  firmware header in the ad9371 project; the same non-permissive
  ADI "API license" family. It covers firmware object code and
  expressly forbids subjecting the software to any "Excluded License"
  (source-disclosure terms).
- ``LicenseRef-ADI-ADRV9001-SLA`` — the ADI "Software License
  Agreement" ``20190814-APIGUIHDLFWNAV-CTSLA`` carried by the ADRV9001
  SDK (navassa) vendor API files under
  ``drivers/rf-transceiver/navassa`` and the generated profiles of the
  adrv9001 project; the same non-permissive ADI "API license" family.
- ``LicenseRef-ADI-AD9084-SLA`` — the ADI "Software License
  Agreement" ``2026-05-25-AD9084-SDK-2.1.0-GA-SLA`` carried by the
  AD9084/AD9088 SDK (Apollo) vendor API files and firmware images under
  ``drivers/rf-transceiver/ad9088``; the same non-permissive ADI
  "API license" family. It incorporates by reference the terms published
  at https://www.analog.com/en/support/software-license-agreement.html.

They are **non-permissive** and must not be used for new files.

Acceptable licenses for new contributions
=========================================

New contributions must use the permissive ``BSD-3-Clause`` license. See
the "Licensing of contributions" section of :doc:`contributing` for the
full rule, including the exceptions for vendor-supplied device APIs that
cannot be relicensed and for third-party code kept under its original
permissive license.

Third-party libraries
=====================

Third-party libraries are not stored in the repository. They are fetched
at configure time (pinned via Kconfig) into ``libraries/<repo-name>/``,
and each carries its own ``LICENSE`` / ``COPYING`` file, which governs
that library; they are not re-described under ``LICENSES/``. A library is
only fetched and built when a project selects it.

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - Library
     - License
   * - ``libraries/FreeRTOS-Kernel/``
     - ``MIT``
   * - ``libraries/azure-sdk-for-c/``
     - ``MIT``
   * - ``libraries/lvgl/``
     - ``MIT``
   * - ``libraries/TMC-API/``
     - ``MIT``
   * - ``libraries/mbedtls/``
     - ``Apache-2.0``
   * - ``libraries/esh/``
     - ``Apache-2.0``
   * - ``libraries/precision-converters-library/``
     - ``Apache-2.0``
   * - ``libraries/lwip/``
     - ``BSD-3-Clause``
   * - ``libraries/pico-sdk/``
     - ``BSD-3-Clause``
   * - ``libraries/paho.mqtt.embedded-c/``
     - ``EPL-1.0`` / ``EDL-1.0``
   * - ``libraries/ftd2xx/``
     - FTDI D2XX driver license (proprietary, downloaded from FTDI)

Two kinds of code are tracked under ``libraries/``:

- ``libraries/fatfs/`` is a bundled copy of FatFs, under the FatFs
  license (BSD-like) in ``libraries/fatfs/LICENSE.txt``.
- The no-OS glue code for the fetched libraries —
  ``libraries/esh-glue/``, ``libraries/lwip-glue/``, ``libraries/mqtt/``
  and ``libraries/fatfs/adi_diskio.c``. Each file carries its own SPDX
  tag or license header; ``lwip-glue/configs/`` holds lwIP configuration
  headers under lwIP's ``BSD-3-Clause`` license.
