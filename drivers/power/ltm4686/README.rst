LTM4686 no-OS driver
====================

Supported Devices
-----------------

`LTM4686 <https://www.analog.com/LTM4686>`_

Overview
--------

The LTM4686 is a dual 10A (12A Peak) or single 20A (24A Peak) step-down µModule
DC/DC regulator with 39ms turn-on time. It features remote configurability and
telemetry-monitoring of power management parameters over PMBus.

Applications
------------

LTM4686
-------

* System Optimization in Prototype and Production

LTM4686 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **ltm4686_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the **chip_id** of the device 
and the optional GPIO parameters for controlling POWER GOOD, RUN, FAULT and
ALERT pins. Use of packet error checking and the external clock can also be
configured in the parameter. These are defined in the header file of the driver.

VOUT Configuration
------------------

The VOUT_COMMAND sets the output voltage, while the VOUT_MAX sets the upper
limit on the commanded voltage. The output voltage and limit for each channel
can be set using the **ltm4686_vout_value** API.

The output voltage transition rate for each channel can be configured using the
**ltm4686_vout_tr** API.

Output voltage lower and higher margin can also be configured using the
**ltm4686_vout_margin** API.

The OPERATION command via the **ltm4686_set_operation** can be used to command
the output voltage to be regulated at the high margin, low margin, or at the
nominal VOUT_COMMAND. This can also be used to set a channel off.

VIN Configuration
-----------------

VIN_ON and VIN_OFF command values sets the input voltage window at which power
conversion will proceed. Both of which can be configured through the
**ltm4686_set_vin** API.

Status Configuration
--------------------

Statuses bytes/words of the device can be read using **ltm4686_read_status**
API.

Telemetry
---------

Measurements for each output channel can be read using the
**ltm4686_read_value** API. Some telemetry values includes input/output voltage,
input/output current, die temperature, and output power.

Timing Configuration
--------------------

The TON_DELAY command sets the time from when a start condition is received
until the output voltage starts to rise. The TON_RISE sets the time from the
from the time the output starts to rise to the time the output enters the
regulation band. The TOFF_DELAY command sets the time from when a stop condition
is received until the output voltage starts to fall. The TOFF_FALL command sets
the time from the end of the turn-off delay time until the output voltage is
commanded to zero. These commands can be set using the **ltm4686_set_timing**
API.

PWM Configuration
-----------------

The PWM of each output channel can be set on forced continuous mode or on
pulse skip mode. This can be set using the **ltm4686_pwm_mode** API.

NVM commands
------------

The device has an internal EEPROM that saves the settings configured to the
device even when power is removed. The **ltm4686_nvm_cmd** API allows the user
to store, compare and restore user settings.

Software Reset Configuration
----------------------------

Software Reset operation is available through **ltm4686_software_reset** API.

LTM4686 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct ltm4686_dev *ltm4686_dev;
        struct no_os_i2c_init_param ltm4686_i2c_ip = {
                .device_id = I2C_DEVICE_ID,
                .max_speed_hz = 100000,
                .platform_ops = I2C_OPS,
                .slave_address = LTM4686_PMBUS_ADDRESS,
                .extra = I2C_EXTRA,
        };

        struct ltm4686_init_param ltm4686_ip = {
                .i2c_init = &ltm4686_i2c_ip,
                .external_clk_en = false,
                .crc_en = true,
        };
	ret = ltm4686_init(&ltm4686_dev, &ltm4686_ip);
	if (ret)
		goto error;

LTM4686 no-OS IIO support
-------------------------

The LTM4686 IIO driver comes on top of the LTM4686 driver and offers support
for interfacing IIO clients through libiio.

LTM4686 IIO Device Configuration
--------------------------------

Input Channel Attributes
------------------------

VIN0/IIN0/TEMP channels are the input channels of the LTM4686 IIO
device and each of them has a total of two channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``

Output Channel Attributes
-------------------------

IOUT0/IOUT1 channels are two output channels with the following channel
attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``

Meanwhile, VOUT0/VOUT1 channels are two output channels with a separate channel
attributes. Each channel has eight attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``
* ``enable - state of the channel``
* ``enable_available - list of available states for the channel``
* ``vout_command - VOUT_COMMAND value of the channel output``
* ``vout_max - VOUT_COMMAND value of the channel output``
* ``vout_margin_low - VOUT_MARGIN_LOW value of the channel output``
* ``vout_margin_high - VOUT_MARGIN_HIGH value of the channel output``

Global Attributes
-----------------

The device has a total of seven global attributes:

* ``vout_ov_fault_limit_0 - Output overvoltage fault limit for channel 0``
* ``vout_ov_fault_limit_1 - Output overvoltage fault limit for channel 1``
* ``vout_uv_fault_limit_0 - Output undervoltage fault limit for channel 0``
* ``vout_uv_fault_limit_1 - Output undervoltage fault limit for channel 1``
* ``ot_fault_limit - Overtemperature fault limit for both channels``
* ``vin_ov_fault_limit - Input overvoltage fault limit for both channels``
* ``vin_uv_warn_limit - Output undervoltage warning limit for both channels``

Debug Attributes
----------------

The device has a total of 11 debug attributes:

* ``status_vout_0 - VOUT status byte value of channel 0``
* ``status_vout_1 - VOUT status byte value of channel 1``
* ``status_iout_0 - IOUT status byte value of channel 0``
* ``status_iout_1 - IOUT status byte value of channel 1``
* ``status_input - INPUT status byte value of channel 0``
* ``status_mfr_specific_0 - MFR_SPECIFIC status byte value of channel 0``
* ``status_mfr_specific_1 - MFR_SPECIFIC status byte value of channel 1``
* ``status_word_0 - Status word value of the channel 0``
* ``status_word_1 - Status word value of the channel 1``
* ``status_temperature - TEMPERATURE status byte value of the device``
* ``status_cml - CML status byte value of the device``

LTM4686 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct ltm4686_iio_desc *ltm4686_iio_desc;
	struct ltm4686_iio_desc_init_param ltm4686_iio_ip = {
		.ltm4686_init_param = &ltm4686_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltm4686_iio_init(&ltm4686_iio_desc, &ltm4686_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltm4686",
			.dev = ltm4686_iio_desc,
			.dev_descriptor = ltm4686_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltm4686_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);