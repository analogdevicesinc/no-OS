MAX34451 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `MAX34451 <https://www.analog.com/MAX34451>`_
* `MAX34455 <https://www.analog.com/MAX34455>`_
* `ADPM12160 <https://www.analog.com/ADPM12160>`_
* `ADPM12200 <https://www.analog.com/ADPM12200>`_
* `ADPM12250 <https://www.analog.com/ADPM12250>`_

Overview
--------

The MAX34451 is a 16-channel PMBus power-supply manager and monitor with
integrated sequencing and margining. It monitors voltage, current and
temperature across its channels and provides programmable sequencing,
margining and fault management for multi-rail power systems. Channels 0-11
support both monitoring and margining; channels 12-15 are monitor-only, and
channels 16-20 are temperature channels. The MAX34451NA6 revision adds input
voltage and input current telemetry (``READ_VIN``, ``READ_IIN``) along with
``STATUS_INPUT``. The MAX34455 is a 12-channel variant of the same part.

The ADPM12160 is a quarter-brick DC-DC power conversion module delivering a
fully regulated 12V/12.5V output at a continuous 1600W, with peak capability
up to 2400W. It accepts a 40V to 60V input, reaches up to 98% peak efficiency
at half load, and includes overtemperature, input undervoltage/overvoltage,
input/output overcurrent and short-circuit protection. The ADPM12200 (2000W
continuous, 3000W peak) and ADPM12250 (2500W continuous) share the same form
factor and PMBus interface.

All supported parts use the PMBus DIRECT data format over I2C, and are paged:
a PMBus ``PAGE`` write selects which channel or subsystem subsequent commands
address.

Applications
------------

MAX34451 / MAX34455
~~~~~~~~~~~~~~~~~~~

* Multi-rail power system management
* Data center and server power monitoring
* Telecommunications equipment
* Storage systems
* Industrial control systems
* System optimization in prototype and production

ADPM12160 / ADPM12200 / ADPM12250
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* Distributed power architectures
* Wireless networks and base stations
* Optical network equipment
* Enterprise networks
* Latest-generation ICs and microprocessor-powered systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

MAX34451 / MAX34455
^^^^^^^^^^^^^^^^^^^

The MAX34451EVKIT is powered entirely from its mini-USB connector (J20); no
external supply is required. Jumpers J23 (``5V-5V``, ``SDA-SDA``, ``GND-GND``,
``SCL-SCL``) and J1 (``VDD-VDUT``) must be populated for the device to power
up and communicate. The four monitored channels are on-board 500mA LDOs fed
from the USB 5V rail, tied to the device by DIP switches S5 (RS), S6 (PSEN)
and S7 (PWM), all closed by default; open them to sequence user-supplied rails
instead. The PMBus interface operates at 3.3V.

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the user guide for
the corresponding demo board:
`MAX34451EVKIT user guide <https://www.analog.com/media/en/technical-documentation/data-sheets/MAX34451EVKIT.pdf>`_

ADPM12160 / ADPM12200 / ADPM12250
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply with output of +40V to
+60V (+54V nominal) can be used to power up the demo board, applied to the
high-current input lugs. The supply must deliver up to 3kW and the load sink
up to 250A, scaled by the number of installed modules, and the modules require
500LFM or higher forced airflow from a separately supplied fan. Each module is
enabled by its own switch, SW1-SW4: OPEN floats the REM pin to enable the
output, GND disables it. Do not turn on the power supply until all connections
are complete.

**Pin Description**

	+----------+------------------------------+
	| Name     | Description                  |
	+----------+------------------------------+
	| IN       | Power Supply, +40V - +60V    |
	+----------+------------------------------+
	| GND      | Connect to Ground            |
	+----------+------------------------------+
	| OUT      | 12V Output, connect to Load  |
	+----------+------------------------------+
	| DGND     | PMBus Ground (J18)           |
	+----------+------------------------------+
	| DATA     | PMBus Serial Data (J18)      |
	+----------+------------------------------+
	| CLK      | PMBus Serial Clock (J18)     |
	+----------+------------------------------+

**Hardware Bringup**

For reference, consult the Quick Start Procedure section in the user guide for
the demo board; all three modules are powered up the same way:
`EVAL-ADPM12160 user guide <https://www.analog.com/media/en/technical-documentation/user-guides/eval-adpm12160.pdf>`_

On-board Connectors
~~~~~~~~~~~~~~~~~~~

The driver communicates with the device entirely over PMBus (I2C). The
following signals must be brought out to the host MCU:

========= =====================================================
Signal    Function
========= =====================================================
SDA       PMBus/I2C serial data
SCL       PMBus/I2C serial clock
GND       Ground, common with the host MCU
ALERT     Optional open-drain fault alert output (SMBALERT#)
RST       Optional active-low reset input
CONTROL0  Optional master on/off input, sequence group 0
CONTROL1  Optional master on/off input, sequence group 1
FAULT0-2  Optional open-drain fault lines (monitored by the host)
========= =====================================================

The ALERT, RST, CONTROL and FAULT pins are optional in the driver — they are
requested with ``no_os_gpio_get_optional()`` and may be left unconnected if the
application does not need them. The device has no dedicated power-good pin;
power-good state is read over PMBus. The CONTROL pins only take effect when
``ON_OFF_CONFIG`` enables them (bit 2), which the factory default does not.

The PMBus slave address is resistor-strapped on the device and must match
``MAX34451_ADDRESS`` in
`src/common/common_data.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/common/common_data.h>`__.
The project ships with ``0x4E``. Note that no-OS expects a **7-bit** address.

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/platform>`__.

The part variant is selected by the ``.id`` field of ``max34451_init_param``
(``ID_MAX34451``, ``ID_MAX34451NA6``, ``ID_MAX34455``, ``ID_ADPM12160``,
``ID_ADPM12200`` or ``ID_ADPM12250``). Set this to match the device actually on
the bus — the driver uses it to size the channel map and to reject commands the
selected part does not implement.

Basic Example
~~~~~~~~~~~~~

The basic example exercises the driver API against a live device and prints the
results to the UART console. It walks through:

* ``CAPABILITY`` reads (PMBus speed, PEC support, alert support)
* ``WRITE_PROTECT`` level cycling and readback
* ``PAGE`` selection across several channels
* ``MFR_SERIAL`` block read and the ``STATUS_MFR_SPECIFIC`` LOCK bit
* ``STATUS_WORD`` / ``STATUS_VOUT`` / ``STATUS_IOUT`` reads per channel
* Telemetry reads (VOUT, IOUT, VOUT_PEAK) in engineering units
* ``CLEAR_FAULTS``
* ``OPERATION`` mode and VOUT margin high/low configuration
* ``TON_DELAY`` / ``TOFF_DELAY`` timing configuration
* A five-iteration continuous monitoring loop

The part under test is selected in
`src/examples/basic/basic_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/examples/basic/basic_example.c>`__.
Several steps in this example are channel-paged and are meaningful only on the
MAX34451/MAX34455 parts; on the ADPM modules the corresponding calls are
expected to report failure, since those pages map to subsystems rather than
independent rails.

In order to build the basic example make sure you are using this command:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project max34451 --variant basic --board max32690evkit

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the board so that the user may
connect to it with an IIO client. Voltage, current and temperature channels are
exposed as IIO channels, and the PMBus status registers are exposed as channel
attributes, with status_cml being debug attribute, along with a raw register
read/write interface.

This variant supports the MAX34451, MAX34451NA6 and MAX34455 only. The ADPM
modules are handled by the basic example; ``max34451_iio_init()`` returns
``-EINVAL`` for those part IDs by design.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/examples/iio>`__

In order to build the IIO project make sure you are using this command:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project max34451 --variant iio --board max32690evkit

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX34451 <https://www.analog.com/MAX34451>`_ evaluation hardware, or one of
  the other supported parts listed above
* `MAX32690EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32690evkit.html>`_

Connections
^^^^^^^^^^^

The device is wired to the MCU's first I2C controller (``I2C_DEVICE_ID 0``) and
the console is brought out on UART2 at 115200 baud, as defined in
`src/platform/maxim/parameters.h <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/platform/maxim/parameters.h>`__.

+------------------+----------------------------------------+-----------------------+
| Device Signal    | Function                               | MCU Board Pin         |
+==================+========================================+=======================+
| SDA              | PMBus/I2C serial data                  | I2C0_SDA              |
+------------------+----------------------------------------+-----------------------+
| SCL              | PMBus/I2C serial clock                 | I2C0_SCL              |
+------------------+----------------------------------------+-----------------------+
| GND              | Ground                                 | GND                   |
+------------------+----------------------------------------+-----------------------+
| ALERT (optional) | SMBALERT# fault output                 | <GPIO of your choice> |
+------------------+----------------------------------------+-----------------------+
| RST (optional)   | Active-low reset input                 | <GPIO of your choice> |
+------------------+----------------------------------------+-----------------------+
| CONTROL0/1 (opt) | Master on/off inputs                   | <GPIO of your choice> |
+------------------+----------------------------------------+-----------------------+
| FAULT0-2 (opt)   | Open-drain fault lines                 | <GPIO of your choice> |
+------------------+----------------------------------------+-----------------------+

The I2C bus requires pull-up resistors to the device's logic supply. The
MAX32690EVKIT drives its I2C pins from VDDIOH, configured through
``MXC_GPIO_VSSEL_VDDIOH`` in
`src/platform/maxim/parameters.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max34451/src/platform/maxim/parameters.c>`__.

The UART console appears over the board's USB connection. Open it at 115200
baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``, ``iio``.
Available boards: ``max32690evkit``
Replace ``--variant`` / ``--board`` accordingly.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32690evkit board)
   python tools/scripts/no_os_build.py build \
      --project max34451 --variant basic --board max32690evkit

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max34451 --variant basic --board max32690evkit \
      --probe openocd --flash
