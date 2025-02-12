ADF5611 no-OS driver
====================

Supported Devices
-----------------

* `ADF5611 <www.analog.com/en/products/adf5611.html>`_
* `ADF5612 <www.analog.com/en/products/adf5612.html>`_

Overview
--------

The `ADF5611 <www.analog.com/en/products/adf5611.html>`_ and `ADF5612 <www.analog.com/en/products/adf5612.html>`_ 
allows implementation of fractional-N or Integer N phase-locked loop (PLL) frequency 
synthesizers when used with an external loop filter and an external reference source. 
The wideband microwave voltage controlled oscillator (VCO) design permits frequency operation 
from 7300 MHz to 14600 MHz for `ADF5611 <www.analog.com/en/products/adf5611.html>`_ and from 7300 
MHz to 8500 MHz for `ADF5612 <www.analog.com/en/products/adf5612.html>`_ at a single radio 
frequency (RF) output. A series of frequency dividers with a differential frequency output 
allows operation from 57 MHz to 14600MHz for `ADF5611 <www.analog.com/en/products/adf5611.html>`_, 
while `ADF5612 <www.analog.com/en/products/adf5612.html>`_ is limited to 57MHz to 8500MHz. 
Analog and digital power supplies for the PLL circuitry range from 3.15 V to 3.45 V, and 
the VCO supplies are between 4.75 V and 5.25 V.
Both `ADF5611 <www.analog.com/en/products/adf5611.html>`_ and `ADF5612 <www.analog.com/en/products/adf5612.html>`_ 
has an integrated VCO with a fundamental frequency of 3650 MHz to 7300 MHz. These frequencies 
are internally doubled and routed to the RFOUT pin. An additional differential output allows 
the doubled VCO frequency to be divided by 1, 2, 4, 8, 16, 32, 64, or 128, allowing the user 
to generate RF output frequencies as low as 57 MHz. A simple 3 or 4 wire serial port interface 
(SPI) provides control of all on-chip registers. To conserve power, this divider block can be disabled 
when not needed through the SPI interface. Likewise, the output power for both the single-
ended output and the differential output are programmable. Their integrated phase detector 
(PD) and delta-sigma (Δ-Σ) modulator, capable of operating at up to 100 MHz, permit wide 
loop bandwidths and fast frequency tuning with a typical spurious level of −100 dBc. 
With phase noise levels from −115 dBc/Hz at 7.3 GHz to −109 dBc/Hz at 14.6 GHz and −112 dBc/Hz 
at 8.5 GHz RFOUT respectively, both synthesizers are equipped to minimize blocker effects, 
and to improve receiver sensitivity and transmitter spectral purity. The low phase noise floor 
eliminates any contribution to modulator and mixer noise floor in transmitter applications.

Applications
------------

* Military and Defense
* Test equipment
* Clock generation
* Wireless infrastructure
* Satellite and very small aperture terminal (VSAT)
* Microwave radio

ADF5611 Device Configuration
----------------------------

Driver Initialization
---------------------

Communicating with the device will require the driver to support Serial Peripheral Interface 
(SPI).

The first API to be called is the **adf5611_init**. Ensure that the function call returns 0,
which means that the driver was initialized correctly. This function also performs a soft reset,
putting the device in a known state.

Reference Clock Configuration
-----------------------------

By default, the device is set to use a 122.88MHz clock. This can be changed to any
value, which is supported by the device, by using the function 
**adf5611_set_ref_clk**. The current configuration can be read out using
**adf5612_get_ref_clk**.The input reference clock value for the function will
have to be expressed in KHz.

Reference Divider Configuration
-------------------------------

On the reference input path there is a divider, allowing the input reference
frequency to be divided by up to 16383. Default this is set to 2 and can be changed
using **adf5611_set_ref_div**. The current configuration can be read out using
**adf5611_get_ref_div**.

Together with the reference doubler this feature will determine the PFD frequency.

Charge Pump Current Configuration
---------------------------------

The ADF5611 has the option for 16 preset charge pump currents, ranging from 0.2
mA to 3.2 mA. By default the charge pump current is set to 3.2 mA. The
supported currents can be configured using **adf5611_set_cp_i** API, which will
expect the register value, ranging form 0 to 15, as inputs. The current
configuration can be read out using **adf5611_get_cp_i**, which will also return
the value in the same range.

In order to determine which value corresponds to your design charge pump
current, please refer to the datasheet `ADF5611 <www.analog.com/en/products/adf5611.html>`_ 
in the register details section for register REG0021h.


Output Frequency Configuration
------------------------------

The `ADF5611 <www.analog.com/en/products/adf5611.html>`_ and `ADF5612 <www.analog.com/en/products/adf5612.html>`_ 
each has a main RF output and a differential divider RF output.
The main RF output range from 7300MHz to 14600MHz, and the differential divided 
output is between 57MHz to 14600MHz for `ADF5611 <www.analog.com/en/products/adf5611.html>`_,
while `ADF5612 <www.analog.com/en/products/adf5612.html>`_ has a similar minimum main RF output 
frequency but has maximum main RF output as well as divided output of 8500MHz.
The main RF output can be reconfigured by using the **adf5611_set_rfout** API. 
The output frequency set by the API is expressed in Hz. The previously configured output 
frequency can be read out using **adf5611_get_rfout**.

This attribute will only set the main RFOUT frequency. While the differential output depends 
on this attribute settings, its output is determined by the Ouput divider settings.

Output Power Configuration
--------------------------

The output power of the RFOUT frequency can be configured using **adf5611_set_output_power** 
API. The current configuration can be read out using **adf5611_get_output_power**.

The Power of the RFOUT frequency is between 0 to 3 with 3 being the default.

Reference Divider Configuration
-------------------------------

On the differential ouput path there is a divider, allowing the main RFOUT frequency to be 
divided evenly up to 128. By default, this is set to 1, which is equal to the RFOUT frequency 
and can be changed using **adf5611_set_rfoutdiv_divider**. The current configuration can be 
read out using **adf5611_get_rfoutdiv_divider**.

Differential Divider Output Power Configuration
-----------------------------------------------

The differential divider output power can be configured using **adf5611_set_rfoutdiv_power** API. 
The current configuration can also be read out using **adf5611_get_rfoutdiv_power**.

The Power of differential output is between 0 to 3 with 3 being the default.

Differential Output Enable Configuration
----------------------------------------

The differential ouput path can be activated independently without affecting the main RF output 
using **adf5611_set_en_rfoutdiv** API.
The API expects a boolean value to enable or disable the path. The current status can be 
determined using the **adf5611_get_en_rfoutdiv** API.

By default the path is disabled. 0 is disabled and 1 is enabled.

ADF5611 Frequency Generation
----------------------------

Frequency Setting
-----------------

As mentioned above, using the default values the generated frequency will be of
12 GHz. This frequency depends on all of the previous configurations and is
recalculated based on them using the **adf5611_set_rfout** API. When the API is
called, it will calculate the VCO frequency, the integer and fractional parts of 
the feedback loop for the PLL while trying to obtain the configured output frequency. 
The API will also write the corresponding registers with the computed values.

ADF5611 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct adf5611_dev *dev;
	int ret;

	struct no_os_uart_init_param adf5611_uart_ip = {
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

	struct no_os_spi_init_param adf5611_spi_ip = {
		.device_id = SPI_DEVICE_ID,
        .max_speed_hz = 1500000,
        .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
        .mode = NO_OS_SPI_MODE_0,
        .platform_ops = SPI_OPS,
        .chip_select = SPI_CS,
        .extra = SPI_EXTRA,
	};

    struct no_os_i2c_init_param adf5611_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.platform_ops = I2C_OPS,
	.max_speed_hz = 100000,
	.extra = I2C_EXTRA,
	.slave_address = 0x54 
    };

    struct eeprom_24xx32a_init_param adf5611_eeprom_init_param = {
        .i2c_init = &adf5611_i2c_ip,
    };

    struct no_os_eeprom_init_param adf5611_eeprom_ip = {
        .device_id = 5,
        .platform_ops = &eeprom_24xx32a_ops,
        .extra = &adf5611_eeprom_init_param,
    };

	struct adf4368_init_param adf5611_ip = {
		.spi_init = &adf5611_spi_ip,
        .spi4wire = true,
        .cmos_3v3 = false,
        .ref_clk_freq = 122880000,
        .rfout_freq = 12000000000,
        .ref_div = 2,
        .cp_i = 15,
        .bleed_word = 17,
        .ld_count = 12,
        .id = ID_ADF5611,
	};


	ret = adf5611_init(&dev, &adf5611_ip);
 	if (ret)
 		goto error;

 	ret = adf5611_set_freq(dev, 12000000000);
 	if (ret)
 		goto error;


ADF5611 no-OS IIO support
-------------------------

The ADF5611 IIO driver comes on top of ADF5611 driver and offers support for
interfacing IIO clients through IIO lib.

ADF5611 IIO Device Configuration
--------------------------------

Device Attributes
-----------------

ADF5611 and ADF5612 has only one channel for the the RF output frequency, which makes most
of the attributes, device attributes.

The attributes are:

* reference_frequency - Sets the current Reference Frequency.
* reference_divider - Sets the current Reference Divider ratio.
* charge_pump_current - Sets the current Charge Pump Configuration.
* charge_pump_current_available - Returns the available Charge Pump Currents.
* rfoutdiv_power - Sets the current Differential Divider Output Power.
* rfoutdiv_divider - Sets the current Differential Divider ratio.
* rfoutdiv_divider_available - Returns the available Differential Divider Ratios.
* en_rfoutdiv - Enables or Disables the Differential Output path.

Device Channels
---------------

ADF5611 and ADF5612 IIO device has 1 output channel which corresponds to the RF output 
frequency.

Which is represented as:

* output altvoltage0 - corresponding to channel 1 on the device

The channel has 2 attributes:

* rfout_frequency - Sets the current RF Output Frequency.
* rfout_power - Sets the current RF Output Power.

ADF5611 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	struct adf5611_iio_dev *adf5611_iio_dev;
	struct adf5611_iio_dev_init_param adf5611_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_ctx_attr *context_attributes;
	int ret;

	struct no_os_uart_init_param adf5611_uart_ip = {
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

    struct no_os_spi_init_param adf5611_spi_ip = {
        .device_id = SPI_DEVICE_ID,
        .max_speed_hz = 1500000,
        .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
        .mode = NO_OS_SPI_MODE_0,
        .platform_ops = SPI_OPS,
        .chip_select = SPI_CS,
        .extra = SPI_EXTRA,
    };

    struct no_os_i2c_init_param adf5611_i2c_ip = {
        .device_id = I2C_DEVICE_ID,
        .platform_ops = I2C_OPS,
        .max_speed_hz = 100000,
        .extra = I2C_EXTRA,
        .slave_address = 0x54 
    };

    struct eeprom_24xx32a_init_param adf5611_eeprom_init_param = {
        .i2c_init = &adf5611_i2c_ip,
    };

    struct no_os_eeprom_init_param adf5611_eeprom_ip = {
        .device_id = 5,
        .platform_ops = &eeprom_24xx32a_ops,
        .extra = &adf5611_eeprom_init_param,
    };

    struct adf5611_init_param adf5611_ip = {
        .spi_init = &adf5611_spi_ip,
        .spi4wire = true,
        .cmos_3v3 = false,
        .ref_clk_freq = 122880000,
        .rfout_freq = 12000000000,
        .ref_div = 2,
        .cp_i = 15,
        .bleed_word = 17,
        .ld_count = 12,
        .id = ID_ADF5611,
    };

	adf5611_iio_ip.adf5611_dev_init = &adf5611_ip;
	ret = adf5611_iio_init(&adf5611_iio_dev, &adf5611_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf5611",
			.dev = adf5611_iio_dev,
			.dev_descriptor = adf5611_iio_dev->iio_dev,
		}
	};

    struct no_os_eeprom_desc *eeprom_dev;
	ret = eeprom_24xx32a_ops.init(&eeprom_dev, &adf5611_eeprom_ip);
	if(ret) 
		return ret;
	char board_name[17] ={0};
	char hw_mezz[25];
	uint32_t eeprom_address = 0x1B;

	ret = eeprom_24xx32a_ops.read(eeprom_dev, eeprom_address, 
			(uint8_t *)board_name, sizeof(board_name));
	if(ret)
		return ret;
	int i, index = 0;
	for (i = 0; board_name[i] != '\0'; i++)
	{
		if (isalnum((unsigned char)board_name[i]) || board_name[i] == '-') {
			hw_mezz[index++] = board_name[i];
		}
	}
	hw_mezz[index] = '\0';

	ret = eeprom_24xx32a_ops.remove(eeprom_dev);
	if(ret){
		return ret;
	}

	context_attributes = (struct iio_ctx_attr *)calloc(NUM_CTX_ATTR, 
				sizeof(*context_attributes));
	context_attributes[0].name = "hw_mezzanine";
	context_attributes[0].value = hw_mezz;
	context_attributes[1].name = "hw_carrier";
	context_attributes[1].value = HW_CARRIER;
	context_attributes[2].name = "hw_name";
	context_attributes[2].value = ACTIVE_DEVICE_NAME;
	context_attributes[3].name = "hw_vendor";
	context_attributes[3].value = ACTIVE_DEVICE_VENDOR;

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf5611_uart_ip;
	app_init_param.ctx_attrs = context_attributes;
	app_init_param.nb_ctx_attr = NUM_CTX_ATTR;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;
	iio_app_run(app);
