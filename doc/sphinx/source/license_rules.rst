*******************
no-OS license rules
*******************

no-OS is, by default, permissively licensed under **BSD-3-Clause**.
Individual files may be under different terms, always stated in the file's
own header. This page explains how licenses are declared and organized so
that the license of any file is clear and, where possible, machine
readable.

For a human-readable map of which directory trees use which license, see
the top-level ``LICENSE`` file. For a short pointer to the default and to
this directory, see the top-level ``COPYING`` file.

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

Full-text license headers remain only in bundled third-party code under
``libraries/``, in the vendor-supplied device-API subtrees that cannot be
relicensed, and in a small number of third-party-derived drivers under a
different license (for example the BSD-2-Clause ``drivers/adc/ltc2312/``);
there the per-file header is the authoritative statement of the file's
license.

The ``LICENSES/`` directory
===========================

The full text of every license used by Analog Devices' own no-OS code is
collected under ``LICENSES/``, one file per license, grouped by role:

``LICENSES/preferred/``
  ``BSD-3-Clause`` (the default and the only license for new no-OS code)
  and ``MIT`` (a preferred permissive license, used by some bundled
  libraries).

``LICENSES/permissive/``
  Permissive licenses that are allowed but are not the default:
  ``Apache-2.0`` and ``BSD-2-Clause``. Note that ``Apache-2.0`` is not
  compatible with ``GPL-2.0-only``.

``LICENSES/copyleft/``
  Copyleft licenses present in the tree: ``GPL-2.0``. Not for new
  general-purpose code.

``LICENSES/proprietary/``
  Vendor device API licenses: ``LicenseRef-ADI-API-License`` (the ADI
  "Source Code Software License Agreement" / CTSLA) that governs the
  RF-transceiver device API code, and the closely related ADI "Software
  License Agreement" carried by a few converter device-API files (for
  example ``drivers/adc/ad9208/ad9208_api/`` and
  ``drivers/dac/ad917x/ad917x_api/``). Non-permissive; confined to their
  vendor driver subtrees.

Each license file begins with a ``Valid-License-Identifier:`` line (the
SPDX or repository-local ``LicenseRef-`` identifier), an optional
``SPDX-URL:`` for standard licenses, a ``Usage-Guide:`` describing when
the license may be used, and the full ``License-Text:``.

Repository-local ``LicenseRef-`` identifiers
============================================

One license used by no-OS is not a standard SPDX license, so it uses a
REUSE-style repository-local identifier:

- ``LicenseRef-ADI-API-License`` — the ADI CTSLA vendor API license.

It is **non-permissive** and must not be used for new files.

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
that library; they are not re-described under ``LICENSES/``.
