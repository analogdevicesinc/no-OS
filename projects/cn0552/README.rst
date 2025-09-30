CN0552 no-OS Example Project
============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-CN0552-PMDZ`

Overview
--------

The EVAL-CN0552-PMDZ is a precision measurement development tool
consists of a high-accuracy capacitive-to-digital converter (AD7746) 
which facilitates precise capacitance measurement. This evaluation board 
is highly integrated with necessary components ready for direct field a
pplication and testing. It supports various applications such as 
capacitive sensor interfacing and environmental monitoring, 
due to the AD7746’s ability to measure small capacitive changes. 
Power supply requirements ensure stable operation, allowing connection 
with various platforms, including Xilinx and STM32, 
for software development with no-OS drivers.

Applications
------------

- Capacitive sensor interfacing
- Environmental monitoring
- Liquid level monitoring
- Pressure measurement
- Position sensing
- Flowmeters
- Humidity sensing

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-CN0552-PMDZ evaluation board operates with an input voltage
between 2.7V and 5.5V, making it compatible with the standard I2C PMOD
interface. This range supports integration with various platforms,
including Xilinx and STM32. The board is designed to handle capacitive
sensing applications reliably, utilizing an integrated power management
system to stabilize operation and manage power fluctuations efficiently.
This ensures precise capacitive measurements across different
applications.

On-board Connectors
~~~~~~~~~~~~~~~~~~~

All the analog and digital input/output pins available on the
EVAL-CN0552-PMDZ are brought out to two (2) separate 8 row 0.1" 
through holes connectors P8 and P13.

============ ====== ============= ======
Connector P8 Pin(s) P13           Pin(s)
============ ====== ============= ======
EXCA         1      GND           1
EXCB         2      VDD           2
EXC_EXTENDED 3      GND           3
REFIN_POS    4      VIN_NEG       4
REFIN_NEG    5      VIN_POS       5
GND          6      GND           6
CIN1_NEG     7      CIN2_NEG      7
CIN1_POS     8      CIN2_POS      8
============ ====== ============= ======

Digital Connections
~~~~~~~~~~~~~~~~~~~

The digital communication on the EVAL-CN0552-PMDZ is accomplished using
a new standard I2C PMOD port.

**Connector P2** 
============ ======
Description  Pin(s)
============ ======
INT          6
RST          5
SCL          4
SDA          3
GND          2
IOVDD        1
============ ======

No-OS Build Setup
------------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
-------------------------

The initialization data used in the example is taken out from the
`Project Data Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/cn0552/src/app>`__

Application example
~~~~~~~~~~~~~~~~~~~

The example code for the CN0552 project initializes and configures
the I2C and UART interfaces to communicate with the AD7746
capacitive-to-digital converter. It sets excitation levels for the
AD7746 and starts the IIO application with this device. The setup
ensures that the AD7746 can be controlled and monitored via UART,
demonstrating capacitive measurement capabilities without a graphical
user interface. This configuration involves specifying UART parameters,
establishing an I2C connection, and running on the No-OS platform,
illustrating the practical application of the AD7746 in capacitive
sensing scenarios.

In order to build the basic example, make sure you have the following
configuration in the Makefile:

.. code-block:: bash

   IIOD=y

No-OS Supported Platforms
-------------------------

ADuCM Platform
~~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^^

- :adi:`EVAL-ADICUP3029`
- :adi:`EVAL-CN0552-PMDZ`

Connections
^^^^^^^^^^^^

- Attach the EVAL-CN0552-PMDZ to the female header connector **P9** of
  the EVAL-ADICUP3029.

- Connect a micro-USB cable to the **P10** port on the EVAL-ADICUP3029
  and then to your PC.

- The EVAL-CN0552-PMDZ is powered through the PMOD connector when
  attached to the ADICUP3029.

Use the I2C communication through the PMOD as configured by default for
data transfer between the boards. No additional wiring is required
beyond the above instructions for standard operation and evaluation.

\

**Sensor & Capacitor Setup**:

- For demo use, place a **2 pF capacitor** between the EXCA pin and
  CIN1(+) pin on the CN0552-PMDZ for the first channel.

- Place another **2 pF capacitor** between the EXCB pin and CIN2(+) pin
  for the second channel.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # Clean previous build artifacts
   make clean
   # Build the project with ADuCM3029-specific settings
   make aducm3029
   # Flash the newly compiled firmware onto the ADuCM device
   make flash
   # Launch a debugging session for the ADuCM platform
   make debug
