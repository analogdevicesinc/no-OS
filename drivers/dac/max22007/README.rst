MAX22007 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX22007 <https://www.analog.com/en/products/max22007.html>`_

Overview
--------

The MAX22007 is a software-configurable four-channel
analog output device that drives a voltage or current output
on each channel. Each output channel of the MAX22007 features a 12-bit
DAC with fast settling time using a shared internal voltage
reference. The MAX22007 communicates with a microcontroller using an
SPI interface at clock rates up to 30MHz with an optional eight-bit CRC for improved data integrity.


Applications
------------

* Building Automation Analog Outputs
* Configurable Analog Output Cards
* Factory Automation Analog Outputs
* Process Automation
* Programmable Logic Controllers

MAX22007 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, SPI communication needs to be established
between the part and a microcontroller. This is done through the
:func:`max22007_init` function. The function requires a structure of type
:c:struct:`max22007_dev` that contains the SPI configuration information and
the chip select GPIO information. The function returns 0 in case of success
or a negative error code otherwise.

Channel Mode Configuration
--------------------------

The MAX22007 has four channels that can be configured to operate in
different modes. The configuration is done through the
:func:`max22007_set_channel_mode` function. The 2 possible channel modes are:
* Voltage Output Mode
* Current Output Mode

Channel Power Mode Configuration
--------------------------------

The MAX22007 has four channels that can be configured to operate in
different power modes. The configuration is done through the
:func:`max22007_set_channel_power` function. The 2 possible channel power
modes are:
* Power off Mode
* Power on Mode

DAC Channel Latch Mode Configuration
------------------------------------

The MAX22007 has four channels that can be configured to operate in
different latch modes. The configuration is done through the
:func:`max22007_set_latch_mode` function. The 2 possible channel
latch modes are:
* Transparent Mode
* Latched Mode (This mode enables the DAC channels to be updated via an LDAC update (register write/GPIO))

Setting the Output Value
------------------------

The output value of each channel can be set through the
:func:`max22007_write_channel_data` function. The function requires the channel
number and the output value to be set. The output value is a 12-bit value
that can be set between 0 and 4095. 

Configuring the SPI timeout
---------------------------

The SPI timeout can be configured through the
:func:`max22007_set_timeout` function. The status of the Timeout select, timeout configuration and timeout enable
is set through this function.

After the basic example code has been programmed to the MCU, the outputs of channel 0 and 3 shall be set to 2 and 4v respectively, 
as the channels have been configured to operate in voltage output mode.

MAX22007 Driver Initialization Example
--------------------------------------

.. code-block:: c

   struct no_os_spi_init_param max22007_spi_init = {
   	.device_id = SPI_DEVICE_ID,
   	.max_speed_hz = 5000000,
   	.mode = NO_OS_SPI_MODE_0,
   	.chip_select = GPIO_CS_PIN,
   	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   	.platform_ops = &spi_platform_ops,
   	.extra = &max22007_spi_extra_ip
   };

   struct max22007_init_param max22007_ip = {
   	.comm_param = &max22007_spi_init,
   	.crc_en = false,
   	.ref_mode = INTERNAL_REF,
   	.timeout_config = {
   		.timeout_en = false,
   		.timeout_sel = MAX22007_TIMEOUT_100MS,
   		.timeout_cnfg = TIMEOUT_RESET,
   	},
   	.channel_config = {
   		[0] = {
   			.channel_mode = MAX22007_VOLTAGE_MODE,
   			.latch_mode = TRANSPARENT_LATCH,
   			.channel_power = MAX22007_CH_POWER_ON,
   		},
   		[1] = {
   			.channel_mode = MAX22007_VOLTAGE_MODE,
   			.latch_mode = LDAC_CONTROL,
   			.channel_power = MAX22007_CH_POWER_OFF,
   		},
   		[2] = {
   			.channel_mode = MAX22007_VOLTAGE_MODE,
   			.latch_mode = LDAC_CONTROL,
   			.channel_power = MAX22007_CH_POWER_OFF,
   		},
   		[3] = {
   			.channel_mode = MAX22007_VOLTAGE_MODE,
   			.latch_mode = LDAC_CONTROL,
   			.channel_power = MAX22007_CH_POWER_ON,
   		},
   	}
   };
   