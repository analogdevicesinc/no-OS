MAX22200 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
----------------------------

* `MAX22200EVKIT <https://www.analog.com/MAX22200EVKIT>`_

Overview
--------

The `MAX22200 <https://www.analog.com/MAX22200>`_ is an octal 36 V
serial-controlled solenoid and motor driver. Each channel features a low
impedance (200 mOhm typ) push-pull output stage with sink-and-source driving
capability and up to 1 A RMS driving current. The device is controlled via a
high-speed SPI interface that also supports daisy-chain configurations.

The half-bridges can be configured as low-side or high-side drivers. Pairs of
half-bridges can be paralleled to double the driving current or configured as
full-bridges to drive up to four latched (bi-stable) valves or four brushed
DC motors. Two control methods are supported: voltage drive regulation (VDR)
and current drive regulation (CDR) with integrated lossless current sensing
(ICS).

For optimal power management in solenoid drive applications, the excitation
drive level (IHIT), the hold drive level (IHOLD), and the excitation drive
time (tHIT) can be individually configured for each channel.

The MAX22200 features overcurrent protection (OCP), thermal shutdown (TSD),
undervoltage lockout (UVLO), open-load detection (OL), and detection of
plunger movement (DPM). It is available in a 5 mm x 5 mm, 32-pin TQFN package
and operates over the -40 C to +85 C temperature range.

Applications
------------

* Relays Driver
* Solenoid, Valves, Electromagnetic Drivers
* Generic Low-Side and High-Side Switch Applications
* Latched (Bi-Stable) Solenoid Valve Drivers
* Brushed DC Motor Driver

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

* Supply voltage (VM): +4 V to +36 V
* On-resistance: 200 mOhm (typ) at +25 C
* Continuous output current: up to 1 A RMS per half-bridge

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22200/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max22200/src/platform>`__.

Basic example
~~~~~~~~~~~~~

The basic example initializes the MAX22200 device via SPI and configures
two channels in half-bridge mode with current drive regulation at 80 kHz
chopping frequency. It demonstrates setting channel frequency division,
enabling channels with DPM (detection of plunger movement), and configuring
fault masking for undervoltage monitoring.

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max22200/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y
   IIO_EXAMPLE = n

IIO example
~~~~~~~~~~~

The IIO example initializes the MAX22200 IIO device descriptor and launches
an IIO application server, allowing real-time interaction with the device
channels through IIO clients such as the IIO Oscilloscope.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

In order to build the IIO example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/max22200/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = n
   IIO_EXAMPLE = y

No-OS Supported Platforms
-------------------------

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX22200EVKIT
* MAX32665

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32665
   # to flash the code
   make run
