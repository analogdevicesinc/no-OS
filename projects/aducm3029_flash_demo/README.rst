ADuCM3029_Flash_Demo no-OS Project
==================================

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADICUP3029`

Overview
--------

The ADuCM3029_Flash_Demo no-OS project operates by initializing platform
components and configuring essential peripherals at startup. First, the
application initializes managed drivers and services, followed by the
setup of the interrupt controller and enabling global interrupts. It
then configures the UART0 peripheral for serial communication at a baud
rate of 115200, establishing it as the standard I/O channel for the
application. Flash memory initialization is performed next, preparing
the device for read and write operations.

The main sequence involves reading 32-bit data from flash memory
addresses 0x3E000 and 0x3E004, with the retrieved values sent out over
UART for user validation. The project proceeds to write specific
values—0xAABBCCDD to 0x3E000 and 0xCCDDAABB to 0x3E004—before
immediately reading these locations again and transmitting the results
to UART, confirming that the write operations completed successfully.

After the flash memory operations, the project disables global
interrupts, removes the initialized IRQ controller, and de-initializes
both UART and flash drivers. The application is built for robust error
handling, with each step checked for errors and a negative status code
causing an immediate exit. Upon successful execution, the application
returns zero, effectively demonstrating flash and UART peripheral
control on the ADuCM3029 platform.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Source Path </projects/aducm3029_flash_demo/src/app>`.

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Hardware Used
^^^^^^^^^^^^^

- :adi:`EVAL-ADICUP3029`

Connections
^^^^^^^^^^^

+-----------------+---------------------+-----------------+
| Function Used   | ADuCM3029 Pin       | Digital Role    |
| in the Example  |                     |                 |
| Project         |                     |                 |
+-----------------+---------------------+-----------------+
| **UART0_TX**    | **P0_10**           | TX              |
+-----------------+---------------------+-----------------+
| **UART0_RX**    | **P0_11**           | RX              |
+-----------------+---------------------+-----------------+
| **SPI2_CLK**    | **P1_02 (GPIO18)**  | SPI Clock       |
+-----------------+---------------------+-----------------+
| **SPI2_MOSI**   | **P1_03 (GPIO19)**  | MOSI            |
+-----------------+---------------------+-----------------+
| **SPI2_MISO**   | **P1_04 (GPIO20)**  | MISO            |
+-----------------+---------------------+-----------------+
| **SPI2_CS0**    | **P1_05 (GPIO21)**  | Chip Select     |
+-----------------+---------------------+-----------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/aducm3029_flash_demo 

   export PLATFORM=aducm3029 
   # to build the project 
   make 
   # to flash the code 
   make run
