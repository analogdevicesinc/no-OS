ADF4368 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADF4368 <www.analog.com/en/products/adf4368.html>`_

Overview
--------


The `ADF4368 <www.analog.com/en/products/adf4368.html>`_ is a 
high performance, ultra-low jitter, integer-N and
fractional-N phase-locked loop (PLL) with integrated VCO ideally
suited for frequency conversion applications.
The high performance PLL has a figure of merit of -239 dBc/Hz,
very low 1/f noise of normalized -287 dBc/Hz and high PFD
frequency that can achieve ultra-low in-band noise and integrated
jitter. The `ADF4368 <www.analog.com/en/products/adf4368.html>`_ 
can generate any frequency from 800 MHz to 12.8 GHz without an 
internal doubler, which eliminates the need for sub-harmonic filters. 
The Σ-Δ modulator includes a 25-bit fixed modulus that allows 
hertz frequency resolution and an additional 17-bit variable modulus,
which allows even finer resolution and flexibility for frequency 
planning. The 9 dBm output power at 12.8 GHz in single-ended 
configuration with 16 step power adjust feature makes it very 
useful for any application.

For multiple frequency conversion applications, such as phase
array radar or massive MIMO systems, the outputs of multiple
`ADF4368 <www.analog.com/en/products/adf4368.html>`_ can 
be aligned by using the SYNC input or EZSync™. The EZSync method is 
used when it is difficult to distribute the SYNC signal to all 
devices precisely. For applications that require deterministic 
delay or delay adjustment capability, a programmable reference 
to output delay with <1 ps resolution is provided. The
reference to output delay is guaranteed across multiple devices
and temperature, allowing for predictable and precise multichip
alignment.


Applications
------------

* High Performance Data Converter Clocking
* Wireless infrastructure (MC-GSM, 5G, 6G)
* Test and Measurement

ADF4368 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **adf4368_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the ADF4368 by performing a soft reset, putting the device in a known
state. 

Reference Clock Configuration
-----------------------------

By default, the device is set to use a 125MHz clock. This can be changed to any
value, which is supported by the device, by using the function 
**adf4368_set_ref_clk**. The current configuration can be read out using
**adf4368_get_ref_clk**.The input reference clock value for the function will
have to be expressed in KHz.

Reference Doubler Configuration
-------------------------------

The ADF4368 uses has a reference doubler on the input path for the reference
frequency. By default it is enabled and can be disabled using 
**adf4368_set_en_ref_doubler**. The current configuration can be read out using
**adf4368_get_en_ref_doubler**.

This feature can only be enable or disabled.

Together with the reference divider this feature will determine the PFD 
requency.

Reference Divider Configuration
-------------------------------

On the reference input path there is a divider, allowing the input reference
frequency to be divided by up to 63. Default this is set to 1 and can be changed
using **adf4368_set_ref_div**. The current configuration can be read out using
**adf4368_get_ref_div**.

Together with the reference doubler this feature will determine the PFD frequency.

Charge Pump Current Configuration
---------------------------------

The ADF4368 has the option for 16 preset charge pump currents, ranging from 0.79
mA to 11.1 mA. by default the charge pump current is set to 11.1 mA. The
supported currents can be configured using **adf4368_set_cp_i** API, which will
expect the register value, ranging form 0 to 15, as inputs. The current
configuration can be read out using **adf4368_set_cp_i**, which will also return
the value in the same range.

In order to determine which value corresponds to your design charge pump
current, please refer to the datasheet
`ADF4368 <www.analog.com/en/products/adf4368.html>`_ in the register details
section for register REG001F.

Bleed Current Configuration
---------------------------

The charge pump current can be optimized using the bleed current feature.
This can be configured using the **adf4368_set_bleed_word**
API, by providing the bleed word as a parameter on 13 bits. The current
configuration can be read out using **adf4368_get_bleed_word**.

The bleed word is composed of the coarse bleed current, which is the upper 4
bits of the bleed word, and the fine bleed current representing the lower 9 bits
of the bleed word. 

Output Frequency Configuration
------------------------------

The ADF4368 has two channels which can be configured to output the a frequency
in the range of 800 MHz to 12.8 GHZ. Output frequency can be reconfigured by 
using the **adf4368_set_rfout** API. The output frequency set by the API 
is expressed in KHz. The previously configured output frequency can be read 
out using **adf4368_get_rfout**.

This attribute will set the frequency for both channels as they are not
independently configurable.

Output Power Configuration
--------------------------

Even though the ADF4368 does not permit independent frequencies on each channel,
the output power on each channel can be configured using
**adf4368_set_out_power** API by providing the channel and the desired power
output.The current configuration of each channel can be read out using
**adf4368_get_out_power**.

Power of each channel is between 0 to 15 with 0 and by default they are both set
to 9.

Output Enable Configuration
---------------------------

Each channel can be activated individually using **adf4368_set_en_chan** API.
The API expects the channel number and a boolean value for enable or disable. To
determine if a channel is currently set the **adf4368_get_en_chan** API can be
used.

By default both channels is enabled.

Phase Adjustment Configuration
------------------------------

The phase of the generated output frequency can be adjusted using
**adf4368_set_phase**. This will skew the phase of the requested
frequency by the desired amount in femto-seconds. The API will automatically
transform the input value from a time adjustment to a phase adjustment relative
to the set output frequency. 

In integer mode **adf4368_set_phase** API chooses the Bleed Current phase 
adjusment method with **adf4368_set_phase_bleedi** API and in fractional mode API chooses the Sigma-Delta Modulation 
phase adjustment method with **adf4368_set_phase_sdm** API.

By default there is no phase adjustment set.

Synchronization Configuration
------------------------------------

The ADF4368 has a synchronization feature wherein Both RF Output signals are
synchronized to an input signal at this pin. It is used for multi-chip phase
synchronization. This can be enabled or disabled using the
**adf4368_set_sync_setup** API. There are 2 methods for synchronization both 
EZSync and Timed Sync supported by this function.

EZSync require a reset signal. Reset can be applied with **adf4368_set_sw_sync** API.

By default synchronization feature is disabled.

Temperature Readback Configuration
------------------------------------

The ADF4368 has a temperature feature which measure the die temperature. 
The purpose of the temperature sensor is to measure changes in the
die temperature and not the absolute junction temperature.
**adf4368_set_temperature** API sets the necessary ADC configuration.
**adf4368_get_temperature** API readback the die temperature from device.

By default it is disabled.

ADF4368 Frequency Generation
----------------------------

Frequency Setting
-----------------

As mentioned above, using the default values the generated frequency will be of
8 GHz. This frequency depends on all of the previous configurations and is
recalculated based on them using the **adf4368_set_freq** API. When the API is
called, it will calculate the VCO, the output divider, the integer and
fractional parts of feedback loop for the PLL while trying to obtain the
configured output frequency. The API will also write the corresponding registers
with the computed values.

ADF4368 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf4368_dev *dev;
	int ret;

	struct no_os_uart_init_param adf4368_uart_ip = {
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

	struct no_os_spi_init_param adf4368_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4368_init_param adf4368_ip = {
		.spi_init = &adf4368_spi_ip,
		.spi_4wire_en = true,
		.cmos_3v3 = false,
		.ref_freq_hz = 125000000,
		.freq = 8000000000,
		.ref_doubler_en = 1,
		.ref_div = 1,
		.cp_i = 14,
		.ld_count = 12,
	};


	ret = adf4368_init(&dev, &adf4368_ip);
 	if (ret)
 		goto error;

 	ret = adf4368_set_freq(dev, 8000000000);
 	if (ret)
 		goto error;


ADF4368 no-OS IIO support
-------------------------

The ADF4368 IIO driver comes on top of ADF4368 driver and offers support for
interfacing IIO clients through IIO lib.

ADF4368 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

While the ADF4368 has two channels these cannot output independent frequencies,
therefor most of the attributes will be device attributes.

The attributes are:

* bleed_current - is the adjustment value for the set charge pump current.
* charge_pump_current - is the current set in your design.
* charge_pump_current_available - lists the available and predefined charge
                                  pump currents of the ADF4368.
* default_regs - writes the default registers and sets the device 4 SPI mode bit.
* reference_divider - is the current value of the input divider.
* reference_doubler_en - enables the input doubler.
* reference_frequency - is the current set input frequency.
* sync_setup - enables the synchronization feature to an external signal.
* temperature - sets temperature setup and reads the die temperature value.
* toggle_swsync - toggles the swsync bit for EZSYNC synchronization feature.

Device Channels
---------------

ADF4368 IIO device has 2 output channels which can have independent output
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

ADF4368 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	struct adf4368_iio_dev *adf4368_iio_dev;
	struct adf4368_iio_dev_init_param adf4368_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	struct no_os_uart_init_param adf4368_uart_ip = {
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

	struct no_os_spi_init_param adf4368_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4368_init_param adf4368_ip = {
		.spi_init = &adf4368_spi_ip,
		.spi_4wire_en = true,
		.cmos_3v3 = false,
		.ref_freq_hz = 125000000,
		.freq = 8000000000,
		.ref_doubler_en = 1,
		.ref_div = 1,
		.cp_i = 14,
		.ld_count = 12,
	};

	adf4368_iio_ip.adf4368_dev_init = &adf4368_ip;
	ret = adf4368_iio_init(&adf4368_iio_dev, &adf4368_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4368",
			.dev = adf4368_iio_dev,
			.dev_descriptor = adf4368_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4368_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
