ADF4371 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADF4371 <https://www.analog.com/ADF4371>`_

`ADF4372 <https://www.analog.com/ADF4372>`_

Overview
--------

The ADF4371 allows implementation of fractional-N or integer-N
phase-locked loop (PLL) frequency synthesizers when used with an external
loop filter and an external reference frequency. The wideband microwave
voltage-controlled oscillator (VCO) design allows frequencies from
62.5 MHz to 32 GHz to be generated.

The ADF4371 has an integrated VCO with a fundamental output frequency
ranging from 4000 MHz to 8000 MHz. RF8 output and RFAUX8 output
connect to divide-by-1/2/4/8/16/32/64 circuits that permit generation
of RF output frequencies as low as 62.5 MHz. RF16 provides a frequency
doubled output (8000 MHz to 16000 MHz) and RF32 provides a frequency
quadrupled output (16000 MHz to 32000 MHz).

Control of all on-chip registers is through a simple 3-wire SPI
interface. The ADF4371 operates with analog and digital power supplies
ranging from 3.15 V to 3.45 V, and 5 V for the VCO power supply. The
device also contains hardware and software power-down modes.

Applications
------------

* Wireless infrastructure (MC-GSM, 5G)
* Test equipment and instrumentation
* Clock generation
* Aerospace and defense

ADF4371 Device Configuration
-----------------------------

Initialization
~~~~~~~~~~~~~~

The ``adf4371_init()`` function configures the device with given
parameters and allocates the necessary resources. The
``adf4371_remove()`` function releases these resources and removes the
device instance from the system.

SPI Communication
~~~~~~~~~~~~~~~~~

The SPI communication functions for the ADF4371 device serve to enable
efficient data transactions between the device and a controlling system.
The ``adf4371_write`` function allows writing an 8-bit value to a
specified 16-bit register by building a command from the register
address and write operation, which is then transmitted through a 3-byte
buffer over SPI. It confirms success with a return code of 0, or an
appropriate negative error code upon failure.

The ``adf4371_write_bulk`` function extends this capability by
facilitating the writing of multiple bytes from an array to a device
register, using a buffer that incorporates the write command, the
register address, and the data. Like ``adf4371_write``, it returns 0
upon success, with negative codes indicating errors. The
``adf4371_read`` function, on the other hand, reads an 8-bit value from
a defined 16-bit register by issuing a read command and storing the
result from the SPI response.

Register and Frequency Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``adf4371_update`` function operates by reading a register's current
value, applying a bitmask to clear certain bits, and then writing back
the modified value to update specific bits of the register. It returns 0
to indicate a successful operation or a negative error code if any step
fails. This function is crucial for ensuring precise control over the
ADF4371 device's configuration and is often employed in modifying
registers to ensure they adhere to desired specifications.

The ``adf4371_pll_fract_n_compute`` function calculates the PLL
parameters such as integer and fractional division components for the VCO
based on its frequency and the PFD frequency.

In contrast, ``adf4371_pll_fract_n_get_rate`` computes the output
frequency for a specific channel, returning it as a 64-bit integer.
Finally, ``adf4371_set_freq`` sets a channel's frequency by validating
the frequency, adjusting dividers, computing the necessary PLL
parameters, and updating device registers accordingly, returning 0 for
success or a negative error upon failure.

Channel Control
~~~~~~~~~~~~~~~

The ``adf4371_channel_power_down`` function modifies the power state of
a specified channel in the ADF4371 device by altering the register bit
corresponding to the power-down state, effectively enabling or disabling
the channel power as needed. It takes a device pointer, channel number,
and a boolean to indicate power state, and returns 0 on success or a
negative error code upon failure.

Meanwhile, ``adf4371_channel_config`` manages the frequency
configuration and power state of channels, ensuring each channel's
frequency matches its desired setting. It iterates over channels, sets
frequencies if needed, and utilizes ``adf4371_channel_power_down`` to
power up channels, reporting 0 on success and a negative code if an
error occurs during operations.

Clock Control
~~~~~~~~~~~~~

The ``adf4371_clk_recalc_rate`` function recalculates and outputs the
current frequency rate for a specified channel on the ADF4371 device. It
checks if the specified channel number is valid, invokes
``adf4371_pll_fract_n_get_rate`` to obtain the recalculated rate, and
stores it in the provided location. It returns 0 on success or -1 if the
channel is invalid.

The ``adf4371_clk_round_rate`` function sets the desired frequency to
the closest achievable rate. It directly assigns the input rate to the
location pointed to by ``rounded_rate`` and consistently returns 0,
indicating success. Meanwhile, ``adf4371_clk_set_rate`` aims to adjust
the output frequency for a specified channel. It checks for valid
channel inputs before calling ``adf4371_set_freq`` to set the new
frequency, returning the outcome of this operation, which is 0 for
success and -1 if the channel is invalid.

ADF4371 Driver Initialization Example
--------------------------------------

.. code-block:: c

	struct adf4371_dev *adf4371_device;

	/* ADF4371 channel specifications */
	struct adf4371_chan_spec adf_chan_spec[1] = {
		{
			.num = 2,
			.power_up_frequency = 1200000000,
		}
	};

	/* SPI initialization parameters */
	struct no_os_spi_init_param clkchip_spi_init_param = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	/* ADF4371 initialization parameters */
	struct adf4371_init_param adf4371_param = {
		.spi_init = &clkchip_spi_init_param,
		.spi_3wire_enable = true,
		.clkin_frequency = 50000000,
		.muxout_select = 1,
		.num_channels = 1,
		.channels = adf_chan_spec,
	};

	ret = adf4371_init(&adf4371_device, &adf4371_param);
	if (ret)
		goto error;
