AD7799 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD7798 <https://www.analog.com/AD7798>`_
* `AD7799 <https://www.analog.com/AD7799>`_

Overview
--------

The AD7798/AD7799 are low power, low noise, complete analog front ends for high
precision measurement applications. They contain a low noise, 16-bit (AD7798)
or 24-bit (AD7799) sigma-delta ADC with three differential analog inputs. The
on-chip, low noise instrumentation amplifier means that signals of small
amplitude can be interfaced directly to the ADC.

With a gain setting of 64, the rms noise is 27 nV for the AD7799 and 40 nV for
the AD7798 when the update rate equals 4.17 Hz. The output data rate is
software-programmable and can be varied from 4.17 Hz to 470 Hz. The AD7798
consumes a current of 300 uA typical, whereas the AD7799 consumes 380 uA
typical.

On-chip features include a low-side power switch, reference detect,
programmable digital output pins, burnout currents, and an internal clock
oscillator. The part operates with a power supply from 2.7 V to 5.25 V. Both
devices are housed in a 16-lead TSSOP package.

Applications
------------

* Weigh scales
* Pressure measurement
* Strain gauge transducers
* Gas analysis
* Industrial process control
* Portable instrumentation
* Smart transmitters

AD7799 Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``ad7799_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. The initialization
parameters include the chip type (``ID_AD7798`` or ``ID_AD7799``), which
determines the register sizes (16-bit vs 24-bit data register). During
initialization, the driver performs a software reset, verifies the chip ID,
and configures the gain and polarity settings.

The ``spi_init`` field in the init parameters is passed by value (not by
pointer).

Channel Selection and Reading
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The AD7798/AD7799 has three differential analog input channels:

* ``AD7799_CH_AIN1P_AIN1M``: AIN1(+) - AIN1(-)
* ``AD7799_CH_AIN2P_AIN2M``: AIN2(+) - AIN2(-)
* ``AD7799_CH_AIN3P_AIN3M``: AIN3(+) - AIN3(-)
* ``AD7799_CH_AIN1M_AIN1M``: AIN1(-) - AIN1(-) (for offset measurement)
* ``AD7799_CH_AVDD_MONITOR``: AVDD Monitor

Use ``ad7799_set_channel`` to select a channel and ``ad7799_get_channel`` to
perform a single conversion and read the raw data from a specific channel.
The ``ad7799_read_channel`` API reads and scales the data to mV or uV depending
on the configured precision.

Gain Configuration
~~~~~~~~~~~~~~~~~~

The programmable gain amplifier supports gains from 1 to 128. Use
``ad7799_set_gain`` and ``ad7799_get_gain`` to configure and read the gain
setting. Available gain values are:

* ``AD7799_GAIN_1`` (1x)
* ``AD7799_GAIN_2`` (2x)
* ``AD7799_GAIN_4`` (4x)
* ``AD7799_GAIN_8`` (8x)
* ``AD7799_GAIN_16`` (16x)
* ``AD7799_GAIN_32`` (32x)
* ``AD7799_GAIN_64`` (64x)
* ``AD7799_GAIN_128`` (128x)

Polarity Configuration
~~~~~~~~~~~~~~~~~~~~~~

The ADC supports both bipolar and unipolar coding. Use ``ad7799_set_polarity``
to configure:

* ``AD7799_BIPOLAR`` (0): Bipolar coding
* ``AD7799_UNIPOLAR`` (1): Unipolar coding

Operating Modes
~~~~~~~~~~~~~~~

The device supports multiple operating modes configured via ``ad7799_set_mode``:

* ``AD7799_MODE_CONT``: Continuous conversion mode
* ``AD7799_MODE_SINGLE``: Single conversion mode
* ``AD7799_MODE_IDLE``: Idle mode
* ``AD7799_MODE_PWRDN``: Power-down mode
* ``AD7799_MODE_CAL_INT_ZERO``: Internal zero-scale calibration
* ``AD7799_MODE_CAL_INT_FULL``: Internal full-scale calibration
* ``AD7799_MODE_CAL_SYS_ZERO``: System zero-scale calibration
* ``AD7799_MODE_CAL_SYS_FULL``: System full-scale calibration

AD7799 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct ad7799_dev *dev;
	int32_t ret;
	int32_t data_mv;

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_3,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct ad7799_init_param init_param = {
		.spi_init = spi_init,
		.chip_type = ID_AD7799,
		.gain = AD7799_GAIN_1,
		.polarity = AD7799_BIPOLAR,
		.vref_mv = 2500,
		.precision = AD7799_PRECISION_MV,
	};

	ret = ad7799_init(&dev, &init_param);
	if (ret)
		goto error;

	/* Read channel 0 scaled data */
	ret = ad7799_read_channel(dev, AD7799_CH_AIN1P_AIN1M, &data_mv);
	if (ret)
		goto error_remove;

	error_remove:
		ad7799_remove(dev);
	error:
		return ret;

AD7799 no-OS IIO Support
--------------------------

The AD7799 IIO driver comes on top of the AD7799 driver and offers support for
interfacing IIO clients through libiio.

AD7799 IIO Device Configuration
---------------------------------

Channel Attributes
~~~~~~~~~~~~~~~~~~

The AD7799 IIO driver exposes 3 differential voltage input channels (channel0,
channel1, channel2). Each channel has the following attribute:

* ``volts`` - reads the raw ADC conversion data for the channel

Global Attributes
~~~~~~~~~~~~~~~~~

* ``gain`` - reads or writes the ADC programmable gain amplifier setting
