AD9361 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`AD9361`
- :adi:`AD9363`
- :adi:`AD9364`

Overview
--------

The AD9361 is a high performance, highly integrated radio frequency (RF) Agile Transceiver™ designed for use in 3G and 4G base station applications. Its programmability and wideband capability make it ideal for a broad range of transceiver applications.
The device combines a RF front end with a flexible mixed-signal baseband section and integrated frequency synthesizers, simplifying design-in by providing a configurable digital interface to a processor.
The AD9361 receiver LO operates from 70 MHz to 6.0 GHz and the transmitter LO operates from 47 MHz to 6.0 GHz range, covering most licensed and unlicensed bands. Channel bandwidths from less than 200 kHz to 56 MHz are supported.
The two independent direct conversion receivers have state-of-the-art noise figure and linearity. Each receive (RX) subsystem includes independent automatic gain control (AGC), dc offset correction, quadrature correction, and digital filtering, thereby eliminating the need for these functions in the digital baseband.
The AD9361 also has flexible manual gain modes that can be externally controlled. Two high dynamic range analog-to-digital converters (ADCs) per channel digitize the received I and Q signals and pass them through configurable decimation filters and 128-tap finite impulse response (FIR) filters to produce a 12-bit output signal at the appropriate sample rate.
The transmitters use a direct conversion architecture that achieves high modulation accuracy with ultralow noise. This transmitter design produces a best in class TX error vector magnitude (EVM) of <−40 dB, allowing significant system margin for the external power amplifier (PA) selection.
The on-board transmit (TX) power monitor can be used as a power detector, enabling highly accurate TX power measurements.
The fully integrated phase-locked loops (PLLs) provide low power fractional-N frequency synthesis for all receive and transmit channels. Channel isolation, demanded by frequency division duplex (FDD) systems, is integrated into the design. All VCO and loop filter components are integrated.
The core of the AD9361 can be powered directly from a 1.3 V regulator. The IC is controlled via a standard 4-wire serial port and four real-time input/output control pins. Comprehensive power-down modes are included to minimize power consumption during normal use. The AD9361 is packaged in a 10 mm × 10 mm, 144-ball chip scale package ball grid array (CSP_BGA).


Applications
------------

* Point to point communication systems
* Femtocell/picocell/microcell base stations
* General-purpose radio systems


AD9361 Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol. The AD9361 is controlled via a standard 4-wire
serial port and, optionally, a number of real-time control GPIOs (reset, sync
and calibration switches).

The whole configuration of the part is passed to the driver through a single
**AD9361_InitParam** structure. This structure mirrors the device-tree
properties used by the Linux driver (each field carries the corresponding
``adi,*`` property name as a comment) and is organized into the following groups:

* **Device selection** - ``dev_sel`` selects the part being driven
  (``ID_AD9361``, ``ID_AD9364`` or ``ID_AD9363A``).
* **Reference Clock** - ``reference_clk_rate``.
* **Base Configuration** - operating-mode flags such as
  ``two_rx_two_tx_mode_enable``, ``frequency_division_duplex_mode_enable``,
  fastlock delays and DC offset tracking settings.
* **ENSM Control** - ``ensm_enable_pin_pulse_mode_enable``,
  ``ensm_enable_txnrx_control_enable``.
* **LO Control** - ``rx_synthesizer_frequency_hz``,
  ``tx_synthesizer_frequency_hz``, ``tx_lo_powerdown_managed_enable``.
* **Rate & BW Control** - ``rx_path_clock_frequencies[6]``,
  ``tx_path_clock_frequencies[6]``, ``rf_rx_bandwidth_hz``,
  ``rf_tx_bandwidth_hz``.
* **RF Port Control** - ``rx_rf_port_input_select``,
  ``tx_rf_port_input_select``.
* **TX Attenuation Control** - ``tx_attenuation_mdB``,
  ``update_tx_gain_in_alert_enable``.
* **Reference Clock Control** - ``xo_disable_use_ext_refclk_enable``,
  ``dcxo_coarse_and_fine_tune[2]``, ``clk_output_mode_select``.
* **Gain Control** - manual (``mgc_*``), automatic (``agc_*``) and fast AGC
  (``fagc_*``) gain-control parameters.
* **RSSI Control** - ``rssi_delay``, ``rssi_duration``, ``rssi_restart_mode``,
  ``rssi_wait``.
* **Aux ADC / Aux DAC Control**, **Temperature Sensor Control**,
  **Control Out Setup** and **External LNA Control**.
* **Digital Interface Control** - port/data-bus swapping, LVDS/CMOS mode and
  the various clock/data delays.
* **GPO Control** and **Tx Monitor Control**.
* **GPIO definitions** - ``gpio_resetb``, ``gpio_sync``, ``gpio_cal_sw1`` and
  ``gpio_cal_sw2`` (each a ``struct no_os_gpio_init_param``).
* **SPI** - ``spi_param`` (a ``struct no_os_spi_init_param``).
* **External LO clocks** - optional ``ad9361_rfpll_ext_recalc_rate``,
  ``ad9361_rfpll_ext_round_rate`` and ``ad9361_rfpll_ext_set_rate`` callbacks
  used when the RF PLL is driven by an external synthesizer.

The first API to be called is **ad9361_init**, passing the address of an
``ad9361_rf_phy`` handle and the populated ``AD9361_InitParam`` structure. On
success the driver allocates and returns, through the first argument, an
``ad9361_rf_phy`` state structure that must be passed to every subsequent API
call. Make sure that **ad9361_init** returns 0, which means that the driver was
initialized correctly.

After a successful **ad9361_init**, the RX and TX FIR filters are typically
configured with **ad9361_set_rx_fir_config** and **ad9361_set_tx_fir_config**.
When fastlock pin-control operation is used, **ad9361_rx_fastlock_recall** (or
**ad9361_tx_fastlock_recall**) must be called for a profile before that profile
can be selected through the pins.

The resources allocated by **ad9361_init** are released with
**ad9361_remove**.

Device Global Settings
~~~~~~~~~~~~~~~~~~~~~~~

These functions control the operating state of the whole transceiver. The
Enable State Machine (ENSM) selects the active mode of the device (sleep,
alert, FDD/TDD, pin control, etc.).

* **ad9361_set_en_state_machine_mode** / **ad9361_get_en_state_machine_mode** -
  set/get the ENSM mode (``ENSM_MODE_TX``, ``ENSM_MODE_RX``,
  ``ENSM_MODE_ALERT``, ``ENSM_MODE_FDD``, ``ENSM_MODE_WAIT``,
  ``ENSM_MODE_SLEEP``, ``ENSM_MODE_PINCTRL``, ``ENSM_MODE_PINCTRL_FDD_INDEP``).
* **ad9361_set_no_ch_mode** - select the number of active channels
  (``MODE_1x1`` or ``MODE_2x2``).
* **ad9361_do_mcs** - perform multi-chip synchronization between a master and a
  slave device.
* **ad9361_get_temperature** - read the internal temperature sensor (result in
  degrees Celsius, scaled by 1000).

Receive Chain Settings
~~~~~~~~~~~~~~~~~~~~~~~

These functions configure the two receive paths. Channel selection uses
``RX1`` / ``RX2``.

Frequency, bandwidth and sampling:

* **ad9361_set_rx_lo_freq** / **ad9361_get_rx_lo_freq** - set/get the RX LO
  frequency, in Hz.
* **ad9361_set_rx_lo_int_ext** - switch the RX LO between internal and external
  sources (``INT_LO``, ``EXT_LO``).
* **ad9361_set_rx_rf_bandwidth** / **ad9361_get_rx_rf_bandwidth** - set/get the
  RX RF bandwidth, in Hz.
* **ad9361_set_rx_sampling_freq** / **ad9361_get_rx_sampling_freq** - set/get
  the RX sampling frequency, in Hz.

Gain and RSSI:

* **ad9361_set_rx_rf_gain** / **ad9361_get_rx_rf_gain** - set/get the RX RF gain
  for a channel, in dB.
* **ad9361_set_rx_gain_control_mode** / **ad9361_get_rx_gain_control_mode** -
  set/get the gain control mode (``RF_GAIN_MGC``, ``RF_GAIN_FASTATTACK_AGC``,
  ``RF_GAIN_SLOWATTACK_AGC``, ``RF_GAIN_HYBRID_AGC``).
* **ad9361_get_rx_rssi** - read the RSSI of a channel into a ``struct rf_rssi``.

Tracking and calibration:

* **ad9361_set_rx_rfdc_track_en_dis** / **ad9361_get_rx_rfdc_track_en_dis** -
  enable/disable RX RF DC offset tracking.
* **ad9361_set_rx_bbdc_track_en_dis** / **ad9361_get_rx_bbdc_track_en_dis** -
  enable/disable RX baseband DC offset tracking.
* **ad9361_set_rx_quad_track_en_dis** / **ad9361_get_rx_quad_track_en_dis** -
  enable/disable RX quadrature tracking.

FIR filter:

* **ad9361_set_rx_fir_config** / **ad9361_get_rx_fir_config** - set/get the RX
  FIR filter configuration through an ``AD9361_RXFIRConfig`` structure.
* **ad9361_set_rx_fir_en_dis** / **ad9361_get_rx_fir_en_dis** - enable/disable
  and read back the state of the RX FIR filter.

RF port and LO power:

* **ad9361_set_rx_rf_port_input** / **ad9361_get_rx_rf_port_input** - select the
  RX RF input port (``A_BALANCED``, ``B_BALANCED``, ``C_BALANCED``, ``A_N``,
  ``A_P``, ``B_N``, ``B_P``, ``C_N``, ``C_P``, ``TX_MON1``, ``TX_MON2``,
  ``TX_MON1_2``).
* **ad9361_rx_lo_powerdown** - power the RX LO up or down (``ON``, ``OFF``).
* **ad9361_get_rx_lo_power** - read the RX LO power state.

Fastlock:

* **ad9361_rx_fastlock_store** - store the current RX LO configuration into a
  fastlock profile (0 to 7).
* **ad9361_rx_fastlock_recall** - recall a previously stored RX fastlock
  profile. This must be called for a profile before it can be selected through
  the pins in fastlock pin-control mode.
* **ad9361_rx_fastlock_load** / **ad9361_rx_fastlock_save** - load a fastlock
  profile from, or save it to, a user buffer.

Transmit Chain Settings
~~~~~~~~~~~~~~~~~~~~~~~~

These functions configure the two transmit paths. Channel selection uses
``TX1`` / ``TX2``.

Frequency, bandwidth and sampling:

* **ad9361_set_tx_lo_freq** / **ad9361_get_tx_lo_freq** - set/get the TX LO
  frequency, in Hz.
* **ad9361_set_tx_lo_int_ext** - switch the TX LO between internal and external
  sources (``INT_LO``, ``EXT_LO``).
* **ad9361_set_tx_rf_bandwidth** / **ad9361_get_tx_rf_bandwidth** - set/get the
  TX RF bandwidth, in Hz.
* **ad9361_set_tx_sampling_freq** / **ad9361_get_tx_sampling_freq** - set/get
  the TX sampling frequency, in Hz.

Attenuation and RSSI:

* **ad9361_set_tx_attenuation** / **ad9361_get_tx_attenuation** - set/get the TX
  attenuation of a channel, in mdB.
* **ad9361_get_tx_rssi** - read the TX RSSI of a channel.

FIR filter and calibration:

* **ad9361_set_tx_fir_config** / **ad9361_get_tx_fir_config** - set/get the TX
  FIR filter configuration through an ``AD9361_TXFIRConfig`` structure.
* **ad9361_set_tx_fir_en_dis** / **ad9361_get_tx_fir_en_dis** - enable/disable
  and read back the state of the TX FIR filter.
* **ad9361_set_tx_auto_cal_en_dis** / **ad9361_get_tx_auto_cal_en_dis** -
  enable/disable the TX quadrature auto-calibration.

RF port and LO power:

* **ad9361_set_tx_rf_port_output** / **ad9361_get_tx_rf_port_output** - select
  the TX RF output port (``TXA``, ``TXB``).
* **ad9361_tx_lo_powerdown** - power the TX LO up or down (``ON``, ``OFF``).
* **ad9361_get_tx_lo_power** - read the TX LO power state.

Fastlock:

* **ad9361_tx_fastlock_store** - store the current TX LO configuration into a
  fastlock profile (0 to 7).
* **ad9361_tx_fastlock_recall** - recall a previously stored TX fastlock
  profile.
* **ad9361_tx_fastlock_load** / **ad9361_tx_fastlock_save** - load a fastlock
  profile from, or save it to, a user buffer.

TRX Settings and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions act on both the receive and transmit chains at once.

* **ad9361_set_trx_path_clks** / **ad9361_get_trx_path_clks** - set/get the RX
  and TX path clock frequencies.
* **ad9361_set_trx_fir_en_dis** - enable/disable both the RX and TX FIR filters.
* **ad9361_trx_load_enable_fir** - load the RX and TX FIR configurations and
  enable the filters in one step.
* **ad9361_set_trx_rate_gov** / **ad9361_get_trx_rate_gov** - set/get the rate
  governor, which selects the oversampling ratio (``HIGHEST_OSR``,
  ``NOMINAL_OSR``).
* **ad9361_do_calib** - run a manual calibration (``TX_QUAD_CAL``,
  ``RFDC_CAL``).
* **ad9361_do_dcxo_tune_coarse** / **ad9361_do_dcxo_tune_fine** - apply the
  coarse and fine DCXO tuning words.

Data Structures
~~~~~~~~~~~~~~~

**ad9361_get_rx_rssi** reports the RSSI of a channel through a ``struct
rf_rssi``:

.. code-block:: C

	struct rf_rssi {
		uint32_t ant;		/* Antenna number for which RSSI is reported */
		uint32_t symbol;	/* Runtime RSSI */
		uint32_t preamble;	/* Initial RSSI */
		int32_t multiplier;	/* Multiplier to convert reported RSSI */
		uint8_t duration;	/* Duration to be considered for measuring */
	};

**ad9361_set_rx_fir_config** / **ad9361_get_rx_fir_config** use an
``AD9361_RXFIRConfig`` structure:

.. code-block:: C

	typedef struct {
		uint32_t rx;			/* 1, 2, 3(both) */
		int32_t rx_gain;		/* -12, -6, 0, 6 */
		uint32_t rx_dec;		/* 1, 2, 4 */
		int16_t rx_coef[128];
		uint8_t rx_coef_size;
		uint32_t rx_path_clks[6];
		uint32_t rx_bandwidth;
	} AD9361_RXFIRConfig;

**ad9361_set_tx_fir_config** / **ad9361_get_tx_fir_config** use an
``AD9361_TXFIRConfig`` structure:

.. code-block:: C

	typedef struct {
		uint32_t tx;			/* 1, 2, 3(both) */
		int32_t tx_gain;		/* -6, 0 */
		uint32_t tx_int;		/* 1, 2, 4 */
		int16_t tx_coef[128];
		uint8_t tx_coef_size;
		uint32_t tx_path_clks[6];
		uint32_t tx_bandwidth;
	} AD9361_TXFIRConfig;

AD9361 Driver Initialization Example
------------------------------------

.. code-block:: C

	struct ad9361_rf_phy *ad9361_phy;

	AD9361_InitParam default_init_param = {
		/* Device selection */
		ID_AD9361,		// dev_sel
		/* Reference Clock */
		40000000UL,		// reference_clk_rate
		/* Base Configuration */
		1,			// two_rx_two_tx_mode_enable
		1,			// one_rx_one_tx_mode_use_rx_num
		1,			// one_rx_one_tx_mode_use_tx_num
		1,			// frequency_division_duplex_mode_enable
		/* ... remaining base, ENSM, gain and interface fields ... */
		/* LO Control */
		2400000000UL,		// rx_synthesizer_frequency_hz
		2400000000UL,		// tx_synthesizer_frequency_hz
		/* Rate & BW Control */
		{983040000, 245760000, 122880000, 61440000, 30720000, 30720000}, // rx_path_clock_frequencies[6]
		{983040000, 122880000, 122880000, 61440000, 30720000, 30720000}, // tx_path_clock_frequencies[6]
		18000000,		// rf_rx_bandwidth_hz
		18000000,		// rf_tx_bandwidth_hz
		/* TX Attenuation Control */
		10000,			// tx_attenuation_mdB
		/* ... */
	};

	/* Provide the SPI settings for the current platform. */
	default_init_param.spi_param.device_id = SPI_DEVICE_ID;
	default_init_param.spi_param.chip_select = SPI_CS;
	default_init_param.spi_param.max_speed_hz = 10000000;
	default_init_param.spi_param.mode = NO_OS_SPI_MODE_1;
	default_init_param.spi_param.platform_ops = SPI_OPS;
	default_init_param.spi_param.extra = SPI_EXTRA;

	/* Provide the control GPIOs (use -1 for the unused ones). */
	default_init_param.gpio_resetb.number = GPIO_RESET_PIN;
	default_init_param.gpio_sync.number = -1;
	default_init_param.gpio_cal_sw1.number = -1;
	default_init_param.gpio_cal_sw2.number = -1;

	ret = ad9361_init(&ad9361_phy, &default_init_param);
	if (ret)
		goto error;

	/* Load the RX and TX FIR filter configurations. */
	ad9361_set_tx_fir_config(ad9361_phy, tx_fir_config);
	ad9361_set_rx_fir_config(ad9361_phy, rx_fir_config);

AD9361 no-OS IIO support
------------------------

The AD9361 IIO driver comes on top of the AD9361 driver and offers support for
interfacing IIO clients through libiio. It exposes the transceiver as an
``ad9361-phy`` IIO device and forwards register reads and writes to the base
driver, so it is typically registered alongside the AXI ADC and AXI DAC IIO
devices that handle the data path.

AD9361 IIO Device Configuration
-------------------------------

Input Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~

The input voltage channels (``voltage0``, ``voltage1``, ``voltage2``) map to
the receive chains and expose the following attributes:

* ``hardwaregain - the RX RF gain of the channel, in dB``
* ``hardwaregain_available - the supported RX RF gain range, as [min step max]``
* ``rssi - the RX power level, in dB, compensated for the receive-path gain (not an absolute value); read-only``
* ``gain_control_mode - the active gain control mode ("manual", "fast_attack", "slow_attack" or "hybrid")``
* ``gain_control_mode_available - the supported gain control modes ("manual fast_attack slow_attack hybrid")``
* ``rf_port_select - the selected RX input port; the setting affects all ports simultaneously``
* ``rf_port_select_available - the available RX input ports (A_BALANCED, B_BALANCED, C_BALANCED, A_N, A_P, B_N, B_P, C_N, C_P, TX_MONITOR1, TX_MONITOR2, TX_MONITOR1_2)``
* ``rf_bandwidth - the analog RX filter 3dB corner frequency, in Hz``
* ``rf_bandwidth_available - the supported RX RF bandwidth range``
* ``sampling_frequency - the RX baseband sample rate, in Hz``
* ``sampling_frequency_available - the supported RX sampling frequency range``
* ``filter_fir_en - enable/disable the RX FIR filter``
* ``rf_dc_offset_tracking_en - enable/disable RX RF DC offset tracking``
* ``bb_dc_offset_tracking_en - enable/disable RX baseband DC offset tracking``
* ``quadrature_tracking_en - enable/disable RX quadrature tracking``

Output Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The output voltage channels (``voltage0`` to ``voltage3``) map to the transmit
chains and expose the following attributes:

* ``hardwaregain - the TX attenuation of the channel, expressed as a negative gain in dB (0 to -89.75 dB in 0.25 dB steps)``
* ``hardwaregain_available - the supported TX gain range, as [min step max]``
* ``rssi - the TX-monitor power level, in dB; read-only``
* ``rf_port_select - the selected TX output port; the setting affects all ports simultaneously``
* ``rf_port_select_available - the available TX output ports (A, B)``
* ``rf_bandwidth - the analog TX filter 3dB corner frequency, in Hz``
* ``rf_bandwidth_available - the supported TX RF bandwidth range``
* ``sampling_frequency - the TX baseband sample rate, in Hz``
* ``sampling_frequency_available - the supported TX sampling frequency range``
* ``filter_fir_en - enable/disable the TX FIR filter``

Alternate Voltage (LO) Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The alternate voltage output channels (``altvoltage0`` for the RX LO,
``altvoltage1`` for the TX LO) control the local oscillators:

* ``frequency - the LO tuning frequency, in Hz (RX 70 MHz to 6 GHz, TX 47 MHz to 6 GHz, with fine tuning granularity)``
* ``frequency_available - the supported LO frequency range, as [min step max]``
* ``external - use an external LO source (1) instead of the internal synthesizer (0)``
* ``powerdown - power the LO down (1) or up (0); powering down the TX LO causes the TX quadrature calibration to fail``
* ``fastlock_store - store the current LO configuration into a fastlock profile (write the profile number, 0 to 7)``
* ``fastlock_recall - recall a stored fastlock profile (write the profile number); reads back the current profile number``
* ``fastlock_load - load a fastlock profile from 16 comma-separated program values (profile number included in the data)``
* ``fastlock_save - save a fastlock profile; reads back the profile as space-separated values``

Temperature Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The temperature input channel (``temp0``) exposes:

* ``input - the die temperature reading, in millidegrees Celsius; read-only``

Output Channel (FIR) Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``out`` channel exposes the global FIR-filter enable:

* ``voltage_filter_fir_en - simultaneously enable/disable both the RX and TX FIR filters``

Global Attributes
~~~~~~~~~~~~~~~~~

The following device-wide attributes are available:

* ``ensm_mode - the current Enable State Machine state (either SPI- or pin-controlled); write to change it``
* ``ensm_mode_available - the ENSM modes valid for the current configuration ("sleep wait alert fdd pinctrl pinctrl_fdd_indep" in FDD, "sleep wait alert rx tx pinctrl" in TDD)``
* ``calib_mode - the TX auto-calibration mode ("auto"/"manual"); writing "tx_quad" (optionally with a phase argument), "rf_dc_offs" or "rssi_gain_step" runs that calibration``
* ``calib_mode_available - the accepted calib_mode values ("auto manual tx_quad rf_dc_offs rssi_gain_step")``
* ``rx_path_rates - the current RX digital-block rates (BBPLL, ADC, R2, R1, RF, RXSAMP); read-only``
* ``tx_path_rates - the current TX digital-block rates (BBPLL, DAC, T2, T1, TF, TXSAMP); read-only``
* ``trx_rate_governor - the active rate governor, which influences the ADC sample rate and decimation/interpolation ("nominal" or "highest_osr")``
* ``trx_rate_governor_available - the supported rate governors ("nominal highest_osr"); read-only``
* ``dcxo_tune_coarse - the coarse DCXO capacitor tuning word (internal oscillator only)``
* ``dcxo_tune_coarse_available - the coarse DCXO tuning range, as [min step max]``
* ``dcxo_tune_fine - the fine DCXO capacitor tuning word (internal oscillator only)``
* ``dcxo_tune_fine_available - the fine DCXO tuning range, as [min step max]``
* ``xo_correction - crystal oscillator correction; not implemented in the no-OS driver (reads back 0)``
* ``xo_correction_available - crystal oscillator correction range; not implemented in the no-OS driver (reads back 0)``
* ``gain_table_config - custom gain table configuration; not implemented in the no-OS driver (reads back 0)``
* ``multichip_sync - write to trigger multi-chip synchronization (reads back 0)``
* ``rssi_gain_step_error - RSSI gain-step calibration error tables; not implemented in the no-OS driver (reads back 0)``
* ``filter_fir_config - the current RX/TX FIR setup, reported as "FIR Rx: <taps>,<decimation> Tx: <taps>,<interpolation>"; write to load a new FIR filter``

Debug Attributes
~~~~~~~~~~~~~~~~

The IIO device does not register any debug attributes, but it provides direct
register access through the ``debug_reg_read`` and ``debug_reg_write`` hooks,
which are wired to **ad9361_reg_read** and **ad9361_reg_write**.

AD9361 IIO Driver Initialization Example
----------------------------------------

.. code-block:: C

	int ret;

	struct iio_ad9361_desc *iio_ad9361_desc;
	struct iio_device *ad9361_dev_desc;
	struct iio_ad9361_init_param iio_ad9361_ip = {
		.ad9361_phy = ad9361_phy,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = iio_ad9361_init(&iio_ad9361_desc, &iio_ad9361_ip);
	if (ret)
		goto exit;

	iio_ad9361_get_dev_descriptor(iio_ad9361_desc, &ad9361_dev_desc);

	struct iio_app_device iio_devices[] = {
		IIO_APP_DEVICE("ad9361-phy", ad9361_phy, ad9361_dev_desc,
			       NULL, NULL, NULL),
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ad9361;

	return iio_app_run(app);
