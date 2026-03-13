ADMV8818 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADMV8818 <https://www.analog.com/ADMV8818>`_

Overview
--------

The ADMV8818 is a fully monolithic microwave integrated circuit (MMIC) that
features a digitally selectable frequency of operation. The device features four
independently controlled high-pass filters (HPFs) and four independently
controlled low-pass filters (LPFs) that span the 2 GHz to 18 GHz frequency
range.

The flexible architecture of the ADMV8818 allows the 3 dB cutoff frequency
(f3dB) of the high-pass and low-pass filters to be controlled independently to
generate up to 4 GHz of bandwidth. The digital logic control on each filter is
4 bits wide (16 states) and controls the on-chip reactive elements to adjust
the f3dB. The typical insertion loss is 9 dB, and the wideband rejection is
35 dB, which is ideally suited for minimizing system harmonics.

This tunable filter can be used as a smaller alternative to large switched
filter banks and cavity tuned filters, and provides a dynamically adjustable
solution in advanced communications applications. The ADMV8818 is housed in a
compact 9 mm x 9 mm, 56-terminal LGA package.

Applications
------------

* Test and measurement equipment
* Military radar, electronic warfare, and electronic countermeasures
* Satellite communications and space
* Industrial and medical equipment

ADMV8818 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is ``admv8818_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver performs a soft reset, enables the SDO output, verifies the chip ID,
and configures single instruction mode. If the filter mode is set to
``ADMV8818_AUTO``, the driver will automatically configure the HPF and LPF
based on the provided RF input frequency.

Filter Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

The ADMV8818 driver supports two filter modes:

* ``ADMV8818_AUTO``: The driver automatically selects the appropriate HPF and
  LPF band and state based on the RF input frequency provided in the
  initialization parameters. This is done by calling ``admv8818_rfin_select``,
  which internally configures both the HPF and LPF to match the specified
  frequency.

* ``ADMV8818_MANUAL``: The HPF and LPF cutoff frequencies are configured
  independently by the user through the ``admv8818_hpf_select`` and
  ``admv8818_lpf_select`` APIs.

High-Pass Filter Control
~~~~~~~~~~~~~~~~~~~~~~~~

The high-pass filter is configured using ``admv8818_hpf_select``, which
accepts a frequency value and automatically selects the closest HPF band and
state. The HPF covers four bands:

* Band 1: 1.75 GHz to 3.55 GHz
* Band 2: 3.40 GHz to 7.25 GHz
* Band 3: 6.60 GHz to 12.00 GHz
* Band 4: 12.50 GHz to 19.90 GHz

Each band has 16 states (4-bit control) that provide fine frequency adjustment
within the band range. The current HPF frequency can be read back using
``admv8818_read_hpf_freq``.

Low-Pass Filter Control
~~~~~~~~~~~~~~~~~~~~~~~

The low-pass filter is configured using ``admv8818_lpf_select``, which
accepts a frequency value and automatically selects the closest LPF band and
state. The LPF covers four bands:

* Band 1: 2.05 GHz to 3.85 GHz
* Band 2: 3.35 GHz to 7.25 GHz
* Band 3: 7.00 GHz to 13.00 GHz
* Band 4: 12.55 GHz to 18.50 GHz

Each band has 16 states (4-bit control) that provide fine frequency adjustment
within the band range. The current LPF frequency can be read back using
``admv8818_read_lpf_freq``.

ADMV8818 Driver Initialization Example
---------------------------------------

.. code-block:: c

	struct admv8818_dev *dev;
	struct admv8818_init_param init_param;
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

	init_param.spi_init = &spi_init;
	init_param.rf_in = 10000000000ULL; /* 10 GHz */
	init_param.mode = ADMV8818_AUTO;

	ret = admv8818_init(&dev, &init_param);
	if (ret)
		goto error;

	/* To manually configure filters: */
	ret = admv8818_hpf_select(dev, 5000000000ULL); /* Set HPF to 5 GHz */
	if (ret)
		goto error_remove;

	ret = admv8818_lpf_select(dev, 8000000000ULL); /* Set LPF to 8 GHz */
	if (ret)
		goto error_remove;

	/* Read back filter frequencies */
	unsigned long long hpf_freq, lpf_freq;
	ret = admv8818_read_hpf_freq(dev, &hpf_freq);
	if (ret)
		goto error_remove;

	ret = admv8818_read_lpf_freq(dev, &lpf_freq);
	if (ret)
		goto error_remove;

	error_remove:
		admv8818_remove(dev);
	error:
		return ret;
