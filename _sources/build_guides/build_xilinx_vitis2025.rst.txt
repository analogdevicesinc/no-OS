*********************************************
Debugging with Xilinx Vitis IDE
*********************************************

.. important::
   The ``make sdkopen`` command automatically detects your Vitis version and launches the appropriate IDE:

   - **Vitis 2023.1 and older**: Classic Eclipse-based IDE
   - **Vitis 2023.2 - 2024.x**: Classic Eclipse-based IDE (using ``-classic`` flag for stability)
   - **Vitis 2025.1 and newer**: Unified IDE (VSCode-based)

**Why Classic Mode for Vitis 2023.2-2024.x?**

Vitis 2023.2 introduced the Unified IDE, but the User Managed Mode (required for makefile-based projects)
has incomplete debug configuration support. The classic Eclipse mode provides a mature, fully-functional
debugging experience for these versions.

**This guide covers debugging workflows for all Vitis versions:**

- **Vitis 2023.2-2024.x**: Classic Eclipse IDE workflow (Step 3A)
- **Vitis 2025.1+**: Unified IDE workflow (Step 3B)

Prerequisites
=============

- Vitis installed (any version)
- Hardware design file (.xsa) in project directory
- JTAG and UART cables connected to target board

.. note::
   After the IDE opens, proceed to the appropriate section based on your Vitis version:

   - **Vitis 2023.2-2024.x**: Continue to **Step 3A** (Classic Eclipse workflow)
   - **Vitis 2025.1+**: Continue to **Step 3B** (Unified IDE workflow)

WSL2 Users: One-Time xsdb Fix
==============================

.. important::
   On WSL2, xsdb crashes with "Segmentation fault" due to rlwrap incompatibility. Apply this fix once per machine.

**If you've already applied this fix previously, skip this step.**

Automated Installation (Recommended)
-------------------------------------

The installation script can automatically detect and fix the xsdb issue.

**For default Vitis installation** (``/xilinx/<version>/Vitis``):

.. code-block:: bash

   cd /path/to/no-OS
   sudo tools/scripts/platform/xilinx/install_xsdb_wsl2_fix.sh

The script will search for Vitis in common installation paths.

**For custom Vitis installation location:**

If Vitis is installed in a non-standard location, provide the path to the Vitis ``bin`` directory:

.. code-block:: bash

   cd /path/to/no-OS
   sudo tools/scripts/platform/xilinx/install_xsdb_wsl2_fix.sh /custom/path/to/Vitis/bin

**Examples:**

.. code-block:: bash

   # Vitis 2025.1 at default location
   sudo tools/scripts/platform/xilinx/install_xsdb_wsl2_fix.sh /xilinx/2025.1/Vitis/bin

   # Vitis 2023.2 at custom location
   sudo tools/scripts/platform/xilinx/install_xsdb_wsl2_fix.sh /opt/Xilinx/Vitis/2023.2/bin

   # Vitis on Windows drive (WSL)
   sudo tools/scripts/platform/xilinx/install_xsdb_wsl2_fix.sh /mnt/c/Xilinx/Vitis/2024.2/bin

Manual Installation
-------------------

.. note::
   The paths shown use the default Vitis installation directory.
   If you installed Vitis to a different location, adjust the paths accordingly.

.. code-block:: bash

   cd /path/to/no-OS

   # Backup original
   # Replace /xilinx/2025.1 with your Vitis installation path
   sudo cp /xilinx/2025.1/Vitis/bin/xsdb /xilinx/2025.1/Vitis/bin/xsdb.original

   # Install fixed version
   sudo cp tools/scripts/platform/xilinx/xsdb-nowrap /xilinx/2025.1/Vitis/bin/xsdb
   sudo chmod +x /xilinx/2025.1/Vitis/bin/xsdb

Per-Project Setup (First Time Only)
====================================

Step 1: Build Project
---------------------

.. code-block:: bash

   source /xilinx/2025.1/Vitis/settings64.sh
   cd /path/to/no-OS/projects/your_project
   make

**What happens:**

- First build automatically runs ``make project`` (generates BSP and FSBL)
- Compiles application
- Creates ``build/your_project.elf``

.. tip::
   You do NOT need to run ``make project`` manually - ``make`` calls it automatically when needed!

Building in Debug Mode
-----------------------

To enable debugging with proper compiler flags:

.. code-block:: bash

   make DEBUG=1

**What this does:**

- Disables optimization: ``-O0`` (so breakpoints work correctly)
- Adds full debug symbols: ``-g3``
- Remaps debug paths: ``-fdebug-prefix-map`` (so debugger finds source files)

**For production/release builds**, simply use ``make`` without ``DEBUG=1`` to get optimized code.

.. tip::
   You can set ``DEBUG = 1`` in your project Makefile to always build in debug mode for that project.

Step 2: Open Vitis IDE
-----------------------

.. code-block:: bash

   make sdkopen

**What happens:**

- The command automatically detects your Vitis version
- **For Vitis 2025.1+**: Unified IDE opens at project root

  - **First time only:** You'll see "Update Workspace" dialog - Click **"Update"** to initialize
  - Workspace metadata initialized (after clicking Update)
  - ``_ide/`` directory created with workspace configuration
  - **Debug configuration automatically generated** (``_ide/launch.json``)
  - Bitstream and initialization files extracted from XSA

- **For Vitis 2023.2-2024.x**: Classic Eclipse IDE opens

  - Workspace opens at ``build/`` directory
  - Standard Eclipse workspace with ``.metadata/`` directory
  - Manual debug configuration required (see Step 3A)

Choose Your Workflow
---------------------

The next steps depend on your Vitis version:

- **Vitis 2023.2-2024.x users**: Continue to **Step 3A** (manual debug configuration)
- **Vitis 2025.1+ users**: Skip to **Step 3B** (automated)

Step 3A: Create Debug Configuration (Classic Eclipse - Vitis 2023.2-2024.x)
----------------------------------------------------------------------------

.. note::
   This section is for **Vitis 2023.2-2024.x users** only.
   If you're using **Vitis 2025.1+**, skip to **Step 3B** below.

A. Open Debug Configurations Dialog
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the Vitis IDE menu bar:

- Go to **Run** → **Debug Configurations...**
- Or click the **Debug** toolbar button dropdown → **Debug Configurations...**

The "Debug Configurations" dialog will open.

B. Create New Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. In the left panel, expand **"Single Application Debug"**
2. Click the **"New Configuration"** button (first icon in the toolbar - looks like a document with a star/plus)
3. A new configuration will be created (e.g., ``Debugger_-Default``)
4. You can rename it if desired (e.g., ``adrv904x-debug``)

C. Configure Main Tab
^^^^^^^^^^^^^^^^^^^^^^

The **"Main"** tab should be selected by default.

**Debug Type:**

- Select **"Standalone Application Debug"** from the dropdown
- (Not "Attach to running target" - we want to reset and program the system)

**Connection:**

- Leave as **"Local"** (debugging via local JTAG connection)

D. Configure Target Setup Tab
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Click the **"Target Setup"** tab at the top.

**Hardware Platform:**

- Should auto-populate with the path to your ``.xsa`` file
- If empty, click **"Browse..."** and select ``system_top.xsa`` from your project root

**Bitstream File:**

- Auto-populated from the XSA file
- Path will be similar to: ``.../projects/your_project/system_top.bit``

**FSBL Configuration** (ZynqMP/Zynq-7000 only):

Check these boxes:

- ☑ **Use FSBL flow for initialization**
- ☑ **Reset entire system**
- ☑ **Program FPGA**
- ☑ **Initialize using FSBL**

**FSBL File:**

Browse to or enter the FSBL path:

.. code-block::

   build/tmp/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf

.. note::
   **For MicroBlaze**: Uncheck "Use FSBL flow for initialization" - MicroBlaze doesn't use FSBL

**Summary Panel:**

After configuration, the Summary panel on the right shows the debug sequence:

1. Reset system and clear FPGA
2. Program FPGA with bitstream
3. Initialize PS using FSBL
4. Load application and suspend processors

E. Configure Application Tab
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Click the **"Application"** tab at the top.

**Processor Selection:**

The IDE shows a table with available processors. Check the box next to your target processor:

- **ZynqMP**: ``psu_cortexa53_0`` (or ``psu_cortexr5_0`` for R5)
- **Zynq-7000**: ``ps7_cortexa9_0``
- **MicroBlaze**: ``microblaze_0``
- **Versal**: ``psv_cortexa72_0``

**Project and Application:**

The IDE typically auto-populates these fields:

- **Project**: Should show your project name (e.g., ``adrv904x``)
- **Application**: Should point to your ELF file:

  .. code-block::

     build/your_project.elf

.. tip::
   If the Application field is empty, click **"Search..."** and browse to ``build/your_project.elf``

**Stop at 'main':**

- Check this box to have the debugger stop at the ``main()`` function (recommended)

F. Save and Apply
^^^^^^^^^^^^^^^^^

1. Click **"Apply"** to save the configuration
2. Click **"Debug"** to start debugging immediately, or **"Close"** to save for later

The configuration is now saved and ready to use!

Step 3B: Automatic Debug Configuration (Unified IDE - Vitis 2025.1+)
----------------------------------------------------------------------

.. note::
   This section is for **Vitis 2025.1+ users** only.
   If you're using **Vitis 2023.2-2024.x**, use **Step 3A** above instead.

.. important::
   For optimal debugging experience, always build with ``make DEBUG=1`` before debugging.
   This ensures proper optimization settings (-O0) and correct source path mapping.

Debug Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When you run ``make sdkopen`` with Vitis 2025.1+, the build system automatically:

1. **Extracts hardware files** from the XSA:

   - Bitstream (``_ide/system_top/system_top.bit``)
   - Initialization script (``psu_init.tcl`` or ``ps_init.tcl``)

2. **Generates debug configuration** (``_ide/launch.json``) with:

   - Correct architecture-specific settings (ZynqMP, Zynq-7000, MicroBlaze, Versal)
   - Hardware platform path (XSA file)
   - FSBL configuration (if applicable)
   - Application ELF path
   - Target processor

**What this means:** No manual configuration needed! The debug configuration is ready to use.

Verify Configuration (Optional)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you want to verify or customize the generated configuration:

1. In Vitis Explorer, expand ``_ide`` folder
2. Open ``launch.json`` to view the configuration
3. You'll see a configuration named ``<project_name>_app_hw_1``

The configuration is automatically regenerated each time you run ``make sdkopen``.

Debugging Your Project
======================

Hardware Setup
--------------

- Connect JTAG cable to your board
- Connect UART cable (for console output)
- Power on the board

Start Debugging
---------------

**Prerequisites:**

1. Make sure you've built your project: ``make`` (in terminal)
2. Hardware connected (JTAG and UART cables, board powered on)

**Classic Eclipse IDE (Vitis 2023.2-2024.x):**

1. Go to **Run** → **Debug Configurations...**
2. Select your debug configuration (e.g., "adrv904x-debug")
3. Click **"Debug"** button
4. The Debug perspective will open automatically

**Unified IDE (Vitis 2025.1+):**

1. Click **FLOW** panel (left side) → Click **"Debug"**
2. The debug session will start

.. note::
   The **Start Debugging (F5)** button in the Debug panel does not currently work for Vitis 2025.1.
   You must use the **FLOW** → **Debug** button to start debugging sessions.

**What happens (both versions):**

- Vitis connects to board via JTAG
- Programs FPGA with bitstream
- Runs FSBL to initialize processor (ZynqMP/Zynq only)
- Loads your application ELF
- Stops at entry point (usually ``main()``) - ready to debug!

**Debug Features (both versions):**

- Set breakpoints (click left margin in code)
- Step through code (F5=Step Into, F6=Step Over, F7=Step Return, F8=Resume)
- Inspect variables, registers, call stack
- Watch expressions
- View memory and disassembly

Daily Development Workflow
==========================

After initial setup:

.. code-block:: bash

   # 1. Edit code

   # 2. Build with debug symbols (recommended for debugging)
   make clean
   make DEBUG=1

   # 3. Open Vitis and debug
   make sdkopen
   # Click FLOW → Debug → Debugging starts!

.. tip::
   For production builds (no debugging), use ``make`` without ``DEBUG=1`` to get optimized code.

Architecture-Specific Notes
============================

ZynqMP (Cortex-A53/R5)
----------------------

- **Processor:** ``psu_cortexa53_0`` or ``psu_cortexr5_0``
- **FSBL Required:** Yes
- **FSBL Path:** ``build/tmp/output/hw0/export/hw0/sw/hw0/boot/fsbl.elf``

Zynq-7000 (Cortex-A9)
---------------------

- **Processor:** ``ps7_cortexa9_0``
- **FSBL Required:** Yes
- **FSBL Path:** Same as ZynqMP

MicroBlaze
----------

- **Processor:** ``microblaze_0``
- **FSBL Required:** No (soft processor)
- **In launch.json:** Set ``initWithFSBL: false``

Versal (Cortex-A72)
-------------------

- **Processor:** ``psv_cortexa72_0``
- **Uses PLM** (Platform Loader Manager) instead of FSBL

Troubleshooting
===============

"Segmentation fault" when debugging
------------------------------------

**Solution:** Install xsdb WSL2 fix (see above)

Summary
=======

**One-time system setup:**

- Fix xsdb on WSL2 (if applicable)

**Per-project setup:**

- **Vitis 2023.2-2024.x (Classic Eclipse):** Build → Open Vitis → Create debug configuration manually (Run → Debug Configurations) → Configure Application and Target Setup tabs → Apply
- **Vitis 2025.1+ (Unified IDE):** Build → Open Vitis → Click "Update" → **Debug configuration auto-generated!**

**Daily workflow:**

.. code-block:: bash

   # Vitis 2025.1+ (Unified IDE - AUTOMATED)
   make clean && make DEBUG=1
   make sdkopen
   # Click FLOW → Debug → Debugging starts!

   # Vitis 2023.2-2024.x (Classic Eclipse - Manual)
   make clean && make DEBUG=1
   make sdkopen
   # Run → Debug Configurations → Select your config → Debug

**Result:** Streamlined debugging workflow for no-OS Xilinx projects across all Vitis versions.

Version-Specific Behavior
==========================

The ``make sdkopen`` command adapts to your Vitis version:

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - Vitis Version
     - IDE Type
     - Notes
   * - 2023.1 and older
     - Classic Eclipse
     - Manual debug configuration required
   * - 2023.2 - 2024.x
     - Classic Eclipse (via ``-classic`` flag)
     - Manual debug configuration required; Unified IDE User Managed Mode has incomplete debug support
   * - 2025.1 and newer
     - Unified IDE
     - **Automatic debug configuration** - launch.json auto-generated by build system

Additional Resources
====================

- **no-OS GitHub:** https://github.com/analogdevicesinc/no-OS
