AD917x no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

`AD9171 <https://www.analog.com/AD9171>`_
`AD9172 <https://www.analog.com/AD9172>`_
`AD9173 <https://www.analog.com/AD9173>`_
`AD9174 <https://www.analog.com/AD9174>`_
`AD9175 <https://www.analog.com/AD9175>`_
`AD9176 <https://www.analog.com/AD9176>`_

Overview
--------

The AD9172 is a high performance, dual, 16-bit digital-to-analog converter
(DAC) that supports DAC sample rates up to 12.6 GSPS. The device features
an 8-lane, 15 Gbps JESD204B data input port, a high performance on-chip
DAC clock multiplier, and digital signal processing capabilities targeted
at single-band and multiband direct to RF wireless applications.

The AD9172 features three bypassable, complex data input channels per RF
DAC. Each data input channel includes a configurable gain stage, an
interpolation filter, and a channel numerically controlled oscillator (NCO)
for flexible, multiband frequency planning. The device supports up to a
1.5 GSPS complex data rate per input channel. The AD9172 proprietary, low
spurious design achieves excellent performance metrics, including a 2-tone
intermodulation distortion (IMD) of -83 dBc at 1.8 GHz and a spurious free
dynamic range (SFDR) of <-80 dBc.

Applications
------------

* Wireless communications infrastructure
* Multiband base station radios
* Microwave/E-band backhaul systems
* Instrumentation, automatic test equipment (ATE)
* Radars and jammers

AD917x Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``ad9172_init()`` function is responsible for setting up the AD9172 DAC
by configuring the SPI and GPIO communication interfaces, allocating memory
for the device descriptor and state, and transferring the initialization
parameters into the driver's internal structures. This function initializes
all essential registers and performs device setup, including DAC clock
configuration, JESD204B datapath configuration, and PLL locking, to ensure
the device is ready for operation. It must be the first API invoked during the
device lifecycle.

JESD Interface Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The JESD204B interface is configured during initialization via the
``ad917x_jesd_config_datapath()`` function, which sets up the link mode,
channel interpolation, and DAC interpolation. The
``ad917x_jesd_set_scrambler_enable()`` function enables or disables the
descrambler for the JESD interface. Lane crossbar mapping is configured using
``ad917x_jesd_set_lane_xbar()``.

Resource Removal
~~~~~~~~~~~~~~~~

The ``ad9172_remove()`` function gracefully deallocates resources that were
previously allocated during device initialization. It handles the removal of
SPI and GPIO descriptors and frees any memory associated with device state or
descriptor structures, ensuring that system resources are reclaimed.

AD917x Driver Initialization Example
-------------------------------------

.. code-block:: c

	struct ad9172_init_param ad9172_ip = {
		.spi_init = &ad9172_spi_ip,
		.gpio_reset = {
			.number = GPIO_RESET_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.gpio_txen0 = {
			.number = GPIO_TXEN0_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.gpio_txen1 = {
			.number = GPIO_TXEN1_PIN,
			.platform_ops = GPIO_OPS,
			.extra = GPIO_EXTRA,
		},
		.dac_rate_khz = 12000000,
		.dac_clkin_Hz = 368640000,
		.jesd_link_mode = 4,
		.jesd_subclass = 1,
		.dac_interpolation = 8,
		.channel_interpolation = 4,
		.clock_output_config = 4,
		.syncoutb_type = SIGNAL_LVDS,
		.sysref_coupling = COUPLING_AC,
	};

	ad9172_dev *dev;
	int ret;

	ret = ad9172_init(&dev, &ad9172_ip);
	if (ret)
		goto error;
