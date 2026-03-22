AD9361 no-OS Example Project
============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_
* `AD-FMCOMMS5-EBZ <https://www.analog.com/AD-FMCOMMS5-EBZ>`_

Overview
--------

The `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_ and
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

DMA example
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

DMA-IRQ example
~~~~~~~~~~~~~~~

The DMA-IRQ example extends the DMA example by enabling interrupt-driven
DMA transfers. Rather than polling for DMA completion, the firmware
registers an interrupt service routine (ISR) triggered by the DMA
controller upon completion of each transfer. This approach allows the
processor to perform other tasks while a DMA transfer is in progress,
and is useful for demonstrating more efficient, production-ready data
acquisition patterns for the AD9361 receive path.

In order to build the DMA-IRQ example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE -DDMA_IRQ_ENABLE

IIO example
~~~~~~~~~~~

The IIO example launches an IIOD server on the carrier board so that the
user may connect to it via an IIO client. Using the IIO Oscilloscope
application, users can access AD9361 device settings such as LO
frequency, sampling rate, channel bandwidth, RF gain, and TX/RX path
configuration, facilitating seamless data management and real-time
streaming of I/Q samples to and from the transceiver through the AXI DMA
infrastructure.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

To build the IIO project, add the following flag when invoking make:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `AD-FMCOMMS2-EBZ <https://www.analog.com/AD-FMCOMMS2-EBZ>`_ or
  `AD-FMCOMMS5-EBZ <https://www.analog.com/AD-FMCOMMS5-EBZ>`_
* `ZC706 <https://www.xilinx.com/ZC706>`_,
  `ZCU102 <https://www.xilinx.com/ZCU102>`_,
  ZC702, or ZedBoard carrier board

Connections
^^^^^^^^^^^

The AD-FMCOMMS2-EBZ connects to the Xilinx carrier board via its FMC LPC
connector. The AD-FMCOMMS5-EBZ requires a dual FMC slot (available on
ZC706 and ZC702).

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

**For AD-FMCOMMS5-EBZ**, add the following flags depending on the example:

DMA example:

.. code-block:: bash

   NEW_CFLAGS=-DFMCOMMS5

IIO example:

.. code-block:: bash

   IIOD=y NEW_CFLAGS=-DFMCOMMS5
