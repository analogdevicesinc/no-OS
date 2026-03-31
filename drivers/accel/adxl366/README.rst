ADXL366 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADXL366 <https://www.analog.com/ADXL366>`_

Overview
--------

The `ADXL366 <https://www.analog.com/ADXL366>`_ is an ultra-low-power 3-axis
MEMS accelerometer that consumes only 2 μA at a 100 Hz output data rate and
370 nA when in motion-triggered wake-up mode. The
`ADXL366 <https://www.analog.com/ADXL366>`_ provides a resolution of 14 bits
at ±2 g, ±4 g, and ±8 g measurement ranges.

The `ADXL366 <https://www.analog.com/ADXL366>`_ offers the lowest power
consumption in its class while still maintaining excellent noise and stability
performance. With its ultra-low power consumption and small form factor, the
`ADXL366 <https://www.analog.com/ADXL366>`_ is ideally suited for wireless
sensor network and Internet of Things (IoT) applications where battery life
is critical.

Applications
------------

* Wireless sensor networks (WSN)
* Internet of Things (IoT) nodes
* Wearable electronics
* Motion-triggered applications
* Battery-powered devices
* Portable health monitoring
* Asset tracking
* Vibration monitoring

ADXL366 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the SPI communication protocol.

The first API to be called is **adxl366_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will
verify the device ID registers to confirm communication with the ADXL366.

After initialization, it is recommended to perform a soft reset by calling
**adxl366_software_reset**, in order to put the device in a known state.

Range Configuration
-------------------

By default, the device uses ±2g range configuration. You may modify this
value to ±4g or ±8g by using **adxl366_set_range** API. To read back the
current range setting, use **adxl366_get_range** API.

Output Data Rate Configuration
-------------------------------

The ADXL366 supports output data rates from 12.5 Hz to 400 Hz. The default
configuration is typically 400 Hz. You may modify the ODR using
**adxl366_set_odr** API and read the current setting with
**adxl366_get_odr** API.

Power Mode Configuration
-------------------------

The ADXL366 supports multiple power modes. Use **adxl366_set_power_mode** API
to configure the power mode (standby or measurement). Use
**adxl366_get_power_mode** API to read the current power mode setting.

ADXL366 Device Measurements
----------------------------

Operation Mode Setting
----------------------

After the specific configuration was performed as mentioned above, you can set
the device in measurement mode using **adxl366_set_power_mode** API with the
measurement mode parameter.

Temperature Data
----------------

The ADXL366 includes an integrated temperature sensor. Temperature data can be
obtained by calling **adxl366_read_temperature** API. The temperature is
returned in millidegrees Celsius.

Acceleration Data
-----------------

To obtain acceleration data on all three axes, use **adxl366_read_accel** API.
This returns the raw 14-bit acceleration values for X, Y, and Z axes. The data
must be scaled according to the currently configured range to obtain values in
units of g.

Status Information
------------------

The device status can be checked using **adxl366_get_status** API, which
returns the status register value indicating data ready, FIFO status, and
activity detection flags.

ADXL366 Driver Initialization Example
--------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adxl366_dev *adxl366;
	struct adxl366_init_param init_param;
	int ret;

	// Configure SPI parameters
	struct no_os_spi_init_param spi_init = {
		.device_id = 0,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};

	init_param.spi_init = spi_init;

	// Initialize the device
	ret = adxl366_init(&adxl366, &init_param);
	if (ret < 0)
		goto error;

	// Perform software reset
	ret = adxl366_software_reset(adxl366);
	if (ret < 0)
		goto error;

	// Configure range to ±4g
	ret = adxl366_set_range(adxl366, ADXL366_RANGE_4G);
	if (ret < 0)
		goto error;

	// Configure ODR to 100Hz
	ret = adxl366_set_odr(adxl366, ADXL366_ODR_100HZ);
	if (ret < 0)
		goto error;

	// Set to measurement mode
	ret = adxl366_set_power_mode(adxl366, ADXL366_MODE_MEASURE);
	if (ret < 0)
		goto error;

	// Read acceleration data
	int16_t x, y, z;
	ret = adxl366_read_accel(adxl366, &x, &y, &z);
	if (ret < 0)
		goto error;

	// Read temperature
	int16_t temp;
	ret = adxl366_read_temperature(adxl366, &temp);
	if (ret < 0)
		goto error;

	// Read status
	uint8_t status;
	ret = adxl366_get_status(adxl366, &status);
	if (ret < 0)
		goto error;

error:
	adxl366_remove(adxl366);
	return ret;
