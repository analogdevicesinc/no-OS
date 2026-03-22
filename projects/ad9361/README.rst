AD9361 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD-FMCOMMS2-EBZ`
- :adi:`AD-FMCOMMS5-EBZ`

Overview
--------

The :adi:`AD-FMCOMMS2-EBZ` and :adi:`AD-FMCOMMS5-EBZ` are high-speed analog FMC
modules designed to showcase the :adi:`AD9361`, a high-performance, highly
integrated RF Agile Transceiver™ intended for use in 3G and 4G base
station, test equipment, and software defined radio (SDR) applications.
The :adi:`AD-FMCOMMS2-EBZ` is optimized for RF engineers and targets 2.4 GHz
operation with high-performance discrete external components, while the
:adi:`AD-FMCOMMS5-EBZ` hosts dual :adi:`AD9361` devices enabling 4×4 MIMO
configurations for advanced beamforming, angle-of-arrival, and
multi-channel SDR prototyping. Both boards are powered through the FMC
connector from the host Xilinx carrier board and provide SMA RF
connectors for TX and RX paths, as well as digital control via the SPI
and GPIO lines routed through the FMC interface.

Applications
------------

- Point to point communication systems
- Femtocell/picocell/microcell base stations
- General-purpose radio systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCOMMS2-EBZ and AD-FMCOMMS5-EBZ boards are powered entirely
through the FMC connector from the host carrier board; no separate
external power supply is needed for the FMC module itself. Internally,
the AD9361 requires three supply rails: the analog supply at 1.3 V
(VDDD1P3_DIG / VDDAx), the interface supply at 1.8 V (VDD_INTERFACE),
and the GPO supply at 3.3 V (VDD_GPO). On-board voltage regulators
derive these rails from the 3.3 V and 12 V supplies available on the FMC
connector. For optimal noise performance, it is recommended that the 1.3
V analog rail be sourced from a low-noise, low-dropout (LDO) regulator
rather than directly from a switching supply.

Connections
~~~~~~~~~~~

The AD-FMCOMMS2-EBZ/AD-FMCOMMS5-EBZ connectors and digital pins include:

+-----------------------+-----------------------+-------------------------+
| Connector / Pin       | Type / Signal         | Description             |
+-----------------------+-----------------------+-------------------------+
| **FMC Connectors**                                                      |
+-----------------------+-----------------------+-------------------------+
| J1                    | FMC LPC               | VITA 57.1-compliant     |
|                       |                       | low-pin-count FMC       |
|                       |                       | (AD-FMCOMMS2-EBZ)       |
+-----------------------+-----------------------+-------------------------+
| J1A / J1B             | FMC LPC               | Dual FMC connectors     |
|                       |                       | (AD-FMCOMMS5-EBZ)       |
+-----------------------+-----------------------+-------------------------+
| **RF Transmit (TX) Channels**                                           |
+-----------------------+-----------------------+-------------------------+
| J2 / TX1A, J3 / TX1B  | SMA                   | Transmit Channel 1      |
|                       |                       | outputs (A & B)         |
+-----------------------+-----------------------+-------------------------+
| J6 / TX2A, J7 / TX2B  | SMA                   | Transmit Channel 2      |
|                       |                       | outputs (A & B)         |
+-----------------------+-----------------------+-------------------------+
| **RF Receive (RX) Channels**                                            |
+-----------------------+-----------------------+-------------------------+
| J4 / RX1A, J5 / RX1B  | SMA                   | Receive Channel 1       |
|                       |                       | inputs (A & B)          |
+-----------------------+-----------------------+-------------------------+
| J8 / RX2A, J9 / RX2B  | SMA                   | Receive Channel 2       |
|                       |                       | inputs (A & B)          |
+-----------------------+-----------------------+-------------------------+
| **Clock & Reference**                                                   |
+-----------------------+-----------------------+-------------------------+
| J10                   | SMA                   | External reference      |
|                       |                       | clock input             |
+-----------------------+-----------------------+-------------------------+
| CLK_OUT               | Digital (FMC)         | Buffered output clock   |
|                       |                       | pin                     |
+-----------------------+-----------------------+-------------------------+
| **SPI Interface**                                                       |
+-----------------------+-----------------------+-------------------------+
| SPI_CLK               | Digital (FMC)         | SPI Clock Input         |
+-----------------------+-----------------------+-------------------------+
| SPI_DI                | Digital (FMC)         | SPI Data Input          |
+-----------------------+-----------------------+-------------------------+
| SPI_DO                | Digital (FMC)         | SPI Data Output         |
+-----------------------+-----------------------+-------------------------+
| SPI_ENB               | Digital (FMC)         | SPI Enable (active low) |
+-----------------------+-----------------------+-------------------------+
| **Control Signals**                                                     |
+-----------------------+-----------------------+-------------------------+
| ENABLE / TXNRX        | Digital (FMC)         | TX/RX Enable control    |
|                       |                       | pin                     |
+-----------------------+-----------------------+-------------------------+
| GPIO_0 – GPIO_5       | Digital (FMC)         | General-purpose I/O for |
|                       |                       | ENSM state monitoring   |
+-----------------------+-----------------------+-------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the 
:git-no-OS:`Project Source Path </projects/ad9361/src/>`.

The supported examples are:

- IIO Example
- DMA Example
- DMA-IRQ Example

IIO Example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can access AD9361 device settings such as LO
frequency, sampling rate, channel bandwidth, RF gain, and TX/RX path
configuration, facilitating seamless data management and real-time
streaming of I/Q samples to and from the transceiver through the AXI DMA
infrastructure.

If you are not familiar with ADI IIO Application, please take a look at:
`https://wiki.analog.com/resources/tools-software/no-os-software/iio`

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash
  
  IIOD = y

DMA Example
~~~~~~~~~~~

The DMA example demonstrates high-speed data transfer between the AD9361
transceiver and DDR memory using the AXI DMA core. The example
configures the AD9361 for a specified carrier frequency, sample rate,
and channel bandwidth, then initiates a DMA transfer to capture a block
of I/Q samples from the receive path into a memory buffer, allowing the
user to verify the data path without requiring an operating system or
IIO client.

In order to build the DMA example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash
  
  NEW_CFLAGS=-DDMA_EXAMPLE

DMA-IRQ Example
~~~~~~~~~~~~~~~

The DMA-IRQ example extends the DMA example by enabling interrupt-driven
DMA transfers. Rather than polling for DMA completion, the firmware
registers an interrupt service routine (ISR) triggered by the DMA
controller upon completion of each transfer. This approach allows the
processor to perform other tasks while a DMA transfer is in progress,
and is useful for demonstrating more efficient, production-ready data
acquisition patterns for the AD9361 receive path.

.. code-block:: bash

  NEW_CFLAGS=-DDMA_EXAMPLE\\-DDMA_IRQ_ENABLE


No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- AD-FMCOMMS2-EBZ or AD-FMCOMMS5-EBZ evaluation board
- Xilinx ZC706, ZCU102, ZC702, or ZedBoard carrier board
- SMA RF cables (for looping TX to RX, or connecting to
  external test equipment)
- USB-UART cable (for console access)
- SD card (for boot image, where applicable)

Connections
^^^^^^^^^^^

The AD-FMCOMMS2-EBZ connects to the Xilinx carrier board via its FMC LPC
connector. The AD-FMCOMMS5-EBZ requires a dual FMC slot (available on
ZC706 and ZC702). The physical connections and pin assignments are:

+-----------------------+-----------------------+-----------------------+
| AD-FMCOMMS2/5 Signal  | Carrier Board Signal  | Description           |
+-----------------------+-----------------------+-----------------------+
| FMC LPC (J1)          | J1 LPC FMC Connector  | All digital signals,  |
|                       |                       | clocking, and power   |
+-----------------------+-----------------------+-----------------------+
| SPI_CLK               | FMC LA00_P (D8)       | SPI Clock to AD9361   |
+-----------------------+-----------------------+-----------------------+
| SPI_DI                | FMC LA01_P (H7)       | SPI Data In (MOSI)    |
+-----------------------+-----------------------+-----------------------+
| SPI_DO                | FMC LA02_P (G9)       | SPI Data Out (MISO)   |
+-----------------------+-----------------------+-----------------------+
| SPI_ENB               | FMC LA03_P (H10)      | SPI Chip Select       |
|                       |                       | (active low)          |
+-----------------------+-----------------------+-----------------------+
| ENABLE                | FMC LA04_P (D11)      | AD9361 ENABLE control |
+-----------------------+-----------------------+-----------------------+
| TXNRX                 | FMC LA05_P (C10)      | TX/RX switching pin   |
+-----------------------+-----------------------+-----------------------+
| GPIO_0 – GPIO_5       | FMC LA06_P – LA11_P   | General purpose       |
|                       |                       | control/status        |
+-----------------------+-----------------------+-----------------------+
| CLK_IN                | FMC CLK0_M2C_P (H4)   | Reference clock from  |
|                       |                       | FPGA to AD9361        |
+-----------------------+-----------------------+-----------------------+
| CLK_OUT               | FMC CLK1_M2C_P (G2)   | Buffered clock output |
|                       |                       | from AD9361           |
+-----------------------+-----------------------+-----------------------+
| TX1A (J2)             | SMA cable to RX1A     | Transmit output       |
|                       | (J4) or antenna       | channel 1A            |
+-----------------------+-----------------------+-----------------------+
| RX1A (J4)             | SMA cable to TX1A     | Receive input channel |
|                       | (J2) or antenna       | 1A                    |
+-----------------------+-----------------------+-----------------------+
| VCC                   | FMC VADJ (3.3 V)      | I/O supply via FMC    |
|                       |                       | connector             |
+-----------------------+-----------------------+-----------------------+
| GND                   | FMC GND               | Common ground         |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    cp <SOME_PATH>/system_top.xsa .
    # to delete current build
    make reset
    # to build the project
    make
    # to flash the code
    make run

Add the following flags to the Makefile depending on the example you want to enable:

**For AD-FMCOMMS5-EBZ**

DMA example:

- NEW_CFLAGS=-DFMCOMMS5

IIO example:

- IIOD=y NEW_CFLAGS=-DFMCOMMS5
