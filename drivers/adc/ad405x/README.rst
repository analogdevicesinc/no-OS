AD405X no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

`AD4050 <https://www.analog.com/AD4050>`_
`AD4052 <https://www.analog.com/AD4052>`_
`AD4056 <https://www.analog.com/AD4056>`_
`AD4058 <https://www.analog.com/AD4058>`_
`AD4060 <https://www.analog.com/AD4060>`_
`AD4062 <https://www.analog.com/AD4062>`_

Overview
--------

The AD4052/AD4062 are low power, compact 16-bit successive approximation
register (SAR) analog-to-digital converters (ADC) designed
for battery powered precision measurement and monitoring applications.
The AD4052/AD4062 feature set supports event-driven programming
for dynamic tradeoff between system power and precision. Using
a patented, power efficient window comparator, the AD4052 autonomously monitors
signals while the host sleeps. The programmable averaging filter enables
on-demand high resolution measurements for optimizing precision for the power
consumed.

The AD4052 includes AFE control signals to minimize the complexity of
host timers.
The control signals automate the power cycling
of the AFE relative to ADC sampling to reduce system power while
minimizing settling error artifacts. The Easy Drive analog inputs
enables compact and low power signal conditioning circuitry by
reducing the dependence on high-speed ADC driver amplifiers. The
small 3.4 pF sampling capacitors result in low dynamic and average
input current, broadening compatibility with low power amplifiers or
direct sensor interfacing. The AD4052 wide common mode input
range supports both differential and single-ended input signals.

The AD4052 family features a 4-wire SPI with a dedicated CNV input.
The AD4062 family features a I3C, allowing multiple devices on the same
bus.
Cyclic redundancy check (CRC) is available on all interface read
and write operations and internal memory to ensure reliable device
configuration and operation.

AD405X Device Configuration
---------------------------

Driver Initialization
---------------------

SPI
~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as well as 3 external GPIOs for the CNV pin and two
general-purpose input/output pins (GP0 and GP1).

SPI&I3C
~~~~~~~

The first API to be called is **ad405x_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

GPIO Configuration
------------------

The device has two general purpose output pins, GP0 and GP1.
These pins can be configured as threshold events, data ready, among other
status signals.
In the driver files the **ad405x_set_gp_mode** can be found and used to choose
the specific signal for the GPIOs.

If GP0 is set as DRDY, the device will assert the pin on the CONV assertion,
and the ADC driver will wait the pin to desert before issuing the ADC data
acquisition.
During initialization, GP1 is used to track the DEV_RDY state,
and no further behaviour is defined at the driver level.

Channel Configuration
---------------------

Channel data can be fetched with **ad405x_get_adc**.

The channel data format can be set using **ad405x_set_data_format**

Channel operation mode can also be configured using **ad405x_set_operating_mode**.

Soft Reset
----------

The device can be soft reset by using **ad405x_soft_reset**.

AD405X Driver Initialization Example
------------------------------------

SPI
~~~

.. code-block:: c

   struct ad405x_dev *ad405x;
   const struct no_os_spi_init_param ad405x_spi_ip = {
   	.device_id = SPI_DEVICE_ID,
   	.max_speed_hz = 100000,
   	.mode = NO_OS_SPI_MODE_0,
   	.chip_select = GPIO_CS_PIN,
   	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   	.platform_ops = SPI_OPS,
   	.extra = &ad405x_spi_extra_ip
   };
   const struct no_os_gpio_init_param gpio_cnv_param = {
   	.port = GPIO_CNV_PORT,
   	.number = GPIO_CNV_PIN,
   	.platform_ops = GPIO_OPS,
   	.extra = &gpio_init
   };
   const struct no_os_gpio_init_param gpio_gpio0_param = {
   	.port = GPIO_GPIO0_PORT,
   	.number = GPIO_GPIO0_PIN,
   	.platform_ops = GPIO_OPS,
   	.extra = &gpio_init
   };
   const struct no_os_gpio_init_param gpio_gpio1_param = {
   	.port = GPIO_GPIO1_PORT,
   	.number = GPIO_GPIO1_PIN,
   	.platform_ops = GPIO_OPS,
   	.extra = &gpio_init
   };
   struct ad405x_init_param ad405x_ip = {
   	.comm_type = AD405X_COMM,
   	.comm_init.spi_init = ad405x_spi_ip,
   	.dev_type = AD405X_DEV_TYPE,
   	.gpio_cnv = &gpio_cnv_param,
   	.gpio_gpio0 = &gpio_gpio0_param,
   	.gpio_gpio1 = &gpio_gpio1_param
   };
   ret = ad405x_init(&ad405x, &ad405x_ip);
   if (ret)
   	goto error;

I3C
~~~

.. code-block:: c

   struct ad405x_dev *ad405x;
   const struct no_os_i3c_init_param *i3c1_devs_param[] = {
	&ad405x_i3c_ip
   };
   struct no_os_i3c_bus_init_param i3c1_ip = {
   	.device_id = I3C_DEVICE_ID,
   	.platform_ops = I3C_OPS,
   	.devs = i3c1_devs_param,
   	.num_devs = LENGTH_I3C_DEVS,
   	.extra = &ad405x_i3c_extra_ip
   };
   const struct no_os_i3c_init_param ad405x_i3c_ip = {
   	.bus = &i3c1_ip,
   	.pid = AD405X_PID(AD405X_INSTANCE_ID),
   	.is_i3c = AD405X_IS_I3C,
   	.addr = AD405X_DYN_ADDR,
   	.is_static = AD405X_NO_STATIC_ADDR,
   };
   const struct no_os_gpio_init_param gpio_gpio0_param = {
   	.port = GPIO_GPIO0_PORT,
   	.number = GPIO_GPIO0_PIN,
   	.platform_ops = GPIO_OPS,
   	.extra = &gpio_init
   };
   const struct no_os_gpio_init_param gpio_gpio1_param = {
   	.port = GPIO_GPIO1_PORT,
   	.number = GPIO_GPIO1_PIN,
   	.platform_ops = GPIO_OPS,
   	.extra = &gpio_init
   };
   struct no_os_i3c_init_param ad405x_i3c_ip = {
   	.bus = &i3c1_ip,
   	.pid = AD405X_PID(AD405X_INSTANCE_ID),
   	.is_i3c = AD405X_IS_I3C,
   	.addr = AD405X_DYN_ADDR,
   	.is_static = AD405X_NO_STATIC_ADDR,
   };
   struct ad405x_init_param ad405x_ip = {
   	.comm_type = AD405X_COMM,
   	.comm_init.i3c_init = ad405x_i3c_ip,
   	.dev_type = AD405X_DEV_TYPE,
   	.gpio_gpio0 = &gpio_gpio0_param,
   	.gpio_gpio1 = &gpio_gpio1_param
   };
   ret = ad405x_init(&ad405x, &ad405x_ip);
   if (ret)
   	goto error;

