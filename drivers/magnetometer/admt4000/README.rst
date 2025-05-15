ADP5055 no-OS driver
====================

Supported Devices
-----------------

`ADMT4000 <https://www.analog.com/en/products/admt4000.html>`_

Overview
--------

TThe ADMT4000 is a magnetic turn count sensor capable 
of recording the number of rotations of a magnetic 
system even while the device is powered down. 
On power-up, the device can be interrogated to report the 
absolute position of the system. The absolute position 
is reported through a serial-peripheral interface (SPI). 
The ADMT4000 counts up to 46-turns of an external 
magnetic field, which increments the absolute position 
in the clockwise (CW) direction. 
The device includes three magnetic sensors, a giant 
magneto resistance (GMR) turn count sensor, which is 
used to count the number of rotations on the system, a 
GMR quadrant detecting sensor, and an anisotropic 
magnetoresistance (AMR) angle sensor. The AMR angle 
sensor is used in combination with a GMR quadrant 
detecting sensor to determine the absolute position of 
the system within 360°. Combining the GMR turn count 
sensor output with the AMR angle sensor output 
enables the device to report the position of the system 
with a high degree of angular accuracy. 

Applications
------------
* Rotation count detection and storage without power 
* Contactless absolute position measurement 
* Brushless DC motor control and positioning 
* Actuator control and positioning 

ADMT4000 Device Configuration
-----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

Status Configuration
--------------------

Register values of the device can be read using **admt4000_read** API. 
Additionally the register values can be written using **admt4000_write** API.


ADMT4000 Driver Initialization Example
--------------------------------------

.. code-block:: bash
    
        struct admt4000_desc *admt4000_desc;
        const struct no_os_spi_init_param admt_spi_ip = {
            .device_id = SPI_DEVICE_ID,
            .max_speed_hz = SPI_BAUDRATE,
            .chip_select = SPI_CS,
            .mode = NO_OS_SPI_MODE_0,
            .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
            .platform_ops = SPI_OPS,
            .extra = SPI_EXTRA,
        };
        struct admt4000_init_param admt4000_ip = {
            .spi_init = admt_spi_ip,
            .gpio_coil_rs = gpio_coil_rs_ip,
            .gpio_gpio0_busy = gpio_gpio0_busy_ip,
            .gpio_shdn_n = gpio_shdn_n_ip,
            .dev_vdd = ADMT4000_3P3V,
        };
        ret = admt4000_init(&admt4000_desc, admt4000_ip);
        if (ret)
            goto exit;



ADMT4000 no-OS IIO support
--------------------------
The ADMT4000 IIO driver comes on top of the ADMT4000 driver and offers support
for interfacing IIO clients through libiio.

ADMT4000 IIO Device Configuration
---------------------------------
Attributes
----------

* ``page - sets and reads the page of the register map to be accessed``
* ``sequencer_mode - to``
* ``angle_filt - toggles the filter for the angle sensor``
* ``conversion_mode - toggles and reads the conversion mode``
* ``h8_ctrl - toggles the H8 control``
* ``sdp_gpio0_busy - gets the status of the GPIO0 busy pin``
* ``sdp_coil_rs - sets the coil for GMR reset``

ADMT4000 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

        int ret;

        struct admt4000_iio_dev *admt4000_iio_desc;

        struct admt4000_iio_dev_init_param admt4000_iio_ip = {
            .admt4000_dev_init = &admt_ip,
        };

        struct iio_app_desc *app;
        struct iio_app_init_param app_init_param = { 0 };

        ret = admt4000_iio_init(&admt4000_iio_desc, &admt4000_iio_ip);
        if (ret)
            goto exit;

        struct iio_app_device iio_devices[] = {
            {
                .name = "admt4000",
                .dev = admt4000_iio_desc,
                .dev_descriptor = admt4000_iio_desc->iio_dev,
            },
        };

        app_init_param.devices = iio_devices;
        app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
        app_init_param.uart_init_params = uart_ip;

        ret = iio_app_init(&app, app_init_param);
        if (ret)
            goto iio_admt4000_remove;

        ret = iio_app_run(app);

        iio_app_remove(app);