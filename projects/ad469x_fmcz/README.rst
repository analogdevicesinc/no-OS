Evaluating the AD469x
======================


Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4696 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4696.html#eb-overview>`_

Overview
--------

The EVAL-AD4696FMCZ is designed to demonstrate AD4696 performance and provide access to its many configuration options via an easy to use ACE plugin graphical interface. The AD4696 is a 16-channel, 16-bit, 1 MSPS, multiplexed successive approximation register (SAR) analog-to-digital converter (ADC) that enables high performance data acquisition of multiple signals in a small form factor. The AD4696 employs easy drive features and on-chip channel sequencing that simplify hardware and software designs and allow it to fit into a variety of space constrained precision multichannel applications.

The EVAL-AD4696FMCZ allows users to quickly evaluate the performance of the AD4696 with no or minimal hardware modifications. The hardware includes two externally driven analog input channels for evaluating ac performance and 14 channels with dc levels generated on board for evaluating dc and settling performance. The externally driven channels have configurable drive circuitry and interface with precision signal generators via SMA connectors.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build


Available make options
-----------------------

AD469X_DEV - select what device to use (ad4695, ad4696*, ad4697, ad4698)

AD469X_TEMP - Enable temperature channel (y, n*)

AD469X_SEQ - Select channel sequence method (standard*, advanced, single)


Example usage:

.. code-block:: bash

        make AD469X_DEV=ad4697 AD469X_TEMP=y AD469X_SEQ=advanced

Running "make" with no options is the  equivalent of:

.. code-block:: bash

        make AD469X_DEV=ad4696 AD469X_TEMP=n AD469X_SEQ=standard


No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ad469x_fmcz/src/common>`_

Basic example
^^^^^^^^^^^^^

This is a simple example that initializes the AD469x, collects a number of samples
and prints the result on the uart.

To build the basic example, use the following command:

.. code-block:: bash

        make IIO_EXAMPLE=n BASIC_EXAMPLE=y

Sequencer example
^^^^^^^^^^^^^^^^^^

This is a simple example that initializes the AD469x, collects a numer of samples
in either advanced or standard sequencer mode and prints the result on the uart.


Standard sequencer Example:

.. code-block:: bash

        make IIO_EXAMPLE=n BASIC_EXAMPLE=y AD469X_SEQ=standard

Advanced sequencer Example:

.. code-block:: bash

        make IIO_EXAMPLE=n BASIC_EXAMPLE=y AD469X_SEQ=advanced


IIO example (default)
^^^^^^^^^^^^^^^^^^^^^^

This project is actually a IIOD demo for EVAL-AD469x evaluation board.
The project launches a IIOD server on the board so that the user may connect
to it via an IIO client.
Using IIO-Oscilloscope, the user can configure the IMU and view the measured data on a plot.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

The No-OS IIO Application together with the No-OS IIO AD469x driver take care of
all the back-end logic needed to setup the IIO server.

This example initializes the IIO device and calls the IIO app as shown in:
`IIO Example <https://github.com/analogdevicesinc/no-OS/tree/main/projects/AD469x_fmcz/src/examples/iio_example>`_

In order to build the IIO project make sure you have the following configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/tree/main/projects/AD469x_fmcz/Makefile>`_

To build the IIO example:

.. code-block:: bash

        make IIO_EXAMPLE=y BASIC_EXAMPLE=n


No-OS Supported Platforms
-------------------------

Xilinx platform
^^^^^^^^^^^^^^^

**Used hardware**

* `eval-ad4696 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4696.html#eb-overview>`_
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
