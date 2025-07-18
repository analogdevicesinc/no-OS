AD9208 no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Board
---------------------------

- :adi:`AD9208-DUAL-EBZ`

Supported Carrier
-----------------

- Xilinx VCU118 FPGA Evaluation Kit

Overview
--------

The AD9208-DUAL-EBZ is an evaluation board designed for demonstrating
the synchronization of dual AD9208 analog-to-digital converters (ADCs)
using the HMC7044 clock synthesizer. It provides integrated clocking
solutions not only for the ADCs but also for an associated Field
Programmable Gate Array (FPGA), operating on a single external 12V
supply. It interfaces with an FPGA development board, such as the
VCU118, through an FMC+ (Vita57.4) connector. Leveraging Analog Devices’
JESD204B IP framework, the board ensures efficient data transfer for
high-speed applications while utilizing low SWaP high efficiency power
delivery using Silent Switcher technology. This makes it suitable for
high-speed data conversion, signal processing, and wideband
communication systems requiring integration with the VCU118 platform.

Applications
------------

- Diversity multiband, multimode digital receivers
- 3G/4G
- Electronic test and measurement systems
- Phased array radar and electronic warfare
- DOCSIS 3.0 CMTS upstream receive paths
- HFC digital reverse path receivers

Hardware Specifications
-----------------------

To establish hardware connections between the AD9208-DUAL-EBZ evaluation
board and the VCU118 FPGA board, the following interfaces are essential:

- **FMC+ Connector**: Ensures physical connectivity and supports
  high-bandwidth data exchange.

- **JESD204B Protocol**: Links the AD9208 board with the VCU118 via
  serial connections using SERDOUT0± to SERDOUT7±.

- **SPI Communication**: Configures the AD9208, allowing control over
  operating parameters and settings.

- **GPIO**: Handles power management tasks.

- **AXI Interfaces**: Facilitate data transfers within the FPGA,
  supporting high-throughput operations.

- **UART Connections**: Established for communication with IIOD
  applications, aiding in remote control and diagnostics.

To ensure stable operation, power delivery and management are handled
via specific GPIO configurations.

+-----------------------+-----------------------+-----------------------+
| Pin Category          | Pin Names             | Description           |
+-----------------------+-----------------------+-----------------------+
| Digital Input/Output  | DIGITAL_CAL, SCL,     | Pins used for digital |
|                       | SDA, LDAC             | control and           |
|                       |                       | configuration.        |
+-----------------------+-----------------------+-----------------------+
| Decoupling and Power  | C7A, C8A, C178        | Capacitors used for   |
|                       |                       | voltage stabilization |
|                       |                       | and noise reduction.  |
+-----------------------+-----------------------+-----------------------+
| Signal Integrity      | DIG_OUT, DIG_CLK      | Pins that ensure      |
|                       |                       | signal reliability    |
|                       |                       | across connections.   |
+-----------------------+-----------------------+-----------------------+
| Analog Input          | J2A, J2B, T1A, T1B    | Inputs for analog     |
|                       |                       | signals, including    |
|                       |                       | amplifiers and        |
|                       |                       | associated            |
|                       |                       | components.           |
+-----------------------+-----------------------+-----------------------+
| FMC Connector         | GND, R1, T1, OUT, IN  | Pins used for         |
|                       |                       | interfacing different |
|                       |                       | components through an |
|                       |                       | FMC connector.        |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9208/src>`__.

IIO example
~~~~~~~~~~~

The IIO example code for the EVAL-AD9208 project establishes multiple
components necessary for data acquisition using the Industrial I/O (IIO)
framework. It configures two AD9208 ADC devices, setting up JESD204
interfaces for high-speed data transfer and DMAC for data movement.
These components connect to data capture pipelines, utilizing DMA to
store acquired data in DDR memory. The code also initializes IIO
interfaces to enable remote communication and control through libiio
clients. The setup involves UART connectivity, buffer management, and
data transfer processes. Inputs are configuration structures and
parameters tailored for hardware, while outputs are ADC samples
available via IIO clients.

This project is actually an IIOD demo for the AD9208-DUAL-EBZ evaluation
board. The project launches the IIOD server on the board, so that the
user may connect to it to the IIO Oscilloscope Client app. Using the
IIO Oscilloscope, the user can read samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The IIO Application together with the no-OS IIO AD9208 driver take
care of all the back-end logic needed to setup the IIO server.

In order to build the IIO example project, make sure you have the
following configuration in the `Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad9208/Makefile>`__ :

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- AD9208-DUAL-EBZ
- Xilinx VCU118 FPGA Board

Connections
^^^^^^^^^^^

The AD9208-DUAL-EBZ evaluation board connects to the Xilinx VCU118 FPGA
board through the FMC HSPC connector. The JESD204B interface of the
AD9208 eval board is connected through serial links (SERDOUT0± to
SERDOUT7±) to the VCU118 board for data transmission. Configuration of
the AD9208 is facilitated via SPI, enabling specific parameter settings.
Power management on the AD9208 board is managed through GPIO
connections.

Additionally, the VCU118 FPGA board utilizes AXI buses to facilitate
data transfer between the ADC and the processing elements on the FPGA.
For communication with the IIOD application, UART connections are
established. This setup is essential for applications involving
real-time signal processing and data acquisition.

This table outlines the critical connections and their functions for
both the AD9208-DUAL-EBZ evaluation board and the VCU118 board, ensuring
seamless integration into high-speed data conversion and signal
processing applications.

+-----------------------+-----------------------+-----------------------+
| Connection Interface  | Board                 | Description           |
+-----------------------+-----------------------+-----------------------+
| **JESD204B Serial     | AD9208-DUAL-EBZ       | Utilizes SERDOUT0± to |
| Links (P2)**          |                       | SERDOUT7± interfaces  |
|                       |                       | for data connection   |
|                       |                       | with the FPGA.        |
|                       |                       | Configures link       |
|                       |                       | parameters including  |
|                       |                       | lanes (L), converters |
|                       |                       | (M), and octets per   |
|                       |                       | frame (F) based on    |
|                       |                       | application needs.    |
|                       |                       | Uses JESD204B         |
|                       |                       | protocol.             |
+-----------------------+-----------------------+-----------------------+
| **SPI (P2)**          | AD9208-DUAL-EBZ       | Device configuration  |
|                       |                       | via SPI write to set  |
|                       |                       | various operating     |
|                       |                       | modes and parameters  |
|                       |                       | necessary for ADC     |
|                       |                       | operation. Includes   |
|                       |                       | enabling features     |
|                       |                       | such as NCO switching |
|                       |                       | and gain adjustment.  |
+-----------------------+-----------------------+-----------------------+
| **GPIO (P2)**         | AD9208-DUAL-EBZ       | Employs GPIO for      |
|                       |                       | power management and  |
|                       |                       | selection of specific |
|                       |                       | device states or      |
|                       |                       | channels. Critical    |
|                       |                       | for operational       |
|                       |                       | accuracy in signal    |
|                       |                       | processing            |
|                       |                       | applications.         |
+-----------------------+-----------------------+-----------------------+
| **AXI Connection      | VCU118                | Establishes data      |
| (P2)**                |                       | streams through AXI   |
|                       |                       | buses for             |
|                       |                       | communication between |
|                       |                       | the AD9208 ADC        |
|                       |                       | outputs and FPGA      |
|                       |                       | processing units.     |
|                       |                       | Facilitates data      |
|                       |                       | transfer crucial for  |
|                       |                       | high-throughput       |
|                       |                       | environments.         |
+-----------------------+-----------------------+-----------------------+
| **UART                | VCU118                | Supports interactions |
| Communication**       |                       | with IIOD             |
|                       |                       | applications via UART |
|                       |                       | channels. Essential   |
|                       |                       | for remote controls   |
|                       |                       | and diagnostics in    |
|                       |                       | data acquisition      |
|                       |                       | systems.              |
+-----------------------+-----------------------+-----------------------+
| **FMC + HSPC          | VCU118                | Enables high-speed    |
| Connector (J22)**     |                       | data exchange between |
|                       |                       | FMC cards and the     |
|                       |                       | FPGA. Provides 160    |
|                       |                       | single-ended or 80    |
|                       |                       | differential signals, |
|                       |                       | as well as multiple   |
|                       |                       | clock and power       |
|                       |                       | connections crucial   |
|                       |                       | for board             |
|                       |                       | integration.          |
+-----------------------+-----------------------+-----------------------+
| **Ethernet Connector  | VCU118                | Provides network      |
| (J10)**               |                       | connectivity vital    |
|                       |                       | for board-based       |
|                       |                       | communications and    |
|                       |                       | data transactions     |
|                       |                       | across networked      |
|                       |                       | systems.              |
+-----------------------+-----------------------+-----------------------+
| **USB-to-UART Bridge  | VCU118                | Converts USB data     |
| (J4)**                |                       | signals into UART     |
|                       |                       | protocol for direct   |
|                       |                       | FPGA communication,   |
|                       |                       | facilitating serial   |
|                       |                       | data flow essential   |
|                       |                       | for debugging and     |
|                       |                       | setup processes.      |
+-----------------------+-----------------------+-----------------------+
| **Power Input         | VCU118                | Connects power supply |
| Connector (J15)**     |                       | to the evaluation     |
|                       |                       | board. Ensures stable |
|                       |                       | power delivery        |
|                       |                       | necessary for         |
|                       |                       | dependable board      |
|                       |                       | operations in various |
|                       |                       | conditions.           |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^^

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset PLATFORM=xilinx
   # to build the IIO project
   make EXAMPLE=iio PLATFORM=xilinx
   # to flash the code
   make run
