ADF4030 no-OS driver
====================

Supported Devices
-----------------

`ADF4030 <https://www.analog.com/en/products/adf4030.html>`_

Overview
--------

The `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_ provides for 10 bidirectional synchronized clock
(BSYNC) channels and accepts a reference clock input (REFIN)
signal as a frequency reference for generating an output clock
on any BSYNC channels that are configured as an output. The
hallmark feature of the `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_ is the ability to time align the clock
edges of any one or more BSYNC channels to <5 ps (at the device
pins) with respect to the BSYNC channel selected as the reference
BSYNC channel.
The `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_  is well adapted for multiple connections with other
`ADF4030 <https://www.analog.com/en/products/adf4030.html>`_  devices for synchronizing clock signals in a system. Each
BSYNC is bidirectional, allowing for reversing the direction of the
clock signal to measure the propagation delay of the transmission
medium. Round trip constructions that use replica paths are also
supported. The bidirectional nature of the round trip delay measurement greatly reduces the error in determining the propagation delay
through the BSYNC transmission medium as compared to using
a replica path. This feature makes the `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_  capable to time
align the clock edges of BSYNC channels across multiple `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_ 
devices, independent of the tree or cascade architecture in which
the `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_  system is designed. The benefits of bidirectional
clocking extend to devices other than the `ADF4030 <https://www.analog.com/en/products/adf4030.html>`_  (assuming
those devices support bidirectional clock exchanges).


Applications
------------

* 5G timing transport high precision synchronization
* Phased array radar
* Automatic test equipment (ATE) pin electronics
* JESD204B/JESD204C support for analog-to-digital converter (ADC) and digital-to-analog converter (DAC) clocking


ADF4030 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **adf4030_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the ADF4030 by performing a soft reset, putting the device in a known
state. 


Reference Clock Configuration
-----------------------------

By default, the device is set to use a 125MHz clock. This can be changed to any
value, which is supported by the device, by using the function 
**adf4030_set_ref_clk**. The current configuration can be read out using
**adf4030_get_ref_clk**.The input reference clock value for the function will
have to be expressed in Hz.

Reference Divider Configuration
-------------------------------

On the reference input path there is a divider, allowing the input reference
frequency to be divided by up to 31. Default this is set to 1 and can be changed
using **adf4030_set_ref_div**. The current configuration can be read out using 
**adf4030_get_ref_div**.

VCO Configuration
------------------

The VCO frequency can be set using the function **adf4030_set_vco_freq**. The
current frequency can be read out using **adf4030_get_vco_freq**. The input
VCO frequency value for the function will have to be expressed in Hz. The
frequency range is between 2.375 GHz and 2.625 GHz.

BSYNC Frequency Configuration
------------------------------

The BSYNC channels frequencies can be set using the function
**adf4030_set_bsync_freq**. The current frequency can be read out using
**adf4030_get_bsync_freq**. The input BSYNC frequency value for the function
will have to be expressed in Hz. The frequency range is between 650 kHz and
200 MHz. There are 2 different output divider for the BSYNC channels. Output dividers
can be selected with the argument **odivb_sel** in the function
**adf4030_set_bsync_freq**. 

TDC Configuration
------------------

The TDC source channel can be set using the function **adf4030_set_tdc_source**. The 
current TDC source channel can be read out using **adf4030_get_tdc_source**. TDC source 
needs to be set before performing TDC measurement or alignments.

The TDC measurement can be performed using the function **adf4030_set_tdc_measurement**. 
Function takes TDC target channel as argument. The TDC measurement will be triggered with this function.
TDC Result can be read out using the function **adf4030_get_tdc_result**. This architecture has 
designed to support the paralilism idea for the multiple Aion systems.

Alignment Configuration
------------------------

The alignment source channel can be set using the function **adf4030_set_tdc_source**. The
current alignment source channel can be read out using **adf4030_get_tdc_source**. Alignment source
needs to be set before performing TDC measurement or alignments.

The single channel alignment can be performed using the function **adf4030_set_single_ch_alignment**.
Function takes the target channel as argument. Alignemnt will be triggered with this function. 
The alignment result can be read out using the function **adf4030_set_tdc_measurement** and 
**adf4030_get_tdc_result** functions.  

The number of iterations for the one alignment can be set using the function
**adf4030_set_alignment_iter**. The current number of iterations can be read out using
**adf4030_get_alignment_iter**. 

The threshold for the alignment can be set using the function **adf4030_set_alignment_threshold**. The
current threshold can be read out using **adf4030_get_alignment_threshold**. If the iteration number is
set previously, the alignment will be continued until the threshold is reached or the number of
iterations is reached.

BSYNC Delay Configuration
-------------------------

The BSYNC channels can be configured to have a delay using the function **adf4030_set_channel_delay**.
The current delay can be read out using **adf4030_get_channel_delay**. The input and output delay 
value is represented in femto-seconds (fs). Delay will be applied after an alignment.


BSYNC Direction Configuration
------------------------------

The BSYNC channels can be configured to be inputs or outputs using
**adf4030_set_channel_direction**. The current configuration can be read out using
**adf4030_get_channel_direction**. The BSYNC channels can be configured to be

BSYNC Termination Configuration
--------------------------------

The BSYNC channels can be configured to have a termination using
**adf4030_set_channel_termination**. The current configuration can be read out using    
**adf4030_get_channel_termination**. Terminations can be set for RX and TX channels.
**adf4030_terminations_e** enumeration is used as argument to set the termination.

There are 3 terminations available for Tx Channels:
* Voltage Driver
* Current Driver Unterminated
* Current Driver Terminated

There are 3 terminations available for Rx Channels:
* DC coupled clocks
* AC coupled clocks
* DC coupled high speed current steering logic (HCSL) clocks

BSYNC Output PRBS Configuration
--------------------------------

The BSYNC channels output a gapped periodic clock (GPC) signal. The PRBS generator produces a 
pseudorandom sequence of ones and zeros at the same rate as the ODIV divider
output clock signal. The BSYNC channels can be configured to have a PRBS using
**adf4030_set_channel_prbs**. The current configuration can be read out using
**adf4030_get_channel_prbs**. 

BSYNC Output Divider Configuration
----------------------------------

The BSYNC channels can be configured to select odiva or odivb for the output divider.
Channels' ouput divider can be set using the function **adf4030_set_channel_odivb**. The current configuration can be read out using
**adf4030_get_channel_odivb**. The output divider can be set for each channel separately.


BSYNC Output Inverter Configuration
------------------------------------

The BYNC channels can be configured to have an inverter using
**adf4030_set_channel_inverter**. The current configuration can be read out using
**adf4030_get_channel_inverter**. This will add channel output to 180 degrees analog phase shift.

BSYNC Output Voltage Level Configuration 
------------------------------------------

The BSYNC channels can be configured to have a voltage level using
**adf4030_set_channel_voltage**. The current configuration can be read out using
**adf4030_get_channel_voltage**. The voltage level can be set for each channel separately.
The voltage level can be set between 0.504 V to 1.304 V.



ADF4030 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf4030_dev *dev;
	int ret;

    struct no_os_uart_init_param adf4030_uart_ip = {
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

    struct no_os_spi_init_param adf4030_spi_ip = {
        .device_id = SPI_DEVICE_ID,
        .max_speed_hz = 3000000,
        .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
        .mode = NO_OS_SPI_MODE_0,
        .platform_ops = SPI_OPS,
        .chip_select = SPI_CS,
        .extra = SPI_EXTRA,
    };

    struct adf4030_init_param adf4030_ip = {
        .spi_init = &adf4030_spi_ip,
        .spi_4wire_en = true,
        .cmos_3v3 = false,
        .ref_freq = 125000000,
        .vco_freq = 2500000000,
        .bsync_freq = 100000000,
        .ref_div = 1,
        .chip_addr = 0,
    };

	ret = adf4030_init(&dev, &adf4030_ip);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_channel_direction(dev, 2, true);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_source(dev, 1);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_channel_delay(dev, 2, 200000);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_single_ch_alignment(dev, 2);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_measurement(dev, 2);
	if (ret)
		goto remove_adf4030;

	int64_t tdc_res;
	ret = adf4030_get_tdc_measurement(dev, &tdc_res);
	if (ret)
		goto remove_adf4030;

	pr_info("tdc_res : %lld\n", tdc_res);

ADF4030 no-OS IIO support
-------------------------

The ADF4030 IIO driver comes on top of ADF4030 driver and offers support for
interfacing IIO clients through IIO lib.

ADF4030 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

Device attributes are used to configure the ADF4030 device. These attributes
affects all channels. The device attributes are:

* default_register : Loads the default register values for eval board configuration
* chip_address : Chip address for the ADF4030 device 
* reference_frequency : Reference frequency for the ADF4030 device
* vco_frequency : VCO frequency for the ADF4030 device
* bsync_freq_odiva : BSYNC frequency for the channels assigned to odiva
* bsync_freq_odivb : BSYNC frequency for the channels assigned to odivb
* tdc_source_ch : TDC source channel
* tdc_measurement : TDC measurement with given TDC source and argument TDC target
* alignment_iter : Number of iterations for the alignment
* alignment_threshold : Threshold for the alignment
* single_ch_alignment : Single channel alignment with given target channel
* serial_alignment : Serial alignment with given channel flags
* background_serial_alignment : Background serial alignment with given channel flags
* temperature : Temperature of the ADF4030 

Device Channels
---------------

ADF4030 IIO device has 10 BSYNC channels.

* outpu/input bsync0 - corresponding to BSYNC channel 0 on the device
* outpu/input bsync1 - corresponding to BSYNC channel 1 on the device
* outpu/input bsync2 - corresponding to BSYNC channel 2 on the device
* outpu/input bsync3 - corresponding to BSYNC channel 3 on the device
* outpu/input bsync4 - corresponding to BSYNC channel 4 on the device
* outpu/input bsync5 - corresponding to BSYNC channel 5 on the device
* outpu/input bsync6 - corresponding to BSYNC channel 6 on the device
* outpu/input bsync7 - corresponding to BSYNC channel 7 on the device
* outpu/input bsync8 - corresponding to BSYNC channel 8 on the device   
* outpu/input bsync9 - corresponding to BSYNC channel 9 on the device

Each channel has 7 independent attributes:
* direction : Channel direction (input/output)
* delay : Channel delay 
* odivb_en : Channel output divider (odiva/odivb)
* termination : Channel termination 
* prbs : Channel PRBS (enabled/disabled)
* inverter : Channel inverter (enabled/disabled)
* voltage : Channel voltage level (0.504V to 1.304V)


ADF4030 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	struct adf4030_iio_dev *adf4030_iio_dev;
	struct adf4030_iio_dev_init_param adf4030_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	struct no_os_uart_init_param adf4030_uart_ip = {
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

	struct no_os_spi_init_param adf4030_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct adf4030_init_param adf4030_ip = {
		.spi_init = &adf4030_spi_ip,
		.spi_4wire_en = true,
		.cmos_3v3 = false,
		.ref_freq_hz = 125000000,
		.freq = 8000000000,
		.ref_doubler_en = 1,
		.ref_div = 1,
		.cp_i = 14,
		.ld_count = 12,
	};

	adf4030_iio_ip.adf4030_dev_init = &adf4030_ip;
	ret = adf4030_iio_init(&adf4030_iio_dev, &adf4030_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf4030",
			.dev = adf4030_iio_dev,
			.dev_descriptor = adf4030_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf4030_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
