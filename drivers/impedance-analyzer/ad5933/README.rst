AD5933 no-OS driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`AD5933`

Overview
--------

The AD5933 is a high precision impedance converter system that combines an
on-board frequency generator with a 12-bit, 1 MSPS analog-to-digital converter
(ADC). The frequency generator excites an unknown impedance with a programmable
output frequency; the response signal is sampled by the ADC and processed by an
on-chip discrete Fourier transform (DFT) engine that returns a real and an
imaginary data word for every measured frequency point.

Communication is over an I2C interface (up to 400 kHz) at the default slave
address ``0x0D``. The excitation frequency is swept from a programmable start
frequency in a programmable increment, over up to 511 increments (512 points
total). The output excitation range and an input-stage PGA gain (x1 / x5) are
software selectable, and a configurable settling-time count lets the excitation
settle before each DFT measurement. From the real/imaginary DFT output, the host
computes magnitude, phase, and — after calibration against a known resistor —
impedance. An on-chip temperature sensor is also available.

Applications
------------

* Electrochemical analysis
* Bioelectrical impedance analysis
* Complex impedance measurement
* Corrosion monitoring and protection equipment
* Biomedical and automotive sensors
* Proximity sensing
* Nondestructive testing and material property analysis

AD5933 Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C).

The first API to be called is **ad5933_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. It allocates the device
descriptor, brings up the I2C peripheral described by the init parameters, and
waits for the part to acknowledge on the bus after power-up (on a cold start the
device NACKs its address until its supply rail is valid, so the first register
access is retried). It then calls **ad5933_setup**, which programs the system
clock source, the output range, the PGA gain, and the settling time from the
values in the descriptor. **ad5933_remove** releases every resource allocated by
**ad5933_init**.

Register Access
~~~~~~~~~~~~~~~

Direct register access is available through **ad5933_reg_write** and
**ad5933_reg_read** for single-byte registers, and through
**ad5933_set_register_value** / **ad5933_get_register_value** for the
multi-byte, MSB-first frequency and increment registers. **ad5933_reset** issues
a device reset (programmed sweep parameters are preserved), and
**ad5933_wait_status** polls the status register until a requested status bit
(temperature valid, data valid, or sweep done) is set or a timeout elapses.

Clock, Range and Gain Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The system clock source (internal 16.776 MHz oscillator or an external clock)
is selected with **ad5933_set_system_clk**. The excitation output range and the
input PGA gain are configured together with **ad5933_set_range_and_gain**, or
individually with **ad5933_set_range** and **ad5933_set_gain**.
**ad5933_set_settling_time** programs the number of output-settling cycles that
elapse before each DFT measurement; the x1 / x2 / x4 multiplier is inferred
automatically from the requested count and the value is clamped to the maximum
of 2044 cycles.

Frequency Sweep Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ad5933_config_sweep** programs the sweep: the start frequency, the frequency
increment, and the number of increments (``num_increments``); the sweep
therefore covers ``num_increments + 1`` points, up to a maximum of 512. It
validates that the increment count does not exceed 511 and that the start and
end (``start_freq + inc_freq * num_increments``) frequencies stay within the
clock-derived output range — from ``MCLK / 16384`` up to ``MCLK / 32`` — and it
resets the internal sweep-point counter. **ad5933_convert_freq_to_reg** converts
a frequency in Hz to its 27-bit register code using the current system clock.

The sweep is driven one datasheet function at a time:
**ad5933_initialize_sweep** runs the standby → reset → init-with-start-frequency
sequence, **ad5933_init_start_freq** outputs the start frequency indefinitely
without measuring, **ad5933_start_sweep** begins the sweep (the ADC measures
after the settling cycles elapse), **ad5933_increment_freq** advances to the
next point, **ad5933_repeat_freq** re-measures the current point, and
**ad5933_sweep_done** reports whether the sweep has completed.
**ad5933_standby** and **ad5933_power_down** place the part in standby or
power-down mode.

Measurement and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ad5933_get_data** issues a frequency function, waits for data-valid, and reads
the real and imaginary DFT words; **ad5933_get_current_data** reads the
real/imaginary words of the current point without issuing a function (the caller
must have already waited for data-valid). **ad5933_get_magnitude** derives the
DFT magnitude sqrt(real^2 + imag^2), and **ad5933_get_phase** derives the phase
in radians (subtracting the system phase captured at calibration). Calibration
against a known reference impedance is performed with
**ad5933_calculate_gain_factor**; once the gain factor is known,
**ad5933_calculate_impedance** returns the impedance magnitude \|Z\| in ohms.
Both take a ``measurement_mode`` argument (``AD5933_MEAS_MODE_2W`` for the
default 2-wire I-measure path, or ``AD5933_MEAS_MODE_4W`` for the 4-wire
V-measure path), which selects how the raw DFT magnitude is mapped to impedance.
The
on-chip temperature sensor is read with **ad5933_get_raw_temperature** (raw
14-bit two's complement code) or **ad5933_get_temperature** (degrees Celsius).

AD5933 Driver Initialization Example
------------------------------------

.. code-block:: c

	struct ad5933_dev *dev;

	struct ad5933_init_param ad5933_ip = {
		.i2c_init = {
			.device_id = I2C_DEVICE_ID,
			.max_speed_hz = I2C_MAX_SPEED,
			.slave_address = AD5933_ADDRESS,
			.platform_ops = I2C_OPS,
			.extra = &i2c_extra_ip,
		},
			.clock_freq = EXTERNAL_CLK_SPEED,
			.clock_source = AD5933_CLOCK_SOURCE_EXTERNAL,
			.pga_gain = AD5933_GAIN_X1,
			.output_range = AD5933_RANGE_2000mVpp,
			.settling_cycles = AD5933_SETTLING_CYCLES,
	};

	ret = ad5933_init(&dev, &ad5933_ip);
	if (ret)
		goto error;
	/* ad5933_init() runs ad5933_setup() internally. */

AD5933 no-OS IIO support
------------------------

The AD5933 IIO driver comes on top of the AD5933 driver and offers support for
interfacing IIO clients through libiio.

The driver exposes scannable ``real`` and ``imaginary`` voltage channels, a
temperature channel, and an ``altvoltage0`` output channel that drives the
excitation DDS and carries the sweep configuration.
A buffered capture triggers a full frequency sweep: the driver initializes and
starts the sweep, walks every point while collecting the real/imaginary words
into an internal staging buffer, and then pushes the collected samples into the
IIO buffer. Individual points can also be driven manually through the debug
attributes.

AD5933 IIO Device Configuration
-------------------------------

Channel Attributes
~~~~~~~~~~~~~~~~~~

All attributes are organized per channel; the driver exposes no device-global
attributes.

The scannable ``voltage0`` (``real``) input channel exposes:

* ``raw`` - the real signed 16-bit DFT word of the current sweep point.
* ``scale`` - the input PGA gain, reported as "0.2" (x5) or "1" (x1).
* ``scale_available`` - the available PGA gain values.

The scannable ``voltage1`` (``imag``) input channel exposes:

* ``raw`` - the imaginary signed 16-bit DFT word of the current sweep point.

The temperature channel exposes:

* ``raw`` - the raw 14-bit two's-complement temperature code from the on-chip sensor.
* ``scale`` - the scale (1/32 °C per LSB, i.e. 0.031250) applied to raw to obtain degrees Celsius.

The ``altvoltage0`` output channel drives the excitation DDS and carries the
sweep configuration:

* ``raw`` - the excitation output voltage range option, see the table below for possible options.
* ``scale_available`` - the available excitation output ranges.
* ``frequency_start`` - the sweep start frequency in Hz.
* ``frequency_increment`` - the sweep frequency increment in Hz.
* ``frequency_points`` - the number of frequency increments (num_increments); points = frequency_points + 1.
* ``settling_cycles`` - the number of output settling cycles before each measurement.

The four output excitation ranges set both the peak-to-peak excitation voltage
amplitude and its DC bias level. The values below are for a 3.3 V supply:
+-------+-------------------------------------+----------------------+
| Range | Output Excitation Voltage Amplitude | Output DC Bias Level |
+=======+=====================================+======================+
| 1     | 1.98 V p-p                          | 1.48 V               |
+-------+-------------------------------------+----------------------+
| 2     | 0.97 V p-p                          | 0.76 V               |
+-------+-------------------------------------+----------------------+
| 3     | 383 mV p-p                          | 0.31 V               |
+-------+-------------------------------------+----------------------+
| 4     | 198 mV p-p                          | 0.173 V              |
+-------+-------------------------------------+----------------------+

Debug Attributes
~~~~~~~~~~~~~~~~

* ``sweep_initialized`` - write a non-zero value to run the initialize-sweep sequence; reads 1 when the device is in the init-start-frequency state.
* ``sweep_started`` - write a non-zero value to start the sweep; reads 1 when the device is in the start-sweep state.
* ``current_output_frequency`` - the current sweep output frequency in Hz (read-only).
* ``repeat_measurement`` - write a non-zero value to re-measure the current point and wait for data-valid (requires the sweep to be started).
* ``incremented_measurement`` - write a non-zero value to advance one point and wait for data-valid (requires the sweep to be started).
* ``clock_frequency`` - the active system clock frequency in Hz (read-only).
* ``minimum_output_frequency`` - the lowest valid excitation frequency for the active clock, ``clock / 16384`` Hz (read-only).
* ``maximum_output_frequency`` - the highest valid excitation frequency for the active clock, ``clock / 32`` Hz (read-only).

Buffered Capture
~~~~~~~~~~~~~~~~

The ``real`` and ``imaginary`` channels are scannable (signed 16-bit). On buffer
enable the driver initializes and starts the sweep; the buffer ``submit``
callback then walks all ``num_increments + 1`` points, waiting for each to
become valid, reading its real/imaginary words, and incrementing the frequency.
The collected points are pushed into the ring in ascending ``scan_index`` order
— real then imaginary — so the buffer layout is interleaved ``real, imag`` per
point. Requested samples beyond the number of points collected are padded with
``INT16_MAX``. On buffer disable the device is placed in power-down mode.

Since a sweep measurement can take a considerable amount of time, the client is
responsible in setting an appropriate timeout to await a valid read.

AD5933 IIO Driver Initialization Example
----------------------------------------

.. code-block:: c

	int ret;

	struct ad5933_iio_dev *ad5933_iio_dev;
	struct ad5933_iio_dev_init_param ad5933_iio_ip = {
		.ad5933_dev_ip = &ad5933_ip,
		.start_freq = 30000,
		.freq_increment = 1000,
		.freq_points = 10,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = ad5933_iio_init(&ad5933_iio_dev, &ad5933_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad5933",
			.dev = ad5933_iio_dev,
			.dev_descriptor = ad5933_iio_dev->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_ad5933;

	return iio_app_run(app);
