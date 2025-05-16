ADRV9001 no-OS Driver
====================

Supported Devices
-----------------

- :adi:`ADRV9002`

Overview
--------

The ADRV9002 is a dual-mode RF transceiver supporting both narrow-band
and wideband operations from 30 MHz to 6000 MHz, suitable for VHF, UHF,
ISM, and cellular bands with bandwidths up to 40 MHz. It features dual
transmitters and receivers, customizable synthesizers, and digital
filters for improved performance. The transceiver supports TDD and FDD
applications, offering high linearity and dynamic range, and includes
capabilities such as low power monitoring, dynamic profile switching,
rapid frequency hopping, and synchronization across multiple chips. In
addition, several auxiliary functions, such as auxiliary
analog-to-digital converters (ADCs), auxiliary digital-to-analog
converters (DACs), and general-purpose inputs/outputs (GPIOs), are
integrated to provide additional monitoring and control capability.

Applications
------------

- Mission Critical Communications
- Very High Frequency (VHF), Ultra High Frequency (UHF), 
  and Cellular to 6 GHz
- Time Division Duplex (TDD) and Frequency Division Duplex (FDD)
  Applications

Operation Modes
---------------

+-------------------------+-------------------------+-----------------+-------------------------+
| Mode Name               | Description             | Configuration   | Typical Use             |
|                         |                         | Bits            | Case                    |
+-------------------------+-------------------------+-----------------+-------------------------+
| ADI_ADRV9001_LDO_POWER_ | Sets all LDO power      | 0x00            | Used during low-power   |
| SAVING_MODE_1           | saving modes for the    |                 | operations to save      |
|                         | device.                 |                 | power.                  |
+-------------------------+-------------------------+-----------------+-------------------------+
| ADI_ADRV9001_SPI_MODE   | Allows operations       | SPI             | Used when direct        |
|                         | through the SPI         |                 | control via SPI is      |
|                         | interface.              |                 | required.               |
+-------------------------+-------------------------+-----------------+-------------------------+
| ADI_ADRV9001_CHANNEL_   | State in which the      | Channel Mask    | Initial state for       |
| PRIMED                  | channel is ready but    |                 | channels in TDD mode.   |
|                         | not transmitting/       |                 |                         |
|                         | receiving.              |                 |                         |
+-------------------------+-------------------------+-----------------+-------------------------+
| ADI_ADRV9001_CHANNEL_   | State allowing full RF  | N/A             | Used in FDD mode for    |
| RF_ENABLED              | functionality.          |                 | active transmission/    |
|                         |                         |                 | reception.              |
+-------------------------+-------------------------+-----------------+-------------------------+
| ADI_ADRV9001_INT_LO1    | Uses internal LO1 for   | LO1             | Standard operation with |
|                         | operations.             | Configuration   | internal local          |
|                         |                         |                 | oscillator.             |
+-------------------------+-------------------------+-----------------+-------------------------+

Device Configuration
--------------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ADRV9002 initialization and configuration functions handle initial
setup, ensuring optimal operation of the RF transceiver. Key functions
like ``adrv9002_setup`` integrate device channels, configure GPIOs,
manage power, and perform initial calibrations. These operations align
the device parameters with application-specific requirements,
facilitating effective device boot-up and functionality. Key operations
include implementing error-handling routines throughout the
initialization process to report and manage issues encountered, allowing
for a robust configuration adaptable to numerous RF applications.

Transceiver Path Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Transceiver path management functions in the ADRV9002 handle the
configuration of RX and TX paths, adjusting gain, transmit power, and
data interfaces to meet specific operation needs. For instance,
``adrv9002_tx_path_config`` and ``adrv9001_rx_path_config`` set channel
states and apply gain control settings through API calls. These
functions ensure that the signal paths remain robust under various
operational modes, such as TDD and FDD. They dynamically manage the
transceiver’s performance metrics to maintain signal integrity aligned
with predetermined operational conditions.

Frequency Hopping and Calibration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These functions support dynamic frequency switching and system
calibration tasks, vital for efficient multi-frequency operations. The
frequency hopping is managed through modes and configurations set during
initialization with precise timing requirements, allowing the ADRV9002
to dynamically adapt its operating frequency to limit interference, as
seen in frequency hopping functions that control PLL retunes and hop
tables efficiently. Calibration routines like
``adi_adrv9001_cals_InitCals_Run`` maintain performance by aligning
internal parameters with environmental conditions and hardware states,
offset by the frequency hopping capabilities.

Test Pattern and Performance Tuning
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Performance tuning functions for the ADRV9002 focus on validating and
enhancing signal integrity through digital interface tuning and test
pattern configurations. Functions like
``adrv9002_axi_tx_test_pattern_set`` and
``adrv9002_axi_rx_test_pattern_pn_sel`` set specific test patterns and
perform interface tuning to optimize transmission and reception paths.
These tasks involve iterative adjustments of clock and data delays,
logging test results, and storing configurations, creating a refined
tuning approach that ensures device operation at desired performance
thresholds. Functions like ``adrv9002_check_tx_test_pattern`` further
assess signal correctness, providing a comprehensive tuning suite.

Driver Initialization Example
-----------------------------

.. code-block:: C

      #include <stdio.h>
      #include "no_os_spi.h"
      #include "no_os_gpio.h"
      #include "no_os_error.h"
      #include "adrv9002.h"

          struct no_os_spi_init_param spi_init = {
              .device_id = XPAR_PS7_SPI_0_DEVICE_ID,
              .max_speed_hz = 20000000,
              .mode = NO_OS_SPI_MODE_0,
              .chip_select = 0,
              .platform_ops = &xil_spi_ops,
          };

          struct no_os_gpio_init_param gpio_reset_init = {
              .number = 46 + GPIO_OFFSET,
              .platform_ops = &xil_gpio_ops,
          };

          struct no_os_gpio_desc *gpio_reset;
          struct no_os_spi_desc *spi_desc;

          struct adrv9002_hal_cfg *phal = (struct adrv9002_hal_cfg *)devHalCfg;

          /* Initialize GPIO for reset */
          ret = no_os_gpio_get(&gpio_reset, &gpio_reset_init);
          if (ret) {
              printf("GPIO initialization failed\n");
              return ret;
          }

          ret = no_os_gpio_direction_output(gpio_reset, NO_OS_GPIO_LOW);
          if (ret) {
              printf("GPIO direction set failed\n");
              return ret;
          }

          /* Initialize SPI */
          ret = no_os_spi_init(&spi_desc, &spi_init);
          if (ret) {
              printf("SPI initialization failed\n");
              return ret;
          }

          /* Reset the device */
          ret = no_os_gpio_set_value(gpio_reset, NO_OS_GPIO_HIGH);
          if (ret) {
              printf("GPIO set value failed\n");
              return ret;
          }

          printf("Driver initialization complete\n");

          /* Cleanup */
          no_os_gpio_remove(gpio_reset);
          no_os_spi_remove(spi_desc);

          return 0;
      }

IIO Initialization Example
--------------------------

.. code-block:: C

   #define IIO_DEV_COUNT 2 // Define the number of IIO devices

   /* Define ADC and DAC buffers */
   static uint8_t adc_buffers[IIO_DEV_COUNT][1024];
   static uint8_t dac_buffers[IIO_DEV_COUNT][1024];

   /* Define ADC and DAC initialization parameters */
   static struct iio_axi_adc_init_param adc_pars[IIO_DEV_COUNT] = {
       // ...initialize ADC parameters for each device...
   };

   static struct iio_axi_dac_init_param dac_pars[IIO_DEV_COUNT] = {
       // ...initialize DAC parameters for each device...
   };

   static int32_t iio_run(struct iio_axi_adc_init_param *adc_pars,
                          struct iio_axi_dac_init_param *dac_pars)
   {
       struct iio_axi_adc_desc *adcs[IIO_DEV_COUNT] = {0};
       struct iio_axi_dac_desc *dacs[IIO_DEV_COUNT] = {0};
       struct iio_data_buffer iio_dac_buffers[IIO_DEV_COUNT];
       struct iio_data_buffer iio_adc_buffers[IIO_DEV_COUNT];
       struct iio_device *iio_descs[IIO_DEV_COUNT * 2];
       struct iio_app_device app_devices[IIO_DEV_COUNT * 2] = {0};
       struct xil_uart_init_param platform_uart_init_par = {
           .type = UART_PS,
           .irq_id = UART_IRQ_ID
       };

       struct no_os_uart_init_param iio_uart_ip = {
           .device_id = UART_DEVICE_ID,
           .irq_id = UART_IRQ_ID,
           .baud_rate = UART_BAUDRATE,
           .size = NO_OS_UART_CS_8,
           .parity = NO_OS_UART_PAR_NO,
           .stop = NO_OS_UART_STOP_1_BIT,
           .extra = &platform_uart_init_par,
           .platform_ops = &xil_uart_ops
       };

       struct iio_app_desc *app;
       struct iio_app_init_param app_init_param = {0};
       int32_t i, ret;
       int32_t a; // Linear iterator for iio_descs and app_devices flat arrays

       for (i = 0; i < IIO_DEV_COUNT; i++) {
           /* ADC setup */
           iio_adc_buffers[i].buff = adc_buffers[i];
           iio_adc_buffers[i].size = sizeof(adc_buffers[i]);
           ret = iio_axi_adc_init(&adcs[i], &adc_pars[i]);
           if (ret < 0) {
               printf("ADC initialization failed for device %d\n", i);
               goto cleanup;
           }
           a = 2 * i;
           iio_axi_adc_get_dev_descriptor(adcs[i], &iio_descs[a]);
           app_devices[a].name = adc_pars[i].rx_adc->name;
           app_devices[a].dev = adcs[i];
           app_devices[a].dev_descriptor = iio_descs[a];
           app_devices[a].read_buff = &iio_adc_buffers[i];

           /* DAC setup */
           iio_dac_buffers[i].buff = dac_buffers[i];
           iio_dac_buffers[i].size = sizeof(dac_buffers[i]);
           ret = iio_axi_dac_init(&dacs[i], &dac_pars[i]);
           if (ret < 0) {
               printf("DAC initialization failed for device %d\n", i);
               goto cleanup;
           }
           a = 2 * i + 1;
           iio_axi_dac_get_dev_descriptor(dacs[i], &iio_descs[a]);
           app_devices[a].name = dac_pars[i].tx_dac->name;
           app_devices[a].dev = dacs[i];
           app_devices[a].dev_descriptor = iio_descs[a];
           app_devices[a].write_buff = &iio_dac_buffers[i];
       }

       app_init_param.devices = app_devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(app_devices);
       app_init_param.uart_init_params = iio_uart_ip;

       ret = iio_app_init(&app, app_init_param);
       if (ret) {
           printf("IIO application initialization failed\n");
           goto cleanup;
       }

       ret = iio_app_run(app);
       if (ret) {
           printf("IIO application run failed\n");
       }

   cleanup:
       for (i = 0; i < IIO_DEV_COUNT; i++) {
           if (adcs[i]) {
               iio_axi_adc_remove(adcs[i]);
           }
           if (dacs[i]) {
               iio_axi_dac_remove(dacs[i]);
           }
       }
       return ret;
   }
