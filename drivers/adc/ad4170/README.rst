AD4170 no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD4170-4 <https://www.analog.com/AD4170-4>`_
* `AD4171-8 <https://www.analog.com/AD4171-8>`_
* `AD4190-4 <https://www.analog.com/AD4190-4>`_
* `AD4195-4 <https://www.analog.com/AD4195-4>`_

Overview
--------

The AD4170-4 is a low noise, completely integrated analog front end built
around a 24-bit sigma-delta ADC. A crosspoint multiplexer presents four
differential or eight single-ended/pseudo-differential analog inputs to an
ultra-low noise PGA with gains of 0.5 to 128, so low amplitude transducers can
be wired directly to the device. Output data rates range from 7.6 SPS to
500 kSPS. Integrated blocks include a band gap reference with 5 ppm/degC
typical drift brought out on REFOUT at 2.5 V, an internal oscillator, a
temperature sensor, four matched programmable excitation current sources
(10 uA to 1.5 mA), low-side power switches, a VBIAS generator, a 12-bit DAC,
four GPIOs, and AC excitation support.

A 16 entry sequencer converts each enabled channel in a predetermined order.
Every sequencer channel picks its analog input pair from the crosspoint mux and
one of eight user-defined setups; a setup carries the PGA gain, reference
selection and buffering, bipolar/unipolar span, filter type, filter word,
chopping options, burnout current, and the offset and gain calibration
registers. That lets transducer readings, system checks, and diagnostic
measurements be interleaved without reprogramming the device between
conversions.

The driver covers the following family members:

* **AD4170-4** - 4 differential/8 pseudo-differential inputs, 7.6 SPS to
  500 kSPS, sinc5+avg, sinc5, sinc3, post filters and a user programmable FIR
  filter, 12-bit DAC, AC excitation. 32-lead 4 mm x 6 mm LFCSP.
* **AD4171-8** - 8 differential/16 pseudo-differential inputs, 7.6 SPS to
  500 kSPS, sinc5+avg, sinc5, sinc3 and post filters only, 12-bit DAC, AC
  excitation. 40-lead 6 mm x 6 mm LFCSP.
* **AD4190-4** and **AD4195-4** - 4 differential/8 pseudo-differential inputs,
  3.8 SPS to 62.5 kSPS, sinc5+avg, sinc5, sinc3 and post filters only, no DAC
  and no AC excitation. The two parts differ in accuracy grade only. 32-lead
  4 mm x 6 mm LFCSP.

All of them share the same register map and are driven through a common set of
APIs; the active part is selected with the ``id`` field of
``struct ad4170_init_param``.

Applications
------------

* Industrial process control: PLC/DCS modules
* Temperature and pressure measurement
* Medical and scientific instrumentation
* Chromatography
* Electrical test and measurement
* Instrumentation
* Weigh scale

AD4170 Device Configuration
---------------------------

Driver Initialization
^^^^^^^^^^^^^^^^^^^^^

The device is controlled over a 3- or 4-wire SPI interface, so a
``no_os_spi_init_param`` has to be provided. Up to three optional GPIOs may be
described as well: ``gpio_sync_inb`` for the SYNC_IN pin, ``gpio_dig_aux1`` for
DIG_AUX1 and ``gpio_dig_aux2`` for DIG_AUX2. DIG_AUX1 is normally wired as the
data ready output, while DIG_AUX2 is driven by the host when it is configured
as the LDAC or START input.

**ad4170_init** is the first API to be called. It resets the device, applies
the requested SPI settings, verifies the product ID against the selected
``id``, checks that the power-on reset flag is set, runs a scratchpad
read/write test, and then writes the whole ``struct ad4170_config`` out:
pin muxing, clock control, standby and power-down switch control, error
enables, ADC control, all eight setups, the channels listed in ``channel_en``
together with the channel enable mask itself, reference control, VBIAS,
pull-up currents, the four excitation current sources, the FIR control block
and the DAC configuration. Make sure it returns 0.

``ad4170_remove`` frees the descriptor and the GPIOs taken during init.

``ad4170_config_reset`` is exported as the configuration the device holds after
a hardware or software reset, and is a convenient starting point for an
application configuration.

Because ``ad4170_init`` programs all ``AD4170_NUM_SETUPS`` setup banks, every
setup has to be initialized, even the unused ones. A zeroed setup would be
written out with an invalid filter word of 0.

SPI Interface Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^

``struct ad4170_spi_settings`` selects the flavour of the serial interface:

* ``short_instruction`` - use the 7-bit addressing instruction instead of the
  default 15-bit one.
* ``crc_enabled`` - enable CRC generation and checking on the communication.
* ``sync_loss_detect`` - enable sync pattern loss detection and recovery.

``ad4170_reset_spi_interface`` puts the serial interface back in a known state,
and ``ad4170_reset`` performs a full software reset of the device.

Register Access
^^^^^^^^^^^^^^^

``ad4170_spi_reg_read``, ``ad4170_spi_reg_write`` and
``ad4170_spi_reg_write_mask`` give raw access to the register map. Register
addresses are the ``AD4170_REG_*`` macros, which encode the transfer length in
their upper bits, so the same API handles the 1 to 4 byte wide registers.
``ad4170_regmap`` dumps the register map to the console for debugging.

Channel and Setup Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The sequencer holds ``AD4170_NUM_CHANNELS`` (16) channels and
``AD4170_NUM_SETUPS`` (8) setups.

* ``ad4170_set_channel_map`` selects the positive and negative multiplexer
  inputs of a sequencer channel. Beside AIN0 to AIN16 and AINCOM, the mux also
  reaches the internal temperature sensor, the supply monitors, the DAC output,
  the LDO outputs, AVSS, DGND and the reference pins.
* ``ad4170_set_channel_setup`` assigns one of the eight setups to a channel,
  plus the number of times the channel is repeated and the settling delay added
  after the channel switch.
* ``ad4170_set_setup`` writes a full setup: the misc register (excitation
  current chopping, ADC/mux chopping, burnout current), the AFE register
  (reference buffers, reference selection, bipolar/unipolar span, PGA gain),
  the filter register (filter type and post filter), the filter word, and the
  offset and gain calibration values.
* ``ad4170_set_channel_en`` enables the channels the sequencer converts.

Reference selection is per setup and picks REFIN1, REFIN2, the 2.5 V REFOUT or
AVDD/AVSS. When REFOUT is used, the internal reference has to be turned on
separately with ``ad4170_set_ref_control``.

ADC Operating Mode
^^^^^^^^^^^^^^^^^^

``ad4170_set_adc_ctrl`` selects the operating mode along with parallel filter
operation, the single versus per-channel data register selection, and the
status byte appended in continuous read/transmit. The available modes are
continuous conversion, continuous conversion with the FIR or the IIR filter,
single conversion, standby, power-down, idle, and the four calibration modes
(self and system, offset and gain).

The AD4171-8 has no FIR or IIR filter. ``ad4170_set_adc_ctrl`` returns
``-ENOTSUP`` when ``AD4170_MODE_CONT_FIR`` or ``AD4170_MODE_CONT_IIR`` is
requested on it, and so does ``ad4170_set_fir_control``. ``ad4170_init``
tolerates the latter, so an unused ``fir_control`` in the configuration does
not make initialization fail on that part.

Reading Conversions
^^^^^^^^^^^^^^^^^^^

Conversion results can be fetched at several widths, with or without the status
byte: ``ad4170_get_data16``, ``ad4170_get_data16s``, ``ad4170_get_data24``,
``ad4170_get_data24s`` and ``ad4170_get_data32``. When
``multi_data_reg_sel`` is set, ``ad4170_get_ch_data`` reads the per-channel
data register of a given sequencer channel instead of the shared one.

The ``ad4170_read16``, ``ad4170_read16s``, ``ad4170_read24``,
``ad4170_read24s`` and ``ad4170_read32`` variants read a burst of samples in
one go. ``ad4170_continuous_read`` streams samples in continuous read mode, and
``ad4170_continuous_read_exit`` leaves that mode. Continuous transmit over the
TDM interface is left with ``ad4170_continuous_transmit_exit``.

``ad4170_get_status`` returns the data status register, whose RDY bit tells
whether a new conversion is available. ``rdy_conv_timeout`` in the init
parameters bounds how long the driver polls for end of conversion.

Clock Configuration
^^^^^^^^^^^^^^^^^^^

``ad4170_set_clocksel`` picks the clock source: the internal oscillator, the
internal oscillator with the clock output on XTAL2/CLKIO, an external clock on
XTAL2/CLKIO, or an external crystal across XTAL1 and XTAL2/CLKIO.
``ad4170_set_mclk_div`` divides the master clock by 1, 2, 4 or 8, and
``ad4170_set_dclk_div`` divides the continuous transmit data clock.

Excitation, Bias and Power Switches
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``ad4170_set_current_source`` routes one of the four excitation current
sources to an analog input, AINCOM or a GPIO, and sets its value to one of
0 uA, 10 uA, 50 uA, 100 uA, 250 uA, 500 uA, 1 mA or 1.5 mA. Currents can be
summed by outputting more than one on the same pin.

``ad4170_set_v_bias`` applies the VBIAS common-mode voltage to a mask of
channels and ``ad4170_set_i_pullup`` enables the pull-up currents.
``ad4170_set_powerdown_sw`` drives the two low-side power switches, which are
typically used to power down a bridge sensor between conversions, and
``ad4170_set_standby_ctrl`` selects which blocks stay alive in standby.

DAC
^^^

The AD4170-4 and the AD4171-8 embed a 12-bit DAC whose output spans 0 V to
REFOUT or 0 V to 2 x REFOUT. ``ad4170_set_dac_config`` enables it and selects
the span together with the hardware toggle and hardware LDAC modes.
``ad4170_set_dac_data`` writes the DAC register directly, while
``ad4170_set_dac_inputa`` and ``ad4170_set_dac_inputb`` load the two toggle
registers. The output is then updated with ``ad4170_dac_sw_ldac``,
``ad4170_dac_sw_toggle`` or ``ad4170_dac_hw_toggle``, the last one driving the
DIG_AUX2 pin.

FIR Filter
^^^^^^^^^^

On the parts that have it, ``ad4170_set_fir_control`` selects the FIR mode -
the default filter or a programmable one with odd/even symmetric, odd/even
anti-symmetric or asymmetric coefficients - the coefficient set, and the number
of coefficients, up to ``AD4170_FIR_COEFF_MAX_LENGTH`` (72). When a
programmable mode is selected, the coefficients pointed to by
``fir_coefficients`` are written to the coefficient memory.

Diagnostics
^^^^^^^^^^^

``ad4170_set_error_en`` selects which diagnostics raise an error: the ALDO and
DLDO power supply monitors, the excitation current compliance checks, the
reference and analog input overvoltage/undervoltage checks, the conversion
error, and the memory map and ROM CRC checks. ``ad4170_get_error`` reads the
error register and ``ad4170_set_error`` clears its sticky bits.

Pin Muxing
^^^^^^^^^^

``ad4170_set_pin_muxing`` configures DIG_AUX1 (disabled, data ready, SYNC_OUT
or modulator data output), DIG_AUX2 (disabled, DAC LDAC, START input or
modulator data output), the SYNC_IN pin, whether the active channel number is
output on the GPIO pins, the digital output drive strength, and whether the
interface resets on the last SCLK or on the CS edge.

AD4170 Driver Initialization Example
------------------------------------

.. code-block:: C

	#define AD4170_SETUP_DEFAULT {					\
		.afe = {						\
			.ref_select = AD4170_REFIN_REFOUT,		\
			.bipolar = true,				\
			.pga_gain = AD4170_PGA_GAIN_1,			\
		},							\
		.filter = {						\
			.filter_type = AD4170_FILT_SINC5_AVG,		\
			.post_filter_sel = AD4170_POST_FILTER_NONE,	\
		},							\
		.filter_fs = 4,						\
		.gain = 0x555555,					\
	}

	struct ad4170_dev *ad4170_desc;
	struct ad4170_init_param ad4170_ip = {
		.id = ID_AD4170,
		.spi_init = {
			.device_id = SPI_DEVICE_ID,
			.max_speed_hz = 10000000,
			.chip_select = GPIO_CS_PIN,
			.mode = NO_OS_SPI_MODE_3,
			.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
			.platform_ops = &spi_platform_ops,
			.extra = SPI_EXTRA,
		},
		.rdy_conv_timeout = 10000000,
		.config = {
			.clock_ctrl = {
				.clocksel = AD4170_INTERNAL_OSC,
				.clockdiv = AD4170_CLKDIVBY1,
			},
			.adc_ctrl = {
				.mode = AD4170_MODE_CONT,
				.multi_data_reg_sel = false,
				.cont_read = AD4170_CONT_READ_OFF,
			},
			.channel_en = AD4170_CHANNEL(0),
			.map = {
				[0] = {
					.ainp = AD4170_AIN0,
					.ainm = AD4170_AIN1,
				},
			},
			/* Every setup is written out, so none may be left zeroed. */
			.setups = {
				[0] = AD4170_SETUP_DEFAULT,
				[1] = AD4170_SETUP_DEFAULT,
				[2] = AD4170_SETUP_DEFAULT,
				[3] = AD4170_SETUP_DEFAULT,
				[4] = AD4170_SETUP_DEFAULT,
				[5] = AD4170_SETUP_DEFAULT,
				[6] = AD4170_SETUP_DEFAULT,
				[7] = AD4170_SETUP_DEFAULT,
			},
			.ref_control = {
				.ref_en = true,
			},
			.fir_control = {
				.fir_length = 1,
			},
		},
	};

	ret = ad4170_init(&ad4170_desc, &ad4170_ip);
	if (ret)
		goto error;

	ret = ad4170_get_data24(ad4170_desc, &code);
	if (ret)
		goto error_dev;

AD4170 no-OS IIO support
------------------------

The AD4170 IIO driver sits on top of the AD4170 driver and exposes the device
to IIO clients through the IIO library.

AD4170 IIO Device Configuration
-------------------------------

Device Channels
^^^^^^^^^^^^^^^

The IIO driver describes one ``IIO_VOLTAGE`` channel named ``ch0`` to ``ch15``
per sequencer channel, regardless of what ``channel_en`` holds, so the channels
the ADC converts can be picked at buffer enable time or at raw read time. A
channel is indexed by its sequencer channel number rather than by the
multiplexer inputs it converts, because several sequencer channels may convert
the same input pair through different setups and the IIO channel ids would then
collide. Samples are 24 bits stored in 32, little endian, signed when the setup
the channel uses is bipolar and unsigned when it is unipolar.

``ad4170_iio_init`` programs the sequencer channels that ``ad4170_init`` left
alone, so that every channel can be enabled later on. Every entry of ``map``
therefore has to be initialized, even for channels the application does not
intend to use: a zeroed map converts AIN0 against itself and reads as a near
zero code. Use ``AD4170_OPEN`` for both inputs of an unused channel.

Channel Attributes
^^^^^^^^^^^^^^^^^^

Each channel carries three attributes:

* ``raw`` - a single conversion result read from the channel. The driver
  enables only that channel, switches the device to single conversion mode,
  waits for the result and then restores the previous mode and channel mask.
* ``scale`` - the reference voltage in mV over 2 to the power of the effective
  bits, derived from the reference the setup selects, the bipolar/unipolar span
  and the PGA gain.
* ``sampling_frequency`` - readable and writable. The rate is
  ``fCLK / (32 x FILTER_FS)``. Writing it converts the requested rate into a
  filter word and clamps it to what the selected filter accepts: 1 to 256 for
  plain sinc5, 4 to 65532 in steps of 4 for the others. FILTER_FS lives in the
  setup, so the new rate applies to every channel mapped to the same setup;
  give a channel its own setup to rate it independently.

Buffered Reads
^^^^^^^^^^^^^^

On buffer enable the IIO driver programs the channel mask the buffer requests,
clears ``multi_data_reg_sel`` and switches the device to continuous conversion
mode, saving the previous mode, mask and data register selection. Samples are
then read from the per-channel data registers as conversions become ready, and
the saved configuration is restored when the buffer is disabled. The scan index
of a channel matches its bit in the Channel_En register.

Debug Register Access
^^^^^^^^^^^^^^^^^^^^^

The IIO driver exposes byte-wise debug access to the whole register map. The
byte address given by the client is mapped back to the register that holds it
and to the byte position inside it, so multi-byte registers can be read and
written one byte at a time.

Reference and Clock
^^^^^^^^^^^^^^^^^^^

``vref_mv`` in ``struct ad4170_iio_dev_init_param`` is the external reference
voltage in mV and is ignored for setups that select REFOUT, for which the
internal 2.5 V is used. ``mclk_hz`` is the frequency of the clock wired to
XTAL1/XTAL2; it is ignored when the internal oscillator is selected, for which
16 MHz is assumed, and has to be between 1 MHz and 17 MHz otherwise.

AD4170 IIO Driver Initialization Example
----------------------------------------

.. code-block:: C

	#define DATA_BUFFER_SIZE 400

	static uint8_t iio_data_buffer[DATA_BUFFER_SIZE * AD4170_NUM_CHANNELS *
				       sizeof(uint32_t)];

	struct ad4170_iio_dev *ad4170_iio_desc;
	struct ad4170_iio_dev_init_param ad4170_iio_ip = {
		.ad4170_init_param = &ad4170_ip,
		.vref_mv = 2500,
	};

	struct iio_data_buffer read_buff = {
		.buff = (void *)iio_data_buffer,
		.size = sizeof(iio_data_buffer),
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = ad4170_iio_init(&ad4170_iio_desc, &ad4170_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4170",
			.dev = ad4170_iio_desc,
			.dev_descriptor = ad4170_iio_desc->iio_dev,
			.read_buff = &read_buff,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = ad4170_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto iio_ad4170_remove;

	return iio_app_run(app);
