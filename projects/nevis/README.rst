ADRV9001 no-OS Example Project
==============================

**Note:** 
The ADRV9001 name in this document refers collectively to the
ADRV9002, ADRV9003, and ADRV9004 device family. Within the document,
**ADRV9001** may be used interchangeably to address any of these specific
models.

Supported Devices
-----------------

- ADRV9002 in `ADRV9002NP/W2/PCBZ <https://www.analog.com/EVAL-ADRV9002>`__

Supported Carriers
-------------------

- **ADRV9002** and **ZCU102** using **FMC HPC0** slot
- **ADRV9002** and **ZC706** using **FMC LPC** slot
- **ADRV9002** and **ZedBoard** using **FMC LPC** slot

Overview
---------

The ADRV9001 no-OS project enables integration of the ADRV9001
series (ADRV9002, ADRV9003, and ADRV9004) into embedded systems without
requiring an operating system. It employs a hardware abstraction layer
to manage SPI communication, GPIO functions, and platform-specific
settings, illustrated in ``no_os_platform.c`` and related headers.
Designed to work with Xilinx hardware platforms such as ZCU102, ZC706,
and ZedBoard, the project outlines build configurations in files like
``src.mk`` and ``builds.json``, detailing drivers for ADC/DAC cores and
potential IIO support. Key components, such as ``headless.c``, handle
hardware initialization, RX/TX channel configuration, and DMA
management, allowing for autonomous signal processing in a no-OS
environment.

Applications
------------

- Mission critical communications
- Very high frequency (VHF) applications
- Ultrahigh frequency (UHF) applications
- Cellular applications from 30 MHz to 6000 MHz
- Time Division Duplexing (TDD) applications
- Frequency Division Duplexing (FDD) applications

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
-------------------------

The demo applications highlight the functionality of the ADRV9002
evaluation board. Two examples are provided in the sample project:

- DMA Example
- IIO Example

The initialization data used in the examples is taken out from the
`Project Source Data Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv9001/src>`__

DMA Example
~~~~~~~~~~~~

The DMA Example project is aimed at Xilinx hardware platforms like the
ZCU102 and is designed to efficiently manage Direct Memory Access (DMA)
for data transfer between the ADRV9001 transceiver and memory. The
project handles both transmitting (TX) and receiving (RX) channels by
setting up the appropriate DMA controllers with defined base addresses,
such as ``RX1_DMA_BASEADDR`` and ``TX1_DMA_BASEADDR``. 
The ``headless.c`` file orchestrates the initialization sequence, 
ADC/DAC core setup. The ``builds.json`` file specifies environment setup,
including compilation flags for enabling specific DMA features. This
configuration supports high-performance data transfers without an
operating system, facilitating mission-critical and high-frequency
communications. Users can load custom data, manage cache operations, and
perform cyclic or non-cyclic DMA transfers to achieve optimal
application performance.

To build the DMA Example, configure the `Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/Makefile>`__
with:

.. code-block:: bash

   NEW_CFLAGS = -DDAC_DMA_EXAMPLE

IIO Example
~~~~~~~~~~~~

The IIO example is designed specifically for Xilinx platforms such as
the ZCU102 to demonstrate the setup of Industrial I/O in a no-OS
environment. It involves initializing ADC and DAC channels with
predetermined parameters and configuring the IIO application through the
``iio_run`` function. This function is responsible for the essential
initialization steps, including the configuration of device descriptors,
data buffers, and UART settings necessary for operation. The project’s
build environment, delineated in ``builds.json``, includes a flag
``IIOD=y`` to activate the IIO Daemon. This setup serves to facilitate
high-performance data streaming, utilizing the ADRV9001 series’
capabilities in embedded systems without an OS.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The No-OS IIO Application together with the ADRV9001 driver take care of
all the back-end logic needed to setup the IIO server.

To build the IIO Demo example, use this configuration on the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/Makefile>`__ :

.. code-block:: bash

   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx Platform
~~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- Zynq UltraScale+™ MPSoC ZCU102 Evaluation Kit
- `ADRV9002NP/W2/PCBZ <https://www.analog.com/EVAL-ADRV9002>`__
  (Evaluation Board for ADRV9002)

Requirements
^^^^^^^^^^^^^

**Software Configuration:**

The SD card must have the ``BOOT.bin``, ``Image``, and ``system.dtb``
files for booting the Zynq ZCU102 board. Full instructions on how to
build the BOOT files are available in this page: `Build the boot image
BOOT.BIN <https://analogdevicesinc.github.io/hdl/user_guide/build_boot_bin.html>`__

**Hardware Setup:**

- Attach the ADRV9002 Evaluation Board to the ZCU102 through the FMC
  HPC0 connector.

- Enable SD Card BOOT mode by switching SW6 to:

    ====== =====
    Switch State
    SW6-1  ON
    SW6-2  OFF
    SW6-3  OFF
    SW6-4  OFF
    ====== =====

- Insert the SD card into the ZCU102 SD card slot (J100).

- Connect a micro-USB cable to the ZCU102 board via the USB-UART
  connector (J83), and the other end of the cable to the Host PC

- Connect the power adapter to the ZCU102 board. via the power supply
  plug (J52).

- Switch on the ZCU102 board using the SW1 switch.

- Observe boot sequence using a serial terminal.

Build Command
~~~~~~~~~~~~~

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
