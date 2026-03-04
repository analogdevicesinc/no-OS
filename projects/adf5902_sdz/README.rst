ADF5902_SDZ No-OS Example Project
=================================

.. contents::
    :depth: 3

Supported Evaluation Board
--------------------------

- :adi:`EV-ADF5902SD1Z`

Overview
--------

The ADF5902_SDZ is a microwave frequency synthesizer tailored for RF and
automotive applications, notably FMCW radar systems, offering frequency
output up to 24 GHz. It features essential capabilities for radar,
including ramp and modulation, a temperature sensor, frequency
computation, and recalibration, all managed via SPI and GPIO interfaces
on the Xilinx platform. The no-OS example project illustrates the
processes of device initialization, calibration, and operation without
operating system dependency, effectively managing SPI communications and
GPIO controls. Practical applications are found in RF signal processing
where precise frequency synthesis and control are crucial.

Applications
------------

- Automotive Radars
- Industrial Radars
- Microwave Radar Sensors

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The EV-ADF5902SD1Z microwave frequency synthesizer requires a 5V
external power supply. This supply supports the stable operation crucial
for microwave synthesizers used in automotive radar systems. The board
utilizes SPI and GPIO interfaces for component initialization and
parameter setting, essential for operating the ADF5902. While specific
power management features beyond these basics are not detailed in the
source code documentation, the user guide and related documents provide
further insights into power supply needs, ensuring compatibility and
integration with radar system components.

+-----------------------------------+-----------------------------------+
| **Parameter**                     | **Details**                       |
+-----------------------------------+-----------------------------------+
| **Power Supply Voltage**          | 5V external power supply          |
+-----------------------------------+-----------------------------------+
| **Current Requirements**          | Approximately 190mA in normal     |
|                                   | operation                         |
+-----------------------------------+-----------------------------------+
| **Power-Down Current**            | 1.2mA in software power-down      |
|                                   | mode, 200µA in hardware           |
|                                   | power-down mode                   |
+-----------------------------------+-----------------------------------+
| **Operating Temperature**         | -40°C to +105°C                   |
+-----------------------------------+-----------------------------------+
| **Voltage Range Constraints**     | 3.3V +- 5% for all input power    |
|                                   | pins such as AH1, TX_AHI, and     |
|                                   | RF_AHI                            |
+-----------------------------------+-----------------------------------+
| **Environmental Conditions**      | Requires proper grounding; all    |
|                                   | GND pins connected together, and  |
|                                   | exposed pad tied to GND           |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path: <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adf5902_sdz/src>`__.

Application Example
-------------------

The application example for the ADF5902_SDZ no-OS project demonstrates
the initialization, calibration, and frequency locking of the ADF5902
device. Key operations include using ``adf5902_init`` and
``adf5902_recalibrate`` for initialization and calibration, calculating
the locked frequency with ``adf5902f_compute_frequency``, and accessing
the on-chip temperature sensor with ``adf5902_read_temp``. It involves
configuring core settings like the charge pump current and reference
frequency, tailored for microwave applications.

No-OS Supported Platform
------------------------

Xilinx Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EV-ADF5902SD1Z`
- Zedboard
- :adi:`SDP-I-FMC Interposer<eval-sdp-i-fmc>`

Connections
^^^^^^^^^^^

- Connect the SPI interface of the EV-ADF5902SD1Z to the Xilinx Zedboard
  using ``SPI_DEVICE_ID`` (``XPAR_PS7_SPI_0_DEVICE_ID``). Use
  ``SPI_ADF5902_CS`` with a value of ``0`` for chip select
  configuration.

- Use ``GPIO_DEVICE_ID`` (``XPAR_PS7_GPIO_0_DEVICE_ID``) to configure
  the GPIO interface, specifically setting ``GPIO_CE`` to
  ``GPIO_OFFSET + 1`` (``86``) to handle the CE signal.

- Initialize the UART communication at a baud rate of ``115200`` using
  ``UART_DEVICE_ID`` (``XPAR_XUARTPS_0_DEVICE_ID``) for serial output.

- Ensure all necessary initialization steps are complete, including
  setting up SPI communication, configuring GPIO for chip enable
  (``GPIO_CE``), and setting up the ramp characteristics and device
  settings as seen in the main example code.

+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| Pin           | Function             | Role                | Voltage   | Configuration Parameters                             |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| GPIO_CE       | Chip Enable (CE)     | GPIO control        | 3.3V ± 5% | Enables the ADF5902 device; configured               |
|               |                      |                     |           | using ``gpio_ce_param`` as ``GPIO_OFFSET + 1``       |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| GPIO_TX_DATA  | Transmit Data        | GPIO for data       | 3.3V ± 5% | Manages transmission signals on GPIO; defined as     |
|               |                      |                     |           | ``GPIO_OFFSET + 2`` in ``parameters.h``              |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| SPI_ADF5902_CS| Chip Select for SPI  | SPI communication   | 3.3V ± 5% | Selects the ADF5902 device for SPI; chip select ``0``|
|               |                      |                     |           | in ``spi_init` (see ``adf5902_sdz.c``)               |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| SPI_MOSI      | SPI Master Out       | SPI data line       | N/A       | Transmits data from master to slave; uses            |
|               | Slave In             | (master -> slave)   |           | default settings in ``parameters.h``                 |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| SPI_MISO      | SPI Master In        | SPI data line       | N/A       | Transmits data back to master; configured            |
|               | Slave Out            | (slave -> master)   |           | according to Xilinx platform specifications          |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| SPI_CLK       | SPI Clock            | SPI synchronization | N/A       | Provides synchronization clock pulses; configured    |
|               |                      | clk                 |           | up to 2 MHz in  ``spi_init`` (``adf5902_sdz.c``)     |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+
| LE            | Load Enable          | Signal control for  | 3.3V ± 5% | Manages internal register communication; set in      |
|               |                      | device registers    |           | ``adf5902_init_param`` with `                        |
|               |                      |                     |           | ``le_sel = ADF5902_LE_FROM_PIN`                      |
+---------------+----------------------+---------------------+-----------+------------------------------------------------------+

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
