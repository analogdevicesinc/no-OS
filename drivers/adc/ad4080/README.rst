AD4080 no-OS Driver
====================

Supported Devices
-----------------

`AD4080 <https://www.analog.com/AD4080>`_

Overview
--------

The AD4080 is a high-speed, low noise, low distortion, 20-bit, Easy Drive,
successive approximation register (SAR) analog-to-digital converter (ADC).
Maintaining high performance (signal-to-noise and distortion (SINAD) ratio >
90 dBFS) at signal frequenciesin excess of 1 MHz enables the AD4080 to service a
wide variety of precision, wide bandwidth data acquisition applications.
Simplification of the input anti-alias filter design can be accomplished by
applying oversampling along with the integrated digital filtering and decimation
to reduce noise and lower the output data rate for applications that do not
require the lowest latency of the AD4080.

Applications
------------

* Digital imaging
* Cell analysis
* Spectroscopy
* Automated test equipment
* High speed data acquisition
* Digital control loops, hardware in the loop
* Power quality analysis
* Source measurement units
* Electron and x-ray microscopy
* Radar level measurement
* Nondestructive test
* Predictive maintenance and structural health

AD4080 Device Configuration
----------------------------

Driver Initialization
---------------------

The **ad4080_init** function initializes the AD4080 device by allocating memory
for the device structure, setting up two SPI interfaces (one for configuration
and one for data), and verifying communication with the chip by reading its ID.
It performs a software reset, configures SPI settings like 3-wire mode and
address auto-increment, sets the device's operating mode, and initializes both
the configuration and data interfaces. Throughout the process, it checks for
errors at each step and performs cleanup if any initialization fails.

SPI Configuration
-------------------

**ad4080_configuration_intf_init** configures the interface of the AD4080 device
by applying three specific settings: single instruction mode, short instruction
mode, and strict register access. It first checks if the device pointer is
valid, then sequentially calls three configuration functions
(ad4080_set_single_instr, ad4080_set_short_instr, and
ad4080_set_strict_reg_access) using values from the init_param structure. If any
of these steps fail, it immediately returns the error code; otherwise, it
completes successfully and returns 0.

AD4080 Main Functions
---------------------------

Read Operation
---------------

The **ad4080_read** function reads a register value from the AD4080 device via
SPI. It builds a read command using the register address, performs an SPI
transfer, and stores the received byte in reg_val. It also handles chip-select
GPIO toggling before and after the transfer.

Read Data
-----------

The **ad4080_read_data** function reads a block of data from the AD4080 device
using the data SPI interface. It pulls the chip-select (CS) line low to start
communication, performs a full-duplex SPI transfer using the provided buffer
and length, then pulls the CS line high to end communication. It returns the
result of the SPI transfer.


AD4080 Driver Initialization Example
-------------------------------------

.. code-block:: c

	struct ad4080_dev *dev;
	uint8_t data;
	int ret;

	dev = (struct ad4080_dev *)calloc(1, sizeof(*dev) + init_param.privdata_len);
	if (!dev)
		return -ENOMEM;

	/* init the config spi */
	ret = ad4080_init_spi(&dev->cfg, &init_param.cfg);
	if (ret)
		goto error_dev;

	/* init the data spi */
	ret = ad4080_init_spi(&dev->data, &init_param.data);
	if (ret)
		goto error_cfg_spi;

	ret = ad4080_read(dev, AD4080_REG_CHIP_TYPE, &data);
	if (ret)
		goto error_data_spi;

	if (data != AD4080_CHIP_ID) {
		ret = -EINVAL;
		goto error_data_spi;
	}

	/* Software Reset */
	ret = ad4080_soft_reset(dev);
	if (ret)
		goto error_data_spi;

	dev->spi3wire = init_param.spi3wire;
	dev->addr_asc = init_param.addr_asc;

	ret = ad4080_update_bits(dev, AD4080_REG_INTERFACE_CONFIG_A,
				 AD4080_ADDR_ASC_MSK |
				 AD4080_SDO_ENABLE_MSK,
				 no_os_field_prep(AD4080_SDO_ENABLE_MSK, dev->spi3wire) |
				 no_os_field_prep(AD4080_ADDR_ASC_MSK, dev->addr_asc));
	if (ret)
		goto error_data_spi;

	/* Set Operation mode */
	ret = ad4080_set_op_mode(dev, init_param.op_mode);
	if (ret)
		goto error_data_spi;

	/* Configuration SPI Interface Initialization */
	ret = ad4080_configuration_intf_init(dev, init_param);
	if (ret)
		goto error_data_spi;

	/* During power on or a software reset, GPI0 is the default config SDO.
	 * AD4080 driver will defer GP initialization and will not make 
	 * assumptions on what the other GPs are for what function. */

	/* Data Interface Initialization */
	ret = ad4080_data_intf_init(dev, init_param);
	if (ret)
		goto error_data_spi;

	*device = dev;

	return 0;

AD4080 no_OS IIO Support
-------------------------

The AD4080 IIO driver comes on top of AD4080 driver and offers support for
interfacing IIO clients through IIO lib.

AD4080 Device Configuration
----------------------------

Device Attributes
-----------------

* ``raw - the raw value read from the device``
* ``scale - the scale that has to be applied to the raw value in order to obtain the converted real value in mC or mV``
* ``offset - the offset value that has to be applied to the raw value in order to obtain the converted real value in mC or mV``
* ``gp0_io_glob - the GPIO 0 I/O configuration``
* ``gp0_func_glob - the GPIO 0 function configuration``
* ``gp1_io_glob - the GPIO 1 I/O configuration``
* ``gp1_func_glob - the GPIO 1 function configuration``
* ``gp2_io_glob - the GPIO 2 I/O configuration``
* ``gp2_func_glob - the GPIO 2 function configuration``
* ``gp3_io_glob - the GPIO 3 I/O configuration``
* ``gp3_func_glob - the GPIO 3 function configuration``
* ``fifo_mode_glob - the FIFO mode configuration``
* ``fifo_watermark_glob - the FIFO watermark configuration``
* ``evt_detection_hysteresis_glob - the event detection hysteresis configuration``
* ``evt_detection_hi_glob - the event detection high threshold configuration``
* ``evt_detection_lo_glob - the event detection low threshold configuration``
* ``filter_sel_glob - the filter selection configuration``
* ``filter_sinc_dec_rate_glob - the filter SINC decimation rate configuration``
* ``device_mode_glob - the device mode configuration``
* ``afe_ctrl_glob - the AFE control configuration``
* ``max_attr - the maximum number of attributes supported by the device``

Device Channels
---------------

The AD4080 IIO device has 1 input voltage channel.

Voltage Channel
---------------

The voltage channel is: 

* Channel 0: Voltage

The voltage channel has 2 attributes:

* raw - the raw voltage value read from the device.
* scale - is the scale that has to be applied to the raw value in order to obtain the converted real value in Volts. It depends on the ADC V Range and has a specific formula.

Device buffers
--------------

The AD4080 IIO devices driver supports the usage of a data buffer for reading purposes.

AD4080 IIO Driver Initialization Example
-----------------------------------------

.. code-block:: bash

	int err;
	struct ad4080_dev *dev;
	struct ad4080_init_param *ad4080_init_param;
	struct iio_ad4080_desc *ad4080_iio;
	struct iio_ad4080_fifo_struct *fifo;

	if (!iio_ad4080)
		return -EINVAL;

	if (!iio_ad4080_init_param)
		return -EINVAL;

	ad4080_init_param = iio_ad4080_init_param->ad4080_init_param;

	ad4080_init_param->privdata_len = sizeof(struct iio_ad4080_desc) + AD4080_ADC_DATA_BUFFER_LEN;
	/* iio ad4080 exposes just 1 iio_app_device */
	ad4080_init_param->privdata_len += (1 * sizeof(struct iio_app_device));
	err = ad4080_init(&dev, *ad4080_init_param);
	if (err)
		return err;

	ad4080_iio = ad4080_privdata(dev);
	ad4080_iio->ad4080 = dev;
	fifo = &ad4080_iio->fifo;
	err = iio_ad4080_fifo_init(fifo, dev);
	if (err)
		return err;

	err = iio_ad4080_fifo_set_watermark(fifo, iio_ad4080_init_param->watermark);
	if (err)
		return err;

	/* initialize the AFE control - default disabled */
	if (!iio_ad4080_init_param->afe_ctrl_init_param)
		goto err_register_irq;
	err = no_os_gpio_get(&ad4080_iio->afe_ctrl,
			     iio_ad4080_init_param->afe_ctrl_init_param);
	if (err)
		return err;
	no_os_gpio_direction_output(ad4080_iio->afe_ctrl, NO_OS_GPIO_LOW);

	*iio_ad4080 = ad4080_iio;

	return 0;
