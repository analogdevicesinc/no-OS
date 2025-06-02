ADMFM2000 no-OS driver
======================

.. no-os-doxygen::

Supported Devices
-----------------

`ADMFM2000 <https://www.analog.com/en/products/admfm2000.html>`_

Overview
--------

The `ADMFM2000 <https://www.analog.com/en/products/admfm2000.html>`_ is a dual-channel
microwave downconverter, with
input RF and local oscillator (LO) frequency ranges covering 5 GHz
to 32 GHz, with an output intermediate frequency (IF) frequency
range from 0.5 GHz to 8 GHz. The downconverting mixer can
also be bypassed allowing direct access to the 0.5 to 8 GHz IF
path. A common LO input signal is split to feed two separate
buffer amplifiers to drive the mixer in each channel. Each down
conversion path consists of a low noise amplifier (LNA), a mixer, an
IF filter, a digital step attenuator (DSA), and an IF amplifier

Fabricated using a combination of surface mount and bare die
components, the `ADMFM2000 <https://www.analog.com/en/products/admfm2000.html>`_
provides precise gain adjustment capabilities with low distortion performance.
The ADMFM2000 comes in a compact, shielded 20.00 mm × 14.00 mm, 179-ball
chip scale package ball grid array (CSP_BGA) and operates over a temperature
range of −40°C to +85°C.

Applications
------------

* Phased array radar receivers
* Satellite communications (satcom) receivers
* Electronic warfare
* Electronic test and measurement equipment
* Automatic test equipmen

ADMFM2000 Device Configuration
--------------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the gpios
connected in order to control the device.

The first API to be called is **admfm2000_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
set the channel to mixer_mode/direct if and set the gain for both channels
according to the initialization parameter. 

Switch configutation
-----------------------------

The ADMFM2000 has a broadband SPDT RF switch in each channel that can be used to
bypass the mixer. epending on the logic level applied to those control pins, the
IF path is either connected to the mixer or to the RF_BYPASS_IN pin. This can be
done using **admfm2000_set_channel_mode**. The current configuration can be read
out using **admfm2000_get_channel_mode**.The channels can only be set
simultaneously, meaning when one channel is set to mixer mode, the other one will
be set to bypass mode.

ADMFM2000 Driver Initialization Example
-----------------------------------------

Platform Communication Example
-----------------------------------------

.. code-block:: bash

	struct admfm2000_dev *dev;
	int ret;

	struct no_os_uart_init_param admfm2000_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = &xuip,
	};

	struct no_os_gpio_init_param admfm2000_gpio_ip[2][2] = {  
		{
			{
				.port = 2,
				.number = 10,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 6,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		},
		{
			{
				.port = 0,
				.number = 12,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 14,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		}
	};

	struct no_os_gpio_init_param admfm2000_gpio_dsa_ip[2][5] = {  
		{
			{
				.port = 2,
				.number = 1,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 11,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 7,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 8,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 9,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		},
		{
			{
				.port = 2,
				.number = 20,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 21,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 3,
				.number = 8,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 4,
				.number = 1,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 13,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		}
	};

	struct admfm2000_init_param admfm2000_ip = {
		.mixer_mode = ADMFM2000_DIRECT_IF_MODE,
		.dsa_gain = 0,
		.gpio_sw_param = &admfm2000_gpio_ip,
		.gpio_dsa_param = &admfm2000_gpio_dsa_ip,
	};

	ret = admfm2000_init(&dev, &admfm2000_ip);
	if (ret)
		goto error;

 	ret = admfm2000_set_channel_config(dev, ADMFM2000_MIXER_MODE);
	if (ret)
		goto error;

 	ret = admfm2000_set_gain(dev, 0, 24);
 	if (ret)
 		goto error;

ADMFM2000 no-OS IIO support
----------------------------

The ADMFM2000 IIO driver comes on top of ADMFM2000 driver and offers support for
interfacing IIO clients through IIO lib.

ADMFM2000 IIO Device Configuration
-----------------------------------

Device Attributes
-----------------

The ADMFM2000 only has two channels, each having a configurable gain.

Device Channels
---------------

ADMFM2000 IIO device has 2 output channels.

The channels are:

* output voltage0 - corresponding to channel 1 on the device
* output voltage1 - corresponding to channel 2 on the device

Each channel has 1 individual attribute:

* hardwaregain - is the dsa gain of the channel. The gain can be set between 0
  (reference) and 31.

ADMFM2000 IIO Driver Initialization Example
--------------------------------------------

.. code-block:: bash

	struct admfm2000_iio_dev *admfm2000_iio_dev;
	struct admfm2000_iio_dev_init_param admfm2000_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	struct no_os_uart_init_param admfm2000_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = &xuip,
	};

	struct no_os_gpio_init_param admfm2000_gpio_ip[2][2] = {  
		{
			{
				.port = 2,
				.number = 10,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 6,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		},
		{
			{
				.port = 0,
				.number = 12,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 14,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		}
	};

	struct no_os_gpio_init_param admfm2000_gpio_dsa_ip[2][5] = {  
		{
			{
				.port = 2,
				.number = 1,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 11,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 7,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 8,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 9,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		},
		{
			{
				.port = 2,
				.number = 20,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 2,
				.number = 21,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 3,
				.number = 8,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 4,
				.number = 1,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
			{
				.port = 0,
				.number = 13,
				.pull = NO_OS_PULL_NONE,
				.platform_ops = GPIO_OPS,
				.extra = GPIO_EXTRA,
			},
		}
	};

	struct admfm2000_init_param admfm2000_ip = {
		.mixer_mode = ADMFM2000_DIRECT_IF_MODE,
		.dsa_gain = 0,
		.gpio_sw_param = &admfm2000_gpio_ip,
		.gpio_dsa_param = &admfm2000_gpio_dsa_ip
	};

	admfm2000_iio_ip.admfm2000_dev_init = &admfm2000_ip;
	ret = admfm2000_iio_init(&admfm2000_iio_dev, &admfm2000_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "admfm2000",
			.dev = admfm2000_iio_dev,
			.dev_descriptor = admfm2000_iio_dev->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = admfm2000_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
