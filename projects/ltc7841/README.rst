Evaluating the LTC7841
======================

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `DC2798A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/DC2798A.html>`_

Overview
--------

The evaluation board includes 2 LTC7841 to be powered up with writing to the MFR_VOUT_COMMAND a non-zero value.
To turn-on the evaluation board, write 0x80 to the OPERATION register and ensure that the MFR_VOUT_COMMAND is actually
non-zero value.

Full performance details are provided in the LTC7841 data sheet, which should
be consulted in conjunction with user guide.

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

For this specific project an external power supply with a variable output of 10-16 Volts
must be used to power up the demo board.

**Hardware Bringup**

For reference, follow the Quick Start Procedure section of the corresponding
demo board.
`user guide <https://www.analog.com/media/en/technical-documentation/user-guides/dc2798a.pdf>`_.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc7841/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc7841/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the LTC7841, and performs telemetry
readings of the voltage, current and temperature of each output channel. Status
bytes/words are also monitored in the example.

In order to build the basic example make sure you are using this command:

.. code-block:: bash

    make EXAMPLE=basic

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for DC2798-A evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO LTC2798 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ltc2798/src/examples/iio_example>`_

In order to build the IIO project make sure you you are using this command:

.. code-block:: bash

    make EXAMPLE=iio_example

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `DC2798A <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/DC2798A.html>`_
* `MAX3290EVKit <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32690evkit.html>`_

**Connections**:

+---------------------+---------------------+-----------------------------+
| LTC7841 test point  | Function            | MAX32690 EVKIT Pin Number   |
+=====================+=====================+=============================+
| GND (J31)           | Ground              | GND                         |
+---------------------+---------------------+-----------------------------+
| SDA                 | Serial Data Line    | P2_7                        |
+---------------------+---------------------+-----------------------------+
| SCL                 | Serial Clock Line   | P2_8                        |
+---------------------+---------------------+-----------------------------+


**Build Command**

.. code-block:: bash

	# to delete current build
	make reset
	# to build the project for running basic example
	make EXAMPLE=basic PLATFORM=maxim TARGET=max32690
	# to build the project for running iio example
	make EXAMPLE=iio_example PLATFORM=maxim TARGET=max32690
	# to flash the code
	make run
