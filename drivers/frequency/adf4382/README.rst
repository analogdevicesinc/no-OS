ADF4382 no-OS driver
====================

Supported Devices
-----------------

`ADF4382 <www.analog.com/en/products/adf4382a.html>`_

Overview
--------

The `ADF4382 <www.analog.com/en/products/adf4382a.html>`_ is a high performance,
ultralow jitter, Frac-N PLL with integrated VCO ideally suited for LO generation
for 5G applications or data converter clock applications. The high performance
PLL has a figure of merit of -238 dBc/Hz, low 1/f Noise and high PFD frequency
of 625MHz in integer mode that can achieve ultra-low in-band noise and
integrated jitter. The ADF4382 can generate  frequencies from 687.5 MHz to
22 GHz without an external doubler, thereby  eliminating the need for
sub-harmonic filters.

For multiple data converter clock applications, the
`ADF4382 <www.analog.com/en/products/adf4382a.html>`_ automatically aligns its
output to the input reference edge by including the output divider in the PLL
feedback loop. For applications that require deterministic delay or delay
adjustment capability, a programmable reference to output delay with <1ps
resolution is provided. The reference to output delay matching across multiple
devices and over temperature, allows for predictable and precise multi-chip
clock and SYSREF alignment.

Applications
------------

* High Performance Data Converter Clocking
* Wireless infrastructure (MC-GSM, 5G, 6G)
* Test and Measurement

ADF4382 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **adf4382_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the ADF4382 by performing a soft reset, putting the device in a known
state. 

Reference Clock Configuration
-----------------------------

By default, the device is set to use a 125MHz clock. This can be changed to any
value, which is supported by the device, by using the function 
**adf4382_set_ref_clk**. The current configuration can be read out using
**adf4382_get_ref_clk**.The input reference clock value for the function will
have to be expressed in KHz.

Reference Doubler Configuration
-------------------------------

The ADF4382 uses has a reference doubler on the input path for the reference
frequency. By default it is enabled and can be disabled using 
**adf4382_set_en_ref_doubler**. The current configuration can be read out using
**adf4382_get_en_ref_doubler**.

This feature can only be enable or disabled.

Together with the reference divider this feature will determine the PFD 
requency.

Reference Divider Configuration
-------------------------------

On the reference input path there is a divider, allowing the input reference
frequency to be divided by up to 64. Default this is set to 1 and can be changed
using **adf4382_set_ref_div**. The current configuration can be read out using
**adf4382_get_ref_div**.

Together with the reference doubler this feature will determine the PFD frequency.

Charge Pump Current Configuration
---------------------------------

The ADF4382 has the option for 16 preset charge pump currents, ranging from 0.7
mA to 11.1 mA. by default the charge pump current is set to 11.1 mA. The
supported currents can be configured using **adf4382_set_cp_i** API, which will
expect the register value, ranging form 0 to 15, as inputs. The current
configuration can be read out using **adf4382_set_cp_i**, which will also return
the value in the same range.

In order to determine which value corresponds to your design charge pump
current, please refer to the datasheet
`ADF4382 <www.analog.com/en/products/adf4382a.html>`_ in the register details
section for register REG001F.

Bleed Current Configuration
---------------------------

The charge pump current can be optimized using the bleed current feature, set 
by default to 4903. This can be configured using the **adf4382_set_bleed_word**
API, by providing the bleed word as a parameter on 13 bits. The current
configuration can be read out using **adf4382_get_bleed_word**.

The bleed word is composed of the coarse bleed current, which is the upper 4
bits of the bleed word, and the fine bleed current representing the lower 9 bits
of the bleed word. 

Output Frequency Configuration
------------------------------

The ADF4382 has two channels which can be configured to output the a frequency
in the range of 687.5 MHz to 22 GHZ. By default the output frequency is set to 
2.8 GHz and can be reconfigured by using the **adf4382_set_rfout** API. The
output frequency set by the API is expressed in KHz. The previously configured
output frequency can be read out using **adf4382_get_rfout**.

This attribute will set the frequency for both channels as they are not
independently configurable.

Output Power Configuration
--------------------------

Even though the ADF4382 does not permit independent frequencies on each channel,
the output power on each channel can be configured using
**adf4382_set_out_power** API by providing the channel and the desired power
output.The current configuration of each channel can be read out using
**adf4382_get_out_power**.

Power of each channel is between 0 to 15 with 0 and by default they are both set
to 9.

Output Enable Configuration
---------------------------

Each channel can be activated individually using **adf4382_set_en_chan** API.
The API expects the channel number and a boolean value for enable or disable. To
determine if a channel is currently set the **adf4382_get_en_chan** API can be
used.

By default only channel 1 is enabled.

Phase Adjustment Configuration
------------------------------

The phase of the generated output frequency can be adjusted using
**adf4382_set_phase_adjust**. This will skew the phase of the requested
frequency by the desired amount in pico-seconds. The API will automatically
transform the input value from a time adjustment to a phase adjustment relative
to the set output frequency. If the phase exceeds 360 degrees, it will apply
only the overshoot.

By default there is no phase adjustment set.

When applying the phase adjust, this can add or subtract the desired amount of
the phase using **adf4382_set_phase_pol**. It expects a boolean value, 0 for
positive polarity and 1 for negative polarity.

To determine the current phase adjustment and polarity,
**adf4382_get_phase_adjust** and **adf4382_get_phase_pol** should be used.

By default the polarity is positive.

Synchronization Enable Configuration
------------------------------------

The ADF4382 has a synchronization feature wherein Both RF Output signals are
synchronized to an input signal at this pin. It is used for multi-chip phase
synchronization. This can be enabled or disabled using the
**adf4382_set_en_sync** API.

By default it is disabled.

ADF4382 Frequency Generation
----------------------------

Frequency Setting
-----------------

As mentioned above, using the default values the generated frequency will be of
2.8 GHz. This frequency depends on all of the previous configurations and is
recalculated based on them using the **adf4382_set_freq** API. When the API is
called, it will calculate the VCO, the output divider, the integer and
fractional parts of feedback loop for the PLL while trying to obtain the
configured output frequency. The API will also write the corresponding registers
with the computed values.

Because of the link between the calculated values and the configurations, when
an attribute is reconfigured using the corresponding API it will not have any
effect until the **adf4382_set_freq** API is called.

ADF4382 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf4382_dev *dev;
	int ret;

	struct no_os_uart_init_param adf4382_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
		.platform_ops = UART_OPS,
	};

	struct no_os_spi_init_param adf4382_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4382_init_param adf4382_ip = {
		.spi_init = &adf4382_spi_ip,
		.spi_3wire_en = false,
		.cmos_3v3 = false,
		.ref_freq_hz = 125000000,
		.freq = 2800000000,
		.ref_doubler_en = 1,
		.ref_div = 1,
		.cp_i = 15,
		.bleed_word = 4903,
		.ld_count = 10,
		.adf4382a = true,
	};

	ret = adf4382_init(&dev, &adf4382_ip);
 	if (ret)
 		goto error;

 	ret = adf4382_set_freq(dev, 20000000000);
 	if (ret)
 		goto error;

 	ret = adf4382_set_phase_adjust(dev, 10);
 	if (ret)
 		goto error;

ADF4382 no-OS IIO support
-------------------------

The ADF4382 IIO driver comes on top of ADF4382 driver and offers support for
interfacing IIO clients through IIO lib.

ADF4382 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

While the ADF4382 has two channels these cannot output independent frequencies,
therefor most of the attributes will be device attributes.

The attributes are:

* bleed_current - is the adjustment value for the set charge pump current.
* charge_pump_current - is the current set in your design.
* charge_pump_current_available - lists the available and predefined charge
                                  pump currents of the ADF4382.
* reference_divider - is the current value of the input divider.
* reference_doubler_en - enables the input doubler.
* reference_frequency - is the current set input frequency.
* sync_en - enables the synchronization feature to an external signal.

Device Channels
---------------

ADXL355 IIO device has 2 output channels which can have independent output
powers.

The channels are:

* output altvoltage0 - corresponding to channel 1 on the device
* output altvoltage1 - corresponding to channel 2 on the device

Each channel has 2 individual attributes:

* en - enables the channel. 
* output_power - determines the output power of the channel between 0 and 15.

Each channel has 2 common attributes:

* frequency - is the desired output frequency which the driver will try
              to obtain given the configuration.
* phase - is the current phase adjustment of the frequency. The phase can be
          subtracted by placing the "-" in fornt of the introduced phase value.

ADF4382 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	struct adf4382_iio_dev *adf4382_iio_dev;
	struct adf4382_iio_dev_init_param adf4382_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	struct no_os_uart_init_param adf4382_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = UART_EXTRA,
		.platform_ops = UART_OPS,
	};

	struct no_os_spi_init_param adf4382_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 1000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4382_init_param adf4382_ip = {
		.spi_init = &adf4382_spi_ip,
		.spi_3wire_en = false,
		.cmos_3v3 = false,
		.ref_freq_hz = 125000000,
		.freq = 2800000000,
		.ref_doubler_en = 1,
		.ref_div = 1,
		.cp_i = 15,
		.bleed_word = 4903,
		.ld_count = 10,
		.adf4382a = true,
	};

	adf4382_iio_ip.adf4382_dev_init = &adf4382_ip;
	ret = adf4382_iio_init(&adf4382_iio_dev, &adf4382_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4382",
			.dev = adf4382_iio_dev,
			.dev_descriptor = adf4382_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4382_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
