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
potential IIO support. The file ``headless.c`` handles hardware
initialization, RX/TX channel configuration, and DMA management,
allowing for autonomous signal processing in a no-OS environment.

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

Please see: `No-OS Build Guide <https://analogdevicesinc.github.io/no-OS/build_guide.html>`__

Project Layout and HDL Generation
---------------------------------

This is how the `adrv9001 no-OS project <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv9001>`__ looks
like as a file tree:

::

   no-OS/projects/adrv9001/
   ├── Makefile
   ├── README.rst
   ├── src
   │   ├── app
   │   │   ├── headless.c
   │   │   ├── Navassa_CMOS_profile.h
   │   │   ├── Navassa_LVDS_profile.h
   │   │   ├── ORxGainTable.h
   │   │   ├── RxGainTable_GainCompensated.h
   │   │   ├── RxGainTable.h
   │   │   └── TxAttenTable.h
   │   ├── firmware
   │   │   ├── Navassa_EvaluationFw.h
   │   │   └── Navassa_Stream.h
   │   └── hal
   │       ├── adi_platform.h
   │       ├── adi_platform_types.h
   │       ├── no_os_platform.c
   │       ├── no_os_platform.h
   │       └── parameters.h
   ├── src.mk
   └── system_top.xsa

Note the presence of the system_top.xsa. In order to build this project, you
need such an .xsa file present in the project directory, as shown above.
In case you don't have one, either obtain a pre-built file or build it yourself
by following the `Building HDL guide <https://analogdevicesinc.github.io/hdl/user_guide/build_hdl.html>`__.

And this is how the corresponding `driver folder <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/rf-transceiver/navassa>`__
looks like as a file tree (the Navassa API can be found under common and devices
directories):

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

   * Copy the generated unsigned char array to the correspoding header file in
     the project's `firmware folder <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/src/firmware/>`__.

#. The profile file also has to be transformed for being included in the project:

   * Generate string literals from the json files using the
     `json2cstring.sh script <https://github.com/analogdevicesinc/no-OS/tree/main/tools/scripts/platform/xilinx>`__:
     ::

       ./json2cstring path/TES_generated_profile.json

   * Copy the contents of the generated file to the corresponding header file
     in the file used as the current profile (CMOS or LVDS profile selected at build
     time using make ``LVDS=y`` for LVDS, or omitting the flag for the default CMOS
     profile).

#. Modify the code in the project so that the new settings are correctly used
   (e.g., `headless.c <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv9001/src/app/headless.c>`__).

#. Build the project.

No-OS Supported Examples
-------------------------

The demo applications highlight the functionality of the ADRV9002
evaluation board. Two examples are provided in the sample project:

- DMA Example
- IIO Example

DMA Example
~~~~~~~~~~~~

DMA example is a standard example that sends a sinewave on Tx channels using DMA
from a lookup table. If you physically loopback a Tx channel to an Rx channel
via an electrical wire, you may run the DMA_EXAMPLE and read the received data
at Rx from its particular memory address.

When DMA_EXAMPLE is disabled, a small LO leakage spike at the carrier frequency
is normal.
When DMA_EXAMPLE is enabled, a 1024-sample sine wave is transmitted in a continuous
loop via cyclic DMA. The resulting spectrum shows equidistant spectral lines rather
than a single clean tone, which is expected due to the repeating finite-length
waveform.

The purpose of DMA_EXAMPLE is to verify that the TX and RX data paths — from memory
through the DMA controller to the RF front-end and back — are functional. It is
not designed to produce a spectrally pure output.

Select the example through command line arguments as below:

.. code-block:: bash

    make IIOD=n DMA_EXAMPLE=y

To run the DMA example, you simply need to run the application as usual by:

* making sure it was built with the ``DMA_EXAMPLE`` flag (``DMA_EXAMPLE=y`` as
  command line argument for ``make``), as already mentioned,
* monitoring the serial terminal for messages printed by the application.

After the output from the basic example, the application will eventually print
something like this:

.. code-block:: bash

	DMA_EXAMPLE Rx2: address=0x105000 samples=32768 channels=2 bits=16
	DMA_EXAMPLE Rx1: address=0xf5000 samples=32768 channels=2 bits=16

This means that the memory addresses where the data at Rx are stored are ``0x105000``
and ``0xf5000``. There are a total of 65536 (2x32768) samples, 16-bit wide across 4 channels,
which is equivalent to 32768, 16-bit samples per channel.

At this point you may use a Tcl script to retrieve data from memory and store it
into .csv files for processing:

.. code-block:: bash

    # Use the address printed by the application for DMA_EXAMPLE Rx1
    # Loopback between Tx and Rx required
    xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU <address> 32768 2 16

You can find more information about  the data `here <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

The data in the .csv files generated can be visualised using the `plot.py <https://github.com/analogdevicesinc/no-OS/blob/master/tools/scripts/platform/xilinx/plot.py>`_ script in the `no-OS repository <https://github.com/analogdevicesinc/no-OS/tree/master>`_.
The following command will display the data on 2 channels:

.. code-block:: bash

    python tools/scripts/platform/xilinx/plot.py 2

IIO Example
~~~~~~~~~~~~

IIO demo is a standard example, provided in most `no-OS projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches a IIOD server on the board so that the user may connect to it via
an IIO client. Using iio-oscilloscope, the user can configure the DAC and view
the ADC data on a plot.

To build the IIOD demo, add the following flags when invoking make which will
build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   make IIOD=y

To run the IIOD demo, first connect to the board via UART to see the runtime
output messages with the following settings:

.. code-block:: bash

   Baud Rate: 115200bps
   Data: 8 bit
   Parity: None
   Stop bits: 1 bit
   Flow Control: none

Please note that for proper message display, you may need to convert all LF
characters to CRLF, if your serial terminal supports it.

With a serial terminal correctly configured and listening to incoming messages,
launch the application (make run or click the debug button in your SDK).
Runtime messages specific to the application will appear on your serial
terminal screen, and eventually the following message is printed, after the
output from the basic example:

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

This message implies a IIOD server is being run and you may connect to it using
a serial-backend enabled `iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`__ and with the settings
indicated at the serial terminal.

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

**Hardware Setup:**

- Attach the ADRV9002 Evaluation Board to the ZCU102 through the FMC
  HPC0 connector.

- Enable SD Card BOOT mode by switching SW6 to:

    ====== =====
    Switch State
    SW6-1  ON
    SW6-2  ON
    SW6-3  ON
    SW6-4  ON
    ====== =====

- Connect a micro-USB cable to the ZCU102 board via the JTAG connector (J2),
  and the other end of the cable to the Host PC.

- Connect a micro-USB cable to the ZCU102 board via the USB-UART
  connector (J83), and the other end of the cable to the Host PC.

- Connect the power adapter to the ZCU102 board. via the power supply
  plug (J52).

- Switch on the ZCU102 board using the SW1 switch.

- Program the board via JTAG.

- Observe application messages using a serial terminal.

Alternatively, a microSD card containing the BOOT.bin file generated during build
can be used for running the ``no-OS`` application.

Build Command
~~~~~~~~~~~~~

.. code-block:: bash

   cp <SOME_PATH>/no-OS/projects/adrv9001/system_top.xsa .
   # go to project folder
   cd <SOME_PATH>/no-OS/projects/adrv9001/
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run

The following build flags are supported:

.. list-table::
   :header-rows: 1

   * - Flag
     - Default
     - Description
   * - ``IIOD=y``
     - ``n``
     - Enable IIO daemon for use with IIO Oscilloscope
   * - ``DMA_EXAMPLE=y``
     - ``n``
     - Enable the DMA loopback example (sine wave TX, capture RX)
   * - ``LVDS=y``
     - ``n``
     - Use the LVDS SSI profile instead of the default CMOS profile
   * - ``RX2TX2=y``
     - ``n``
     - Enable RX2TX2 mode (combined RX1+RX2 and TX1+TX2 AXI cores)

Example:

.. code-block:: bash

   make DMA_EXAMPLE=y LVDS=y
