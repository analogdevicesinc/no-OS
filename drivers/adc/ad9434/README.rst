AD9434 no-OS driver
===================

Supported Devices
-----------------

- :adi:`AD9434`

Overview
--------

The AD9434 is a high-performance 12-bit ADC designed for applications
demanding fast conversion rates up to 500 MSPS. It integrates essential
features such as a sample-and-hold circuit and voltage reference
on-chip, ensuring comprehensive signal conversion. This ADC requires a
1.8 V analog supply and uses a differential clock for optimal
functionality. Its digital output is compatible with LVDS standards,
supporting formats including twos complement, offset binary, or Gray
code. Additionally, it features a data clock output for accurate data
timing. Available in a 56-lead LFCSP, the AD9434 is suitable for
industrial environments with temperatures ranging from -40°C to +85°C.

Applications
------------

- Wireless and wired broadband communications
- Cable reverse path
- Communications test equipment
- Radar and satellite subsystems
- Power amplifier linearization

Operation Modes
----------------

+--------------------------+-----------------+-----------------+-----------------+
| Mode Name                | Description     | Configuration   | Typical Use     |
|                          |                 | Bits            | Case            |
+--------------------------+-----------------+-----------------+-----------------+
| OUTPUT_MODE_E            | Represents the  | 0x0             | General ADC     |
| _OFFSET_BINARY           | output mode for |                 | operation using |
|                          | the ADC when    |                 | offset binary   |
|                          | configured to   |                 | output.         |
|                          | use offset      |                 |                 |
|                          | binary format.  |                 |                 |
+--------------------------+-----------------+-----------------+-----------------+
| OUTPUT_MODE              | Represents the  | 0x1             | General ADC     |
| _TWOS_COMPLEMENT         | two’s           |                 | operation using |
|                          | complement      |                 | two’s           |
|                          | output mode for |                 | complement      |
|                          | the ADC.        |                 | output.         |
+--------------------------+-----------------+-----------------+-----------------+
| OUTPUT_MODE_GRAY_CODE    | Represents the  | 0x2             | Used in         |
|                          | gray code       |                 | applications    |
|                          | output mode for |                 | requiring       |
|                          | the ADC.        |                 | gray-coded      |
|                          |                 |                 | output.         |
+--------------------------+-----------------+-----------------+-----------------+
| OUTPUT_EVEN_ODD_MODE_EN  | Enable even-odd | 0x20            | Used when       |
|                          | output mode in  |                 | alternating     |
|                          | the ADC’s       |                 | even and odd    |
|                          | output phase    |                 | outputs is      |
|                          | register.       |                 | required.       |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_OFF             | Indicates that  | 0x0             | Standard        |
|                          | the test mode   |                 | operation       |
|                          | for the ADC is  |                 | without test    |
|                          | turned off.     |                 | modes.          |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_MIDSCALE_SHORT  | Configures the  | 0x1             | Testing ADC     |
|                          | ADC for a       |                 | functionality   |
|                          | midscale short  |                 | at midscale.    |
|                          | test mode.      |                 |                 |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_POS_FULLSCALE   | Positive        | 0x2             | Verify ADC      |
|                          | full-scale test |                 | response at     |
|                          | mode.           |                 | positive full   |
|                          |                 |                 | scale.          |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_NEG_FULLSCALE   | Negative        | 0x3             | Verify ADC      |
|                          | full-scale test |                 | response at     |
|                          | mode.           |                 | negative full   |
|                          |                 |                 | scale.          |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_ALT_CHECKERBOARD| Enables an      | 0x4             | Testing ADC     |
|                          | alternating     |                 | with a          |
|                          | checkerboard    |                 | checkerboard    |
|                          | pattern in test |                 | pattern input.  |
|                          | mode.           |                 |                 |
+--------------------------+-----------------+-----------------+-----------------+
| TESTMODE_PN23_SEQ        | PN23 sequence   | 0x5             | Using PN23      |
|                          | test mode.      |                 | sequences for   |
|                          |                 |                 | ADC testing.    |
+--------------------------+-----------------+-----------------+-----------------+

Device Configuration
---------------------

Device Management
~~~~~~~~~~~~~~~~~

The ``ad9434_setup`` function is responsible for initializing the AD9434
ADC device. It begins by allocating memory for an ``ad9434_dev``
structure, which represents the device context, and initializes SPI
communication using the settings provided in the ``ad9434_init_param``
structure. The function verifies the communication by reading the chip
ID from the device and comparing it with the expected
``AD9434_CHIP_ID``. If the ID does not match, it returns an error with a
status code of -1. Following successful verification, the default output
mode is configured using ``ad9434_outputmode_set``. The memory
allocation and SPI initialization must succeed for the function to
complete, with each step’s result contributing to the final return
value. The initialized ``ad9434_dev`` structure is then assigned to the
provided device pointer, and a success message is printed upon
successful execution, returning 0 to indicate success or -1 upon
failure.

SPI Communication
~~~~~~~~~~~~~~~~~

Key to the AD9434 driver’s functionality are the SPI communication
operations, ``ad9434_spi_read`` and ``ad9434_spi_write``. These
functions handle low-level reading and writing of the ADC’s registers
over SPI, facilitating both configuration and data acquisition.
``ad9434_spi_read`` constructs a read command, while
``ad9434_spi_write`` sends data to specified registers, both reporting
operation status.

Output Mode and Test Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver enables output mode adjustments and test scenarios through
functions like ``ad9434_outputmode_set`` and ``ad9434_testmode_set``.
``ad9434_outputmode_set`` writes the required configuration to the
output register for proper data formatting, whereas
``ad9434_testmode_set`` configures the ADC into various test modes by
writing specific values to the test register. These functions allow
customization of the ADC for diverse application needs.

Data Delay and Phase Adjustment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To enhance ADC performance, the driver offers the ``ad9434_data_delay``
function to adjust output data delay, synchronizing with system
components by writing to the delay register. Macros for phase and offset
adjustments are defined, though specific functions for these were not
detailed in the content provided. These settings enable precise control
for optimized ADC functioning across various conditions.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdint.h>
   #include <stdlib.h>
   #include "no_os_spi.h"
   #include "no_os_delay.h"
   #include "no_os_alloc.h"
   #include "ad9434.h"

   struct ad9434_dev *device;
   int32_t ret;
   struct ad9434_init_param init_param = {
       .spi_init = {
           .device_id      = 0,
           .max_speed_hz   = 10000000u,
           .chip_select    = 0,
           .mode       = NO_OS_SPI_MODE_0,
           .platform_ops   = &xil_spi_ops,
           .extra      = &xil_spi_init_param,
       }
   };

   ret = ad9434_setup(&device, init_param);
   if (ret)
       goto err;
   printf("AD9434 initialization successful\n");
   goto done;
   err:
   printf("AD9434 initialization failed with error: %ld\n", ret);
   done:
   ;

IIO Support
-----------

IIO Device Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~

To efficiently manage the setup of IIO devices using the AD9434, the
initialization begins with the ``ad9434_setup`` function. This function
allocates an ``ad9434_dev`` structure to establish the device context,
involving SPI setup parameters for communication. It checks the chip ID
via ``ad9434_spi_read`` and sets the default output mode through SPI
writes. The ``iio_axi_adc_init`` function is responsible for
initializing the IIO device. It takes parameters such as the
``iio_axi_adc_desc`` and initializes them with the structures like
``iio_axi_adc_init_param``, which encompasses ADC and DMAC cores and
cache invalidation settings.

Upon any failure, error codes prompt the ``ad9434_remove`` function,
which cleans up resources by deallocating memory and releasing SPI
components. Error handling via return codes is essential for device
integrity.

IIO Application Execution
~~~~~~~~~~~~~~~~~~~~~~~~~

The ``iio_app_init`` function sets up the IIO application, incorporating
UART initialization parameters from ``no_os_uart_init_param``. Running of
the IIO app is handled by ``iio_app_run``, using settings defined in
``iio_app_init_param``, including buffer details for data access. These
functions collectively manage the complete life cycle of IIO device
initialization and configuration, ensuring seamless communication and
data processing for the AD9434.

IIO Device Initialization Example
---------------------------------

.. code-block:: C

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

    struct iio_axi_adc_desc *iio_axi_adc_desc;
    struct iio_device *dev_desc;
    struct iio_axi_adc_init_param iio_axi_adc_init_par;
    iio_axi_adc_init_par = (struct iio_axi_adc_init_param) {
        .rx_adc = ad9434_core,
        .rx_dmac = ad9434_dmac,
        .dcache_invalidate_range = (void (*)(uint32_t,
                                uint32_t))Xil_DCacheInvalidateRange
    };
    struct iio_app_desc *app;
    struct iio_app_init_param app_init_param = { 0 };

    status = iio_axi_adc_init(&iio_axi_adc_desc, &iio_axi_adc_init_par);
    if (status < 0)
        return status;

    iio_axi_adc_get_dev_descriptor(iio_axi_adc_desc, &dev_desc);
    struct iio_data_buffer read_buff = {
        .buff = (void *)ADC_DDR_BASEADDR,
        .size = 0xFFFFFFFF,
    };
    struct iio_app_device devices[] = {
        IIO_APP_DEVICE("ad9434_dev", iio_axi_adc_desc, dev_desc,
                      &read_buff, NULL, NULL),
    };

    app_init_param.devices = devices;
    app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
    app_init_param.uart_init_params = iio_uart_ip;

    status = iio_app_init(&app, app_init_param);
    if (status)
        return status;

    return iio_app_run(app);
