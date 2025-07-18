FMCDAQ3 no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD-FMCDAQ3-EBZ`

Overview
--------

The AD-FMCDAQ3-EBZ evaluation board is a high-speed data acquisition
system designed for interfacing with FPGA boards like the Xilinx ZCU102.
Equipped with AD9680 dual ADC and AD9152 DAC, it utilizes the JESD204B/C
transceiver protocol for data transfer. The board supports multiple
connectivity options, including SD card slots, USB, and HDMI, allowing
for flexible integration in various testing setups. It can handle
digital signals at lane rates up to 12.33 Gbps and features independent
data paths for transmission and reception. The AD9528 on-board clock
generator provides precise synchronization. Configuration is managed
through software interfaces compatible with No-OS and Linux platforms,
making it adaptable to different project demands.

Applications
------------

- Electronic test and measurement equipment
- General-purpose software radios
- Radar systems
- Ultra-wideband satellite receivers
- Signals intelligence (SIGINT)
- Point to point communication systems
- DOCSIS 3.0 CMTS and HFC networks
- Multiple input/multiple output (MIMO) radios
- Automated test equipment

Hardware Specifications
------------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCDAQ3-EBZ evaluation board requires a 12V power input,
delivered through its power connector, to operate its integrated
components. This power supply supports crucial board functions,
including the AD9680 ADC, AD9152 DAC, and JESD204B/C interface
operations, as well as precise clock generation via the AD9528 clock
generator. Additionally, the board’s GPIOs manage power states across
various sections to enable or disable peripheral features, maintaining
energy efficiency and stability during operation.

On-board Connectors
-------------------

+-----------------------+-----------------------+-----------------------+
| Connector Name        | Component             | Function/Use          |
+-----------------------+-----------------------+-----------------------+
| SD Card Slot (J100)   | SD Card Interface     | Inserting SD card for |
|                       |                       | data storage          |
+-----------------------+-----------------------+-----------------------+
| AD-FMCDAQ3-EBZ FMC    | FMC Connector         | Connects to the FPGA  |
| Board                 |                       | carrier HPC FMC       |
|                       |                       | socket                |
+-----------------------+-----------------------+-----------------------+
| Display Port Video    | Video Output          | Connecting a Display  |
| Connector (J11)       |                       | Port monitor          |
+-----------------------+-----------------------+-----------------------+
| USB 2.0 UPLT          | USB Input             | Connecting USB        |
| Controller (J83)      |                       | mouse/keyboard        |
+-----------------------+-----------------------+-----------------------+
| 12V Power Input       | Power Supply          | Connecting the main   |
| Connector (J52)       |                       | power supply          |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from the 
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/fmcdaq3/src>`__

IIO example
~~~~~~~~~~~

The IIO example code for the AD-FMCDAQ3-EBZ evaluation board integrates
with the Industrial I/O (IIO) framework to facilitate standardized data
acquisition and device configuration. It sets up the necessary SPI and
GPIO interfaces, configures the AD9528 for clock distribution, and
manages data transfers using DMA controllers. The setup includes
initializing IIO application descriptors and UART communication,
enabling real-time signal processing and interaction with IIO clients.
It supports Xilinx and Altera platforms, facilitating operations such as
ADC-to-memory data transfers and PRBS tests for data integrity. The
integration with IIO allows seamless communication and control through a
unified interface, enhancing the board’s utility in diverse
applications.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

In order to build the IIO project, make sure you have the following
configuration in the `Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/fmcdaq3/Makefile>`__ :

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
--------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- AD-FMCDAQ3-EBZ
- `AMD Zynq UltraScale+™ MPSoC ZCU102 Evaluation Kit <https://www.amd.com/en/products/adaptive-socs-and-fpgas/evaluation-boards/ek-u1-zcu102-g.html>`__

Requirements
^^^^^^^^^^^^^

**Software Configuration**

The SD card must have the ``BOOT.bin``, ``Image``, and ``system.dtb``
files for booting the Zynq ZCU102 board. Full instructions on how to
build the BOOT files are available in this page: 
`Build the boot image <https://analogdevicesinc.github.io/hdl/user_guide/build_boot_bin.html>`__

**Hardware Setup**

To interface the AD-FMCDAQ3-EBZ with the AMD Zynq UltraScale+™ MPSoC
ZCU102 and run the no-OS project, follow these steps:

- Connect the AD-FMCDAQ3-EBZ to the FMC HPC0 socket on the ZCU102 board.

- Enable SD Card BOOT mode by switching SW6 to:

  ====== =====
  Switch State
  SW6-1  ON
  SW6-2  OFF
  SW6-3  OFF
  SW6-4  OFF
  ====== =====

- Insert an SD card into the SD Card Slot (J100).

- Connect a micro-USB cable to the ZCU102 board via the USB-UART
  connector (J83), and the other end of the cable to the Host PC.

- Connect the power supply to the 12V Power Input Connector (J52).

- Power ON the board, wait for the green LED to light up (located near
  the SD card slot).

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
