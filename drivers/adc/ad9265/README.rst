AD9265 no-OS Driver
===================

Supported Devices
------------------

- :adi:`AD9265`

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

- Communications
- Multimode digital receivers (3G)
     - GSM, EDGE, W-CDMA, LTE, CDMA2000, WiMAX, and TD-SCDMA
- Smart antenna systems
- General-purpose software radios
- Broadband data applications
- Ultrasound equipment

Operation Modes
----------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| Normal Mode     | The AD9265      | 0x00            | Standard        |
|                 | operates in its |                 | operation for   |
|                 | standard ADC    |                 | signal          |
|                 | conversion mode |                 | conversion.     |
|                 |                 |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Full Power-Down | ADC enters a    | 0x01            | Reducing power  |
|                 | complete        |                 | consumption     |
|                 | power-down      |                 | when the ADC is |
|                 | state to        |                 | not in use.     |
|                 | conserve        |                 |                 |
|                 | energy.         |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Standby Mode    | ADC stops       | 0x02            | Useful for      |
|                 | conversions but |                 | applications    |
|                 | maintains a     |                 | with            |
|                 | quick startup   |                 | intermittent    |
|                 | time compared   |                 | data capture.   |
|                 | to a full       |                 |                 |
|                 | shutdown.       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Test Mode       | ADC outputs     | 0x03            | Debugging and   |
|                 | predefined test |                 | verifying ADC   |
|                 | patterns for    |                 | integration and |
|                 | validating the  |                 | performance.    |
|                 | digital         |                 |                 |
|                 | interface and   |                 |                 |
|                 | system.         |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Output Mode:    | ADC outputs     | 0x00            | Used based on   |
| Offset Binary   | data in offset  |                 | system’s data   |
|                 | binary format.  |                 | protocol        |
|                 |                 |                 | requirements.   |
+-----------------+-----------------+-----------------+-----------------+
| Output Mode:    | ADC outputs     | 0x01            | Common in       |
| Two’s           | data in two’s   |                 | digital signal  |
| Complement      | complement      |                 | processing      |
|                 | format.         |                 | applications.   |
+-----------------+-----------------+-----------------+-----------------+
| Output Mode:    | Data is coded   | 0x02            | Reducing error  |
| Gray Code       | in Gray Code    |                 | during data     |
|                 | format to       |                 | transmission.   |
|                 | reduce error in |                 |                 |
|                 | conversion.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| PN9 Sequence    | Outputs a       | 0x06            | Checking signal |
| Test Mode       | pseudorandom    |                 | path and ADC    |
|                 | PN9 sequence    |                 | internal error  |
|                 | for testing the |                 | detection.      |
|                 | data path       |                 |                 |
|                 | integrity.      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Positive        | Configures the  | 0x02            | Testing ADC     |
| Full-Scale Test | ADC for         |                 | response at the |
| Mode            | positive        |                 | maximum input   |
|                 | full-scale      |                 | level.          |
|                 | response        |                 |                 |
|                 | testing.        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Alternating     | Configures the  | 0x04            | Generating      |
| Checkerboard    | ADC to output   |                 | patterns for    |
| Test Mode       | an alternating  |                 | system check    |
|                 | checkerboard    |                 | and             |
|                 | pattern for     |                 | verification.   |
|                 | testing.        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| PN23 Sequence   | Outputs a       | 0x05            | Validating ADC  |
| Test Mode       | pseudorandom    |                 | performance.    |
|                 | PN23 sequence   |                 |                 |
|                 | for testing and |                 |                 |
|                 | validation.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| Even-Odd Output | Toggles between | 0x20            | Configuring     |
| Mode Enabled    | even and odd    |                 | output phases.  |
|                 | outputs.        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+

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
binary, two’s complement, or Gray code, based on system needs.
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

.. code-block:: C

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
           .device_id = XPAR_PS7_SPI_0_DEVICE_ID,
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
           .size = 16384 * 2,  // Number of bytes to write/read
           .transfer_done = 0,  // Transfer done flag
           .cyclic = NO,  // Signal transfer mode
           .src_addr = 0,  // Address of data source
           .dest_addr = (uintptr_t)ADC_DDR_BASEADDR  // Address of data destination
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
