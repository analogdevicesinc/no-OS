Demo ESP no-OS Example Project
==============================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Overview
--------

The Demo ESP project demonstrates Wi-Fi connectivity and MQTT
communication using an ESP Wi-Fi module with no-OS. The application
connects to a Wi-Fi network, establishes a connection with an MQTT
broker, and continuously publishes messages to a configured topic.

The initialization data used in the examples is taken out from the
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/demo_esp/src/common>`__.

The macros used in Common Data are defined in platform specific files
found in the
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/demo_esp/src/platform>`__.

No-OS Build Setup
-----------------

Please see: `No-OS Build Guide <https://wiki.analog.com/resources/no-os/build>`_

No-OS Supported Examples
------------------------

Basic example
~~~~~~~~~~~~~

The basic example initializes the ESP Wi-Fi module, connects to a
configured Wi-Fi network and MQTT broker, then continuously publishes
messages to a specified MQTT topic.

This is the only example in this project, so it is selected with the
``basic`` variant (see the Build Command sections below).

No-OS Supported Platforms
-------------------------

ADuCM3029
~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adicup3029.html>`_ (on-board ESP8266 Wi-Fi module)

Connections
^^^^^^^^^^^

The EVAL-ADICUP3029 carries an on-board ESP8266 Wi-Fi module wired to
connector P1, so no external module has to be added. Set the UART switch
**S2** to the "WiFi Module" (right) position to route UART0 to the P1
Wi-Fi connector; the module is driven over UART0 at 115200 baud and its
reset line is not connected (the module is reset in software).

=================== =========================== ==============================================
P1 Wi-Fi Net        ADuCM3029 Pin               Function
=================== =========================== ==============================================
UART0_TX            P0.10 (UART0 TX)            Data from the ADuCM3029 to the module
UART0_RX            P0.11 (UART0 RX)            Data from the module to the ADuCM3029
Reset               Not connected               Module is reset in software
Power (pin 8)       +3.3V                       Module power supply
Ground (pin 1)      DGND                        Common ground
=================== =========================== ==============================================

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
      --project demo_esp --variant basic --board eval-adicup3029

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project demo_esp --variant basic --board eval-adicup3029 \
      --probe openocd --flash

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX32690 platform (MAX32690EVKIT or AD-APARD32690-SL)
* ESP Wi-Fi module (e.g., ESP8266); the AD-APARD32690-SL integrates an
  on-board Wi-Fi module

Connections
^^^^^^^^^^^

The Wi-Fi module is driven over UART2 at 115200 baud and, unlike the
ADuCM3029, is reset through a hardware line wired to GPIO P3.9. The
MAX32690 signals used by the firmware are listed below; connect them to
the module UART, reset and power pins (on the AD-APARD32690-SL these are
already routed on-board).

=================== =========================== ==============================================
MAX32690 Signal     Pin                         Function
=================== =========================== ==============================================
UART2 TX            UART2 TX                    Data from the MAX32690 to the module
UART2 RX            UART2 RX                    Data from the module to the MAX32690
Reset               P3.9 (GPIO)                 Hardware reset driven by the MAX32690
Power               3.3V                        Module power supply
Ground              GND                         Common ground
=================== =========================== ==============================================

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``ad-apard32690-sl``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project demo_esp --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project demo_esp --variant basic --board ad-apard32690-sl \
      --probe openocd --flash
