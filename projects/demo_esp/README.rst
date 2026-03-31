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

In order to build the basic example, make sure you have the following
configuration in the
`Makefile <https://github.com/analogdevicesinc/no-OS/blob/main/projects/demo_esp/Makefile>`__:

.. code-block:: bash

   # Select the example you want to enable by choosing y for enabling and n for disabling
   BASIC_EXAMPLE = y

No-OS Supported Platforms
-------------------------

ADuCM3029
~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `EVAL-ADICUP3029 <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-adicup3029.html>`_

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=aducm3029
   # to flash the code
   make run

Maxim
~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX32690 platform (MAX32690EVKIT or AD-APARD32690-SL)
* ESP Wi-Fi module (e.g., ESP8266)

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

   # to delete current build
   make reset
   # to build the project
   make PLATFORM=maxim TARGET=max32690
   # to flash the code
   make run
