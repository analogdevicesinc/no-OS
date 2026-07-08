AD413x no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
   :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4130-8WARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4130-8.html>`_

Overview
--------

The EVAL-AD4130-8WARDZ is an evaluation board that allows users to
evaluate the AD4130-8, an ultra low power, high precision ADC. The fully
integrated analog front end (AFE) includes a multiplexer for up to 16
single-ended or eight differential inputs, a programmable gain amplifier
(PGA), and a 24-bit sigma-delta ADC. It features programmable gain
amplifiers and multiplexers for selecting input channels, supporting
digital communication over an SPI interface.

Applications
------------

* Smart transmitters
* Wireless battery and harvester powered sensor nodes
* Portable instrumentation
* Temperature measurement: thermocouple, RTD, thermistors
* Pressure measurement: bridge transducers
* Healthcare and wearables

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD4130-8 evaluation board operates with a 5V USB power supply
and provides flexibility in power configuration. It features a default
3.3V regulator for AVDD and an optional 1.8V regulator. External
AVDD/AVSS connections can be made via Connector J7. The board is
equipped to facilitate both AVDD and IOVDD selections with integrated
regulators, enhancing its utility in ADC performance testing and data
acquisition tasks.

============ =========================
J7 Connector Function
============ =========================
1            External IOVDD connection
2            External GND connection
3            External AVSS connection
4            External AVDD connection
============ =========================

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| Connector J10 (Wire Bridge)       | Handles analog input for 4/6 Wire |
|                                   | bridge setups, with specific pin  |
|                                   | functions such as Excitation,     |
|                                   | External Reference, and Ground    |
|                                   | connections.                      |
+-----------------------------------+-----------------------------------+
| Connector J12 (DC Input)          | Provides DC connections for AIN8  |
|                                   | to AIN15, facilitating analog     |
|                                   | input without DC filtering.       |
+-----------------------------------+-----------------------------------+

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

================= ========= ========== ========
PMOD Connector P7 Function  Connection Function
================= ========= ========== ========
1                 CS_N      7          SYNC_N
2                 MOSI/DIN  8          INT
3                 MISO/DOUT 9          N/C
4                 SCLK      10         N/C
5                 GND       11         GND
6                 V_USB     12         V_USB
================= ========= ========== ========

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
------------------------

This project uses the no-OS variant-based build flow. Selecting a variant
at build time (``--variant <name>``) chooses which application is compiled.
The initialization data used in the examples is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad413x/src/app>`__.

Demo Example
~~~~~~~~~~~~

The demo example initializes the AD413x via SPI in four-wire mode, configures
GPIO for conversion readiness, and applies predefined presets for channel setup
(input sources, gain settings, filters, and reference settings). A soft reset
prepares the device for operation. Once initialized, ADC operation modes
(single or continuous) are managed to facilitate data acquisition. Results
are printed to the UART console.

IIO Example
~~~~~~~~~~~

The IIO example sets up the AD413x ADC within the IIO framework via
``ad413x_iio_init``. Channel management is handled dynamically. Sample
data is retrieved using ``ad413x_iio_read_samples``, applying continuous
conversion techniques. The example enables IIO attribute setup for scale
and raw data properties.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-AD4130-8WARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4130-8.html>`_
* `ZedBoard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_

Connections
^^^^^^^^^^^

* Power the EVAL-AD4130-8 board with a 5V USB supply.

* Use on-board regulators to set up AVDD (default 3.3V) and IOVDD.

* Connect SPI lines of the EVAL-AD4130-8 board PMOD connector P7 to the
  ZedBoard's appropriate PMOD header.

* Ensure a common ground is established between EVAL-AD4130-8 and ZedBoard.

+-----------------+-----------------+-----------------+-----------------+
| EVAL-AD4130-8   | Function        | ZedBoard PMOD   | Function        |
| PMOD P7         |                 | Header Pin      |                 |
+-----------------+-----------------+-----------------+-----------------+
| P7-1            | CS_N            | 1               | Chip Select     |
|                 |                 |                 | (Active Low)    |
+-----------------+-----------------+-----------------+-----------------+
| P7-2            | MOSI/DIN        | 2               | Master Out      |
|                 |                 |                 | Slave In        |
+-----------------+-----------------+-----------------+-----------------+
| P7-3            | MISO/DOUT       | 3               | Master In Slave |
|                 |                 |                 | Out             |
+-----------------+-----------------+-----------------+-----------------+
| P7-4            | SCLK            | 4               | Serial Clock    |
+-----------------+-----------------+-----------------+-----------------+
| P7-5            | GND             | 5               | Ground          |
+-----------------+-----------------+-----------------+-----------------+
| P7-6            | V_USB           | 6               | Voltage Supply  |
+-----------------+-----------------+-----------------+-----------------+

* Use a 12V supply in the barrel jack (J20) to power the ZedBoard.

* Connect the ZedBoard's USB-UART to your PC via MicroUSB. The UART
  console appears at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``, ``iio``.
Available boards: ``zed``.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis environment (sets XILINX_VITIS and adds tools to PATH)
   source /path/to/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   $env:XILINX_VITIS = "<C:\path\to\Vitis>"

   cd no-OS

   # build the example on the ZedBoard (replace --variant as needed)
   python tools/scripts/no_os_build.py build \
      --project ad413x --variant demo --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ad413x --variant demo --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash
