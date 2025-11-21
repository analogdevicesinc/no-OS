AD6676-EBZ no-OS Example Project
================================

.. no-os-doxygen::
.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD6676`

Overview
--------

The EVAL-AD6676 is an evaluation board for the AD6676 high dynamic
range, wideband IF receiver. It supports frequencies from 70 MHz to 
450 MHz with an IF signal bandwidth of up to 160 MHz. The board features
digital signal processing, on-chip NCOs, and a quadrature digital
downconverter with decimation factors of 12, 16, 24, and 32. High-speed
data transfer is facilitated by JESD204B single or dual-lane outputs.
The board operates with supply voltages of 1.1V and 2.5V, consuming as
little as 1.20W. It includes an integrated attenuator with a 27 dB
range, adjustable in 1 dB increments, and supports automatic gain
control (AGC) via an AGC data port. Connection options include FMC
connectors and a SPI interface for ADC setup and control.

Applications
------------

- Wideband cellular infrastructure equipment and repeaters
- Point-to-point microwave equipment
- Instrumentation
  - Spectrum and communication analyzers
- Software defined radio

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD6676 evaluation board for the AD6676 wideband IF receiver
utilizes onboard low-dropout (LDO) regulators powered via the FMC
interface for efficient power supply management. The board requires a
reference clock of 200 MHz, with ADC frequencies reaching up to 3200
MHz. High-speed sampling necessitates careful power considerations to
ensure stability and low noise, although specific current and voltage
requirements are not provided.

Connectors and Interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Name                    | Description                       |
+-----------------------------------+-----------------------------------+
| FMC Connector                     | Used for interfacing with the     |
|                                   | evaluation platform for the       |
|                                   | AD6676, enabling high-speed data  |
|                                   | transfer.                         |
+-----------------------------------+-----------------------------------+
| SPI Interface                     | Provides setup and control        |
|                                   | functionalities for the ADC       |
|                                   | through SPI communication.        |
+-----------------------------------+-----------------------------------+
| JESD204B Interface Outputs        | Facilitates high-speed data       |
|                                   | outputs through a single or       |
|                                   | dual-lane configuration.          |
+-----------------------------------+-----------------------------------+
| Clock Inputs (CLK+ and CLK–)      | Differential clock inputs for     |
|                                   | synchronizing the ADC operation.  |
+-----------------------------------+-----------------------------------+
| Analog Inputs (VIN+ and VIN–)     | Differential inputs for analog    |
|                                   | signals.                          |
+-----------------------------------+-----------------------------------+
| GPIO (General-Purpose             | Used for configuration and        |
| Input/Output)                     | monitoring purposes within the    |
|                                   | system.                           |
+-----------------------------------+-----------------------------------+
| AGC Pins                          | Used in the automatic gain        |
|                                   | control loop for signal           |
|                                   | optimization.                     |
+-----------------------------------+-----------------------------------+
| Power Supply Connectors           | Supply power to various parts of  |
|                                   | the evaluation board, such as     |
|                                   | VDD1, VDDL, VDDQ, etc.            |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad6676-ebz/src>`__.

Basic example
~~~~~~~~~~~~~

The basic example for the AD6676 in the no-OS framework requires
initializing the hardware components of the board to facilitate its
operation. It configures the GPIO and SPI interfaces for communication
with the AD6676 and sets up the JESD204 transport protocol using AXI
cores to ensure effective data transmission. Additionally, the example
configures ADC core parameters. The code primarily executes
initialization routines for the JESD, XCVR cores, and DMAC to support
data transfers.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIOD = n

IIO example
~~~~~~~~~~~

The IIO example in the AD6676 no-OS project configures the AD6676 device
to interact with an IIO client for data capture and processing. The
process involves initializing necessary platform resources, including
GPIO and SPI, for device configuration. Key operations include setting
up the JESD204 interface and configuring the AD6676 for data
acquisition. The IIO application is set up with parameters such as
device descriptors and buffer settings, which are essential for data
transfer. An IIO daemon runs, enabling real-time data capture through
the IIO client interface, allowing manipulation of device settings such
as frequency and offset for effective data management.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   BASIC_EXAMPLE = n
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD6676
- AMD Zynq™ 7000 SoC ZC706 Evaluation Kit

Connections
^^^^^^^^^^^

- Plug the AD6676EBZ FMC connector into the HPC (J37) connector on the
  ZC706 board. The connector and board are keyed to ensure correct
  orientation; the evaluation board should face away from the main ZC706
  PCB.

- The AD6676EBZ receives its power directly via the FMC connector from
  the ZC706 board. No additional external power is needed if correctly
  mounted.

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
