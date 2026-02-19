AD717X no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

`AD4111 <https://www.analog.com/AD4111>`_
`AD4112 <https://www.analog.com/AD4112>`_
`AD4113 <https://www.analog.com/AD4113>`_
`AD4114 <https://www.analog.com/AD4114>`_
`AD4115 <https://www.analog.com/AD4115>`_
`AD4116 <https://www.analog.com/AD4116>`_
`AD7172-2 <https://www.analog.com/AD7172-2>`_
`AD7172-4 <https://www.analog.com/AD7172-4>`_
`AD7173-8 <https://www.analog.com/AD7173-8>`_
`AD7175-2 <https://www.analog.com/AD7175-2>`_
`AD7175-8 <https://www.analog.com/AD7175-8>`_
`AD7176-2 <https://www.analog.com/AD7176-2>`_
`AD7177-2 <https://www.analog.com/AD7177-2>`_

Overview
--------

The AD717x/AD411x family are low power, multiplexed, sigma-delta (Σ-Δ)
analog-to-digital converters (ADCs) designed for high precision measurement
applications. The AD717x devices provide 24-bit resolution with configurable
output data rates, multiple input channels, and flexible reference options.
The AD411x devices integrate an analog front end (AFE) for direct connection
to ±10 V inputs. The AD4113 is a 16-bit variant with four differential or
eight single-ended ±10 V inputs.

All devices communicate via SPI and support up to 16 channels with 8
independent setup configurations. Features include programmable gain,
digital filtering (sinc3, sinc5+sinc1), bipolar/unipolar operation,
CRC/XOR data integrity checking, and multiple ADC operating modes
(continuous, single, standby, power-down, and calibration modes).

Applications
------------

- Process control and industrial automation
- Temperature and pressure measurement
- Precision data acquisition systems
- Sensor interfacing and signal conditioning
- Power metering and energy monitoring

AD717X Device Configuration
----------------------------

Driver Initialization
---------------------

In order to use the device, you will have to provide the support for the
SPI communication protocol. The first API to be called is **AD717X_Init**.
Make sure that it returns 0, which means that the driver was initialized
correctly. The ``ad717x_init_param`` structure must be configured with the
appropriate device type, SPI settings, and register map for the target device.

Channel Configuration
---------------------

The driver supports up to 16 channels. Each channel can be independently
enabled or disabled using **ad717x_set_channel_status**. Analog inputs
are connected to channels using **ad717x_connect_analog_input**, which
accepts device-specific input pairs for AD411x devices or individual
analog input pins for AD717x devices. Each channel is mapped to one of
8 setup configurations via **ad717x_assign_setup**.

Setup Configuration
-------------------

Each setup defines the signal path configuration including polarity
(bipolar or unipolar) via **ad717x_set_polarity**, reference source
selection (external, internal, or AVDD-AVSS) via
**ad717x_set_reference_source**, and input/reference buffer enable
via **ad717x_enable_buffers**.

ADC Mode and Data Rate
----------------------

The operating mode is set using **ad717x_set_adc_mode**, supporting
continuous conversion, single conversion, standby, power-down, and
calibration modes. The output data rate is configured per filter
register using **ad717x_configure_device_odr**.

Data Read
---------

Conversion results can be read using **ad717x_single_read** for
single conversions or **AD717X_ReadData** for reading raw data.
The **AD717X_WaitForReady** function polls the status register
to wait for conversion completion.

Driver Initialization Example
-----------------------------

.. code-block:: c

   ad717x_dev *ad717x;
   struct no_os_spi_init_param spi_init = {
   	.device_id = SPI_DEVICE_ID,
   	.max_speed_hz = 1000000,
   	.mode = NO_OS_SPI_MODE_3,
   	.chip_select = SPI_CS,
   	.platform_ops = SPI_OPS,
   	.extra = &spi_extra
   };
   ad717x_init_param init_param = {
   	.spi_init = spi_init,
   	.active_device = ID_AD4111,
   	.regs = ad4111_regs,
   	.num_regs = ARRAY_SIZE(ad4111_regs),
   	.num_channels = 4,
   	.num_setups = 4,
   	.mode = CONTINUOUS,
   };
   int ret = AD717X_Init(&ad717x, init_param);
   if (ret)
   	goto error;
