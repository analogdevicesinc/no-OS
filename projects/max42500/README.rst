MAX42500 no-OS Example Project
==============================

.. no-os-doxygen::

..contents::
        :depth: 3

Supported Evaluation Boards
---------------------------

* `MAX42500 <https://www.analog.com/en/products/max42500>`_

Overview
--------

The MAX42500 evaluation board is a fully assembled and tested application 
circuit for the MAX42500 seven-input industrial power-system monitor. The test 
point taps allow for routing to the other subsystems for monitoring. A dedicated 
I2C 4-pin header is included for easy interface to an MCU.

Applications
------------

* Industrial Process Control
* Robotics
* Remote Sensor Modules
* Power System Supervision
* MCU/SoC Monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The MAX42500 evaluation device must be supplied with 3.3V or 5V.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from **Project Common Data Path**.

The macros used in Common Data are defined in platform specific files found in **Project Platform Configuration Path**.

Basic example
^^^^^^^^^^^^^^^^^^

This is a simple example that initializes the MAX42500 device and configures the 
nominal voltage and OV/UV thresholds of the voltage monitor input 1. In the while 
loop, the example reads the status of the voltage monitor input and prints the 
results to the console. Optionally, varying input signals can be applied to the 
input voltage monitor pin to test and see the status of the pin.

To build the basic example, ensure you have the following configuration 
in the **Makefile**:

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX42500 <https://www.analog.com/en/products/max42500>`_
* `AD-APARD32690-SL <https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

+---------------------+---------------------+-----------------------------+
| Max42500 test point | Function            | AD-APARD32690-SL Pin Number |
+=====================+=====================+=============================+
| EN0_H               | Enable Input 0      | P2_26                       |
+---------------------+---------------------+-----------------------------+
| GND (J31)           | Ground              | GND                         |
+---------------------+---------------------+-----------------------------+
| EN1_H               | Enable Input 1      | P2_29                       |
+---------------------+---------------------+-----------------------------+
| VSUP (J31)          | Power Supply        | 3V3                         |
+---------------------+---------------------+-----------------------------+
| SDA                 | Serial Data Line    | P2_7                        |
+---------------------+---------------------+-----------------------------+
| SCL                 | Serial Clock Line   | P2_8                        |
+---------------------+---------------------+-----------------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the project
        make PLATFORM=maxim TARGET=max32690
        # to flash the code
        make run
