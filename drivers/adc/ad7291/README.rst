AD7291 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD7291 <https://www.analog.com/AD7291>`_

Overview
--------

The AD7291 is a 12-bit, 8-channel analog-to-digital converter with an
integrated temperature sensor, communicating over an I2C interface. It
operates from a 2.5 V to 5.5 V supply and features an internal 2.5 V
reference, eliminating the need for an external voltage reference in many
applications. Each voltage channel can be independently enabled for
conversion, and the device supports programmable high and low limit
registers with hysteresis for alert monitoring on all channels and the
temperature sensor. The temperature sensor provides 0.25 deg C resolution
with a range of -40 deg C to +125 deg C. The AD7291 also supports autocycle
mode for continuous monitoring without processor intervention. Available
in a 20-lead LFCSP, it is rated for the industrial temperature range of
-40 deg C to +85 deg C.

Applications
------------

* Power monitoring in embedded systems
* Temperature and voltage monitoring
* Phased array antenna systems
* System health monitoring
* Industrial process control

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``ad7291_init`` function initializes the AD7291 device. It allocates
memory for an ``ad7291_desc`` structure, initializes I2C communication using
the settings provided in the ``ad7291_init_param`` structure, and sets the
reference voltage. If no external reference voltage is specified, the
internal 2.5 V reference is used. The command register is then configured
with the delay bit set to allow settling time between channel conversions.

I2C Communication
~~~~~~~~~~~~~~~~~

The driver communicates with the AD7291 over I2C using ``ad7291_reg_read``
and ``ad7291_reg_write``. These functions handle low-level reading and
writing of 16-bit device registers. A read transaction sends the register
address followed by a two-byte read, while a write transaction sends the
register address followed by the two data bytes in big-endian order.

Voltage Channel Reading
~~~~~~~~~~~~~~~~~~~~~~~

The ``ad7291_read_channel_voltage`` function reads a single ADC channel
and returns the result in millivolts. It configures the command register
to enable only the requested channel (0-7) with the delay bit set,
reads the voltage result register, verifies the channel address field
matches the requested channel, and converts the 12-bit code to millivolts
using the configured reference voltage.

Temperature Sensor Reading
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad7291_read_temp`` function reads the internal temperature sensor
and returns the result in millidegrees Celsius. It enables the temperature
sensor in the command register, reads the temperature data register,
performs sign extension for negative temperatures, and converts the
12-bit value to millidegrees (1 LSB = 0.25 deg C).

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "ad7291.h"
   #include "no_os_i2c.h"

   struct ad7291_desc *device;
   int32_t ret;

   struct ad7291_init_param init_param = {
       .i2c_init = {
           .device_id   = I2C_DEVICE_ID,
           .max_speed_hz = 400000,
           .slave_address = 0x20,
           .platform_ops = I2C_OPS,
           .extra        = I2C_EXTRA,
       },
       .vref_mv = 2500,
   };

   ret = ad7291_init(&device, &init_param);
   if (ret)
       goto error;
