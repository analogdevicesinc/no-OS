AD5592R no-OS Driver
======================

Supported Devices
-----------------

- :adi:`AD5592R`
- :adi:`AD5593R`

Overview
---------

The AD5592R and AD5593R are both 8-channel, 12-bit, configurable ADC/DAC
on top of SPI and I2C interface respectively. Each of the 8 I/Ox pins can be 
independently configured as digital-to-analog (DAC) outputs, analog-to-digital
(ADC) inputs, digital outputs (GPI) or digital inputs (GPO).

When an I/Ox pin is configured as an analog output, it is driven by a 12-bit DAC.
The output range of the DAC is 0V to Vref or 0V to 2xVref.

When an I/Ox pin is configured as an analog input, it is connected to a 12-bit
ADC via an analog multiplexer. Like the DAC, the input range of the ADC is 0V
to Vref or 0V to 2xVref. 

Additionally, both the AD5592R and AD5593R have an integrated temperature
indicator which gives an indication of the die temperature. The temperature is
read back as part of an ADC sequence.

Applications
------------

- Industrial control systems
- Process control
- Factory automation

Device Configuration
---------------------

Initialization and Deinitialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Both AD5592R and AD5593R use the same initialization structure
``ad5592r_init_param``. The table below describes the fields in this
initialization parameter.

+---------------------+-------------------------------------------------------+
|        Field        | Remarks                                               |
+=====================+=======================================================+
|             int_ref | Use the 2v5 internal reference or an external one.    |
|                     |                                                       |
+---------------------+-------------------------------------------------------+
|            spi_init | The SPI device initialization parameters for AD5592R. |
|                     |                                                       |
+---------------------+-------------------------------------------------------+
|             ss_init | Provide a GPIO pin as Software Slave Select when      |
|                     | needed. Can be left NULL if SS is integrated one.     |
+---------------------+-------------------------------------------------------+
|            i2c_init | The I2C device initialization parameters for AD5593R. |
|                     |                                                       |
+---------------------+-------------------------------------------------------+
|    channel_modes[8] | Selects the mode for each pin: Can be one of the ff:  |
|                     | CH_MODE_ADC : pin is connected to ADC                 |
|                     | CH_MODE_DAC : pin is connected to DAC                 |
|                     | CH_MODE_DAC_AND_ADC: pin is connected to both ADC and |
|                     |                      DAC                              |
|                     | CH_MODE_GPI : pin can read digital signal in.         |
|                     | CH_MODE_GPO : pin can provide digital signal out.     |
|                     | CH_MODE_UNUSED : refer to channel_offstate.           |
+---------------------+-------------------------------------------------------+
| channel_offstate[8] | When pin is unused it can be in 1 of four states:     |
|                     | CH_OFFSTATE_OUT_LOW: pin provides logic 0 signal.     |
|                     | CH_OFFSTATE_OUT_HIGH: pin provides logic 1 signal.    |
|                     | CH_OFFSTATE_OUT_TRISTATE: pin is in high-Z state.     |
|                     | CH_OFFSTATE_PULLDOWN: Lastly, a pin can be connected  |
|                     | to an internal 85K Ohm pull down resistor.            |
+---------------------+-------------------------------------------------------+
|           adc_range | Voltage Reference for ADC. Can be one of the ff:      |
|                     | ZERO_TO_VREF                                          |
|                     | ZERO_TO_2VREF                                         |
+---------------------+-------------------------------------------------------+
|           dac_range | See adc_range field.                                  |
+---------------------+-------------------------------------------------------+
|             adc_buf | Enable or disable buffering for the ADC.              |
+---------------------+-------------------------------------------------------+
|       power_down[8] | Depending on the channel_mode[x], if its operating as |
|                     | a DAC, this controls if its powered down or operating |
|                     | normally.                                             |
+---------------------+-------------------------------------------------------+

Initialization functions are mirrored yet distinct. Initialization APIs follow
the ``[chip]_init()`` pattern where ``ad5592r_init()`` is for initializing the
ad5592r device and ``ad5593r_init()`` is for initializing the ad5593r device.

These initialization functions create a driver object for either ad5592r or
ad5593r and properly initialize their operating parameters based on the 
initialization values passed.


Register Access Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~

Register access functions take the form of ``ad5592r_base_reg_[op]`` where 
[op] is either read or write.

``ad5592r_base_reg_read()``
``ad5592r_base_reg_write()``


DAC Conversion and Channel Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DAC write APIs take the pattern of ``[chip]_write_dac``. Since AD559xR uses
DACs that have 12-bit resolution, valid values are from 0 up to (2**12)-1 or
0 to 4095.

``ad5592r_write_dac()``
``ad5593r_write_dac()``

As with every DAC, digital value translations are dependent on the VRef setting
that was passed during driver initialization. 

ADC Configuration and Conversion Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ADC read APIs take the pattern of ``[chip]_read_adc``. Since AD559xR uses ADCs
that have 12-bit resolution, values read out from the ADCs are from the range 
0 to (2**12)-1 or 0 to 4095.

``ad5592r_read_adc()``
``ad5593r_read_adc()``

As with every ADC, digital value translations are dependent on the VRef setting
that was passed during driver initialization. 

Digital I/O and GPIO Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GPIO APIs are unified for ad5592r and ad5593r it is the device parameter that
determines which lowlevel driver to use. Use ``ad5592r_gpio_get()`` to read
from either ad5592r or ad5593r pins and then use ``ad5592r_gpio_set()`` to 
write to the same.

For configuration, GPIO direction can be set with the 
``ad5592r_gpio_direction_[dir]`` where [dir] can be either input or output.


System Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All of the APIs below can be used on either ad5592r or ad5593r.

``ad5592r_software_reset()`` resets the chip.
``ad5592r_set_adc_range()`` updates the adc range.
``ad5592r_set_dac_range()`` updates the dac range.
``ad5592r_power_down()`` powers down a specific channel in an ad559xr device.
``ad5592r_set_int_ref()`` selects the voltage reference for any ad559xr device.
``ad5592r_set_adc_buffer()`` turns on or off the ADC buffer for any ad559xr 
                             device.

Driver Initialization Example
------------------------------

.. code-block:: C

        #include <stdio.h>
        #include "ad5592r.h"

        int ret;

        struct ad5592r_dev *add592r;
        struct ad5592r_dev *add593r;

        /* AD5592R device initialization parameters */
        struct ad5592r_init_param ad559xr_ip = {
                .int_ref = true,
        	.spi_init = &board_spi_init,
                .i2c_init = &board_i2c_init,
                .ss_init = &board_ss_init, 
                .channel_modes = { 
                        CH_MODE_DAC,         /* channel 0 */
                        CH_MODE_ADC,         /* channel 1 */
                        CH_MODE_DAC_AND_ADC, /* channel 2 */
                        CH_MODE_DAC_AND_ADC, /* channel 3 */
                        CH_MODE_GPI, 	     /* channel 4 */
                        CH_MODE_GPO,         /* channel 5 */
                        CH_MODE_GPI,         /* channel 6 */
                        CH_MODE_GPO,         /* channel 7 */
                },
                .channel_offstate = {
                        CH_OFFSTATE_OUT_TRISTATE, /* channel 0 */
                        CH_OFFSTATE_OUT_TRISTATE, /* channel 1 */
                        CH_OFFSTATE_OUT_TRISTATE, /* channel 2 */
                        CH_OFFSTATE_OUT_TRISTATE, /* channel 3 */
                        CH_OFFSTATE_PULLDOWN,     /* channel 4 */
                        CH_OFFSTATE_OUT_LOW,      /* channel 5 */
                        CH_OFFSTATE_PULLDOWN,     /* channel 6 */
                        CH_OFFSTATE_OUT_LOW,      /* channel 7 */
                },
                .adc_range = ZERO_TO_VREF,
                .dac_range = ZERO_TO_VREF,
                .adc_buf = false,
        };
        uint8_t adc_val;

        /* Initialize the AD5592R device */
        ret = ad5592r_init(&ad5592r, &ad559xr_ip);
        if (ret) {
                printf("AD5592R initialization failed (err: %d)\n", ret);
                return ret;
        }

        /* Initialize the AD5593R device */
        ret = ad5593r_init(&ad5593r, &ad559xr_ip);
        if (ret) {
                printf("AD5593R initialization failed (err: %d)\n", ret);
                return ret;
        }

        /* write to AD5592R DAC channel 0 */
        ret = ad5592r_write_dac(ad5592r, 0, 1024);
        if (ret)
                return ret;

        /* read from AD5592R ADC channel 1 */
        ret = ad5592r_read_adc(ad5592r, 1, &adc_val);
        if (ret)
                return ret;

        /* write to AD5593R DAC channel 0 */
        ret = ad5593r_write_dac(ad5593r, 0, 1024);
        if (ret)
                return ret;

        /* read from AD5593R ADC channel 1 */
        ret = ad5593r_read_adc(ad5593r, 1, &adc_val);
        if (ret)
                return ret;

        /* Cleanup resources */
        ad5593r_remove(ad5593r);
        ad5592r_remove(ad5592r);


AD5592R/AD5593R IIO Support
===========================

The AD5592R/AD5593R IIO driver comes on top of the base ad559xr drivers and 
offer support support for interfacing IIO clients through lib IIO. Channels and
attributes are modeled closely on the Linux IIO channels for the same device.


Device Channels
~~~~~~~~~~~~~~~

The ad559xr IIO driver supports creation of four kinds of channels namely: 
ADC, DAC, GPI and GPO channels.


ADC channels & attributes
-------------------------

ADC channels are created for all ad559xr device pins that are configured to be
ADC or ADC-DAC. ADC channels support buffered streaming and have the following 
attributes.

+-----------------------------------------------------------------------------+
| ATTRIBUTE | DESCRIPTION                                               | R/W |
+===========+===========================================================+=====+
|       raw | represents raw ADC 12-bit sensor reading directly from    |  R  |
|           | ad559xr.                                                  |     |
+-----------+-----------------------------------------------------------------+
|     scale | conversion value applied to the raw ADC value to convert  |     |
|           | to voltage. This is dependent on the configured VRef that | R/W |
|           | is used.                                                  |     |
+-----------+-----------------------------------------------------------------+
|    offset | represents the adjustment applied to raw data before      |     |
|           | scaling to adjust for any bias or to mid-scale the zero   | R/W |
|           | reading.                                                  |     |
+-----------------------------------------------------------------------------+

DAC channels & attributes
-------------------------

DAC channels are created for all ad559xr device pins that are configured to be
DAC or ADC-DAC. DAC channels do not support buffered streaming and have the 
following attributes.

+-----------------------------------------------------------------------------+
| ATTRIBUTE  | DESCRIPTION                                              | R/W |
+===========+===========================================================+=====+
|       raw  | raw 12-bit value to write to ad559xr DAC.                | R/W |
+------------+----------------------------------------------------------------+
|     scale  | conversion value used by the DAC. This is dependent on   |  R  |
|            | the configured VRef that is used.                        |     |
+------------+----------------------------------------------------------------+
|    offset  | represents the adjustment applied to raw data before     |     |
|            | scaling to adjust for any bias or to mid-scale the zero  |  R  |
|            | reading.                                                 |     |
+------------+----------------------------------------------------------------+
| power_down | when configured as a DAC, we can further control when an | R/W |
|            | ad559xr pin is powered or not.                           |     |
+-----------------------------------------------------------------------------+

GPI channels & attributes
-------------------------

GPI channels are created for all ad559xr device pins that are configured to be
input GPIOs. GPI channels do not support buffered streaming and have the 
following attributes.

+-----------------------------------------------------------------------------+
| ATTRIBUTE | DESCRIPTION                                               | R/W |
+===========+===========================================================+=====+
|     label | the pin label                                             |  R  |
+-----------+-----------------------------------------------------------------+
|       raw | 0 or 1 digital value appearing on the GPI pin.            |  R  |
+-----------+-----------------------------------------------------------------+

GPO channels & attributes
-------------------------

GPO channels are created for all ad559xr device pins that are configured to be
output GPIOs. GPO channels do not support buffered streaming and have the 
following attributes.

+-----------------------------------------------------------------------------+
| ATTRIBUTE | DESCRIPTION                                               | R/W |
+===========+===========================================================+=====+
|     label | the pin label                                             |  R  |
+-----------+-----------------------------------------------------------+-----+
|       raw | 0 or 1 digital value that the GPI pin will output.        | R/W |
+-----------+-----------------------------------------------------------+-----+

TEMPERATURE channel & attributes
---------------------------------

Each device will export one special temperature channel that has the following
attributes.

+-----------+-----------------------------------------------------------+-----+
| ATTRIBUTE | DESCRIPTION                                               | R/W |
+===========+===========================================================+=====+
|       raw | represents raw ADC 12-bit sensor reading directly from    |  R  |
|           | ad559xr for temperature.                                  |     |
+-----------+-----------------------------------------------------------+-----+
|     scale | conversion value applied to the raw ADC value to convert  |     |
|           | to voltage. This is dependent on the configured VRef that |  R  |
|           | is used.                                                  |     |
+-----------+-----------------------------------------------------------+-----+
|    offset | represents the adjustment applied to raw data before      |     |
|           | scaling to adjust for any bias or to mid-scale the zero   |  R  |
|           | reading.                                                  |     |
+-----------+-----------------------------------------------------------+-----+


AD559xR IIO Initialization Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: C

        int iio_example(void)
        {
                int err;
                struct ad5592r_dev *ad5592r;
                struct ad5592r_dev *ad5593r;
                struct iio_ad559xr_desc *iio_ad5592r;
                struct iio_ad559xr_desc *iio_ad5593r;
                struct iio_ad559xr_desc *iio_ad5592r_1bit_adcdac;
                struct iio_app_desc *iio_app;
                struct iio_app_init_param iio_app_ip = {0};
                struct iio_app_device adalm_lsmspg_devices[4] = {0};
                struct iio_data_buffer ad5592r_read_data_buffer;
                struct iio_data_buffer ad5593r_read_data_buffer;

                static uint8_t ad5592r_read_buf[10 * 400];
                static uint8_t ad5593r_read_buf[10 * 400];
                
                err = ad5592r_init(&ad5592r, &ad559xr_ip);
                if (err)
                        return err;

                err = iio_ad559xr_aio_init(&iio_ad5592r, ad5592r);
                if (err)
                        return err;

                err = ad5593r_init(&ad5593r, &ad559xr_ip);
                if (err)
                        return err;

                err = iio_ad559xr_aio_init(&iio_ad5593r, ad5593r);
                if (err)
                        return err;

                /* developer notes:
                 * the gpio map here models 1:1 with the 
                 * rpi-adalm-lsmspg-overlay.dts to make the curve tracer 
                 * scripts compatible across RPI and Max326xx FTHR boards.
                 */
                struct iio_ad559xr_gpio_map gpio_map[2] = {
                        { .ad559xr = ad5592r, .map = {0, 0, 0, 0, 0, 0, 1, 1}, },
                        { .ad559xr = ad5593r, .map = {0, 0, 0, 0, 2, 2, 3, 3}, },
                };
                err = iio_ad559xr_gpio_init(&iio_ad5592r_1bit_adcdac, gpio_map, 2);
                if (err)
                        return err;

                err = lm75_init(&lm75, &lm75_ip);
                if (err)
                        return err;

                err = iio_lm75_init(&iio_lm75, lm75);
                if (err)
                        return err;

                ad5592r_read_data_buffer.size = sizeof ad5592r_read_buf;
                ad5592r_read_data_buffer.buff = ad5592r_read_buf;
                ad5593r_read_data_buffer.size = sizeof ad5593r_read_buf;
                ad5593r_read_data_buffer.buff = ad5593r_read_buf;

                adalm_lsmspg_devices[0].name = "lm75";
                adalm_lsmspg_devices[0].dev = iio_lm75;
                adalm_lsmspg_devices[0].dev_descriptor = &iio_lm75->iio_dev;
                adalm_lsmspg_devices[1].name = "ad5592r";
                adalm_lsmspg_devices[1].dev = iio_ad5592r;
                adalm_lsmspg_devices[1].dev_descriptor = &iio_ad5592r->iiodev;
                adalm_lsmspg_devices[1].read_buff = &ad5592r_read_data_buffer;
                adalm_lsmspg_devices[2].name = "ad5593r";
                adalm_lsmspg_devices[2].dev = iio_ad5593r;
                adalm_lsmspg_devices[2].dev_descriptor = &iio_ad5593r->iiodev;
                adalm_lsmspg_devices[2].read_buff = &ad5593r_read_data_buffer;

                iio_app_ip.devices = adalm_lsmspg_devices;
                iio_app_ip.nb_devices = 3;
                iio_app_ip.uart_init_params = uart_ip;
                err = init_iio_context(&iio_app_ip);
                if (err)
                        return err;

                err = iio_app_init(&iio_app, iio_app_ip);
                if (err)
                        return err;

                err = iio_app_run(iio_app);

                iio_ad559xr_remove(iio_ad5592r_1bit_adcdac);
                iio_ad559xr_remove(iio_ad5593r);
                ad5593r_remove(ad5593r);
                iio_ad559xr_remove(iio_ad5592r);
                ad5592r_remove(ad5592r);
                return err;
        }
