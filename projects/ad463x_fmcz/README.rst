AD463x-FMCZ no-OS Example Project
=================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD4030-24FMCZ`
- :adi:`EVAL-AD4630-16FMCZ`
- :adi:`EVAL-AD4630-24FMCZ`
- :adi:`EVAL-ADAQ4224-FMCZ`
- :adi:`EV-ISO-4224-FMCZ`

Overview
--------

The EVAL-AD4630-24FMCZ, EVAL-AD4030-24FMCZ, and EVAL-AD4630-16FMCZ
evaluation boards enable quick and easy evaluation of the AD4X3X family
of 24-bit and 16-bit precision successive approximation register (SAR)
analog-to-digital converters (ADCs). The :adi:`AD4630-24` and
:adi:`AD4630-16` are 2 MSPS per channel, low power, dual channel 24-bit 
or 16-bit SAR ADCs, while the :adi:`AD4030-24` is a single channel 
24-bit precision SAR ADC that supports a sampling rate of up to 2 MSPS. 
The evaluation boards demonstrate the performance of either the
AD4630-24, AD4030-24. or AD4630-16 and provides a configurable analog
front end (AFE) for a variety of system applications.

The evaluations board are designed for use with a ZedBoard.
The ZedBoard is used to control data capture and buffering. The
evaluation board connects to the ZedBoard board via a field-programmable
gate array (FPGA) mezzanine card (FMC) low pin count (LPC) connector.

**Note:**

The name ``AD463x`` is used as a generic identifier in the no-OS driver code; 
this driver supports multiple devices in the AD463x family including 
AD4630-24, AD4630-20, AD4630-16, AD4631-24, AD4631-20, AD4631-16, 
AD4632-24, AD4632-20, AD4632-16, AD4030, ADAQ4216, and ADAQ4224.

Applications
------------

- Automatic test equipment
- Digital control loops
- Medical instrumentation
- Seismology
- Semiconductor manufacturing
- Scientific instrumentation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4630-24FMCZ evaluation board derives its primary 12V power
supply directly from the ZedBoard via the FMC connector. This 12V input
is crucial as it is first converted to a 7.5V intermediate voltage by a
switching regulator before being post-regulated to provide various
required voltage rails. Moreover, the 12V is instrumental in generating
negative rail voltages, such as the -3.3V needed for the buffers and
drive amplifiers, ensuring precise operational amplifier functionality.

============ ========================================= ======== ========
Power Supply Function                                  Min. (V) Max. (V)
+12V         12V primary supply via FMC connector      N/A      N/A
GND          Ground connection                         N/A      N/A
+3.3V        3.3V for various digital logic            3.26     3.33
+1.8V        1.8V for the ADC                          1.77     1.81
VIO          1.8V supply for the ADC digital I/O       1.77     1.81
+5.4V        5.4V for the ADC                          5.34     5.46
REFIN        5V ADC reference input                    4.95     5.05
VAMP+        Positive supply for the amplifiers        6.35     6.5
VAMP-        Negative supply for the amplifiers        -3.35    -3.28
VP1          7.5V at the input of the switcher         7.425    7.575
REF          5V at the ADC reference output            4.95     5.05
EN           1.8V enable signal for the power supplies 1.75     1.85
============ ========================================= ======== ========

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the 
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad463x_fmcz/src/platform>`__

Basic example
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
`Makefile: <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad463x_fmcz/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
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
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

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

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD4630-24FMCZ`
- ZedBoard

Connections
^^^^^^^^^^^

**EVAL-AD4030-24FMCZ**

+-----------------------+-----------------------------------------------+
| **Connection**        | **Function**                                  |
+-----------------------+-----------------------------------------------+
| FMC Connector         | Connect to ZedBoard FMC for data and 12V      |
|                       | power supply                                  |
+-----------------------+-----------------------------------------------+
| SMA Connectors        | Connect differential signal sources to input  |
|                       | channels                                      |
+-----------------------+-----------------------------------------------+
| VADJ SELECT Jumper    | Set to correct voltage (e.g., **2.5V**) to    |
|                       | avoid damage                                  |
+-----------------------+-----------------------------------------------+
| CLK IN SMA            | Optional external clock input ≤100 MHz        |
+-----------------------+-----------------------------------------------+


**ZedBoard**     

+-----------------------+---------------------------------------+
| **Connection**        | **Function**                          |
+-----------------------+---------------------------------------+
| Boot Jumpers          | Configure for SD card boot            |
+-----------------------+---------------------------------------+
| USB OTG Port          | Connect to PC for communication and   |
|                       | power                                 |
+-----------------------+---------------------------------------+
| J20/DC Input          | Connect power supply to power the     |
|                       | system                                |
+-----------------------+---------------------------------------+
| LD1/3 POWER LED       | Verify green LED is lit after         |
|                       | powering up                           |
+-----------------------+---------------------------------------+

Build Command
^^^^^^^^^^^^^

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

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD4630-24FMCZ`
- :adi:`EVAL-SDP-CK1Z` (SDP-K1)

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| **Board**             | **Connection Point**  | **Function**          |
+-----------------------+-----------------------+-----------------------+
| EVAL-AD4630-24FMCZ    | FMC Connector         | Align and mate with   |
|                       |                       | SDP-K1’s 120-pin FMC  |
|                       |                       | connector             |
+-----------------------+-----------------------+-----------------------+
| Power via FMC         | Receives +12V, +3.3V, |                       |
|                       | +18V through          |                       |
|                       | connector interface   |                       |
+-----------------------+-----------------------+-----------------------+
| SDP-K1                | DC Jack               | Connect 7V to 12V DC  |
|                       |                       | power supply (center  |
|                       |                       | positive, min 300mA,  |
|                       |                       | max 3.5A)             |
+-----------------------+-----------------------+-----------------------+
| VIO_ADJUST Header     | Set to correct        |                       |
|                       | voltage (1.8V or      |                       |
|                       | 3.3V) before powering |                       |
|                       | to avoid damage       |                       |
+-----------------------+-----------------------+-----------------------+
| SYS_PWR LED           | Should light up to    |                       |
|                       | indicate proper power |                       |
+-----------------------+-----------------------+-----------------------+
| USB Mini-B Port       | Connect to PC using   |                       |
|                       | USB standard-A to     |                       |
|                       | mini-B cable          |                       |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=stm32
   # to flash the code
   make run
   # to debug the code
   make debug
