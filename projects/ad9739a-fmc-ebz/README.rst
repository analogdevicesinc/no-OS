AD9739A-FMC-EBZ no-OS Example Project
=====================================

Supported Evaluation Boards
---------------------------

- :adi:`AD9739A-FMC-EBZ`

Overview
---------

The AD9739A Evaluation Board is engineered to assess the
capabilities of the AD9739A DAC, a 14-bit, 2.5 GSPS device that
efficiently synthesizes signals up to 3 GHz. This reference design is
compatible with Xilinx FPGA platforms, such as the ML605, KC705, and
VC707, supporting applications in direct digital synthesis (DDS). It
includes the AD9739A-FMC-EBZ board, a DAC FMC interposer board, and
necessary signal generators. Key software requirements include the
Xilinx ISE development environment and the ADI DPG DAC Software Suite.
Configuration is managed via SPI interfaces and initialization scripts,
facilitating integration with Xilinx tools for expansive RF signal
synthesis.

Applications
-------------

- Broadband communications systems
  - DOCSIS CMTS systems
- Military jammers
- Instrumentation and automatic test equipment
- Radar and avionics

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The AD9739A Evaluation Board requires a stable +5V DC power supply with
a capacity of 2A to function optimally. This power input supports the
board in synthesizing wideband signals up to 3 GHz, which is crucial for
applications in broadband communications and radar systems.
Additionally, the board’s power management capabilities include various
power-down modes for its components, such as the LVDS interface and DAC
bias, increasing power efficiency during operation. Configuration is
conducted via SPI, ensuring effective integration with supported
systems.

Onboard Connectors
~~~~~~~~~~~~~~~~~~

+-------------------+-----------------------------------------------------------+
| Connector         | Description                                               |
+-------------------+-----------------------------------------------------------+
| J1                | DAC output connection point for analysis with a spectrum  |
|                   | analyzer or oscilloscope.                                 |
+-------------------+-----------------------------------------------------------+
| J3                | Clock signal connection for low jitter clock source.      |
+-------------------+-----------------------------------------------------------+
| J17               | Power supply connection requiring +5V, 2A.                |
+-------------------+-----------------------------------------------------------+
| USB               | Interface for communication with a host PC running        |
|                   | relevant software.                                        |
+-------------------+-----------------------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
-------------------------

The demo applications highlight the functionality of the AD9739A
evaluation board. Three examples are provided in the sample project:

- Demo Example

- DMA Example

- IIO Example

The initialization data used in the examples is taken out from the
`Project Source Data Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad9739a-fmc-ebz/src>`__ .

Demo example
------------

The Demo example code for the AD9739A Evaluation Board begins by
initializing the AD9739A DAC via the ``ad9739a_setup`` function, setting
internal parameters and configuring operational modes through SPI
register writes. The setup entails initializing clock sources, setting
up the SPI interface, and loading waveform patterns, such as a
single-tone, using ADI’s DPG Downloader software. Essential operations
include resetting the device, configuring power modes, setting operation
modes, and adjusting the full-scale output current. The configuration
ensures compatibility with Xilinx platforms like the ML605, KC705, and
VC707. Additionally, the ``axi_dac_data_setup`` function facilitates DAC
data stream management and enables DDS mode, supporting seamless
integration with targeted RF applications.

In order to build the demo example, make sure you have the following
configuration in the `builds.json file <https://github.com/analogdevicesinc/no-OS/blob/2a253df2c05f1836734eea4a5769ff771fd0677c/projects/ad9739a-fmc-ebz/builds.json>`__ .

.. code-block:: bash

   # Select the example you want to enable 
   DEMO

DMA example
-----------

The DMA example for the AD9739A Evaluation Board sets up SPI
communication with the ADF4350 PLL and initializes the AD9739A DAC using
``adf4350_setup`` and ``axi_dac_init``. This process involves enabling
caches for efficient operation and configuring Direct Memory Access
(DMA) for high-speed data transfers from a sine lookup table into the
DAC. The ``dma_example`` target in ``builds.json`` includes the
``NEW_CFLAGS=-DDMA_EXAMPLE`` directive for enabling this DMA
functionality. Caches are disabled prior to program termination to
ensure a smooth exit. This structure allows for effective data streaming
and processing on compatible Xilinx FPGA platforms, supporting
applications that require rapid data conversion.

To enable the DMA example, make sure you have the following
configuration in the builds.json file:

.. code-block:: bash

   NEW_CFLAGS=-DDMA_EXAMPLE

IIO example
-----------

The AD9739A IIO example code is designed to initialize and configure
both the AD9739A DAC and the ADF4350 PLL on a Xilinx platform. It sets
up SPI communication, initializes the AXI DAC core specific to the
AD9739A, and configures DMA for data transfer if needed. The IIO
framework, included for UART support, facilitates remote control and
data acquisition, allowing efficient high-speed data conversion on
compatible Xilinx FPGA boards.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The No-OS IIO Application together with the AD9001 driver take care of
all the back-end logic needed to setup the IIO server.

To build the IIO Demo example, use this configuration on the builds.json
file:

.. code-block:: bash

   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx Platform
~~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- AD9739A Evaluation Board (AD9739A-FMC-EBZ)
- AMD Zynq™ 7000 SoC ZC706 Evaluation Kit

Connections
^^^^^^^^^^^^

1.  Simply plug the AD9739A-FMC-EBZ board into the FMC HPC connector on the ZC706 evaluation board.

    - Make sure the connector aligns properly and is seated firmly — the FMC
      connector is a high-density connector and should fit snugly.

2.  The FMC connector handles power and ground lines, so no additional power cable is required  
    for the AD9739A board if it’s powered via the FMC connector.

    - Just ensure the ZC706 board is powered via its standard power supply.

3. **Clock Signals and Reference**

    - Some clock or reference signals might require an external input or
      configuration depending on your setup.

    - The AD9739A-FMC-EBZ board can take clock inputs from the FMC connector
      or an onboard clock module.

4. **Optional Connections**

    - If you want to use additional peripherals like external reference
      clock sources or monitoring signals, those might require SMA cables or
      other connectors available on the AD9739A board.

**Typical Signal Mapping**

+-----------------+-----------------+-----------------+-----------------+
| AD9739A Signal  | ZC706 FMC Pin   | Direction       | Notes           |
+-----------------+-----------------+-----------------+-----------------+
| D[13:0] (LVDS   | FMC HPC LVDS    | FPGA → DAC      | Data input      |
| pairs)          |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| DACCLK+/-       | FMC Clock Input | Clock           | DAC sample      |
| (LVDS)          |                 |                 | clock           |
+-----------------+-----------------+-----------------+-----------------+
| FRAME+/−        | FMC LVDS        | FPGA → DAC      | Optional frame  |
|                 |                 |                 | signal for data |
|                 |                 |                 | alignment       |
+-----------------+-----------------+-----------------+-----------------+
| SYNC+/−         | FMC LVDS        | FPGA ← DAC      | Synchronization |
|                 |                 |                 | feedback        |
|                 |                 |                 | (optional)      |
+-----------------+-----------------+-----------------+-----------------+

To build and run the AD9739A-FMC-EBZ no-OS example project on ZC706, you
need to do the following:

**Clone the Repository**

- Run the following command to clone the project:

.. code-block:: bash

    git clone --recursive https://github.com/analogdevicesinc/no-os

- To update submodules if already cloned:

.. code-block:: bash

    git submodule update --recursive --init

**Navigate to Project Directory**

- Change to the project directory with:

.. code-block:: bash

    cd no-os/projects/ad9739a-fmc-ebz

**Build the Project**

- Compile the source files using the ``make`` command:

.. code-block:: bash

    make

- This will generate the project .hex file

**Execute Build**

- Upload the compiled program to the hardware using:

.. code-block:: bash

    make run

Build Command
~~~~~~~~~~~~~~

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
