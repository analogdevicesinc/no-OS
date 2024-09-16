LTP8800 no-OS driver
====================

Supported Devices
-----------------

`LTP8800-1A <https://www.analog.com/LTP8800-1A>`_
`LTP8800-4A <https://www.analog.com/LTP8800-4A>`_
`LTP8800-2 <https://www.analog.com/LTP8800-2>`_

Overview
--------

The LTP8800 is a family of step-down μModule regulators that provides
microprocessor core voltage from 54V power distribution architecture. LTP8800-4A
features remote configurability and telemetry monitoring of power management
parameters over PMBus—an open standard I2C-based digital interface protocol.

Applications
------------

LTP8800
-------

* High Current Distributed Power Systems
* Servers, Network, and Storage Equipment
* Intelligent Energy Efficient Power Regulation

LTP8800 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **ltp8800_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the optional GPIO pin
parameters for interfacing with SMBALERT and CTRL pins, and configurations for
external clock, polyphase, write protect and packet error checking.

Compensation
------------

The device offers a programmable loop compensation to optimize the transient
response without hardware change. The type 3 filter can be configured using the
**ltp8800_loop_compensation** API.

Polyphase Configuration
-----------------------

The device can be configured to operate in polyphase mode. When in polyphase,
each device can be configured to operate in a specific phase. The phase can be
set to a multiple of 22.5 degrees using the **ltp8800_interleave_order** API.

Status Bytes
------------

Assertion in the status bytes/words indicates fault/warning in device input/
output, temperature, and communication, memory and logic. These statuses can be
accessed via the **ltp8800_read_status** API.

Telemetry
---------

Measurements for each output channel can be read using the
**ltp8800_read_value** API. Some telemetry values includes input/output voltage,
input/output current, die temperature, and output power.

Overvalue and Undervalue Limits Configuration
---------------------------------------------

Overvalue and undervalue limits sets the threshold at which the device voltage,
current, and temperature must meet. When these measurements cross the limits, a
status bit may be asserted. These limits can be configured using the
**ltp8800_set_fault_limit** API.

VIN Configuration
-----------------

VIN_ON and VIN_OFF command values sets the input voltage window at which power
conversion will proceed. Both of which can be configured through the
**ltp8800_set_vin** API.

VOUT Configuration
------------------

The LTP8800 output voltage is programmable from 0.5V to 1.0V. These can be
configured using the **ltp8800_vout_value** API.

Clock Configuration
-------------------

When using the device in parallel with others of the same device, the PWM clock
of all devices can be synchronized. Using an external clock or enabling the
clock input can be configured using the **ltp8800_sync_config** API.

Device Configuration in NVM
---------------------------

User settings can be saved in the non-volatile EEPROM of the device. This can be
done using the **ltp8800_store_user_settings** API. Meanwhile, the settings from
the EEPROM can be restored using the **ltp8800_restore_user_settings** API.

LTP8800 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct ltp8800_dev *ltp8800_dev;
	struct no_os_i2c_init_param ltp8800_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = LTP8800_PMBUS_ADDRESS,
		.extra = I2C_EXTRA,
        };

	struct no_os_gpio_init_param ltp8800_ctrl_ip = {
		.port = GPIO_CTRL_PORT,
		.number = GPIO_CTRL_NUMBER,
		.pull = NO_OS_PULL_UP,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ltp8800_init_param ltp8800_ip = {
		.i2c_init = &ltp8800_i2c_ip,
		.smbalert_param = NULL,
		.ctrl_param = &ltp8800_ctrl_ip,
		.ext_clk_param = NULL,
		.write_protect_en = false,
		.external_clk_en = false,
		.sync_en = false,
		.crc_en = false,
	};

	ret = ltp8800_init(&ltp8800_dev, &ltp8800_ip);
	if (ret)
		goto error;

LTP8800 no-OS IIO support
-------------------------

The LTP8800 IIO driver comes on top of the LTP8800 driver and offers support
for interfacing IIO clients through libiio.

LTP8800 IIO Device Configuration
--------------------------------

Channels
--------

The device has a total of 3 input channels and 2 output channels. The input
consists of the input voltage, input current, and the forward diode
temperature. The output consists of the output voltage and current.

* ``vout - output voltage``
* ``iout - output current``
* ``vin - input voltage``
* ``iin - input current``
* ``temperature - forward diode temperature``

Channel Attributes
------------------

EAch channels have 2 channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel using a priv``

Global Attributes
-----------------

The device has a total of 18 global attributes:

* ``sync - Enable/Disable sync device configuration for polyphase application``
* ``sync_available - Available state of the sync enable``
* ``vout_command - VOUT_COMMAND value of the channel output``
* ``vout_scale_loop - VOUT_COMMAND gain for the internal reference voltage``
* ``vout_scale_monitor - VOUT_COMMAND gain for READ_VOUT``
* ``vin_ov_fault_limit - Input overvoltage fault limit``
* ``vin_uv_fault_limit - Input undervoltage fault limit``
* ``iin_oc_fault_limit - Output overcurrent fault limit``
* ``pout_op_fault_limit - Output overvoltage warning limit``
* ``interleave_order - Polyphase order``
* ``loop_pole - Compensation filter pole value``
* ``loop_zero - Compensation filter zero value``
* ``loop_hf_gain - Compensation filter high frequency gain``
* ``loop_lf_gain - Compensation filter low frequency gain``
* ``store_user_settings - Store user settings in NVM``
* ``store_user_settings_available - Available store_user_settings option``
* ``restore_user_settings - Restore user settings from NVM``
* ``restore_user_settings_available - Available restore_user_settings option``

Debug Attributes
----------------

The device has a total of 7 debug attributes:

* ``status_vout - VOUT status byte value``
* ``status_iout - IOUT status byte value``
* ``status_input - INPUT status byte value``
* ``status_mfr_specific - MFR_SPECIFIC status byte value``
* ``status_word - Status word value``
* ``status_temperature - TEMPERATURE status byte value of the device``
* ``status_cml - CML status byte value of the device``

LTP8800 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct ltp8800_iio_desc *ltp8800_iio_desc;
	struct ltp8800_iio_desc_init_param ltp8800_iio_ip = {
		.ltp8800_init_param = &ltp8800_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ltp8800_iio_init(&ltp8800_iio_desc, &ltp8800_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ltp8800",
			.dev = ltp8800_iio_desc,
			.dev_descriptor = ltp8800_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ltp8800_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
