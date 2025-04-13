ADP5055 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADP5055 <https://www.analog.com/ADP5055>`_

Overview
--------

The ADP5055 combines three high performance buck regulators enabling direct
connection to high input voltages up to 18V with no preregulators.
This controller integrates a high precision 8-bit digital-to-analog 
converter (DAC) to enable the output voltage dynamic voltage scaling (DVS)
via the PMBus®-compatible, 2-wire interface.
The PMBus interface provides other flexible configurations, such as 
start-up and shutdown sequence control, individual forced pulse-width
modulation or power saving mode (FPWM or PSM) selection, an output discharge
switch, and a power-good signal.

Applications
------------

ADP5055
-------

* Small cell base stations
* Field programmable gate array (FPGA) and processor applications 
* Security and surveillance
* Medical applications 

ADP5055 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **adp5055_init**. Make sure that it return 0,
which means that the driver was initialized correctly.

VIDx Configuration
------------------

The actual output voltage of each of the three output channels is externally set by a
resistive divider to the FBx pins. The output voltage reference (VIDx) can be set
through the registers which commands the pulsed-width modulation (PWM)
outputs. The actualy output voltage is therefore set using the VIDx voltage scaled
down by a fixed value based on the resistor divider. The VIDx voltage is set using the
8-bit register code with the following equation: VIDx = 408 mV + (1.5 mV × code). The API
used for setting the raw 8-bit VIDx code is **adp5055_set_vid_code**

The upper and lower limits of the output voltage reference (VIDx), which is relative to
the default value 600mV, is configurable through the VIDx_HIGH and VIDx_LOW values. The
actual upper reference voltage limit is set using the 4-bit code with the following
equation: VIDx_HIGH = 600mV + 192 mV − (12 mV × code). The actual lower reference voltage
limit is set using the 4-bit code with the following equation:
VIDx_LOW = 600mV − 190.5 mV + (12 mV × code). The API used for setting these two values
is **adp5055_set_vidx_lim**. If a VIDx_HIGH setting is written that is lower than the
existing VIDx code, or a VIDx_LOW setting is written that is higher that the existing
VIDx code, it is ignored.

DVS and FPWM/PSM Configuration
------------------------------

The ADP5055 provides a DVS function to each channel. The step inteval during the DVS
transition can be set using the **adp5055_set_dvs_interval** API. The execution of
the transition can be set using **adp5055_set_dvs_auto** API. When set, the output
voltage of a channel only transitions with a write to the VIDx_GO bit using
**adp5055_start_vout**.

To achieve higher efficiency at light loads, the buck regulators in the ADP5055
smoothly transition to variable frequency PSM operation when the output load falls
below a current threshold. This can be enabled using the **adp5055_set_auto_pwm_psm**
API.

During the DVS transition, the ADP5055 generates a blanking period where the
regulators are forced into FPWM mode, regardless of the PSM setting. Hiccup
protection is also masked if set using the **adp5055_set_ocp_blanking** API

Startup Configuration
---------------------

The ADP5055 has a configurable enable and disable delay per channel based on a factor
of the externally set timer (tSET) which can either be 2.6ms or 20.8ms. The enable and
delay time is set using the **adp5055_set_enable_disable_delay**.

For large load setp conditions, the ADP5055 includes a fast transient response. When
the output voltage falls below the specific threshold, the internal loop gain
gradually increases to improve the load transient response speed. The fast transient
sensitivity is configured using the **adp5055_set_fast_transient** API.

Active Output Discharge Configuration
-------------------------------------

Each output channel has a discharge switch from the switching node to ground. This helps
to discharge the output capacitor quickly and can be enabled using
**adp5055_set_output_discharge** API.

Status Configuration
--------------------

Statuses bytes/words of the device can be read using **adp5055_read_status** API.
Additionally, the latched status can be read using **adp5055_read_latched_status** API.


ADP5055 Driver Initialization Example
-------------------------------------

.. code-block:: bash

	struct adp5055_desc *adp5055_desc;
	struct no_os_i2c_init_param adp5055_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.platform_ops = I2C_OPS,
		.slave_address = ADP5055_PMBUS_23KOHM_ADDRESS,
		.extra = I2C_EXTRA,
	};
	struct adp5055_init_param adp5055_ip = {
		.i2c_param = &adp5055_i2c_ip
	};
	ret = adp5055_init(&adp5055_desc, &adp5055_ip);
	if (ret)
		goto exit;


ADP5055 no-OS IIO support
-------------------------

The ADP5055 IIO driver comes on top of the ADP5055 driver and offers support
for interfacing IIO clients through libiio.

ADP5055 IIO Device Configuration
--------------------------------

Output Channel Attributes
-------------------------

OUTA/OUTB/SR1/SR2 channels are thee output channels of the ADP1050 IIO device
and each of them has a total of 7 channel attributes:

* ``enable - state of the channel``
* ``enable_available - list of available states for the channel``
* ``auto_pwm_psm - channel auto pwm/psm mode setting``
* ``auto_pwm_psm_available - list of available auto pwm/psm mode setting``
* ``output_discharge - channel output discharge setting``
* ``output_discharge_available - list of available output_discharge setting``
* ``enable_delay - enable delay of the channel``
* ``enable_delay_available - list of available enable delay values``
* ``disable_delay - disable delay setting of the channel``
* ``disable_delay_available - list of available disable delay values``
* ``vid_raw - raw channel output reference voltage``
* ``vid_go - enable channel output voltage transition``
* ``vid_go_available -  list of available vid_go setting``
* ``dvs_interval - dvs interval of the channel``
* ``dvs_interval_available - list of available dvs_interval values``
* ``vid_high_limit - raw vidx high limit of the channel``
* ``vid_low_limit - raw vidx low limit of the channel``
* ``fast_transient - fast transient setting of the channel``
* ``fast_transient_available - list of available fast transient settings``
* ``pg_hw_enable - hardware power-good (pg) pin masking for the channel``
* ``pg_hw_enable_available - list of available hardware pg pin enable settings``
* ``pg - power-good status of the channel``

Global Attributes
-----------------

The device has a total of 13 global attributes:

* ``enable_mode - enable mode of all channels of the device``
* ``enable_mode_available - list of available enable modes of the device``
* ``dvs_auto_enable - enable setting for automatic dvs``
* ``dvs_auto_enable_available - list of available enable setting for automatic dvs``
* ``ocp_blanking_enable - enable setting of ocp blanking during dvs``
* ``ocp_blanking_enable_available - list of available enable setting for ocp blanking``
* ``pg_delay_enable - enable setting for power-good delay``
* ``pg_delay_enable_available - list of available enable setting for power-good delay``

Debug Attributes
----------------

* ``status_cml - CML status byte value of the device``
* ``status_lch - 8-bit latch status value of the device``

ADP5055 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int ret;

	struct adp5055_iio_desc *adp5055_iio_desc;
	struct adp5055_iio_desc_init_param adp5055_iio_ip = {
		.adp5055_init_param = &adp5055_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp5055_iio_init(&adp5055_iio_desc, &adp5055_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp5055",
			.dev = adp5055_iio_desc,
			.dev_descriptor = adp5055_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp5055_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp5055;

	return iio_app_run(app);
