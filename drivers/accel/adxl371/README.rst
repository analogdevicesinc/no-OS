ADXL371 no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADXL371`

Overview
--------

The ADXL371 is an ultra low power, 3-axis, ±200 g accelerometer that consumes
28 µA at a 2560 Hz output data rate (ODR). The ADXL371 does not power
cycle its front end to achieve its low power operation and therefore
does not run the risk of aliasing the output of the sensor.

In addition to its ultra low power consumption, the ADXL371 enables
impact detection while providing system level power reduction. The device
includes a deep multimode output first in, first out (FIFO) queue,
several activity detection modes, and a method for capturing only the peak
acceleration of overthreshold events.

Applications
------------

* Impact and Shock Detection
* Asset Health Assessment
* Portable Internet of Things (IoT) edge nodes
* Concussion and head trauma detection

Operation Modes
---------------

+----------------------------+---------------------+---------------------+---------------------+
| Mode Name                  | Description         | Configuration Bits  | Typical Use Case    |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL371_STANDBY            | Standby mode for    | 0                   | When the device     |
|                            | lowest power        |                     | needs to conserve   |
|                            | consumption.        |                     | energy and data     |
|                            |                     |                     | acquisition is not  |
|                            |                     |                     | required            |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL371_WAKE_UP            | Standby mode for    | 1                   | Ideal for simple    |
|                            | lowest power        |                     | detection of the    |
|                            | consumption.        |                     | presence or absence |
|                            |                     |                     | of motion at an     |
|                            |                     |                     | extremely low power |
|                            |                     |                     | consumption         |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL371_INSTANT_ON         | Extremely low-power | 2                   | Capture impact      |
|                            | impact / shock      |                     | events and store    |
|                            | detection           |                     | peak acceleration   |
|                            |                     |                     | values at very low  |
|                            |                     |                     | power               |
+----------------------------+---------------------+---------------------+---------------------+
| ADXL371_FULL_BW_MEASUREMENT| Full-bandwidth      | 3                   | When full-bandwidth |
|                            | measurement mode    |                     | sampling is required|
|                            |                     |                     |                     |
|                            |                     |                     |                     |
|                            |                     |                     |                     |
+----------------------------+---------------------+---------------------+---------------------+


Device Configuration
--------------------

Initialization Functions
~~~~~~~~~~~~~~~~~~~~~~~~

The ``adxl371_init`` function sets up ADXL371 accelerometer by
configuring the device for SPI or I2C communication, establishing
initial operational settings, and managing errors. The ``adxl371_reset``
function also resets the device to power-on default settings.

Communication and Control Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADXL371 driver facilitates communication and control through
functions like ``adxl371_write_reg``, ``adxl371_read_reg``, and
``adxl371_write_mask`` for manipulatibg register values.
The ``adxl371_set_op_mode`` and ``adxl371_get_op_mode`` functions allow
switching between standby and measurement modes.

Data Processing Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

To process data from ADXL371 accelerometers, first configure the FIFO to
the appropriate settings using the ``adxl371_configure_fifo`` function.

The ``adxl371_get_fifo_xyz_data_workaround`` function retrieves raw data from
the FIFO using the anomaly workaround described in the :adi:`ADXL371` datasheet.
The raw samples are placed into a data structure representing 3-axis XYZ data.

The ``adxl371_service_fifo_ev`` function is a wrapper around the
``adxl371_get_fifo_xyz_data_workaround`` function to streamline the process of
servicing FIFO events.+

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "adxl371.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   #define ACCEL_MAX_XYZ 170

   int main(void)
   {
       struct adxl371_dev *adxl371_desc = NULL;
       struct adxl371_xyz_accel_data accel_data[ACCEL_MAX_XYZ];
       uint8_t device_range;
       int ret;

       struct no_os_uart_init_param adxl371_uart_ip = {
           .device_id = 1,
           .irq_id = USART3_IRQn,
           .asynchronous_rx = true,
           .baud_rate = 115200,
           .size = NO_OS_UART_CS_8,
           .parity = NO_OS_UART_PAR_NO,
           .stop = NO_OS_UART_STOP_1_BIT,
           .extra = NULL, // Replace 'adxl371_uart_extra_ip' with appropriate value or NULL
           .platform_ops = &stm32_uart_ops,
       };

       struct no_os_spi_init_param adxl371_spi_ip = {
           .device_id = 1,
           .max_speed_hz = 8000000,
           .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &stm32_spi_ops,
           .chip_select = 14,
           .extra = NULL, // Replace 'SPI_EXTRA' with appropriate value or NULL
       };

       // Initialize the device
        struct adxl371_init_param adxl_ip = {
            .spi_init = spiInitParams,
            .gpio_int1 = int1Init,
            .int1_config = {},
            .gpio_int2 = int2Init,
            .int2_config = {},
            .comm_type =  SPI,
            .odr =  ADXL371_ODR_5120HZ,
            .bw =  ADXL371_BW_2560HZ,
            .fifo_config = {
                .fifo_format = ADXL371_XYZ_PEAK_FIFO,
                .fifo_mode = ADXL371_FIFO_STREAMED,
                .fifo_samples = ACCEL_MAX_SAMPLES
            },
            .op_mode = ADXL371_FULL_BW_MEASUREMENT,
            .act_proc_mode = ADXL371_LOOPED
        };
       ret = adxl371_init(&adxl371_desc, adxl371_ip);
       if (ret)
           goto cleanup;

       ret = adxl371_set_op_mode(adxl371_desc, ADXL371_MODE_LP);
       if (ret)
           goto cleanup;

        /** NOTE: Workaround for er001 requires internal sync to start */
        adxl371_set_internal_sync(adxl_device);

        adxl371_set_op_mode(adxl_device, ADXL371_FULL_BW_MEASUREMENT);

       /* Read # available samples into "fifo_entries" */
       ret = adxl371_get_status(adxl_device, &status1, &status2, &fifo_entries);
       if (ret)
           goto cleanup

       // Read raw acceleration data
       ret = adxl371_get_raw_xyz_workaround(adxl371_desc, accel_data, fifo_entries);
       if (ret)
           goto cleanup;

       // Print raw data
       printf("FIFO:\n%8s, %8s, %8s,\n", "X", "Y", "Z");
       for (int i = 0; i < fifo_entries / 3; i++) {
           // 12-bit samples should be sign-extended as 16-bits
           accel_data[i].x = no_os_sign_extend16(accel_data[i].x, 11);
           accel_data[i].y = no_os_sign_extend16(accel_data[i].y, 11);
           accel_data[i].z = no_os_sign_extend16(accel_data[i].z, 11);

           // Print sample data, multiplied by scale factor of 0.1g/LSB
           printf("%d, %8.2lf, %8.2lf, %8.2lf,\n",
               i + 1,
               (double)(accel_data[i].x * 0.1),
               (double)(accel_data[i].y * 0.1),
               (double)(accel_data[i].z * 0.1));
       }

   cleanup:
       if (adxl371_desc)
           adxl371_remove(adxl371_desc);

       // Deinitialize SPI
       no_os_spi_remove(&adxl371_spi_ip);

       // Deinitialize UART
       no_os_uart_remove(&adxl371_uart_ip);

       return ret;
   }


Impact Detection Example
------------------------

The ADXL371 Impact Detection features allow the device to capture impact events
as both single peak samples in the MAX_PEAK registers, or as sequences of peaks
inside the FIFO. To capture full peak events, "Triggered Mode" should be used.

The below example shows an example configuration for using the ADXL371 in
Impact Detection use cases.

.. code-block:: C

   #include "adxl371.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   // ACT/INACT thresholds in units of 0.1g
   #define ACT_THRESHOLD 1000
   #define ACT_TIME 0
   #define INACT_THRESHOLD ACT_THRESHOLD
   #define INACT_TIME 0

   #define ACCEL_MAX_SAMPLES 512
   #define ACCEL_MAX_XYZ 170

   void adxl_configure_impact(struct adxl371_dev *dev)
   {
       int err;
       struct adxl371_irq_config int1_config = {
           .activity = false, .inactivity = false,
           .data_rdy = false, .fifo_rdy = false,
           .fifo_full = false, .fifo_ovr = false,
           .awake = false, .low_operation = false,
       };
       struct adxl371_irq_config int2_config = {
           .activity = false, .inactivity = false,
           .data_rdy = false, .fifo_rdy = false,
           .fifo_full = true, .fifo_ovr = false,
           .awake = true, .low_operation = false,
       };

       /* Configure in standby mode */
       adxl371_set_op_mode(dev, ADXL371_STANDBY);

       /* Generic ADXL Part parameters */
       adxl371_set_autosleep(dev, true);
       adxl371_set_bandwidth(dev, ADXL371_BW_2560HZ);
       adxl371_set_odr(dev, ADXL371_ODR_5120HZ);
       adxl371_set_wakeup_rate(dev, ADXL371_WUR_65ms);
       adxl371_set_act_proc_mode(dev, ADXL371_LOOPED);

       /* Set Instant On threshold (Low threshold 10-15 G) */
       adxl371_set_instant_on_th(dev, ADXL371_INSTANT_ON_HIGH_TH);

       /** NOTE: Recommended FIFO Configurations for Impact Detection
        *
        * - BYPASSED, XYZ_PEAK_FIFO:   Store only the maximum peak event,
        *         no samples in FIFO, only in MAX_PEAK registers
        * - STREAMED, XYZ_PEAK_FIFO:   Capture single peak sample events
        *         between ACT/INACT
        *
        */

       /* Put fifo in Peak Detect and streamed Mode */
       dev->fifo_config.fifo_mode = ADXL371_FIFO_STREAMED;
       dev->fifo_config.fifo_format = ADXL371_XYZ_PEAK_FIFO;
       dev->fifo_config.fifo_samples = ACCEL_MAX_SAMPLES;
       adxl371_configure_fifo(dev,
                      dev->fifo_config.fifo_mode,
                      dev->fifo_config.fifo_format,
                      dev->fifo_config.fifo_samples,
                      ADXL371_STANDBY);

       /* Set activity/inactivity threshold */
       adxl371_set_activity_threshold(dev, ADXL371_ACTIVITY, ACT_THRESHOLD,
            true, true);
       adxl371_set_activity_threshold(dev, ADXL371_INACTIVITY, INACT_THRESHOLD,
            true, true);

       /* Set activity/inactivity time settings */
       adxl371_set_activity_time(dev, ACT_TIME);
       adxl371_set_inactivity_time(dev, INACT_TIME);

       /* Set instant-on interrupts and activity interrupts */
       adxl371_interrupt_config(dev, int1_config, int2_config);

       /* Set filter settle time */
       adxl371_set_filter_settle(dev, ADXL371_FILTER_SETTLE_4divODR);

       /* Disable LPF */
       adxl371_write_mask(dev,
                  ADXL371_POWER_CTL,
                  ADXL371_POWER_CTL_LPF_DIS_MSK,
                  ADXL371_POWER_CTL_LPF_DIS_MODE(0b1));

       /* Disable HPF */
       adxl371_write_mask(dev,
                  ADXL371_POWER_CTL,
                  ADXL371_POWER_CTL_HPF_DIS_MSK,
                  ADXL371_POWER_CTL_HPF_DIS_MODE(0b1));

       /*
        * Enable full-BW Measurement to set the reference
        * if using internally referenced act/inact thresholds
        */
       adxl371_set_op_mode(dev, ADXL371_FULL_BW_MEASUREMENT);
       no_os_mdelay(10);
       adxl371_set_op_mode(dev, ADXL371_STANDBY);

       /* Clear fifo completely before enabling Instant On mode */
       err = adxl371_get_status(adxl_device, &status1, &status2, &fifo_entries);
       if (err) {
           printf("ERR Getting Status %d\n", err);
           while(1);
       }
       adxl371_get_fifo_xyz_data_workaround(adxl_device, accel_data, fifo_entries);

       /* Clear all status flags */
       adxl371_get_status(adxl_device, &status1, &status2, &fifo_entries);

       /* Set operation mode to Instant-On */
       adxl371_set_op_mode(dev, ADXL371_INSTANT_ON);
   }


Triggered Mode Example
----------------------

The "Triggered Mode" feature of the ADXL371 allows for full event detection
based off of the ACTIVITY and INACTIVITY interrupts. These require Configuration
of both the thresholds and time windows for the activity /inactivity events,
as well as the coupling between them.

The below example shows an example configuration for using the ADXL371 in
triggered mode.

.. code-block:: C

   #include "adxl371.h"
   #include "no_os_i2c.h"
   #include "no_os_spi.h"
   #include "no_os_alloc.h"
   #include "no_os_delay.h"

   // ACT/INACT thresholds in units of 0.1g
   #define ACT_THRESHOLD 1000
   #define ACT_TIME 0
   #define INACT_THRESHOLD ACT_THRESHOLD
   #define INACT_TIME 0

   #define ACCEL_MAX_SAMPLES 512
   #define ACCEL_MAX_XYZ 170

   void adxl_configure_triggered_mode(struct adxl371_dev *dev)
   {
       int err;
       struct adxl371_irq_config int1_config = {
           .activity = false, .inactivity = false,
           .data_rdy = false, .fifo_rdy = false,
           .fifo_full = false, .fifo_ovr = false,
           .awake = false, .low_operation = false,
       };
       /** NOTE: Triggered mode should react on FIFO_RDY interrupt */
       struct adxl371_irq_config int2_config = {
           .activity = false, .inactivity = false,
           .data_rdy = false, .fifo_rdy = true,
           .fifo_full = false, .fifo_ovr = false,
           .awake = false, .low_operation = false,
       };

       /* Configure in standby mode */
       adxl371_set_op_mode(dev, ADXL371_STANDBY);

       /* Generic ADXL Part parameters */
       adxl371_set_bandwidth(dev, ADXL371_BW_2560HZ);
       adxl371_set_odr(dev, ADXL371_ODR_2560HZ);
       adxl371_set_act_proc_mode(dev, ADXL371_LOOPED);

       /** NOTE: FIFO Configurations
        * Triggered mode can be used to capture events with XYZ_FIFO sample mode
        *
        * Recommended Configurations:
        * - TRIGGERED, XYZ_FIFO:   Capture full events surrounding ACT / INACT triggers
        *                          Num samples refers in this case to the # of samples
        *                          after an event
        *
       */

       /* Put fifo in Peak Detect and streamed Mode */
       dev->fifo_config.fifo_mode = ADXL371_FIFO_TRIGGERED;
       dev->fifo_config.fifo_format = ADXL371_XYZ_FIFO;
       dev->fifo_config.fifo_samples = EVENTS_TRAILING_SAMPLES;
       adxl371_configure_fifo(dev,
                      dev->fifo_config.fifo_mode,
                      dev->fifo_config.fifo_format,
                      dev->fifo_config.fifo_samples,
                      ADXL371_STANDBY);

       /* Set activity/inactivity threshold */
       adxl371_set_activity_threshold(dev, ADXL371_ACTIVITY, ACT_THRESHOLD,
            true, true);
       adxl371_set_activity_threshold(dev, ADXL371_INACTIVITY, INACT_THRESHOLD,
            true, true);

       /* Set activity/inactivity time settings */
       adxl371_set_activity_time(dev, ACT_TIME);
       adxl371_set_inactivity_time(dev, INACT_TIME);

       /* Set instant-on interrupts and activity interrupts */
       adxl371_interrupt_config(dev, int1_config, int2_config);

       /* Set filter settle time */
       adxl371_set_filter_settle(dev, ADXL371_FILTER_SETTLE_4divODR);

       /* Disable LPF */
       adxl371_write_mask(dev,
                  ADXL371_POWER_CTL,
                  ADXL371_POWER_CTL_LPF_DIS_MSK,
                  ADXL371_POWER_CTL_LPF_DIS_MODE(0b1));

       /* Disable HPF */
       adxl371_write_mask(dev,
                  ADXL371_POWER_CTL,
                  ADXL371_POWER_CTL_HPF_DIS_MSK,
                  ADXL371_POWER_CTL_HPF_DIS_MODE(0b1));

       /*
        * Enable full-BW Measurement to set the reference
        * if using internally referenced act/inact thresholds
        */
       adxl371_set_op_mode(dev, ADXL371_FULL_BW_MEASUREMENT);
       MXC_Delay(MXC_DELAY_MSEC(10));
       adxl371_set_op_mode(dev, ADXL371_STANDBY);

       /* Clear fifo completely before enabling */
       err = adxl371_get_status(adxl_device, &status1, &status2, &fifo_entries);
       if (err) {
           printf("ERR Getting Status %d\n", err);
           while(1);
       }
       adxl371_get_fifo_xyz_data_workaround(adxl_device, accel_data, fifo_entries);

       /* Clear all status flags */
       adxl371_get_status(adxl_device, &status1, &status2, &fifo_entries);

       /* Set operation mode to Full BW Measurement */
       adxl371_set_op_mode(dev, ADXL371_FULL_BW_MEASUREMENT);
       return;
   }
