EVAL-CN0391-ARDZ no-OS Example Project
=======================================

.. no-os-doxygen::

.. contents::
    :depth: 3

Supported Devices
-----------------

* `AD7124-8 <https://www.analog.com/AD7124-8>`_

Supported Evaluation Boards
----------------------------

* `EVAL-CN0391-ARDZ <https://www.analog.com/cn0391>`_

Overview
--------

The EVAL-CN0391-ARDZ is a 4-channel Type-K thermocouple temperature measurement
board based on the AD7124-8, a low noise, low power, 24-bit sigma-delta ADC.
Each channel reads the thermocouple hot junction temperature using cold junction
compensation (CJC) provided by a PT1000 RTD driven by a 750 µA current source
(IOUT0). The board connects to the host microcontroller via SPI.

This no-OS project targets the EVAL-ADIN1110 board (STM32L4 microcontroller)
and exposes the 4 thermocouple channels over a 10BASE-T1L Ethernet link using
the ADIN1110 PHY and the lwIP network stack.

See complementary documentation at
https://www.analog.com/cn0391.

Hardware Specifications
------------------------

The EVAL-CN0391-ARDZ connects to the EVAL-ADIN1110 Arduino shield connector.
The board is powered through the USB port of the EVAL-ADIN1110.

**EVAL-ADIN1110 Connections**

+------------------+--------------------------------------+
| EVAL-CN0391-ARDZ | EVAL-ADIN1110                        |
+------------------+--------------------------------------+
| Arduino Shield   | Arduino Shield connector             |
+------------------+--------------------------------------+
| USB (power only) | USB Type-A port                      |
+------------------+--------------------------------------+

No-OS Build Setup
-----------------

Please see: https://analogdevicesinc.github.io/no-OS/build_guide.html

No-OS Supported Examples
-------------------------

The initialization data used in the examples is taken from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-cn0391-ardz/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-cn0391-ardz/src/platform>`_

Basic example
^^^^^^^^^^^^^

This is a simple example which initializes the CN0391 board and reads all
4 thermocouple channels in a continuous loop with a 2-second delay between
cycles. The hot junction temperature of each channel is printed over UART.

The channel index (CH0..CH3) corresponds directly to the ``CHx`` label
silkscreened on the EVAL-CN0391-ARDZ board.

In order to build the basic example, use the following command:

.. code-block:: bash

        make -j$(nproc) -C projects/eval-cn0391-ardz EXAMPLE=basic_example

IIO over lwIP example
^^^^^^^^^^^^^^^^^^^^^

This is the primary example for the EVAL-ADIN1110 platform. It launches an
IIO daemon (iiod) over a 10BASE-T1L Ethernet link using the ADIN1110 PHY and
the lwIP network stack. No serial output is produced by this example.

The IIO device exposed is an ``ad7124-8`` with 4 channels (``voltage0`` ..
``voltage3``), each providing:

* ``raw`` — hot junction temperature in °C (float string)
* ``scale`` — fixed value ``1000`` (Zephyr convention for demo compatibility)

The channel index corresponds directly to the ``CHx`` label silkscreened on
the EVAL-CN0391-ARDZ board.

An optional ``cn0391`` IIO device can be enabled with ``CN0391_IIO_SUPPORT=y``,
exposing 4 channels (``temp0`` .. ``temp3``) with per-channel attributes:
``hot_junction_temp``, ``cold_junction_temp``, ``tc_voltage``,
``rtd_resistance``.

In order to build the IIO over lwIP example, use the following command:

.. code-block:: bash

        make -j$(nproc) -C projects/eval-cn0391-ardz EXAMPLE=iio_lwip_example ADIN1110_STATIC_IP=y

To override the default static IP address (``192.168.90.60``):

.. code-block:: bash

        make -j$(nproc) -C projects/eval-cn0391-ardz EXAMPLE=iio_lwip_example ADIN1110_STATIC_IP=y \
            NO_OS_IP=192.168.1.100 NO_OS_NETMASK=255.255.255.0 NO_OS_GATEWAY=192.168.1.1

To enable the optional ``cn0391`` IIO device:

.. code-block:: bash

        make -j$(nproc) -C projects/eval-cn0391-ardz EXAMPLE=iio_lwip_example ADIN1110_STATIC_IP=y CN0391_IIO_SUPPORT=y

No-OS Supported Platforms
--------------------------

STM32 Platform
^^^^^^^^^^^^^^

**Used hardware**:

* `EVAL-CN0391-ARDZ <https://www.analog.com/cn0391>`_ with
* `EVAL-ADIN1110 <https://www.analog.com/eval-adin1110>`_

**Build Command**

.. code-block:: bash

        # to delete current build
        make reset
        # to build the IIO over lwIP example (default)
        make -j$(nproc) -C projects/eval-cn0391-ardz EXAMPLE=iio_lwip_example ADIN1110_STATIC_IP=y
        # to flash the code
        make -C projects/eval-cn0391-ardz run
