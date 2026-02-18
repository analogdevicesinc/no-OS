ADGS6414D Family no-OS Example Project
======================================

.. no-os-doxygen::https://www.analog.com/ADGS6414D

.. contents::
    :depth: 3

Supported Evaluation Boards
---------------------------

* `ADGS6414D <https://www.analog.com/ADGS6414D>`_


Overview
--------

The ADGS6414D contains eight independent, low on-resistance, single-pole/single-throw
(SPST) switches in a 4mm x 5mm, 30-terminal LGA package.

The ADGS6414D enables higher channel density in systems where printed circuit board space
is constrained or existing system form factors restrict expansion.

When using SPI daisy-chain mode, the unique route through pins provide considerable space
savings when multiple ADGS6414D instances are combined to design very high channel count
systems, such as large switching matrices and fanout applications. The integrated supply
decoupling capacitors and SDO pullup resistor further increase the space savings and reduce
printed circuit board complexity.

Applications
------------

* Automatic test equipment
* Solid-state relay replacement
* Relay replacement
* Instrumentation
* Data acquisition
* Avionics
* Audio and video switching
* Communication systems

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The ADGS6414D eval devices have to be supplied with +5V voltage on VDD pin. Connect VSS to GND

**Pin Description**

        +------+----------+---------------------------------------+
        | Pin  | Name     | Description                           |
        +======+==========+=======================================+
        | 1    | D1       | Drain Terminal 1                      |
        +------+----------+---------------------------------------+
        | 2    | D2       | Drain Terminal 2                      |
        +------+----------+---------------------------------------+
        | 3    | S1       | Source Terminal 1                     |
        +------+----------+---------------------------------------+
        | 4    | S2       | Source Terminal 2                     |
        +------+----------+---------------------------------------+
        | 5    | VSS      | Most Negative Power Supply Potential  |
        +------+----------+---------------------------------------+
        | 6    | S3       | Source Terminal 3                     |
        +------+----------+---------------------------------------+
        | 7    | S4       | Source Terminal 4                     |
        +------+----------+---------------------------------------+
        | 8    | D3       | Drain Terminal 3                      |
        +------+----------+---------------------------------------+
        | 9    | D4       | Drain Terminal 4                      |
        +------+----------+---------------------------------------+
        | 10,30| VDD      | Most Positive Power Supply Potential  |
        +------+----------+---------------------------------------+
        | 11,29| GND      | Ground (0V) Reference                 |
        +------+----------+---------------------------------------+
        | 12,28| RESET/VL | RESET/Logic Power Supply Input (VL)   |
        +------+----------+---------------------------------------+
        | 13   | SDO      | Serial Data Output                    |
        +------+----------+---------------------------------------+
        | 14,26| SCLK     | Serial Clock Input                    |
        +------+----------+---------------------------------------+
        | 15,25| CS       | Active Low Control Input              |
        +------+----------+---------------------------------------+
        | 16   | D5       | Drain Terminal 5                      |
        +------+----------+---------------------------------------+
        | 17   | D6       | Drain Terminal 6                      |
        +------+----------+---------------------------------------+
        | 18   | S5       | Source Terminal 5                     |
        +------+----------+---------------------------------------+
        | 19   | S6       | Source Terminal 6                     |
        +------+----------+---------------------------------------+
        | 20   | NIC      | Not Internally Connected              |
        +------+----------+---------------------------------------+
        | 21   | S7       | Source Terminal 7                     |
        +------+----------+---------------------------------------+
        | 22   | S8       | Source Terminal 8                     |
        +------+----------+---------------------------------------+
        | 23   | D7       | Drain Terminal 7                      |
        +------+----------+---------------------------------------+
        | 24   | D8       | Drain Terminal 8                      |
        +------+----------+---------------------------------------+
        | 27   | SDI      | Serial Data Input                     |
        +------+----------+---------------------------------------+
        | EPAD | Exposed  | Exposed Pad                           |
        +------+----------+---------------------------------------+

**Cabling**

       JP1, JP2, JP5, JP6, JP7: position A
       JP3, JP4: position B

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adgs6414d/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-adgs6414d/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the adgs6414d selected device and
allows the user to set and get switches state using the serial interface.

**Menu Options:**

.. code-block:: text

        ADGS6414D Switch Control Menu
        ==============================
        1 - Set individual switch state
        2 - Get individual switch state
        3 - Set all switch states
        4 - Get all switch states
        5 - Software reset                                                                                      
        6 - Toggle CRC mode                                                                                     
        7 - Read error flags                                                                                    
        8 - Clear error flags                                                                                   
        h - Help                                                                                                
        q - Quit 

To build the basic example, add the following to the Makefile:

.. code-block:: bash

        EXAMPLE = basic

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `EVAL-ADGS-6414D <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adgs6414d.html>`_  and
* `MAX32655FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32655fthr.html>`_

**Connections**:

+--------------------+----------+-------------------------------------------------------+-------------------------+
| EVAL-ADGS6414D Port| Mnemonic | Function                                              | MAX32655FTHR Pin Number |
+====================+==========+=======================================================+=========================+
| P3                 | ~CS      | Chip Select (Serial Peripheral Interface), Active Low | P0_20                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | SCLK     | Serial Clock                                          | P0_23                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | SDI      | Data Input (Serial Peripheral Interface)              | P0_21                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | VL       | Voltage Level/~RESET,  Active Low                     | 3V3                     |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | GND      | Ground                                                | GND                     |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | VDD      | Power Supply, +5V                                     | VBUS                    |
+--------------------+----------+-------------------------------------------------------+-------------------------+
| P3                 | SDO      | Data Output (Serial Peripheral Interface)             | P0_22                   |
+--------------------+----------+-------------------------------------------------------+-------------------------+


**Build Command**

.. code-block:: bash

        # to delete current build
        make clean TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to build the project
        make TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to flash the code
        make run  TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
        # to debug the code
        make sdkopen TARGET=max32655 MAXIM_LIBRARIES=/home/$USER/MaximSDK/Libraries/
