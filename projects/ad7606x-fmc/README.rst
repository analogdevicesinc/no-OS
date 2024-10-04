AD7606x no-OS Example Project
=============================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD7606B-FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606b-fmcz.html>`_
* `EVAL-AD7606CFMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606c-18.html>`_

Overview
--------

The EVAL-AD7606BFMCZ is a full featured evaluation board that allows users to
easily evaluate the features of the AD7606B analog-to-digital converter (ADC).

The EVAL-AD7606CFMCZ is a fully featured evaluation board that allows users to
evaluate the features of the AD7606C-18 and AD7606C-16 analog-to-digital converters (ADCs).

These board operate in conjunction with the Zedboard. The evaluation software
is provided to enable the user to perform detailed analysis of the AD7606B or
AD7606C-16/AD7606C-18's performance. The technical user guide includes the detailed
description of the operation and set up of the evaluation board and software when operated
with the System Demonstration Platform board.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples can be found in:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7606x-fmc/src/common_data.c/>`_

This contains data for both Serial or Parallel interfaces.

Depending on the how the `system_top.xsa` was built (with Serial or Parallel interface),
the driver will adapt to the correct interface. The driver will look for certain macro
definitions which are generated only for parallel or serial interfaces.

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the AD7606x, collects a numer of samples
and prints the result on the uart.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7606x-fmc/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD7606B-FMCZ/EVAL-AD7606CFMCZ evaluation boards.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can read samples from the device.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD7606 driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/blob/main/projects/ad7606x-fmc/src/iio_example.c>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad7606x-fmc/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx platorm
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-AD7606B-FMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606b-fmcz.html>`_
* `EVAL-AD7606CFMCZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7606c-18.html>`_
* `Zedboard <https://www.analog.com/en/resources/reference-designs/powering-zynq-evaluation-development-board-zedboard.html>`_


**Build Command**

.. code-block:: bash

        cp <SOME_PATH>/system_top.xsa .
        # to delete current build
        make reset
        # to build the project
        make
        # to flash the code
        make run
