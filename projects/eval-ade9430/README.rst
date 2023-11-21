EVAL-ADE9430 Sensor-to-Cloud
============================

Building project
----------------

The project was developed on max32650 and supports multiple configurations:
 - non-encrypted communication with a MQTT Broker.
 - encrypted communication with Azure IoT Hub (with the option of configurating
   and registering the device in the cloud via the Azure Device Provisioning
   Service prior to the IoT Hub connection and telemetry data publishing)

Default configuration: simple, non-encrypted connection.

Building attributes
-------------------

+-----------------+---------------------------------------------------+
|  Libraries      |                Description                        |
+-----------------+---------------------------------------------------+
|  AZURE_IOT_HUB  |  Enable the Azure IoT encrypted connection.       |
+-----------------+---------------------------------------------------+

+-----------------+---------------------------------------------------+
|  C Flags        |                Description                        |
+-----------------+---------------------------------------------------+
|  CONFIG_DPS     |  Enable the device registering in the cloud.      |
+-----------------+---------------------------------------------------+
|  RTC_DEFAULT    |  Set the default date and time for the RTC.       |
+-----------------+---------------------------------------------------+

Build Configuration Examples
----------------------------

1.
  .. code-block:: bash

    make PLATFORM=maxim TARGET=max32650

  - Default usage of the project consisting of simple, non-encrypted connection
    to a MQTT broker.

2.
  .. code-block:: bash

    make PLATFORM=maxim TARGET=max32650 NEW_CFLAGS=-DRTC_SET_DEFAULT

  - Default usage of the project consisting of simple, non-encrypted connection
    to a MQTT broker, including the RTC default date and time set required for
    obtaining relevant timestamp values.

3.
  .. code-block:: bash

    make PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y

  - Encrypted connection and telemetry data publishing into the Azure IoT Hub.

4.
  .. code-block:: bash

    make PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y NEW_CFLAGS=-DCONFIG_DPS

  - Encrypted connection to the Azure Device Provisioning Service for the device
    registering in the cloud, followed by encrypted connection and telemetry
    data publishing into the Azure IoT Hub.

5.
  .. code-block:: bash

    make PLATFORM=maxim TARGET=max32650 AZURE_IOT_HUB=y NEW_CFLAGS=-DCONFIG_DPS NEW_CFLAGS=-DRTC_SET_DEFAULT

  - Encrypted connection to the Azure Device Provisioning Service for the device
    registering in the cloud, followed by encrypted connection and telemetry
    data publishing into the Azure IoT Hub, including the RTC default date and
    time set required for obtaining relevand timestamp values.
