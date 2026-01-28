FMCDAQ2 no-OS Example Project
=============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`AD-FMCDAQ2-EBZ`

Overview
--------

The AD-FMCDAQ2-EBZ evaluation board is a high-speed data acquisition
system designed for interfacing with FPGA boards like the Xilinx ZCU706.
The board features a sophisticated signal processing system. It
incorporates the AD9144, a Quad, 16-Bit, 2800 MSPS Digital-to-Analog
Converter (DAC), ideal for digital signal generation using either Direct
Digital Synthesis (DDS) or external inputs via Direct Memory Access
(DMA). The receive path includes the AD9680, a 14-Bit, 1000 MSPS Dual
Analog-to-Digital Converter (ADC), which facilitates efficient data
transfer to DDR memory and supports spectrum analysis through an FFT
core. The key timing operations are managed by the AD9523-1, a Low
Jitter Clock Generator capable of producing multiple clock frequencies
to ensure synchronized data conversion. The system’s power is
efficiently managed through various regulators and converters, drawing
energy from an FPGA board through FPGA Mezzanine Card (FMC) interfaces.
This board is compatible with numerous platforms, including Xilinx,
making it suitable for a range of applications such as electronic test
equipment, software radios, radar systems, and more.

Applications
------------

- Electronic test and measurement equipment
- General-purpose software radios
- Radar systems
- Ultra wideband satellite receivers
- Signals intelligence (SIGINT)
- Point to point communication systems

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD-FMCDAQ2-EBZ evaluation board receives power through the FMC
connector from the attached FPGA carrier board. The board features
multiple power management components, including the ADP2384, which is 
a 20V step-down switcher delivering up to 4A, and multiple low dropout
regulators (LDOs) such as the ADP7104, ADP150, ADP1740, ADP1741, and
ADP1755. These LDOs provide output currents ranging from 150 mA to 2A
to meet various voltage requirements across the board’s components,
ensuring stable operation for devices like the AD9144 DAC and the AD9680
ADC. The design allows for precise voltage regulation entirely powered
by the connected FPGA board, eliminating the need for external power
supplies.

Connections
~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector                         | Description                       |
+-----------------------------------+-----------------------------------+
| FMC HPC Connector                 | The primary connection interface  |
|                                   | conforming to the VITA 57.1 FPGA  |
|                                   | Mezzanine Card specification,     |
|                                   | providing numerous IO pins and    |
|                                   | high-speed data paths.            |
+-----------------------------------+-----------------------------------+
| FMC LPC Connector                 | A low-pin-count interface         |
|                                   | complementary to the FMC HPC,     |
|                                   | offering additional connectivity  |
|                                   | for user-defined signals.         |
+-----------------------------------+-----------------------------------+
| JESD204B Data Interface           | Supports the high-speed serial    |
|                                   | communication protocol for        |
|                                   | connecting the AD9144 DAC and     |
|                                   | AD9680 ADC.                       |
+-----------------------------------+-----------------------------------+
| Power Connector                   | Facilitates electrical power      |
|                                   | delivery to the AD-FMCDAQ2-EBZ    |
|                                   | board from a compatible FPGA      |
|                                   | carrier board.                    |
+-----------------------------------+-----------------------------------+
| JTAG Interface                    | Used for debugging and            |
|                                   | configuring components on the     |
|                                   | board, such as the ADC and DAC    |
|                                   | devices.                          |
+-----------------------------------+-----------------------------------+
| SPI Interface                     | For serial communication with     |
|                                   | peripherals and configuration of  |
|                                   | key components like the clock     |
|                                   | generator.                        |
+-----------------------------------+-----------------------------------+
| GPIO Connectors                   | General-purpose input/output pins |
|                                   | available for custom              |
|                                   | configurations and interfacing.   |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the 
:adi:`Project Common Data Path </projects/fmcdaq2/src/app>`.

DMA example
~~~~~~~~~~~

The DMA example code for the AD-FMCDAQ2-EBZ begins by initializing and
configuring necessary components such as GPIOs, SPI, clocks, and JESD204
parameters via the ``fmcdaq2_setup`` function. The AD9680 ADC and AD9144
DAC are configured with parameters like lane rate and sampling frequency
for ADC operations and similar settings for DAC operations. DMA paths
are set using ``axi_dmac_init``, linking ADC/DAC data paths with DMA
controllers to facilitate efficient data transfer without CPU
intervention. When ``DMA_EXAMPLE`` is enabled, the AD9144 is set up for
DMA, leveraging a sine wave data source loaded into memory. The
``axi_dmac_transfer_start`` function initiates DMA transfers, while
JESD204 topologies synchronize the RX and TX paths, ensuring streamlined
data transfers. This setup allows for high-speed data streaming with
minimal latency, suitable for applications such as test equipment and
software-defined radios.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
~~~~~~~~~~~

The IIO example code for the AD-FMCDAQ2-EBZ sets up and manages signal
processing tasks by defining initialization parameters in the
``fmcdaq2_init_param`` structure, which includes configurations for the
AD9523 clock distribution, AD9144 DAC, AD9680 ADC, transceivers, and
JESD204 links. The process begins with configuring SPI interfaces for
device communication, initializing JESD204 transceivers and clocks with
functions like ``fmcdaq2_clk_init`` and ``fmcdaq2_transceiver_setup``.
This is followed by initializing DAC and ADC cores and setting up data
paths using DMA controllers for efficient data transfer. The
``iio_axi_adc_init`` and ``iio_axi_dac_init`` functions manage IIO
application parameters for ADC and DAC devices. The main function
orchestrates these operations, manages JESD204 link state machines, and
facilitates DMA data transfers to ensure the system is configured for
IIO-based signal processing before commencing data operations.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Configure the example you want to enable by setting 'y' for enable and 'n' for disable
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- AD-FMCDAQ2-EBZ
- AMD Zynq™ 7000 SoC ZC706 Evaluation Kit

Connections
^^^^^^^^^^^

- **HPC FMC connector (J37)**  
  Connect AD-FMCDAQ2-EBZ to ZC706. Ensure the FMC card is firmly seated 
  on the high‑pin‑count connector.

- **SD Card Slot (J30)**  
  Insert a compatible SD card into the designated slot.

- **HDMI Video Connector (P1)**  
  Connect HDMI display. HDMI output is provided by ADV7511 at P1.

- **USB 2.0 ULPI Controller (J49)**  
  Connect USB mouse/keyboard using the Micro‑B USB port.

- **12V Power Input Connector (J22)**  
  Connect power supply with the supplied adapter cable. Do not power on yet.

- **SW11 DIP switch**  
  Set boot mode:  
  - 1: Down  
  - 2: Down  
  - 3: Up  
  - 4: Up  
  - 5: Down  
  *(For SD boot: Down, Down, Up, Up, Down)*

- **SW12 DIP switch**  
  Set all switches to Down (off).

- **SW1 (power switch)**  
  Slide to ON. Wait for green LED (DS22) to illuminate.

- **HDMI display**  
  Confirm signal appears on display after board boots.  


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
