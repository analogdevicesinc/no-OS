ADAR300X no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* :adi:`ADAR3000`
* :adi:`ADAR3001`

Overview
--------

The ADAR3000 is a 17.7 GHz to 21.2 GHz transmit or receive, 4-beam and
4-element Ka band beamforming core chip for phased arrays. Each of the 16 RF
channels provides a 31.5 dB gain range in 0.5 dB steps and up to 54.5 ps of
time delay in 0.865 ps steps, through variable amplitude and phase (VAP)
blocks. Every VAP contains a digital stepped attenuator (DSA) and a time delay
unit (TDU).

The device stores 256 prestored beam positions in RAM, arranged as 64
beamstates per beam across 4 beams, plus a FIFO holding 64 beam positions as 16
beamstates per beam. A user programmable sequencer selects beamstates, and
update, reset and mute commands can be applied to between 1 and 16 beams. An
integrated 8-bit ADC serves an on-chip temperature sensor and two general
purpose analog inputs, and the amplifier bias modes are programmable.

All on-chip registers are controlled through a 4-wire SPI. Four address pins
allow up to 16 devices to share the same serial lines, which the driver
supports through the ``dev_addr`` field and an optional shared SPI descriptor.
The part is supplied from a 1.2 V analog rail and a 1.8 V digital rail, and is
available in a 311-ball CSP_BGA specified from -40°C to +85°C.

Applications
------------

* Geosynchronous high-throughput satellite (GEO HTS)
* Low earth orbit (LEO) constellations
* Mobile satellite terminals (land, air, and sea)
* Terrestrial, airborne and satellite phased arrays

ADAR300X Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI), configured for mode 0, MSB first, at a
clock rate of up to 1 MHz.

The first API to be called is **adar300x_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

**adar300x_init** allocates the device descriptor, requests the optional
control GPIOs, pulses the hardware reset, brings up the SPI bus, issues the
soft reset sequence, proves the SPI link with a scratchpad write and read back
test, and finally validates the ``CHIPTYPE`` register. Resources are released
with **adar300x_remove**.

Register Access
~~~~~~~~~~~~~~~

Registers are accessed with a three byte SPI frame carrying a 16-bit address
word followed by 8 bits of data. Bit 15 selects read or write, bits 13 down to
10 carry the chip address and bits 9 down to 0 the register address.

**adar300x_reg_write** writes a single register and **adar300x_reg_read** reads
one back. **adar300x_reg_update** performs a read modify write under a caller
supplied mask and skips the write when the value is unchanged.

Address Paging
~~~~~~~~~~~~~~

The beamstate RAM is reached through an address page selected by register
``0x008``. **adar300x_set_page** selects a page and caches the selection, so
repeated accesses to the same page do not repeat the register write.
**adar300x_page_reg_write** and **adar300x_page_reg_read** combine a page
selection with a register access.

The cached page is invalidated by both reset paths, so a reset can never leave
the driver believing that a stale page is still selected.

Reset
~~~~~

**adar300x_hard_reset** pulses the RSTB pin, and is a no-op when no reset GPIO
is wired. **adar300x_soft_reset** issues the register based reset sequence.

The data sheet recommends holding RSTB low across the supply ramp. When that is
not possible it asks for at least two, and preferably three, back to back soft
resets after power up, which **adar300x_soft_reset** performs.

Multiple Devices on One SPI Bus
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Up to 16 devices may share the same SPI lines, distinguished by the 4-bit chip
address transmitted in every frame, which must match the ADDR3 to ADDR0 strap
pins of each part.

Initialize the first device normally and pass its ``spi_desc`` as
``spi_desc_shared`` when initializing the others, each with its own
``dev_addr``. Only the device that owns the bus frees it in
**adar300x_remove**, so the shared descriptor stays valid until the owner is
removed last.

ADAR300X Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct adar300x_dev *dev;
	int ret;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct adar300x_init_param adar300x_ip = {
		.spi_init = {
			.device_id = SPI_DEVICE_ID,
			.chip_select = SPI_CS,
			.max_speed_hz = 1000000,
			.mode = NO_OS_SPI_MODE_0,
			.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
			.platform_ops = SPI_OPS,
			.extra = SPI_EXTRA,
		},
		.spi_desc_shared = NULL,
		.gpio_rstb = &adar300x_gpio_rstb_ip,
		.gpio_mux_sel = &adar300x_gpio_mux_sel_ip,
		.gpio_update = &adar300x_gpio_update_ip,
		.gpio_reset = &adar300x_gpio_reset_ip,
		.gpio_mute = &adar300x_gpio_mute_ip,
		.type = ADAR3000,
		.dev_addr = 0,
	};

	ret = adar300x_init(&dev, &adar300x_ip);
	if (ret)
		goto error;
