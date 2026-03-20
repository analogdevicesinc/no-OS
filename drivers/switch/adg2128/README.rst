ADG2128 no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADG2128`

Overview
--------

The ADG2128 is an I²C-compatible CMOS analog crosspoint switch with an
8×12 matrix, providing 96 bidirectional switch channels. Configuration
is achieved via the I²C interface, supporting standard, full-speed, and
high-speed (3.4 MHz) operation. It allows for simultaneous switch
configurations and bulk updates through the LDSW command. A RESET option
is available to disable all switches at power-on or manually. The device
can operate with dual supplies up to ±15V or a single +12V supply and is
housed in a 32-lead LFCSP package. It is suitable for applications in AV
switching, automotive infotainment, telecommunications, and
instrumentation.

Applications
------------

- AV switching in TV
- Automotive infotainment
- AV receivers
- CCTV
- Ultrasound applications
- KVM switching
- Telecom applications
- Test equipment/instrumentation
- PBX systems

Operation Modes
---------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| Transparent     | Switch          | LDSW set to 1   | Synchronous     |
| Mode            | positions       |                 | signal          |
|                 | change as soon  |                 | transmission    |
|                 | as the new      |                 | requiring       |
|                 | command is      |                 | instantaneous   |
|                 | written and     |                 | switch updates. |
|                 | LDSW is set to  |                 |                 |
|                 | 1.              |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Latched Mode    | Switch          | LDSW set to 0   | Applications    |
|                 | positions are   | until final     | needing         |
|                 | updated after   | command         | simultaneous    |
|                 | writing the     |                 | channel         |
|                 | final command,  |                 | updates, such   |
|                 | allowing all    |                 | as coordinated  |
|                 | switches to     |                 | signal          |
|                 | change          |                 | switching.      |
|                 | simultaneously  |                 |                 |
|                 | when LDSW is    |                 |                 |
|                 | set to 1 on the |                 |                 |
|                 | last command.   |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

IIO Support
------------

IIO Device Configuration
------------------------

Initialization
~~~~~~~~~~~~~~

The ``adg2128_iio_init`` function allocates memory for the ADG2128 device
structure and associates it with an I2C descriptor. It links to the
iio_device instance, defining the device’s read and write registers
capability.

Control
~~~~~~~

Within the IIO context, ``adg2128_read_register2`` and
``adg2128_write_register2`` manage low-level I2C operations, setting the
device’s slave address, and performing data transfers for register
operations.

Utility
~~~~~~~

The ``adg2128_iio_remove`` function ensures proper memory management by
freeing the allocated resources, preventing memory leaks during device
shutdown.

IIO Device Initialization Example
---------------------------------

.. code-block:: C

   #include <stdio.h>
   #include "no_os_i2c.h"
   #include "no_os_error.h"
   #include "no_os_util.h"
   #include "iio_app.h"
   #include "iio_adg2128.h"
   #include "platform_init.h" // Platform-specific init

   int main(void)
   {
       int ret;
       struct iio_app_desc *app;
       struct no_os_i2c_desc *i2c_desc;
       struct adg2128_iio_dev *adg2128_iio_dev;

       // I2C Init parameters
       struct no_os_i2c_init_param i2c_ip = {
           .device_id = 0,
           .max_speed_hz = 100000,
           .slave_address = 0x70,  // Adjust if hardware uses another address
           .platform_ops = &platform_i2c_ops,  // Set platform ops
           .extra = NULL
       };

       // Initialize platform (GPIOs, UART, clocks, etc.)
       ret = platform_init();
       if (ret) {
           printf("Platform init failed: %d\n", ret);
           return ret;
       }

       // Initialize I2C
       ret = no_os_i2c_init(&i2c_desc, &i2c_ip);
       if (ret) {
           printf("I2C init failed: %d\n", ret);
           return ret;
       }

       // Initialize ADG2128 IIO device
       ret = adg2128_iio_init(&adg2128_iio_dev, i2c_desc);
       if (ret) {
           printf("ADG2128 IIO init failed: %d\n", ret);
           return ret;
       }

       // Register IIO device
       struct iio_app_device devices[] = {
           {
               .name = "adg2128",
               .dev = adg2128_iio_dev,
               .dev_descriptor = adg2128_iio_get_descriptor(adg2128_iio_dev)
           }
       };

       // Initialize the IIO app
       ret = iio_app_init(&app);
       if (ret) {
           printf("IIO app init failed: %d\n", ret);
           return ret;
       }

       // Run the IIO server
       ret = iio_app_run(app, devices, NO_OS_ARRAY_SIZE(devices));
       if (ret) {
           printf("IIO app run failed: %d\n", ret);
           return ret;
       }

       // Cleanup (optional depending on your system)
       // adg2128_iio_remove(adg2128_iio_dev);
       // no_os_i2c_remove(i2c_desc);

       return 0;
   }
