ADAQ7980 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADAQ7980`

Overview
---------

The ADAQ7980 is a 16-bit µModule ADC with an integrated
system-in-package design. It combines a high-accuracy SAR ADC, low-power
ADC driver, reference buffer, and power management block within a
compact 5mm x 4mm LGA package. The ADAQ7980 operates at 1 MSPS. The
ADAQ7980 supports multiple supply voltages (1.8V to 5V) and offers a
wide temperature operating range of -55°C to +125°C. Featuring
SPI-compatible interfaces, it can be configured in daisy-chain modes,
suitable for automated test equipment and medical instruments.

Applications
------------

- Automated test equipment (ATE)
- Battery powered instrumentation
- Communications
- Data acquisition
- Process control
- Medical instruments

Operation Modes
----------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| 3-wire CS Mode  | Operates with   | Set SDI high;   | Suitable for    |
| with Busy       | an              | Use 3-wire with | applications    |
| Indicator       | SPI-compatible  | CNV, SCK, and   | needing minimal |
|                 | interface using | SDO enabled.    | connections     |
|                 | a busy          |                 | while retaining |
|                 | indicator for   |                 | conversion      |
|                 | conversion      |                 | status          |
|                 | status.         |                 | visibility.     |
+-----------------+-----------------+-----------------+-----------------+
| 3-wire CS Mode  | Uses            | Set SDI high;   | Ideal for       |
| Without Busy    | SPI-compatible  | Use 3-wire with | systems where   |
| Indicator       | interface       | CNV, SCK, and   | reduced wiring  |
|                 | without a busy  | SDO.            | is critical and |
|                 | indicator       |                 | conversion      |
|                 |                 |                 | status checks   |
|                 |                 |                 | are managed in  |
|                 |                 |                 | software.       |
+-----------------+-----------------+-----------------+-----------------+
| 4-wire CS Mode  | Utilizes a      | Set SDI high;   | Used in         |
| with Busy       | 4-wire SPI      | Include an      | applications    |
| Indicator       | interface and   | extra wire for  | requiring       |
|                 | busy indicator  | busy status     | robust data     |
|                 | for improved    | signal.         | communication   |
|                 | data integrity  |                 | assurance.      |
|                 | during          |                 |                 |
|                 | conversion.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| 4-wire CS Mode  | Operates with a | Set SDI high;   | Effective where |
| Without Busy    | 4-wire SPI      | Use CNV, SCK,   | additional      |
| Indicator       | interface       | SDO, and SDI.   | communication   |
|                 | without busy    |                 | monitoring is   |
|                 | indicators,     |                 | unnecessary.    |
|                 | emphasizing     |                 |                 |
|                 | simplicity in   |                 |                 |
|                 | design.         |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Chain Mode with | Multiple ADCs   | Set SDI low     | Built for       |
| Busy Indicator  | can be          | with external   | cascading ADCs  |
|                 | daisy-chained   | busy signal;    | in complex data |
|                 | with a busy     | Use 4-wire      | acquisition     |
|                 | indicator to    | configuration.  | systems         |
|                 | manage          |                 | requiring       |
|                 | conversion      |                 | conversion      |
|                 | sequences       |                 | synchronization |
+-----------------+-----------------+-----------------+-----------------+
| Chain Mode      | Allows          | Set SDI low;    | Common in       |
| without Busy    | daisy-chaining  | Use 3-wire      | systems with    |
| Indicator       | ADCs without an | configuration.  | precise         |
|                 | internal busy   |                 | external timing |
|                 | management,     |                 | and control     |
|                 | relying on      |                 | over conversion |
|                 | external        |                 | cycles.         |
|                 | control.        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| PD_AMP Shutdown | Minimizes power | Drive PD_AMP    | Essential for   |
|                 | consumption by  | low to reduce   | power-sensitive |
|                 | shutting down   | current draw to | application     |
|                 | the amplifier   | 2.9 µA.         | like a battery  |
|                 | when idle.      |                 | operated device |
+-----------------+-----------------+-----------------+-----------------+

Device Configuration
--------------------

Device Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``adaq7980_setup`` function initializes the ADAQ7980 by configuring
SPI communication, PWM trigger generation, and GPIO settings. It
allocates memory for the device structure, configures power-down
control, and initializes SPI and PWM interfaces. This sets the device
for operation, streamlining the integration process.

Data Acquisition
~~~~~~~~~~~~~~~~

The ``ad7980_read_data`` function reads data from the ADAQ7980 via SPI
engine offload mode. Given the device structure, a data buffer, and
sample count, it sets up SPI engine offload messages for chip select
operations and efficient data transfers, ensuring high-speed acquisition
and data integrity for precise measurements.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdbool.h>
   #include <inttypes.h>
   #include <stdint.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <xil_cache.h>
   #include "parameters.h"
   #include "adaq7980.h"
   #include "no_os_pwm.h"
   #include "axi_pwm_extra.h"
   #include "no_os_gpio.h"
   #include "xilinx_gpio.h"
   #include "no_os_error.h"

   #define ADAQ7980_EVB_SAMPLE_NO 1000

   int main()
   {
       uint16_t buf[ADAQ7980_EVB_SAMPLE_NO] __attribute__((aligned));
       struct adaq7980_dev *dev;
       int32_t ret, i;

       struct spi_engine_offload_init_param spi_engine_offload_init_param = {
           .offload_config = OFFLOAD_RX_EN,
           .rx_dma_baseaddr = ADAQ7980_DMA_BASEADDR,
       };

       struct spi_engine_init_param spi_eng_init_param  = {
           .ref_clk_hz = 100000000,
           .type = SPI_ENGINE,
           .spi_engine_baseaddr = ADAQ7980_SPI_ENGINE_BASEADDR,
           .cs_delay = 0,
           .data_width = 16,
       };

       struct axi_pwm_init_param axi_pwm_init = {
           .base_addr = AXI_PWMGEN_BASEADDR,
           .ref_clock_Hz = 100000000,
           .channel = 0,
       };

       struct no_os_pwm_init_param trigger_pwm_init = {
           .period_ns = 10000,     /* 100Khz */
           .duty_cycle_ns = 10,
           .polarity = NO_OS_PWM_POLARITY_HIGH,
           .extra = &axi_pwm_init,
       };

       struct xil_gpio_init_param gpio_extra_param = {
           .device_id = GPIO_DEVICE_ID,
           .type = GPIO_PS,
       };

       struct no_os_gpio_init_param adaq7980_pd_ldo = {
           .number = GPIO_0,
           .platform_ops = &xil_gpio_ops,
           .extra = &gpio_extra_param
       };
       struct no_os_gpio_init_param adaq7980_ref_pd = {
           .number = GPIO_REF_PUB,
           .platform_ops = &xil_gpio_ops,
           .extra = &gpio_extra_param
       };
       struct no_os_gpio_init_param adaq7980_rbuf_pd = {
           .number = GPIO_RBUF_PUB,
           .platform_ops = &xil_gpio_ops,
           .extra = &gpio_extra_param
       };

       struct no_os_spi_init_param spi_init = {
           .chip_select = SPI_ADAQ7980_CS,
           .max_speed_hz = 10000000,
           .mode = NO_OS_SPI_MODE_2,
           .platform_ops = &spi_eng_platform_ops,
           .extra = (void*)&spi_eng_init_param,
       };

       struct adaq7980_init_param adaq7980_init_param = {
           .spi_init = &spi_init,
           .offload_init_param = &spi_engine_offload_init_param,
           .trigger_pwm_init = &trigger_pwm_init,
           .gpio_pd_ldo = &adaq7980_pd_ldo,
       };

       Xil_DCacheDisable();
       Xil_ICacheEnable();

       ret = adaq7980_setup(&dev, &adaq7980_init_param);
       if (ret < 0)
           return -1;

       while (1) {
           ret = ad7980_read_data(dev, buf, ADAQ7980_EVB_SAMPLE_NO);
           if (ret < 0)
               return -1;

           for (i = 0; i < ADAQ7980_EVB_SAMPLE_NO; i++)
               printf("ADC sample %"PRIu32" %"PRIu16" \n", i, buf[i]);
       }

       printf("Success\n\r");

       Xil_ICacheDisable();

       return 0;
   }