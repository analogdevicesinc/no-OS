MAX31855 no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`MAX31855`

Overview
--------

The MAX31855 is a thermocouple-to-digital converter that provides
cold-junction compensation and digitizes signals from K-, J-, N-, T-,
S-, R-, or E-type thermocouples. It delivers data in a 14-bit,
SPI-compatible, read-only format with 0.25°C resolution. The device
measures temperatures between -270°C to +1800°C, achieving ±2°C accuracy
for K-type thermocouples within -200°C to +700°C. Fault detection
capabilities include sensing open thermocouples and short circuits to
GND or VCC, ensuring reliability in industrial settings such as HVAC
systems and process control.

Applications
------------

- HVAC systems
- Process control equipment
- Industrial temperature monitoring

Operating Modes
---------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| Normal          | Reads           | IIO_TEMP        | Typical         |
| Temperature     | temperature     | address: 0 & 2  | industrial      |
|                 | from            |                 | measurement and |
|                 | thermocouple    |                 | monitoring      |
|                 | and board’s     |                 |                 |
|                 | internal        |                 |                 |
|                 | temperature     |                 |                 |
|                 | sensor          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Fault Detection | Checks for      | MAX31855        | Ensures         |
|                 | faults such as  | _GET_FAULT_BIT  | reliability and |
|                 | open            |                 | safety in       |
|                 | thermocouple,   |                 | monitoring      |
|                 | short circuits  |                 | systems         |
|                 | to GND/ VCC     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~~

The ``max31855_init`` function sets up the device descriptor and SPI
communication for the MAX31855. It handles memory allocation and SPI
initialization to ensure the system can interface with the thermocouple
sensor.

Resource Management
~~~~~~~~~~~~~~~~~~~

``max31855_remove`` handles the deallocation of resources used by
MAX31855. It frees all allocated memory to maintain system stability and
prevent memory leaks.

Data Reading
~~~~~~~~~~~~

The ``max31855_read_raw`` function retrieves raw data from the MAX31855
register using SPI. It checks and filters data for faults before
processing or returning it.

Temperature Conversion
~~~~~~~~~~~~~~~~~~~~~~

``max31855_read_temp`` processes raw register data to provide
thermocouple and internal temperatures in Celsius. It includes sign
extension and scaling for accurate temperature measurement, enabling
easy application and display.

Driver Initialization Example
-----------------------------

.. code-block:: C

   int basic_example_main()
   {
       struct max31855_decimal thermocouple_temp;
       struct max31855_decimal ambient_temp;
       struct max31855_dev *dev;
       int ret;

       ret = max31855_init(&dev, &max31855_ip);
       if (ret)
           goto error;

       while (1) {
           pr_info("%c", 27);
           pr_info("%c", '[');
           pr_info("%c", '2');
           pr_info("%c", 'J');

           ret = max31855_read_temp(dev, &thermocouple_temp, &ambient_temp);
           if (ret)
               goto free_dev;

           pr_info("Thermocouple temperature %.02d.%.02d\n", thermocouple_temp.integer,
                   thermocouple_temp.decimal);
           pr_info("Ambient temperature %.02d.%.02d\n", ambient_temp.integer,
                   ambient_temp.decimal);

           no_os_mdelay(2000);
       }

   free_dev:
       max31855_remove(dev);
   error:
       pr_info("Error!\r\n");
       return 0;
   }

IIO Device Configuration
------------------------

Initialization and Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``max31855_iio_init`` initializes the MAX31855 IIO driver by setting
up the necessary device descriptor infrastructure, including memory
allocation and device configuration, and associates the ``iio_device``
structure with the device descriptor. The ``max31855_iio_remove``
handles cleanup, freeing resources allocated during initialization.

Data Handling and Reading
~~~~~~~~~~~~~~~~~~~~~~~~~

The ``max31855_iio_read_raw`` function manages fetching raw temperature
data from the MAX31855 device, processes it, and formats the data into
the provided buffer. ``max31855_iio_read_scale`` handles scale attribute
requests, setting and formatting scale values for the channels to ensure
accurate sensor reading interpretation.

Channel Management
~~~~~~~~~~~~~~~~~~

The ``max31855_iio_update_channels`` updates active channels within the
MAX31855 IIO device using a bitmask to modify the internal configuration
and recalculate the count of active channels, allowing selective
monitoring and data acquisition.

IIO Initialization Example
--------------------------

.. code-block:: c

   #include "iio_example.h"
   #include "common_data.h"

   #define DATA_BUFFER_SIZE 400

   uint8_t iio_data_buffer[DATA_BUFFER_SIZE * sizeof(int32_t)];

   int iio_example_main() {
       int ret;
       struct max31855_iio_dev *max31855_iio_dev;
       struct max31855_iio_dev_init_param max31855_iio_ip;
       struct iio_app_desc *app;
       struct iio_data_buffer dbuff = {
           .buff = (void *)iio_data_buffer,
           .size = DATA_BUFFER_SIZE * sizeof(int32_t)
       };

       struct iio_app_init_param app_init_param = {0};

       max31855_iio_ip.max31855_dev_init = &max31855_ip;
       ret = max31855_iio_init(&max31855_iio_dev, &max31855_iio_ip);
       if (ret)
           return ret;

       struct iio_app_device iio_devices[] = {
           {
               .name = "maxim_thermocouple",
               .dev = max31855_iio_dev,
               .dev_descriptor = max31855_iio_dev->iio_dev,
               .read_buff = &dbuff,
           },
       };

       app_init_param.devices = iio_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
       app_init_param.uart_init_params = uip;

       // Initialize IIO application
       ret = iio_app_init(&app, &app_init_params);
       if (ret)
           goto error_remove_device;

       // Run the IIO application
       ret = iio_app_run(app);
       if (ret)
           goto error_remove_app;

       return ret;

   error_remove_app:
       iio_app_remove(app);
   error_remove_device:
       max31855_iio_remove(max31855_iio_dev);

       return ret;
   }
