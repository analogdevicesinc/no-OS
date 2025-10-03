MAX17616 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `MAX17616/MAX17616A <https://www.analog.com/MAX17616>`_

Overview
--------

The MAX17616/MAX17616A offers highly versatile and programmable protection
boundaries for systems against input voltage faults and output overcurrent
faults. Input-voltage faults (with positive polarity) are protected up to +80V
(without Reverse Current Protection)/+75V (with Reverse Current Protection), by
an internal nFET featuring low ON-resistance (20mΩ typ). The devices feature a
programmable undervoltage-lockout (UVLO) thresholds by using external voltage-
dividers. The MAX17616 features a programmable overvoltage-lockout (OVLO) while
MAX17616A offers a programmable output voltage clamp function through the OVFB
pin that features an output voltage limiting regulation during input transient
surge events. Input undervoltage and overvoltage protection (MAX17616)/output
voltage clamp function (MAX17616A) can be programmed across the entire 3V to 80V
operating range.

Applications
------------

MAX17616
--------

* Input Voltage and Output Overcurrent Protections
* Loss of Ground Protection
* Surge Protection

MAX17616 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **max17616_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter.

Status Bytes
------------

Assertion in the status bytes/words indicates fault/warning in device input/
output, temperature, and communication, memory and logic. These statuses can be
accessed via the **max17616_read_status** API.

Telemetry
---------

Measurements for each output channel can be read using the
**max17616_read_telemetry_all** API. Some telemetry values includes input/output
voltage, input current, temperature, and output power.

MAX17616 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max17616_dev *max17616_dev;

	struct no_os_i2c_init_param i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = I2C_CLK_SPEED,
		.platform_ops = I2C_OPS,
		.slave_address = I2C_ADDR,
		.extra = I2C_EXTRA,
	};

	struct max17616_init_param max17616_ip = {
		.i2c_init = &_i2c_ip,
		.chip_id = ID_MAX17616,
	};

	ret = max17616_init(&max17616_dev, &max17616_ip);
	if (ret)
		goto error;

MAX17616 no-OS IIO support
--------------------------

The MAX17616 IIO driver comes on top of the MAX17616 driver and offers support
for interfacing IIO clients through libiio.

MAX17616 IIO Device Configuration
---------------------------------

Channels
--------

The device has a total of 15 input channels and 7 output channels:

**Telemetry Input Channels:**

* ``vin - input voltage``
* ``vout - output voltage``
* ``iout - output current``
* ``temp1 - device temperature``
* ``pout - output power (calculated from vout * iout)``

**Status Input Channels:**

* ``status_word - 16-bit comprehensive status register (hex)``
* ``status_vout - VOUT status byte (hex)``
* ``status_iout - IOUT status byte (hex)``
* ``status_input - INPUT status byte (hex)``
* ``status_temperature - TEMPERATURE status byte (hex)``
* ``status_cml - Communication/Memory/Logic status byte (hex)``
* ``status_mfr_specific - Manufacturer-specific status byte (hex)``
* ``capability - Device capability register (hex)``

**Control Output Channels:**

* ``operation - device operation state (0=disabled, 1=enabled)``
* ``clmode - current limit mode setting``
* ``istart_ratio - current start ratio setting``
* ``tstoc - short circuit timeout setting``
* ``istlim - current limit setting``
* ``vout_uv_fault_limit - output undervoltage fault limit setting``

Input Channel Attributes
------------------------

The telemetry input channels (vin, vout, iout, temp1, pout) each have 2 channel
attributes:

* ``raw - the raw converted value from the device using DIRECT format conversion``
* ``scale - the scale factor (always returns 1 for DIRECT format values)``

The status input channels (status_word, status_vout, status_iout, status_input,
status_temperature, status_cml, status_mfr_specific, capability) each have
1 channel attribute:

* ``raw - the raw hexadecimal value of the status register``

Output Channel Attributes
-------------------------

The control output channels (operation, clmode, istart_ratio, tstoc, istlim,
vout_uv_fault_limit) each have 1 channel attribute:

* ``raw - read/write the integer value of the control setting``

Global Attributes
-----------------

The device has a total of 4 global attributes:

* ``operation - device operation state (read/write: "enabled"/"disabled" or "1"/"0")``
* ``clear_faults - clears all asserted faults (write-only, write any value to trigger)``
* ``device_info - device identification information (read-only string)``
* ``fault_summary - summary of current active faults (read-only string)``

Debug Attributes
----------------

The device has no debug attributes implemented.

MAX17616 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max17616_iio_desc *max17616_iio_desc;
	struct max17616_iio_desc_init_param max17616_iio_ip = {
		.max17616_init_param = &max17616_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max17616_iio_init(&max17616_iio_desc, &max17616_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max17616",
			.dev = max17616_iio_desc,
			.dev_descriptor = max17616_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
