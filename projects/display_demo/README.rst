Display Demo no-OS Example Project
==================================

Overview
--------

The Display_Demo no-OS project connects an SSD1306 OLED display 
to a ZedBoard using SPI and GPIO controls. Key components,
located in the ``src`` directory, involve initializing the display with
specified hardware parameters and managing control via
``display_demo/src/app/main.c``. The implementation sets up SPI
parameters like device ID, speed, mode, and chip select, and configures
GPIO pins for data/command and reset. It initializes power supplies
(VBAT and VDD), enables caches, and controls display operations using
libraries such as ``no_os_gpio``, ``xilinx_gpio``, ``no_os_spi``, and
``ssd_1306``, ensuring efficient hardware interactions and error
management. The code then enables the display, clears it, prints a message,
and optimizes operations with instruction and data caches. This setup
exemplifies effective ZedBoard and OLED display communication through
defined hardware abstraction layers.

The initialization data used in the examples is taken out from: 
:git-no-OS:`Project Source Data Path </projects/display_demo/src/app>`.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Platforms
-------------------------

Xilinx
~~~~~~

Hardware Used
^^^^^^^^^^^^^

- SSD1306 OLED Display
- ZedBoard

Connections
^^^^^^^^^^^

+-----------------------+-----------------------+-----------------------+
| SSD1306 Pin Name      | ZedBoard Connection   | Description           |
+-----------------------+-----------------------+-----------------------+
| DC_PIN                | GPIO Pin 59           | Controls the          |
|                       | (GPIO_OFFSET + 5)     | Data/Command          |
|                       |                       | selection signal.     |
+-----------------------+-----------------------+-----------------------+
| RST_PIN               | GPIO Pin 60           | Resets the SSD1306    |
|                       | (GPIO_OFFSET + 6)     | display.              |
+-----------------------+-----------------------+-----------------------+
| VBAT_PIN              | GPIO Pin 63           | Manages power to the  |
|                       | (GPIO_OFFSET + 9)     | OLED’s VBAT.          |
+-----------------------+-----------------------+-----------------------+
| VDD_PIN               | GPIO Pin 64           | Manages power to the  |
|                       | (GPIO_OFFSET + 10)    | OLED’s VDD.           |
+-----------------------+-----------------------+-----------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/display_demo

   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run
