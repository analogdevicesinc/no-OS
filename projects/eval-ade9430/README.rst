EVAL-ADE9430 no-OS Example Project
==================================

.. no-os-doxygen::

Supported Evaluation Boards
---------------------------

* `EVAL-ADE9430 <https://www.analog.com/EVAL-ADE9430>`_

Overview
--------

The ADE9430 is a high performance, multiphase energy and power quality
monitoring device. This project runs on the MAX32650 and demonstrates both a
minimal SPI read example and a Sensor-to-Cloud IoT application. The
application is selected at build time through the ``--variant`` option, and
each variant maps to a ``.conf`` defconfig in the project directory.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ade9430/src/common>`_

The macros used in Common Data are defined in platform specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/eval-ade9430/src/platform>`_

Basic Example
^^^^^^^^^^^^^

Initializes the ADE9430 over SPI and, in a loop, reads and prints the Phase A
RMS voltage and RMS current together with the on-chip temperature. Two variants
are provided:

* ``basic_example`` - basic SPI read example for boards without an ADE9430
  reset line wired to the MCU (default variant).
* ``basic_example_hw_reset`` - same as above, plus the ADE9430 hardware reset
  sequence performed before SPI access. Use this on the EVAL-PQMON board, where
  the reset pin is wired to the MCU.

IoT Example
^^^^^^^^^^^

Sensor-to-Cloud application that publishes ADE9430 telemetry data. It supports
non-encrypted communication with an MQTT broker and encrypted communication
with Azure IoT Hub, optionally registering the device via the Azure Device
Provisioning Service (DPS) beforehand. The following variants are provided:

* ``iot_example`` - simple, non-encrypted connection to an MQTT broker (base
  IoT configuration).
* ``iot_example_rtc`` - non-encrypted MQTT connection, with the RTC default
  date and time set for relevant timestamp values.
* ``iot_example_azure_iot_hub`` - encrypted connection and telemetry publishing
  to the Azure IoT Hub.
* ``iot_example_azure_iot_hub_rtc`` - Azure IoT Hub connection, with the RTC
  default date and time set.
* ``iot_example_azure_iot_hub_dps`` - device registration through the Azure
  Device Provisioning Service, followed by encrypted Azure IoT Hub telemetry
  publishing.
* ``iot_example_azure_iot_hub_dps_rtc`` - Azure DPS registration and IoT Hub
  telemetry publishing, with the RTC default date and time set.

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used Hardware**

* `EVAL-ADE9430 <https://www.analog.com/EVAL-ADE9430>`_
* `MAX32650FTHR <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32650fthr.html>`_

**Build Command**

Available variants: ``basic_example``, ``basic_example_hw_reset``,
``iot_example``, ``iot_example_azure_iot_hub``,
``iot_example_azure_iot_hub_dps``, ``iot_example_azure_iot_hub_dps_rtc``,
``iot_example_azure_iot_hub_rtc``, ``iot_example_rtc``.
Available boards: ``max32650fthr``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the default variant (basic example on the max32650fthr board)
   python tools/scripts/no_os_build.py build \
      --project eval-ade9430 --variant basic_example --board max32650fthr

   # build the base IoT example
   python tools/scripts/no_os_build.py build \
      --project eval-ade9430 --variant iot_example --board max32650fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-ade9430 --variant iot_example --board max32650fthr \
      --probe openocd --flash
