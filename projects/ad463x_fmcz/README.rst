AD463x-FMCZ no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-AD4030-24FMCZ <https://www.analog.com/EVAL-AD4030-24>`_
* `EVAL-AD4630-16FMCZ <https://www.analog.com/EVAL-AD4630-16>`_
* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `EVAL-ADAQ4224-FMCZ <https://www.analog.com/ADAQ4224>`_
* `EV-ISO-4224-FMCZ <https://www.analog.com/ADAQ4224>`_

Overview
--------

The EVAL-AD4630-24FMCZ, EVAL-AD4030-24FMCZ, and EVAL-AD4630-16FMCZ
evaluation boards enable quick and easy evaluation of the AD4X3X family
of 24-bit and 16-bit precision successive approximation register (SAR)
analog-to-digital converters (ADCs). The `AD4630-24 <https://www.analog.com/AD4630-24>`_
and `AD4630-16 <https://www.analog.com/AD4630-16>`_ are 2 MSPS per channel,
low power, dual channel 24-bit or 16-bit SAR ADCs, while the
`AD4030-24 <https://www.analog.com/AD4030-24>`_ is a single channel
24-bit precision SAR ADC that supports a sampling rate of up to 2 MSPS.
The evaluation boards demonstrate the performance of either the
AD4630-24, AD4030-24, or AD4630-16 and provide a configurable analog
front end (AFE) for a variety of system applications.

The evaluation boards are designed for use with a ZedBoard.
The ZedBoard is used to control data capture and buffering. The
evaluation board connects to the ZedBoard via a field-programmable
gate array (FPGA) mezzanine card (FMC) low pin count (LPC) connector.

.. note::

	The name ``AD463x`` is used as a generic identifier in the no-OS driver code;
	this driver supports multiple devices in the AD463x family including
	AD4630-24, AD4630-20, AD4630-16, AD4631-24, AD4631-20, AD4631-16,
	AD4632-24, AD4632-20, AD4632-16, AD4030, ADAQ4216, and ADAQ4224.

Applications
------------

* Automatic test equipment
* Digital control loops
* Medical instrumentation
* Seismology
* Semiconductor manufacturing
* Scientific instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4630-24FMCZ evaluation board derives its primary 12V power
supply directly from the ZedBoard via the FMC connector. This 12V input
is first converted to a 7.5V intermediate voltage by a switching
regulator before being post-regulated to provide various required
voltage rails. Moreover, the 12V is instrumental in generating negative
rail voltages, such as the -3.3V needed for the buffers and drive
amplifiers, ensuring precise operational amplifier functionality.

============  =========================================  ========  ========
Power Supply  Function                                   Min. (V)  Max. (V)
============  =========================================  ========  ========
\+12V         12V primary supply via FMC connector       N/A       N/A
GND           Ground connection                          N/A       N/A
\+3.3V        3.3V for various digital logic             3.26      3.33
\+1.8V        1.8V for the ADC                           1.77      1.81
VIO           1.8V supply for the ADC digital I/O        1.77      1.81
\+5.4V        5.4V for the ADC                           5.34      5.46
REFIN         5V ADC reference input                     4.95      5.05
VAMP+         Positive supply for the amplifiers         6.35      6.5
VAMP-         Negative supply for the amplifiers         -3.35     -3.28
VP1           7.5V at the input of the switcher          7.425     7.575
REF           5V at the ADC reference output             4.95      5.05
EN            1.8V enable signal for power supplies      1.75      1.85
============  =========================================  ========  ========

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/platform>`_

Basic Example
~~~~~~~~~~~~~

The basic example code demonstrates the initialization, configuration,
and data acquisition for AD463x series ADCs. The process begins with the
``basic_example_main`` function, which initializes the ADC using
``ad463x_init``. If the ADAQ4224 is used, gain settings are configured
via a scale table with ``ad463x_set_pgia_gain``. The function then exits
the register configuration mode using ``ad463x_exit_reg_cfg_mode``. ADC
data is read into a buffer with ``ad463x_read_data`` and sign-extended
with ``no_os_sign_extend32``. Finally, data for two channels is printed
to the console, providing a clear demonstration of ADC data handling.
This example includes error management and cleanup operations for robust
execution.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad463x_fmcz/Makefile>`_:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO Example
~~~~~~~~~~~

The IIO example code for the ad463x_fmcz project demonstrates the use of
the Industrial I/O framework to interface with AD463x series ADCs. It
focuses on initializing the ADC device and setting up the IIO interface
through the ``iio_example_main()`` function. This involves allocating
and initializing an ``ad463x_dev`` structure, alongside an
``iio_ad463x`` structure to support IIO functions. The example routinely
streams data from the ADC, handling it via an IIO-configured buffer,
showcasing ADC configuration and data management capabilities using the
IIO framework. It provides a practical implementation of IIO for
enhanced device interfacing, forming a key part of hardware-software
integration within the project.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

In order to build the IIO project, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = n
	IIO_EXAMPLE = y

No-OS Supported Platforms
--------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

**Connections**

*EVAL-AD4630-24FMCZ to ZedBoard*

* Connect the EVAL-AD4630-24FMCZ to the ZedBoard through the FMC LPC connector
  for data and 12V power supply.
* Connect differential signal sources to the input channels via the SMA
  connectors.
* Set the VADJ SELECT jumper to the correct voltage (e.g., **2.5V**) to avoid
  damage.
* Optionally connect an external clock input (up to 100 MHz) via the CLK IN SMA
  connector.

*ZedBoard*

* Configure the boot jumpers for SD card boot.
* Connect a USB cable to the USB OTG port for communication.
* Connect a power supply to J20/DC Input to power the system.
* Verify the green power LED (LD1/3) is lit after powering up.

**Build Command**

.. code-block:: bash

	cp <SOME_PATH>/system_top.xsa .
	# to delete current build
	make reset
	# to build the project
	make PLATFORM=xilinx
	# to flash the code
	make run
	# to debug the code
	make debug

STM32 Platform
~~~~~~~~~~~~~~~

**Used Hardware**

* `EVAL-AD4630-24FMCZ <https://www.analog.com/EVAL-AD4630-24>`_
* `EVAL-SDP-CK1Z (SDP-K1) <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/sdp-k1.html>`_

**Connections**

* Align and mate the EVAL-AD4630-24FMCZ with the SDP-K1 120-pin FMC connector.
  The evaluation board receives +12V, +3.3V, and +18V through the connector
  interface.
* Connect a 7V to 12V DC power supply (center positive, min 300 mA, max 3.5 A)
  to the SDP-K1 DC jack.
* Set the VIO_ADJUST header to the correct voltage (1.8V or 3.3V) before
  powering to avoid damage.
* Verify the SYS_PWR LED lights up to indicate proper power.
* Connect a USB standard-A to mini-B cable from the SDP-K1 USB mini-B port to
  the PC.

**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project
	make PLATFORM=stm32
	# to flash the code
	make run
	# to debug the code
	make debug
