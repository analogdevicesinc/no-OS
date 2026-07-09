AD9371 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `ADRV9371 <https://www.analog.com/ADRV9371>`_

Overview
--------

The AD9371 is a compact, dual-channel transceiver built for 3G and 4G
wireless applications. It covers the 300 MHz to 6 GHz frequency range
and incorporates data conversion, serial interfaces, and power management
features. With integrated digital predistortion and closed-loop gain
control, it offers extensive programmability for various digital and
analog I/O, making it suitable for dynamic telecommunications signal
processing.

The device uses JESD204B high-speed serial interfaces for ADC and DAC
data transport, and the AD9528 clock IC provides the reference and device
clocks needed for the transceiver and JESD204B framer/deframer. The
evaluation board connects to the carrier board through a single FMC HPC
connector.

Applications
------------

* 3G/4G micro and macro base stations (BTS)
* 3G/4G multicarrier picocells
* FDD and TDD antenna systems
* Microwave, non-line of sight (NLOS) backhaul systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9371 evaluation board requires specific power supply
characteristics to maintain stable performance. It utilizes the ADP5054
power management IC to manage multiple power domains. Key voltage levels
include a 1.3 V supply for the main analog domain with a voltage
tolerance of +/-2.5%, and 3.3 V and 1.8 V supplies for other sections
with tolerances of +/-5%. Input voltage should be between 6 V and 15 V.
Proper power sequencing is essential to prevent undesired currents,
starting with the simultaneous powering of VDIG and VDDA_1P3, followed
by VDDA_3P3 and other supplies. Ferrite beads are implemented to
minimize noise and ensure isolation between RF and digital sections.

Digital Communication Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| Pin Name  | Pin No.  | Type    | Voltage | Maximum | Description                                                 |
|           |          |         | (V)     | Current |                                                             |
|           |          |         |         | (mA)    |                                                             |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDIG      | L8, L9   | Digital | 1.3     | 1700    | 1.3 V digital core high current                             |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDDA_RXRF | B1       | Analog  | 1.3     | 20      | Sniffer front end only                                      |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDDA_RXTX | F2       | Analog  | 1.3     | 560     | 1.3 V supply for Tx/ORx baseband circuits,                  |
|           |          |         |         |         | TIA/Tx GM/baseband filters                                  |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDDA_BB   | E5       | Analog  | 1.3     | 670     | Rx ADC, ORx ADC, Tx DAC, auxiliary ADC, REF_CLK             |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDDA_RXLO | C6       | Analog  | 1.3     | 270     | 1.3 V LO generator for Rx synthesizer, external LO          |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+
| VDDA_TXLO | F12      | Analog  | 1.3     | 400     | 1.3 V LO generator for Tx synthesizer, buffers, external LO |
+-----------+----------+---------+---------+---------+-------------------------------------------------------------+

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9371/src>`__.

Demo Example
~~~~~~~~~~~~

The demo example (variant ``demo``) covers initial setup and configuration
of the AD9371 transceiver. It initializes the AD9528 clock device with a
hard reset and SPI register writes, sets up JESD204B interfaces, checks
PLL lock status, and incorporates the Mykonos M3 processor firmware
binary data to finalize system setup.

DMA Example
~~~~~~~~~~~

The DMA example (variant ``dma_example``) demonstrates efficient data
transfer from ADC buffers to memory using Direct Memory Access. The
receive DMA controller is configured with the source address pointing to
the ADC buffer and the destination address pointing to a memory location,
enabling fast capture of ADC samples directly into memory. This reduces
CPU overhead and increases throughput, optimizing performance for large
datasets in high-frequency telecommunications tasks.

IIO Example
~~~~~~~~~~~

The IIO example (variant ``iio``) launches an IIOD server on the carrier
board so that the user may connect to it via an IIO client. It sets up
AXI ADC and DAC cores through ``iio_axi_adc_init`` and
``iio_axi_dac_init``, manages data transfer via DMA, and exposes the
AD9371 transceiver attributes through the IIO framework, enabling
real-time streaming and interaction through the IIO Oscilloscope
application.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `ADRV9371 <https://www.analog.com/ADRV9371>`_ (ADRV9371-N/PCBZ)
* One of: `ZC706 <https://www.xilinx.com/ZC706>`_,
  `ZCU102 <https://www.xilinx.com/ZCU102>`_, or
  `KCU105 <https://www.xilinx.com/KCU105>`_ carrier board

Connections
^^^^^^^^^^^

Connect the ADRV9371 evaluation board to the carrier board via the FMC
HPC connector (J37 on the ZC706, J5 on the ZCU102, J22 on the KCU105).

+-----------------+-----------------+-----------------+
| Pins            | Component       | Description     |
+=================+=================+=================+
| RX1 Connector   | J200            | Receive Signal  |
|                 |                 | Input, RX1      |
+-----------------+-----------------+-----------------+
| RX2 Connector   | J201            | Receive Signal  |
|                 |                 | Input, RX2      |
+-----------------+-----------------+-----------------+
| SnRxA Connector | J202            | Sniffer Receive |
+-----------------+-----------------+-----------------+
| TX2 Connector   | J306            | Transmit Signal |
|                 |                 | Output, TX2     |
+-----------------+-----------------+-----------------+
| HPC FMC         | J37             | Data and        |
| Connector       |                 | Control         |
|                 |                 | Interface       |
+-----------------+-----------------+-----------------+

Connect a USB cable to the carrier board USB-UART port and the host PC
for serial console access at 115200 baud, 8N1.

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``demo``,
``dma_example``, ``iio``. Available boards: ``zc706``, ``zcu102``,
``kcu105``.

A Xilinx XSA hardware description file is required. The HDL design name
is ``adrv9371x``; the hardware name is composed as
``adrv9371x_<board>`` (e.g. ``adrv9371x_zc706``).

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment
   source ~/.xilinx/2025.1/Vitis/settings64.sh
   # PowerShell (Windows) equivalent:
   #   & "$env:USERPROFILE\.xilinx\2025.1\Vitis\settings64.bat"

   cd no-OS

   # Build the demo example on ZC706
   python tools/scripts/no_os_build.py build \
       --project ad9371 --variant demo --board zc706 \
       --hardware /path/to/adrv9371x_zc706/system_top.xsa

   # Build and flash via JTAG
   python tools/scripts/no_os_build.py build \
       --project ad9371 --variant demo --board zc706 \
       --hardware /path/to/adrv9371x_zc706/system_top.xsa \
       --probe openocd --flash

   # Build the IIO example on ZCU102
   python tools/scripts/no_os_build.py build \
       --project ad9371 --variant iio --board zcu102 \
       --hardware /path/to/adrv9371x_zcu102/system_top.xsa

   # Build the DMA example on KCU105
   python tools/scripts/no_os_build.py build \
       --project ad9371 --variant dma_example --board kcu105 \
       --hardware /path/to/adrv9371x_kcu105/system_top.xsa
