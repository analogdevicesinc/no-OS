ADMV1013 no-OS Driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADMV1013 <https://www.analog.com/ADMV1013>`_

Overview
--------

The ADMV1013 is a wideband, microwave
upconverter that operates over a 24 GHz to 44 GHz RF output frequency range.
The device supports two upconversion modes: direct conversion from baseband
I/Q signals to RF, and single-sideband upconversion from a real intermediate
frequency (IF) input. The LO input frequency range is 5.4 GHz to 10.25 GHz,
with an integrated LO quadrupler providing LO frequencies up to 41 GHz. The
ADMV1013 features a matched 50 ohm single-ended RF output and IF inputs, with
the option between matched 100 ohm balanced or 50 ohm single-ended LO inputs.
Baseband inputs are 100 ohm balanced. The device includes a variable
attenuator for transceiver power control and supports sideband suppression and
carrier feedthrough optimization. It is programmable via a 4-wire SPI
interface and is housed in a 40-terminal LGA package with a case temperature
range of -40 C to +85 C.

Applications
------------

* Point to point microwave radios
* Radar and electronic warfare
* Instrumentation and ATE

ADMV1013 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is ``admv1013_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver configures the SPI interface, performs a soft reset, verifies the
chip ID (0xA), writes the VVA temperature compensation value, configures the
quadrature single-ended mode, updates the mixer vgate based on the common-mode
voltage, sets the quad filters based on the LO input frequency, and enables
the detector and IF mode as configured.

The ``spi_init`` field in the init parameters is passed by pointer.

Input Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

The ADMV1013 supports two input modes, configured via the ``input_mode`` field
in the initialization parameters:

* ``ADMV1013_IQ_MODE``: Direct conversion mode from baseband I/Q signals to
  RF output. In this mode, the baseband I and Q signals are mixed with the
  quadrature LO to produce the upconverted RF output.
* ``ADMV1013_IF_MODE``: Single-sideband upconversion mode from a real IF
  input. In this mode, a single IF signal is upconverted to RF using the
  mixer.

LO Quadrature Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The LO quadrature single-ended mode is configured via the ``quad_se_mode``
field in the initialization parameters. The available modes are:

* ``ADMV1013_SE_MODE_POS``: Single-ended positive LO input
* ``ADMV1013_SE_MODE_NEG``: Single-ended negative LO input
* ``ADMV1013_SE_MODE_DIFF``: Differential LO input

The LO band filter selection is automatically configured during
initialization based on the ``lo_in`` frequency parameter. The driver selects
the appropriate filter band from the following options:

* 5.4 GHz to 7 GHz
* 5.4 GHz to 8 GHz
* 6.6 GHz to 9.2 GHz
* 8.62 GHz to 10.25 GHz

I/Q Phase and Offset Adjustment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``admv1013_set_iq_phase`` and ``admv1013_get_iq_phase`` APIs provide fine
phase adjustment of the I and Q LO paths. This allows optimization of
sideband suppression by adjusting the relative phase between the I and Q
channels. The phase values are written to the fine phase adjustment registers
for the I and Q LO amplifiers.

The ``admv1013_set_iq_offset`` and ``admv1013_get_iq_offset`` APIs allow
adjustment of the mixer offset for carrier feedthrough nulling. Each path
(I and Q) has separate positive and negative offset adjustments
(``i_offset_p``, ``i_offset_n``, ``q_offset_p``, ``q_offset_n``), providing
fine control over DC offset to minimize carrier leakage at the RF output.

Envelope Detector
~~~~~~~~~~~~~~~~~

The ADMV1013 includes an output envelope detector that can be enabled or
disabled via the ``det_en`` parameter in the initialization structure. When
enabled, the detector provides an output proportional to the RF output
envelope.

ADMV1013 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct admv1013_dev *dev;
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

	struct admv1013_init_param init_param = {
		.spi_init = &spi_init,
		.lo_in = 8000000000ULL,
		.input_mode = ADMV1013_IQ_MODE,
		.quad_se_mode = ADMV1013_SE_MODE_DIFF,
		.det_en = true,
		.vcm_uv = 1800000,
	};

	ret = admv1013_init(&dev, &init_param);
	if (ret)
		goto error;

	/* Adjust I/Q phase for sideband suppression optimization */
	ret = admv1013_set_iq_phase(dev, 0, 0);
	if (ret)
		goto error_remove;

	/* Adjust I/Q offset for carrier feedthrough nulling */
	ret = admv1013_set_iq_offset(dev, 0, 0, 0, 0);
	if (ret)
		goto error_remove;

	error_remove:
		admv1013_remove(dev);
	error:
		return ret;
