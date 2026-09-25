AD-APARDSPOE-SL no-OS Example Project
======================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD-APARDSPOE-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apardspoe-sl.html>`_
* `AD-APARDPFWD-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apardpfwd-sl.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

Overview
--------

The AD-APARDSPOE-SL is a Single Pair Ethernet (SPoE) power sourcing carrier
board that stacks onto an AD-APARD32690-SL host. It carries an ADIN1110 low
power 10BASE-T1L MAC/PHY, giving that MAX32690 host a T1L Ethernet link that
can also deliver power to a downstream node over the same pair.

The examples in this project require **two** AD-APARD32690-SL boards
daisy-chained over a 10BASE-T1L link:

#. **APARDSPOE node** - an AD-APARD32690-SL with the AD-APARDSPOE-SL stacked
   on top. This is the board these examples run on.
#. **APARDPFWD node** - a second AD-APARD32690-SL with an AD-APARDPFWD-SL
   stacked on top instead, acting as the downstream/forwarding peer on the
   T1L link (see the `apardpfwd project <https://github.com/analogdevicesinc/no-OS/tree/main/projects/apardpfwd>`__).

This project demonstrates the AD-APARDSPOE-SL together with the ADIN1110
driver and the no-OS LWIP glue: a TCP command server is started over the
T1L link, and a second example additionally reads RGBC (red, green, blue,
clear) values from a TCS34725 color light-to-digital converter attached over
I2C, exposing the readings through the same TCP command channel.

Applications
------------

* Single Pair Ethernet (10BASE-T1L) power and data over one cable pair
* Sensor-to-cloud edge nodes over long cable runs
* Industrial process and factory automation
* Building automation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-APARD32690-SL host board is powered over its USB-C connector. The
AD-APARDSPOE-SL and AD-APARDPFWD-SL shields draw their logic supply from the
stacking connector and do not require a separate external supply for the
examples in this project.

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

========= =====================================================
Connector Function
========= =====================================================
J1        Stacking connector to the AD-APARD32690-SL host board
T1L       Single Pair Ethernet (10BASE-T1L) terminal block
========= =====================================================

The AD-APARDPFWD-SL forwarding shield stacks on the second, downstream
AD-APARD32690-SL board and is connected to the APARDSPOE node's T1L terminal
block over a twisted pair cable; its output port must be enabled in software
on that downstream board before frames are forwarded (see the
`apardpfwd project <https://github.com/analogdevicesinc/no-OS/tree/main/projects/apardpfwd>`__).

No-OS Supported Examples
------------------------

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/apardspoe/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/apardspoe/src/platform>`__.

APARD Communication Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Selected by default (``apard_communication_example``). This example enables
the AD-APARDPFWD-SL output port, initializes the ADIN1110 over SPI, and
starts a TCP server on the T1L link (default settings: static IP
192.168.97.60, port 10000). Any bytes received from a connected client are
echoed back.

Color Sensor Example
~~~~~~~~~~~~~~~~~~~~~

Selected with ``--variant color_sensor_example``. In addition to the ADIN1110
TCP server, this example initializes a TCS34725 color sensor over I2C and
periodically reads RGBC values. Connected TCP clients may send text commands
(terminated with ``\n`` or ``\r``) to the same port; the parser lives in
`color_sensor_example.c <https://github.com/analogdevicesinc/no-OS/tree/main/projects/apardspoe/src/examples/color_sensor_example/color_sensor_example.c>`__.
The only supported command is:

* ``COLOR_READ`` - replies with ``R:<r> G:<g> B:<b> C:<c>``.

Unrecognized commands receive ``ERR:UNKNOWN_CMD``, and a failed sensor read
replies with ``ERR:READ_FAILED``.

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* 2x `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_
* `AD-APARDSPOE-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apardspoe-sl.html>`_
* `AD-APARDPFWD-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apardpfwd-sl.html>`_
* TCS34725 color sensor breakout (``color_sensor_example`` variant only)

Connections
^^^^^^^^^^^

Stack the AD-APARDSPOE-SL onto one AD-APARD32690-SL (this is the node this
project's firmware runs on), and stack the AD-APARDPFWD-SL onto a second,
separate AD-APARD32690-SL. No manual wiring is required for either stacking
connection. Then daisy-chain the two boards by connecting the
AD-APARDSPOE-SL's T1L terminal block to the AD-APARDPFWD-SL's T1L terminal
block with a twisted pair cable. For the ``color_sensor_example`` variant,
additionally wire the TCS34725 breakout to the APARDSPOE node's
AD-APARD32690-SL I2C1 header:

========= ==================================
TCS34725  AD-APARD32690-SL
========= ==================================
VIN       3V3
GND       GND
SDA       I2C1 SDA
SCL       I2C1 SCL
========= ==================================

The serial console is available on the UART connected through the on-board
debug adapter, at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Maxim platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants:
``apard_communication_example``, ``color_sensor_example``. Available boards:
``ad-apard32690-sl``.

For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # PowerShell (Windows) equivalent:
   #   $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the apard_communication_example example on the ad-apard32690-sl board
   python tools/scripts/no_os_build.py build \
      --project apardspoe --variant apard_communication_example --board ad-apard32690-sl

   # build the color_sensor_example example
   python tools/scripts/no_os_build.py build \
      --project apardspoe --variant color_sensor_example --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project apardspoe --variant apard_communication_example --board ad-apard32690-sl \
      --probe openocd --flash
