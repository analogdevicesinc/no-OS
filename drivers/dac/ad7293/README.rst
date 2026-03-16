AD7293 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD7293 <https://www.analog.com/AD7293>`_

Overview
--------

The AD7293 is a 12-bit power amplifier current controller with an integrated
ADC, DACs, temperature sensors, and current sense amplifiers. It features four
uncommitted 12-bit ADC inputs with configurable input ranges (0 V to 1.25 V,
0 V to 2.5 V, or 0 V to 5 V), eight 12-bit voltage DAC outputs (4 bipolar and
4 unipolar) with 1.3 us maximum settling time, four high-side current sense
amplifiers with programmable gain, two external temperature sensor inputs, and
an internal temperature sensor. The device includes a 2.5 V on-chip reference
and communicates via SPI with VDRIVE supporting 1.8 V, 3 V, and 5 V logic
interfaces. The AD7293 is housed in a 56-lead LFCSP package and operates over
the -40 C to +125 C temperature range.

Applications
------------

* GaN and GaAs PA monitoring and controls
* Base station power amplifiers
* General-purpose system monitoring

AD7293 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``ad7293_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver configures the SPI interface, optionally initializes a reset GPIO,
performs a device reset (hardware reset via GPIO if available, otherwise a
software reset), and verifies the chip ID.

The ``spi_init`` and ``gpio_reset`` fields in the init parameters are passed
by pointer.

Device Reset
~~~~~~~~~~~~

The AD7293 can be reset using either hardware or software methods. The
``ad7293_reset`` API first attempts a hardware reset by toggling the reset GPIO
low and then high. If no reset GPIO is configured, it falls back to a software
reset by writing the reset value (0x7293) to the soft reset register followed
by clearing it. The ``ad7293_soft_reset`` API performs a software reset only.

ADC Input Range Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The four ADC voltage input channels (VIN0-VIN3) support configurable input
ranges. Use ``ad7293_adc_set_range`` and ``ad7293_adc_get_range`` to configure
and read the input range for a specific channel.

Current Sense Gain Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The four current sense amplifier channels (ISENSE0-ISENSE3) have programmable
gain settings. Use ``ad7293_isense_set_gain`` and ``ad7293_isense_get_gain``
to configure and read the gain for a specific channel.

Channel Offset Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Offset calibration is available for all channel types (ADC voltage inputs,
temperature sensors, current sense, and DAC outputs). Use
``ad7293_set_offset`` and ``ad7293_get_offset`` to write and read offset
values. The channel type is specified using the ``ad7293_ch_type`` enum:

* ``AD7293_ADC_VINX``: ADC voltage input channels
* ``AD7293_ADC_TSENSE``: Temperature sensor channels
* ``AD7293_ADC_ISENSE``: Current sense channels
* ``AD7293_DAC``: DAC output channels

Reading ADC Channels
~~~~~~~~~~~~~~~~~~~~

The ``ad7293_ch_read_raw`` API reads raw 12-bit conversion data from any
channel type. For ADC voltage inputs, it configures the channel sequencer and
issues a conversion command. For temperature and current sense channels, it
additionally enables the respective bandgap reference and waits 9 ms for
settling before triggering the conversion. For DAC channels, it reads the
output register directly.

The raw data is returned in bits [15:4] of the 16-bit register value.

Writing DAC Outputs
~~~~~~~~~~~~~~~~~~~

The ``ad7293_dac_write_raw`` API writes a raw 12-bit value to a DAC output
channel. It first enables the DAC output by setting the corresponding bit
in the DAC enable register, then writes the value to the unipolar VOUT
register.

AD7293 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct ad7293_dev *dev;
	int ret;
	uint16_t adc_raw;
	uint16_t dac_val;

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param gpio_reset = {
		.port = GPIO_RESET_PORT_NUM,
		.number = GPIO_RESET_PIN_NUM,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ad7293_init_param init_param = {
		.spi_init = &spi_init,
		.gpio_reset = &gpio_reset,
	};

	ret = ad7293_init(&dev, &init_param);
	if (ret)
		goto error;

	/* Read ADC voltage input channel 0 */
	ret = ad7293_ch_read_raw(dev, AD7293_ADC_VINX, 0, &adc_raw);
	if (ret)
		goto error_remove;

	/* Write midscale value to DAC channel 0 */
	dac_val = 0x800;
	ret = ad7293_dac_write_raw(dev, 0, dac_val);
	if (ret)
		goto error_remove;

error_remove:
	ad7293_remove(dev);
error:
	return ret;
