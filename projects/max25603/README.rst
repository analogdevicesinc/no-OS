MAX25603 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `MAX25603EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max25603evkit.html>`_

Overview
--------

The MAX25603 is a high-voltage, high-brightness LED controller for
automotive lighting applications. It integrates a buck-boost converter that
operates over a 5V to 60V input range and can drive LED strings with
currents up to 1A. The device supports both analog and digital (PWM)
dimming and provides LED protection features including input current
limiting and overvoltage protection.

The MAX25603 is designed for combination automotive headlamp systems,
where multiple lighting functions such as low beam, high beam, daytime
running lights (DRL), and turn signals can be controlled within a single
LED string using shunting FETs for segment control.

Applications
------------

* Automotive headlamps
* LED lighting controllers

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max25603/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max25603/src/platform>`__.

Basic example
~~~~~~~~~~~~~

This example initializes the MAX25603 device, selects the COMP1
comparator, enables EN2 with PWM dimming, configures a low beam
output, waits 5 seconds, then disables the beam and enable outputs
before removing the device.

This is the only example in this project, so it builds by default.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX25603EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max25603evkit.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``ad-apard32690-sl``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project max25603 --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max25603 --variant basic --board ad-apard32690-sl \
      --probe openocd --flash
