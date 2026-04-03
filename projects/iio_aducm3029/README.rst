IIO_ADUCM3029 no-OS Example Project
====================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Overview
--------

The IIO_ADUCM3029 no-OS project demonstrates data acquisition and control
on the EVAL-ADICUP3029 evaluation board, an Arduino Uno form factor
compatible platform based on the ADuCM3029 ultralow power ARM Cortex-M3
microcontroller. The project initializes the on-chip Analog-to-Digital
Converter (ADC), Pulse Width Modulation (PWM) modules, and UART
communication. It configures multiple ADC input channels, reads a
specified number of samples, and transmits the formatted data over UART.

When IIO support is enabled, the project launches an IIOD server that
allows remote interaction through IIO client applications such as IIO
Oscilloscope. The IIO interface provides access to ADC channel data and
PWM control parameters, enabling real-time data visualization and device
configuration from a host computer.

Applications
------------

* Internet of Things (IoT)
* Electronic shelf label (ESL) and signage
* Smart infrastructure
* Smart lock
* Asset tracking
* Wearables
* Fitness and clinical

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADICUP3029 board can be powered through the micro-USB connector
(P10), the DC barrel jack (P2, +7 V to +12 V DC input), or a battery
header (BT1). The USB connection provides power and also serves as the
programming and debug interface.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_aducm3029/src>`__.

Serial Print Example
~~~~~~~~~~~~~~~~~~~~

The default example initializes the ADC, three PWM channels, and UART.
It continuously reads samples from the enabled ADC channels and transmits
the formatted data over UART at 115200 baud.

This is the default build configuration. No additional flags are
required.

IIO Example
~~~~~~~~~~~

This project launches an IIOD server on the board so that the user may
connect to it via an IIO client. Using IIO Oscilloscope, the user can
access device settings such as the ADC and PWM functionalities.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

In order to build the IIO project make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/iio_aducm3029/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   IIOD = y

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADICUP3029 to a PC via the micro-USB connector (P10).
This provides power, programming, and serial communication. Ensure the
UART switch (S2) is set to the USB position for PC terminal access.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # remove sp characters from the build directory
   make reset
   # build the project
   make PLATFORM=aducm3029
   # flash the code
   make run
