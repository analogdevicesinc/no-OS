EVAL-AD3552R no-OS Example Project
==================================

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD3552RFMCZ`

Overview
---------

The EVAL-AD3552RFMCZ Evaluation Board is designed for the dual-channel,
16-bit AD3552R digital-to-analog converter (DAC). It features two
channels with distinct transimpedance amplifiers—one for dynamic
performance and one for DC precision over temperature. The board
supports comprehensive testing across all DAC outputs, waveform
generation, and offers various power supply and reference options.
Connectivity is facilitated through the USB port using the SDP-H1 system
demonstration platform or via a pin header for ZedBoard or other
controllers.

Applications
-------------

- Instrumentation
- Hardware in the loop
- Process control equipment
- Medical devices
- Automated test equipment
- Data acquisition systems
- Programmable voltage sources
- Optical communications

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

To power the EVAL-AD3552RFMCZ board, connect it using either a
controller board or external power supplies via connector P3.

Pin Assignment on Power Supply Connector P3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| Pin Number            | Signal                | Description           |
+-----------------------+-----------------------+-----------------------+
| 1                     | EXT_PVDD              | External positive     |
|                       |                       | supply for            |
|                       |                       | transconductance      |
|                       |                       | amplifier. 12V ± 5%.  |
+-----------------------+-----------------------+-----------------------+
| 2                     | EXT_PVSS              | External negative     |
|                       |                       | supply for            |
|                       |                       | transconductance      |
|                       |                       | amplifier. −12V ± 5%  |
+-----------------------+-----------------------+-----------------------+
| 3                     | EXT_VDD               | External AVDD analog  |
|                       |                       | supply for AD3552R.   |
|                       |                       | 5V ± 5%.              |
+-----------------------+-----------------------+-----------------------+
| 4                     | EXT_DVDD              | External DVDD digital |
|                       |                       | supply for AD3552R.   |
|                       |                       | 1.8V ± 5%.            |
+-----------------------+-----------------------+-----------------------+
| 5                     | EXT_VLOGIC            | External VLOGIC       |
|                       |                       | digital I/O supply    |
|                       |                       | for AD3552R. 1.1V to  |
|                       |                       | 1.9V.                 |
+-----------------------+-----------------------+-----------------------+
| 6                     | GND                   | Ground.               |
+-----------------------+-----------------------+-----------------------+

Digital Signals
~~~~~~~~~~~~~~~~

Pin Assignment on Connector P5
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-------------+--------------+---------------------------------------------------------------+
| Pin Number  | Signal       | Description                                                   |
+=============+==============+===============================================================+
| 1           | +12V_FMC     | External 12 V power supply. Use this pin to supply the        |
|             |              | EVAL-AD3552RFMCZ board when using a custom controller         |
|             |              | different from the SDP-H1 or the ZedBoard.                    |
+-------------+--------------+---------------------------------------------------------------+
| 2           | POWER_ON_FMC | Enable signal for the onboard regulators. This pin is used to |
|             |              | turn on the LDOs and DC/DC converters. Set a voltage higher   |
|             |              | than 1.24 V to turn on.                                       |
+-------------+--------------+---------------------------------------------------------------+
| 3           | VIO          | Voltage supply used for AD3552R I/O pins. If this pin is      |
|             |              | used to supply VLOGIC, remove the VLOGIC link or set it to    |
|             |              | EXT.                                                          |
+-------------+--------------+---------------------------------------------------------------+
| 4           | SPI_QPI1     | Signal SPI_QPI. A high level sets the bus in Quad SPI mode.   |
|             |              | A low level sets the bus in classic SPI mode.                 |
+-------------+--------------+---------------------------------------------------------------+
| 5           | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 6           | SPI_CS1      | SPI chip select signal.                                       |
+-------------+--------------+---------------------------------------------------------------+
| 7           | SPI_SCK1     | SPI clock signal.                                             |
+-------------+--------------+---------------------------------------------------------------+
| 8           | SPI_SDI1     | SPI data input signal.                                        |
+-------------+--------------+---------------------------------------------------------------+
| 9           | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 10          | SPI_SDIO01   | SDO/MISO signal in Classic SPI mode or SDIO0 signal in        |
|             |              | Dual/Quad SPI modes.                                          |
+-------------+--------------+---------------------------------------------------------------+
| 11          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 12          | SPI_SDIO11   | SDO/MISO signal in Classic SPI mode or SDIO1 signal in        |
|             |              | Dual/Quad SPI modes.                                          |
+-------------+--------------+---------------------------------------------------------------+
| 13          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 14          | SPI_SDIO21   | SPI SDIO2 signal in Quad SPI mode.                            |
+-------------+--------------+---------------------------------------------------------------+
| 15          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 16          | SPI_SDIO31   | SPI SDIO3 signal in Quad SPI mode.                            |
+-------------+--------------+---------------------------------------------------------------+
| 17          | GND          | Ground.                                                       |
+-------------+--------------+---------------------------------------------------------------+
| 18          | /LDAC1       | LDAC signal.                                                  |
+-------------+--------------+---------------------------------------------------------------+
| 19          | /RESET1      | RESET signal.                                                 |
+-------------+--------------+---------------------------------------------------------------+
| 20          | /ALERT1      | ALERT signal.                                                 |
+-------------+--------------+---------------------------------------------------------------+

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`__

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad3552r_fmcz/src/common>`__

The macros used in Common Data are defined in platform specific files
found in the `Project Platform Configuration Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad3552r_fmcz/src/platform/>`__

AXI Quad SPI example
~~~~~~~~~~~~~~~~~~~~~~

The ``axi_qspi_example.c`` demonstrates using the AD3552R DAC via the
AXI QSPI interface, emphasizing hardware initialization, such as GPIOs,
SPI, and the DAC itself. It showcases data writing operations, including
raw sample data transfer to the DAC, with cyclic DMA capabilities. The
example supports sine wave generation via the ``no_os_sine_lut_16``
lookup table, with provisions for IIO integration for broader
compatibility with Linux kernel subsystems, offering a flexible
foundation suitable for diverse signal processing applications.

In order to build the axi_qspi example, make sure you have the following
configuration in the `Makefile: <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad3552r_fmcz/Makefile>`__

.. code-block:: bash

   # Enable AXI QSPI Example
   EXAMPLE = axi_qspi

Generic SPI example
~~~~~~~~~~~~~~~~~~~

The ``generic_spi_example.c`` code functions by initializing hardware
components like GPIOs and the SPI interface to facilitate communication
with the AD3552R DAC. It configures SPI parameters and utilizes a sine
wave lookup table for data generation. The code supports conditional
compilation to enable standalone operation or integration with the IIO
framework, which enhances data handling and device management. The main
function orchestrates system setup, ensuring proper DAC operation by
managing the initialization and execution of GPIO and SPI
configurations, ultimately allowing the DAC to process and output the
sine wave data accurately.

In order to build the axi_qspi example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   # Enable Generic SPI Example
   EXAMPLE = generic_spi

IIO example
-----------

The IIO demo is a standard example, provided in most 
`no-OS projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO-Oscilloscope application, the
user can configure the device attributes and visualize DAC sequence
executions.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__ 

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flags when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   # Enable IIO example for AXI QSPI
   EXAMPLE = axi_qspi
   IIOD = y

   # Enable IIO example for Generic SPI
   EXAMPLE = generic_spi
   IIOD = y

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-AD3552RFMCZ
- ZedBoard

Connections
^^^^^^^^^^^^

The EVAL-AD3552RFMCZ board interfaces with the ZedBoard via connector
P5, which handles both digital signal transmission and power
requirements. For power, utilize the +12V_FMC pin, and activate the
POWER_ON_FMC pin to enable the board’s regulators. I/O voltage is
selected through the VIO pin, while grounding is managed by the GND pin.
The SPI bus can operate in either Quad mode or the classic configuration
controlled by the SPI_QPI pin. Voltage reference is adjustable via
jumper J_REF for external configurations. Proper initialization requires
setting GPIO_RESET_N and GPIO_LDAC_N pins high at startup.

- Connect the :adi:`EVAL-AD3552RFMCZ` board into the ZedBoard FMC connector.
- Connect USB UART J14 (Micro USB) to your host PC.
- Plug your ethernet cable into the RJ45 ethernet connector(J11).
- Plug the Power Supply into the 12V Power input connector (J20).

+-----------------------+-----------------------+-----------------------+
| **Pin Function**      | **Pin Name**          | **Description**       |
+-----------------------+-----------------------+-----------------------+
| SPI Mode Selector     | SPI_QPI1              | High level sets to    |
|                       |                       | Quad SPI; low level   |
|                       |                       | sets to Classic SPI.  |
+-----------------------+-----------------------+-----------------------+
| SPI Chip Select       | SPI_CS1               | SPI chip select       |
|                       |                       | signal.               |
+-----------------------+-----------------------+-----------------------+
| SPI Clock             | SPI_SCK1              | SPI clock signal.     |
+-----------------------+-----------------------+-----------------------+
| SPI Data Input        | SPI_SDI1              | SPI data input        |
|                       |                       | signal.               |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 0        | SPI_SDIO01            | SDO/MISO in Classic   |
|                       |                       | SPI or SDIO0 in       |
|                       |                       | Dual/Quad mode.       |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 1        | SPI_SDIO11            | SDO/MISO in Classic   |
|                       |                       | SPI or SDIO1 in       |
|                       |                       | Dual/Quad mode.       |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 2        | SPI_SDIO21            | SDIO2 in Quad SPI     |
|                       |                       | mode.                 |
+-----------------------+-----------------------+-----------------------+
| SPI SDIO Bit 3        | SPI_SDIO31            | SDIO3 in Quad SPI     |
|                       |                       | mode.                 |
+-----------------------+-----------------------+-----------------------+
| **Special Purpose     |                       |                       |
| Pins**                |                       |                       |
+-----------------------+-----------------------+-----------------------+
| Load DAC Data         | /LDAC1                | Load DAC data.        |
+-----------------------+-----------------------+-----------------------+
| Reset                 | /RESET1               | Reset signal to       |
|                       |                       | initialize the board. |
+-----------------------+-----------------------+-----------------------+
| Alert                 | /ALERT1               | Indicates errors or   |
|                       |                       | alerts.               |
+-----------------------+-----------------------+-----------------------+

Build Command
~~~~~~~~~~~~~

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset PLATFORM=xilinx
   # to build the basic project
   make EXAMPLE=axi_qspi PLATFORM=xilinx
   # to flash the code
   make run
