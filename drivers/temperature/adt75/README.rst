ADT75 no-OS Driver
==================

Supported Devices
------------------

- :adi:`ADT75`

Overview
--------

The ADT75 is a temperature monitoring system available in 8-lead MSOP
and SOIC packages, incorporating a 12-bit ADC that allows for
temperature readings with a precision of 0.0625°C. It is compatible with
LM75 and AD7416 register formats, functioning over a voltage range of
2.7V to 5.5V, with a typical power consumption of 200µA at 3.3V. The
device features a low-power shutdown mode with a consumption of 3µA, and
it operates within a temperature range of −55°C to +125°C. Address
configuration is carried out via pins A0, A1, and A2, while the OS/ALERT
pin, which is open-drain, activates if the temperature exceeds
user-defined limits. This pin can be set to comparator or interrupt
modes, making the ADT75 well-suited for diverse temperature monitoring
applications.

Applications
-------------

- Isolated sensors
- Environmental control systems
- Computer thermal monitoring
- Thermal protection
- Industrial process control
- Power-system monitors
- Hand-held applications

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The ``adt75_init`` function manages the setup of the ADT75 device
structure. It allocates memory for the device descriptor and initializes
the I2C communication interface using specified parameters. The function
includes mechanisms to gracefully handle errors in memory allocation or
I2C setup by releasing any allocated resources, thus preserving system
integrity.

Configuration
~~~~~~~~~~~~~

Configuration of the ADT75 temperature sensor is facilitated by the
``adt75_reg_write`` function. This function enables writing raw values
to specified registers within the device, adjusting the register size,
and preparing the data buffer for I2C write operations. It allows for
setting operational modes and adjusting sensor settings to ensure
operation within desired parameters.

Data Management
~~~~~~~~~~~~~~~

Data management with the ADT75 involves functions such as
``adt75_reg_read`` and ``adt75_get_single_temp``. ``adt75_reg_read`` 
is responsible for retrieving raw register values and handles both
single-byte and double-byte registers, essential for extracting
configuration or status data. Meanwhile, ``adt75_get_single_temp``
focuses on capturing temperature readings, converting raw register data
into milliCelsius values for application use, with both functions
prioritizing structured error handling and data integrity.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdlib.h>
   #include <stdint.h>
   #include "no_os_i2c.h"
   #include "adt75.h"
   #include "no_os_delay.h"
   #include "no_os_error.h"
   #include "no_os_util.h"
   #include "no_os_units.h"
   #include "no_os_alloc.h"

   int main()
   {
       adt75_init_param adt75_param = {
           .comm_param = {
               .device_id      = 1,
               .max_speed_hz   = 400000,
               .slave_address  = ADT75_ADDR(0),
               .platform_ops   = &max_i2c_ops, // Replace with your I2C operations
               .extra          = &adt75_i2c_extra,
           }
       };

       adt75_desc *adt75_dev = NULL;
       int32_t ret;
       int32_t reg_val;

       ret = adt75_init(&adt75_dev, &adt75_param);
       if (ret != 0) {
           printf("ADT75 initialization failed: %d\n", ret);
           goto init_error;
       }
       printf("ADT75 initialization successful\n");

       /* Read temperature value */
       ret = adt75_get_single_temp(adt75_dev, &reg_val);
       if (ret != 0) {
           printf("Failed to read temperature: %d\n", ret);
           goto init_error;
       }
       printf("Temperature: %ld mC\n", (long)reg_val);

       /* Clean up and exit */
       adt75_remove(adt75_dev);
       return 0;

   init_error:
       if (adt75_dev)
           adt75_remove(adt75_dev);
       return ret;
   }

IIO Support
-----------

The ADT75 driver is designed to support the Industrial I/O (IIO)
framework, which is a subsystem in the Linux kernel for interfacing with
industrial sensors and actuators. The IIO framework provides a unified
API for accessing various sensors, enabling applications to read sensor
data, configure sensors, and manage their lifecycle in a consistent
manner. The ADT75 driver integrates with this framework, allowing users
to leverage the IIO application for temperature monitoring tasks. 

IIO Initialization Functions
----------------------------

The initialization of the IIO example code for the ADT75 temperature
sensor involves several key functions: ``adt75_iio_init`` prepares the
ADT75 IIO descriptor using specified initialization parameters
(``adt75_iio_init_param``), which includes UART and I2C setup. The
``iio_app_init`` function configures the IIO environment by setting up
devices like the ADT75 sensor. The main function, ``iio_example_main``,
orchestrates this process, integrating the sensor into the IIO framework
and configuring the application to acquire and process temperature data
continuously with ``iio_app_run``. This initialization process ensures
proper operation and data handling from the sensor within the IIO
infrastructure.

IIO Device Configuration
------------------------

Post-initialization, the key functions essential for running the IIO
example for the ADT75 temperature sensor include ``adt75_iio_read_raw``,
``adt75_iio_read_scale``, ``adt75_iio_read_offset``, and
``adt75_iio_read_samples``. These functions are responsible for
acquiring raw temperature data, handling scale and offset adjustments
for accurate readings, and supporting batch processing of multiple
temperature samples, respectively. Initialization and cleanup are
performed by the ``adt75_iio_init`` and ``adt75_iio_remove`` functions
to manage resource allocation and deallocation within the IIO framework,
enabling continuous and precise temperature data acquisition for further
application-level processing.

IIO Initialization Example
--------------------------

.. code-block:: C

   int iio_example_main()
   {
       int ret;
       struct adt75_iio_desc *adt75_iio_desc;
       struct adt75_iio_init_param adt75_iio_ip;
       struct iio_app_desc *app;
       struct iio_data_buffer buff = {
           .buff = (void *)iio_data_buffer,
           .size = DATA_BUFFER_SIZE * sizeof(int16_t)
       };
       struct iio_app_init_param app_init_param = { 0 };

       adt75_iio_ip.adt75_init_param = &adt75_ip;
       ret = adt75_iio_init(&adt75_iio_desc, &adt75_iio_ip);
       if (ret)
           return ret;

       struct iio_app_device iio_devices[] = {
           {
               .name = "adt75",
               .dev = adt75_iio_desc,
               .dev_descriptor = adt75_iio_desc->iio_dev,
               .read_buff = &buff,
           }
       };

       app_init_param.devices = iio_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
       app_init_param.uart_init_params = adt75_uart_ip;

       ret = iio_app_init(&app, app_init_param);
       if (ret)
           return ret;

       return iio_app_run(app);
   }
