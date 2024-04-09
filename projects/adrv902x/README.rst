============================
adrv902x no-OS Sample Poject
============================

Supported Devices
=================

`ADRV9026 <https://www.analog.com/ADRV9026>`_

`ADRV9029 <https://www.analog.com/ADRV9029>`_

Supported Carriers
==================

`ZCU102 <https://www.xilinx.com/ZCU102>`_

Naming Conventions
==================

This guide is a source of information for system engineers and software developers using the Analog Devices, Inc., ADRV902x
family of software defined radio transceivers. This family consists of the ADRV9026 integrated quad RF transceiver and the ADRV9029
integrated quad RF transceiver with digital predistortion (DPD) and crest factor reduction (CFR) capability.

Prerequisites
=============

Prior to building the project, the environment for the development of Xilinx projects must be set up. These are presented in the *Build Prerequisites* section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build.

Building and Running the Project
================================

The steps indicated at the following address have to be followed for building the project, for debugging, and running. These steps are adapted to the used platform and host OS: https://wiki.analog.com/resources/no-os/build.

Currently, the project is only intended to be run on the `ZCU102 <https://www.xilinx.com/ZCU102>`_ target, so the Xilinx procedure is to be followed.

1. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` has to be used).
2.  Type ``make -j`` in order to build the project.

A successful build should end with the following terminal output:

.. code-block:: bash

	[23:38:48] Creating BOOT.BIN and archive with files
	text	   data	    bss	    dec	    hex	filename
	1213864	 368588	1239608	2822060	 2b0fac	/home/george/max_workspace/no-OS/projects/adrv902x/build/adrv902x.elf
	[23:36:50] Done (build/adrv902x.elf)

Make sure to connect your adrv902x evaluation board to the correct FMC connector or the carrier you use before programming it.

Fore more details about the available make rules, check out `this page <https://wiki.analog.com/resources/no-os/make>`_.

Project Layout and HDL Generation
=================================

This is how the `adrv902x no-OS project <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv902x>`_ looks like as a file tree:

| no-OS/projects/adrv902x/
| ├── Makefile
| ├── README.rst
| ├── src
| │   ├── common
| │   │   ├── ADRV9025_RxGainTable.h
| │   │   ├── ADRV9025_TxAttenTable.h
| │   │   ├── app_config.h
| │   │   ├── common_data.c
| │   │   ├── common_data.h
| │   │   ├── firmware
| │   │   │   ├── ActiveUseCase_profile.h
| │   │   │   ├── ActiveUtilInit_profile.h
| │   │   │   ├── ADRV9025_DPDCORE_FW.h
| │   │   │   ├── ADRV9025_FW.h
| │   │   │   └── stream_image_6E3E00EFB74FE7D465FA88A171B81B8F.h
| │   │   └── hal
| │   │       ├── no_os_platform.c
| │   │       └── no_os_platform.h
| │   ├── examples
| │   │   ├── basic_example
| │   │   │   ├── basic_example.c
| │   │   │   └── basic_example.h
| │   │   ├── dma_example
| │   │   │   ├── dma_example.c
| │   │   │   └── dma_example.h
| │   │   ├── examples_src.mk
| │   │   └── iio_example
| │   │       ├── iio_example.c
| │   │       └── iio_example.h
| │   ├── LICENSE_API
| │   ├── LICENSE_FW
| │   └── platform
| │       ├── platform_includes.h
| │       └── xilinx
| │           ├── main.c
| │           ├── parameters.c
| │           ├── parameters.h
| │           └── platform_src.mk
| ├── src.mk
| └── system_top.xsa

Note the presence of the system_top.xsa. In order to build this `no-OS <https://wiki.analog.com/resources/no-os>`_ project, you need such an .xsa file present in the project directory, as shown above. In case you don't have one, either obtain a pre-built file or build it yourself by following the `Building HDL guide <https://wiki.analog.com/resources/fpga/docs/build>`_.

And this is how the corresponding `drivers section <https://github.com/analogdevicesinc/no-OS/tree/main/drivers/rf-transceiver/madura>`_ looks like as a file tree (the Madura API can be found under common and devices directories):

| no-OS/drivers/rf-transceiver/madura/
| ├── adrv9025.c
| ├── adrv9025_conv.c
| ├── adrv9025.c.orig
| ├── adrv9025.h
| ├── common
| │   ├── adi_common.h
| │   ├── adi_common_macros.h
| │   ├── adi_common_types.h
| │   ├── adi_common_user.h
| │   ├── adi_error
| │   ├── adi_halMake sure to connect your adrv9002 evaluation board to the correct FMC connector or the carrier you use: 
| │   └── adi_logging
| ├── devices
| │   └── adrv9025
| └── platforms
|     ├── adi_platform.h
|     └── adi_platform_types.h

Switching Between Use Cases
===========================

When the implementation of a different use case than the one in the project folder is desired, the following steps have to be followed:

1. From the Madura TES GUI, generate the resources folder that contains the files listed below:

* Firmware files (ADRV9025_FW.bin and ADRV9025_DPDCORE_FW.bin),

* Stream binary (e.g., stream_image_6E3E00EFB74FE7D465FA88A171B81B8F.bin),

* ActiveUseCase.profile and ActiveUtilInit.profile.

2. Since no-OS does not have mechanisms for manipulating files, create a hex dump for each .bin file. As can be seen in the project structure, these are added as header files to the project.

* Use the following command for storing the hex dump in a file::

	xxd -i ADRV9025_FW.bin > ADRV9025_FW.h

* Copy the generated unsigned char array to the correspoding header file in the `project structure <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv902x/src/common/firmware>`_ (ADRV9025_FW.bin, ADRV9025_DPDCORE_FW.bin or stream_image_x.bin).

3. Profile files also have to be transformed for being included in the project:

* Generate string literals from the json files using the `json2cstring.sh <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv902x/json2cstring.sh>`_ script in the `no-OS project <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv902x>`_::

	./json2cstring path/ActiveUseCase.profile

* Copy the contents of the generated files to the correspoding header files in the `project structure <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv902x/src/common/firmware>`_ (ActiveUseCase_profile.h and ActiveUtilInit_profile.h).

4. Modify the code in the project so that the new settings are correctly used (e.g., `app_config.h <https://github.com/analogdevicesinc/no-OS/blob/main/projects/adrv902x/src/common/app_config.h>`_).

5. Build the project.

Demo Applications
=================

The demo applications highlight the functionality of the adrv902x evaluation board. Three examples are provided in the sample project:

* Basic Example
* DMA Example
* IIO Example

An example is selected by specifying ``y`` as the value of the corresponding variable in the project Makefile. Below is an example for activating the basic example:

.. code-block:: bash

    BASIC_EXAMPLE = y
    DMA_EXAMPLE = n
    IIO_EXAMPLE = n

Another way of selecting a demo is by passing variables to ``make`` through command line arguments:

.. code-block:: bash

    make DMA_EXAMPLE=y IIO_EXAMPLE=n

Note that only one example can be enabled at a time. Therefore, when using command line arguments, the settings in the Makefile should be taken into account. The command line overwrites the values in the Makefile.

Basic Example
-------------

The basic example simply initializes the components on the evaluation board and enables a JESD link. Tx will transmit a DDS waveform with the default parameters set by the DAC driver. The output looks like the one below:

.. code-block:: bash

    adrv9025-phy Rev 0, API version: 6.4.0.14 found
    tx_adxcvr: OK (9830400 kHz)
    rx_adxcvr: OK (9830400 kHz)
    adrv9025-phy Rev 176, Firmware 6.4.0.6 API version: 6.4.0.14 Stream version: 9.4.0.1 successfully initialized via jesd204-fsm
    tx_jesd status:
        Link is enabled
        Measured Link Clock: 245.778 MHz
        Reported Link Clock: 245.760 MHz
        Lane rate: 9830.400 MHz
        Lane rate / 40: 245.760 MHz
        LMFC rate: 7.680 MHz
        SYNC~: deasserted
        Link status: DATA
        SYSREF captured: Yes
        SYSREF alignment error: No
    rx_jesd status:
        Link is enabled
        Measured Link Clock: 245.778 MHz
        Reported Link Clock: 245.760 MHz
        Lane rate: 9830.400 MHz
        Lane rate / 40: 245.760 MHz
        LMFC rate: 7.680 MHz
        Link status: DATA
        SYSREF captured: Yes
        SYSREF alignment error: No
    Bye

DMA Example
-----------

DMA example is a standard example that sends a sinewave on Tx channels using DMA from a lookup table. If you physically loopback a Tx channel to an Rx channel via an electrical wire, you may run the DMA_EXAMPLE and read the received data at Rx from its particular memory address.

Select the example through command line arguments as below:

.. code-block:: bash

    make BASIC_EXAMPLE=n DMA_EXAMPLE=y IIO_EXAMPLE=n

To run the DMA example, you simply need to run the application as usual by:

* making sure it was built with the ``DMA_EXAMPLE`` flag (``DMA_EXAMPLE = y`` in the Makefile or as command line argument for ``make``), as already mentioned
* monitoring the serial terminal for messages printed by the application

After the output from the basic example, the application will eventually print something like this: 

.. code-block:: bash

    DMA_EXAMPLE Tx: address=0x1dc900 samples=8192 channels=8 bits=32
    DMA_EXAMPLE Rx: address=0x1e4900 samples=65536 channels=8 bits=16

This means that the memory address where the data at Rx is stored is ``0x1e4900``. There are a total of 65536 samples, 16-bit wide across 8 channels, which is equivalent to 8192, 16-bit samples per channel. The location of the transmitted data is also given (0x1dc900).

At this point you may use a Tcl script to retrieve data from memory and store it into .csv files for processing:

.. code-block:: bash

    xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU 0x1e4900 65536 8 16

You can find more information about  the data `here <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

The data in the .csv files generated can be visualised using the `plot.py <https://github.com/analogdevicesinc/no-OS/blob/main/tools/scripts/platform/xilinx/plot.py>`_ script in the `no-OS repo <https://github.com/analogdevicesinc/no-OS>`_. The following command will display the data on all 8 channels:

.. code-block:: bash

    python do_graph.py 8

IIO Example
-----------

IIO demo is a standard example, provided in most `no-OS projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`_, that launches a IIOD server on the board so that the user may connect to it via an IIO client. Using iio-oscilloscope, the user can configure the DAC and view the ADC data on a plot.

To build the IIOD demo, add the following flags when invoking make which will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

    make BASIC_EXAMPLE=n DMA_EXAMPLE=n IIO_EXAMPLE=y

To run the IIOD demo, first connect to the board via UART to see the runtime output messages with the following settings: 

.. code-block:: bash

    Baud Rate: 115200bps
    Data: 8 bit
    Parity: None
    Stop bits: 1 bit
    Flow Control: none

Please note that for proper message display, you may need to convert all LF characters to CRLF, if your serial terminal supports it.

With a serial terminal correctly configured and listening to incoming messages, launch the application (make run or click the debug button in your SDK). Runtime messages specific to the application will apear on your serial terminal screen, and eventually the following message is printed, after the output from the basic example:

.. code-block:: bash

    Running IIOD server...
    If successful, you may connect an IIO client application by:
    1. Disconnecting the serial terminal you use to view this message.
    2. Connecting the IIO client application using the serial backend configured as shown:
        Baudrate: 115200
        Data size: 8 bits
        Parity: none
        Stop bits: 1
        Flow control: none

This message implies a IIOD server is being run and you may connect to it using a serial-backend enabled `iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`_ and with the settings indicated at the serial terminal.
