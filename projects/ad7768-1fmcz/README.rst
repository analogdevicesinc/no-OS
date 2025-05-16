AD7768-1 no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EV-AD7768-1FMCZ`

Overview
--------

The EV-AD7768-1FMCZ evaluation kit contains the AD7768-1, a 24-bit, 
256 kSPS ADC, and connects to a PC via the EVAL-SDP-CH1Z motherboard 
through USB. It receives power at 5V and 3.3V, regulated by the EVAL-SDP-CH1Z
for the ADC and its components. The software provided configures the
ADC’s device registers and enables performance evaluation through time
domain analysis tools, including waveform graphs and noise analysis.
This setup allows detailed exploration of the ADC’s capabilities and is
designed for professional electronic assessment and data collection.

Applications
------------

- Platform ADC to serve a superset of measurements and sensor types
   - Sound and vibration, acoustic, and material science research and
     development
   - Control and hardware in loop verification
   - Condition monitoring for predictive maintenance
   - Electrical test and measurement
   - Audio testing and current and voltage measurement
   - Clinical electroencephalogram (EEG), electromyogram (EMG), and
     electrocardiogram (ECG) vital signs monitoring
   - USB-, PXI-, and Ethernet-based modular DAQ
   - Channel to channel isolated modular DAQ designs

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

To power the EV-AD7768-1FMCZ evaluation board internally, set the VINSEL
jumper to Position B to derive power from the EVAL-SDP-CH1Z system
demonstration platform, which provides the necessary 5V and 3.3V
power. The board’s power system includes linear regulators (U8, U9, U10)
for 5V and 7V, and components ADP1718 and ADP7118 for 3.3V and 10V
regulation. AD840 and AD841 reference amplifiers stabilize operating
conditions.

Link Configuration
~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Link Name             | Default Link Option   | Description           |
+-----------------------+-----------------------+-----------------------+
| GPIO0                 | R10                   | Connects to the       |
|                       |                       | driver amplifier      |
|                       |                       | power mode.           |
+-----------------------+-----------------------+-----------------------+
| GPIO1                 | R11                   | Connects to the       |
|                       |                       | SDP-H1 board.         |
+-----------------------+-----------------------+-----------------------+
| GPIO2                 | R12                   | Connects to the       |
|                       |                       | SDP-H1 board in test  |
|                       |                       | mode.                 |
+-----------------------+-----------------------+-----------------------+
| GPIO3                 | R13                   | Connects to the       |
|                       |                       | SDP-H1 board in mode  |
|                       |                       | selection.            |
+-----------------------+-----------------------+-----------------------+
| PIN/SPI               | SPI1_A                | Selects serial        |
|                       |                       | peripheral interface  |
|                       |                       | configuration.        |
+-----------------------+-----------------------+-----------------------+
| MCLK                  | MCLK1A                | Source configuration  |
|                       |                       | for the master clock. |
+-----------------------+-----------------------+-----------------------+
| CLK_SEL               | CSEL1A                | Selects the clock     |
|                       |                       | source.               |
+-----------------------+-----------------------+-----------------------+
| XTL1                  | R36                   | Configuration for the |
|                       |                       | external oscillator.  |
+-----------------------+-----------------------+-----------------------+
| SYNC_IN               | R25                   | Synchronization       |
|                       |                       | mechanism settings.   |
+-----------------------+-----------------------+-----------------------+
| Voltage Input Select  | VVINSEL A B           | Selects power input   |
|                       |                       | source.               |
+-----------------------+-----------------------+-----------------------+

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector Type        | Connector Function    | Description           |
+-----------------------+-----------------------+-----------------------+
| SMB Connectors        | AIN1+ and AIN1-       | Provides access to    |
|                       | Analog Inputs         | the main analog input |
|                       |                       | channels.             |
+-----------------------+-----------------------+-----------------------+
| Terminal Blocks       | Additional Analog     | Alternative access    |
|                       | Inputs                | points for analog     |
|                       |                       | signals.              |
+-----------------------+-----------------------+-----------------------+
| J4 Connector          | System Demonstration  | Connects to the       |
| (EVAL-SDP-CH1Z)       | Platform Connection   | system demonstration  |
|                       |                       | platform board.       |
+-----------------------+-----------------------+-----------------------+
| P1 Socket             | Primary Board         | Main connection       |
|                       | Interface             | socket for external   |
|                       |                       | connections.          |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
~~~~~~~~~~~~~~~~~~~~~~~~

The initialization data used in the examples is taken out from the
`Project SRC Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7768-1fmcz/src>`__ .

Basic Example
-------------

The basic example demonstrates how to read data from the AD7768-1 ADC
evaluation board using a Xilinx platform. The example relies on the AXI
clock generator to synchronize the system clock at a specified
frequency. Data can be read using either direct SPI commands or an
offload mode for higher throughput, determined by compilation flags.

In order to build the basic example, make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7768-1fmcz/Makefile>`__ :

.. code-block:: bash

    # Force select an example by assigning y for enabling.
    BASIC_EXAMPLE = y


No-OS Supported Platforms
-------------------------

- Xilinx

Xilinx Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- EV-AD77681FMCZ evaluation board
- Zedboard

Connections
^^^^^^^^^^^

The EV-AD77681FMCZ evaluation board connects to a Zedboard via essential
SPI and GPIO connections to facilitate communication and control. The
SPI connections—comprising SPI_CLK (serial clock line), SPI_MISO (master
input slave output), SPI_MOSI (master output slave input), and SPI_CS
(chip select)—enable data exchange between the ADC and the Zedboard in
SPI control mode.

+-----------------+-----------------+-----------------+-----------------+
| Signal          | Description     | Zedboard        | EV-AD77681FMCZ  |
|                 |                 | Multipurpose    | Pin             |
|                 |                 | Input/Output    |                 |
|                 |                 | (MIO) Pins      |                 |
+-----------------+-----------------+-----------------+-----------------+
| SPI_CLK         | Serial Clock    | P16             | P6              |
|                 | Line            |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| SPI_MISO        | Master Input    | P17             | P5              |
|                 | Slave Output    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| SPI_MOSI        | Master Output   | P21             | P4              |
|                 | Slave Input     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| SPI_CS          | Chip Select     | P1              | P3              |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset PLATFORM=xilinx
           # to build the basic project
           make EXAMPLE=basic PLATFORM=xilinx
   # to flash the code
   make run
