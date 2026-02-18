****************************************
Lattice Propel (RISC-V RX) Build Guide
****************************************

RISC-V RX overview
------------------
The Lattice RISC-V RX soft IP is a 32-bit RISC-V core which supports AXI
interfaces. It bundles a PLIC (Platform Level Interrupt Controller),
a CLINT (Core Local Interrupter), and a watchdog, and supports the RV32IMACF
instruction set. The core offers three privilege modes
(Machine, Supervisor, User), multiple processor modes (Advanced, Balanced, Lite),
and includes instruction/data caches plus a JTAG (IEEE 1149.1) debug interface.
Because most of our IPs available in the `HDL repository <https://github.com/analogdevicesinc/hdl>`_,
use AXI-Lite control interface and our DMA also uses the AXI interface,
this is the RISC-V variant we currently support on Lattice FPGAs. See the vendor
page for details:
`RISC-V RX CPU IP | Lattice Semiconductor (latticesemi.com)
<https://www.latticesemi.com/products/designsoftwareandip/intellectualproperty/ipcore/ipcores04/risc-v-rx-cpu>`_


Scope
=====
Bare-minimum steps to build and run a Lattice RISC-V RX design. Tested with the
build tools provided in Propel SDK 2024.2 and the RISCV-RX
``latticesemi.com:ip:riscv_rtos:2.5.0`` core used by ``projects/ad738x_fmcz``.

Install & Inputs
================
1. Install the `Propel SDK <https://www.latticesemi.com/Products/DesignSoftwareAndIP/FPGAandLDS/LatticePropel>`_
   (includes GCC, srec_cat, OpenOCD).
2. Export a hardware package from your HDL project (``sge`` directory or
   ``sge.zip``). Drop it next to the project ``Makefile``.
3. *(Optional but recommended)* Generate the FPGA bitstream (``*.bit``)
   separately via Propel Builder/Radiant from the matching HDL design hosted at
   the `HDL repository <https://github.com/analogdevicesinc/hdl>`_ so it is
   ready for programming time.

PATH Setup
==========
Add the Propel toolchain to your ``PATH`` before running ``make`` by prepending
the three tool directories explicitly:

.. code-block:: bash

  export PATH="/opt/lscc/propel/2024.2/sdk/riscv-none-embed-gcc/bin":$PATH
  export PATH="/opt/lscc/propel/2024.2/sdk/tools/bin":$PATH
  export PATH="/opt/lscc/propel/2024.2/openocd/bin":$PATH

Verify with ``which riscv-none-embed-gcc srec_cat openocd`` to ensure the tools
are visible.

Build only
==========

.. code-block:: bash

  cd projects/ad738x_fmcz
  # when the sge assets already live in the project root
  make PLATFORM=lattice
  # automatic platform when the sge assets already live in the project root
  make
  # explicitly point to BSP assets that live elsewhere
  make HARDWARE=/path/to/your/sge

The lattice platform is auto-selected only when the project root already
contains an ``sge`` directory or ``sge.zip``, or when the ``HARDWARE`` variable
points to those assets. When a zip is detected, it is unpacked into
``<project>/sge``. When ``HARDWARE`` points to an external ``sge`` directory,
that directory is copied into ``<project>/sge`` as the canonical local runtime
location. After the first ``make HARDWARE=<non_project_root>/sge`` or
``make HARDWARE=<non_project_root>/sge.zip``, plain ``make`` can be used
for subsequent runs.

- Output artifacts:

  - ``build/ad738x_fmcz.elf`` (CPU executable)
  - ``build/ad738x_fmcz.map`` (linker map)
  - ``build/ad738x_fmcz.lst`` (objdump/listing)
  - ``build/ad738x_fmcz.siz`` (size report)
  - ``build/ad738x_fmcz.bin`` (raw binary image)
  - ``build/ad738x_fmcz.mem`` (byte-swapped memory image)

Build & Program
===============
1. Use `Lattice Radiant Programmer <https://www.latticesemi.com/Products/DesignSoftwareAndIP/FPGAandLDS/Radiant>`_
   to load the FPGA bitstream (``*.bit``) generated from the matching HDL design
   (for reference see the no-OS companion
   `HDL repository <https://github.com/analogdevicesinc/hdl>`_).
   This must happen before loading the CPU binary.
2. Run ``make run`` to upload the ``.elf`` with OpenOCD. The plain command
   works only when the project root contains the ``sge`` folder (or an
   auto-unpacked ``sge.zip``). Otherwise point ``HARDWARE`` to the folder that
   holds the exported BSP (``sge`` directory or ``sge.zip``).

   .. code-block:: bash

     # when the sge assets already live in the project root
     make run PLATFORM=lattice
     # automatic platform when the sge assets already live in the project root
     make run
     # explicitly point to BSP assets that live elsewhere
     make run HARDWARE=/path/to/your/sge

   Both recipes halt the CPU, load the ELF, and resume execution.

Clean
=====

Use one of the following cleanup targets, depending on scope:

- ``make clean``: removes compiled artifacts (``*.o``, ``*.elf``, etc.)
- ``make reset``: removes the entire ``build`` directory
- ``make reset-all``: runs ``reset`` and also removes ``<project>/sge``

Examples:

.. code-block:: bash

  # remove compiled artifacts
  make clean PLATFORM=lattice
  make clean
  make clean HARDWARE=/path/to/your/sge

  # remove the entire build directory
  make reset PLATFORM=lattice
  make reset
  make reset HARDWARE=/path/to/your/sge

  # remove build directory and the local copied/extracted sge folder
  make reset-all PLATFORM=lattice
  make reset-all
  make reset-all HARDWARE=/path/to/your/sge
