MAX34451 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX34451 <https://www.analog.com/MAX34451>`_

`MAX34455 <https://www.analog.com/MAX34455>`_

`ADPM12160 <https://www.analog.com/ADPM12160>`_

`ADPM12200 <https://www.analog.com/ADPM12200>`_

`ADPM12250 <https://www.analog.com/ADPM12250>`_

Overview
--------

The MAX34451 is a 16-channel PMBus power-supply manager and monitor with integrated
sequencing and margining capabilities. It provides comprehensive monitoring and control
for multi-rail power systems, featuring voltage, current, and temperature monitoring
across 16 channels with programmable sequencing, margining, and fault management.

The MAX34455 is a 12-channel variant with similar functionality but reduced channel count.

The ADPM12160 is a high-performance quarter-brick DC-DC power conversion module that
delivers a fully regulated 12V/12.5V output with a continuous power level of 1600W and
a peak power capability of up to 2400W. It features a 40V to 60V input range, peak
efficiency up to 98% at half load, and comprehensive protections including
overtemperature, input undervoltage/overvoltage, input/output overcurrent, and
short-circuit detection. It uses PMBus for digital configuration and management.

The ADPM12200 is a 2000W variant with a peak power capability of up to 3000W, and
the ADPM12250 is a 2500W variant. Both share the same quarter-brick form factor and
PMBus interface as the ADPM12160.

Applications
------------

MAX34451
--------

* Multi-Rail Power System Management
* Data Center and Server Power Monitoring
* Telecommunications Equipment
* Storage Systems
* Industrial Control Systems
* System Optimization in Prototype and Production

ADPM12160
---------

* Distributed Power Architectures
* Wireless Networks and Base Stations
* Optical Network Equipment
* Enterprise Networks
* Latest-Generation ICs and Microprocessor-Powered Systems

MAX34451 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C) alongside other GPIO pins if needed in the
specific application (depends on the way the device is used).

The first API to be called is **max34451_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

The initialization API uses the device descriptor and an initialization
parameter. The initialization parameter contains the **chip_id** of the device
(ID_MAX34451, ID_MAX34451NA6, ID_MAX34455, ID_ADPM12160, ID_ADPM12200, or
ID_ADPM12250) and optional GPIO parameters for controlling POWER GOOD, RUN, 
FAULT and ALERT pins. These are defined in the header file of the driver.

Page Selection
--------------

The MAX34451 uses PMBus paging to access different channels. Each channel (0-20)
can be selected using the **max34451_set_page** API. Use MAX34451_CHAN_ALL (0xFF)
to address all channels simultaneously for global commands.

For the ADPM12160/ADPM12200/ADPM12250, page selection determines which subsystem
is being controlled or read. Each page maps to a specific function rather than an
independent power rail:

* **Page 0, 1, 15** - Internal use (do NOT modify)
* **Page 2** - Output voltage monitoring and control
* **Page 3** - PGOOD output delay control
* **Page 4** - Output current control
* **Page 5** - Phase 1 current monitoring and control
* **Page 6** - Phase 2 current monitoring and control
* **Page 7** - Phase 3 current monitoring and control (not used by ADPM12250)
* **Page 8** - Phase 4 current monitoring and control (not used by ADPM12250)
* **Page 9** - Input voltage monitoring and control
* **Page 10** - Input current monitoring and control
* **Page 14** - READ_IOUT (0x8C) only
* **Page 18** - PCB board temperature monitoring and control

Use ADPM12XXX_CHAN_ALL (0xFF) to address all channels simultaneously. PMBus
commands issued on a given page will read or configure the subsystem associated
with that page.

VOUT Configuration
------------------

The MAX34451 monitors voltage across 16 channels. Channels 0-11 support both
monitoring and margining capabilities, while channels 12-15 are monitor-only.

Output voltage margin high and low limits can be configured using the
**max34451_vout_margin** API. This allows for testing power supply response to
voltage variations.

The OPERATION command via the **max34451_set_operation** API can be used to command
the output voltage to operate at high margin, low margin, or nominal level. The
operation modes include:

* MAX34451_OPERATION_MARGIN_HIGH_ACT_FAULT - Margin to high voltage with active fault response
* MAX34451_OPERATION_MARGIN_LOW_ACT_FAULT - Margin to low voltage with active fault response
* MAX34451_OPERATION_OFF - Turn channel off
* MAX34451_OPERATION_SEQ_OFF - Sequenced turn-off

Status Monitoring
-----------------

Status bytes/words of the device can be read using **max34451_read_status**
API. The available status types include:

* MAX34451_STATUS_WORD_TYPE - 16-bit status word with comprehensive fault indicators
* MAX34451_STATUS_VOUT_TYPE - Voltage-related status (OV/UV faults and warnings)
* MAX34451_STATUS_IOUT_TYPE - Current-related status (overcurrent conditions)
* MAX34451_STATUS_TEMP_TYPE - Temperature status (overtemperature warnings/faults)
* MAX34451_STATUS_CML_TYPE - Communication and memory fault status
* MAX34451_STATUS_MFR_SPECIFIC_TYPE - Manufacturer-specific status bits
* MAX34451NA6_STATUS_INPUT_TYPE - Input voltage/current status (for MAX34451NA6 and later)

Faults can be cleared using the **max34451_clear_faults** API.

Telemetry
---------

Measurements for each channel can be read using the **max34451_read_value** API.
Supported telemetry values include:

* MAX34451_VAL_VOUT - Output voltage reading
* MAX34451_VAL_IOUT - Output current reading
* MAX34451_VAL_TEMP - Temperature reading
* MAX34451_VAL_VOUT_PEAK - Peak voltage measurement
* MAX34451_VAL_VOUT_MIN - Minimum voltage measurement
* MAX34451_VAL_IOUT_PEAK - Peak current measurement
* MAX34451_VAL_TEMP_PEAK - Peak temperature measurement
* MAX34451_VAL_IOUT_AVG - Average current measurement

For MAX34451NA6 and later revisions, input monitoring is also available:

* MAX34451NA6_VAL_VIN - Input voltage reading
* MAX34451NA6_VAL_IIN - Input current reading

Timing Configuration
--------------------

The TON_DELAY command sets the time from when a start condition is received
until the output voltage starts to rise. The TOFF_DELAY command sets the time
from when a stop condition is received until the output voltage starts to fall.

These timing parameters can be configured using the **max34451_set_timing** API
with the following timing types:

* MAX34451_TON_DELAY_TYPE - Turn-on delay in milliseconds
* MAX34451_TOFF_DELAY_TYPE - Turn-off delay in milliseconds
* MAX34451_RETRY_DELAY - Fault retry delay in milliseconds

The TON_MAX_FAULT_LIMIT can also be set to trigger a fault if turn-on exceeds
a specified time.

Fault Management
----------------

The MAX34451 provides comprehensive fault detection and management:

* Overvoltage and undervoltage fault/warning limits
* Overcurrent fault/warning limits
* Overtemperature fault/warning limits
* Programmable fault responses and retry mechanisms
* Nonvolatile fault logging
* System-level ALERT pin for fault indication

Fault thresholds can be configured using the write_word_data API with appropriate
PMBus commands (e.g., MAX34451_VOUT_OV_FAULT_LIMIT, MAX34451_IOUT_OC_FAULT_LIMIT).

Sequencing and Margining
-------------------------

Channels 0-11 support power supply sequencing with programmable turn-on and
turn-off delays. The sequencing enables controlled power-up and power-down of
multiple rails with dependency management.

Channels 0-7 use integrated PWM outputs for voltage margining, while channels
8-11 interface with DS4424 DACs for analog voltage margining control.

Bit-Level Access
----------------

For advanced configuration, the driver provides bit-level register access:

* **max34451_update_bit** - Modify specific bits in a register using a mask
* **max34451_read_bit** - Read specific bit fields from a register

These APIs allow fine-grained control over device configuration without
affecting other bits in the same register.

Software Reset
--------------

Software reset operation is available through **max34451_software_reset** API.
This resets the device to its power-on state without requiring a hardware reset.

PMBus Commands
--------------

The driver supports standard PMBus commands and manufacturer-specific commands:

* Page selection and management
* Read/write byte operations
* Read/write word operations
* Read/write block data operations
* Status monitoring
* Telemetry reading
* Configuration and control

All PMBus operations automatically handle page selection for paged commands.

MAX34451 Driver Initialization Example
--------------------------------------

To select which device variant is being used, set the **.id** field in the
initialization parameter to the appropriate chip ID:

* **ID_MAX34451** - MAX34451ETNA4 and earlier revisions
* **ID_MAX34451NA6** - MAX34451NA6+ and NA6+T revisions
* **ID_MAX34455** - MAX34455
* **ID_ADPM12160** - ADPM12160
* **ID_ADPM12200** - ADPM12200
* **ID_ADPM12250** - ADPM12250

.. code-block:: bash

	struct max34451_dev *max34451_dev;
	struct no_os_i2c_init_param max34451_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = 0x74,
		.extra = I2C_EXTRA,
	};

	struct max34451_init_param max34451_ip = {
		.i2c_init = &max34451_i2c_ip,
		.id = ID_MAX34451,  /* Change to ID_MAX34451NA6 or ID_MAX34455 as needed */
		.alert_param = &alert_gpio_param,
		.pgood_param = NULL,
		.run_param = NULL,
		.fault_param = NULL,
	};

	ret = max34451_init(&max34451_dev, &max34451_ip);
	if (ret)
		goto error;

MAX34451 no-OS IIO support
--------------------------

The MAX34451 IIO driver comes on top of the MAX34451 driver and offers support
for interfacing IIO clients through libiio.

IIO driver source files:

* :git-no-OS:`drivers/power/max34451/iio_max34451.c`
* :git-no-OS:`drivers/power/max34451/iio_max34451.h`

MAX34451 IIO Device Configuration
---------------------------------

Input Channel Attributes
------------------------

VOUT12-15/IOUT0-11/VIN/IIN/TEMP channels are the input channels of the
MAX34451 IIO device and each of them has a total of two channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel calculated accordingly to each specific channel``

Output Channel Attributes
-------------------------

VOUT0-11 channels are the output channels of the MAX34451 IIO device and each
of them has a total of five channel attributes:

* ``raw - the raw value of the channel``
* ``scale - the scale value of the channel``
* ``enable - the enable status of the channel``
* ``enable_available - available enable options (Disabled, Enabled)``
* ``vout_margin_high - the high margin voltage of the channel``
* ``vout_margin_low - the low margin voltage of the channel``

Global Attributes
-----------------

The MAX34451 IIO device has the following global attributes for fault/warning
limit configuration and timing:

* ``vout_ov_fault_limit_N - Overvoltage fault limit for channel N (0-15)``
* ``vout_uv_fault_limit_N - Undervoltage fault limit for channel N (0-15)``
* ``iout_oc_fault_limit_N - Overcurrent fault limit for channel N (0-11)``
* ``ot_fault_limit_N - Overtemperature fault limit for sensor N (0-4)``
* ``ot_warn_limit_N - Overtemperature warning limit for sensor N (0-4)``
* ``power_good_on_N - Power good on threshold for channel N (0-15)``
* ``power_good_off_N - Power good off threshold for channel N (0-15)``
* ``ton_delay_N - Turn-on delay for channel N (0-11)``
* ``toff_delay_N - Turn-off delay for channel N (0-11)``
* ``fault_retry - Global fault retry configuration``

Debug Attributes
----------------

The MAX34451 IIO device has the following debug attributes for status monitoring:

* ``status_vout_N - VOUT status byte for channel N (0-15)``
* ``status_iout_N - IOUT status byte for channel N (0-11)``
* ``status_temperature_N - TEMPERATURE status byte for sensor N (0-4)``
* ``status_mfr_specific_N - Manufacturer-specific status for channel N (0-15)``
* ``status_word_N - Status word for channel N (0-15)``
* ``status_cml - CML status byte of the device``

MAX34451 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct max34451_iio_desc *max34451_iio_desc;
	struct max34451_iio_desc_init_param max34451_iio_ip = {
		.max34451_init_param = &max34451_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = max34451_iio_init(&max34451_iio_desc, &max34451_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "max34451",
			.dev = max34451_iio_desc,
			.dev_descriptor = max34451_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = max34451_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
