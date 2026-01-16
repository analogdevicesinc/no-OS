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
shutdown or low power mode, set the overtemperature and hysteresis values, 
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

Data management with the LM75 involves functions such as
``lm75_read_temperature``, ``lm75_read_mode``. ``lm75_read_os_polarity``, 
and ``lm75_read_fault_queue``. The most useful of which is the 
``lm75_read_temperature`` function which will be used for temperature monitoring.

Temperature reads are raw LM75 values which need further processing to extract
the actual milliCelsius temperature reading. We use the function 
``lm75_raw_to_millicelsius`` to convert the raw temperature value to 
milliCelsius. Conversely, if we have a milliCelsius and want to have a raw 
temperature for writing to the over-temperature and hysteresis, we can use the
``lm75_millicelsius_to_raw`` function.

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
           int mc;

           ret = lm75_init(&lm75, &lm75_ip);
           if (ret != 0) {
                   printf("LM75 initialization failed: %d\n", ret);
                   goto init_error;
           }
           printf("LM75 initialization successful\n");

           /* Read raw temperature value */
           ret = lm75_read_temperature(lm75, lm75_die_temperature, &raw_temp);
           if (ret != 0) {
                   printf("Failed to read temperature: %d\n", ret);
                   goto init_error;
           }

           /* and convert it to MilliCelsius to make it more meaningful */
           mc = lm75_raw_to_millicelsius(raw_temp);
           printf("Temperature: %d MilliCelsius\n", mc);

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

The LM75 IIO design mimics the way Linux hwmon presents the LM75 driver to 
userspace via IIO. That is, it exposes attributes like input, max and max_hyst.

IIO Initialization Functions
----------------------------

The initialization of the IIO example code for the LM75 temperature
sensor involves several key functions: ``iio_lm75_init`` prepares the
LM75 IIO descriptor using specified just a plain lm75 device descriptor
which has been created and initialized prior. The ``iio_app_init`` function
configures the IIO environment by setting up devices like the LM75 sensor.
It also orchestrates the process of integrating the sensor into the IIO 
framework and configuring the application to acquire and process temperature
data continuously with ``iio_app_run``. This initialization process ensures
proper operation and data handling from the sensor within the IIO
infrastructure.

IIO Device Configuration
------------------------

Post-initialization, the LM75 IIO driver exposes a single temperature type 
channel ``temp`` with 3 attributes, aligning closely with the Linux kernel way
of exposing sensors of this type to the user via hwmon and IIO.

The key attributes are ``input`` which is the raw die temperature, ``max``
which is the threshold temperature which the chip uses to trigger an over 
temperature event/alarm and lastly, the ``max_hyst`` which provides the 
temperature value where the alarm condition is not valid.

IIO Initialization Example
--------------------------

.. code-block:: C

   int iio_example_main()
   {
           int ret;
           struct lm75_init_param lm75_ip = {
                   .fault_count = 0,
                   .os_polarity = lm75_os_active_low,
                   .i2c_ip = &lm75_i2c_ip,
           };
           struct lm75_dev *lm75;
           struct iio_lm75 *iio_lm75;
           struct iio_app_desc *app;
           struct iio_app_init_param app_init_param = { 0 };

           /* initialize the lm75 device separately */
           ret = lm75_init(&lm75, &lm75_ip);
           if (ret)
                   return ret;

           ret = iio_lm75_init(&iio_lm75, lm75);
           if (ret)
                   goto err_cleanup_lm75;

           struct iio_app_device iio_devices[] = {
                   {
                           .name = "lm75",
                           .dev = iio_lm75,
                           .dev_descriptor = &iio_lm75->iio_dev,
                   }
           };

           app_init_param.devices = iio_devices;
           app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
           app_init_param.uart_init_params = lm75_uart_ip;

           ret = iio_app_init(&app, app_init_param);
           if (ret)
                   goto err_cleanup_lm75_iio;

           ret = iio_app_run(app);

   err_cleanup_lm75_iio:
           iio_lm75_remove(iio_lm75);
   err_cleanup_lm75:
           lm75_remove(lm75);
           return ret;
   }
