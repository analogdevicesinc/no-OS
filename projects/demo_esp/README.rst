Demo_ESP no-OS Example Project
==============================

.. contents::
    :depth: 3

Overview
--------

The ``demo_esp`` project provides a bare-metal software example for
Wi-Fi and MQTT communication on ADUCM3029 and Maxim hardware platforms.
It demonstrates basic connectivity functions and serves as a guide for
integrating Wi-Fi and MQTT functionalities in no-OS environments.

No-OS Build Setup
-----------------

Please see: `https://wiki.analog.com/resources/no-os/build`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from the
:git-no-OS:`Project Common Data Path </projects/demo_esp/src/common>`.

The macros used in Common Data are defined in platform specific files
found in the 
:git-no-OS:`Project Platform Configuration Path </projects/demo_esp/src/platform>`.

Basic example
~~~~~~~~~~~~~

The basic example code in the no-OS demo project for ADUCM3029 and Maxim
platforms serves as a demonstration of connecting to a Wi-Fi network and
an MQTT broker using a microcontroller. The file includes functions to
initialize and connect to a Wi-Fi network, establish a connection with
an MQTT broker, and send messages to the broker. The main function,
``basic_example_main``, orchestrates these operations by first connecting 
to Wi-Fi, then to the MQTT broker, and continuously sending data 
to the broker in a loop.

Key components of the code include the ``init_and_connect_wifi``
function, which handles the initialization and connection to a Wi-Fi
network, and the ``init_and_connect_to_mqtt_broker`` function, 
which sets up a TCP socket connection to an MQTT broker and
subscribes to a topic. The ``mqtt_message_handler`` function 
processes incoming MQTT messages, and the ``send``
function publishes messages to the broker. The code uses several
external libraries for handling operations such as GPIO, UART, and
network communication, indicated by the inclusion of headers like
``no_os_gpio.h``, ``mqtt_client.h``, and ``tcp_socket.h``. The file is
designed to be executed as part of a larger application, as indicated by
the presence of the ``basic_example_main`` function, which serves as 
the entry point for the example execution.

In order to build the basic example, make sure you have the following
configuration in the :git-no-OS:`Makefile <projects/demo_esp/Makefile>`:

.. code-block:: bash

   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

ADuCM Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- ADuCM3029 platform (EVAL-ADICUP3029) with on-board ESP8266 Wi-Fi
  Module

Connection
^^^^^^^^^^

- Connect the board to your PC using a standard micro-USB cable plugged
  into the USB connector labeled P10.

- Set the UART switch (S2) fully to the left (“USB” position). This
  routes the UART signals to the USB connector, enabling read/write data
  between the board and PC.

- The board can be powered directly via the USB connection. Alternative
  power sources include the DC barrel jack (+7V to +12V) or batteries
  attached to the BT1 connector.

- Push buttons: 3029_RESET offers hardware reset, 3029_BOOT with RESET
  enables UART download mode for firmware flashing.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/demo_esp

   export PLATFORM=aducm3029

   # to delete current build
   make reset
   # to build the project
   make
   # to flash the code
   make run


Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

- MAX32690-based platform (either MAX32690EVKIT or AD-APARD32690-SL)
- ESP Wifi Module (such as ESP8266)

Connection
^^^^^^^^^^

- Connect the MAX32690 UART TX/RX/GND pins to the corresponding ESP
  Wi-Fi module pins.

================ ====================
MAX32690 Pin     ESP Wi-Fi Module Pin
UART_TX          RX
UART_RX          TX
GND              GND
3.3V (if needed) VCC
================ ====================

- Use the on-board 3.3V supply to power the ESP module if your system
  supports the current requirement.

- After wiring up, configure and initialize the UART peripheral in your
  MAX32690 firmware, matching the ESP module’s default baud rate
  (typically 115200) and UART settings.

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   cd no-OS/projects/demo_esp

   # to delete current build
   make reset
   # to build the project
   make PLATFORM TARGET=max32690
   # to flash the code
   make run
