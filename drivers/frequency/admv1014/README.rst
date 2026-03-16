ADMV1014 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADMV1014 <https://www.analog.com/ADMV1014>`_

Overview
--------

The ADMV1014 is a wideband,
microwave downconverter optimized for point to point microwave radio designs
operating in the 24 GHz to 44 GHz frequency range. The device is fabricated on a
silicon germanium (SiGe) process and supports two downconversion modes: direct
quadrature demodulation to baseband I/Q and image rejection downconversion to
complex intermediate frequency (IF). The local oscillator (LO) input frequency
range is 5.4 GHz to 10.25 GHz, with an integrated LO quadrupler providing
operation up to 41 GHz. The ADMV1014 features a matched 50 ohm single-ended RF
input and complex IF outputs, with an option between matched 100 ohm balanced or
50 ohm single-ended LO inputs. Baseband I/Q outputs have 100 ohm balanced output
impedance with adjustable common-mode voltage. A square law power detector is
included for setting mixer input power, and a variable attenuator enables
receiver power control. The device is programmable via a 4-wire SPI interface and
is available in a 32-terminal, 5 mm x 5 mm LGA package, operating over a case
temperature range of -40 C to +85 C.

Applications
------------

* Point to point microwave radios
* Radar and electronic warfare
* Instrumentation and automatic test equipment (ATE)

ADMV1014 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``admv1014_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization, the
driver configures the SPI interface, performs a soft reset, writes the VVA
temperature compensation value, and verifies the chip ID. It then configures the
quadrature single-ended mode and selects the appropriate LO band filter based on
the provided LO input frequency. The VCM levels are set according to the
specified common-mode voltage, and the enable register is configured to control
P1dB compensation, IF amplifier, baseband amplifier, and detector settings. The
SPI initialization parameters are passed via a pointer through the ``spi_init``
field of the initialization parameter structure.

Input Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

The ADMV1014 supports two input modes, configured via the ``input_mode`` field
in the initialization parameter structure. In I/Q mode
(``ADMV1014_IQ_MODE``), the device performs direct quadrature demodulation to
baseband I and Q outputs. In IF mode (``ADMV1014_IF_MODE``), the device
operates as an image rejection downconverter to complex intermediate frequency.

LO Quadrature Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The LO quadrature single-ended mode is configured via the ``quad_se_mode`` field
in the initialization parameter structure. The available modes are single-ended
positive (``ADMV1014_SE_MODE_POS``), single-ended negative
(``ADMV1014_SE_MODE_NEG``), and differential (``ADMV1014_SE_MODE_DIFF``). The
LO band filter is automatically selected during initialization based on the
``lo_in`` frequency parameter, matching the appropriate filter range from the
available bands (5.4 GHz to 7 GHz, 5.4 GHz to 8 GHz, 6.6 GHz to 9.2 GHz,
and 8.62 GHz to 10.25 GHz).

IF Amplifier Gain
~~~~~~~~~~~~~~~~~

The IF amplifier gain can be configured using ``admv1014_set_if_amp_gain`` and
read back using ``admv1014_get_if_amp_gain``. The gain is set independently for
the I and Q paths, with both coarse and fine gain controls. The function takes
four parameters: ``i_coarse_gain``, ``q_coarse_gain``, ``i_fine_gain``, and
``q_fine_gain``.

Baseband Amplifier Gain
~~~~~~~~~~~~~~~~~~~~~~~

The baseband amplifier gain can be configured using ``admv1014_set_bb_amp_gain``
and read back using ``admv1014_get_bb_amp_gain``. This controls the gain of the
baseband amplifier stage.

Phase Adjustment
~~~~~~~~~~~~~~~~

The LO amplifier phase can be adjusted independently for the I and Q paths using
``admv1014_set_phase`` and read back using ``admv1014_get_phase``. This allows
optimization of image rejection performance by tuning the I and Q phase
parameters.

Power Detector Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The digital Rx detector sensitivity range can be configured using
``admv1014_set_det_prog`` and read back using ``admv1014_get_det_prog``. The
available detector program ranges are:

* ``ADMV1014_DET_PROG_NEG_12_DBM_TO_POS_4DBM`` - -12 dBm to +4 dBm
* ``ADMV1014_DET_PROG_NEG_13_DBM_TO_POS_3DBM`` - -13 dBm to +3 dBm
* ``ADMV1014_DET_PROG_NEG_14_DBM_TO_POS_2DBM`` - -14 dBm to +2 dBm
* ``ADMV1014_DET_PROG_NEG_15_DBM_TO_POS_1DBM`` - -15 dBm to +1 dBm
* ``ADMV1014_DET_PROG_NEG_15_5_DBM_TO_POS_0_5_DBM`` - -15.5 dBm to +0.5 dBm
* ``ADMV1014_DET_PROG_NEG_16_25_DBM_TO_NEG_0_25_DBM`` - -16.25 dBm to -0.25 dBm
* ``ADMV1014_DET_PROG_NEG_17_DBM_TO_NEG_1DBM`` - -17 dBm to -1 dBm
* ``ADMV1014_DET_PROG_NEG_18_DBM_TO_NEG_2DBM`` - -18 dBm to -2 dBm

ADMV1014 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct admv1014_dev *dev;
	int ret;

	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_MAX_SPEED,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct admv1014_init_param admv1014_ip = {
		.spi_init = &spi_ip,
		.lo_in = 8000000000ULL,
		.input_mode = ADMV1014_IQ_MODE,
		.quad_se_mode = ADMV1014_SE_MODE_DIFF,
		.det_en = true,
		.vcm_mv = 1050,
		.p1db_comp_en = true,
	};

	ret = admv1014_init(&dev, &admv1014_ip);
	if (ret)
		goto error;

	ret = admv1014_set_if_amp_gain(dev, 8, 8, 7, 7);
	if (ret)
		goto error_remove;

	ret = admv1014_set_phase(dev, 3, 3);
	if (ret)
		goto error_remove;

	error_remove:
		admv1014_remove(dev);
	error:
		return ret;
