ADF4377 No-OS Driver
====================

.. no-os-doxygen::

Supported Devices  
-----------------

* `ADF4377 <https://www.analog.com/en/products/adf4377.html>`_
* `ADF4378 <https://www.analog.com/en/products/adf4378.html>`_

Overview
--------

The ADF4377 is a high performance, ultralow jitter, dual output
integer-N phased locked loop (PLL) with an integrated voltage 
controlled oscillator (VCO) ideally suited for data converter and mixed
signal front end (MxFE) clock applications. The high performance
PLL has a figure of merit of −239 dBc/Hz, ultralow 1/f noise, and
a high phase frequency detector (PFD) frequency that can achieve
ultralow in-band noise and integrated jitter. The fundamental VCO
and output divider of the ADF4377 generate frequencies from 800
MHz to 12.8 GHz. The ADF4377 integrates all necessary power
supply bypass capacitors, saving board space on compact boards.

For multiple data converter and MxFE clock applications, the
ADF4377 simplifies clock alignment and calibration routines required 
with other clock solutions by implementing the automatic
reference to output synchronization feature, the matched reference
to output delays across process, voltage, and temperature feature,
and the less than ±0.1 ps, jitter free reference to output delay
adjustment capability feature.

These features allow for predictable and precise multichip clock and
system reference (SYSREF) alignment. JESD204B and JESD204C
Subclass 1 solutions are supported by pairing the ADF4377 with
an integrated circuit (IC) that distributes pairs of reference and
SYSREF signals.

Applications
------------
* High performance data converters
* Wireless infrastructure (MC-GSM, 5G)
* Test and measurement equipment

ADF4377 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **adf4377_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the ADF4377 by performing a soft reset, putting the device in a known
state. 

Reference Clock Configuration
-----------------------------

By default, the device is set to use a 125MHz clock. This can be changed to any
value, which is supported by the device, by using the function 
**adf4377_set_ref_clk**. The current configuration can be read out using
**adf4377_get_ref_clk**.The input reference clock value for the function will
have to be expressed in Hz.

Reference Doubler Configuration
-------------------------------

The ADF4377 has a reference doubler on the input path for the reference frequency. 
By default it is enabled and can be disabled using **adf4377_set_en_ref_doubler**. 
The current configuration can be read out using **adf4377_get_en_ref_doubler**.

This feature can only be enable or disabled.

Together with the reference divider this feature will determine the PFD frequency.

Reference Divider Configuration
-------------------------------

On the reference input path there is a divider, allowing the input reference
frequency to be divided by up to 63. Default this is set to 2 and can be changed
using **adf4377_set_ref_div**. The current configuration can be read out using
**adf4377_get_ref_div**.

Together with the reference doubler this feature will determine the PFD frequency.

Charge Pump Current Configuration
---------------------------------

The ADF4377 has the option for 16 preset charge pump currents, ranging from 0.79
mA to 11.1 mA. By default the charge pump current is set to 11.1 mA. The
supported currents can be configured using **adf4377_set_cp_i** API, which will
expect the register value, ranging form 0 to 15, as inputs. The current
configuration can be read out using **adf4377_get_cp_i**, which will also return
the value in the same range.

In order to determine which value corresponds to your design charge pump
current, please refer to the datasheet
`ADF4377 <https://www.analog.com/en/products/adf4377.html>`_ in the register details
section for register REG0015.

Bleed Current Configuration
---------------------------

The charge pump current can be optimized using the bleed current feature, set 
by default to 1023. This can be configured using the **adf4377_set_bleed_word**
API, by providing the bleed word as a parameter on 10 bits. The current
configuration can be read out using **adf4377_get_bleed_word**.

Output Frequency Configuration
------------------------------

The ADF4377 has two channels which can be configured to output the a frequency
in the range of 800 MHz to 12.8 GHZ. By default the output frequency is set to 
10 GHz and can be reconfigured by using the **adf4377_set_rfout** API. The
output frequency set by the API is expressed in Hz. The previously configured
output frequency can be read out using **adf4377_get_rfout**.

This attribute will set the frequency for both channels as they are not
independently configurable.

Output Power Configuration
--------------------------

Though the ADF4377 does not permit independent frequencies on each channel,
the output power on each channel can be configured using 
**adf4377_set_out_power** API by providing the channel and the desired power
output expressed in mV.The current configuration of each channel can be read 
out using **adf4377_get_out_power**.

Power of each channel is between 0 to 3, by default they are both set to 1 
(420mV).

Output Enable Configuration
---------------------------

Each channel can be activated individually using **adf4377_set_en_chan** API.
The API expects the channel number and a boolean value for enable or disable. To
determine if a channel is currently set the **adf4377_get_en_chan** API can be
used.

By default only channel 1 is enabled.

SYSREF Output Configuration
---------------------------
There are two distinct ways to configure the SYSREF output:

using the **adf4377_set_sr_del_adj** API, which will set the delay adjustment
value for the SYSREF signal incrementally. While **adf4377_get_sr_del_adj** will
return the current register value of the SYSREF delay.

using the **adf4377_set_sr_inv_adj** API, which will apply a constant delay
adjustment value for the SYSREF signal. While **adf4377_get_sr_inv_adj** will
return the current register value of the SYSREF delay.

The SYSREF monitoring feature can be enabled/disabled using **adf4377_set_en_sysref_monitor**
while the current configuration can be read out using **adf4377_get_en_sysref_monitor**.
This api will power up/down the SYSREF monitoring feature as well as toggle the reset
signal.

Delay Adjustment Configuration
-------------------------------

The ADF4377 has two delay adjustment features, one for the N delay and one for
the R delay. These can be configured using the **adf4377_set_rdel** and
**adf4377_set_ndel** APIs. The current configurations can be read out using **adf4377_get_rdel** and
**adf4377_get_ndel** APIs.

Appromitely, the R delay is used add delay to the output frequency phase, while the N delay
is used to remove delay from output frequency phase. 
Each RDEL bits represents a delay of ~1 ps and each NDEL bit represents a delay of ~1.2 ps.
Please refer to the datasheet
`ADF4377 <https://www.analog.com/en/products/adf4377.html>` for exact delay calculations.


ADF4377 Frequency Generation
----------------------------

Frequency Setting
-----------------

As mentioned above, using the default values the generated frequency will be of
10 GHz. This frequency depends on all of the previous configurations and is
recalculated based on them using the **adf4377_set_freq** API. When the API is
called, it will calculate the VCO, the output divider, the integer parts of feedback 
loop for the PLL while trying to obtain the configured output frequency. The API 
will also write the corresponding registers with the computed values.

Because of the link between the calculated values and the configurations, when
an attribute is reconfigured using the corresponding API it will not have any
effect until the **adf4377_set_freq** API is called.

ADF4377 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf4377_dev *dev;
	int ret;

	struct no_os_spi_init_param adf4377_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA
    };

    struct no_os_gpio_init_param gpio_ce_param = {
        .number = GPIO_CE,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_gpio_init_param gpio_enclk1_param = {
        .number = GPIO_ENCLK1,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_gpio_init_param gpio_enclk2_param = {
        .number = GPIO_ENCLK2,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_uart_init_param adf4377_uart_ip = {
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

    struct adf4377_init_param adf4377_ip = {
        .dev_id = ADF4377,
        .spi_init = &adf4377_spi_ip,
        .spi4wire = true,
        .gpio_ce_param = &gpio_ce_param,
        .gpio_enclk1_param = &gpio_enclk1_param,
        .gpio_enclk2_param = &gpio_enclk2_param,
        .clkin_freq = 125000000,
        .ref_doubler_en = 1,
        .f_clk = 10000000000,
        .ref_div_factor = 2,
        .clkout_op = ADF4377_CLKOUT_420MV,
        .muxout_select = ADF4377_MUXOUT_HIGH_Z,
        .cp_i = ADF4377_CP_10MA1,
    };

	ret = adf4377_init(&dev, &adf4377_ip);
	if (ret)
		goto error;
	ret = no_os_gpio_get(&arduino_gpio, &adf4377_gpio_ip);
	if (ret)
		goto error;
	ret = no_os_gpio_direction_output(arduino_gpio, 1);
	if (ret)
		goto error;

	ret = adf4377_set_rfout(dev, 10000000000);
	if (ret)
		goto error;

	return ret;

ADF4377 no-OS IIO support
-------------------------

The ADF4377 IIO driver comes on top of ADF4377 driver and offers support for
interfacing IIO clients through IIO lib.

ADF4377 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

While the ADF4377 has two channels these cannot output independent frequencies,
therefore most of the attributes will be device attributes.

The attributes are:

* bleed_current - is the adjustment value for the set charge pump current.
* charge_pump_current - is the current set in your design.
* charge_pump_current_available - lists the available and predefined charge
                                  pump currents of the ADF4377.
* reference_divider - is the current value of the input divider.
* rfout_divider - sets the current value of the output divider.
* rfout_divider_available - lists the available and predefined output dividers
                            of the ADF4377.
* reference_doubler_en - enables the input doubler.
* n_del - is the current value of the N delay.
* r_del - is the current value of the R delay.
* reference_frequency - is the current set input frequency.
* sysref_delay_adjust - is the delay adjustment value for the SYSREF signal.
* sysref_invert_adjust - is a constant delay adjustment value for the SYSREF signal.
* sysref_monitoring - enables the SYSREF monitoring feature.
* default_register : Loads the default register values for eval board configuration

Device Channels
---------------

ADF4377 IIO device has 2 output channels which can have independent output
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

ADF4377 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

    struct adf4377_iio_dev *adf4377_iio_dev;
	struct adf4377_iio_dev_init_param adf4377_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

    struct no_os_spi_init_param adf4377_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = 2000000,
	.chip_select = SPI_CS,
	.mode = NO_OS_SPI_MODE_0,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = SPI_OPS,
	.extra = SPI_EXTRA
    };

    struct no_os_gpio_init_param gpio_ce_param = {
        .number = GPIO_CE,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_gpio_init_param gpio_enclk1_param = {
        .number = GPIO_ENCLK1,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_gpio_init_param gpio_enclk2_param = {
        .number = GPIO_ENCLK2,
        .platform_ops = GPIO_OPS,
        .extra = GPIO_EXTRA
    };

    struct no_os_uart_init_param adf4377_uart_ip = {
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

    struct adf4377_init_param adf4377_ip = {
        .dev_id = ADF4377,
        .spi_init = &adf4377_spi_ip,
        .spi4wire = true,
        .gpio_ce_param = &gpio_ce_param,
        .gpio_enclk1_param = &gpio_enclk1_param,
        .gpio_enclk2_param = &gpio_enclk2_param,
        .clkin_freq = 125000000,
        .ref_doubler_en = 1,
        .f_clk = 10000000000,
        .ref_div_factor = 2,
        .clkout_op = ADF4377_CLKOUT_420MV,
        .muxout_select = ADF4377_MUXOUT_HIGH_Z,
        .cp_i = ADF4377_CP_10MA1,
    };

    adf4377_iio_ip.adf4377_dev_init = &adf4377_ip;
	ret = adf4377_iio_init(&adf4377_iio_dev, &adf4377_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4377",
			.dev = adf4377_iio_dev,
			.dev_descriptor = adf4377_iio_dev->iio_dev,
		}
	};

    app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4377_uart_ip;
	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;
        
	iio_app_run(app);