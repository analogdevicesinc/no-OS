MAX22196 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX22196EVKIT <https://www.analog.com/MAX22196EVKIT>`_

Overview
--------

The `MAX22196 <https://www.analog.com/MAX22196>`_ is an industrial octal digital
input that translates eight industrial 24 V or TTL level inputs to logic level
outputs. The device has a serial interface allowing configuration and reading
of serialized data through SPI.

The input channels are individually configurable as sinking (p-type) or
sourcing (n-type) inputs. Current limiters on each digital input minimize power
dissipation while ensuring compliance with the IEC 61131-2 standard. With a
single current-setting resistor, the inputs are individually configurable for
Type 1/3, Type 2, TTL, or HTL (high-impedance 24 V levels).

Every input channel has a programmable glitch/debounce filter and an optional
16-bit down-counter. The MAX22196 can be powered from a field supply from 8 V
up to 36 V and has an integrated 5 V linear regulator that can provide up to
20 mA of load current.

Applications
------------

* Programmable Logic Controllers
* Factory Automation
* Process Control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Field-side supply (V24): 8 V to 36 V
* Integrated 5 V LDO regulator: up to 20 mA load current
* Logic interface supply (VL): 2.5 V to 5.5 V

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22196/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22196/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the MAX22196 device via SPI and configures
channel 0 in source mode with a specific filter delay. It then reads back
the channel's filter delay and counter values to verify the configuration.

In order to build the basic example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
	   --project max22196 --variant basic --board max32666fthr

IIO example
~~~~~~~~~~~

The IIO example initializes the MAX22196 IIO device descriptor and launches
an IIO application server, allowing real-time interaction with the device
channels through IIO clients such as the IIO Oscilloscope.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

In order to build the IIO example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
	   --project max22196 --variant iio --board max32666fthr

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX22196EVKIT
* AD-APARD32690-SL (MAX32690)

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``basic``, ``iio``.
Available boards: ``ad-apard32690-sl``, ``max32666fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project max22196 --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max22196 --variant basic --board ad-apard32690-sl \
      --probe openocd --flash
