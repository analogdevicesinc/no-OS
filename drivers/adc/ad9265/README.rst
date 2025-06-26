AD9265 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
------------------

* `AD9265 <https://www.analog.com/AD9265>`_

Overview
---------

The AD9265 is a 16-bit, 125MSPS analog-to-digital converter (ADC) ideal
for communications applications requiring high performance at low cost.
It employs a multistage differential pipelined architecture with
integrated output error correction, ensuring accurate 16-bit resolution
and no missing codes across its operating temperature range of -40°C to
+85°C. The ADC includes a broad bandwidth differential sample-and-hold
input and supports user-selectable input ranges. It features a
differential clock input for conversion control and offers output
formats in parallel 1.8V CMOS or LVDS. The device includes a 3-wire
SPI-compatible interface for configuration and control, and is housed in
a Pb-free, 48-lead LFCSP with options for flexible power-down to enhance
power efficiency.

Applications
-------------

* Communications
* Multimode digital receivers (3G)

  * GSM, EDGE, W-CDMA, LTE, CDMA2000, WiMAX, and TD-SCDMA

* Smart antenna systems
* General-purpose software radios
* Broadband data applications
* Ultrasound equipment

Operation Modes
----------------

The AD9265 supports the following operation modes:

* **Normal Mode**: Standard ADC conversion mode for signal acquisition.
* **Full Power-Down**: Enters a complete power-down state to conserve
  energy when the ADC is not in use.
* **Standby Mode**: Stops conversions but maintains quick startup time
  for intermittent data capture applications.
* **Test Mode**: Outputs predefined test patterns (PN9, PN23,
  checkerboard, one-zero toggle) for interface validation and debugging.

Device Configuration
---------------------

Data Conversion Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9265_spi_read`` and ``ad9265_spi_write`` functions facilitate
SPI communication with the AD9265 device. ``ad9265_spi_read`` reads
register values and stores the response, while ``ad9265_spi_write``
sends commands and data to write to specific registers. These functions
enable configuration and real-time data operations for high-speed data
acquisition.

Configuration and Setup Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9265_setup`` function initializes the AD9265 device, setting up
SPI communication, output mode, and calibration. It ensures correct
device operation and alignment with system requirements.
``ad9265_outputmode_set`` configures the output data format to offset
binary, two's complement, or Gray code, based on system needs.
``ad9265_calibrate`` adjusts delay settings for accurate data
conversion, essential for high-speed data acquisition systems.

Power Management Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~

Power management is implemented by configuring specific registers in the
AD9265 through SPI commands. Though explicit power management functions
like ``power_down`` are not separately defined, adjusting output mode
and communication settings aids in efficient power management, crucial
for energy conservation in embedded systems.

Debugging and Testing Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``ad9265_testmode_set`` configures the AD9265 to output test patterns
for interface validation and integration checks. The function supports
predefined test modes like the one-zero toggle, allowing developers to
debug and verify system performance before production deployment.

Device Removal Function
~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9265_remove`` function is responsible for preparing the AD9265
device for safe removal by deallocating resources. It employs
``no_os_spi_remove`` to disconnect the SPI interface and ``no_os_free``
to free the allocated memory of the device structure, ensuring resource
release and maintaining system stability. The function returns an
integer status of the SPI removal operation, which indicates whether the
removal was successful. This comprehensive cleanup routine ensures that
no resources are left allocated after the device is removed, preventing
potential memory leaks or system issues.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "no_os_error.h"
   #include "no_os_print_log.h"
   #include "xil_cache.h"
   #include "xparameters.h"
   #include "axi_adc_core.h"
   #include "axi_dmac.h"
   #include "ad9265.h"
   #include "no_os_spi.h"
   #include "xilinx_spi.h"
   #include "parameters.h"

   int main(void)
   {
       int32_t status;

       /* Initialize SPI structures */
       struct xil_spi_init_param xil_spi_param = {
           .type = SPI_PS,
       };

       struct no_os_spi_init_param ad9265_spi_param = {
           .device_id = SPI_DEVICE_ID,
           .max_speed_hz = 10000000u,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .extra = &xil_spi_param,
           .platform_ops = &xil_spi_ops,
       };

       /* ADC Core */
       struct axi_adc_init ad9265_core_param = {
           .name = "ad9265_core",
           .num_channels = 1,
           .base = RX_CORE_BASEADDR
       };
       struct axi_adc *ad9265_core;

       /* AXI DMAC */
       struct axi_dmac_init ad9265_dmac_param = {
           .name = "ad9265_dmac",
           .base = RX_DMA_BASEADDR,
           .irq_option = IRQ_DISABLED
       };
       struct axi_dmac *ad9265_dmac;

       /* AD9265 Initialization */
       struct ad9265_init_param ad9265_param = {
           .spi_init = ad9265_spi_param
       };
       struct ad9265_dev *ad9265_device;

       #ifdef XILINX_PLATFORM
       /* Enable the instruction cache. */
       Xil_ICacheEnable();
       /* Enable the data cache. */
       Xil_DCacheEnable();
       #endif /* XILINX_PLATFORM */

       status = axi_adc_init(&ad9265_core, &ad9265_core_param);
       if (status != 0) {
           pr_err("axi_adc_init() error: %s\n", ad9265_core->name);
           return -1;
       }

       status = axi_dmac_init(&ad9265_dmac, &ad9265_dmac_param);
       if (status != 0) {
           pr_err("axi_dmac_init() error: %s\n", ad9265_dmac->name);
           return -1;
       }

       status = ad9265_setup(&ad9265_device, ad9265_param, *ad9265_core);
       if (status != 0) {
           pr_err("ad9265_setup() failed!\n");
           return -1;
       }

       status = ad9265_testmode_set(ad9265_device, TESTMODE_ONE_ZERO_TOGGLE);
       if (status != 0) {
           pr_err("ad9265_testmode_set() TESTMODE_ONE_ZERO_TOGGLE failed!\n");
           return -1;
       }

       struct axi_dma_transfer read_transfer = {
           .size = 16384 * 2,
           .transfer_done = 0,
           .cyclic = NO,
           .src_addr = 0,
           .dest_addr = (uintptr_t)ADC_DDR_BASEADDR
       };

       status = axi_dmac_transfer_start(ad9265_dmac, &read_transfer);
       if (status != 0) {
           pr_err("axi_dmac_transfer_start() failed!\n");
           return -1;
       }

       /* Wait until transfer finishes */
       status = axi_dmac_transfer_wait_completion(ad9265_dmac, 500);
       if (status < 0)
           return status;

       Xil_DCacheInvalidateRange((uintptr_t)ADC_DDR_BASEADDR, 16384 * 2);

       pr_info("Capture done.\n");

       status = ad9265_testmode_set(ad9265_device, TESTMODE_OFF);
       if (status != 0) {
           pr_err("ad9265_testmode_set() TESTMODE_OFF failed!\n");
           return -1;
       }

       pr_info("Done\n");

       #ifdef XILINX_PLATFORM
       /* Disable the instruction cache. */
       Xil_ICacheDisable();
       /* Disable the data cache. */
       Xil_DCacheDisable();
       #endif /* XILINX_PLATFORM */

       return 0;
   }
