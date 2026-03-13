****************************************
Lattice Propel (RISC-V RX) Build Guide
****************************************

RISC-V RX overview
------------------
The Lattice RISC-V RX soft IP is a 32-bit RISC-V core with AXI connectivity,
aligned to the AXI-based IPs used in the `HDL repository
<https://github.com/analogdevicesinc/hdl>`_. It bundles a
PLIC (Platform Level Interrupt Controller), a CLINT (Core Local Interrupter),
and a watchdog, and supports the RV32IMACF instruction set. The core offers
three privilege modes (Machine, Supervisor, User), multiple processor modes
(Advanced, Balanced, Lite), and includes instruction/data caches plus a
JTAG (IEEE 1149.1) debug interface. Because its AXI interface is compatible with
the AXI control interface used for most of our IPs in the HDL repository,
this is the RISC-V variant we currently support on Lattice FPGAs. See the vendor
page for details:
https://www.latticesemi.com/products/designsoftwareandip/intellectualproperty/ipcore/ipcores04/risc-v-rx-cpu

Scope
=====
Bare-minimum steps to build and run a Lattice RX design. Tested with the build
tools provided in Propel SDK 2024.2 and the RISCV-RX
``latticesemi.com:ip:riscv_rtos:2.5.0`` core used by ``projects/ad738x_fmcz``.

Install & Inputs
================
1. Install the `Propel SDK <https://www.latticesemi.com/Products/DesignSoftwareAndIP/FPGAandLDS/LatticePropel>`_
   (includes GCC, srec_cat, OpenOCD).
2. Export a hardware package from your HDL project (``sge`` directory or
   ``sge.zip``). Drop it next to the project ``Makefile``.
3. *(Optional but recommended)* Generate the FPGA bitstream separately via
   Radiant/Propel from the matching HDL design hosted at the `HDL repository
   <https://github.com/analogdevicesinc/hdl>`_ so it is ready for programming
   time.

PATH Setup
==========
Add the Propel toolchain to your ``PATH`` before running ``make`` by prepending
the three tool directories explicitly:

.. code-block:: bash

  export PATH="/opt/lscc/propel/2024.2/sdk/riscv-none-embed-gcc/bin":$PATH
  export PATH="/opt/lscc/propel/2024.2/sdk/tools/bin":$PATH
  export PATH="/opt/lscc/propel/2024.2/openocd/bin":$PATH

Verify with ``which riscv-none-embed-gcc`` to ensure the tools are visible.

Build
=====

.. code-block:: bash

  cd projects/ad738x_fmcz
  make PLATFORM=lattice
  # when the sge assets already live in the project root (or HARDWARE is set)
  make
  # explicitly point to BSP assets that live elsewhere
  make HARDWARE=/path/to/your/sge

``make`` compiles the BSP and produces ``build/ad738x_fmcz.elf`` plus the
``*.mem`` artifact. The lattice platform is auto-selected only when the project
root already contains an ``sge`` directory (or ``sge.zip``) or when the
``HARDWARE`` variable points at those assets. When a zip is detected it is
unpacked into ``<project>/sge`` before continuing, so a plain ``make`` works in
those cases.

Program
=======
1. Use Lattice Radiant Programmer (or your preferred flow) to load the FPGA
   bitstream generated from the matching HDL design (for reference see the
   no-OS companion `HDL repository <https://github.com/analogdevicesinc/hdl>`_).
   This must happen before touching the CPU binary.
2. Run ``make run`` to upload the ``.elf`` with OpenOCD. The plain command
   works only when the project root contains the ``sge`` folder (or an
   auto-unpacked ``sge.zip``). Otherwise point ``HARDWARE`` to the folder that
   holds the exported BSP (``sge`` directory or ``sge.zip``).

   .. code-block:: bash

     make run PLATFORM=lattice
     # when the sge assets already live in the project root (or HARDWARE is set)
     make run
     # explicitly point to BSP assets that live elsewhere
     make run HARDWARE=/path/to/your/sge

   Both recipes halt the CPU, load the ELF, and resume execution.

Notes
=====
- ``make run`` assumes the default Lattice USB cable (channel 14). Override
  ``LATTICE_OPENOCD_INTERFACE`` / ``LATTICE_OPENOCD_TARGET`` if needed.
- To rebuild from scratch use the same variants shown above, e.g.
  ``make clean PLATFORM=lattice``, plain ``make clean`` when the BSP sits in the
  project, or ``make clean HARDWARE=/path/to/your/sge`` when it does not.
