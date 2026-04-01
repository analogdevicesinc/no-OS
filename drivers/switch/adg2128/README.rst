ADG2128 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADG2128 <https://www.analog.com/ADG2128>`_

Overview
--------

The ADG2128 is an I2C-compatible CMOS analog crosspoint switch with an 8 x 12
matrix, providing 96 bidirectional switch channels. It features double-buffered
input logic, allowing simultaneous update of multiple switches via the LDSW
command. The I2C interface supports standard, full-speed, and high-speed
(3.4 MHz) operation.

The device offers up to 300 MHz bandwidth and a maximum on resistance of
35 ohms, with low quiescent current (less than 20 uA). It operates with dual
+/-5 V or single +12 V supplies and is housed in a 32-lead LFCSP package.
A RESET option is available to disable all switches at power-on or manually.

Applications
------------

* AV switching in TV
* Automotive infotainment
* AV receivers
* CCTV
* Ultrasound applications
* KVM switching
* Telecom applications
* Test equipment/instrumentation
* PBX systems

ADG2128 no-OS IIO Support
-------------------------

The ADG2128 IIO driver provides support for interfacing IIO clients through
libiio. It exposes the device's I2C register interface through the IIO
debug register read/write mechanism.

ADG2128 IIO Device Configuration
---------------------------------

Initialization
~~~~~~~~~~~~~~

The ``adg2128_iio_init`` function initializes the ADG2128 IIO device.
It takes an already initialized I2C descriptor as input, allocates memory for
the device structure, and associates it with the IIO device definition that
provides debug register read and write capabilities.

Removal
~~~~~~~

The ``adg2128_iio_remove`` function frees the allocated resources for the
ADG2128 IIO device.

ADG2128 IIO Driver Initialization Example
------------------------------------------

.. code-block:: c

        struct adg2128_iio_dev *adg2128_iio;
        struct no_os_i2c_desc *i2c_desc;

        struct no_os_i2c_init_param i2c_ip = {
                .device_id = I2C_DEVICE_ID,
                .max_speed_hz = 100000,
                .slave_address = 0x70,
                .platform_ops = I2C_OPS,
                .extra = I2C_EXTRA,
        };

        ret = no_os_i2c_init(&i2c_desc, &i2c_ip);
        if (ret)
                goto error;

        ret = adg2128_iio_init(&adg2128_iio, i2c_desc);
        if (ret)
                goto error;

        struct iio_app_device devices[] = {
                {
                        .name = "adg2128",
                        .dev = adg2128_iio,
                        .dev_descriptor = adg2128_iio->iio_dev,
                },
        };

        struct iio_app_desc *app;
        struct iio_app_init_param app_init_param = { 0 };

        app_init_param.devices = devices;
        app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
        app_init_param.uart_init_params = uart_ip;

        ret = iio_app_init(&app, app_init_param);
        if (ret)
                goto error;

        return iio_app_run(app);
