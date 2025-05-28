ADXRS290-PMDZ no-OS Project
===========================

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADXRS290-PMDZ`

Overview
---------

The EVAL-ADXRS290-PMDZ offers a robust evaluation platform for the
ADXRS290 high-performance MEMS gyroscope, specifically designed for
stabilization applications. The sensor measures angular rates for roll
and pitch within a ±100°/s full-scale range and outputs 16-bit two’s
complement data via a 4-wire SPI interface operating at clock speeds up
to 12 MHz. This board also features an ultralow noise floor of 0.004°/s/√Hz, 
rapid startup, and low latency.

Applications
-------------

- Optical image stabilization
- Platform stabilization
- Wearable products

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

+-----------------------+-----------------------+------------------------------------------------+
| Parameter             | Specification         | Notes                                          |
+-----------------------+-----------------------+------------------------------------------------+
| Primary Supply        | 5V (4.75V to 5.25V    | Regulated DC supply required for both digital  |
| Voltage               | tolerance)            | and analog circuits.                           |
+-----------------------+-----------------------+------------------------------------------------+

Pin Description
~~~~~~~~~~~~~~~

**SPI Connector**

+-----------------+-----------------+----------------------------------------------------------+
| Pin Name        | Pin Function    | Description                                              |
+-----------------+-----------------+----------------------------------------------------------+
| SCLK (P4)       | Clock           | Provides the timing signal for SPI communication.        |
+-----------------+-----------------+----------------------------------------------------------+
| Data In (P2)    | Input Data      | Receives data from the host processor.                   |
+-----------------+-----------------+----------------------------------------------------------+
| Data Out (P3)   | Output Data     | Transmits data from the board to the host processor.     |
+-----------------+-----------------+----------------------------------------------------------+
| CS (P1)         | Chip Select     | Selects the sensor during SPI communication.             |
+-----------------+-----------------+----------------------------------------------------------+

**Power and Ground Connector**

+-----------------+-----------------+----------------------------------------------------------+
| Pin Name        | Pin Function    | Description                                              |
+-----------------+-----------------+----------------------------------------------------------+
| VDD  (P6, P12)  | Power Supply    | Supplies the positive operating voltage to the board.    |
+-----------------+-----------------+----------------------------------------------------------+
| GND  (P5, P11)  | Ground          | Provides the common ground reference for the board.      |
+-----------------+-----------------+----------------------------------------------------------+


No-OS Build Setup
-----------------

For detailed build setup instructions, please refer to the 
:dokuwiki:`no-OS build guide </resources/no-os/build>`.

No-OS Supported Examples
------------------------

The initialization data used in the examples is sourced from the
:git-no-OS:`Project Common Data Path </projects/adxrs290-pmdz/src/common>`. 
The macros used in Common Data are defined in platform-specific files found in the 
:git-no-OS:`Project Platform Configuration Path </projects/adxrs290-pmdz/src/platform>`.

IIO Example
~~~~~~~~~~~

This example demonstrates basic sensor initialization and data
acquisition using the IIO framework in a no-OS environment. It shows how
to capture and stream data from the ADXRS290 gyroscope with minimal
configuration.

In order to build the IIO project make sure you have the following configuration in the
:git-no-OS:`Makefile <projects/adxrs290-pmdz/Makefile>`.

.. code-block:: bash

   # Enable Standard IIO Example                                                                                                                                                    
    IIO_EXAMPLE=y                                                                                                                                                                    
    IIO_TRIGGER_EXAMPLE=n                                                                                                                                                            
    IIO_TIMER_TRIGGER_EXAMPLE=n   

IIO Trigger Example
~~~~~~~~~~~~~~~~~~~

This example extends the basic IIO setup by incorporating hardware or
software triggers to synchronize sensor data capture with external
events. It is ideal for scenarios where sensor readings must be
initiated by specific trigger conditions.

.. code-block:: bash

    # Enable IIO Trigger Example                                                                                                                                                     
    IIO_EXAMPLE=n                                                                                                                                                                    
    IIO_TRIGGER_EXAMPLE=y                                                                                                                                                            
    IIO_TIMER_TRIGGER_EXAMPLE=n  

IIO Timer Trigger Example
~~~~~~~~~~~~~~~~~~~~~~~~~

This example uses timer-based triggers to achieve periodic sensor
sampling through timer interrupts. It is suited for applications
requiring consistent sample intervals to reliably monitor the ADXRS290
sensor data.

.. code-block:: bash

    # Enable IIO Timer Trigger Example                                                                                                                                               
    IIO_EXAMPLE=n                                                                                                                                                                    
    IIO_TRIGGER_EXAMPLE=n                                                                                                                                                            
    IIO_TIMER_TRIGGER_EXAMPLE=y 

No-OS Supported Platforms
-------------------------

ADuCM Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-ADXRS290-PMDZ`
- :adi:`EVAL-ADICUP3029`

Connections
^^^^^^^^^^^

1. Connect **EVAL-ADXRS290-PMDZ** board at connector **P8** of the
   **EVAL-ADICUP3029**.

2. Connect a micro-USB cable to the P10 connector of the EVAL-ADICUP3029
   and connect it to a computer.

The table below summarizes the primary physical connections between the
ADXRS290 sensor board (EVAL-ADXRS290-PMDZ) and the evaluation board
(EVAL-ADICUP3029). The table lists the board pin (or connector pin
number), a suggested connection mnemonic, and the pin function.

+-----------------+-----------------+-----------------+--------------------------------------+
| ADXRS290-PMDZ   | EVAL-ADICUP3029 | Connection      | Pin Function                         |
| Board Pin /     | Board Pin /     | Mnemonic        |                                      |
| Connector       | Connector       |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| P1-4 (SPI SCLK) | SPI_SCLK (as    | SCLK            | SPI clock – Synchronizes SPI data.   |
|                 | configured in   |                 |                                      |
|                 | pinmux)         |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| P1-2 (SPI MOSI) | SPI_MOSI (as    | MOSI            | SPI data out from master to sensor.  |
|                 | defined in      |                 |                                      |
|                 | configuration)  |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| P1-3 (SPI MISO) | SPI_MISO (as    | MISO            | SPI data in to master from sensor.   |
|                 | defined in      |                 |                                      |
|                 | configuration)  |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| P1-1 (SPI SS /  | SPI_CS (e.g.,   | CS              | Chip Select – Enables SPI sensor.    |
| Chip Select)    | CS_0 in         |                 | Active low.                          |
|                 | configuration)  |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| UART0_TX (per   | UART_RX (per    | TX              | UART transmit – MCU to host PC.      |
| configuration)  | pinmux setup)   |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| UART0_RX (per   | UART_TX (per    | RX              | UART receive – Host PC to MCU.       |
| configuration)  | pinmux setup)   |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| Dedicated GPIO  | GPIO_SYNC (as   | SYNC            | Sync signal – Triggers/captures.     |
| (SYNC signal)   | configured)     |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| IOVDD (power    | VDD (power      | Power           | Power – Regulated supply for sensor. |
| supply)         | input)          |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+
| GND             | GND             | GND             | Ground – Common reference.           |
|                 |                 |                 |                                      |
+-----------------+-----------------+-----------------+--------------------------------------+

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

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-ADXRS290-PMDZ`
- :adi:`MAX32655EVKIT`

Connections
^^^^^^^^^^^

The table below summarizes the required connections between the
EVAL‑ADXRS290‑PMDZ board and the MAX32655EVKIT evaluation board. Each
row lists the connection mnemonic, the pin identifier on each board, and
a brief description of the pin function.

+-------------------+-----------------------+-------------------+-----------------------------+
| Connection        | EVAL‑ADXRS290‑PMDZ    | MAX32655EVKIT     | Function                    |
| Mnemonic          |                       | Board Pin         |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| SPI1_SCLK         | Sensor SCLK           | GPIO1.12          | Serial Clock for SPI        |
|                   | (per board’s          | (SPI1_SCLK)       | communication               |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| SPI1_MISO         | Sensor MISO           | GPIO1.14          | Master In Slave Out for SPI |
|                   | (per board’s          | (SPI1_MISO)       | data                        |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| SPI1_MOSI         | Sensor MOSI           | GPIO1.16          | Master Out Slave In for SPI |
|                   | (per board’s          | (SPI1_MOSI)       | data                        |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| SPI1_CS_0         | Sensor CS             | GPIO1.18          | Chip Select for gyroscope   |
|                   | (per board’s          | (SPI1_CS_0)       | sensor                      |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| UART0_TX          | Debug TX              | GPIO0.20          | Transmit line for UART      |
|                   | (per board’s          | (UART0_TX)        | debugging                   |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| UART0_RX          | Debug RX              | GPIO0.22          | Receive line for UART       |
|                   | (per board’s          | (UART0_RX)        | communication               |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+
| GPIO_SYNC         | Sync/Interrupt        | GPIO1.9           | Sensor synchronization and  |
|                   | (per board’s          | (Sensor Sync)     | interrupt                   |
|                   | schematic)            |                   |                             |
+-------------------+-----------------------+-------------------+-----------------------------+


Build Command
^^^^^^^^^^^^^

Below is a consolidated set of commands to clean, build, flash, and
debug the ADXRS290-PMDZ project on a Maxim platform:

.. code-block:: bash

    # Clean the project (from the project root: no-OS/projects/adxrs290-pmdz)                                                                                                        
    make clean                                                                                                                                                                       
                                                                                                                                                                                     
    # Full project build for the MAXIM target (uses settings from builds.json)                                                                                                       
    make TARGET=maxim                                                                                                                                                                
                                                                                                                                                                                     
    # Build the IIO example application:                                                                                                                                             
    cd no-OS/projects/adxrs290-pmdz/src/examples/iio_example && make -f example.mk                                                                                                   
                                                                                                                                                                                     
    # Build the IIO Timer Trigger example:                                                                                                                                           
    cd no-OS/projects/adxrs290-pmdz/src/examples/iio_timer_trigger_example && make -f example.mk                                                                                     
                                                                                                                                                                                     
    # Flash the compiled binary to the MAXIM hardware                                                                                                                                
    make flash TARGET=maxim                                                                                                                                                          
                                                                                                                                                                                     
    # Start a debug session with debugging symbols for the MAXIM target                                                                                                              
    make debug TARGET=maxim 

PICO Platform
~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-ADXRS290-PMDZ`
- `Raspberry Pi Pico <https://www.raspberrypi.com/products/raspberry-pi-pico/>`__

Connections
^^^^^^^^^^^

+-------------+-------------------+-------------------+-------------+-----------------------------+
| Interface   | Raspberry Pi Pico | EVAL-ADXRS290-PMDZ| Connection  | Pin Function / Description  |
|             | Pin (Function)    | Pin (Signal)      | Mnemonic    |                             |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| SPI         | SPI0_TX (GP19)    | MOSI (Data In)    | SPI_TX      | Transmits data from Pico    |
|             |                   |                   |             | to sensor                   |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| SPI         | SPI0_RX (GP16)    | MISO (Data Out)   | SPI_RX      | Receives sensor data on Pico|
+-------------+-------------------+-------------------+-------------+-----------------------------+
| SPI         | SPI0_SCK (GP18)   | SCLK              | SPI_CLK     | Provides clock signal for   |
|             |                   |                   |             | SPI communication           |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| SPI         | SPI0_CS (GP17)    | CS (Chip Select)  | SPI_CS      | Activates sensor            |
|             |                   |                   |             | communication (active low)  |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| UART        | UART0_TX (GP0)    | UART_RX           | UART_TX     | Transmits debug/command     |
|             |                   |                   |             | data from Pico              |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| UART        | UART0_RX (GP1)    | UART_TX           | UART_RX     | Receives debug/command      |
|             |                   |                   |             | data on Pico                |
+-------------+-------------------+-------------------+-------------+-----------------------------+
| GPIO        | GPIO_SYNC (GP20)  | Sync/Interrupt    | GPIO_SYNC   | Synchronization and sensor  |
|             |                   |                   |             | interrupt trigger           |
+-------------+-------------------+-------------------+-------------+-----------------------------+

Ensure that the pin numbers and functions detailed here align with your
platform-specific header files and the ADXRS290 documentation before
wiring the boards.

Build Command
^^^^^^^^^^^^^

Below is a single code block containing the primary commands for
cleaning, building, flashing, and debugging the ADXRS290 project on the
PICO platform:

.. code-block:: bash

    # Navigate to the project directory (if not already there)                                                                                                                       
    cd no-OS/projects/adxrs290-pmdz/src/examples/iio_example                                                                                                                                                                                                                                                    
                                                                                                                                                                                     
    # Set the target platform to PICO                                                                                                                                                 
    make TARGET=pico

   # Clean: remove previous build artifacts                                                                                                                                         
    make clean PLATFORM=pico                                                                                                                                                         
                                                                                                                                                                                     
    # Build:                                                                                                                                                                         
    #   - Default full build (using 'all' target or simply 'make')                                                                                                                   
    make all PLATFORM=pico                                                                                                                                                           
                                                                                                                                                                                     
    # Build specific IIO examples:                                                                                                                                                   
    #   IIO example build                                                                                                                                                            
    make iio_example TARGET=pico                                                                                                                                                     
    #   IIO trigger example build                                                                                                                                                    
    make iio_trigger_example TARGET=pico                                                                                                                                             
    #   IIO timer trigger example build                                                                                                                                              
    make iio_timer_trigger TARGET=pico                                                                                                                                               
                                                                                                                                                                                     
    # Flash: program the firmware onto the PICO board                                                                                                                                
    make flash PLATFORM=pico                                                                                                                                                         
                                                                                                                                                                                     
    # Debug: launch a GDB debugging session for troubleshooting                                                                                                                      
    make debug PLATFORM=pico  

STM32 Platform
~~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-ADXRS290-PMDZ`
- :adi:`EVAL-SDP-CK1Z`

Connections
^^^^^^^^^^^

**SPI Connections**

+---------------------+---------------------+-----------------+-------------------------------+
| EVAL-ADXRS290-PMDZ  | EVAL-SDP-CK1Z       | Connection      | Pin Function                  |
| Board Pin           | Board Pin           | Mnemonic        |                               |
|                     | (STM32F469NIH6)     |                 |                               |
+---------------------+---------------------+-----------------+-------------------------------+
| SCLK (e.g., Pin 1)  | SPI1_SCLK (PA5)     | SCLK            | SPI Clock – synchronizes      |
|                     |                     |                 | data transfer                 |
+---------------------+---------------------+-----------------+-------------------------------+
| MOSI (e.g., Pin 2)  | SPI1_MOSI (PA7)     | MOSI            | Master Out, Slave In –        |
|                     |                     |                 | transmit data                 |
+---------------------+---------------------+-----------------+-------------------------------+
| MISO (e.g., Pin 3)  | SPI1_MISO (PA6)     | MISO            | Master In, Slave Out –        |
|                     |                     |                 | receive data                  |
+---------------------+---------------------+-----------------+-------------------------------+
| CS (e.g., Pin 4)    | SPI1_CS (PA4)       | CS              | Chip Select – enables the     |
|                     |                     |                 | sensor                        |
+---------------------+---------------------+-----------------+-------------------------------+

**UART Connections**

+---------------------+---------------------+-----------------+-------------------------------+
| EVAL-ADXRS290-PMDZ  | EVAL-SDP-CK1Z       | Connection      | Pin Function                  |
| Board Pin           | Board Pin           | Mnemonic        |                               |
|                     | (STM32F469NIH6)     |                 |                               |
+---------------------+---------------------+-----------------+-------------------------------+
| RX (e.g., Pin 5)    | UART_TX (e.g., PB6) | UART_TX         | UART Transmit – sends data to |
|                     |                     |                 | sensor RX                     |
+---------------------+---------------------+-----------------+-------------------------------+
| TX (e.g., Pin 6)    | UART_RX (e.g., PB7) | UART_RX         | UART Receive – receives data  |
|                     |                     |                 | from sensor TX                |
+---------------------+---------------------+-----------------+-------------------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    # Navigate to the project directory (if not already there)                                                                                                                       
    cd no-OS/projects/adxrs290-pmdz/src/examples/iio_example                                                                                                                                                                                                                                                    
                                                                                                                                                                                     
    # Set the target platform to STM32                                                                                                                                                
    export TARGET=stm32

   # Clean previous build artifacts                                                                                                                                                 
    make clean                                                                                                                                                                       
                                                                                                                                                                                     
    # Build the project for the STM32 platform (compiles both core and example applications)                                                                                         
    make all                                                                                                                                                                         
                                                                                                                                                                                     
    # Flash the compiled firmware onto the STM32F469NIH6 microcontroller                                                                                                             
    make flash                                                                                                                                                                       
                                                                                                                                                                                     
    # Launch the debugging environment for development and troubleshooting                                                                                                           
    make debug    
