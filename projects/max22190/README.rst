MAX22190 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX22190EVKIT <https://www.analog.com/MAX22190EVKIT>`_

Overview
--------

The `MAX22190 <https://www.analog.com/MAX22190>`_ is an octal industrial digital
input device with serializer and diagnostics. It translates eight 24 V
industrial inputs into serialized SPI-compatible data and complies with
IEC 61131-2 Type 1, 2, and 3 input standards. Each input includes a
programmable glitch filter with independently configurable delay values
between 50 us and 20 ms, or filter bypass.

The MAX22190 field-side accepts a single 7 V to 65 V supply on the VDD24 pin.
When powered by the field supply, an integrated LDO regulator generates a
3.3 V output capable of providing up to 25 mA for external loads. Alternatively,
the device can be powered from a 3.0 V to 5.5 V logic-side supply connected to
VDD. The SPI interface operates at 3.3 V or 5 V logic levels as controlled by
the VL pin.

Built-in diagnostics include wire-break detection, field-supply voltage
monitoring, overtemperature monitoring, and 5-bit CRC error detection on SPI.

Applications
------------

* Programmable Logic Controllers
* Industrial Automation
* Process Automation
* Building Automation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Field-side supply (VDD24): 7 V to 65 V
* Logic-side supply (VDD): 3.0 V to 5.5 V (alternative to VDD24)
* LDO output: 3.3 V, up to 25 mA for external loads
* SPI logic levels (VL): 3.3 V or 5 V

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22190/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22190/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the MAX22190 device via SPI and demonstrates
reading digital input states across all eight channels. It also configures
channel filter delays, manages channel states, and exercises the onboard
diagnostics by enabling fault detection and wire-break detection.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max22190/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example initializes the MAX22190 IIO device descriptor and launches
an IIO application server, allowing real-time interaction with the device
channels through IIO clients such as the IIO Oscilloscope.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max22190/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX22190EVKIT
* AD-APARD32690-SL (MAX32690)

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
