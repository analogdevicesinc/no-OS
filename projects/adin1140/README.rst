ADIN1140 no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-ADIN1140 <https://www.analog.com/EVAL-ADIN1140>`_

Overview
--------

The ADIN1140 is a low-power, single-port 10BASE-T1S MAC/PHY designed for
multi-drop industrial Ethernet over a single twisted pair (SPE). It
complies with the IEEE 802.3cg-2019 standard and implements the Physical
Layer Collision Avoidance (PLCA) reconciliation sublayer, allowing
multiple nodes to share a mixing segment without classic CSMA/CD
overhead. The integrated MAC is accessed through a 4-wire SPI link using
the OPEN Alliance TC6 (OA-TC6) chunked protocol, which makes the
ADIN1140 a good fit for host MCUs that lack an on-chip Ethernet MAC.

This project provides bare-metal and FreeRTOS-based examples that
exercise the no-OS ADIN1140 driver in three modes:

* Raw Ethernet frame transmit/receive (driver-level smoke test)
* TCP/UDP throughput measurement with lwIP + lwiperf
* Networked sensor access via the IIO server over lwIP, using an
  ADT7422 temperature sensor as the IIO device

Applications
------------

* Building and factory automation
* HVAC and lighting control
* Field instruments and edge sensors
* Multi-drop sensor and actuator networks
* Industrial process monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-ADIN1140 evaluation board operates with a 3.3 V supply, sourced
from the host MCU board through the SPI/IO header. The board's VDDIO
domain is selected to match the host (typically 3.3 V); confirm jumper
selection on the eval board before powering up.

For the IIO over lwIP example, an ADT7422 temperature sensor (for
example on a `EVAL-ADT7422 <https://www.analog.com/EVAL-ADT7422>`_
breakout) is connected over I2C and shares the host's 3.3 V supply.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1140/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1140/src/platform>`__.

The project uses the CMake build system. The example to build is
selected by pointing ``PROJECT_DEFCONFIG`` at one of the project's
``.conf`` defconfig files:

* ``frame_rx_tx.conf``
* ``iperf_lwip_example.conf``
* ``iperf_lwip_freertos.conf``

The target board is selected with a CMake preset (``max32690fthr`` or
``max78000fthr``). See the `Build Command`_ section below for the full
invocation.

frame_rx_tx example
~~~~~~~~~~~~~~~~~~~

The ``frame_rx_tx`` example is a bare-metal driver-level smoke test. It
brings up the ADIN1140 over SPI/OA-TC6, enables promiscuous mode and the
broadcast filter, and then in a 200 ms loop:

* prints the link state and the on-chip TX/RX frame counters
  (``ADIN1140_TX_FRAME_CNT_REG`` / ``ADIN1140_RX_FRAME_CNT_REG``);
* transmits a broadcast Ethernet frame containing the ASCII payload
  ``"Hello ADIN1140!"`` via ``adin1140_write_fifo()``;
* drains any received frame with ``adin1140_read_fifo()`` and prints
  source/destination MAC addresses and the first bytes of the payload.

The test source MAC is ``CA:2F:B7:10:23:63``. The example does not use
lwIP and so does not require a host IP/network configuration; it only
needs another node on the same 10BASE-T1S mixing segment to observe or
respond to the broadcast frames.

In order to build the ``frame_rx_tx`` example, pass its defconfig to
CMake:

.. code-block:: bash

   -DPROJECT_DEFCONFIG=adin1140/frame_rx_tx.conf

iperf_lwip_example
~~~~~~~~~~~~~~~~~~

The ``iperf_lwip_example`` runs the lwIP stack on top of the ADIN1140
netif glue (``no_os_lwip_init`` + ``adin1140_lwip_ops``) and starts the
default lwiperf TCP server on the standard iperf port. It is the
canonical bare-metal Ethernet-over-SPE throughput test.

The example wires the ADIN1140 ``INT`` pin to a GPIO interrupt on the
host MCU. The ISR sets a flag in the ADIN1140 descriptor; the main loop
checks ``adin1140_irq_triggered()`` and calls ``no_os_lwip_step()`` only
when the MAC-PHY has signalled work, so SPI traffic is driven by the
hardware rather than by polling.

Default network parameters are baked into the project CFLAGS:

* IP address: ``192.168.97.40``
* Netmask:    ``255.255.0.0``
* Gateway:    ``192.168.97.1``

The host running iperf must be on the same subnet (for example a static
``192.168.97.x`` IP on the host's network interface). Once the board
prints
``iperf server started. Network is ready for iperf testing.``, run::

   iperf -c 192.168.97.40 -i 1

on the host. Throughput reports are printed both on the host (by iperf)
and on the board's UART through the ``lwiperf_report`` callback.

In order to build the ``iperf_lwip_example``, pass its defconfig to
CMake:

.. code-block:: bash

   -DPROJECT_DEFCONFIG=adin1140/iperf_lwip_example.conf

iperf_lwip_freertos example
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``iperf_lwip_freertos`` example is the FreeRTOS counterpart of
``iperf_lwip_example``. It creates a single ``net_task`` that owns
``no_os_lwip_init``, the GPIO-IRQ setup and the lwiperf server.

The ADIN1140 ``INT`` pin is configured for level-low triggering; the
ISR sets the descriptor's IRQ flag, masks the pin at the GPIO
controller and uses ``xTaskNotifyFromISR()`` to wake the network task.
The task blocks on ``ulTaskNotifyTake()``, calls
``no_os_lwip_step()``, then re-enables the GPIO IRQ. This is the
recommended pattern for combining the ADIN1140 with FreeRTOS-based
applications, since it avoids the busy-wait loop used by the
bare-metal version while still being driven entirely by the MAC-PHY's
``INT`` signal.

The example uses the same default IP / netmask / gateway as
``iperf_lwip_example``. It pulls the FreeRTOS configuration from
``projects/adin1140/src/FreeRTOSConfig.h``.

In order to build the ``iperf_lwip_freertos`` example, pass its
defconfig to CMake:

.. code-block:: bash

   -DPROJECT_DEFCONFIG=adin1140/iperf_lwip_freertos.conf

iio_lwip_example
~~~~~~~~~~~~~~~~

The ``iio_lwip_example`` brings up an IIO server on top of the ADIN1140
+ lwIP transport so that an IIO client can reach the device over
10BASE-T1S Ethernet. The exposed IIO device is an ADT7422 temperature
sensor connected to the host MCU over I2C; the ADIN1140 is used purely
as the network interface.

The IIO demo is a standard example, provided in most `no-OS
projects <https://github.com/analogdevicesinc/no-OS/tree/main/projects>`__,
that launches an IIOD server on the board so that the user may connect
to it via an IIO client.

If you are not familiar with ADI IIO Application, please take a look
at: `IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`__

If you are not familiar with ADI IIO Oscilloscope Client, please take
a look at: `IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`__

The board listens on the IIOD network port at the IP configured in the
project defconfig (default ``192.168.97.40``). Connect with, for
example::

   iio_info -u ip:192.168.97.40

.. note::

   The ``iio_lwip_example`` is not yet available in the CMake build.
   Only ``frame_rx_tx``, ``iperf_lwip_example`` and
   ``iperf_lwip_freertos`` are currently supported.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `EVAL-ADIN1140 <https://www.analog.com/EVAL-ADIN1140>`_
* One of the following Maxim host boards:

  * MAX32690FTHR (``TARGET=max32690``)
  * MAX78000FTHR (``TARGET=max78000``)

* For the ``iio_lwip_example`` only: an ADT7422 temperature sensor
  breakout (for example `EVAL-ADT7422 <https://www.analog.com/EVAL-ADT7422>`_)

Connections
^^^^^^^^^^^

Common control signals (UART log + ADIN1140 SPI + INT) are wired as
follows. SPI uses no-OS chip-select index 1, mode 0; SPI clock and INT
pin assignments differ per target.

**MAX32690FTHR (TARGET=max32690)**

SPI is on instance 0, clocked at 25 MHz; the ADIN1140 ``INT`` line is
routed to GPIO ``P0.8`` (``GPIO0_IRQn``).

============== ========================== =====================
Signal         MAX32690FTHR Pin           EVAL-ADIN1140 Pin
============== ========================== =====================
GND            GND                        GND
3V3            3V3                        VDDIO / 3V3
SPI SCLK       SPI0 SCK                   SCLK
SPI SS (CS)    SPI0 SS1                   /CS
SPI MOSI       SPI0 MOSI                  MOSI
SPI MISO       SPI0 MISO                  MISO
ADIN1140 INT   P0.8                       /INT
UART TX (log)  UART0 TX                   --
UART RX (log)  UART0 RX                   --
============== ========================== =====================

**MAX78000FTHR (TARGET=max78000)**

SPI is on instance 1, clocked at 30 MHz; the ADIN1140 ``INT`` line is
routed to GPIO ``P1.6`` (``GPIO1_IRQn``).

============== ========================== =====================
Signal         MAX78000FTHR Pin           EVAL-ADIN1140 Pin
============== ========================== =====================
GND            GND                        GND
3V3            3V3                        VDDIO / 3V3
SPI SCLK       SPI1 SCK                   SCLK
SPI SS (CS)    SPI1 SS1                   /CS
SPI MOSI       SPI1 MOSI                  MOSI
SPI MISO       SPI1 MISO                  MISO
ADIN1140 INT   P1.6                       /INT
UART TX (log)  UART0 TX                   --
UART RX (log)  UART0 RX                   --
============== ========================== =====================

The ``iio_lwip_example`` additionally requires the ADT7422 sensor to
be connected to the host's I2C bus (no-OS I2C device id 1, 100 kHz),
sharing the same 3.3 V/GND domain.

Build Command
^^^^^^^^^^^^^

The ``MAXIM_LIBRARIES`` environment variable must point at the MaximSDK
``Libraries`` directory (see the build prerequisites in the
`No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_).

The build is a two-step CMake flow: configure with a board preset and a
project defconfig, then build the ``adin1140`` target. Run the commands
from the no-OS repository root.

.. code-block:: bash

   # Configure for the MAX32690FTHR, iperf_lwip_example
   cmake --preset max32690fthr -B build \
         -DPROJECT_DEFCONFIG=adin1140/iperf_lwip_example.conf --fresh

   # or, for the MAX78000FTHR
   cmake --preset max78000fthr -B build \
         -DPROJECT_DEFCONFIG=adin1140/iperf_lwip_example.conf --fresh

   # Build the project
   cmake --build build --target adin1140

   # Flash the board
   cmake --build build --target flash

To build a different example, point ``PROJECT_DEFCONFIG`` at another
``.conf`` file (for example ``adin1140/frame_rx_tx.conf`` or
``adin1140/iperf_lwip_freertos.conf``). The resulting ``adin1140.elf``,
``adin1140.hex`` and ``adin1140.bin`` are placed in the ``build``
directory.
