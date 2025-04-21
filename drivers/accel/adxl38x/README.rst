ADXL38x no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADXL380`
- :adi:`ADXL382`

Overview
--------

The ADXL380 and ADXL382 accelerometers are differentiated by their
selectable measurement ranges, influencing their application
suitability. The ADXL380 supports measurement ranges of ±4g, ±8g, and
±16g, making it optimal for low to medium acceleration applications.
These settings are particularly suitable for use cases like wearables,
robotics, and low-motion detection, which require capturing subtle
movements. The ADXL382 offers extended measurement ranges of ±15g, ±30g,
and ±60g. This expanded capability for ADXL382 device enhances its
adaptability for applications requiring higher sensitivity and broader
range detection, such as structural health monitoring and seismic
imaging.

Applications
------------

- Audio and active noise cancellation
- Robotics
- Wearables and low motion detection
- Seismic imaging
- Condition-based monitoring
- Structural health monitoring

Operation Modes
---------------

+----------------------------+---------------------+---------------------+---------------------+
| Mode Name                  | Description         | Configuration Bits  | Typical Use Case    |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_STDBY         | Standby mode for    | 0                   | When the device     |
|                            | lowest power        |                     | needs to conserve   |
|                            | consumption.        |                     | energy and data     |
|                            |                     |                     | acquisition is not  |
|                            |                     |                     | required.           |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_HRT_SND       | Heart sound mode    | 1                   | Specific use in     |
|                            | for capturing       |                     | applications        |
|                            | heart rate.         |                     | requiring heart     |
|                            |                     |                     | sound detection.    |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_ULP           | Ultra-low power     | 2                   | Ideal for           |
|                            | mode for minimal    |                     | applications with   |
|                            | power usage.        |                     | strict power        |
|                            |                     |                     | constraints needing |
|                            |                     |                     | basic functionality.|
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_VLP           | Very low power mode | 3                   | Efficient data      |
|                            | balancing power and |                     | acquisition with    |
|                            | performance.        |                     | limited power       |
|                            |                     |                     | availability.       |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_LP            | Low power mode for  | 4                   | General use where   |
|                            | moderate power      |                     | some power          |
|                            | consumption and     |                     | conservation is     |
|                            | functionality.      |                     | needed without      |
|                            |                     |                     | sacrificing too     |
|                            |                     |                     | much functionality. |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_LP_SER_ULP_OP | Low power serial    | 6                   | Use in scenarios    |
|                            | ultra-low power     |                     | where serial        |
|                            | operation mode.     |                     | communication is    |
|                            |                     |                     | key while           |
|                            |                     |                     | maintaining ultra-  |
|                            |                     |                     | low power.          |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_LP_SER_VLP_OP | Low power serial    | 7                   | For applications    |
|                            | very low power      |                     | requiring serial    |
|                            | operation mode.     |                     | communication with  |
|                            |                     |                     | very low power      |
|                            |                     |                     | constraints.        |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_RBW           | Reduced bandwidth   | 8                   | When bandwidth      |
|                            | mode for limited    |                     | limitations are     |
|                            | data transmission   |                     | necessary to match  |
|                            | needs.              |                     | specific            |
|                            |                     |                     | transmission        |
|                            |                     |                     | capabilities.       |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_RBW_SER_ULP_OP| Reduced bandwidth   | 10                  | Balancing minimal   |
|                            | serial ultra-low    |                     | bandwidth when      |
|                            | power operation.    |                     | serial communication|
|                            |                     |                     | and ultra-low power |
|                            |                     |                     | modes are required. |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_RBW_SER_VLP_OP| Reduced bandwidth   | 11                  | Use in high-        |
|                            | serial very low     |                     | efficiency scenarios|
|                            | power operation.    |                     | that require low    |
|                            |                     |                     | bandwidth serial    |
|                            |                     |                     | communication.      |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_HP            | High power mode for | 12                  | When performance    |
|                            | maximum device      |                     | and data speed are  |
|                            | performance.        |                     | prioritized, and    |
|                            |                     |                     | power is not a      |
|                            |                     |                     | constraint.         |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_HP_SER_ULP_OP | High power serial   | 14                  | Combines high       |
|                            | ultra-low power     |                     | performance with    |
|                            | operation mode.     |                     | ultra-low power in  |
|                            |                     |                     | serial              |
|                            |                     |                     | communications.     |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL38X_MODE_HP_SER_VLP_OP | High power serial   | 15                  | Maximizing          |
|                            | very low power      |                     | performance with    |
|                            | operation mode.     |                     | serial communication|
|                            |                     |                     | under very low      |
|                            |                     |                     | power conditions.   |
+----------------------------+---------------------+---------------------+---------------------+

Device Configuration
--------------------

Initialization Functions
~~~~~~~~~~~~~~~~~~~~~~~~

``The adxl38x_init`` function sets up ADXL38x accelerometers by
configuring the device for SPI or I2C communication, establishing
initial operational settings, and managing errors. The
``adxl38x_set_to_standby`` function ensures safe device states by
transitioning it to standby mode before altering registers.

Data Processing Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

To process data from ADXL38x accelerometers, the
``adxl38x_read_device_data`` and ``adxl38x_get_raw_xyz`` functions retrieve
raw data from registers, supporting SPI and I2C protocols. The
``adxl38x_accel_conv`` function converts this raw data into physical units
using scaling factors determined by the device’s configuration and range
settings.

Communication and Control Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADXL38x driver facilitates communication and control through
functions like ``adxl38x_write_device_data`` and
``adxl38x_register_update_bits`` for modifying register values. The
``adxl38x_set_op_mode`` and ``adxl38x_get_op_mode`` functions allow
switching between standby and measurement modes. Additionally,
``adxl38x_set_range`` and ``adxl38x_get_range`` functions enable adjustments
to the accelerometer’s measurement range to suit different applications.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "adxl38x.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   int adxl38x_init(struct adxl38x_dev **device,
                    struct adxl38x_init_param init_param)
   {
       struct adxl38x_dev *adxl38x_desc = NULL;
       struct adxl38x_init_param adxl38x_ip = init_param;
       uint8_t device_range;
       int16_t x, y, z;
       int ret;

       struct no_os_uart_init_param adxl38x_uart_ip = {
           .device_id = 1,
           .irq_id = USART3_IRQn,
           .asynchronous_rx = true,
           .baud_rate = 115200,
           .size = NO_OS_UART_CS_8,
           .parity = NO_OS_UART_PAR_NO,
           .stop = NO_OS_UART_STOP_1_BIT,
           .extra = NULL, // Replace 'adxl38x_uart_extra_ip' with appropriate value or NULL
           .platform_ops = &stm32_uart_ops,
       };

       struct no_os_spi_init_param adxl38x_spi_ip = {
           .device_id = 1,
           .max_speed_hz = 8000000,
           .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &stm32_spi_ops,
           .chip_select = 14,
           .extra = NULL, // Replace 'SPI_EXTRA' with appropriate value or NULL
       };

       // Initialize the device
       ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
       if (ret)
           goto cleanup;

       ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_LP);
       if (ret)
           goto cleanup;

       ret = adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_15G);
       if (ret)
           goto cleanup;

       ret = adxl38x_get_range(adxl38x_desc, &device_range);
       if (ret)
           goto cleanup;

       uint8_t device_id;
       ret = adxl38x_get_deviceID(adxl38x_desc, &device_id);
       if (ret)
           goto cleanup;

       if (adxl38x_desc->dev_type == ID_ADXL382)
           printf("Device Type = ADXL382\n");
       else
           printf("Device Type = ADXL380\n");

       no_os_mdelay(500);

       // Device is now initialized
       
       // Read raw acceleration data
       ret = adxl38x_get_raw_xyz(adxl38x_desc, &x, &y, &z);
       if (ret)
           goto cleanup;

       // Print raw data
       printf("X: %d, Y: %d, Z: %d\n", x, y, z);

   cleanup:
       if (adxl38x_desc)
           adxl38x_remove(adxl38x_desc);

       // Deinitialize SPI
       no_os_spi_remove(&adxl38x_spi_ip);

       // Deinitialize UART
       no_os_uart_remove(&adxl38x_uart_ip);

       return ret;
   }
