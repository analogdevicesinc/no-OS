EVAL-ADE9430 no-OS Example Project
==================================

.. no-os-doxygen::

Overview
--------

The project was developed on max32650 and is organized around the no-OS
``EXAMPLE`` based build flow. Selecting an example at build time chooses which
application is compiled:

- ``basic_example`` - minimal SPI example that reads and prints the RMS voltage
  and current for Phase A together with the on-chip temperature.
- ``iot_example`` - Sensor-to-Cloud application, supporting:

  - non-encrypted communication with a MQTT Broker.
  - encrypted communication with Azure IoT Hub (with the option of configuring
    and registering the device in the cloud via the Azure Device Provisioning
    Service prior to the IoT Hub connection and telemetry data publishing).

The default example is ``iot_example`` and its default configuration is a
simple, non-encrypted connection.

Project Structure
-----------------

::

    eval-ade9430/
    ├── Makefile
    ├── src.mk               - shared no-OS core and ADE9430 driver sources
    ├── builds.json          - CI build matrix (one entry per configuration)
    └── src/
        ├── common/          - init parameters shared across examples
        │   └── common_data.{c,h}
        ├── examples/
        │   ├── basic_example/   - basic SPI read example
        │   └── iot_example/     - MQTT / Azure IoT Hub application
        └── platform/
            └── maxim/           - platform main(), parameters.h, platform_src.mk

The platform ``main()`` is a thin dispatcher that calls ``example_main()``,
which is provided by the selected example.

Building project
----------------

Select the example with the ``EXAMPLE`` variable (defaults to ``iot_example``):

.. code-block:: bash

    make EXAMPLE=basic_example PLATFORM=maxim TARGET=max32650
    make EXAMPLE=iot_example   PLATFORM=maxim TARGET=max32650

Building attributes
-------------------

+-----------------+-----------------------------------------------------------+
|  Variable       |                Description                                |
+-----------------+-----------------------------------------------------------+
|  EXAMPLE        |  Select the example: ``basic_example`` or                 |
|                 |  ``iot_example`` (default: ``iot_example``).              |
+-----------------+-----------------------------------------------------------+
|  HW_RESET       |  ``y`` to drive the ADE9430 hardware reset pin before     |
|                 |  SPI access. Required on the EVAL-PQMON board, which      |
|                 |  wires the reset line to the MCU (P1.27). Default: ``n``. |
|                 |  Only relevant for ``basic_example``.                     |
+-----------------+-----------------------------------------------------------+
|  AZURE_IOT_HUB  |  Enable the Azure IoT encrypted connection                |
|                 |  (``iot_example`` only).                                  |
+-----------------+-----------------------------------------------------------+

+-----------------+---------------------------------------------------+
|  C Flags        |                Description                        |
+-----------------+---------------------------------------------------+
|  CONFIG_DPS     |  Enable the device registering in the cloud.      |
+-----------------+---------------------------------------------------+
|  RTC_SET_DEFAULT|  Set the default date and time for the RTC.       |
+-----------------+---------------------------------------------------+

basic_example
-------------

Initializes the ADE9430 over SPI and, in a loop, reads and prints:

- Phase A RMS voltage (``Vrms``) and RMS current (``Irms``),
- the on-chip temperature, obtained by starting a conversion and polling the
  ``TEMP_RDY`` bit in ``STATUS0`` until the result is ready (with a timeout).
  This is implemented locally as ``basic_read_temp()``. If preferred, it can be
  replaced by the driver's ``ade9430_read_temp()``, which uses a fixed delay
  instead of polling the conversion-ready status.

On the EVAL-PQMON board the ADE9430 reset pin is wired to the MCU and must be
driven high before communication. Build with ``HW_RESET=y`` to include the
reset sequence; on boards without this connection leave it disabled (default).

Build Configuration Examples
----------------------------

1.
  .. code-block:: bash

    make EXAMPLE=basic_example PLATFORM=maxim TARGET=max32650

  - Basic SPI read example (RMS voltage/current and temperature) on a board
    without a reset line wired to the MCU.

2.
  .. code-block:: bash

    make EXAMPLE=basic_example PLATFORM=maxim TARGET=max32650 HW_RESET=y

  - Basic SPI read example on the EVAL-PQMON board, including the ADE9430
    hardware reset sequence before SPI access.

3.
  .. code-block:: bash

    make EXAMPLE=iot_example PLATFORM=maxim TARGET=max32650

  - Default usage of the IoT example consisting of simple, non-encrypted
    connection to a MQTT broker.

4.
  .. code-block:: bash

    make EXAMPLE=iot_example PLATFORM=maxim TARGET=max32650 NEW_CFLAGS=-DRTC_SET_DEFAULT

  - Simple, non-encrypted connection to a MQTT broker, including the RTC default
    date and time set required for obtaining relevant timestamp values.

5.
  .. code-block:: bash

    make EXAMPLE=iot_example PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y

  - Encrypted connection and telemetry data publishing into the Azure IoT Hub.

6.
  .. code-block:: bash

    make EXAMPLE=iot_example PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y NEW_CFLAGS=-DCONFIG_DPS

  - Encrypted connection to the Azure Device Provisioning Service for the device
    registering in the cloud, followed by encrypted connection and telemetry
    data publishing into the Azure IoT Hub.

7.
  .. code-block:: bash

    make EXAMPLE=iot_example PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y NEW_CFLAGS=-DCONFIG_DPS NEW_CFLAGS=-DRTC_SET_DEFAULT

  - Encrypted connection to the Azure Device Provisioning Service for the device
    registering in the cloud, followed by encrypted connection and telemetry
    data publishing into the Azure IoT Hub, including the RTC default date and
    time set required for obtaining relevant timestamp values.
