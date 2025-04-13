LT7170 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `LT7170/LT7170-1 <https://www.analog.com/LT7170>`_
* `LT7171/LT7171-1 <https://www.analog.com/LT7171>`_

Overview
--------

The LT7170/LT7170-1 are monolithic DC/DC synchronous step-down regulators that 
deliver up to 20 A of continuous output current. The LT7170-1 option has two 
switching phases that are connected to two inductors to drive a 
single-regulated output supply.

Similarly, the LT7171/LT7171-1 are monolithic PolyPhase DC/DC synchronous 
step-down regulators that also deliver up to 20A of continuous output current. 
The LT7171-1 option has two switching phases that are connected to two 
inductors to drive a single-regulated output supply.

Both series offer quick, clean, low-overshoot switching edges deliver high 
efficiency while minimizing electromagnetic interference (EMI) emissions.

An I2C-based PMBus 1.3-compliant serial interface allows control of device 
functions while providing telemetry information for system monitoring. The 
LT7170/LT7170-1 and LT7171/LT7171-1 are supported by the LTpowerPlay graphical 
user interface (GUI) tool.

Applications
------------

LT7170
-------

* Communications, Storage, and Industrial Systems
* Data Center and Solid-State Drives

LT7170 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **lt7170_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the **chip_id** of the device 
and the optional GPIO
parameters for controlling POWER GOOD, RUN, FAULT and ALERT pins. Use of packet
error checking and the external clock can also be configured in the parameter.
These are defined in the header file of the driver.

VOUT Configuration
------------------

The VOUT_COMMAND sets the output voltage, while the VOUT_MAX sets the upper
limit on the commanded voltage. The output voltage and limit can be set using 
the **lt7170_vout_value** API.

The output voltage transition rate can be configured using the 
**lt7170_vout_tr** API.

Output voltage lower and higher margin can also be configured using the
**lt7170_vout_margin** API.

The OPERATION command via the **lt7170_set_operation** can be used to command
the output voltage to be regulated at the high margin, low margin, or at the
nominal VOUT_COMMAND. This can also be used to set a channel off.

VIN Configuration
-----------------

VIN_ON and VIN_OFF command values sets the input voltage window at which power
conversion will proceed. Both of which can be configured through the
**lt7170_set_vin** API.

Status Configuration
--------------------

Statuses bytes/words of the device can be read using **lt7170_read_status**
API. Faults asserted by these statuses can be cleared using the
**lt7170_clear_faults** API.

Telemetry
---------

Measurements can be read using the **lt7170_read_value** API. Some telemetry 
values includes input/output voltage,
input/output current, external VCC, die temperature, and output power.

The frequency at which telemetry values are converted, as well as enabling debug
telemetries such as external VCC and ITH values, can be configured using the
**lt7170_adc_control** API.

Timing Configuration
--------------------

The TON_DELAY command sets the time from when a start condition is received
until the output voltage starts to rise. The TON_RISE sets the time from the
from the time the output starts to rise to the time the output enters the
regulation band. The TOFF_DELAY command sets the time from when a stop condition
is received until the output voltage starts to fall. The TOFF_FALL command sets
the time from the end of the turn-off delay time until the output voltage is
commanded to zero. These commands can be set using the **lt7170_set_timing**
API.

Overvalue and Undervalue Limits Configuration
---------------------------------------------

Overvalue and undervalue limits sets the threshold at which the device voltage,
current, temperature, and timing must meet. When these measurements cross the
limits, a status bit may be asserted. These limits can be configured using the
**lt7170_set_limit** API.

PWM Configuration
-----------------

The PWM can be set on forced continuous mode or on
pulse skip mode. This can be set using the **lt7170_pwm_mode** API.

The phase offset of the PWM can also be set using the
**lt7170_pwm_phase** API.

Clock Configuration
-------------------

When using the device in parallel with others of the same device, the PWM clock
of all devices can be synchronized. Using an external clock or enabling the
clock input can be configured using the **lt7170_sync_config** API.

NVM commands
------------

The device has an internal EEPROM that saves the settings configured to the
device even when power is removed. The **lt7170_nvm_cmd** API allows the user
to lock and unlock EEPROM, and store, compare and restore user settings.

Software Reset Configuration
----------------------------

Software Reset operation is available through **lt7170_software_reset** API.

LT7170 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct lt7170_dev *lt7170_dev;
        struct no_os_i2c_init_param lt7170_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = LT7170_PMBUS_ADDRESS,
		.extra = I2C_EXTRA,
	};

	struct lt7170_init_param lt7170_ip = {
		.i2c_init = &lt7170_i2c_ip,
		.pg_param = NULL,
		.run_param = NULL,
		.alert_param = NULL,
		.fault_param = NULL,
		.fault_cfg = LT7170_FAULT_PIN_OUTPUT,
		.chip_id = ID_LT7170,
		.external_clk_en = false,
		.crc_en = false,
	};
	ret = lt7170_init(&lt7170_dev, &lt7170_ip);
	if (ret)
		goto error;

LT7170 no-OS IIO support
-------------------------

The LT7170 IIO driver comes on top of the LT7170 driver and offers support
for interfacing IIO clients through libiio.

LT7170 IIO Device Configuration
--------------------------------

Input Channel Attributes
------------------------

VIN/IIN/TEMP/VCC channels are the input channels of the LT7170 IIO
device and each of them has a total of 2 channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly using a priv``

Output Channel Attributes
-------------------------

IOUT channel is an output channel with the following channel
attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly using a priv``

Meanwhile, VOUT channel is an output channel with a separate channel
attributes. Each channel has 11 attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly using a priv``
* ``enable - state of the channel``
* ``enable_available - list of available states for the channel``
* ``vout_command - VOUT_COMMAND value of the channel output``
* ``vout_max - VOUT_COMMAND value of the channel output``
* ``vout_margin_low - VOUT_MARGIN_LOW value of the channel output``
* ``vout_margin_high - VOUT_MARGIN_HIGH value of the channel output``
* ``phase - Phase offset value of the channel output``
* ``pulse_skipping - PWM pulse skip mode of the channel output``
* ``pulse_skipping_available - pulse skipping available states of the each channel``

Global Attributes
-----------------

The device has a total of 12 global attributes:

* ``freq_sync - Clock frequency sync enable of the device``
* ``freq_sync_available - Available state of the clock sync enable``
* ``frequency - Device switching frequency``
* ``vout_ov_fault_limit - Output overvoltage fault limit``
* ``vout_ov_warn_limit - Output overvoltage warning limit``
* ``vout_uv_fault_limit - Output undervoltage fault limit``
* ``vout_uv_warn_limit - Output undervoltage warning limit``
* ``iin_oc_warn_limit - Input overcurrent warning limit``
* ``iout_oc_warn_limit - Output overcurrent warning limit``
* ``ot_fault_limit - Overtemperature fault limit for both channels``
* ``ot_warn_limit - Overtemperature warning limit for both channels``
* ``vin_uv_warn_limit - Output undervoltage warning limit for both channels``

Debug Attributes
----------------

The device has a total of 7 debug attributes:

* ``status_vout - VOUT status byte value of channel 0``
* ``status_iout - IOUT status byte value of channel 0``
* ``status_input - INPUT status byte value of channel 0``
* ``status_mfr_specific - MFR_SPECIFIC status byte value of channel 0``
* ``status_word - Status word value of the channel 0``
* ``status_temperature - TEMPERATURE status byte value of the device``
* ``status_cml - CML status byte value of the device``

LT7170 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct lt7170_iio_desc *lt7170_iio_desc;
	struct lt7170_iio_desc_init_param lt7170_iio_ip = {
		.lt7170_init_param = &lt7170_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = lt7170_iio_init(&lt7170_iio_desc, &lt7170_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "lt7170",
			.dev = lt7170_iio_desc,
			.dev_descriptor = lt7170_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = lt7170_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
