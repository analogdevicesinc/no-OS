AD7746-EBZ no-OS Example Project
================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD7746EBZ`

Overview
--------

The EVAL-AD7746EBZ is an evaluation board specifically designed to
facilitate the testing and development of applications using the AD7746
capacitance-to-digital converter. It features dedicated connectors for
all capacitive input and excitation signals, flexible configuration
options for interfacing via I²C, and supports both single-ended and
differential capacitance measurements.

Applications
------------

- Pressure measurement
- Position sensing
- Level sensing
- Flowmeters
- Humidity sensing
- Impurity detection

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-AD7746EBZ evaluation board primarily operates on a 5V power
supply through a USB connector (J1), utilizing an on-board 3.3V
regulator (ADP3303) to deliver the regulated voltage needed for optimal
performance. Additionally, the board supports alternative power supply
options of either 3V or 5V via an external input (J2), providing
flexibility for various configurations. A power indicator LED (D1) is
included to signal the presence of power when connected via USB,
streamlining setup by combining power supply with data communication
through the same USB interface, allowing efficient integration with
PC-based applications such as the AD7746 Evaluation Software
without needing an additional power source.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

+-----------------------------------+-----------------------------------+
| Connector Type                    | Description                       |
+-----------------------------------+-----------------------------------+
| USB connector                     | Provides 5V power supply and      |
|                                   | data communication interface via  |
|                                   | J1.                               |
+-----------------------------------+-----------------------------------+
| External Power Input              | Alternative power supply option   |
|                                   | of 3V or 5V via connector J2.     |
+-----------------------------------+-----------------------------------+
| SMB connectors (CIN1+, CIN1−,     | Used for direct interfacing with  |
| EXCA, EXCB)                       | capacitive inputs and excitation  |
|                                   | outputs, supporting capacitive    |
|                                   | measurement.                      |
+-----------------------------------+-----------------------------------+

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the example is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7746-ebz/src/app>`__

IIO example
~~~~~~~~~~~

The IIO example code within the EVAL-AD7746EBZ project initializes an
IIOD server on the AD7746 evaluation board, enabling it to connect with
IIO clients. The setup is implemented through the IIO application, which
initiates the AD7746 sensor for capacitive and temperature measurements.
This configuration includes enabling capacitance channels and setting
temperature read modes. Users can connect using an IIO client like the
IIO Oscilloscope to manage device settings, such as offset, sampling
frequency, and transform raw ADC data into voltage values. Communication
is handled via a UART interface, ensuring continuous data interaction
and flexible ADC usage.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take a
look at: 
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

To build the IIOD demo, add the following flag when invoking make. This
will build the IIOD server and the IIO section of the driver:

.. code-block:: bash

   IIOD = y

No-OS Supported Platforms
-------------------------

ADuCM3029 Platform
~~~~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-AD7746EBZ`
- :adi:`EVAL-ADICUP3029` (ADUCM3029 Evaluation Board)

Connections
^^^^^^^^^^^

Pin-level Mapping

+-----------------------+-----------------------+-----------------------+
| EVAL-AD7746EBZ LK2    | EVAL-ADICUP3029 Pin   | Function              |
| Pin                   |                       |                       |
+-----------------------+-----------------------+-----------------------+
| VDD (3.3V)            | 3.3V Output (or input | Power supply          |
|                       | if powered            |                       |
|                       | externally)           |                       |
+-----------------------+-----------------------+-----------------------+
| GND                   | GND                   | Ground                |
+-----------------------+-----------------------+-----------------------+
| SDA                   | I²C SDA (e.g., P0.14) | I²C data line         |
+-----------------------+-----------------------+-----------------------+
| SCL                   | I²C SCL (e.g., P0.15) | I²C clock line        |
+-----------------------+-----------------------+-----------------------+
| ADDR (optional)       | GND or VDD            | Sets I²C address      |
|                       |                       | (0x48 or 0x49)        |
+-----------------------+-----------------------+-----------------------+
| CAP/DIG pins          | —                     | For sensing, leave    |
|                       |                       | connected as designed |
+-----------------------+-----------------------+-----------------------+

Board-level Mapping

+-----------------------+-----------------------+-----------------------+
|                       | **Connection Point**  | **Function**          |
+-----------------------+-----------------------+-----------------------+
| EVAL-AD7746EBZ        | LK2 (SDA, SCL, RDY)   | Remove links to       |
|                       |                       | disconnect USB        |
|                       |                       | microcontroller from  |
|                       |                       | I²C bus               |
+-----------------------+-----------------------+-----------------------+
|                       | USB Connector         | Optional power source |
+-----------------------+-----------------------+-----------------------+

+-----------------------+-----------------------+-----------------------+
|                       | **Connection Point**  | **Function**          |
+-----------------------+-----------------------+-----------------------+
| EVAL-ADICUP3029       | 3.3V Rail             | Can power the         |
|                       |                       | EVAL-AD7746EBZ        |
+-----------------------+-----------------------+-----------------------+
|                       | Header J2             | Alternative power     |
|                       |                       | source                |
+-----------------------+-----------------------+-----------------------+

**Additional Notes**

- The I2C lines (SDA, SCL) should have pull-up resistors (typically
  4.7 kΩ or 10 kΩ) to the EVAL-AD7746EBZ supply voltage if not already
  present on the board.

- Confirm voltage compatibility between the I/O voltage levels of the
  ADuCM3029 and the EVAL-AD7746EBZ.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/ad7746-ebz

   # to delete current build
   make clean
   # to build the project
   make
   # to set hardware target
   make PLATFORM=aducm3029
   # to flash the code
   make run
