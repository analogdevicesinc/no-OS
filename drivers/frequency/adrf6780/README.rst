ADRF6780 no-OS Driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADRF6780 <https://www.analog.com/ADRF6780>`_

Overview
--------

The ADRF6780 is a wideband, microwave
upconverter optimized for point to point microwave radio designs operating over
an RF output frequency range of 5.9 GHz to 23.6 GHz. The device is fabricated
on a silicon germanium (SiGe) process and supports two upconversion modes:
direct conversion from baseband I/Q signals to RF, and single-sideband (SSB)
upconversion from a real intermediate frequency (IF) input. The local oscillator
(LO) input frequency range is 5.4 GHz to 14 GHz, with an integrated LO
frequency doubler extending the effective LO range up to 28 GHz. The ADRF6780
features matched 100 ohm balanced RF output, LO input, and IF input, along with
high impedance baseband inputs. The device integrates a variable attenuator and
power detector for transmit power control, as well as sideband suppression and
carrier feedthrough optimization capabilities. It is programmable via a 4-wire
SPI interface and is available in a 32-lead, 5 mm x 5 mm LFCSP package, rated
for operation from -40 C to +85 C.

Applications
------------

* Point to point microwave radios
* Radar/electronic warfare
* Instrumentation/ATE

ADRF6780 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``adrf6780_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver configures the SPI interface, performs a soft reset, and verifies
the chip ID (expected value 0xA). It then configures the enable register, which
controls the VGA buffer, detector (always enabled), LO buffer, IF/IQ mode
selection, LO x2/PPF selection, LO enable, and UC bias enable. The
initialization also sets the LO sideband selection and configures the ADC VDET
output source. Note that IF mode and IQ mode are mutually exclusive in the
enable register, and LO x2 and LO PPF are also mutually exclusive.

The ``spi_init`` field in the init parameters is passed by pointer.

Mode Configuration
~~~~~~~~~~~~~~~~~~

The ADRF6780 supports two upconversion modes, which are mutually exclusive in
the enable register. Setting ``if_mode_en`` to true enables IF mode, which
uses single-sideband (SSB) upconversion from a real IF input. When
``if_mode_en`` is false, the device operates in I/Q mode, performing direct
conversion from baseband I/Q signals to RF.

LO Path Configuration
~~~~~~~~~~~~~~~~~~~~~

The ``lo_x2_en`` parameter enables the LO frequency doubler. When disabled,
the LO polyphase filter (PPF) is used instead. These two options are mutually
exclusive in the enable register. The ``lo_sideband`` parameter selects between
upper and lower sideband operation. The ``lo_buff_en`` and ``lo_en`` parameters
control the LO buffer amplifier and the LO path respectively.

Linearization
~~~~~~~~~~~~~

The ``adrf6780_set_rdac_linearize`` and ``adrf6780_get_rdac_linearize`` APIs
allow setting and reading the RDAC linearize value. This setting is used to
improve intermodulation distortion (IMD) performance of the upconverter.

I/Q Phase Accuracy
~~~~~~~~~~~~~~~~~~

The ``adrf6780_set_cdac_iq_phase_accuracy`` and
``adrf6780_get_cdac_iq_phase_accuracy`` APIs allow tuning the CDAC I/Q phase
accuracy to optimize sideband suppression. The I and Q paths can be adjusted
independently through separate ``i_data`` and ``q_data`` parameters.

ADC Power Detector
~~~~~~~~~~~~~~~~~~

The ``adrf6780_read_adc_raw`` API reads the on-chip ADC output from the
integrated power detector. The ``vdet_out_en`` initialization parameter selects
the VDET output source for the ADC measurement.

ADRF6780 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct adrf6780_dev *dev;
	int ret;

	struct no_os_spi_init_param spi_init = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct adrf6780_init_param adrf6780_ip = {
		.spi_init = &spi_init,
		.lo_in = 12000000000ULL,
		.vga_buff_en = true,
		.lo_buff_en = true,
		.if_mode_en = false,
		.lo_x2_en = true,
		.lo_en = true,
		.uc_bias_en = true,
		.lo_sideband = false,
		.vdet_out_en = false,
	};

	ret = adrf6780_init(&dev, &adrf6780_ip);
	if (ret)
		goto error;

	ret = adrf6780_set_cdac_iq_phase_accuracy(dev, 0x08, 0x08);
	if (ret)
		goto error_remove;

	uint16_t adc_raw;

	ret = adrf6780_read_adc_raw(dev, &adc_raw);
	if (ret)
		goto error_remove;

	error_remove:
		adrf6780_remove(dev);
	error:
		return ret;
