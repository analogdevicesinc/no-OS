LM75 no-OS Driver
==================

Supported Devices
------------------

- :adi:`LM75`

Overview
--------

The LM75 is a digital temperature sensor and thermal watchdog with 2-wire 
interface. It is available in 8-pin SOP and uSOP packages - incorporating a 
9-bit Delta Sigma ADC that allows for temperature readings with precision of 
0.5°C. It is compatible with ADT75 and AD7416 register formats, functioning 
over a voltage range of 3.3V to 5V, with a typical power consumption of 5mA.The
device features a low-power shutdown mode with a typical consumption of 4µA,
and it operates within a temperature range of −55°C to +125°C. Address
configuration is carried out via pins A0, A1, and A2, while the OS/ALERT
pin, which is open-drain, activates if the temperature exceeds
user-defined limits. This pin can be set to comparator or interrupt
modes, making the LM75 well-suited for diverse temperature monitoring
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

Setup and Teardown
~~~~~~~~~~~~~~~~~~

The ``lm75_init`` function manages the setup of the LM75 device
structure. It allocates memory for the device descriptor and initializes
the I2C communication interface using specified parameters. The function
includes mechanisms to gracefully handle errors in memory allocation or
I2C setup by releasing any allocated resources, thus preserving system
integrity.

The ``lm75_remove`` function handles the deinitialization and release of memory
structures that the lm75 driver allocated.

Configuration
~~~~~~~~~~~~~

A set of APIs are available for the configuration of the LM75 temperature
sensor. The range of configuration includes functions to bring the LM75 into
shutdown or low power mode, set the overtemperature and hyteresis values, 
change the fault tolerance values as well as change to polarity of the 
overtemperature signal.

The ``lm75_write_temperature`` function can target both the hysteresis and 
overtemperature setting of LM75. This function provides the user with an
interface to change the raw temperature values of LM75.

The ``lm75_shutdown`` function is used to bring the LM75 in or out of low-power
mode.

The ``lm75_write_os_polarity`` function is used to update the polarity of the 
OS signal of LM75 to either active high or active low.

The ``lm75_read_fault_queue`` function is used to update the fault tolerance 
of LM75 which can help to prevent tripping in noisy environments.

Data Management
~~~~~~~~~~~~~~~

Data management with the ADT75 involves functions such as
``lm75_read_temperature``, ``lm75_read_mode``. ``lm75_read_os_polarity``, 
and ``lm75_read_fault_queue``. The most useful of which is the 
``lm75_read_temperature`` function which will be used for temperature monitoring.

Temperature reads are raw LM75 values which need further processing to extract
the actual Celcius temperature reading. We use the function 
``lm75_raw_to_celcius`` to convert the raw temperature value to Celcius.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdlib.h>
   #include <stdint.h>
   #include "no_os_i2c.h"
   #include "lm75.h"
   #include "no_os_delay.h"
   #include "no_os_error.h"
   #include "no_os_util.h"
   #include "no_os_units.h"
   #include "no_os_alloc.h"

   int main()
   {
           struct lm75_init_param lm75_ip = {
           .fault_count = 0, /* POR state */
           .os_polarity = lm75_os_active_low,
           .i2c_ip = &lm75_i2c_ip,
           };
           struct lm75_dev *lm75;

           int ret;
           uint16_t raw_temp;
           float celcius;

           ret = lm75_init(&lm75, &lm75_ip);
           if (ret != 0) {
                   printf("LM75 initialization failed: %d\n", ret);
                   goto init_error;
           }
           printf("ADT75 initialization successful\n");

           /* Read raw temperature value */
           ret = lm75_read_temperature(lm75, lm75_die_temperature, &raw_temp);
           if (ret != 0) {
                   printf("Failed to read temperature: %d\n", ret);
                   goto init_error;
           }

           /* and convert it to celcius to make it more meaningful */
           lm75_raw_to_celcius(lm75, raw_temp, &celcius);
           printf("Temperature: %f celcius\n", celcius);

           /* Clean up and exit */
           lm75_remove(lm75);
           return 0;

   init_error:
           if (lm75)
                   lm75_remove(lm75);
           return ret;
   }

IIO Support
-----------

The LM75 driver is designed to support the Industrial I/O (IIO)
framework, which is a subsystem in the Linux kernel for interfacing with
industrial sensors and actuators. The IIO framework provides a unified
API for accessing various sensors, enabling applications to read sensor
data, configure sensors, and manage their lifecycle in a consistent
manner. The LM75 driver integrates with this framework, allowing users
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
