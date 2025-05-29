AD74416H no-OS Project
======================

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD74416H`

Overview
---------

The AD74416H is a quad-channel, software-configurable device designed
for robust industrial control applications. It offers versatile analog
and digital I/O capabilities for measuring voltage, current, RTD, and
thermocouples while incorporating a 16-bit DAC and a 24-bit sigma-delta
ADC with optional 50Hz/60Hz rejection. Each channel features an
integrated HART modem that provides both HART and SPI communications
using configurable pin assignments that support multiple operation
modes. This is complemented by adaptive power switching, built-in
diagnostics for open-circuit/short-circuit detection, a high-accuracy
2.5V reference, and auxiliary channels for sensing and power supply
measurements.

Applications
------------

- Industrial control systems
- Process control
- Factory automation

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The following tables summarize the power supply requirements for the
EVAL-AD74416H board.

Voltage Requirements
~~~~~~~~~~~~~~~~~~~~

+-------------------+-------------------+-------------------+-------------------+
| Supply            | Minimum Voltage   | Typical Voltage   | Maximum Voltage   |
+===================+===================+===================+===================+
| Operating Supply  | 24 V              | 24 V              | 24 V              |
+-------------------+-------------------+-------------------+-------------------+
| AVDD_HI           | 6 V               | 24 V              | 28.8 V            |
+-------------------+-------------------+-------------------+-------------------+
| AVDD_LO           | 6 V               | 14.5 V            | 28.8 V            |
+-------------------+-------------------+-------------------+-------------------+
| AVSS              | -18 V             | -15 V             | -2.5 V            |
+-------------------+-------------------+-------------------+-------------------+
| DVCC              | 2.7 V             | 3.3 V             | 5.5 V             |
+-------------------+-------------------+-------------------+-------------------+
| AVCC              | 4.5 V             | 5.0 V             | 5.5 V             |
+-------------------+-------------------+-------------------+-------------------+
| DO_VDD            | 10 V              | 24 V              | 35 V              |
+-------------------+-------------------+-------------------+-------------------+

Quiescent Current Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------+-------------------+-------------------+-------------------+
| Supply Rail         | Minimum Current   | Typical Current   | Maximum Current   |
+=====================+===================+===================+===================+
| High Impedance      |                   |                   |                   |
| AVDD_HI             | 5.5 mA            | 7.5 mA            | 9.0 mA            |
+---------------------+-------------------+-------------------+-------------------+
| AVDD_LO             | 0 mA              | 0.3 mA            | 0.5 mA            |
+---------------------+-------------------+-------------------+-------------------+

Pin Description
---------------

The following tables list the pin assignments of the EVAL-AD74416H board
connectors that users will interact with.

**SPI Connectors**

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Pin Name        | Pin Function    | Description     |
+-----------------+-----------------+-----------------+-----------------+
| P11             | SPI Address     | SPI Address     | Sets the SPI    |
|                 | Header          | Configuration   | address of the  |
|                 |                 |                 | AD74416H;       |
|                 |                 |                 | jumper settings |
|                 |                 |                 | between Pins 1  |
|                 |                 |                 | & 2 (for AD0)   |
|                 |                 |                 | and Pins 3 & 4  |
|                 |                 |                 | (for AD1)       |
|                 |                 |                 | determine the   |
|                 |                 |                 | default address |
|                 |                 |                 | 00.             |
+-----------------+-----------------+-----------------+-----------------+
| P36             | SDO Enable      | SPI Data Output | Hosts the SDO   |
|                 |                 | Enable          | enable jumper;  |
|                 |                 |                 | when enabled,   |
|                 |                 |                 | the AD74416H    |
|                 |                 |                 | generates the   |
|                 |                 |                 | SDO signal.     |
+-----------------+-----------------+-----------------+-----------------+
| P38             | SPI Signals     | SPI             | Routes SPI      |
|                 |                 | Communication   | signals         |
|                 |                 |                 | including SYNC, |
|                 |                 |                 | SDI, and SCLK.  |
+-----------------+-----------------+-----------------+-----------------+
| P62             | SPI             | SPI Enable      | Enables the     |
|                 | Interconnection |                 | overall SPI     |
|                 |                 |                 | interconnection |
+-----------------+-----------------+-----------------+-----------------+
| P8              | SYNC Signal     | SYNC Source     | Selects the     |
|                 | Header          | Selection       | source for the  |
|                 |                 |                 | SYNC (chip      |
|                 |                 |                 | select) signal; |
|                 |                 |                 | for evaluation  |
|                 |                 |                 | with ACE        |
|                 |                 |                 | software, the   |
|                 |                 |                 | jumper must be  |
|                 |                 |                 | set between Pin |
|                 |                 |                 | 1 and Pin 3.    |
+-----------------+-----------------+-----------------+-----------------+

**SPI/Digital Power**

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Pin Name        | Pin Function    | Description     |
+-----------------+-----------------+-----------------+-----------------+
| P7              | +3V3_IO Power   | Digital Power   | Supplies        |
|                 |                 | Supply          | +3V3_IO power   |
|                 |                 |                 | to the          |
|                 |                 |                 | isolators on    |
|                 |                 |                 | the Arduino     |
|                 |                 |                 | side via a      |
|                 |                 |                 | jumper          |
|                 |                 |                 | connection      |
|                 |                 |                 | between Pin 1   |
|                 |                 |                 | and Pin 2.      |
+-----------------+-----------------+-----------------+-----------------+

**Digital Signals and Reset**

+-----------------+-----------------+-----------------+-----------------+
| Pin Number      | Pin Name        | Pin Function    | Description     |
+-----------------+-----------------+-----------------+-----------------+
| P34             | ALERT Driver    | Digital Signal  | Bypasses the    |
|                 | Bypass          |                 | open collector  |
|                 |                 |                 | driver for the  |
|                 |                 |                 | ALERT signal; a |
|                 |                 |                 | red LED (DS8)   |
|                 |                 |                 | indicates       |
|                 |                 |                 | activation.     |
+-----------------+-----------------+-----------------+-----------------+
| P35             | ADC_RDY Driver  | Digital Signal  | Bypasses the    |
|                 | Bypass          |                 | open collector  |
|                 |                 |                 | driver for the  |
|                 |                 |                 | ADC_RDY signal; |
|                 |                 |                 | an orange LED   |
|                 |                 |                 | (DS9) indicates |
|                 |                 |                 | activation.     |
+-----------------+-----------------+-----------------+-----------------+
| P37             | ALERT/ADC_RDY   | Signal Enable   | Enables the     |
|                 | Enable          |                 | ALERT and       |
|                 |                 |                 | ADC_RDY signals |
|                 |                 |                 | generated by    |
|                 |                 |                 | the AD74416H.   |
+-----------------+-----------------+-----------------+-----------------+
| P39             | RESET/GPIO      | Reset and GPIO  | Manages the     |
|                 |                 |                 | RESET signal    |
|                 |                 |                 | along with      |
|                 |                 |                 | additional GPIO |
|                 |                 |                 | functions on    |
|                 |                 |                 | the Arduino     |
|                 |                 |                 | side.           |
+-----------------+-----------------+-----------------+-----------------+
| P40             | Additional GPIO | GPIO            | Provides        |
|                 |                 |                 | additional GPIO |
|                 |                 |                 | control on the  |
|                 |                 |                 | Arduino side.   |
+-----------------+-----------------+-----------------+-----------------+

**Arduino Header Connector**

Facilitates integration with the Arduino ecosystem by providing power
through isolators and enabling signals; works in conjunction with jumper
settings on P36 and P38.

No-OS Build Setup
-----------------

Please see: :dokuwiki:`No-OS Build Setup </resources/no-os/build>`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
:git-no-OS:`Project Common Data Path </projects/ad74416h/src/common>`. 
The macros used in Common Data are defined in platform-specific files found in:
:git-no-OS:`Platform Platform Configuration Path </projects/ad74416h/src/platform>`.

Basic Example
~~~~~~~~~~~~~

The Basic Example demonstrates the fundamental initialization and
configuration of the AD74416H device by setting up communication
interfaces such as UART, SPI, and GPIO using common data structures in a
no-OS environment. To enable the Basic Example and disable other
examples, update the :git-no-OS:`Makefile </projects/ad74416h/Makefile>` 
with the following settings:

.. code-block:: bash

    # Enable Basic Example 
    EXAMPLE = basic

No-OS Supported Platforms
-------------------------

STM32 Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-AD74416H`
- :adi:`EVAL-SDP-CK1Z`

Connections
^^^^^^^^^^^

Below is a table summarizing the physical connections between the
EVAL-AD74416H board and the EVAL-SDP-CK1Z evaluation board:

+-----------------------+-----------------------+-----------------------+
| Connection            | EVAL-AD74416H Board   | EVAL-SDP-CK1Z Board   |
|                       | Pin & Function        | Pin & Function        |
+-----------------------+-----------------------+-----------------------+
| SPI_CS (SYNC)         | Arduino Header P8     | PA15 – SPI1 chip      |
|                       | - configured via      | select; configured as |
|                       | jumper (Pins 1 & 3)   | a GPIO output with    |
|                       | to select the default | very high speed       |
|                       | SPI address and       |                       |
|                       | designate the SYNC    |                       |
|                       | signal (chip select)  |                       |
+-----------------------+-----------------------+-----------------------+
| SPI_MOSI              | Arduino Header        | PA7 – SPI1_MOSI       |
|                       | - ARDUINO_UNO_D11,    | output                |
|                       | serving as Master Out |                       |
|                       | Slave In              |                       |
+-----------------------+-----------------------+-----------------------+
| SPI_MISO              | Arduino Header        | PB4 – SPI1_MISO input |
|                       | - ARDUINO_UNO_D12,    |                       |
|                       | serving as MISO       |                       |
|                       | (Master In, Slave     |                       |
|                       | Out)                  |                       |
+-----------------------+-----------------------+-----------------------+
| SPI_CLK               | Arduino Header        | PB3 – SPI1_SCK;       |
|                       | - ARDUINO_UNO_D13,    | configured with       |
|                       | delivering the SPI    | pull-up and set for a |
|                       | clock signal          | 12 Mbps clock rate    |
+-----------------------+-----------------------+-----------------------+
| UART_TX               | Defined by the        | PC12 – UART5_TX       |
|                       | UART_TX_PIN macro in  | output                |
|                       | parameters.c (UART    |                       |
|                       | transmit interface)   |                       |
+-----------------------+-----------------------+-----------------------+
| UART_RX               | Defined by the        | PD2 – UART5_RX input  |
|                       | UART_RX_PIN macro in  |                       |
|                       | parameters.c (UART    |                       |
|                       | receive interface)    |                       |
+-----------------------+-----------------------+-----------------------+
| I2C_SDA               | Defined by the        | Refer to board        |
|                       | I2C_SDA macro in      | documentation for     |
|                       | parameters.c (I2C     | exact I2C SDA pin     |
|                       | data line)            | assignment on the     |
|                       |                       | SDP-CK1Z board        |
+-----------------------+-----------------------+-----------------------+
| I2C_SCL               | Defined by the        | Refer to board        |
|                       | I2C_SCL macro in      | documentation for     |
|                       | parameters.c (I2C     | exact I2C SCL pin     |
|                       | clock line)           | assignment on the     |
|                       |                       | SDP-CK1Z board        |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    # Clean the build environment                                                                                                   
    make clean                                                                                                                                                                       
                                                                                                                                                                                     
    # Build the project                                                                             
    make all                                                                                                                                                                         
                                                                                                                                                                                     
    # Flash the firmware               
    make flash                                                                                                                                                                       
                                                                                                                                                                                     
    # Debug the project                                                                                    
    make debug   

Mbed Platform
~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-AD74416H`
- :adi:`EVAL-SDP-CK1Z`

Connections
^^^^^^^^^^^

The table below summarizes the wiring between the EVAL‑AD74416H board
and the EVAL‑SDP‑CK1Z evaluation board:

+-----------------+-----------------+-----------------+-----------------+
| Connection      | EVAL‑AD74416H   | EVAL‑SDP‑CK1Z   | Function /      |
| Mnemonic        | Board           | Board           | Description     |
|                 | (Pin/Connector) | (Pin/Connector) |                 |
+-----------------+-----------------+-----------------+-----------------+
| SPI_MISO        | MISO pin        | Arduino Uno D12 | SPI data        |
|                 |                 |                 | Master In,      |
|                 |                 |                 | Slave Out       |
+-----------------+-----------------+-----------------+-----------------+
| SPI_MOSI        | MOSI pin        | Arduino Uno D11 | SPI data        |
|                 |                 |                 | Master Out,     |
|                 |                 |                 | Slave In        |
+-----------------+-----------------+-----------------+-----------------+
| SPI_SCLK        | SPI_CLK pin     | Arduino Uno D13 | SPI clock       |
|                 |                 |                 | signal          |
+-----------------+-----------------+-----------------+-----------------+
| SPI_CS (SYNC)   | SYNC (via       | Arduino header  | SPI chip select |
|                 | header P8,      | (chip select    | (active low)    |
|                 | jumper between  | via header P8)  | - ensure proper |
|                 | Pin 1 and 3)    |                 | jumper settings |
+-----------------+-----------------+-----------------+-----------------+
| UART_TX         | UART_TX_PIN (as | UART_RX pin (as | Serial          |
|                 | defined in      | defined in      | transmission    |
|                 | parameters)     | parameters)     | from AD74416H   |
+-----------------+-----------------+-----------------+-----------------+
| UART_RX         | UART_RX_PIN (as | UART_TX pin (as | Serial          |
|                 | defined in      | defined in      | reception to    |
|                 | parameters)     | parameters)     | AD74416H from   |
|                 |                 |                 | SDP‑CK1Z board  |
+-----------------+-----------------+-----------------+-----------------+
| I2C_SDA         | I2C_SDA line    | SDA_0 (per      | I2C data line   |
|                 |                 | SDP‑CK1Z        |                 |
|                 |                 | mapping)        |                 |
+-----------------+-----------------+-----------------+-----------------+
| I2C_SCL         | I2C_SCL line    | SCL_0 (per      | I2C clock line  |
|                 |                 | SDP‑CK1Z        |                 |
|                 |                 | mapping)        |                 |
+-----------------+-----------------+-----------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    # Ensure you are in the project directory where the Makefile is located                                                                                                           
    cd path/to/your/project                                                                                                                                                          
                                                                                                                                                                         
    # Set the target platform                                                                                                               
    make TARGET=mbed                                                                                                                                                                
                                                                                                                                                                                     
    # Set the board type                                                                                                                          
    make BOARD=EVAL-AD74416H

   # Clean previous build artifacts (if applicable)                                                                                                                                 
    make clean                                                                                                                                                                       
                                                                                                                                                                                     
    # Build the project with MBED-specific drivers and configuration                                                                                                                 
    make all                                                                                                                                                                         
                                                                                                                                                                                     
    # Flash the project to your hardware (modify to your flash tool if needed)                                                                                                       
    make flash  # or use: mbed flash                                                                                                                                                 
                                                                                                                                                                                     
    # Start a debugging session                                                                                                                
    make debug  # or use: mbed debug --run  
