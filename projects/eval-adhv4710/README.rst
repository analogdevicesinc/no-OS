EVAL-ADHV4710 no-OS Example Project
===================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADHV4710 <https://www.analog.com/ADHV4710>`_
* `EVAL-ADHV4711 <https://www.analog.com/ADHV4711>`_

Overview
--------

The ADHV4710 and ADHV4711 are high voltage, high precision amplifiers
controlled over an SPI interface. The two parts are variants of the same
device and share the same register map and control logic; they differ
only in the value reported by the product-version (chip-id) register
(``CTRL_REG_26``): ``0x46`` for the ADHV4710 and ``0x47`` for the
ADHV4711. A single no-OS driver (``drivers/amplifiers/adhv4710``)
supports both variants, selected at initialization time.

The firmware initializes the amplifier, configures its overcurrent
(source and sink), overvoltage (positive and negative) and
overtemperature protection limits, and then periodically polls the alarm
status register, reporting any triggered protection over UART and
re-enabling the amplifier from a latched shutdown.

Applications
------------

* High voltage signal conditioning
* Piezoelectric transducer and MEMS driving
* Programmable power supplies
* Automated test equipment (ATE)

Selecting the Device Variant
----------------------------

The driver distinguishes the two parts through the ``id`` field of
``struct adhv4710_init_param``, using ``enum adhv4710_type``:

.. code-block:: c

   enum adhv4710_type {
           ID_ADHV4710,
           ID_ADHV4711,
   };

At initialization the driver reads ``CTRL_REG_26`` and validates it
against the expected product-version value for the selected variant
(returning ``-ENXIO`` on mismatch). The variant used by the example is
controlled by the ``ADHV4710_DEV_ID`` macro in the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adhv4710/src/common>`__:

.. code-block:: c

   /* Device variant: ID_ADHV4710 or ID_ADHV4711 */
   #define ADHV4710_DEV_ID                 ID_ADHV4710

Set this to ``ID_ADHV4711`` when running against an ADHV4711 board.

Hardware Specifications
------------------------

Board Connections
~~~~~~~~~~~~~~~~~~~

The evaluation board is controlled over a 4-wire SPI interface plus a
GPIO used for hardware reset. The example also toggles a user LED to
indicate liveness. The SPI, reset and LED pins are defined in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adhv4710/src/platform>`__.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the example is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adhv4710/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adhv4710/src/platform>`__.

ADHV4710 example
~~~~~~~~~~~~~~~~~

The ``adhv4710_example`` initializes the UART console and the SPI
interface, performs a hardware reset of the amplifier, and initializes
the device for the variant selected by ``ADHV4710_DEV_ID``. It then
clears the shutdown latch, enables all alarm latches, programs the
nominal quiescent current, and configures the overcurrent (source and
sink), overvoltage (positive and negative) and overtemperature
protection limits from the user-defined values in ``common_data.h``.

In its main loop the example periodically reads the alarm status
register (``CTRL_REG_14``), prints any triggered protection over UART,
clears the latched alarm flags, and re-enables the amplifier from
shutdown when required, toggling the user LED each cycle.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADHV4710 <https://www.analog.com/ADHV4710>`_ or
  `EVAL-ADHV4711 <https://www.analog.com/ADHV4711>`_
* `MAX32690EVKIT <https://www.analog.com/MAX32690>`_

Connections
^^^^^^^^^^^

Connect the evaluation board to the MAX32690 SPI pins (CS, MOSI, MISO,
SCLK), the reset GPIO and ground/power as configured in the platform
files.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
