*******************
no-OS license rules
*******************

no-OS is, by default, permissively licensed under **BSD-3-Clause**.
Individual files may be under different terms, always stated in the file's
own header. This page explains how licenses are declared and organized so
that the license of any file is clear and, where possible, machine
readable.

The top-level ``LICENSE`` file is a short pointer to this default and to
this page. The license of any individual file is stated by its own SPDX
tag or header; that tag is authoritative.

Declaring a license in a source file
====================================

Every source file declares its license with an SPDX identifier in its
header comment, in a form appropriate to the file type. For C sources and
headers::

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
  ``Apache-2.0`` and ``MIT`` (the latter used by some bundled libraries
  and vendor SDKs).

``LICENSES/proprietary/``
  Non-permissive ADI vendor device-API licenses, each confined to its
  driver subtree (detailed below): ``LicenseRef-ADI-API-License``,
  ``LicenseRef-ADI-ADRV9025-SLA``, ``LicenseRef-ADI-ADRV9001-SLA`` and
  ``LicenseRef-ADI-AD937X-Firmware-CTSLA``.

Each license file begins with a ``Valid-License-Identifier:`` line (the
SPDX or repository-local ``LicenseRef-`` identifier), an optional
``SPDX-URL:`` for standard licenses, a ``Usage-Guide:`` describing when
the license may be used, and the full ``License-Text:``.

Repository-local ``LicenseRef-`` identifiers
============================================

Some licenses used by no-OS are not standard SPDX licenses, so they use
REUSE-style repository-local identifiers:

- ``LicenseRef-ADI-API-License`` — the ADI CTSLA vendor API license.
- ``LicenseRef-ADI-ADRV9025-SLA`` — the ADI "Software License Agreement"
  ``20180813-ADI-N3PT-CTSLA`` carried by the ADRV9025 (madura) vendor
  API files; the same non-permissive ADI "API license" family. It
  expressly forbids subjecting the software to source-disclosure terms.
- ``LicenseRef-ADI-AD937X-Firmware-CTSLA`` — the ADI "Software License
  Agreement" ``20180301-AD937X-CTSLA`` carried by the Talise (AD937x)
  ARM and stream firmware binaries under
  ``drivers/rf-transceiver/talise/firmware``; the same non-permissive
  ADI "API license" family. It covers firmware object code and
  expressly forbids subjecting the software to any "Excluded License"
  (source-disclosure terms).
- ``LicenseRef-ADI-ADRV9001-SLA`` — the ADI "Software License
  Agreement" ``20190814-APIGUIHDLFWNAV-CTSLA`` carried by the ADRV9001
  SDK (navassa) vendor API files under
  ``drivers/rf-transceiver/navassa``; the same non-permissive ADI
  "API license" family.

They are **non-permissive** and must not be used for new files.

Acceptable licenses for new contributions
=========================================

New contributions must use the permissive ``BSD-3-Clause`` license. See
the "Licensing of contributions" section of :doc:`contributing` for the
full rule, including the single exception for vendor-supplied device APIs
that cannot be relicensed.

Bundled third-party libraries
=============================

Libraries bundled under ``libraries/`` are third-party code. Each carries
its own ``LICENSE`` / ``COPYING`` file in its directory, which governs
that library; they are not re-described under ``LICENSES/``. A library is
only built when a project selects it, and none is under the no-OS
BSD-3-Clause default.

.. list-table::
   :header-rows: 1
   :widths: 55 45

   * - Library
     - License
   * - ``libraries/free_rtos/``
     - ``MIT``
   * - ``libraries/azure/``
     - ``MIT``
   * - ``libraries/lvgl/``
     - ``MIT``
   * - ``libraries/mbedtls/``
     - ``Apache-2.0``
   * - ``libraries/lwip/``
     - ``BSD``
   * - ``libraries/fatfs/``
     - FatFs (BSD-like)
   * - ``libraries/pico-sdk/``
     - ``BSD-3-Clause``
   * - ``libraries/mqtt/``
     - ``EPL-1.0`` / ``EDL-1.0``
   * - ``libraries/precision-converters-library/``
     - see its ``LICENSE``
   * - ``libraries/esh/``, ``libraries/tmc/``
     - see their ``LICENSE``
