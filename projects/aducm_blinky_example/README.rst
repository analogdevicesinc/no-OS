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

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. The
initialization data used in the examples is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/aducm_blinky_example/src/common>`__,
and the platform-specific macros in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/aducm_blinky_example/src/platform>`__.

Basic Example
~~~~~~~~~~~~~

This is the default example (``basic`` variant) which initializes
the GPIO pins for the on-board LEDs (DS3 green, DS4 blue) and toggles them
in a blink pattern.

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

Available variants: ``basic``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3

   cd no-OS

   # build the basic example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project aducm_blinky_example --variant basic --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project aducm_blinky_example --variant basic --board eval-adicup3029 \
      --probe openocd --flash
