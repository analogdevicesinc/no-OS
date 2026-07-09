AD9361/AD9364 no-OS Example Project
=====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_
* `AD-FMCOMMS3-EBZ <https://www.analog.com/AD-FMCOMMS3-EBZ>`_
* `AD-FMCOMMS4-EBZ <https://www.analog.com/AD-FMCOMMS4-EBZ>`_
* `AD-FMCOMMS5-EBZ <https://www.analog.com/AD-FMCOMMS5-EBZ>`_

Overview
--------

The `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_,
`AD-FMCOMMS3-EBZ <https://www.analog.com/AD-FMCOMMS3-EBZ>`_,
`AD-FMCOMMS4-EBZ <https://www.analog.com/AD-FMCOMMS4-EBZ>`_ and
`AD-FMCOMMS5-EBZ <https://www.analog.com/AD-FMCOMMS5-EBZ>`_ are high-speed
analog FMC modules designed to showcase the
`AD9361 <https://www.analog.com/AD9361>`_, a high-performance, highly
integrated RF Agile Transceiver intended for use in 3G and 4G base
station, test equipment, and software defined radio (SDR) applications.
The AD-FMCOMMS2-EBZ is optimized for RF engineers and targets 2.4 GHz
operation with high-performance discrete external components, while the
AD-FMCOMMS5-EBZ hosts dual AD9361 devices enabling 4x4 MIMO
configurations for advanced beamforming, angle-of-arrival, and
multi-channel SDR prototyping. Both boards are powered through the FMC
connector from the host Xilinx carrier board and provide SMA RF
connectors for TX and RX paths, as well as digital control via the SPI
and GPIO lines routed through the FMC interface.

Applications
------------

* Point to point communication systems
* Femtocell/picocell/microcell base stations
* General-purpose radio systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCOMMS2-EBZ, AD-FMCOMMS3-EBZ, AD-FMCOMMS4-EBZ
and AD-FMCOMMS5-EBZ boards are powered entirely
through the FMC connector from the host carrier board; no separate
external power supply is needed for the FMC module itself. Internally,
the AD9361 requires three supply rails: the analog supply at 1.3 V
(VDDD1P3_DIG / VDDAx), the interface supply at 1.8 V (VDD_INTERFACE),
and the GPO supply at 3.3 V (VDD_GPO). On-board voltage regulators
derive these rails from the 3.3 V and 12 V supplies available on the FMC
connector. For optimal noise performance, it is recommended that the 1.3
V analog rail be sourced from a low-noise, low-dropout (LDO) regulator
rather than directly from a switching supply.

Board Connectors
~~~~~~~~~~~~~~~~

* **J1** -- FMC LPC connector (AD-FMCOMMS2-EBZ), VITA 57.1-compliant
* **J1A / J1B** -- Dual FMC LPC connectors (AD-FMCOMMS5-EBZ)
* **J2, J3** -- SMA connectors for TX Channel 1 (TX1A, TX1B)
* **J4, J5** -- SMA connectors for RX Channel 1 (RX1A, RX1B)
* **J6, J7** -- SMA connectors for TX Channel 2 (TX2A, TX2B)
* **J8, J9** -- SMA connectors for RX Channel 2 (RX2A, RX2B)
* **J10** -- SMA connector for external reference clock input

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9361/src>`__.

This project supports six build variants selectable via ``--variant``:

Demo Example (FMComms2)
~~~~~~~~~~~~~~~~~~~~~~~

The demo example (variant ``demo``) demonstrates basic initialization and
bring-up of the AD9361 transceiver via AXI ADC/DAC cores and DMAC,
targeting the FMCOMMS2/3/4 board. It configures the AD9361 for a
specified carrier frequency, sample rate, and channel bandwidth.

DMA Example (FMComms2)
~~~~~~~~~~~~~~~~~~~~~~

The DMA example (variant ``dma-example``) demonstrates high-speed data
transfer between the AD9361 transceiver and DDR memory using the AXI DMA
core. The example configures the AD9361, then initiates a DMA transfer to
capture a block of I/Q samples from the receive path into a memory buffer,
allowing the user to verify the data path without requiring an operating
system or IIO client.

DMA-IRQ Example (FMComms2)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The DMA-IRQ example (variant ``dma-irq-example``) extends the DMA example
by enabling interrupt-driven DMA transfers. Rather than polling for DMA
completion, the firmware registers an interrupt service routine (ISR)
triggered by the DMA controller upon completion of each transfer. This
approach allows the processor to perform other tasks while a DMA transfer
is in progress.

IIO Example (FMComms2)
~~~~~~~~~~~~~~~~~~~~~~

The IIO example (variant ``iio``) launches an IIOD server on the carrier
board so that the user may connect to it via an IIO client. Using the IIO
Oscilloscope application, users can access AD9361 device settings such as
LO frequency, sampling rate, channel bandwidth, RF gain, and TX/RX path
configuration, facilitating seamless data management and real-time
streaming of I/Q samples to and from the transceiver through the AXI DMA
infrastructure.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

Demo Example (FMComms5)
~~~~~~~~~~~~~~~~~~~~~~~

The FMComms5 demo example (variant ``demo_fmcomms5``) demonstrates basic
initialization and bring-up of the dual AD9361 transceiver on the
FMCOMMS5 board for 4x4 MIMO configurations.

IIO Example (FMComms5)
~~~~~~~~~~~~~~~~~~~~~~

The FMComms5 IIO example (variant ``iio_fmcomms5``) provides an IIOD
server for the dual AD9361 transceiver on the FMCOMMS5 board, enabling
host-side data capture and playback for 4x4 MIMO configurations.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

**Note for AD-FMCOMMS4-EBZ:** This board carries the AD9364 (single
transceiver) rather than the AD9361. In ``app_config.h``, set
``AD9364_DEVICE=1`` and ``AD9361_DEVICE=0`` before building.

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_,
  `AD-FMCOMMS3-EBZ <https://www.analog.com/AD-FMCOMMS3-EBZ>`_,
  `AD-FMCOMMS4-EBZ <https://www.analog.com/AD-FMCOMMS4-EBZ>`_, or
  `AD-FMCOMMS5-EBZ <https://www.analog.com/AD-FMCOMMS5-EBZ>`_
* One of: `ZedBoard <https://digilent.com/shop/zedboard-zynq-7000-arm-fpga-soc-development-board/>`_,
  `ZC706 <https://www.xilinx.com/ZC706>`_,
  ZC702, `ZCU102 <https://www.xilinx.com/ZCU102>`_, or
  `KCU105 <https://www.xilinx.com/KCU105>`_ carrier board

Connections
^^^^^^^^^^^

The AD-FMCOMMS2-EBZ/3-EBZ/4-EBZ connects to the Xilinx carrier board
via its FMC LPC connector. The AD-FMCOMMS5-EBZ requires a dual FMC slot
(available on ZC706 and ZC702).

Connect a USB cable to the carrier board USB-UART port and the host PC
for serial console access at 115200 baud, 8N1.

Board and variant availability:

+------------------+--------------------------------------------------+
| Board            | Supported variants                               |
+==================+==================================================+
| zed              | demo, dma-example, dma-irq-example, iio          |
+------------------+--------------------------------------------------+
| zc702            | demo, dma-example, dma-irq-example, iio,         |
|                  | demo_fmcomms5, iio_fmcomms5                      |
+------------------+--------------------------------------------------+
| zc706            | demo, dma-example, dma-irq-example, iio,         |
|                  | demo_fmcomms5, iio_fmcomms5                      |
+------------------+--------------------------------------------------+
| zcu102           | demo, dma-example, dma-irq-example, iio,         |
|                  | demo_fmcomms5, iio_fmcomms5                      |
+------------------+--------------------------------------------------+
| kcu105           | demo, dma-example, dma-irq-example, iio          |
+------------------+--------------------------------------------------+

Build Command
^^^^^^^^^^^^^

The Xilinx platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script.

For toolchain setup and prerequisites, see the
`Xilinx CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_xilinx_cmake.html>`__.

.. code-block:: bash

   # Source the Vitis toolchain environment (adjust version as needed)
   source ~/.xilinx/2025.1/Vitis/settings64.sh

   cd no-OS

   # build the demo example on ZedBoard
   python tools/scripts/no_os_build.py build \
      --project ad9361 --variant demo --board zed \
      --hardware /path/to/system_top.xsa

   # build and flash
   python tools/scripts/no_os_build.py build \
      --project ad9361 --variant demo --board zed \
      --hardware /path/to/system_top.xsa \
      --probe openocd --flash

   # build the IIO example on ZCU102
   python tools/scripts/no_os_build.py build \
      --project ad9361 --variant iio --board zcu102 \
      --hardware /path/to/system_top.xsa

   # build the FMComms5 demo on ZC706
   python tools/scripts/no_os_build.py build \
      --project ad9361 --variant demo_fmcomms5 --board zc706 \
      --hardware /path/to/system_top.xsa

   # build the DMA-IRQ example on KCU105
   python tools/scripts/no_os_build.py build \
      --project ad9361 --variant dma-irq-example --board kcu105 \
      --hardware /path/to/system_top.xsa
