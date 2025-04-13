ADP1055 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADP1055 <https://www.analog.com/ADP1055>`_

Overview
--------

The ADP1055 is an advanced digital controller with a PMBus™ interface targeting
high density, high efficiency dc-to-dc power conversion.
This controller implements voltage mode control with high speed, input voltage
feedforward operation for enhanced transient and noise performance.
The ADP1055 has four programmable pulse-width modulation (PWM) outputs capable
of controlling most high efficiency power supply topologies,
with the added control of synchronous rectification (SR).

Applications
------------

ADP1055
-------

* High density, isolated dc-to-dc power supplies
* Intermediate bus converters
* High availability parallel power systems
* Server, storage, industrial, networking, and communications infrastructure

ADP1055 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **adp1055_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter, which contains an unsigned 8-bit integer variable on_off_config
containing the information to be sent to the ON_OFF_CONFIG command in the
PMBus commands which tells the device how it will be started-up, default
configuration for this can be found in the header file of the driver.

VOUT Configuration
------------------

Since the device is to be connected in parallel to other circuit, VOUT is
referring to the VS pins which are actually Voltage Input pins to which a
resistor divider is applied in orde to have the VS differential voltage at 1V. 
Depending on the resistor divider information a scale is to be set for the
VOUT using the **adp1055_vout_scale** API.

The VOUT_COMMAND sets the output voltage referrence which commands the PWMs
(actual outputs of the ADP1055), if the voltage sensed by the VS pins is greater
than VOUT_COMMAND, the PWMs will be set off, regarding the state set by the user.
Also the VOUT_MAX value can be changed with the same API, and this applies a
limit to the previous mentioned VOUT_COMMAND. The API used for setting these
two values is **adp1055_vout_value**.

VOUT also has an offset that can be set in order to correctly sense the voltage
output of the parallel-connected circuit using **adp1055_vout_offset** API, and
also a transition rate using **adp1055_vout_tr** API.

For VOUT margins can also be set for the case where at the start-up of the
device margin low/high values will be used for the voltage output value using
**adp1055_vout_margin** API.

Voltage value sensed on the VS pins can be read using **adp1055_read_vsense**
API, correct values will be returned only if vout scale is set using the
previous mentioned API.

VIN Configuration
-----------------

VIN_ON and VIN_OFF command values can be set using **adp1055_set_vin** API and
they refer to the input voltage at which the unit starts, respectively stops the
power conversion.

More than that the value of the input voltage(value read at the VF pin) can be
read using **adp1055_read_value** API, and correct values will be returned only
if VIN_SCALE_MONITOR command value is written accordingly to the datasheet using
**adp1055_write** API.

Status Configuration
--------------------

Statuses bytes/words of the device can be read using **adp1055_read_status**
API.

PWM configuration
-----------------

PWMs duty-cycle and modulation can be set using the **adp1055_pwm_config** 
API but also each PWM can be enabled using **adp1055_set_pwm** API which
also selects the frequency of the specific set PWM channel.

Frequency Synchronization
-------------------------

Frequency Synchronization can be disabled or enabled using **adp1055_freq_sync**
API.

ADP1055 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct adp1055_desc *adp1055_desc;
	struct no_os_i2c_init_param adp1055_i2c_ip = {
		.device_id = 0,
		.max_speed_hz = 100000,
		.platform_ops = &max_i2c_ops,
		.slave_address = ADP1055_PMBUS_DEFAULT_ADDRESS,
		.extra = &adp1055_i2c_extra,
	};
	struct no_os_gpio_init_param adp1055_pg_alt_ip = {
		.port = 0,
		.number = 24,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = *&adp1055_gpio_extra_ip,
	};
	struct adp1055_init_param adp1055_ip = {
		.i2c_param = &adp1055_i2c_ip,
		.pg_alt_param = &adp1055_pg_alt_ip,
		.flgi_param = NULL,
		.syni_param = NULL,
		.on_off_config = ADP1055_ON_OFF_DEFAULT_CFG,
	};
	ret = adp1055_init(&adp1055_desc, &adp1055_ip);
	if (ret)
		goto error;

ADP1055 no-OS IIO support
-------------------------

The ADP1055 IIO driver comes on top of the ADP1055 driver and offers support
for interfacing IIO clients through libiio.

ADP1055 IIO Device Configuration
--------------------------------

Input Channel Attributes
------------------------

VOUT/VIN/IIN/TEMP channels are the input channels of the ADP1055 IIO device
and each of them has a total of 2 channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``

Output Channel Attributes
-------------------------

OUTA/OUTB/OUTC/OUTD/SR1/SR2 channels are six output channels of the ADP1055 IIO device
and each of them has a total of 3 channel attributes:

* ``enable - state of the channel``
* ``frequency - frequency of the channel, all channels share the same frequency``
* ``frequency_available - list of available frequency for the output channels``

Global Attributes
-----------------

The device has a total of 3 global attributes:

* ``vout_command - VOUT_COMMAND value of the device``
* ``vout_scale_monitor - VOUT_SCALE_MONITOR value of the device``
* ``vout_offset - VOUT_OFFSET value of the device``

Debug Attributes
----------------

* ``status_vout - VOUT status byte value of the device``
* ``staus_iout - IOUT status byte value of the device``
* ``status_input - INPUT status byte value of the device``
* ``status_temperature - TEMPERATURE status byte value of the device``
* ``status_cml - CML status byte value of the device``
* ``status_other - OTHER status byte value of the device``
* ``status_word - status WORD value of the device``
* ``status_mfr - MRF status value of the device``

ADP1055 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct adp1055_iio_desc *adp1055_iio_desc;
	struct adp1055_iio_desc_init_param adp1055_iio_ip = {
		.adp1055_init_param = &adp1055_ip,
		.vout_scale_monitor = 0xA155,
		.vin_scale_monitor = 0xB033,
		.iin_scale_monitor = 0x01,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp1055_iio_init(&adp1055_iio_desc, &adp1055_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp1055",
			.dev = adp1055_iio_desc,
			.dev_descriptor = adp1055_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp1055_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp1055;

	return iio_app_run(app);
