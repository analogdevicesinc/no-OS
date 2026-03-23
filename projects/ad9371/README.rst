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
wireless applications. It covers the 300 MHz to 6 GHz frequency ranges
and incorporates data conversion, serial interfaces, and power
management features. With integrated digital predistortion and
closed-loop gain control, it offers extensive programmability for
various digital and analog I/O, making it suitable for dynamic
telecommunications signal processing.

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
include a 1.3V supply for the main analog domain with a voltage
tolerance of +/-2.5%, and 3.3V and 1.8V supplies for other sections with
tolerances of +/-5%. Input voltage should be between 6V and 15V.
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

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The AD9371 evaluation board supports several no-OS examples for testing
different functionalities: the demo example, the IIO example, and the
DMA example. The demo example covers initial setup and configuration of
the AD9371 transceiver. The IIO example integrates the Industrial I/O
framework to allow communication with host systems, enabling streaming
and interaction through the IIO Oscilloscope application. The DMA
example focuses on high-speed data transfer capabilities using Direct
Memory Access for handling large datasets efficiently.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9371/src>`__.

Demo example
~~~~~~~~~~~~

The demo example code in ``headless.c`` configures the AD9371
transceiver system in a headless mode by executing its ``main``
function, which initializes system clocks and sets up JESD204 interfaces
specific to Mykonos transceiver hardware. Through conditional
compilation, it adapts configurations for Altera and Xilinx platforms.
The code initializes the AD9528 clock device with a hard reset and SPI
register writes, handles potential initialization errors, and checks PLL
lock status. It incorporates Mykonos M3 processor firmware binary data
to finalize system setup.

In order to build the demo example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9371/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = n

IIO example
~~~~~~~~~~~

The IIO example code in the AD9371 no-OS project initializes and
operates the AD9371 transceiver using industrial I/O (IIO) interfaces.
It sets up key components including ADC and DAC through
``iio_axi_adc_init`` and ``iio_axi_dac_init`` functions. The code
manages data transfer via DMA, using ``axi_dmac_transfer_start`` and
``axi_dmac_transfer_wait_completion``. The example utilizes
``iio_app_init`` and ``iio_app_run`` to configure and execute the
application while setting up UART communication through
``iio_app_init_param``. The data capture is handled via DMA buffers for
ADC and DAC using device descriptors for buffer initialization.

If you are not familiar with ADI IIO Application, please take a look at:
:dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`

In order to build the IIO project, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9371/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

DMA example
~~~~~~~~~~~

The DMA example in the AD9371 no-OS project demonstrates efficient data
transfer from ADC buffers to memory using Direct Memory Access. This
code initializes DMA controllers by setting source and destination
addresses to facilitate high-speed data exchange between peripherals and
memory, bypassing CPU intervention. The receive DMA controller is
configured with the ``src_addr`` pointing to the ADC buffer and the
``dest_addr`` pointing to a memory location, enabling fast capture of
ADC samples directly into memory. This setup reduces CPU overhead and
increases throughput, optimizing performance for large datasets, crucial
for high-frequency telecommunications tasks utilizing the AD9371
transceiver.

In order to build the DMA example, use the following build command:

.. code-block:: bash

   make NEW_CFLAGS=-DDMA_EXAMPLE

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `ADRV9371 <https://www.analog.com/ADRV9371>`_ (ADRV9371-N/PCBZ)
* `ZC706 <https://www.xilinx.com/ZC706>`_
* `ZCU102 <https://www.xilinx.com/ZCU102>`_
* `KCU105 <https://www.xilinx.com/KCU105>`_

Connections
^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| Pins            | Component       | Purpose         | Connections     |
+-----------------+-----------------+-----------------+-----------------+
| RX1 Connector   | J200            | Receive Signal  | Connects to RX1 |
|                 |                 | Input           | channel of the  |
|                 |                 |                 | ADRV9371        |
+-----------------+-----------------+-----------------+-----------------+
| RX2 Connector   | J201            | Receive Signal  | Connects to RX2 |
|                 |                 | Input           | channel of the  |
|                 |                 |                 | ADRV9371        |
+-----------------+-----------------+-----------------+-----------------+
| SnRxA Connector | J202            | Sniffer Receive | Allows sniffer  |
|                 |                 |                 | reception on    |
|                 |                 |                 | the ADRV9371    |
+-----------------+-----------------+-----------------+-----------------+
| TX2 Output      | J306            | Transmit Signal | Connects to TX2 |
| Connector       |                 | Output          | channel output  |
|                 |                 |                 | of the ADRV9371 |
+-----------------+-----------------+-----------------+-----------------+
| HPC FMC         | J37             | Data and        | Connects to the |
| Connector       |                 | Control         | FMC HPC         |
|                 |                 | Interface       | connector on    |
|                 |                 |                 | the carrier     |
|                 |                 |                 | board           |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # copy the Xilinx hardware description file
   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
