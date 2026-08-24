AD719X no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`AD7190`
- :adi:`AD7192`
- :adi:`AD7193`
- :adi:`AD7194`
- :adi:`AD7195`

Overview
--------

The AD719X family are low noise, complete analog front ends for high precision
measurement applications. Each device contains a low noise, 24-bit sigma-delta
ADC preceded by a low noise gain stage, so that signals of small amplitude can
be interfaced directly to the ADC. The on-chip channel sequencer allows several
analog input channels to be enabled, and the device automatically sequences
through the enabled channels.

The output data rate from the part is software-programmable and can be varied
from 4.7 Hz to 4.8 kHz. A flexible digital filter is provided, offering both a
fast settling option and a zero-latency option. The devices can be configured
for buffered or unbuffered, bipolar or unipolar analog inputs and offer a
programmable gain stage supporting gains of 1, 8, 16, 32, 64, and 128.

The AD719X can operate with an internal 4.92 MHz clock or with an external clock
or crystal. On-chip features include a bridge power-down switch, an internal
temperature sensor, and programmable general-purpose digital outputs. The parts
communicate through a serial interface (SPI) compatible with standard microcontroller
and DSP interface protocols.

Applications
------------

* Weigh scales
* Strain gauge transducers
* Pressure measurement
* Temperature measurement
* Chromatography
* Industrial process control
* Portable instrumentation

AD719X Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as a GPIO used to monitor the MISO/RDY
line. An optional SYNC GPIO can be provided when synchronizing multiple devices.

The first API to be called is **ad719x_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. The ``chip_id`` field in
the init parameters selects the target part (``AD7190``, ``AD7192``, ``AD7193``,
``AD7194`` or ``AD7195``). During initialization, the driver initializes the SPI
and GPIOs, performs a software reset, verifies the device ID against ``chip_id``,
and then applies the initial range/gain, output data rate, buffer, clock source
and operating mode from the init parameters. The input mode is applied only on
the AD7193 and AD7194, and the bridge power-down switch is configured on every
part except the AD7194.

Note that **ad719x_init** takes the init parameters structure by value, while
``spi_init``, ``gpio_miso`` and ``sync_pin`` are supplied as pointers inside it.
The ``sync_pin`` is optional (it may be ``NULL``) and is only needed when
synchronizing multiple devices.

Register Access
~~~~~~~~~~~~~~~

Low-level register access is available through **ad719x_set_register_value**,
**ad719x_get_register_value** and **ad719x_set_masked_register_value**, which
read and write the 8-, 24- or 32-bit device registers (Communications, Status,
Mode, Configuration, Data, ID, GPOCON, Offset and Full-Scale). Use
**ad719x_reset** to return the device to its default state.

Operating Modes and Synchronization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use **ad719x_set_operating_mode** to place the device into one of its operating
modes: continuous conversion, single conversion, idle, power-down, or one of the
zero-scale/full-scale internal and system calibration modes (see
``enum ad719x_adc_modes``). **ad719x_wait_rdy_go_low** polls the MISO/RDY line
and blocks until a conversion result is ready (or an internal timeout expires).

Channel and Input Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ad719x_channels_select** enables one or more analog input channels using a
channel mask (see the ``AD719X_CH_*`` defines and ``AD719X_CH_MASK``). The number
of selectable channels depends on the part, so the accepted mask width differs
(up to 10 bits on the AD7193, 9 bits on the AD7194, 8 bits on the others); an
out-of-range mask returns ``-EINVAL``.

**ad719x_config_input_mode** selects between differential and pseudo-differential
inputs; it is only supported on the AD7193 and AD7194 and returns ``-ENOTSUP`` on
the other parts. **ad719x_buffer_select** enables or disables the input buffer.
**ad719x_range_setup** selects the conversion polarity (0 = bipolar,
1 = unipolar) together with the ADC input range (gain of 1, 8, 16, 32, 64 or 128,
see ``enum ad719x_adc_gain``).

Filter and Clock Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ad719x_output_rate_select** sets the filter update (output data) rate code
(1 to 1023). **ad719x_clock_select** selects the clock source: external crystal,
external clock on MCLK2, or the internal 4.92 MHz clock (see
``enum ad719x_adc_clock``).

Calibration
~~~~~~~~~~~

**ad719x_calibrate** performs the requested calibration (internal/system,
zero-scale/full-scale) on the specified channel.

Measurement and Conversion
~~~~~~~~~~~~~~~~~~~~~~~~~~

**ad719x_single_conversion** returns the result of a single conversion, while
**ad719x_continuous_read_avg** averages a configurable number of samples.
**ad719x_temperature_read** reads the internal temperature sensor and returns
the value in degrees Celsius. **ad719x_convert_to_millivolts** converts a 24-bit
raw conversion result to millivolts for a given reference voltage, accounting for
the currently configured polarity and gain.

Bridge Power-Down Switch
~~~~~~~~~~~~~~~~~~~~~~~~

**ad719x_set_bridge_switch** opens (0) or closes (1) the bridge power-down switch
(GPOCON BPDSW), which can be used to disconnect a bridge sensor between
conversions to save power. It is not available on the AD7194 and returns
``-ENOTSUP`` on that part.

AD719X Driver Initialization Example
------------------------------------

.. code-block:: c

	struct ad719x_dev *dev;
	int ret;
	uint32_t raw_data;
	float millivolts;

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_3,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct no_os_gpio_init_param gpio_miso = {
		.number = GPIO_MISO_NUMBER,
		.port = GPIO_MISO_PORT,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct ad719x_init_param init_param = {
		.spi_init = &spi_init,
		.gpio_miso = &gpio_miso,
		.sync_pin = NULL,
		.chip_id = AD7193,
		.current_polarity = 0,
		.current_gain = AD719X_ADC_GAIN_1,
		.operating_mode = AD719X_MODE_SINGLE,
		.data_rate_code = 480,
		.clock_source = AD719X_INT_CLK_4_92_MHZ,
		.input_mode = 0,
		.buffer = 1,
		.bpdsw_mode = 0,
	};

	ret = ad719x_init(&dev, init_param);
	if (ret)
		goto error;

	/* Configure range and read a single conversion on channel 0 */
	ret = ad719x_range_setup(dev, 0, AD719X_ADC_GAIN_1);
	if (ret)
		goto error_remove;

	ret = ad719x_channels_select(dev, AD719X_CH_MASK(AD719X_CH_0));
	if (ret)
		goto error_remove;

	ret = ad719x_single_conversion(dev, &raw_data);
	if (ret)
		goto error_remove;

	millivolts = ad719x_convert_to_millivolts(dev, raw_data, 2.5);

	error_remove:
		ad719x_remove(dev);
	error:
		return ret;
