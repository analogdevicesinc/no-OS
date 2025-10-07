======================================
No-OS CMake Build System Quick Guide
======================================

This guide provides a quick start for using the CMake build system in the No-OS repository. It covers listing available boards, compiling projects, flashing firmware, and configuring features using Kconfig.

Prerequisites
=============

* CMake 3.28 or later
* Python 3 with pip
* Cross-compilation toolchain for your target platform
* OpenOCD or J-Link for flashing (optional)

Available Boards and Platforms
==============================

Listing Available Boards
-------------------------

To see all available board configurations::

    cmake --list-presets=configure

Current supported boards:

* ``max78000_fthr`` - MAX78000 Feather board (Maxim platform)
* ``max32650_fthr`` - MAX32650 Feather board (Maxim platform)
* ``apard32690`` - APARD32690 board (Maxim platform)
* ``stm32f756_nucleo`` - STM32F756 Nucleo board (STM32 platform)
* ``adin1110ebz`` - ADIN1110-EBZ board (STM32 platform)

Board configurations are stored in ``board_configs/{platform}/{board}_defconfig`` files.

Available Projects
==================

To see available build targets::

    cmake --list-presets=build

Current projects include:

* ``iio_demo`` - Industrial I/O demonstration project
* ``eval-adxl355-pmdz`` - ADXL355 evaluation examples
* ``ad74413r`` - AD74413R driver example

Project configurations are in ``projects/{project}/project.conf`` files.

Building Projects
=================

Basic Build Process
-------------------

1. Configure the build with a board preset::

    cmake -B build --preset <board_name> -DPROJECT_DEFCONFIG=<project>/project.conf

2. Build the project::

    cmake --build build --target <project_name>

Example: Building iio_demo for MAX32690
----------------------------------------

Configure::

    cmake -B build --preset apard32690 -DPROJECT_DEFCONFIG=iio_demo/project.conf

Build::

    cmake --build build --target iio_demo

The resulting firmware will be in ``build/projects/iio_demo/``.

Alternative: Using Project Name
-------------------------------

You can also use the PROJECT_NAME parameter (requires unified presets)::

    cmake -B build --preset apard32690 -DPROJECT_NAME=iio_demo
    cmake --build build --preset iio_demo

Board Compatibility
===================

Board-Project Compatibility Checking
-------------------------------------

The build system automatically enforces compatibility between boards and projects. Each project defines which boards it supports through board-specific configuration files in ``projects/{project}/boards/``.

To check if a board supports a project, look for a corresponding ``.conf`` file::

    projects/
    └── iio_demo/
        └── boards/
            ├── max78000_fthr.conf      # iio_demo supports max78000_fthr
            ├── apard32690.conf         # iio_demo supports apard32690
            └── stm32f756_nucleo.conf   # iio_demo supports stm32f756_nucleo

If you try an incompatible combination, CMake will show an error with supported boards::

    ❌ Board compatibility error!
       Project: iio_demo
       Board: unsupported_board
       Platform: maxim

       Project 'iio_demo' does not support board 'unsupported_board'.
       Supported boards: max78000_fthr, apard32690, stm32f756_nucleo

Checking Compatibility Manually
-------------------------------

You can verify compatibility before building:

1. List available presets to see supported boards::

    cmake --list-presets=configure

2. Check if a project has a board configuration file::

    ls projects/<project_name>/boards/<board_name>.conf

3. Try the configuration (will fail fast if incompatible)::

    cmake -B test-build --preset <board_name> -DPROJECT_DEFCONFIG=<project>/project.conf

Flashing Firmware
=================

The CMake build system provides flash targets for both J-Link and OpenOCD.

J-Link Flashing
---------------

For boards with J-Link support, use the flash target::

    cmake --build build --target flash_<project_name>

Example for iio_demo::

    cmake --build build --target flash_iio_demo

This automatically:
1. Connects to the target via J-Link
2. Loads the firmware
3. Resets and starts execution

OpenOCD Flashing
----------------

For OpenOCD-compatible debug probes, use the OpenOCD flash target::

    cmake --build build --target flash_openocd_<project_name>

Example for iio_demo::

    cmake --build build --target flash_openocd_iio_demo

This uses an auto-generated OpenOCD configuration file specific to your board and project.

VS Code Debugging
-----------------

After building a project, VS Code debugging files are automatically generated:

* ``.vscode/tasks.json`` - Build and flash tasks
* ``.vscode/launch.json`` - Debug configurations for Cortex Debug extension

To debug in VS Code:

1. Install the Cortex-Debug extension
2. Build your project::

    cmake --build build --target <project_name>

3. Open the project in VS Code
4. Press F5 or go to Run and Debug
5. Select the generated debug configuration for your project

The debug configuration will automatically:
- Flash the firmware
- Start the debug session
- Break at main()

Kconfig and Feature Configuration
=================================

Understanding Configuration Files
---------------------------------

The No-OS build system uses Kconfig for feature selection:

* **Board defconfig**: Hardware-specific settings (``board_configs/{platform}/{board}_defconfig``)
* **Project config**: Application-specific settings (``projects/{project}/project.conf``)

Configuration files use ``CONFIG_`` prefixed options that enable/disable features.

Core Configuration Categories
-----------------------------

**Core Peripheral Drivers** (``CONFIG_<PERIPHERAL>=y``)

These enable generic peripheral support across all platforms::

    CONFIG_SPI=y          # Serial Peripheral Interface
    CONFIG_I2C=y          # Inter-Integrated Circuit
    CONFIG_UART=y         # Universal Asynchronous Receiver-Transmitter
    CONFIG_GPIO=y         # General Purpose Input/Output
    CONFIG_IRQ=y          # Interrupt handling
    CONFIG_DMA=y          # Direct Memory Access
    CONFIG_USB_UART=y     # USB-to-UART bridge functionality

**Platform Support**

These are configured automatically in board defconfig files::

    CONFIG_PLATFORM_DRIVERS=y    # Enable platform-specific drivers
    CONFIG_MAXIM=y               # Enable Maxim platform support
    CONFIG_STM32=y               # Enable STM32 platform support

**Framework and Application Support**::

    CONFIG_IIO=y              # Industrial I/O framework
    CONFIG_FREERTOS=y         # FreeRTOS real-time operating system
    CONFIG_LWIP=y             # Lightweight TCP/IP stack
    CONFIG_ADC_DEMO=y         # ADC demonstration code
    CONFIG_DAC_DEMO=y         # DAC demonstration code

Interactive Configuration
-------------------------

Use menuconfig for interactive feature selection::

    cmake --build build --target menuconfig

This opens a text-based interface for browsing and modifying configuration options. Navigate through categories like:

* **Drivers** - All driver categories (ADC, DAC, Platform, etc.)
* **Platform Drivers** - Platform-specific implementations
* **IIO** - Industrial I/O framework options
* **Libraries** - Third-party libraries (FreeRTOS, lwIP, etc.)

Adding New Drivers/Features
---------------------------

1. **Find the relevant Kconfig file**: Driver options are typically in ``drivers/{category}/Kconfig``

2. **Enable the driver**: Add the CONFIG option to your project.conf::

    CONFIG_AD7124=y

3. **Check dependencies**: Some drivers require additional features::

    CONFIG_AD7124=y
    CONFIG_SPI=y              # AD7124 requires SPI
    CONFIG_IIO=y              # For IIO interface

4. **Rebuild**: Clean and rebuild to apply changes::

    cmake --build build --target clean
    cmake --build build --target <project_name>

Configuration Hierarchy
-----------------------

Configuration files are merged in this order:

1. Board defconfig (hardware settings)
2. Board-specific project config (if exists: ``projects/{project}/boards/{board}.conf``)
3. Project config (application settings)

Later files override earlier ones for conflicting options.

Build Targets and Utilities
============================

Useful Build Targets
--------------------

* ``menuconfig`` - Interactive configuration editor
* ``clean`` - Clean build files
* ``distclean`` - Clean build files and configuration
* ``flash_<project>`` - Flash firmware via J-Link
* ``flash_openocd_<project>`` - Flash firmware via OpenOCD

Generate compile_commands.json for IDE support::

    cmake -B build --preset <board> -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

Out-of-Tree Builds
===================

The CMake system supports building projects outside the No-OS repository. For detailed instructions on setting up out-of-tree projects, see the separate out-of-tree documentation.

Troubleshooting
===============

Common Issues
-------------

**Configuration not found**:
  Ensure PROJECT_DEFCONFIG path is correct relative to source directory

**Toolchain errors**:
  Check that cross-compilation toolchain is in PATH

**Missing dependencies**:
  Verify Python packages are installed: ``pip install -r tools/scripts/requirements.txt``

**Build failures**:
  Try a clean rebuild: ``cmake --build build --target clean && cmake --build build --target <project>``

**Flash failures**:
  Ensure OpenOCD/J-Link can connect to target before loading firmware

**Board compatibility errors**:
  Check that the project supports your board by looking for ``projects/{project}/boards/{board}.conf``

Debug Build
-----------

For debug builds with symbols::

    cmake -B build --preset <board> -DCMAKE_BUILD_TYPE=Debug -DPROJECT_DEFCONFIG=<project>/project.conf

This enables VS Code debugging and disables optimizations.

Custom Board Support
--------------------

To add a new board:

1. Create board defconfig: ``board_configs/{platform}/{board}_defconfig``
2. Add preset to ``CMakePresets.json``::

    {
        "name": "my_board",
        "description": "My Custom Board",
        "inherits": "default",
        "cacheVariables": {
            "BOARD": "my_board",
            "PLATFORM": "maxim",
            "TARGET": "max32690",
            "TARGET_NUM": "32690",
            "BOARD_CONFIG_FILE": "my_board_defconfig"
        }
    }

3. Add board support to projects by creating ``projects/{project}/boards/my_board.conf`` files