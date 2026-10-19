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

Operation Modes
---------------

Each beam takes its beamstate from one of four sources, selected per beam with
**adar300x_set_beam_mode**.

+------------------------------------+--------------------------------------+-------------------------------+
| Mode                               | Beamstate source                     | Applied by                    |
+====================================+======================================+===============================+
| ``ADAR300X_BEAM_MODE_DIRECT``      | Direct control registers             | An update command             |
+------------------------------------+--------------------------------------+-------------------------------+
| ``ADAR300X_BEAM_MODE_MEMORY``      | RAM, 64 beamstates per beam, stepped | Each update advances the      |
|                                    | through by the sequencer             | sequencer by one beamstate    |
+------------------------------------+--------------------------------------+-------------------------------+
| ``ADAR300X_BEAM_MODE_FIFO``        | FIFO, 16 beamstates per beam         | Each update dequeues one      |
|                                    |                                      | beamstate                     |
+------------------------------------+--------------------------------------+-------------------------------+
| ``ADAR300X_BEAM_MODE_INST_DIRECT`` | Direct control registers             | Immediately, on the SPI write |
+------------------------------------+--------------------------------------+-------------------------------+

In every mode except instantaneous direct, a reset or mute command sends the
beam to its reset or mute beamstate instead.

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
test, and finally validates the ``CHIPTYPE`` and ``PRODUCT_ID_L`` registers
against the part selected in ``type``. Resources are released with
**adar300x_remove**.

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

Addresses ``0x100`` to ``0x3FF`` are reused across six address pages selected
by register ``0x008``: page 0 holds the configuration registers, pages 1 to 4
the RAM of beams 0 to 3, and page 5 the FIFO load registers.
**adar300x_set_page** selects a page and caches the selection, so repeated
accesses to the same page do not repeat the register write.
**adar300x_page_reg_write** and **adar300x_page_reg_read** combine a page
selection with a register access.

The data sheet describes ``0x000`` to ``0x0FF`` as common to all pages, but
beam commands written while the FIFO page is selected are ignored. The driver
therefore accesses every register above the standard SPI block at ``0x000`` to
``0x00B`` with page 0 selected. The cached page is invalidated by both reset
paths, so a reset can never leave the driver believing that a stale page is
still selected.

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

Beam Mode and Update Source
~~~~~~~~~~~~~~~~~~~~~~~~~~~

**adar300x_set_beam_mode** and **adar300x_get_beam_mode** select and read back
the beamstate source of one beam, see `Operation Modes`_.

Update, reset and mute are driven either by the UPDATE, RESET and MUTE pins or
over SPI. **adar300x_set_update_source_spi** selects SPI when passed true, and
**adar300x_get_update_source_spi** reads the selection back. The commands in
`Beam Commands`_ only take effect with SPI selected.

Direct, Reset and Mute Beamstates
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Three register banks hold one beamstate per beam each: the direct control
bank, applied by an update in direct mode, and the reset and mute banks,
applied by the reset and mute commands. They share one layout, so a single
pair of functions covers all three.

**adar300x_set_element** writes, and **adar300x_get_element** reads, the delay
(``ADAR300X_DELAY``) or attenuation (``ADAR300X_ATTENUATION``) of one element
of one beam, in the bank selected by ``ADAR300X_BEAMSTATE_DIRECT``,
``ADAR300X_BEAMSTATE_RESET`` or ``ADAR300X_BEAMSTATE_MUTE``. Both values are
6-bit, from 0 to 63: delay in 0.865 ps steps and attenuation in 0.5 dB steps.

Beam Commands
~~~~~~~~~~~~~

**adar300x_beam_command** applies ``ADAR300X_BEAM_CMD_UPDATE``,
``ADAR300X_BEAM_CMD_RESET`` or ``ADAR300X_BEAM_CMD_MUTE`` to every beam set in
a mask, bit 0 being beam 0. Reset and mute are qualifiers staged per beam and
applied by the update strobe, the same way the RESET and MUTE pins qualify the
UPDATE pin.

A beam in instantaneous direct mode ignores all three commands: updates have no
effect because writes already apply immediately, and the data sheet states
that such a beam cannot enter its reset or mute beamstate.

Amplifier Bias and Enable
~~~~~~~~~~~~~~~~~~~~~~~~~

Each beam amplifier has a 3-bit bias and an enable bit for each of four
states, reset, operational, mute and sleep, set with **adar300x_set_beam_amp**
and read with **adar300x_get_beam_amp**. The element amplifiers have the same
controls for the operational and sleep states, through
**adar300x_set_element_amp** and **adar300x_get_element_amp**. The amplifiers
power up enabled with a bias of 3.

The data sheet also lists element amplifier mute registers at ``0xD8`` to
``0xDB``. They read back 0 and discard writes on the ADAR3000, so the driver
does not expose them.

Beamstate RAM
~~~~~~~~~~~~~

Each beam holds 64 beamstates in RAM, on its own address page. A beamstate is
eight 6-bit values, the delay then the attenuation of each element in turn,
packed MSB first into six bytes.

**adar300x_set_ram_beamstate** writes one beamstate from its eight unpacked
values and **adar300x_get_ram_beamstate** reads one back. The read follows the
data sheet's fetch protocol: a first read latches the whole beamstate and
returns invalid data, after which the six bytes read back singly.

RAM Sequencer
~~~~~~~~~~~~~

In memory mode every update advances a beam's sequencer from the start pointer
to the stop pointer, then wraps back to the start. **adar300x_set_seq_range**
sets both pointers and **adar300x_get_seq_range** reads them. A stop pointer
below the start pointer is valid and wraps from pointer 63 to pointer 0. The
range powers up as 0 to 63.

FIFO
~~~~

**adar300x_load_fifo_beamstate** queues one beamstate onto a beam's FIFO,
which holds 16. The write to the last of the six load registers is what
commits the beamstate, and a load onto a full FIFO is discarded by the device.

The FIFO cannot be read back. **adar300x_get_fifo_pointers** reads the write
pointer, which advances on each load, and the read pointer, which advances on
each update. Both wrap at 16, and a reset empties the queue by moving the read
pointer onto the write pointer.

Temperature Sensor
~~~~~~~~~~~~~~~~~~

**adar300x_get_temp** returns the raw 8-bit code of the on-chip temperature
sensor. The on-chip ADC runs off SCLK, so a conversion only advances while the
bus is active; the function supplies the dummy transactions and polls for the
end of conversion.

The data sheet gives a slope of 1.09 LSB/°C and a nominal code of 127 at 25°C.
In integer millidegrees, using the defines in ``adar300x.h``:

.. code-block:: bash

	mdegc = ADAR300X_TEMP_NOMINAL_MDEGC +
		((int)code - ADAR300X_TEMP_NOMINAL_CODE) *
		ADAR300X_TEMP_SLOPE_NUM / ADAR300X_TEMP_SLOPE_DEN;

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

ADAR300X no-OS IIO support
--------------------------

The ADAR300X IIO driver comes on top of the ADAR300X driver and offers support
for interfacing IIO clients through libiio. It exposes the same interface as
the Linux ``adar300x`` IIO driver, so channel and attribute names carry over,
with the differences listed in `Differences from the Linux Driver`_.

ADAR300X IIO Device Configuration
---------------------------------

Channels
~~~~~~~~

The device has 33 channels. For each of the 16 elements, numbered
``beam * 4 + element``, there is a ``phase`` channel carrying the delay and a
``power`` channel carrying the attenuation. The pattern continues through all
four beams:

+--------------------------+------+---------+--------------+
| Channels                 | Beam | Element | Scan index   |
+==========================+======+=========+==============+
| ``phase0``, ``power0``   | 0    | 0       | 0, 1         |
+--------------------------+------+---------+--------------+
| ``phase1``, ``power1``   | 0    | 1       | 2, 3         |
+--------------------------+------+---------+--------------+
| ``phase4``, ``power4``   | 1    | 0       | 8, 9         |
+--------------------------+------+---------+--------------+
| ``phase15``, ``power15`` | 3    | 3       | 30, 31       |
+--------------------------+------+---------+--------------+
| ``temp16``               | none | none    | not buffered |
+--------------------------+------+---------+--------------+

Channel Attributes
~~~~~~~~~~~~~~~~~~

* ``out_phaseN_raw - delay of the element, 0 to 63, in the bank selected by
  the beam's load_mode``
* ``out_phaseN_scale - 0.865, picoseconds of delay per step``
* ``out_powerN_raw - attenuation of the element, 0 to 63, in the bank selected
  by the beam's load_mode``
* ``out_powerN_scale - 0.5, decibels of attenuation per step``
* ``in_temp16_raw - raw code of the temperature sensor``
* ``in_temp16_offset - -99.75``
* ``in_temp16_scale - 917.431192660, millidegrees Celsius per code``

The temperature in millidegrees Celsius is ``(raw + offset) * scale``.

The ``raw`` attributes of a beam address the bank chosen by its ``load_mode``:
the direct control bank for ``direct`` and ``instant_direct``, the reset or
mute bank for ``reset`` or ``mute``, RAM beamstate ``ram_index`` for
``memory``, and a staging beamstate for ``fifo``. In ``fifo`` load mode the
staged beamstate is queued when its eighth value, ``powerN`` of the beam's
element 3, is written.

Global Attributes
~~~~~~~~~~~~~~~~~

In the attributes below, ``N`` is the beam number, 0 to 3.

* ``beamN_mode - beam mode: direct, memory, fifo, instant_direct, or reset or
  mute to have the next update apply that beamstate``
* ``beamN_mode_available - the accepted beamN_mode values``
* ``beamN_update - write any value to issue an update, or the reset or mute
  selected in beamN_mode; reads 0``
* ``beamN_load_mode - bank addressed by the beam's channels and buffer, same
  values as beamN_mode``
* ``beamN_load_mode_available - the accepted beamN_load_mode values``
* ``beamN_ram_index - RAM beamstate addressed by the channels in memory load
  mode, and the first one the buffer writes, 0 to 63``
* ``beamN_ram_start - sequencer start pointer, 0 to 63``
* ``beamN_ram_stop - sequencer stop pointer, 0 to 63``
* ``beamN_fifo_rd - FIFO read pointer, read only``
* ``beamN_fifo_wr - FIFO write pointer, read only``
* ``update_intf_ctrl - source of update, reset and mute, pin or SPI``
* ``update_intf_ctrl_available - the accepted update_intf_ctrl values``

The amplifier attributes take a bias of 0 to 7 or an enable of 0 or 1:

* ``beamN_amp_bias_<state>, beamN_amp_en_<state> - bias and enable of beam
  amplifier N, for <state> reset, operational, mute and sleep``
* ``elementN_amp_bias_<state>, elementN_amp_en_<state> - bias and enable of
  element amplifier N, for <state> operational and sleep``

Debug Attributes
~~~~~~~~~~~~~~~~

* ``direct_reg_access - raw register read and write. The address is not paged:
  select the page through register 0x008 first when reaching 0x100 and
  above``

Output Buffer
~~~~~~~~~~~~~

The output buffer loads whole beamstates into RAM or the FIFO. Enable all eight
channels of each beam that takes part, ``phase4B`` to ``power4B+3`` for beam
``B``; partially enabled beams are rejected. Each scan carries one beamstate
per enabled beam, whose ``load_mode`` must be ``memory`` or ``fifo``:

* ``memory`` - scan ``n`` is written to RAM beamstate ``ram_index + n``. A
  buffer that would run past beamstate 63 is rejected.
* ``fifo`` - each scan is queued in turn. Loads beyond the 16 the FIFO holds
  are discarded by the device.

Each value takes one byte, of which the low six bits are used. The largest
buffer, 64 beamstates for each of the four beams, is 2048 bytes.

Differences from the Linux Driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The attribute names and behaviour follow the Linux driver except where it
disagrees with the data sheet:

* The ``phase`` scale is 0.865 ps per step, the time delay the part actually
  applies. Linux reports 360 / 63 degrees, which is not a property of a true
  time delay unit.
* The ``power`` scale is 0.5 dB per step. Linux reports 31 / 63.
* The temperature offset and scale reproduce 25°C at code 127. The Linux
  values give 194 m°C there.
* ``beamN_ram_index`` counts whole beamstates for the buffer. Linux adds it to
  the RAM address as a byte offset.
* The amplifier attributes are named after the bank they control. Linux names
  the beam amplifiers ``ELnV`` and the element amplifiers ``ELnH``.

ADAR300X IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	#define ADAR300X_IIO_BUFFER_SIZE	(ADAR300X_RAM_STATES_PER_BEAM * \
						 ADAR300X_IIO_BEAMS * \
						 ADAR300X_UNPACKED_BEAMSTATE_LEN)

	static uint8_t adar300x_iio_buffer[ADAR300X_IIO_BUFFER_SIZE];

	int ret;

	struct adar300x_iio_dev *adar300x_iio_dev;
	struct adar300x_iio_dev_init_param adar300x_iio_ip = {
		.adar300x_dev_init = &adar300x_ip,
	};

	struct iio_data_buffer write_buff = {
		.buff = adar300x_iio_buffer,
		.size = sizeof(adar300x_iio_buffer),
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = adar300x_iio_init(&adar300x_iio_dev, &adar300x_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adar3000",
			.dev = adar300x_iio_dev,
			.dev_descriptor = adar300x_iio_dev->iio_dev,
			.write_buff = &write_buff,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adar300x;

	return iio_app_run(app);
