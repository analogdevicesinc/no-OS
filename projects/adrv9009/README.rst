ADRV9009 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- ZC706 + ADRV9009-W/PCBZ
- ADRV2CRR-FMC + ADRV9009-ZU11EG
- ZCU102 + FMCOMMS8

Overview
--------

The ADRV9009 no-OS project is designed to integrate the ADRV9009 RF
transceiver into embedded systems without the need for a traditional
operating system. It provides the infrastructure to initialize and
configure the ADRV9009 device, covering the setup of its SPI interface,
clock management, and gain control parameters. Key components include
data structures like ``adrv9009_init_param`` and ``adrv9009_rf_phy``, 
which handle initialization parameters and device-specific configurations,
such as RX and TX sample clocks, firmware identifiers, and gain tables.
The project includes functions like ``adrv9009_setup`` for comprehensive
initialization, device setting configuration, firmware loading, and RF
signal path enabling. It supports various embedded platforms and offers
macros for conditional configurations to enhance compatibility across
different hardware setups for efficient ADRV9009 operation in RF
communication applications.

Applications
------------

- 3G, 4G, and 5G TDD macrocell base stations
- TDD active antenna systems
- Massive multiple input, multiple output (MIMO)
- Phased array radar
- Electronic warfare
- Military communications
- Portable test equipment

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9009 transceiver requires several distinct power domains for
stable operation. Key supplies include VDD_AP13 and VDD_DPD13 at 1.3V
for analog and digital circuits respectively, VDD_AIP18 at 1.8V for
transmission, and VDD_A_3P3 at 3.3V for auxiliary functions. Each
transmitter output needs approximately 200 mA of current supplied
externally, with 0.1 μF decoupling capacitors recommended for stability.
A specific power-up sequence must be adhered to, starting with the 1.3V
supplies to prevent power surges, ensuring optimal transceiver
performance as detailed in the ADRV9009 technical resources.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector Name        | Type                  | Description           |
+-----------------------+-----------------------+-----------------------+
| JESD204B Interface    | Differential          | Used for high-speed   |
|                       |                       | data transfer between |
|                       |                       | the ADRV9009 and      |
|                       |                       | other devices.        |
+-----------------------+-----------------------+-----------------------+
| REF_CLK_IN            | Differential          | Reference clock       |
|                       |                       | input, routed with    |
|                       |                       | shielding to prevent  |
|                       |                       | interference.         |
+-----------------------+-----------------------+-----------------------+
| RF_EXT_LO_I/O         | I/O                   | External local        |
|                       |                       | oscillator            |
|                       |                       | input/output.         |
+-----------------------+-----------------------+-----------------------+
| SPI Interface         | Serial                | Used for serial       |
|                       |                       | communication and     |
|                       |                       | control of the        |
|                       |                       | ADRV9009.             |
+-----------------------+-----------------------+-----------------------+
| GPIO Pins             | I/O                   | General purpose       |
|                       |                       | input/output for      |
|                       |                       | various control       |
|                       |                       | signals.              |
+-----------------------+-----------------------+-----------------------+
| Power Supply          | Power                 | Supplies power to the |
| Connectors            |                       | ADRV9009 evaluation   |
|                       |                       | board.                |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

Here the available examples and/or platforms:

- IIO
- DMA
- MB
- ZU

The initialization data used in the examples is taken out 
from the :git-no-OS:`Project Source Data Path </projects/adrv9009/src>`.

For full build guide, visit: `ADRV9009/ADRV9008 No-OS System Level
Design Setup <https://wiki.analog.com/resources/eval/user-guides/adrv9009/no-os-setup>`__

IIO Example
~~~~~~~~~~~

The IIO (Industrial I/O) example enables IIO server functionality on
supported hardware. When built with IIO support, the firmware starts an
IIOD (IIO Daemon) server which allows remote data streaming and
configuration through IIO client applications such as IIO Oscilloscope.
This example enables control and monitoring of ADC and DAC data via
standard IIO interfaces, exposing the ADRV9009’s capabilities to the
remote host using libiio for data acquisition, device configuration, and
signal visualization.

- Implemented via ``start_iiod()`` function, integrating IIO application
  logic with device DMA and buffer operations.

- Activated by setting ``IIOD=y`` in the build configuration
  (``builds.json``).

Supported platforms: ``adrv9009_zc706``, ``adrv9009_zcu102``

--------------

DMA Example
~~~~~~~~~~~

The DMA example demonstrates direct control of the on-board Direct
Memory Access (DMA) engines for moving ADC/DAC data between the ADRV9009
and system memory. It initializes the JESD204, DMA, and data converter
cores, exercising streaming data paths for test and verification without
requiring the IIO interface. This is a minimal example focusing on basic
data movement and integration with the FPGA fabric.

- Enabled with the build flag ``NEW_CFLAGS=-DDMA_EXAMPLE`` in
  ``builds.json``.
- Associated with files such as ``headless_mb.c`` and ``headless_arm.c``
  which show initialization and control of DMA engines.

Supported platforms: ``adrv9009_zc706``, ``adrv9009_zcu102``

--------------

MB (MicroBlaze)
~~~~~~~~~~~~~~~

MB refers to the Xilinx MicroBlaze soft processor platform. Examples
marked MB run on Xilinx FPGAs with a MicroBlaze CPU. The
MicroBlaze-specific implementation file (``headless_mb.c``) contains
code for hardware initialization, SPI, GPIO, JESD204, DMA, and IIO
support appropriate to this processor. These builds use
platform-specific Makefile includes (e.g., ``mb_src.mk``).

Key implementation file: ``src/app/headless_mb.c``

--------------

ZU (Zynq UltraScale+ MPSoC)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

ZU refers to Xilinx Zynq UltraScale+ platforms (such as ZCU102). ZU
builds use the ARM Cortex-A53 cores within the Zynq MPSoC, with
initialization and application logic in ``headless_arm.c``. These builds
use the ``zu_src.mk`` platform-specific Makefile, and provide support
for system integration, DMA/ADC/DAC handling, and optional IIO
functionality.

Key implementation file: ``src/app/headless_arm.c``

--------------

Summary Table
~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Example               | Description           | Platform/Target       |
+-----------------------+-----------------------+-----------------------+
| IIO                   | IIO server for remote | ZC706, ZCU102         |
|                       | data/control via IIO  |                       |
|                       | clients               |                       |
|                       | (e.g. Oscilloscope)   |                       |
+-----------------------+-----------------------+-----------------------+
| DMA                   | Direct memory access  | ZC706, ZCU102         |
|                       | data transfer         |                       |
|                       | example, no IIO       |                       |
+-----------------------+-----------------------+-----------------------+
| MB                    | MicroBlaze softcore   | FPGA w/ MicroBlaze    |
|                       | processor-specific    |                       |
|                       | integration/example   |                       |
+-----------------------+-----------------------+-----------------------+
| ZU                    | Zynq UltraScale+      | ZCU102, compatible    |
|                       | MPSoC (ARM)           | Zynq boards           |
|                       | processor-specific    |                       |
|                       | integration/example   |                       |
+-----------------------+-----------------------+-----------------------+

These examples can be selected in the build system and provide reference
implementations for integrating and testing the ADRV9009 on various
Xilinx platforms.

To build the Basic Example for the ADRV9009 no-OS project, follow these
steps to configure the Makefile and invoke the build:

- Navigate to the ADRV9009 project directory:

  ::

    cd no-OS/projects/adrv9009

- Open ``Makefile`` in a text editor. By default, the project will build
  for the ZC706 or ZCU102 platform with the standard “Basic Example”
  functionality unless any specific examples (“IIO”, “DMA_EXAMPLE”) are
  enabled. Ensure that all of the following build options are commented
  out:

  ::

     // #define DMA_EXAMPLE
     // #define IIO_SUPPORT

- Do not enable any special options; leaving these commented uses the
  basic example.

- Optionally, select the appropriate ``PROFILE`` for your RF
  configuration (bandwidth, sample rates) by uncommenting the desired
  line.

- Build the Project. Run ``make`` with your desired platform. For example, to build for the
  ZCU102:

  ::

     make PLATFORM=zynqmp TARGET=adrv9009_zcu102

  For ZC706:

  ::

    make PLATFORM=zynq TARGET=adrv9009_zc706

If needed, specify cross-compilation toolchains or board-specific
details as required by your development setup.

Summary
~~~~~~~

- Ensure no ``DMA_EXAMPLE`` or ``IIO_SUPPORT`` defines are active in the
  Makefile.

- Select the proper hardware platform in the make command.

- Run ``make`` from the project directory.

This procedure will compile the default “basic” no-OS firmware for the
ADRV9009, suitable for standalone device initialization and data path
verification.

No-OS Supported Platforms
-------------------------

Building the ADRV9009 no-OS Project for Xilinx Platforms
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Supported Hardware
^^^^^^^^^^^^^^^^^^

- ZCU706
- ZCU102

Connections
^^^^^^^^^^^

The ADRV9009 interfaces with Xilinx boards using:

- **SPI Interface**: Configures and controls the transceiver’s settings.
  Include ``no_os_spi.h`` for relevant operations.

- **GPIO Interface**: Handles hardware signaling and control. Use
  ``no_os_gpio.h``.

Build Configuration
^^^^^^^^^^^^^^^^^^^

The ``builds.json`` file defines configurations for the ADRV9009
project, accommodating different requirements:

1. **Demo Configuration**:

   - No additional flags.

   - Supported platforms: ``adrv9009_zc706``, ``adrv9009_zcu102``.

2. **IIO Configuration**:

   - Flag: ``IIOD=y`` to enable the Industrial I/O Daemon.

   - Supported platforms: Identical to the demo configuration.

Build Command
^^^^^^^^^^^^^

To build the project:

- Navigate to the project directory: no-OS/projects/adrv9009/
- Execute the ``make`` command

The build command is only “make” but make sure to choose between what
board will be used. 
