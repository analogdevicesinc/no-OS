==============================
adrv904x no-OS Sample Project
==============================

Supported Devices
=================

`ADRV9040 <https://www.analog.com/ADRV9040>`_

Supported Carriers
==================

`ZCU102 <https://www.xilinx.com/ZCU102>`_

.. Naming Conventions
.. ==================

.. This guide is a source of information for system engineers and software developers using the Analog Devices, Inc., ADRV904x
.. family of software defined radio transceivers. This family consists of the ADRV9026 integrated quad RF transceiver and the ADRV9029
.. integrated quad RF transceiver with digital predistortion (DPD) and crest factor reduction (CFR) capability.

Prerequisites
=============

Prior to building the project, the environment for the development of Xilinx projects must be set up. These are presented in the *Build Prerequisites* section of no-OS build guide available here: https://wiki.analog.com/resources/no-os/build.

Building and Running the Project
================================

The steps indicated at the following address have to be followed for building the project, for debugging, and running. These steps are adapted to the used platform and host OS: https://wiki.analog.com/resources/no-os/build.

Currently, the project is only intended to be run on the `ZCU102 <https://www.xilinx.com/ZCU102>`_ target, so the Xilinx procedure is to be followed.

1. Open a terminal and navigate to this project directory (if building on Windows, `Git Bash` has to be used).
2. Type ``make -j`` in order to build the project.
3. Increase the HEAP size to 0x800000
4. Add the math library to the gcc linker

The last two steps are performed in the Vitis IDE.

Make sure to connect your adrv904x evaluation board to the correct FMC connector (FMC2 for ZCU102) or the carrier you use before programming it.

Fore more details about the available make rules, check out `this page <https://wiki.analog.com/resources/no-os/make>`_.

Project Layout and HDL Generation
=================================

This is how the `adrv904x no-OS project <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adrv904x>`_ looks like as a file tree:

| no-OS/projects/adrv904x/
| ├── Makefile
| ├── README.rst
| ├── src
| │   ├── common
| │   │   ├── ADRV9040_RxGainTable.h
| │   │   ├── app_config.h
| │   │   ├── common_data.c
| │   │   ├── common_data.h
| │   │   ├── firmware
| │   │   │   ├── ActiveUseCase_profile.h
| │   │   │   ├── ActiveUtilInit_profile.h
| │   │   │   ├── ADRV9040_DFE_CALS_FW.h
| │   │   │   ├── ADRV9040_FW.h
| │   │   │   ├── DeviceProfileTest.h
| │   │   │   └── stream_image.h
| │   │   ├── hal
| │   │   │   ├── no_os_platform.c
| │   │   │   └── no_os_platform.h
| │   │   ├── initdata.c
| │   │   └── initdata.h
| │   ├── examples
| │   │   ├── basic_example
| │   │   │   ├── basic_example.c
| │   │   │   └── basic_example.h
| │   │   ├── dma_example
| │   │   │   ├── dma_example.c
| │   │   │   └── dma_example.h
| │   │   └── iio_example
| │   │       ├── iio_example.c
| │   │       ├── iio_example.h
| │   │       └── iio_example.mk
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

And this is how the corresponding `drivers section <https://github.com/analogdevicesinc/no-OS/tree/master/drivers/rf-transceiver/koror>`_ looks like as a file tree (the Madura API can be found under common and devices directories):

| no-OS/drivers/rf-transceiver/koror/
| ├── adrv904x.c
| ├── adrv904x_conv.c
| ├── adrv904x.h
| ├── common
| │   ├── adi_common.h
| │   ├── adi_common_macros.h
| │   ├── adi_common_types.h
| │   ├── adi_common_user.h
| │   ├── adi_error
| │   │   ├── adi_common_error.c
| │   │   ├── adi_common_error.h
| │   │   └── adi_common_error_types.h
| │   ├── adi_hal
| │   │   ├── adi_common_hal.c
| │   │   └── adi_common_hal.h
| │   └── adi_logging
| │       ├── adi_common_log.c
| │       ├── adi_common_log.h
| │       └── adi_common_log_types.h
| ├── devices
| │   └── adrv904x
| │       ├── multiver
| │       │   ├── adi_adrv904x_all_types.h
| │       │   └── adi_adrv904x_api.h
| │       ├── private
| │       │   ├── bf
| │       │   ├── include
| │       │   └── src
| │       └── public
| │           ├── include
| │           └── src
| ├── platforms
| │   ├── adi_library.c
| │   ├── adi_library.h
| │   ├── adi_library_types.h
| │   ├── adi_platform_aux.c
| │   ├── adi_platform_aux.h
| │   ├── adi_platform.c
| │   ├── adi_platform.h
| │   ├── adi_platform_impl.h
| │   ├── adi_platform_impl_types.h
| │   ├── adi_platform_types.h
| │   └── common
| │       ├── adi_logging.c
| │       ├── adi_logging.h
| │       ├── tls.c
| │       └── tls.h
| └── README.rst


Demo Applications
=================

The demo applications highlight the functionality of the adrv904x evaluation board. Three examples are provided in the sample project:

* Basic Example
* DMA Example
* IIO Example

An example is selected by setting the ``EXAMPLE`` variable in the project Makefile. Below is an example for activating the basic example:

.. code-block:: bash

    EXAMPLE ?= basic_example

Another way of selecting an example is by passing the variable to ``make`` through command line arguments:

.. code-block:: bash

    make EXAMPLE=dma_example

Note that only one example can be enabled at a time. The command line overwrites the value in the Makefile.

Basic Example
-------------

The basic example simply initializes the components on the evaluation board and enables a JESD link. Tx will transmit a DDS waveform with the default parameters set by the DAC driver. The output looks like the one below:

.. code-block:: bash

	rx_adxcvr: Using QPLL with previously defined settings.
	Firmware file: ADRV9040_FW.bin
	Gain Table file: RxGainTable.csv
	Streams file: stream_image.bin
	Device Profile file: DeviceProfileTest.bin
	DFE file: ADRV9040_DFE_CALS_FW.bin
	Radio Sequencer file:

	adrv904x_setup()
	adrv904x-phy Rev 0, API version: 2.10.0.4 found
	adrv904x-device revision: 0xa0

		Using the Profile Init and PostMcsInit Structures
	tx_dac: Successfully initialized (491564941 Hz)
	tx_adxcvr: OK (16220160 kHz)
	rx_adxcvr: OK (16220160 kHz)
	Link0 deframerStatus linkState 0x3
	WARNING: Link0 deframerStatus 0 laneStatus 0xE
	WARNING: Link0 deframerStatus 1 laneStatus 0xE
	WARNING: Link0 deframerStatus 2 laneStatus 0xE
	WARNING: Link0 deframerStatus 3 laneStatus 0xE
	WARNING: Link0 deframerStatus 4 laneStatus 0xE
	WARNING: Link0 deframerStatus 5 laneStatus 0xE
	WARNING: Link0 deframerStatus 6 laneStatus 0xE
	WARNING: Link0 deframerStatus 7 laneStatus 0xE

	adrv904x-phy Rev 160, API version: 2.10.0.4

	tx_jesd status:
		Link is enabled
		Measured Link Clock: 245.782 MHz
		Reported Link Clock: 245.760 MHz
		Lane rate: 16220.160 MHz
		Lane rate / 66: 245.760 MHz
		LEMC rate: 7.680 MHz
		Link status: DATA
		SYSREF captured: Yes
		SYSREF alignment error: No
	rx_jesd status:
		Link is enabled
		Measured Link Clock: 245.782 MHz
		Reported Link Clock: 245.760 MHz
		Lane rate: 16220.160 MHz
		Lane rate / 66: 245.760 MHz
		LEMC rate: 7.680 MHz
		Link status: DATA
		SYSREF captured: Yes
		SYSREF alignment error: No
	ERR: ../adrv904x/src/common/hal/no_os_platform.c:244:no_os_LogWrite(): ERROR:Device Information: Type: 0, ID: 1, Name: ADRV9040
						Recovery Action: 0xFFFFFFF6
						Event Information:
							Error Code: 0x6
							Error Source: 0x1
							Error Message: API Enter Issue
						File Information:
							Line: 574
							Function: adi_adrv904x_Shutdown
							File: 2
						Variable Information:
							Variable Name: 0
							Variable Data: 0x0



DMA Example
-----------

DMA example is a standard example that sends a sinewave on Tx channels using DMA from a lookup table. If you physically loopback a Tx channel to an Rx channel via an electrical wire, you may run the DMA example and read the received data at Rx from its particular memory address.

Select the example through command line arguments as below:

.. code-block:: bash

    make EXAMPLE=dma_example

To run the DMA example, you simply need to run the application as usual by:

* making sure it was built with ``EXAMPLE=dma_example`` (``EXAMPLE ?= dma_example`` in the Makefile or as command line argument for ``make``), as already mentioned
* monitoring the serial terminal for messages printed by the application

After the output from the basic example, the application will eventually print something like this:

.. code-block:: bash

	DMA_EXAMPLE Tx: address=0x434800 samples=8192 channels=16 bits=32
	DMA_EXAMPLE Rx: address=0x43c800 samples=262144 channels=16 bits=16

This means that the memory address where the data at Rx is stored is ``0x43c800``. There are a total of 65536 samples, 16-bit wide across 8 channels, which is equivalent to 8192, 16-bit samples per channel. The location of the transmitted data is also given (0x1dc900).

At this point you may use a Tcl script to retrieve data from memory and store it into .csv files for processing:

.. code-block:: bash

    xsct tools/scripts/platform/xilinx/capture.tcl ZYNQ_PSU 0x43c800 262144 16 16

You can find more information about  the data `here <https://wiki.analog.com/resources/no-os/dac_dma_example>`_.

The data in the .csv files generated can be visualised using the `plot.py <https://github.com/analogdevicesinc/no-OS/blob/master/tools/scripts/platform/xilinx/plot.py>`_ script in the `no-OS repository <https://github.com/analogdevicesinc/no-OS/tree/master>`_. The following command will display the data on all 8 channels:

.. code-block:: bash

    python do_graph.py 16

IIO Example
-----------

IIO demo is a standard example, provided in most `no-OS projects <https://github.com/analogdevicesinc/no-OS/tree/master/projects>`_, that launches a IIOD server on the board so that the user may connect to it via an IIO client. Using iio-oscilloscope, the user can configure the DAC and view the ADC data on a plot.

To build the IIOD demo, add the following flags when invoking make which will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

    make EXAMPLE=iio_example

To run the IIOD demo, first connect to the board via UART to see the runtime output messages with the following settings:

.. code-block:: bash

    Baud Rate: 115200
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
		Baudrate: 921600
		Data size: 8 bits
		Parity: none
		Stop bits: 1
		Flow control: none

This message implies a IIOD server is being run and you may connect to it using a serial-backend enabled `iio-oscilloscope <https://github.com/analogdevicesinc/iio-oscilloscope>`_ and with the settings indicated at the serial terminal.
