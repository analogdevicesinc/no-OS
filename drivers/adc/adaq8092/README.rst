ADAQ8092 no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`ADAQ8092`

Overview
--------

The ADAQ8092 is a dual-channel, 14-bit ADC designed for high-speed data
acquisition at 105 MSPS, integrating signal conditioning, ADC drivers,
and reference circuitry for compact applications in communications, GPS
receivers, and medical imaging. It supports CMOS and LVDS output modes,
along with power-down and test modes for power management and
performance evaluation. The device features clock duty cycle
stabilization, adjustable LVDS output current, and data randomization
controlled via a flexible register map interface, facilitating system
integration and configuration. Operating between -40°C and +125°C, the
ADAQ8092 ensures reliable performance in diverse conditions, including
industrial and outdoor environments. Its built-in power supply
decoupling capacitor minimizes voltage fluctuations, enhancing power
stability and reducing noise in high-speed systems. These capabilities
make the ADAQ8092 ideal for robust and adaptable data acquisition
solutions.

Applications
------------

- Communications
- Cellular base stations

Operation Modes
---------------

===========================  ==================================================  =====================================
Mode Name                    Description                                         Typical Use Case
===========================  ==================================================  =====================================
ADAQ8092_NORMAL_OP           Normal operating mode fully functional              General usage when full operation 
                             without power-down.                                 is required
                                                              
ADAQ8092_CH1_NORMAL_CH2_NAP  Channel 1 operates normally while channel 2         Use when power saving is needed 
                             is in nap mode.                                     and only one channel is active

ADAQ8092_CH1_CH2_NAP         Both channels are in nap mode to save power.        Use when temporary pause in operation
                                                                                 is permissible

ADAQ8092_SLEEP               Complete device power-down to conserve power.       Use for maximum power saving when the 
                                                                                 device is idle

ADAQ8092_FULL_RATE_CMOS      Full rate CMOS output mode for digital data.        General digital data transmission 
                                                                                 at full speed

ADAQ8092_DOUBLE_RATE_LVDS    Double rate LVDS output mode for enhanced           High-speed data transmission scenarios
                             performance.                                       

ADAQ8092_DOUBLE_RATE_CMOS    Double rate CMOS output mode for increased          Applications requiring high data
                             data rate.                                          throughput

ADAQ8092_TEST_OFF            Test mode disabled with no test pattern applied.    Standard operation without testing

ADAQ8092_TEST_ONES           Outputs a stream of ones for testing purposes.      Use in system verification and testing

ADAQ8092_TEST_ZEROS          Outputs a stream of zeros for testing purposes.     Use in system verification and testing

ADAQ8092_TEST_CHECKERBOARD   Outputs a checkerboard pattern for testing.         System verification and pattern testing

ADAQ8092_TEST_ALTERNATING    Outputs alternating values for testing purposes.    Testing alternating signal integrity
===========================  ==================================================  ===========================================

Device Configuration
--------------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``adaq8092_init`` function sets up SPI and GPIO interfaces, handles
power-up sequences, and configures initial settings based on provided
parameters. Additionally, ``adaq8092_update_bits``, ``adaq8092_read``, and
``adaq8092_write`` are essential for register communication, facilitating
read and write operations that are foundational for setting up the
device for operation.

Power Management and Clock Settings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section includes functions that manage power modes and clock
settings to ensure optimal device performance. ``adaq8092_set_pd_mode``
and ``adaq8092_get_pd_mode`` manage power states, enhancing energy
efficiency. Clock settings are controlled with functions like
``adaq8092_set_clk_pol_mode``, ``adaq8092_get_clk_pol_mode``,
``adaq8092_set_clk_phase_mode``, and ``adaq8092_get_clk_phase_mode``, which
adjust clock signal polarity and phase for synchronization with system
components.

LVDS and Digital Output Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The file supports LVDS output settings with ``adaq8092_set_lvds_cur_mode``
and ``adaq8092_get_lvds_cur_mode`` to manage current drive strength.
Digital output controls, such as ``adaq8092_set_dout_en``,
``adaq8092_get_dout_en``, ``adaq8092_set_dout_mode``, and
``adaq8092_get_dout_mode``, control output enabling and modes, offering
flexibility for various applications.

Data Formatting and Test Modes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For data management and testing, the file offers functions like
``adaq8092_set_data_rand_en``, ``adaq8092_get_data_rand_en``,
``adaq8092_set_twos_comp``, and ``adaq8092_get_twos_comp`` to enable data
randomization and set representation modes like Two’s Complement.
``adaq8092_set_test_mode`` and ``adaq8092_get_test_mode`` are used for
outputting test patterns to verify digital operation, aiding in
debugging and development processes.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdint.h>
   #include <stdio.h>
   #include "adaq8092.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"
   #include "no_os_delay.h"
   #include "no_os_alloc.h"
   #include "no_os_util.h"
   #include "xilinx_spi.h"
   #include "xilinx_gpio.h"
   #include "parameters.h"

   struct adaq8092_dev *dev;
   int32_t ret;

   struct adaq8092_init_param init_param = {
       .spi_init = {
           .device_id      = 0,
           .max_speed_hz   = 2000000,
           .chip_select    = 0,
           .mode           = NO_OS_SPI_MODE_0,
           .platform_ops   = &xil_spi_ops,
           .extra          = &xil_spi_init_param
       },
       .gpio_adc_pd1_param = {
           .number         = ADC_PD1_GPIO,
           .platform_ops   = &xil_gpio_ops,
           .extra          = &xil_gpio_init_param
       },
       .gpio_adc_pd2_param = {
           .number         = ADC_PD2_GPIO,
           .platform_ops   = &xil_gpio_ops,
           .extra          = &xil_gpio_init_param
       },
       .gpio_en_1p8_param = {
           .number         = ENABLE_1P8_GPIO,
           .platform_ops   = &xil_gpio_ops,
           .extra          = &xil_gpio_init_param
       },
       .gpio_par_ser_param = {
           .number         = PAR_SER_GPIO,
           .platform_ops   = &xil_gpio_ops,
           .extra          = &xil_gpio_init_param
       },
       .pd_mode            = ADAQ8092_NORMAL_OP,
       .clk_pol_mode       = ADAQ8092_CLK_POL_NORMAL,
       .clk_phase_mode     = ADAQ8092_NO_DELAY,
       .clk_dc_mode        = ADAQ8092_CLK_DC_STABILIZER_OFF,
       .lvds_cur_mode      = ADAQ8092_4MA,
       .lvds_term_mode     = ADAQ8092_TERM_ON,
       .dout_en        = ADAQ8092_DOUT_ON,
       .dout_mode      = ADAQ8092_FULL_RATE_CMOS,
       .test_mode      = ADAQ8092_TEST_OFF,
       .alt_bit_pol_en     = ADAQ8092_ALT_BIT_POL_OFF,
       .data_rand_en       = ADAQ8092_DATA_RAND_OFF,
       .twos_comp      = ADAQ8092_OFFSET_BINARY
   };

   printf("Starting ADAQ8092 initialization\n");

   ret = adaq8092_init(&dev, &init_param);
   if (ret != 0) {
       printf("ADAQ8092 initialization failed\n");
       goto error;
   }

   printf("ADAQ8092 initialization successful\n");
   goto end;

   error:
       printf("Error encountered during ADAQ8092 initialization\n");

   end:
       ;

IIO Support
-----------

To configure the ADAQ8092 device for IIO applications, begin by
initializing the device with the ``adaq8092_init`` function, ensuring
SPI and GPIO parameters are correctly defined. The ``axi_adc_init`` and
``axi_dmac_init`` functions configure the device for data acquisition
and transfer readiness. Integrate IIO-specific headers and libraries
like ``iio_app.h`` and ``iio_axi_adc.h``, and utilize conditional
compilation with the ``IIOD`` flag for IIO context. Use the
``iio_app_init`` function to establish IIO-specific initialization
parameters, such as defining the ``iio_axi_adc_init_param`` to link the
ADC core and DMAC for real-time data streaming. The ``iio_app_run``
function launches the IIO application, managing data transmission via
the IIO interface, while reading the data buffer through
``iio_axi_adc_get_dev_descriptor`` and required IIO devices setup. This
setup enables effective data acquisition and streaming tailored for
high-performance applications such as communications and medical
imaging.

IIO Device Initialization Example
---------------------------------

.. code-block:: C

   #include "xil_cache.h" 
   #include "xparameters.h" 
   #include "axi_adc_core.h" 
   #include "axi_dmac.h" 
   #include "adaq8092.h" 
   #include "no_os_spi.h" 
   #include "no_os_gpio.h" 
   #include "xilinx_spi.h" 
   #include "parameters.h" 
   #include "no_os_error.h" 
   #include "xilinx_gpio.h" 

   #include "no_os_print_log.h"  

   #ifdef IIO_SUPPORT 
   #include "iio_app.h" 
   #include "iio_axi_adc.h" 
   #include "xilinx_uart.h" 
   #endif 

   #define ADAQ8092_SAMPLES_PER_CH 1000 
   #define ADAQ8092_NUM_CH 2 
    
   int main(void) { 
       int ret; 
       uint16_t adc_buffer[ADAQ8092_SAMPLES_PER_CH * ADAQ8092_NUM_CH] __attribute__((aligned)); 
    

   struct xil_spi_init_param xil_spi_init = { 
           .flags = 0, 
           .type = SPI_PS 
       }; 
    
   struct no_os_spi_init_param adaq8092_spi_param = { 
           .device_id = SPI_DEVICE_ID, 
           .max_speed_hz =  1000000u, 
           .chip_select = 0, 
           .mode = NO_OS_SPI_MODE_0, 
           .platform_ops = &xil_spi_ops, 
           .extra = &xil_spi_init 
       }; 
    
   struct xil_gpio_init_param xil_gpio_init = { 
           .device_id = GPIO_DEVICE_ID, 
           .type = GPIO_PS 
       }; 
    
   struct no_os_gpio_init_param gpio_par_ser_init_param = { 
           .number = GPIO_PAR_SER_NR, 
           .platform_ops = &xil_gpio_ops, 
           .extra = &xil_gpio_init 
       }; 
    
   struct no_os_gpio_init_param gpio_adc_pd1_param = { 
           .number = GPIO_PD1_NR, 
           .platform_ops = &xil_gpio_ops, 
           .extra = &xil_gpio_init 
       }; 
    
   struct no_os_gpio_init_param gpio_adc_pd2_param = { 
           .number = GPIO_PD2_NR, 
           .platform_ops = &xil_gpio_ops, 
           .extra = &xil_gpio_init 
       }; 
    
   struct no_os_gpio_init_param gpio_en_1v8_param = { 
           .number = GPIO_1V8_NR, 
           .platform_ops = &xil_gpio_ops, 
           .extra = &xil_gpio_init 
       }; 
    
   struct axi_adc_init adaq8092_core_param = { 
           .name = "adaq8092_core", 
           .num_channels = 2, 
           .base = RX_CORE_BASEADDR 
       }; 
       struct axi_adc *adaq8092_core; 
    
   struct axi_dmac_init adaq8092_dmac_param = { 
           .name = "adaq8092_dmac", 
           .base = RX_DMA_BASEADDR, 
           .irq_option = IRQ_DISABLED 
       }; 
       struct axi_dmac *adaq8092_dmac; 
    
   struct adaq8092_init_param adaq8092_init_param = { 
           .spi_init = &adaq8092_spi_param, 
           .gpio_adc_pd1_param = &gpio_adc_pd1_param, 
           .gpio_adc_pd2_param = &gpio_adc_pd2_param, 
           .gpio_en_1p8_param = &gpio_en_1v8_param, 
           .gpio_par_ser_param = &gpio_par_ser_init_param, 
           .pd_mode = ADAQ8092_NORMAL_OP, 
           .clk_pol_mode = ADAQ8092_CLK_POL_INVERTED, 
           .clk_phase_mode = ADAQ8092_NO_DELAY, 
           .clk_dc_mode = ADAQ8092_CLK_DC_STABILIZER_OFF, 
           .lvds_cur_mode = ADAQ8092_3M5A, 
           .lvds_term_mode = ADAQ8092_TERM_OFF, 
           .dout_en = ADAQ8092_DOUT_ON, 
           .dout_mode = ADAQ8092_DOUBLE_RATE_LVDS, 
           .test_mode = ADAQ8092_TEST_CHECKERBOARD, 
           .alt_bit_pol_en = ADAQ8092_ALT_BIT_POL_OFF, 
           .data_rand_en = ADAQ8092_DATA_RAND_OFF, 
           .twos_comp = ADAQ8092_TWOS_COMPLEMENT 
       }; 
       struct adaq8092_dev *adaq8092_device; 
    
   ret = adaq8092_init(&adaq8092_device, adaq8092_init_param); 
       if (ret) { 
           pr_err("ADAQ8092 device initialization failed!"); 
           return ret; 
       } 
    
   ret = axi_adc_init(&adaq8092_core,  &adaq8092_core_param); 
       if (ret) { 
           pr_err("axi_adc_init() error: %s\n", adaq8092_core->name); 
           return ret; 
       } 
    
   ret = axi_dmac_init(&adaq8092_dmac, &adaq8092_dmac_param); 
       if (ret) { 
           pr_err("axi_dmac_init() error: %s\n", adaq8092_dmac->name); 
           return ret; 
       } 
    
   pr_info("Start Capture with Test pattern - Checkerboard \n"); 
    
   struct axi_dma_transfer read_transfer = { 
           .size = sizeof(adc_buffer), 
           .transfer_done = 0, 
           .cyclic = NO, 
           .src_addr = 0, 
           .dest_addr = (uintptr_t)adc_buffer 
       }; 
    
   ret = axi_dmac_transfer_start(adaq8092_dmac, &read_transfer); 
       if (ret) { 
           pr_err("axi_dmac_transfer_start() failed!\n"); 
           return ret; 
       } 
    
   ret = axi_dmac_transfer_wait_completion(adaq8092_dmac, 500); 
       if (ret) 
           return ret; 
    
   Xil_DCacheInvalidateRange((uintptr_t)adc_buffer, sizeof(adc_buffer)); 
    
   for (int i = 0; i < ADAQ8092_SAMPLES_PER_CH; i += 2) 
           pr_info("CH1: %d CH2: %d \n", adc_buffer[i], adc_buffer[i + 1]); 
    
   ret = adaq8092_set_test_mode(adaq8092_device, ADAQ8092_TEST_OFF); 
       if (ret) 
           return ret; 
    
   pr_info("\n Capture done.\n"); 

   struct iio_app_desc *app; 
       struct iio_app_init_param app_init_param = { 0 }; 
       struct iio_axi_adc_desc *iio_axi_adc_desc; 
       struct iio_device *dev_desc; 
       struct iio_axi_adc_init_param iio_axi_adc_init_par; 
       iio_axi_adc_init_par = (struct iio_axi_adc_init_param) { 
           .rx_adc = adaq8092_core, 
           .rx_dmac = adaq8092_dmac, 
           .dcache_invalidate_range = (void (*)(uint32_t, uint32_t))Xil_DCacheInvalidateRange 
       }; 
    
   struct iio_data_buffer read_buff = { 
           .buff = (void *)adc_buffer, 
           .size = 0xFFFFFFFF, 
       }; 
    
   ret = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par); 
       if (ret < 0) 
           return ret; 
       iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc); 
    
   struct iio_app_device devices[] = { 
           IIO_APP_DEVICE("adaq8092_dev", iio_axi_adc_desc, dev_desc, &read_buff, NULL, NULL), 
       }; 
    
   app_init_param.devices = devices; 
       app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices); 
       app_init_param.uart_init_params = iio_uart_ip; 

   ret = iio_app_init(&app, app_init_param); 
       if (ret) 
           return ret; 
    
   iio_app_run(app);  
    
   return 0; 
   }
