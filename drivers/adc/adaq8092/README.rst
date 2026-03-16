ADAQ8092 no-OS Driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADAQ8092 <https://www.analog.com/ADAQ8092>`_

Overview
--------

The ADAQ8092 is a 14-bit, 105 MSPS, high-speed dual-channel data acquisition
(DAQ) uModule solution. The device incorporates signal conditioning, an
analog-to-digital (ADC) driver, a voltage reference, and an ADC in a single
package via system in package (SiP) technology. uModule solutions simplify the
development of high-speed data acquisition systems by transferring the design
burden, component selection, optimization, and layout from the designer to the
device. The ADAQ8092 enables a 6x footprint reduction compared to a discrete
solution, with a 7 mm x 6 mm, 72-ball CSP_BGA package.

Built-in power supply decoupling capacitors enhance power supply rejection
performance, making it a robust DAQ solution. The device operates on 3.3 V to
5 V analog and 1.8 V digital supplies. The digital outputs can be CMOS, double
data-rate CMOS, or double data-rate LVDS. The operating temperature range is
-40 C to +105 C.

Applications
------------

* Communications
* Cellular base stations
* GPS receivers
* Nondestructive testing
* Portable medical imaging
* Multichannel data acquisition

ADAQ8092 Device Configuration
-------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as 4 GPIOs: ``gpio_adc_pd1`` (ADC
power-down channel 1), ``gpio_adc_pd2`` (ADC power-down channel 2),
``gpio_en_1p8`` (1.8 V supply enable), and ``gpio_par_ser`` (parallel/serial
mode select).

The first API to be called is ``adaq8092_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization,
the driver performs the following power-up sequence:

1. All GPIOs are set to output low.
2. A 1000 ms delay allows the supplies to stabilize.
3. The 1.8 V supply is enabled (``gpio_en_1p8`` set high).
4. After a 500 ms delay, the ADC drivers are powered on (``gpio_adc_pd1`` and
   ``gpio_adc_pd2`` set high).
5. A software reset is issued via the SPI register interface.
6. All device configuration parameters from the init structure are applied.

The ``init_param`` structure is passed by value (not by pointer).

Power-Down Modes
~~~~~~~~~~~~~~~~

The ADAQ8092 supports multiple power-down modes configured via
``adaq8092_set_pd_mode``:

* ``ADAQ8092_NORMAL_OP``: Normal operation
* ``ADAQ8092_CH1_NORMAL_CH2_NAP``: Channel 1 normal, Channel 2 in nap mode
* ``ADAQ8092_CH1_CH2_NAP``: Both channels in nap mode
* ``ADAQ8092_SLEEP``: Sleep mode (entire device powered down)

In nap mode, the ADC core is powered down while the internal reference stays
active, allowing faster wake-up (at least 100 clock cycles). In sleep mode,
both the ADC core and internal reference are powered down, requiring more than
2 ms to recover.

Output Clock Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~

The output clock can be configured using three parameters:

**Clock Polarity** - Use ``adaq8092_set_clk_pol_mode`` to invert the output
clock polarity:

* ``ADAQ8092_CLK_POL_NORMAL``: Normal CLKOUT polarity
* ``ADAQ8092_CLK_POL_INVERTED``: Inverted CLKOUT polarity

**Clock Phase Delay** - Use ``adaq8092_set_clk_phase_mode`` to shift the output
clock relative to the data outputs. The clock duty-cycle stabilizer must be
enabled when using phase delay:

* ``ADAQ8092_NO_DELAY``: No delay
* ``ADAQ8092_CLKOUT_DELAY_45DEG``: 45 degree delay
* ``ADAQ8092_CLKOUT_DELAY_90DEG``: 90 degree delay
* ``ADAQ8092_CLKOUT_DELAY_180DEG``: 180 degree delay

**Clock Duty-Cycle Stabilizer** - Use ``adaq8092_set_clk_dc_mode`` to maintain
a constant 50% internal duty cycle when the encode duty cycle varies from 30%
to 70%:

* ``ADAQ8092_CLK_DC_STABILIZER_OFF``: Stabilizer disabled
* ``ADAQ8092_CLK_DC_STABILIZER_ON``: Stabilizer enabled

Digital Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Output Mode** - Use ``adaq8092_set_dout_mode`` to select the digital output
interface:

* ``ADAQ8092_FULL_RATE_CMOS``: Full rate CMOS output mode
* ``ADAQ8092_DOUBLE_RATE_LVDS``: Double data-rate LVDS output mode
* ``ADAQ8092_DOUBLE_RATE_CMOS``: Double data-rate CMOS output mode

**Output Enable** - Use ``adaq8092_set_dout_en`` to enable or disable the
digital outputs:

* ``ADAQ8092_DOUT_ON``: Digital outputs enabled
* ``ADAQ8092_DOUT_OFF``: Digital outputs disabled (high impedance)

**LVDS Output Current** - In LVDS mode, use ``adaq8092_set_lvds_cur_mode`` to
adjust the output driver current:

* ``ADAQ8092_3M5A``: 3.5 mA (default)
* ``ADAQ8092_4MA``: 4.0 mA
* ``ADAQ8092_4M5A``: 4.5 mA
* ``ADAQ8092_3MA``: 3.0 mA
* ``ADAQ8092_2M5A``: 2.5 mA
* ``ADAQ8092_2M1A``: 2.1 mA
* ``ADAQ8092_1M75``: 1.75 mA

**LVDS Internal Termination** - Use ``adaq8092_set_lvds_term_mode`` to enable
the optional internal 100 ohm termination resistor. When enabled, the output
driver current is doubled to maintain the same voltage swing:

* ``ADAQ8092_TERM_OFF``: Internal termination off
* ``ADAQ8092_TERM_ON``: Internal termination on

Data Format Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

**Twos Complement** - Use ``adaq8092_set_twos_comp`` to select the data output
format:

* ``ADAQ8092_OFFSET_BINARY``: Offset binary format (default)
* ``ADAQ8092_TWOS_COMPLEMENT``: Twos complement format

**Data Output Randomizer** - Use ``adaq8092_set_data_rand_en`` to reduce
unwanted tones caused by digital interference. The digital output is randomized
by applying an exclusive-OR operation between the LSB and all other data output
bits:

* ``ADAQ8092_DATA_RAND_OFF``: Randomizer disabled
* ``ADAQ8092_DATA_RAND_ON``: Randomizer enabled

**Alternate Bit Polarity** - Use ``adaq8092_set_alt_pol_en`` to reduce digital
noise by inverting odd-numbered output bits (D1, D3, D5, D7, D9, D11, D13):

* ``ADAQ8092_ALT_BIT_POL_OFF``: Alternate bit polarity disabled
* ``ADAQ8092_ALT_BIT_POL_ON``: Alternate bit polarity enabled

Digital Output Test Patterns
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use ``adaq8092_set_test_mode`` to verify the digital output interface by forcing
known output patterns:

* ``ADAQ8092_TEST_OFF``: Test patterns disabled
* ``ADAQ8092_TEST_ONES``: All outputs set to 1
* ``ADAQ8092_TEST_ZEROS``: All outputs set to 0
* ``ADAQ8092_TEST_CHECKERBOARD``: Alternating 10101... and 01010... pattern
* ``ADAQ8092_TEST_ALTERNATING``: Alternating all 0s and all 1s

ADAQ8092 Driver Initialization Example
----------------------------------------

.. code-block:: c

	struct adaq8092_dev *dev;
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

	struct no_os_gpio_init_param gpio_adc_pd1 = {
		.port = GPIO_PORT,
		.number = GPIO_ADC_PD1_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct no_os_gpio_init_param gpio_adc_pd2 = {
		.port = GPIO_PORT,
		.number = GPIO_ADC_PD2_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct no_os_gpio_init_param gpio_en_1p8 = {
		.port = GPIO_PORT,
		.number = GPIO_EN_1P8_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct no_os_gpio_init_param gpio_par_ser = {
		.port = GPIO_PORT,
		.number = GPIO_PAR_SER_PIN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct adaq8092_init_param init_param = {
		.spi_init = &spi_init,
		.gpio_adc_pd1_param = &gpio_adc_pd1,
		.gpio_adc_pd2_param = &gpio_adc_pd2,
		.gpio_en_1p8_param = &gpio_en_1p8,
		.gpio_par_ser_param = &gpio_par_ser,
		.pd_mode = ADAQ8092_NORMAL_OP,
		.clk_pol_mode = ADAQ8092_CLK_POL_NORMAL,
		.clk_phase_mode = ADAQ8092_NO_DELAY,
		.clk_dc_mode = ADAQ8092_CLK_DC_STABILIZER_OFF,
		.lvds_cur_mode = ADAQ8092_3M5A,
		.lvds_term_mode = ADAQ8092_TERM_OFF,
		.dout_en = ADAQ8092_DOUT_ON,
		.dout_mode = ADAQ8092_FULL_RATE_CMOS,
		.test_mode = ADAQ8092_TEST_OFF,
		.alt_bit_pol_en = ADAQ8092_ALT_BIT_POL_OFF,
		.data_rand_en = ADAQ8092_DATA_RAND_OFF,
		.twos_comp = ADAQ8092_OFFSET_BINARY,
	};

	ret = adaq8092_init(&dev, init_param);
	if (ret)
		goto error;
