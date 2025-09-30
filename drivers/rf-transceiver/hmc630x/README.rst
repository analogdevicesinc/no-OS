HMC630X no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`HMC6300 <https://www.analog.com/HMC6300>`_
`HMC6301 <https://www.analog.com/HMC6301>`_

Overview
--------

The HMC6300 is a complete millimeterwave transmitter integrated circuit that
operates from 57 GHz to 64 GHz with up to 1.8 GHz modulation bandwidth. An
integrated synthesizer provides tuning in 250 MHz, 500 MHz, or 540 MHz steps
with excellent phase noise to support up to 64 QAM modulation. The transmitter
provides up to 15 dBm linear output power into a 100 ohm differential load.
Key features include digital and analog RF and IF gain control, an integrated
image reject filter, and support for an external local oscillator (LO).

The HMC6301 is the companion millimeterwave receiver that, together with the
HMC6300, forms a complete 60 GHz transceiver chipset for multiple Gbps operation
in the unlicensed 60 GHz ISM band. Both devices feature a 3-wire serial digital
interface for configuration and an on-chip temperature sensor.

Applications
------------

* Small cell backhaul
* 60 GHz ISM band data transfer
* Multiple Gbps data communication
* WiGig/802.11ad radio
* High definition video transmission
* Radar/high resolution imaging

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The HMC630X driver initializes the device with ``hmc630x_init``, using
parameters defined in ``hmc630x_init_param``. The initialization
configures the device type (HMC6300 or HMC6301), the reference clock,
GPIO signals for the 3-wire serial interface (EN, CLK, DATA, SCANOUT),
VCO frequency, and attenuation settings.

Configuration
~~~~~~~~~~~~~

Device configurations are set with functions like
``hmc630x_set_enable``, ``hmc630x_set_vco``, ``hmc630x_set_if_attn``,
and ``hmc630x_write_regmap``. These handle enabling the device, VCO
frequencies, and register map configurations. Additional
HMC6301-specific configurations are available through
``hmc6301_set_lna_gain``, ``hmc6301_set_bb_attn``,
``hmc6301_set_bb_attn_fine``, and ``hmc6301_set_bb_lpc_hpc``.

Reading
~~~~~~~

Data retrieval functions include ``hmc630x_read``, ``hmc630x_read_row``,
and ``hmc630x_read_regmap``, which access individual parameters, rows,
or the full register map. ``hmc630x_get_if_attn`` and
``hmc630x_get_vco`` provide access to intermediate frequency attenuation
settings and VCO frequencies respectively. For HMC6301-specific gain
measurements, use ``hmc6301_get_lna_gain``, ``hmc6301_get_bb_attn``,
``hmc6301_get_bb_attn_fine``, and ``hmc6301_get_bb_lpc_hpc``.

Writing
~~~~~~~

Writing operations utilize ``hmc630x_write``, ``hmc630x_write_row``, and
``hmc630x_write_regmap`` to alter specific parameters, rows, or the full
register map. RF gain settings can be adjusted with
``hmc6300_set_rf_attn``, and FM mode with ``hmc6300_set_fm_en``.

Temperature Control
~~~~~~~~~~~~~~~~~~~

Temperature management is facilitated by ``hmc630x_set_temp_en``,
``hmc630x_get_temp_en``, and ``hmc630x_get_temp``. These functions
enable and monitor the temperature reading capabilities of the device,
crucial for maintaining operational stability.

Driver Initialization Example
------------------------------

.. code-block:: c

	struct hmc630x_dev *dev;

	struct hmc630x_init_param txip = {
		.type = HMC6300,
		.ref_clk = HMC6300_REF_CLK_75MHz,
		.en = {
			.port = GPIO_EN_PORT,
			.number = GPIO_EN_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.clk = {
			.port = GPIO_CLK_PORT,
			.number = GPIO_CLK_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.data = {
			.port = GPIO_DATA_PORT,
			.number = GPIO_DATA_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.scanout = {
			.port = GPIO_SCANOUT_PORT,
			.number = GPIO_SCANOUT_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.vco = 0,
		.enabled = true,
		.temp_en = true,
		.if_attn = 0,
		.tx.rf_attn = 0,
	};

	ret = hmc630x_init(&dev, &txip);
	if (ret)
		goto error;

HMC630X no-OS IIO Support
--------------------------

The HMC630X IIO driver comes on top of the HMC630X driver and offers support
for interfacing IIO clients through libiio.

HMC630X IIO Device Configuration
---------------------------------

Attribute Handling
~~~~~~~~~~~~~~~~~~

The IIO driver provides register management and attribute handling for the
HMC6300 and HMC6301 devices. Attributes such as enabled state, VCO frequency,
VCO band, lock detect, IF attenuation, and temperature enable are accessible
through ``hmc630x_iio_read_attr`` and ``hmc630x_iio_write_attr``. For
HMC6300-specific attributes, RF attenuation is available. For HMC6301-specific
attributes, LNA gain, baseband attenuation, fine attenuation, and baseband
filter corner frequencies are supported.

Temperature and Frequency Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Temperature monitoring is exposed through the IIO interface via
``hmc630x_iio_read_raw``. For frequency management,
``hmc630x_iio_read_vco_available`` provides access to available VCO
frequencies, allowing for dynamic adjustments to optimize device operation.

HMC630X IIO Driver Initialization Example
------------------------------------------

.. code-block:: c

	int ret;

	struct hmc630x_iio_dev *hmc630x_iio_dev;
	struct hmc630x_iio_init_param hmc630x_iio_ip = {
		.ip = &txip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = hmc630x_iio_init(&hmc630x_iio_dev, &hmc630x_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "hmc6300",
			.dev = hmc630x_iio_dev,
			.dev_descriptor = hmc630x_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_hmc630x;

	return iio_app_run(app);
