AD5460 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD5460 <https://www.analog.com/AD5460>`_

Overview
--------

The AD5460 is a quad-channel, software configurable analog output device. Each
channel features a 16-bit monotonic DAC that can be independently configured for
voltage or current output. The current output ranges are 0 mA to 25 mA, 0 mA to
20 mA, and 4 mA to 20 mA. The voltage output ranges are 0 V to 12 V and
+/-12 V. Adaptive power switching drivers reduce power dissipation by 40% in
current output mode. The device includes on-chip diagnostics for open-circuit
and short-circuit detection, an internal 10 ppm/C reference, and overvoltage
tolerance up to +/-50 V. Communication is through SPI with CRC error checking.
The AD5460 operates over the -40C to +105C temperature range and is available in
a 48-lead 7 mm x 7 mm LFCSP package.

Applications
------------

* Factory automation
* Actuator control
* Group isolated analog outputs
* PLC and DCS applications
* HART network connectivity

AD5460 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``ad5460_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver sets up SPI communication, configures the device address, populates
the CRC lookup table (polynomial 0x7), and optionally initializes the reset
GPIO. A device reset is then performed (hardware reset via GPIO if available,
otherwise software reset via the CMD_KEY register). Finally, a scratch register
test is performed by writing 0x1234 and reading it back to verify communication.

The ``spi_ip`` field in the init parameters is passed by value (not by pointer).

Channel Function Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each channel can be independently configured for a specific operation mode using
``ad5460_set_channel_function``. The available modes are:

* ``AD5460_HIGH_Z``: High impedance (output disabled)
* ``AD5460_VOLTAGE_OUT``: Voltage output mode
* ``AD5460_CURRENT_OUT``: Current output mode
* ``AD5460_CURRENT_OUT_HART``: Current output mode with HART support

When changing modes, the driver first transitions the channel through the
``AD5460_HIGH_Z`` state with a 200 us delay before applying the new mode.

Output Range Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

In voltage output mode, the output range is configured using
``ad5460_set_channel_vout_range``:

* ``AD5460_VOUT_RANGE_0_12V``: 0 V to 12 V
* ``AD5460_VOUT_RANGE_NEG12_12V``: -12 V to +12 V

In current output mode, the output range is configured using
``ad5460_set_channel_iout_range``:

* ``AD5460_IOUT_RANGE_0_25MA``: 0 mA to 25 mA
* ``AD5460_IOUT_RANGE_0_20MA``: 0 mA to 20 mA
* ``AD5460_IOUT_RANGE_4_20MA``: 4 mA to 20 mA

DAC Code Setting
~~~~~~~~~~~~~~~~~

DAC output values can be set using ``ad5460_set_channel_dac_code`` with a 16-bit
code. Helper functions ``ad5460_dac_voltage_to_code`` and
``ad5460_dac_current_to_code`` are available to convert millivolt or microamp
values to the corresponding DAC code for a given channel configuration.

Slew Rate Control
~~~~~~~~~~~~~~~~~~

Slew rate control can be enabled per channel using ``ad5460_dac_slew_enable``,
which configures both the linear step size and the update rate. The step sizes
are 0.8%, 1.5%, 6.1%, and 22.2% of full scale. The update rates are 4.8 kHz,
76.8 kHz, 153.6 kHz, and 230.4 kHz. Use ``ad5460_dac_slew_disable`` to disable
slew rate control on a channel.

GPIO Configuration
~~~~~~~~~~~~~~~~~~~

Each channel has an associated GPIO pin that can be configured using
``ad5460_set_gpio_config``:

* ``AD5460_GPIO_SEL_HIGH_Z``: High impedance
* ``AD5460_GPIO_SEL_GPIO``: General-purpose output
* ``AD5460_GPIO_SEL_GPI``: General-purpose input

Use ``ad5460_gpio_set`` and ``ad5460_gpio_get`` to write and read GPIO values.

Diagnostics
~~~~~~~~~~~~

The device includes a diagnostic multiplexer that can route various internal
signals to the diagnostic output. Use ``ad5460_set_diag`` to select the
diagnostic source per channel. The ``ad5460_get_live`` function reads the live
status register, which reports open-circuit and short-circuit conditions for each
channel, supply status, and temperature alerts.

Thermal Reset Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The thermal reset threshold can be configured using ``ad5460_set_therm_rst``.
Setting the parameter to false selects a 110C threshold, while true selects
a 140C threshold.

AD5460 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct ad5460_desc *ad5460_dev;
	int ret;
	uint16_t dac_code;

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param reset_gpio_init = {
		.number = RESET_GPIO_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ad5460_init_param ad5460_ip = {
		.dev_addr = 0,
		.spi_ip = spi_init,
		.reset_gpio_param = &reset_gpio_init,
	};

	ret = ad5460_init(&ad5460_dev, &ad5460_ip);
	if (ret)
		goto error;

	/* Configure channel A for voltage output, 0V to 12V range */
	ret = ad5460_set_channel_function(ad5460_dev, AD5460_CH_A,
					  AD5460_VOLTAGE_OUT);
	if (ret)
		goto error_remove;

	ret = ad5460_set_channel_vout_range(ad5460_dev, AD5460_CH_A,
					    AD5460_VOUT_RANGE_0_12V);
	if (ret)
		goto error_remove;

	/* Convert 6000 mV to DAC code and set the output */
	ret = ad5460_dac_voltage_to_code(ad5460_dev, 6000, &dac_code,
					 AD5460_CH_A);
	if (ret)
		goto error_remove;

	ret = ad5460_set_channel_dac_code(ad5460_dev, AD5460_CH_A, dac_code);
	if (ret)
		goto error_remove;

error_remove:
	ad5460_remove(ad5460_dev);
error:
	return ret;
