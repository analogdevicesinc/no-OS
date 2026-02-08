AD9553 no-OS driver
====================

Supported Devices
-----------------

- :adi:`AD9553`

Overview
---------

The AD9553 is a PLL-based clock translator designed for passive optical
networks (PON) and base stations. It employs an integer-N PLL for
specific frequency translation, allowing up to two single-ended or one
differential input via the REFA and REFB inputs. The device supports
holdover applications with a 25 MHz crystal attached to the XTAL input.
It is pin programmable, offering frequency translations from 15 input
frequencies to 52 output frequency pairs (OUT1 and OUT2). The AD9553
includes a 3-wire SPI for custom frequency translations and supports
LVPECL, LVDS, or CMOS logic levels. It operates within an industrial
temperature range of −40°C to +85°C.

Applications
-------------

- Cost effective replacement of high frequency VCXO, OCXO, and SAW
  resonators
- Extremely flexible frequency translation for SONET/SDH, Ethernet,
  Fiber Channel, DRFI/DOCSIS, and PON/EPON/GPON
- Wireless infrastructures
- Test and measurement (including handheld devices)

Operation Modes
----------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| LSB First Mode  | Configures SPI  | Register        | Alternate data  |
|                 | data order to   | 0x00[6]: LSB    | formats for SPI |
|                 | transmit Least  | first enable    | communication   |
|                 | Significant Bit |                 |                 |
|                 | first.          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| MSB First Mode  | Default         | Register        | Standard        |
|                 | operation mode  | 0x00[6]: LSB    | operation for   |
|                 | sending Most    | first is 0      | most serial     |
|                 | Significant Bit |                 | communications  |
|                 | first.          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Enable SPI      | Enables SPI     | Register        | Dynamic         |
| Control         | control for     | 0x0B[7]         | alteration of   |
|                 | output mode     |                 | mode control    |
|                 |                 |                 | using SPI       |
+-----------------+-----------------+-----------------+-----------------+
| SPI Mode        | Allows SPI      | Register        | Remote device   |
|                 | configuration   | 0x0E[2]         | configuration   |
|                 | and control of  |                 |                 |
|                 | multiple        |                 |                 |
|                 | functions       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Normal          | Default PLL     | Register        | Standard clock  |
| Operation       | operation       | 0x0C[2:0] = 000 | translation     |
|                 | without         |                 |                 |
|                 | additional      |                 |                 |
|                 | clock divisions |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Feedback Clock  | Divides the     | Register        | Device          |
| Div             | feedback clock  | 0x0C[2:0] = 001 | debugging and   |
|                 | for testing     |                 | validation      |
+-----------------+-----------------+-----------------+-----------------+
| PFD Reference   | Divides the     | Register        | Internal        |
| Div             | phase frequency | 0x0C[2:0] = 010 | testing and     |
|                 | detector (PFD)  |                 | clock           |
|                 | reference input |                 | adjustments     |
|                 | clock           |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Antibacklash    | Adjusts the PFD | Register        | Minimizing      |
| Control         | antibacklash    | 0x0D[7:6]       | jitter and      |
|                 | period to alter |                 | phase noise     |
|                 | PLL dynamics    |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| VCO Calibration | Initiates and   | Register        | Optimal VCO     |
|                 | controls        | 0x0E[7],        | performance     |
|                 | vol             | 0x0E[2]         | during          |
|                 | tage-controlled |                 | frequency       |
|                 | oscillator      |                 | changes         |
|                 | (VCO)           |                 |                 |
|                 | calibration     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| ALC Threshold   | Configures the  | Register        | Custom          |
|                 | automatic level | 0x0E[5:3]       | frequency       |
|                 | control (ALC)   |                 | translation and |
|                 | threshold for   |                 | noise           |
|                 | the VCO         |                 | management      |
+-----------------+-----------------+-----------------+-----------------+
| OUT1 Driver     | Configures the  | Register        | Adaptation for  |
| Control         | output driver   | 0x32[5:3],      | different load  |
|                 | mode and        | 0x32[7]         | conditions      |
|                 | strength for    |                 |                 |
|                 | output 1        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| OUT2 Driver     | Configures the  | Register        | Dual output     |
| Control         | output driver   | 0x34[5:3],      | configurations  |
|                 | mode and        | 0x34[7],        | with distinct   |
|                 | strength for    | 0x34[2]         | needs           |
|                 | output 2,       |                 |                 |
|                 | includes        |                 |                 |
|                 | polarity        |                 |                 |
|                 | settings        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
---------------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``ad9553_setup`` configures the AD9553 chip for operation by initiating
SPI communication, resetting the device, and configuring charge pumps
and dividers. It prepares necessary settings such as the PLL charge pump
control, lock detector activation, and division values through SPI
transactions.

SPI Communication
~~~~~~~~~~~~~~~~~

``ad9553_reg_read`` and ``ad9553_reg_write`` manage SPI protocol
transactions for reading and writing to the AD9553 registers.
``ad9553_reg_read`` retrieves data from specific registers, while
``ad9553_reg_write`` configures registers by sending data. These
functions enable flexible communication and control over the device’s
operations.

Resource Management
~~~~~~~~~~~~~~~~~~~

``ad9553_remove`` handles resource management by deallocating resources
and removing SPI descriptors used by the AD9553 driver, ensuring
efficient use of system resources and preventing memory leaks.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include "ad9553.h"
   #include "no_os_spi.h"

   int ad9553_initialize()
   {
       struct no_os_spi_init_param ad9553_spi_param = {
           .device_id = 0,
           .max_speed_hz = 2000000u,
           .chip_select = 2,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       };

       struct ad9553_init_param ad9553_param;
       ad9553_param.spi_init = ad9553_spi_param;

       struct ad9553_dev *ad9553_device;
       
       // Initialize the ad9553 device
       if (ad9553_setup(&ad9553_device, &ad9553_param) != 0) {
           printf("The ad9553 chip could not be setup!\n");
           return -1;
       }
       return 0;
   }

IIO Support
------------

To initialize the IIO example code for the AD9553 device, begin by
setting up SPI communication via the ``ad9553_spi_param`` structure,
containing parameters like ``device_id``, ``max_speed_hz``,
``chip_select``, and ``mode``. Use the ``ad9553_setup`` function to
initialize the AD9553 device; this prepares the SPI interface and sets
up device-specific configurations, including charge pumps and divisors.
Ensure the function writes to necessary registers, resetting the device
and correctly setting up the frequency translation capabilities through
SPI commands. Handle resource deallocation with ``ad9553_remove`` once
operations are complete to manage system resources efficiently and
mitigate memory leaks. Include appropriate headers and define the SPI
parameters for successful communication and setup of the AD9553 within
the IIO framework.

IIO Example Code
----------------

.. code-block:: C

   int main(void)
   {
       int32_t status;

       struct xil_spi_init_param xil_spi_param = {
           .type = SPI_PS,
           .flags = 0U
       };

       /* Initialize SPI structures */
       struct no_os_spi_init_param ad9508_spi_param = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 2000000u,
           .chip_select = 1,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       };

       struct no_os_spi_init_param ad9553_spi_param = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 2000000u,
           .chip_select = 2,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       };
       struct no_os_spi_init_param ad9656_spi_param = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 2000000u,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .platform_ops = &xil_spi_ops,
           .extra = &xil_spi_param
       };

       /* this pattern is outputed by the ad9656 chip after the JESD204 test is finished */
       struct ad9656_user_input_test_pattern user_input_test_pattern = {
           .user_test_pattern1 = 0xA1B2,
           .user_test_pattern2 = 0xC3D4
       };

       struct ad9508_init_param    ad9508_param;
       struct ad9553_init_param    ad9553_param;
       struct ad9656_init_param    ad9656_param;

       ad9508_param.spi_init = ad9508_spi_param;
       ad9553_param.spi_init = ad9553_spi_param;
       ad9656_param.spi_init = ad9656_spi_param;

       struct ad9508_dev *ad9508_device;
       struct ad9553_dev *ad9553_device;
       struct ad9656_dev *ad9656_device;

   #ifdef IIO_SUPPORT
       struct xil_uart_init_param platform_uart_init_par = {
   #ifdef XPAR_XUARTLITE_NUM_INSTANCES
           .type = UART_PL,
   #else
           .type = UART_PS,
           .irq_id = UART_IRQ_ID
   #endif
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
       struct iio_app_init_param app_init_param = { 0 };
       struct iio_axi_adc_desc *iio_axi_adc_desc;
       struct iio_device *dev_desc;
       struct iio_axi_adc_init_param iio_axi_adc_init_par;
       iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
           .rx_adc = ad9656_core,
           .rx_dmac = ad9656_dmac,
           .dcache_invalidate_range = (void (*)(uint32_t,
                                uint32_t))Xil_DCacheInvalidateRange
       };

       status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
       if (status < 0)
           return status;

       iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);

       struct iio_data_buffer read_buff = {
           .buff = (void *)ADC_DDR_BASEADDR,
           .size = 0xFFFFFFFF,
       };

       struct iio_app_device devices[] = {
           IIO_APP_DEVICE("ad9656_dev", iio_axi_adc_desc, dev_desc,
                      &read_buff, NULL, NULL),
       };

       app_init_param.devices = devices;
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
       app_init_param.uart_init_params = iio_uart_ip;

       status = iio_app_init(&app, app_init_param);
       if (status)
           return status;

       return iio_app_run(app);
   #endif

       /* Memory deallocation for devices and spi */
       ad9508_remove(ad9508_device);
       ad9553_remove(ad9553_device);
       ad9656_remove(ad9656_device);

       /* Disable the instruction cache. */
       Xil_ICacheDisable();
       /* Disable the data cache. */
       Xil_DCacheDisable();

       printf("Bye!\n");

       return (0);
   }
