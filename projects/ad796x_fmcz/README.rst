Evaluating the AD796x
======================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `AD796xDC1-EVALZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-AD796x.html#eb-documentation>`_

Overview
--------

The EVAL-AD7960FMCZ is a fully featured evaluation Kit designed to demonstrate
the performance of the low power 18-bit,5 MSPS PulSAR® Differential ADC AD7960.

This board operates in conjunction with the Zedboard. The evaluation software
is provided to enable the user to perform detailed analysis of the AD7960's
performance. The technical user guide includes the detailed description of
the operation and set up of the evaluation board and software when operated
with the System Demonstration Platform board.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the AD796x, collects a numer of samples
and prints the result on the uart.

In order to build the basic example make sure you have the following configuration in the Makefile
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/Makefile>`_

.. code-block:: bash

	# Select the example you want to enable by choosing y for enabling and n for disabling
	BASIC_EXAMPLE = y
	IIO_EXAMPLE = n

IIO example
^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD7960FMCZ evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the IMU and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD796x driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad796x_fmcz/Makefile>`_

.. code-block:: bash

        # Select the example you want to enable by choosing y for enabling and n for disabling
        BASIC_EXAMPLE = n
        IIO__EXAMPLE = y

No-OS Supported Platforms
-------------------------

Xilinx platorm
^^^^^^^^^^^^^^

**Used hardware**

* `eval-ad7960 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad7960.html>`_
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
