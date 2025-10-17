Wethlink no-OS Example Project
==============================

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

- :adi:`ADMV96S-WGBE-EK1`

Overview
--------

The ADMV96S-WGBE-EK1 board facilitates evaluation and prototyping of 
60 GHz short data links, serving as a replacement for mechanical connectors
in moving joints. It incorporates a 1 Gbps Ethernet interface on each
side of the wireless link, supported by the ADMV9615 and ADMV9625
modules for signal processing. The board’s operation is managed by the
MAX32650 microcontroller, complemented by a power management system
delivering required voltage levels. Customization is enabled through
open-source firmware and a GUI configuration tool. The board is
applicable in slip rings, medical imaging, autonomous vehicles, and
industrial coupling systems.

Applications
------------

- Slip rings
- Magnetic resonance imaging systems
- Autonomous guided vehicles
- Truck and trailer coupling
- Railway coupling

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADMV96S-WGBE-EK1 board requires a 12V power supply for optimal
operation, though it can accommodate voltages from 4V to 16V. The board
integrates voltage regulation modules, such as the LTM4623, LTC3621, and
ADP170, to supply the necessary 5V, 3.3V, 1.8V, and 1.1V outputs for its
components. A power-on LED indicates the board’s operational status,
ensuring reliable performance across various applications.

Connectors and Buttons
~~~~~~~~~~~~~~~~~~~~~~

+-------------------+-------------------------------------------------------+
| Connector         | Description                                           |
+-------------------+-------------------------------------------------------+
| P1                | MAXDAP Adapter                                        |
+-------------------+-------------------------------------------------------+
| P2                | Connector for ADMV9615 or ADMV9625 module             |
+-------------------+-------------------------------------------------------+
| P14               | 12V DC Jack connector                                 |
+-------------------+-------------------------------------------------------+
| J1                | RJ45 Ethernet connector                               |
+-------------------+-------------------------------------------------------+
| S1                | Switch for ADMV9615/ADMV9625, must be positioned      |
|                   | correctly for transceiver module insertion            |
+-------------------+-------------------------------------------------------+
| S2                | Reset button for MAX32650 microcontroller             |
+-------------------+-------------------------------------------------------+
| S3                | Factory default reset button                          |
+-------------------+-------------------------------------------------------+


No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/wethlink/src>`__

Application example
~~~~~~~~~~~~~~~~~~~

The example project for the ADMV96S-WGBE-EK1 system involves
initializing and controlling hardware components such as LEDs and
network interfaces through GPIO. It configures source and header files
specific to LED control and network setup.

In order to build the basic example, make sure you have the following
configuration in the 
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/wethlink/Makefile>`__:

For Rev A:

.. code-block:: bash

   make TARGET=max32650 HW_VERSION=0

For Rev B:

.. code-block:: bash

   make TARGET=max32650 HW_VERSION=1


No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- ADMV96S-WGBE-EK1 Rev A or Rev B with on-board MAX32650 MCU.

Connections
^^^^^^^^^^^

1. **Switch Configuration:**

  - Ensure the S1 switch is set to match the module’s label on P2. 
    This is critical for establishing a reliable wireless link between 
    the ADMV9615 and ADMV9625 modules.

2. **Module Assembly:**

  - Mount the ADMV9615 and ADMV9625 modules securely on the rail system, 
    ensuring their patch antennas face each other for optimal signal transmission.

3. **Ethernet Connection:**

  - Use CAT5 RJ45 Ethernet cables to connect the boards to your network. 

4. **Power Supply:**

  - Provide a 12V power supply to each assembly. Ensure the power supply is stable 
    and within the specified voltage range.

5. **Optional Programmer Connection:**

  - If using the Wethlink GUI application, connect the MAXDAP programmers to your PC 
    via micro-USB. Ensure the programmers are properly recognized on your PC’s COM ports.

Build Command
^^^^^^^^^^^^^

**For Hardware Rev A:**

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make TARGET=max32650 HW_VERSION=0
   # to flash the code
   make run

**For Hardware Rev B:**

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make TARGET=max32650 HW_VERSION=1
   # to flash the code
   make run
