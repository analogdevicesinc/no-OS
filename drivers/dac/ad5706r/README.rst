AD5706R no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------
`AD5706R <https://www.analog.com/en/products/ad5706r.html>`_

Overview
--------

The `AD5706R <https://www.analog.com/en/products/ad5706r.html>`_ is a 4-channel,
16-bit resolution, low noise programmable current output, digital-to-analog
converter (DAC). The device is ideal for applications such as photonics control
and current-mode biasing.

The device features an integrated 2.5V voltage reference, along with load DAC,
A/B toggle, and dither functions. An onboard multiplexer allows for monitoring
of supply and output voltages, output currents, and the internal die
temperature.

The `AD5706R <https://www.analog.com/en/products/ad5706r.html>`_ 
supports several programmable output current ranges, with a maximum of 300mA.
With addressable pins, up to four devices can share a single SPI bus for
communication.

Each DAC channel operates independently with its own positive power supply rail
(PVDDx) that can range from 1.65V to 3.6V, optimizing both power efficiency and
thermal performance. The device requires a 2.9V to 3.6V AVDD supply and is rated
for operation across a wide temperature range from −40°C to +125°C.

Applications
------------

* Photonics Control
* Optical Communications
* LED Driver Programmable Current Source
* Current Mode Biasing

AD5706R Driver configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **ad5706r_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the AD5706R by performing a soft reset, putting the device in a known 
state. Inside **ad5706r_init** API, **ad5706r_set_device_spi** and
**ad5706r_device_config** APIs are called to set the device registers
for SPI configuration and LDAC modes, respectively.

Utility and Support
~~~~~~~~~~~~~~~~~~~

Utility functions, including **ad5706r_remove**, manage device cleanup
and resource deallocation. 

Device Initialization Example
-----------------------------

.. code-block:: C

        #include "ad5706r.h"
        struct no_os_spi_init_param spi_params_init = {
                .device_id = SPI_DEVICE_ID, 
                .max_speed_hz = SPI_BAUDRATE,
                .chip_select = SPI_CS, 
                .mode = NO_OS_SPI_MODE_0,
                .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
                .platform_ops = SPI_OPS,
                .extra = SPI_EXTRA,
        };
        struct ad5706r_init_param ad5706r_user_init = {
                .spi_init_prm = &spi_params_init,
                .gpio_ldac_tgp = &gpio_ldac_init,
                .gpio_reset = &gpio_reset_init,
                .gpio_shdn = &gpio_shdn_init,
                .dev_addr = 0,
                .crc_en = false,
                .vref_enable = 0,
                .spi_cfg.addr_asc = 0,
                .spi_cfg.single_instr = 0,
                .spi_cfg.short_instr = 0,
                .spi_cfg.stream_mode_length = 0,
                .op_mode = {AD5706R_SHUTDOWN_SW, 
                        AD5706R_SHUTDOWN_SW,
                        AD5706R_SHUTDOWN_SW, 
                        AD5706R_SHUTDOWN_SW},
                .ldac_cfg = {
                        .edge_trig = {AD5706R_RISING_EDGE_TRIG, 
                                AD5706R_RISING_EDGE_TRIG,
                                AD5706R_RISING_EDGE_TRIG, 
                                AD5706R_RISING_EDGE_TRIG},
                },
        };
        int main()
        {
                int ret;
                struct ad5706r_dev *ad5706r_dac;
                ret = ad5706r_init(&ad5706r_dac, &ad5706r_user_init);
                if (ret) {
                        pr_err("ad5706r_init failed with code: %d\n", ret);
                        return ret;
                }
                ret = run_example(ad5706r_dac);
                if (ret) {
                        pr_debug("Example failed with code: %d\n", ret);
                        return ret;
                }
                ad5706r_remove(ad5706r_dac);
                return 0;
        };
