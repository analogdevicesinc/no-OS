ADUCM Blinky no-OS Example Project
==================================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-ADICUP3029`

Overview
--------

The ADuCM3029_Blink example project uses the EVAL-ADICUP3029, an Ultra
Low Power, Arduino compatible ARM Cortex M3 based platform. The platform
supports various sensor shields and is created using the Analog Devices
Cross Core Embedded Studio.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/aducm_blinky_example/app_src>`.

Application Example
~~~~~~~~~~~~~~~~~~~

The ``ADuCM3029_Blink`` example uses the EVAL-ADICUP3029 board,
featuring an ARM Cortex-M3 microcontroller, to demonstrate LED control
using GPIO. The ``blinky_example.c`` file initializes the GPIO pins
corresponding to on-board LEDs DS3 (green) and DS4 (red). It enters a
loop that toggles these pins to achieve a binary count pattern,
demonstrating basic power management and GPIO control without an
operating system. Configuration files, such as ``builds.json`` and
``Makefile``, outline the build and platform specifics, while
``pinmux_config.c`` manages pin multiplexing. The project runs for a set
number of iterations and outputs “All Done” upon completion.

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Hardware Used
^^^^^^^^^^^^^

- EVAL-ADICUP3029

Software Used
^^^^^^^^^^^^^

- CrossCore Embedded Studio (2.6.0 or higher)
- ADuCM302x DFP (2.0.0 or higher)

Connection
^^^^^^^^^^

+-----------------------------------+-----------------------------------+
| Component                         | Description                       |
+-----------------------------------+-----------------------------------+
| Micro USB Cable                   | Connect the EVAL-ADICUP3029 board |
|                                   | to a PC for programming,          |
|                                   | debugging, and virtual serial     |
|                                   | terminal connections. Ensure the  |
|                                   | UART switch is in the USB         |
|                                   | position.                         |
+-----------------------------------+-----------------------------------+
| Power Options                     | The board can be powered via USB, |
|                                   | a DC wall power supply, or        |
|                                   | batteries. Each option offers     |
|                                   | different functionalities and     |
|                                   | peripherals.                      |
+-----------------------------------+-----------------------------------+
| UART Switch                       | Ensure the UART switch is set to  |
|                                   | USB for virtual serial terminal   |
|                                   | communication through the USB     |
|                                   | connector.                        |
+-----------------------------------+-----------------------------------+
|                                   |                                   |
+-----------------------------------+-----------------------------------+

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/aducm_blinky_example

   export PLATFORM=aducm3029 
   make
