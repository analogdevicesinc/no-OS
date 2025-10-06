CN0561 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-CN0561-ARDZ`

Overview
--------

The EVAL-CN0561-ARDZ is a data acquisition system tailored for vibration
analysis, primarily utilizing IEPE sensors. Designed to evaluate and
forecast mechanical health, it employs a precise 24-bit AD4134 ADC
capable of continuous-time sampling at 1.496 MSPS. Supplementing this is
the LTC6373, a differential output instrumentation amplifier, and the
LT3092, a programmable two-terminal current source, enhancing data
processing efficiency. Its compatibility with Arduino systems
facilitates integration with diverse platforms, offering robust fault
detection for increased system reliability.

Applications
------------

- Data Acquisition for IEPE Sensors
- Temperature Monitoring
- Strain Measurement
- Shock Detection
- Displacement Sensing
- Condition-Based Monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0561-ARDZ requires a DC power supply input ranging from 9V to
12V. The board is designed to draw power from a microcontroller or
development board via Arduino-style or FMCZ-type connectors. It handles
an initial inrush current of up to 200mA for power-up, while steady
state operation needs around 250mA. The power system within the board
generates necessary operational voltages: 5V and 1.8V for the AD4134
ADC, and ±15V for the instrumentation amplifier and current source
circuits. The LT3092 component provides a constant current of 2.5mA to
IEPE sensors. To accommodate potential power fluctuations, a power
source capable of delivering 1.5A consistently is recommended.

Connectors
~~~~~~~~~~~

+-----------------------+-----------------------+-----------------------+
| Connector             | Type                  | Description           |
+-----------------------+-----------------------+-----------------------+
| P1, P2, P3, P4, P5    | Arduino Connectors    | General data          |
|                       |                       | interfacing with the  |
|                       |                       | evaluation board      |
+-----------------------+-----------------------+-----------------------+
| P17                   | Arduino Zio Connector | Supports additional   |
|                       |                       | Arduino expansions    |
+-----------------------+-----------------------+-----------------------+
| P3                    | FMCZ Connector        | Used for high-speed   |
|                       |                       | data acquisition      |
+-----------------------+-----------------------+-----------------------+
| P6, P7, J6, J8        | SMA Connectors        | Interface for IEPE    |
|                       |                       | sensor connections    |
|                       |                       | and analog signals    |
+-----------------------+-----------------------+-----------------------+
| P8                    | Analog Input          | Primary input for     |
|                       |                       | data acquisition      |
|                       |                       | processes             |
+-----------------------+-----------------------+-----------------------+
| P9                    | External Power Input  | Supports 9V to 12V DC |
|                       |                       | power for board       |
|                       |                       | operation             |
+-----------------------+-----------------------+-----------------------+
| P14                   | External Supply       | Provides ±15V for     |
|                       |                       | analog front-end      |
|                       |                       | capabilities          |
+-----------------------+-----------------------+-----------------------+
| J5, J7                | Additional SMA        | Amenities for further |
|                       | Connectors            | analog channels       |
+-----------------------+-----------------------+-----------------------+
| P11, P12              | Optional Connectors   | To accommodate extra  |
|                       |                       | analog inputs as      |
|                       |                       | required              |
+-----------------------+-----------------------+-----------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0561/src>`__.

UART example
~~~~~~~~~~~~

In the CN0561 no-OS project, the UART example code establishes
communication by configuring the UART interface with key parameters such
as ``UART_DEVICE_ID``, ``UART_IRQ_ID``, and ``UART_BAUDRATE``. This
setup allows communication at a baud rate of 115200. The UART
configuration is conditionally included when IIO_SUPPORT is enabled,
integrating the UART for data exchange within the IIO application. This
interface supports interaction with the AD713x ADC device, facilitating
data acquisition and communication with external applications necessary
for industrial diagnostics.

In order to build the uart example, make sure you have the following
configuration in the build.json file:

.. code-block:: bash

   #Use this if you are using a Zedboard as carrier
   NEW_CFLAGS=-DCN0561_ZED_CARRIER

   #Use this if you are using Cora Z7S as carrier
   NEW_CFLAGS=-DCN0561_CORAZ7S_CARRIER

IIO example
~~~~~~~~~~~

The IIO demo is a standard example, provided in most `no-OS
projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client. Using the IIO Oscilloscope application, users
can configure the ADC for data acquisition, using DMA to transfer ADC
data to memory and converting raw data into voltage readings accessible
via the IIO interface.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__.

To build the IIOD demo, add the following flags when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   #Use this if you are using a Zedboard as carrier
   IIOD=y NEW_CFLAGS=-DCN0561_ZED_CARRIER

   #Use this if you are using Cora Z7S as carrier
   IIOD=y NEW_CFLAGS=-DCN0561_CORAZ7S_CARRIER

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-CN0561-ARDZ
- Cora Z7-07S Zynq-7000 or ZedBoard

Connections
^^^^^^^^^^^

+-----------------+-----------------+-----------------+-----------------+
| **Carrier       | **Connection    | **CN0561        | **Function**    |
| Board**         | Point**         | Interface**     |                 |
+-----------------+-----------------+-----------------+-----------------+
| ZedBoard        | FMC Connector   | FMC Interface   | Connect CN0561  |
|                 | (P3)            |                 | to ZedBoard via |
|                 |                 |                 | FMC for data    |
|                 |                 |                 | and power       |
+-----------------+-----------------+-----------------+-----------------+
| Power Supply    | CN0561 Board    | 9V to 12V       |                 |
| Input           |                 | input,          |                 |
|                 |                 | typically       |                 |
|                 |                 | supplied by     |                 |
|                 |                 | ZedBoard        |                 |
+-----------------+-----------------+-----------------+-----------------+
| Gigabit         | Host PC         | Connect         |                 |
| Ethernet Port   |                 | Ethernet cable  |                 |
|                 |                 | for network     |                 |
|                 |                 | communication   |                 |
+-----------------+-----------------+-----------------+-----------------+
| SMA Connectors  | IEPE Sensor     | Connect IEPE    |                 |
|                 | Inputs          | sensors to      |                 |
|                 |                 | CN0561 SMA      |                 |
|                 |                 | connectors      |                 |
+-----------------+-----------------+-----------------+-----------------+

+-----------------+-----------------+-----------------+-----------------+
| **Carrier       | **Connection    | **CN0561        | **Function**    |
| Board**         | Point**         | Interface**     |                 |
+-----------------+-----------------+-----------------+-----------------+
| Cora Z7S        | Arduino/chipKIT | Arduino         | Plug CN0561     |
|                 | Shield Header   | Compatible      | directly onto   |
|                 |                 | Header          | Cora Z7S shield |
|                 |                 |                 | connector       |
+-----------------+-----------------+-----------------+-----------------+
| Analog Inputs   | XADC Inputs     | Routed from     |                 |
| A0 to A11       | (FPGA)          | CN0561 analog   |                 |
|                 |                 | channels;       |                 |
|                 |                 | follow Arduino  |                 |
|                 |                 | convention      |                 |
+-----------------+-----------------+-----------------+-----------------+
| I/O Voltage     | CN0561 Shield   | Max 3.4V        |                 |
| Limit           |                 | (absolute max   |                 |
|                 |                 | 3.75V); CN0561  |                 |
|                 |                 | must not exceed |                 |
|                 |                 | this            |                 |
+-----------------+-----------------+-----------------+-----------------+
| Analog Input    | CN0561 Shield   | Differential:   |                 |
| Range           |                 | 0V to 1V;       |                 |
|                 |                 | Single-ended:   |                 |
|                 |                 | scaled to       |                 |
|                 |                 | 0V to 3.3V via  |                 |
|                 |                 | resistor        |                 |
|                 |                 | divider         |                 |
+-----------------+-----------------+-----------------+-----------------+
| Power Input     | Main Power      | Do not power    |                 |
|                 | Connector       | through CN0561  |                 |
|                 |                 | shield; use     |                 |
|                 |                 | Cora Z7S main   |                 |
|                 |                 | input only      |                 |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

ZedBoard:

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make PLATFORM=xilinx

   make TARGET=cn0561_zed IIOD=y NEW_CFLAGS=-DCN0561_ZED_CARRIER
   # to flash the code
   make run

Cora Z7S:

.. code-block:: bash

   cp <SOME_PATH>/system_top.xsa .
   # to delete current build
   make reset
   # to build the project
   make PLATFORM=xilinx

   make TARGET=cn0561_coraz7sIIOD=y NEW_CFLAGS=-DCN0561_CORAZ7S_CARRIER
   # to flash the code
   make run
