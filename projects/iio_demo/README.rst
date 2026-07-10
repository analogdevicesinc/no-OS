IIO Demo no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Overview
--------

The IIO Demo project runs an IIO (Industrial I/O) server on the target board,
exposing demo ADC and DAC devices to any IIO client. The demo devices use
loopback buffers, so data written to the DAC can be read back from the ADC,
making this project a self-contained reference for the no-OS IIO framework that
requires no external hardware.

Because the demo devices are entirely software based, this project is also a
convenient way to bring up and validate a new board port: it exercises the
UART/USB/Wi-Fi transport, the IRQ and timer infrastructure, and the IIO stack
without depending on any physical sensor being present.

If you are not familiar with the ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with the ADI IIO-Oscilloscope Client, please take a look
at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_demo/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/iio_demo/src/platform>`__.

Each example is selected with its own ``--variant`` (the ``.conf`` files in the
project directory). Not every variant is available on every board; see the
combination list with
``python tools/scripts/no_os_build.py list --project iio_demo``.

IIO example
~~~~~~~~~~~

The ``iio`` example instantiates the demo ADC and DAC devices and launches an
IIOD server over the board's UART. Data written to the DAC loopback buffer can
be read back from the ADC, so no external hardware is required. Using the IIO
Oscilloscope application (or the ``iio_readdev`` / ``iio_writedev`` command line
tools), users can access device attributes such as sampling frequency, scale,
and offset and stream buffered data.

This example is built by selecting the ``iio`` variant (see the Build Command
sections below).

IIO Timer Trigger example
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``iio_timer_trigger`` example demonstrates buffered data acquisition driven
by a hardware timer trigger. A periodic timer interrupt paces the sampling of
the demo ADC, feeding the IIO buffer at a deterministic rate. This showcases the
no-OS IIO trigger infrastructure without needing an external trigger source.

This example is built by selecting the ``iio_timer_trigger`` variant (see the
Build Command sections below).

IIO USB UART example
~~~~~~~~~~~~~~~~~~~~~

The ``iio_usb_uart`` example is functionally identical to the basic ``iio``
example, but exposes the IIOD server over a USB CDC-ACM (virtual serial)
interface instead of a physical UART. The board enumerates as a serial device on
the host, and the IIO client connects to that virtual COM port.

This example is built by selecting the ``iio_usb_uart`` variant (see the Build
Command sections below).

IIO Wi-Fi example
~~~~~~~~~~~~~~~~~~

The ``iio_wifi`` example runs the IIOD transport over an ESP8266 Wi-Fi module
(driven via UART AT commands) instead of the serial backend. The board connects
to the configured access point and the IIO client connects to the reported IP
address, allowing remote access to the demo devices over the network.

Set the network credentials in the ``iio_wifi`` defconfig via
``CONFIG_WIFI_SSID`` and ``CONFIG_WIFI_PWD`` before building.

This example is built by selecting the ``iio_wifi`` variant (see the Build
Command sections below).

IIO Wi-Fi with Software Trigger example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``iio_wifi_with_sw_trig`` example combines the Wi-Fi transport with a
software trigger for buffered data acquisition. As with ``iio_wifi``, the IIOD
server is reachable over the network. The software trigger requires networking
and is therefore not supported over a plain UART backend.

Set the network credentials in the ``iio_wifi_with_sw_trig`` defconfig via
``CONFIG_WIFI_SSID`` and ``CONFIG_WIFI_PWD`` before building.

This example is built by selecting the ``iio_wifi_with_sw_trig`` variant (see the
Build Command sections below).

Interacting with the device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Once the board is running, an IIO client such as ``iio_writedev`` / ``iio_readdev``
can be used to write to and read from the demo devices.

Write to device:

.. code-block:: bash

   cat sample.dat | iio_writedev -u serial:/dev/ttyUSB0,921600 -b 400 demo_device

Read from device:

.. code-block:: bash

   iio_readdev -u serial:/dev/ttyUSB0,921600 -b 400 -s 6400 demo_device > sample.dat

On Windows, use the ``COM`` port name for the serial URI:

.. code-block:: bash

   Get-Content ascii.dat | iio_writedev -u serial:COM9,921600 -b 100 -s 100 demo_device_output
   iio_readdev -u serial:COM9,921600 -b 100 -s 100 demo_device_input voltage0 voltage1

No-OS Supported Platforms
-------------------------

All platforms below are built through the unified no-OS CMake build flow using
``tools/scripts/no_os_build.py``. Because the demo devices are software
loopbacks, no external sensor wiring is required for any platform; simply connect
the board to the host (typically over USB).

ADuCM Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/EVAL-ADICUP3029>`_
* An ESP8266 Wi-Fi module (only for the ``iio_wifi`` / ``iio_wifi_with_sw_trig``
  variants)

Connections
^^^^^^^^^^^

No external sensor connections are required. Connect the EVAL-ADICUP3029 to the
host via its USB port for both power and the serial console. For the Wi-Fi
variants, attach the ESP8266 module to the UART pins used by the platform
configuration.

Build Command
^^^^^^^^^^^^^

Available variants: ``iio``, ``iio_wifi``, ``iio_wifi_with_sw_trig``.
Available boards: ``eval-adicup3029``.
Replace ``--variant`` accordingly.

.. code-block:: bash

   # point at the CrossCore Embedded Studio install (only if not auto-detected)
   export CCES_HOME=/opt/analog/cces/3.0.3

   cd no-OS

   # build the iio example on the EVAL-ADICUP3029
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board eval-adicup3029

   # build the iio example over Wi-Fi (ESP8266)
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio_wifi --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board eval-adicup3029 \
      --probe openocd --flash

The ``iio_wifi`` variants run the IIOD transport over an ESP8266 Wi-Fi module
(UART AT commands) instead of the serial backend. Set the network credentials in
the ``iio_wifi`` (or ``iio_wifi_with_sw_trig``) defconfig via
``CONFIG_WIFI_SSID`` and ``CONFIG_WIFI_PWD`` before building, for example:

.. code-block:: bash

   CONFIG_WIFI_SSID="MyNetwork"
   CONFIG_WIFI_PWD="MyPassword"

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `MAX32650FTHR <https://www.analog.com/MAX32650FTHR>`_
* `MAX32655FTHR <https://www.analog.com/MAX32655FTHR>`_
* `MAX32666FTHR <https://www.analog.com/MAX32666FTHR>`_
* `MAX78000FTHR <https://www.analog.com/MAX78000FTHR>`_
* `AD-APARD32690-SL <https://www.analog.com/AD-APARD32690-SL>`_

Connections
^^^^^^^^^^^

No external sensor connections are required. Connect the chosen Maxim board to
the host via its USB port for power and the serial console.

Build Command
^^^^^^^^^^^^^

Available variants: ``iio``, ``iio_usb_uart``.
Available boards: ``max32650fthr``, ``max32655fthr``, ``max32666fthr``,
``max78000fthr`` (``iio``); ``ad-apard32690-sl``, ``max32650fthr``,
``max32666fthr`` (``iio_usb_uart``).
Replace ``--variant`` / ``--board`` accordingly. Not every variant is available
on every board; see the combination list with
``python tools/scripts/no_os_build.py list --project iio_demo``.

.. code-block:: bash

   # point at the Maxim SDK libraries (only if not auto-detected)
   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the iio example on the max32650fthr board
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr \
      --probe openocd --flash

STM32 Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `SDP-K1 <https://www.analog.com/SDP-K1>`_
* `NUCLEO-F413ZH <https://www.st.com/en/evaluation-tools/nucleo-f413zh.html>`_
* ST debugger

Connections
^^^^^^^^^^^

No external sensor connections are required. Connect the chosen STM32 board to
the host via its USB port for power and the serial console.

Build Command
^^^^^^^^^^^^^

Available variants: ``iio``, ``iio_timer_trigger``, ``iio_usb_uart``.
Available boards: ``sdp-ck1z`` (all three variants); ``nucleo-f413zh``
(``iio_usb_uart``).
Replace ``--variant`` / ``--board`` accordingly. Not every variant is available
on every board; see the combination list with
``python tools/scripts/no_os_build.py list --project iio_demo``.

.. code-block:: bash

   # set the path to STM32CubeMX and STM32CubeIDE (only if they are not
   # in a default install location)
   export STM32CUBEMX=</path/to/stm32cubemx>
   export STM32CUBEIDE=</path/to/stm32cubeide>

   cd no-OS

   # build the iio example on the sdp-ck1z board
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board sdp-ck1z

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board sdp-ck1z \
      --probe openocd --flash

Pico Platform
~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* Raspberry Pi Pico

Connections
^^^^^^^^^^^

No external sensor connections are required. Connect the Raspberry Pi Pico to
the host via its USB port for power and the serial console.

Build Command
^^^^^^^^^^^^^

Available variants: ``iio``.
Available boards: ``rpi-pico``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   cd no-OS

   # build the iio example on the Raspberry Pi Pico
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board rpi-pico

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board rpi-pico \
      --probe openocd --flash
