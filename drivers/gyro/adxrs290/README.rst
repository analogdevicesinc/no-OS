ADXRS290 no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`ADXRS290`

Overview
--------

The ADXRS290 is a high-performance MEMS pitch and roll rate gyroscope
designed for use in stabilization applications. It features an ultralow
noise performance of 0.004°/s/√Hz and delivers an output full-scale
range of ±100°/s with a sensitivity of 200 LSB/°/s, ensuring precise
angular rate measurements. The gyroscope incorporates programmable
high-pass and low-pass filters and boasts high vibration rejection over
a wide frequency range for robust operation in dynamic environments.
Additionally, it offers a power-saving standby mode with an 80 µA
current consumption, fast startup time (<100 ms), and a low delay (<0.5
ms for a 30 Hz input). Packaged in a compact 4.5 mm × 5.8 mm × 1.2 mm
cavity laminate format, the ADXRS290 is ideally suited for integration
into advanced motion sensing and stabilization systems.

Applications
------------

- Optical image stabilization
- Platform stabilization
- Wearable products

Operation Modes
----------------

+-----------------+--------------------+----------------------+------------------+
| Mode Name       | Description        | Configuration Bits   | Typical Use      |
+-----------------+--------------------+----------------------+------------------+
| Standby         | Low-power state,   | ADXRS290_TSM         | Minimal power    |
|                 | prepared for       |                      | consumption or   |
|                 | configuration and  |                      | temporary pause  |
|                 | conserving energy. |                      | in measurements. |
+-----------------+--------------------+---------------------+-------------------+
| Measurement     | Actively acquires  | ADXRS290_MEASUREMENT | Active data      |
|                 | sensor data for    |                      | acquisition or   |
|                 | angular velocity   |                      | real-time        |
|                 | and temperature.   |                      | monitoring.      |
+-----------------+--------------------+----------------------+------------------+

Device Configuration
--------------------

The ADXRS290 driver exposes a comprehensive API through its header file,
grouping its public functions into several categories based on their
roles in device configuration. Each of these functions is designed to
facilitate a specific aspect of sensor management from initialization to
data retrieval.

Initialization and Cleanup
~~~~~~~~~~~~~~~~~~~~~~~~~~

The initialization category includes the functions ``adxrs290_init()``
and ``adxrs290_remove()``. The ``adxrs290_init()`` function is
responsible for allocating necessary resources, establishing SPI
communication, setting up filter settings, and configuring the sensor’s
operational mode, while ``adxrs290_remove()`` properly deallocates
resources and cleans up the device instance when it is no longer needed.

Register Access
~~~~~~~~~~~~~~~

For low-level device interaction, the register access category provides
the functions ``adxrs290_reg_read()`` and ``adxrs290_reg_write()``. The
``adxrs290_reg_read()`` function enables reading from specific registers
via the SPI interface, whereas ``adxrs290_reg_write()`` allows for
writing data to designated registers, forming the foundation for direct
hardware communication.

Operational Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Managing the sensor’s overall operating state is achieved through the
``adxrs290_set_op_mode()`` function. This function transitions the
sensor between standby and measurement modes by manipulating power
control register settings, thereby integrating with various system power
management schemes.

Filter Configuration
~~~~~~~~~~~~~~~~~~~~

Precise control over signal conditioning is facilitated by the filter
configuration functions. The functions ``adxrs290_get_lpf()`` and
``adxrs290_set_lpf()`` retrieve and configure the low-pass filter
parameters, respectively, while ``adxrs290_get_hpf()`` and
``adxrs290_set_hpf()`` perform analogous operations for the high-pass
filter settings. This suite of functions allows for effective noise
reduction and signal filtering.

Data Acquisition
~~~~~~~~~~~~~~~~

Sensor data retrieval is managed by a trio of functions in the data
acquisition category. The ``adxrs290_get_rate_data()`` function is used
to acquire angular velocity (rate) data from a specified channel,
``adxrs290_get_temp_data()`` reads temperature measurements, and
``adxrs290_get_burst_data()`` supports burst-mode acquisitions for
obtaining multiple channel data rapidly.

Active Channel Management
~~~~~~~~~~~~~~~~~~~~~~~~~

The driver enables customization of sensor operations through active
channel management. The ``adxrs290_set_active_channels()`` function
configures which data channels are enabled by setting the appropriate
channel mask, ensuring that only the desired sensor streams are active.

Data Readiness
~~~~~~~~~~~~~~

To support reliable data processing, the data readiness category
includes the ``adxrs290_get_data_ready()`` function. This function
checks the status of the sensor’s data ready signal, providing a
synchronization mechanism that confirms when new measurement data is
available for retrieval.

Driver Initialization Example
------------------------------

.. code-block:: C

    #include <stdio.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include "adxrs290.h"
    #include "no_os_spi.h"
    #include "no_os_gpio.h"
    #include "no_os_error.h"

    static struct adxrs290_dev *gyro_dev;

    int main(void)
    {
        int32_t ret;
        struct adxrs290_init_param init_param = {
            .spi_init = {
                    .device_id      = 0,
                    .max_speed_hz   = 4000000,
                    .chip_select    = 0,
                    .mode           = NO_OS_SPI_MODE_3,
                    .platform_ops   = &aducm_spi_ops,
                    .extra          = &adxrs290_spi_extra_ip
            },
            .gpio_sync = {
                    .port           = 0x10,
                    .number         = 0,
                    .pull           = NO_OS_PULL_NONE,
                    .platform_ops   = &aducm_gpio_ops,
                    .extra          = NULL
            },
            .mode       = ADXRS290_MODE_MEASUREMENT,
            .lpf        = ADXRS290_LPF_480HZ,
            .hpf        = ADXRS290_HPF_ALL_PASS
        };

        /* Initialize the ADXRS290 device */
        ret = adxrs290_init(&gyro_dev, &init_param);
        if (ret != 0) {
            printf("ADXRS290 initialization failed: %d\n", ret);
            return ret;
        }

        printf("ADXRS290 initialization successful\n");

        /* Perform cleanup */
        ret = adxrs290_remove(gyro_dev);
        if (ret != 0) {
            printf("ADXRS290 removal failed: %d\n", ret);
            return ret;
        }

        return 0;
    }
