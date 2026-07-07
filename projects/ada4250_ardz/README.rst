ADA4250-ARDZ no-OS Example Project
==================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADA4250-ARDZ <https://www.analog.com/ADA4250>`_

Overview
--------

The EVAL-ADA4250-ARDZ evaluates the ADA4250, a programmable gain
instrumentation amplifier with very low offset, drift, and bias current.
The device offers SPI-configurable gain settings (1, 2, 4, 8, 16, 32,
64, 128), a selectable reference buffer, and a bias current compensation
feature, making it well suited for high-precision signal-conditioning
applications. The board is an Arduino form-factor shield that connects to
a controller such as the EVAL-ADICUP3029 which runs the firmware and
exposes the device through an IIO server.

Applications
------------

* Medical instrumentation
* Precision data acquisition
* Sensor signal conditioning
* Industrial measurement and control

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADA4250-ARDZ is powered from the host controller board through
the Arduino header. When paired with the EVAL-ADICUP3029, the shield is
supplied from the carrier board, which is itself powered over the
micro-USB connector. The amplifier accepts a single supply of up to 5 V
(``avdd_v`` defaults to 5000 mV in the example configuration).

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
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ada4250_ardz/src/common>`__,
and the platform-specific macros in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/ada4250_ardz/src/platform>`__.

IIO Example
~~~~~~~~~~~

The IIO example (``iio`` variant, the default) initializes the
ADA4250 over SPI and launches an IIOD server over UART. Using IIO
Oscilloscope, or any IIO-compatible client, the user can read and adjust
the amplifier settings such as gain, reference buffer, and bias current
compensation.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a
look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Supported Platforms
-------------------------

ADuCM
~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADA4250-ARDZ <https://www.analog.com/ADA4250>`_
* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_

Connections
^^^^^^^^^^^

Mount the EVAL-ADA4250-ARDZ on the Arduino headers of the
EVAL-ADICUP3029 so the connectors align. Connect a micro-USB cable from
the P10 connector of the EVAL-ADICUP3029 to the host PC; this provides
power, programming, and serial communication. Ensure the UART switch
(S2) is set to the USB position for PC terminal access.

Build Command
^^^^^^^^^^^^^

Available variants: ``iio``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3

   cd no-OS

   # build the iio example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project ada4250_ardz --variant iio --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ada4250_ardz --variant iio --board eval-adicup3029 \
      --probe openocd --flash
