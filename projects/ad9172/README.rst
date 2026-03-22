AD9172 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD917x <eval-ad9172>`

Overview
--------

The EVAL-AD917x evaluation board family (AD9171-FMC-EBZ, AD9172-FMC-EBZ,
AD9173-FMC-EBZ) are FMC form-factor boards with FMC connectors that
comply with the VITA 57.1 standard, designed for evaluating the AD917x
family of high-performance, dual, 16-bit RF DACs. The AD9172, for
example, supports DAC sample rates up to 12.6 GSPS with an 8-lane, 15
Gbps JESD204B data input interface and features three complex,
bypassable data input channels per RF DAC, each with configurable gain,
interpolation filters, and numerically controlled oscillators (NCOs) for
flexible multiband frequency planning. To operate the evaluation board,
the user must attach it to a compatible FMC carrier board such as the
Xilinx ZC706 or ZCU102; alternatively, Analog Devices provides the
ADS7-V2 FPGA carrier as a data source. A low-noise HMC7044 clock
synthesizer on the evaluation board provides all required clocking,
including SYSREF for JESD204B Subclass 1 operation, while an optional
external clock input SMA allows users to supply their own low-jitter
clock source.

Applications
------------

- Wireless communications infrastructure
  - Multiband base station radios
  - Microwave/E-band backhaul systems
- Instrumentation, automatic test equipment (ATE)
- Radars and jammers

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD917x board is primarily powered from the FMC carrier board
when connected to the Xilinx ZC706 or ZCU102. The carrier board supplies
12V and 3.3V rails through the FMC connector to the evaluation board,
where on-board switching regulators and LDOs generate the internal
supply rails: 1.8V for the AD9172 analog and I/O power domains and 
1.2V for the AD9172 digital core. The HMC7044 clock chip and associated
circuitry are also powered from these on-board regulated rails. When
using the board in standalone NCO-only mode (without an FMC carrier), an
external laboratory power uspply can be connected to the board as
described in the EVAL-AD917x user guide. The FMC carrier-based power-up
sequence is managed automatically by the FPGA carrier board.

Connections
~~~~~~~~~~~

The following table lists the on-board connectors and key digital
pins/interfaces of the EVAL-AD917x evaluation board:

+-------------------------+-------------------+-----------------+-----------------+
| **Connector/Component** | **Signal /        | **Direction**   | **Description** |
|                         | Interface**       |                 |                 |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | JESD204B          | Input           | High-speed      |
|                         | SERDIN0±–SERDIN7± |                 | JESD204B        |
|                         | (8 lanes)         |                 | differential    |
|                         |                   |                 | serial data     |
|                         |                   |                 | input lanes     |
|                         |                   |                 | from FPGA       |
|                         |                   |                 | carrier         |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | SYNCOUT0±,        | Output          | JESD204B        |
|                         | SYNCOUT1±         |                 | synchronization |
|                         |                   |                 | output signals  |
|                         |                   |                 | to FPGA carrier |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | SYSREF±           | Input           | System          |
|                         |                   |                 | reference clock |
|                         |                   |                 | from HMC7044    |
|                         |                   |                 | for JESD204B    |
|                         |                   |                 | Subclass 1      |
|                         |                   |                 | operation       |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | SPI (SCLK, SDIO,  | I/O             | SPI interface   |
|                         | CSB, SDO)         |                 | for device      |
|                         |                   |                 | register        |
|                         |                   |                 | configuration   |
|                         |                   |                 | via FPGA        |
|                         |                   |                 | carrier         |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | CLK±              | Input           | Reference or    |
|                         |                   |                 | DAC clock input |
|                         |                   |                 | from HMC7044    |
|                         |                   |                 | on-board clock  |
|                         |                   |                 | synthesizer     |
+-------------------------+-------------------+-----------------+-----------------+
| P1 – FMC HPC Connector  | DACOUT0±,         | Output          | Differential RF |
|                         | DACOUT1±          |                 | DAC output      |
|                         |                   |                 | signals (via    |
|                         |                   |                 | Mini-Circuits   |
|                         |                   |                 | balun to SMA)   |
+-------------------------+-------------------+-----------------+-----------------+
| J1, J2 – SMA Output     | DAC Channel 0/1   | Output          | Balun-coupled   |
|                         | RF Output         |                 | RF DAC          |
|                         |                   |                 | differential    |
|                         |                   |                 | outputs         |
|                         |                   |                 | accessible via  |
|                         |                   |                 | SMA connectors  |
+-------------------------+-------------------+-----------------+-----------------+
| J3 – External CLK SMA   | EXT_CLK           | Input           | Optional        |
|                         |                   |                 | external        |
|                         |                   |                 | reference or    |
|                         |                   |                 | DAC clock input |
|                         |                   |                 | (sine or square |
|                         |                   |                 | wave)           |
+-------------------------+-------------------+-----------------+-----------------+
| HMC7044 (U2) On-Board   | CLKOUT, SYSREF    | Output          | Low-jitter      |
|                         |                   |                 | multi-output    |
|                         |                   |                 | clock           |
|                         |                   |                 | synthesizer     |
|                         |                   |                 | providing       |
|                         |                   |                 | device clocks   |
|                         |                   |                 | and SYSREF      |
+-------------------------+-------------------+-----------------+-----------------+
| VCC_12V / VCC_3V3       | Board Power Input | Power           | 12V and 3.3V    |
|                         |                   |                 | from FMC        |
|                         |                   |                 | carrier board   |
|                         |                   |                 | (ADS7-V2 or     |
|                         |                   |                 | FPGA carrier)   |
+-------------------------+-------------------+-----------------+-----------------+
| VCC_1V8 / VCC_1V2       | DAC Core / I/O    | Power           | On-board        |
|                         | Supply            |                 | regulators      |
|                         |                   |                 | supply 1.8V     |
|                         |                   |                 | analog and 1.2V |
|                         |                   |                 | digital rails   |
|                         |                   |                 | for AD9172      |
+-------------------------+-------------------+-----------------+-----------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Source Path </projects/ad9172/src>`.

DMA example
~~~~~~~~~~~

The DMA example initializes the AD9172 dual RF DAC device over the SPI
interface, configures the JESD204B high-speed serial link between the
FPGA and the DAC, and uses the AXI DMA controller on the Xilinx FPGA
fabric to transfer a pre-generated DAC data vector (such as a multi-tone
or sine wave LUT) from DDR memory directly to the AD9172 data input at
full speed. The example demonstrates wideband signal generation by
streaming I/Q samples through the JESD204B lanes, verifying lock of the
SERDES PLL, code group synchronization, and frame sync status, allowing
the user to observe the expected RF waveform on a spectrum analyzer
connected to the DAC output SMA connectors.

In order to build the DMA example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
~~~~~~~~~~~

The IIO example launches an IIOD server on the Xilinx FPGA carrier board
so that the user may connect to it via an IIO client. Using the IIO
Oscilloscope application, users can access device settings such as DAC
interpolation rate, channel NCO frequency offset, output scale, and
JESD204B link parameters, facilitating seamless configuration and
real-time control of the AD9172 RF DAC output signal.

If you are not familiar with ADI IIO Application, please take a look at:
`https://wiki.analog.com/resources/tools-software/no-os-software/iio`

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable

   IIOD ?=n/y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD917x Evaluation Board
- Xilinx Board (ZC706, ZCU102)

Connections
^^^^^^^^^^^

**ZC706 Carrier Board**

Connect the EVAL-AD917x FMC board to the ZC706 FMC HPC connector (J3).
The FMC connector routes all JESD204B high-speed serial lanes, clock
signals, SPI interface, and power rails between the two boards. The pin
assignments are as follows:

+-----------------------+-----------------------+-----------------------+
| **EVAL-AD917x         | **ZC706 FMC HPC Pin** | **Description**       |
| Signal**              |                       |                       |
+-----------------------+-----------------------+-----------------------+
| SERDIN0±–SERDIN7± (8  | FMC HPC DP0±–DP7±     | JESD204B high-speed   |
| JESD lanes)           |                       | serial data input     |
|                       |                       | lanes                 |
+-----------------------+-----------------------+-----------------------+
| SYNCOUT0±             | FMC HPC LA00±         | JESD204B              |
|                       |                       | synchronization       |
|                       |                       | signal from DAC to    |
|                       |                       | FPGA                  |
+-----------------------+-----------------------+-----------------------+
| SYSREF±               | FMC HPC LA01± or      | JESD204B system       |
|                       | CLK1±                 | reference from        |
|                       |                       | HMC7044               |
+-----------------------+-----------------------+-----------------------+
| CLK± (device clock)   | FMC HPC CLK0±         | DAC reference /       |
|                       |                       | device clock          |
+-----------------------+-----------------------+-----------------------+
| SPI: SCLK, SDIO, CSB, | FMC HPC LA02–LA05     | SPI configuration     |
| SDO                   |                       | interface             |
+-----------------------+-----------------------+-----------------------+
| DACOUT0± / DACOUT1±   | SMA on eval board     | Connect to spectrum   |
| (RF output)           |                       | analyzer for DAC      |
|                       |                       | output measurement    |
+-----------------------+-----------------------+-----------------------+
| VCC_12V, VCC_3V3      | FMC VADJ, VCC, power  | Carrier-provided      |
|                       | pins                  | power to eval board   |
+-----------------------+-----------------------+-----------------------+
| GND                   | FMC GND pins          | Common ground         |
+-----------------------+-----------------------+-----------------------+

**ZCU102 Carrier Board**

Connect the EVAL-AD917x FMC board to the ZCU102 FMC HPC0 connector (J5).
All JESD204B serial lanes, SPI, clocking, and power connections are
routed through this FMC interface. The pin assignments are as follows:

+-----------------------+-----------------------+-----------------------+
| **EVAL-AD917x         | **ZCU102 FMC HPC      | **Description**       |
| Signal**              | Pin**                 |                       |
+-----------------------+-----------------------+-----------------------+
| SERDIN0±–SERDIN7± (8  | FMC HPC DP0±–DP7±     | JESD204B high-speed   |
| JESD lanes)           | (HPC0)                | serial data input —   |
|                       |                       | connect to HPC0 FMC   |
+-----------------------+-----------------------+-----------------------+
| SYNCOUT0±             | FMC HPC0 LA00±        | JESD204B              |
|                       |                       | synchronization       |
|                       |                       | output from AD9172    |
+-----------------------+-----------------------+-----------------------+
| SYSREF±               | FMC HPC0 LA01± or     | System reference from |
|                       | CLK1±                 | HMC7044               |
+-----------------------+-----------------------+-----------------------+
| CLK±                  | FMC HPC0 CLK0±        | DAC reference /       |
|                       |                       | device clock          |
+-----------------------+-----------------------+-----------------------+
| SPI: SCLK, SDIO, CSB, | FMC HPC0 LA02–LA05    | SPI configuration bus |
| SDO                   |                       |                       |
+-----------------------+-----------------------+-----------------------+
| DACOUT0± / DACOUT1±   | SMA connectors on     | Connect to spectrum   |
| (RF output)           | eval board            | analyzer              |
+-----------------------+-----------------------+-----------------------+
| VCC_12V, VCC_3V3      | FMC HPC0 VADJ, VCC,   | Carrier power rails   |
|                       | power pins            |                       |
+-----------------------+-----------------------+-----------------------+
| GND                   | FMC HPC0 GND          | Common ground         |
+-----------------------+-----------------------+-----------------------+

After making the physical connections, connect the FPGA carrier board to
the host PC using the appropriate USB cable for JTAG programming and
UART communication.

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
