ADuCM3029 Blinky no-OS Example Project
======================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Overview
--------

The ADuCM3029 Blinky Example is a simple demonstration project for the
EVAL-ADICUP3029 platform, which is based on the ultra low power ADuCM3029
ARM Cortex-M3 microcontroller. The project uses the no-OS GPIO driver to
toggle the on-board LEDs (DS3 green, DS4 blue) in a binary count pattern,
demonstrating basic GPIO control without an operating system.

The ADuCM3029 features single-supply operation from 1.74 V to 3.6 V, with
active mode dynamic current of 30 uA/MHz and hibernate mode current of
750 nA.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Source Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/aducm_blinky_example/app_src>`__.

Application example
~~~~~~~~~~~~~~~~~~~

This is the default example which initializes the GPIO pins for the
on-board LEDs and toggles them in a binary count pattern for a set
number of iterations.

No-OS Supported Platforms
-------------------------

ADuCM3029
~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Connections
^^^^^^^^^^^

Connect the EVAL-ADICUP3029 to a PC via micro USB cable. Ensure the UART
switch is set to the USB position for serial terminal communication.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=aducm3029
   # to flash the code
   make run
