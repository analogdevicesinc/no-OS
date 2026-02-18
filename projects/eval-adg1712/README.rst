ADG1712 Family no-OS Example Project
======================================

.. no-os-doxygen::

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

* `ADG1712 <https://www.analog.com/ADG1712>`_


Overview
--------

The ADG1712 contains four independent single-pole/single-throw (SPST) switches and
operates with a low-voltage single supply range from +1.08V to +5.5V or a low-voltage dual
supply range from ±1.08V to ±2.75V.

The ADG1712 is designed for small size without compromising on performance. The 2mm x
2mm land grid array (LGA) package is ideal for a broad range of applications where area is a
concern.

Applications
------------

* Automated test equipment
* Data acquisition systems
* Medical equipment
* FPGA and microcontroller systems
* Audio and video signal routing
* Communications systems
* Relay replacement

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADG1712 eval devices have to be supplied with +5V voltage on VDD pin. Connect VSS to GND

**Pin Description**

        +------+------+---------------------------------------+
        | Pin  | Name | Description                           |
        +======+======+=======================================+
        | 1    | S1   | Source Terminal 1                     |
        +------+------+---------------------------------------+
        | 2    | IN1  | Digital Control Input                 |
        +------+------+---------------------------------------+
        | 3    | AGND | Ground (0V) Reference                 |
        +------+------+---------------------------------------+
        | 4    | IN4  | Digital Control Input                 |
        +------+------+---------------------------------------+
        | 5    | S4   | Source Terminal 4                     |
        +------+------+---------------------------------------+
        | 6    | D4   | Drain Terminal 4                      |
        +------+------+---------------------------------------+
        | 7    | VSS  | Most Negative Power-Supply Potential  |
        +------+------+---------------------------------------+
        | 8    | D3   | Drain Terminal 3                      |
        +------+------+---------------------------------------+
        | 9    | S3   | Source Terminal 3                     |
        +------+------+---------------------------------------+
        | 10   | IN3  | Digital Control Input                 |
        +------+------+---------------------------------------+
        | 11   | VL   | Digital Logic Power Supply            |
        +------+------+---------------------------------------+
        | 12   | IN2  | Digital Control Input                 |
        +------+------+---------------------------------------+
        | 13   | S2   | Source Terminal 2                     |
        +------+------+---------------------------------------+
        | 14   | D2   | Drain Terminal 2                      |
        +------+------+---------------------------------------+
        | 15   | VDD  | Most Positive Power-Supply Potential  |
        +------+------+---------------------------------------+
        | 16   | D1   | Drain Terminal 1                      |
        +------+------+---------------------------------------+
        | EP   | EPAD | Exposed Pad                           |
        +------+------+---------------------------------------+

**Cabling**

       NEG_SUPPLY GND
       POS_SUPPLY 3V3
       VL 3V3
       JP2, JP3, JP4, JP5 removed
       P11, P12, P13, P14 inserted


No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg1712/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adg1712/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the ADG1712 and sets switch 1 on and off in a while loop
with a period of 2s. The data is printed on the serial interface.

To build the basic example, add the following to the Makefile:

.. code-block:: bash

        EXAMPLE = basic


No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `EVAL-ADG1712 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adg1712.html>`_  and
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

The labels on the eval board are wrong. DIGIO2 (IN2) controls SW4 (S4, D4), and DIGI04 (IN4) controls SW2 (S2, D2)
+--------------------+----------+-------------------------------------------------------+-------------------------+
| EVAL-ADG1712 Port  | Mnemonic | Function                                              | MAX32655FTHR Pin Number |
+====================+==========+=======================================================+=========================+
| P5                 | DIGI01   | Configurable Digital Input and Output 1               | P0_31                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P5                 | DIGI02   | Configurable Digital Input and Output 2               | P0_30                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P5                 | DIGI03   | Configurable Digital Input and Output 3               | P0_24                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P5                 | DIGI04   | Configurable Digital Input and Output 4               | P0_25                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P6                 | GND      | Ground                                                | GND                     |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P6                 | VDD      | Power Supply, +5V                                     | VBUS                    |
+--------------------+----------+-------------------------------------------------------+-------------------------+

**Build Command**

.. code-block:: bash

        # to delete current build
        make clean TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to build the project
        make  TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to flash the code
        make run TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to debug the code
        make sdkopen TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
