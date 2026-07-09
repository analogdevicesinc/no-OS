ADRV9001 no-OS Example Project
===============================

**Note:**
The ADRV9001 name in this document refers collectively to the
ADRV9002, ADRV9003, and ADRV9004 device family. Within the document,
**ADRV9001** may be used interchangeably to address any of these specific
models.

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Devices
-----------------

* `ADRV9002NP/W2/PCBZ <https://www.analog.com/EVAL-ADRV9002>`__ (Evaluation Board for ADRV9002)

Supported Carriers
------------------

* **ADRV9002** and **ZCU102** using **FMC HPC0** slot
* **ADRV9002** and **ZC706** using **FMC LPC** slot
* **ADRV9002** and **ZedBoard** using **FMC LPC** slot

Overview
--------

The ADRV9001 no-OS project enables integration of the ADRV9001 series
(ADRV9002, ADRV9003, and ADRV9004) into embedded systems without
requiring an operating system. It employs a hardware abstraction layer
to manage SPI communication, GPIO functions, and platform-specific
settings, implemented in ``no_os_platform.c`` and related headers.
Designed to work with Xilinx hardware platforms such as ZCU102 and
ZedBoard, the project supports AXI ADC/DAC cores, DMA-based data
movement, and optional IIO server support. The ``headless.c`` file
handles hardware initialization, RX/TX channel configuration, and DMA
management for autonomous signal processing in a no-OS environment.

Applications
------------

* Mission critical communications
* Very high frequency (VHF) applications
* Ultrahigh frequency (UHF) applications
* Cellular applications from 30 MHz to 6000 MHz
* Time Division Duplexing (TDD) applications
* Frequency Division Duplexing (FDD) applications

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://analogdevicesinc.github.io/no-OS/build_guide.html>`__

Driver Layout
-------------

The Navassa API is located in the no-OS driver directory under:

::

   no-OS/drivers/rf-transceiver/navassa/
   ├── adrv9002.c
   ├── adrv9002_conv.c
   ├── adrv9002.h
   ├── common
   │   ├── adi_common_error.c
   │   ├── adi_common_error.h
   │   ├── adi_common_error_types.h
   │   ├── adi_common.h
   │   ├── adi_common_hal.h
   │   ├── adi_common_hal_wrapper.c
   │   ├── adi_common_hal_wrapper.h
   │   ├── adi_common_log.c
   │   ├── adi_common_log.h
   │   ├── adi_common_macros.h
   │   └── adi_common_types.h
   ├── devices
   │   └── adrv9001
   │       ├── private
   │       │   ├── include
   │       │   └── src
   │       └── public
   │           ├── include
   │           └── src
   └── third_party
       ├── adi_pmag_macros
       └── jsmn

Switching Between Use Cases
---------------------------

When the implementation of a different use case than the one in the project
folder is desired, the following steps have to be followed:

#. Implement the desired settings in ADRV9001 TES GUI and generate the files listed below:

   * Stream binary (e.g., TES_generated_stream.bin), by clicking on File->Generate Stream Image;
   * Profile JSON (e.g., TES_generated_profile.json), by clicking on File->Generate Profile File.

#. Since no-OS does not have mechanisms for manipulating files, create a hex dump for the stream .bin file. As can be seen in the project structure, the .bin files are added as header files to the project in the 'firmware' folder.

   * Use the following command for storing the hex dump in a file:
     ::

        xxd -i TES_generated_stream.bin > Navassa_Stream.h

   * Copy the generated unsigned char array to the corresponding header file in
     the project's `firmware folder <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/src/firmware/>`__.

#. The profile file also has to be transformed for being included in the project:

   * Generate string literals from the json files using the
     `json2cstring.sh script <https://github.com/analogdevicesinc/no-OS/tree/main/tools/scripts/platform/xilinx>`__:
     ::

       ./json2cstring path/TES_generated_profile.json

   * Copy the contents of the generated file to the corresponding header file
     in the file used as the current profile (CMOS or LVDS profile). The
     ``demo`` variant uses the CMOS profile; the ``demo_adrv9002`` variant
     uses the LVDS profile.

#. Modify the code in the project so that the new settings are correctly used
   (e.g., `headless.c <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/src/app/headless.c>`__).

#. Build the project.

No-OS Supported Examples
-------------------------

The demo applications highlight the functionality of the ADRV9002
evaluation board. The following variants are provided:

Demo Example
~~~~~~~~~~~~

Basic initialization and bring-up for the ADRV9001/ADRV9002 transceiver
in standard 2-channel (1T1R) mode via AXI ADC/DAC cores and DMA.

Demo ADRV9002 (RX2TX2) Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Basic initialization and bring-up with ADRV9002 in RX2TX2 mode
(4-channel), enabling both TX/RX channel pairs simultaneously. Uses the
LVDS SSI profile instead of the default CMOS profile.

DMA Example
~~~~~~~~~~~

DMA example sends a sine wave on TX channels using DMA from a lookup
table. If you physically loopback a TX channel to an RX channel via an
electrical wire, you may run the DMA example and read the received data
at RX from its particular memory address.

After the output from the basic example, the application will eventually
print something like this:

.. code-block:: bash

    DMA_EXAMPLE Rx2: address=0x105000 samples=32768 channels=2 bits=16
    DMA_EXAMPLE Rx1: address=0xf5000 samples=32768 channels=2 bits=16

This means that the memory addresses where the data at RX are stored are
``0x105000`` and ``0xf5000``. There are a total of 65536 (2x32768) samples,
16-bit wide across 4 channels, which is equivalent to 32768, 16-bit samples
per channel.

At this point you may use a Tcl script to retrieve data from memory and store
it into .csv files for processing:

.. code-block:: bash

    # Use the address printed by the application for DMA_EXAMPLE Rx1
    # Loopback between Tx and Rx required
    xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU <address> 32768 2 16

You can find more information about the data `here <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

The data in the .csv files generated can be visualised using the
`plot.py <https://github.com/analogdevicesinc/no-OS/blob/master/tools/scripts/platform/xilinx/plot.py>`_
script in the no-OS repository. The following command will display the data
on 2 channels:

.. code-block:: bash

    python tools/scripts/platform/xilinx/plot.py 2

IIO Example
~~~~~~~~~~~

IIO demo is a standard example that launches a IIOD server on the board so
that the user may connect to it via an IIO client. Using iio-oscilloscope,
the user can configure the DAC and view the ADC data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
:dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

To run the IIOD demo, connect to the board via UART with the following
settings:

.. code-block:: bash

   Baud Rate: 115200bps
   Data: 8 bit
   Parity: None
   Stop bits: 1 bit
   Flow Control: none

Please note that for proper message display, you may need to convert all LF
characters to CRLF if your serial terminal supports it.

After the ADRV9001 initialization messages, the application will print:

.. code-block:: bash

    Running IIOD server...
    If successful, you may connect an IIO client application by:
    1. Disconnecting the serial terminal you use to view this message.
    2. Connecting the IIO client application using the serial backend configured as shown:
        Baudrate: 921600
        Data size: 8 bits
        Parity: none
        Stop bits: 1
        Flow control: none

This message implies a IIOD server is being run and you may connect to it
using a serial-backend enabled
`iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`__
with the settings indicated at the serial terminal.

IIO ADRV9002 (RX2TX2) Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

IIO example with ADRV9002 in RX2TX2 mode (4-channel). Exposes both TX/RX
channel pairs through the IIO framework over UART. Uses the same IIOD
serial settings as the IIO example above.

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ADRV9002NP/W2/PCBZ <https://www.analog.com/EVAL-ADRV9002>`__ (Evaluation Board for ADRV9002)
* `ZCU102 Evaluation Kit <https://www.xilinx.com/ZCU102>`__ (Zynq UltraScale+ MPSoC)

Connections
^^^^^^^^^^^

Attach the ADRV9002 Evaluation Board to the ZCU102 through the FMC HPC0
connector.

Enable SD Card BOOT mode by switching SW6 to:

====== =====
Switch State
SW6-1  ON
SW6-2  ON
SW6-3  ON
SW6-4  ON
====== =====

Connect a micro-USB cable to the ZCU102 board via the JTAG connector (J2),
and the other end of the cable to the Host PC.

Connect a micro-USB cable to the ZCU102 board via the USB-UART connector
(J83), and the other end of the cable to the Host PC.

Connect the power adapter to the ZCU102 board via the power supply plug
(J52). Switch on the ZCU102 board using the SW1 switch.

The serial console appears at 115200 baud, 8N1 (921600 for the IIOD
protocol in IIO variants).

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``,
``demo_adrv9002``, ``dma_example``, ``iio``, ``iio_adrv9002``.
Available boards: ``zcu102``, ``zcu102_lvds``.

A Xilinx XSA hardware description file is required. Pass its path via
the ``--hardware`` flag (local dev) or set the ``HARDWARE`` environment
variable. The HDL design name is ``adrv9001``; the hardware name is
composed as ``adrv9001_<board>`` (e.g. ``adrv9001_zcu102``).

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (adjust path to your installation)
   source /path/to/Vitis/2025.1/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "C:\path\to\Vitis\2025.1\settings64.bat"

   cd no-OS

   # Build the demo example for ZCU102 (supply your own .xsa)
   python tools/scripts/no_os_build.py build \
       --project adrv9001 --variant demo --board zcu102 \
       --hardware /path/to/adrv9001_zcu102/system_top.xsa

   # Build the DMA example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv9001 --variant dma_example --board zcu102 \
       --hardware /path/to/adrv9001_zcu102/system_top.xsa

   # Build the IIO example for ZCU102
   python tools/scripts/no_os_build.py build \
       --project adrv9001 --variant iio --board zcu102 \
       --hardware /path/to/adrv9001_zcu102/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project adrv9001 --variant demo --board zcu102 \
       --hardware /path/to/adrv9001_zcu102/system_top.xsa \
       --probe openocd --flash

   # LVDS SSI profile — use zcu102_lvds board
   python tools/scripts/no_os_build.py build \
       --project adrv9001 --variant demo_adrv9002 --board zcu102_lvds \
       --hardware /path/to/adrv9001_zcu102_lvds/system_top.xsa
