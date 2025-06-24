AD9208 no-OS Driver
===================

Supported Devices
-----------------

:adi:`AD9208`

Overview
---------

The AD9208 is a dual 14-bit, 3 GSPS analog-to-digital converter (ADC)
designed by Analog Devices, optimized for communications applications
requiring high sampling rates, wide input bandwidth, and low power
consumption. It offers direct sampling of analog signals with input
frequencies up to 5 GHz and -3 dB bandwidth reaching 9 GHz. The device
integrates dual ADC cores with a multistage, differential pipelined
architecture and error correction logic, supporting wide input
bandwidths and user-selectable ranges. Additionally, it features four
digital downconverters (DDCs) with cascaded processing stages such as
frequency translation and decimation filters, enhanced by automatic gain
control and programmable threshold detection. Its JESD204B high-speed
serialized output is suitable for diverse communication standards,
making it ideal for multiband digital receivers and high-speed precision
data conversion applications like electronic test systems, phased array
radars, and DOCSIS 3.0 CMTS paths.

Applications
--------------

- Diversity multiband and multimode digital receivers
- 3G/4G, TD-SCDMA, W-CDMA, GSM, LTE, LTE-A
- Electronic test and measurement systems
- Phased array radar and electronic warfare
- DOCSIS 3.0 CMTS upstream receive paths
- HFC digital reverse path receivers

Operation Modes
---------------

+-----------------+-----------------+-----------------+-----------------+
| Mode Name       | Description     | Configuration   | Typical Use     |
|                 |                 | Bits            | Case            |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_FULL     | Configures ADC  | None            | High-speed data |
| _BANDWIDTH_MODE | for full        |                 | acquisition     |
|                 | bandwidth       |                 | without         |
|                 | operation       |                 | decimation.     |
|                 | without         |                 |                 |
|                 | decimation.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_1        | Single DDC      | None            | Suitable for    |
| _DDC_MODE       | configuration   |                 | applications    |
|                 | for             |                 | using a single  |
|                 | applications    |                 | channel.        |
|                 | requiring       |                 |                 |
|                 | decimation.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_2        | Sets the device | None            | Dual-channel    |
| _DDC_MODE       | for dual DDC    |                 | decimated data  |
|                 | output for      |                 | applications.   |
|                 | applications    |                 |                 |
|                 | with two        |                 |                 |
|                 | decimated       |                 |                 |
|                 | outputs.        |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_4        | Configures four | None            | Multi-channel   |
| _DDC_MODE       | DDCs to work    |                 | systems         |
|                 | simultaneously, |                 | requiring four  |
|                 | providing       |                 | decimated       |
|                 | flexibility in  |                 | outputs.        |
|                 | channel usage.  |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9             | Indicates no    | 0               | Scenarios       |
| 208_SYSREF_NONE | SYSREF support, |                 | without need    |
|                 | used when       |                 | for external    |
|                 | synchronization |                 | s               |
|                 | is not          |                 | ynchronization. |
|                 | required.       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208          | Enables         | 1               | Single event    |
| _SYSREF_ONESHOT | one-shot SYSREF |                 | s               |
|                 | mode for single |                 | ynchronization. |
|                 | synchronization |                 |                 |
|                 | pulse.          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD              | SYSREF monitor  | 3               | For monitoring  |
| 9208_SYSREF_MON | mode to ensure  |                 | purposes in     |
|                 | that the SYSREF |                 | synchronized    |
|                 | signal is       |                 | applications.   |
|                 | active and      |                 |                 |
|                 | valid.          |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_POWERUP  | Normal          | 0x0             | Standard        |
|                 | operational     |                 | operational     |
|                 | power-up mode,  |                 | state.          |
|                 | device operates |                 |                 |
|                 | as normal.      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_STANDBY  | Standby mode    | 0x2             | Power           |
|                 | that reduces    |                 | management in   |
|                 | power           |                 | idle states.    |
|                 | consumption     |                 |                 |
|                 | while retaining |                 |                 |
|                 | the ability to  |                 |                 |
|                 | quickly resume. |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208          | Full power-down | 0x3             | Long periods of |
| _POWERDOWN      | mode, reduces   |                 | inactivity to   |
|                 | power but takes |                 | save power.     |
|                 | longer to       |                 |                 |
|                 | return to full  |                 |                 |
|                 | operation.      |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_ADC      | Variable IF     | 0               | Use when        |
| _NCO_VIF        | Mode            |                 | intermediate    |
|                 | configuration   |                 | frequencies are |
|                 | for the ADC     |                 | needed.         |
|                 | NCO.            |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_ADC      | Zero IF Mode    | 1               | Direct          |
| _NCO_ZIF        | for the ADC NCO |                 | conversion      |
|                 | to handle       |                 | applications.   |
|                 | signals         |                 |                 |
|                 | directly        |                 |                 |
|                 | centered at     |                 |                 |
|                 | zero frequency. |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_ADC      | Configures the  | 3               | Testing and     |
| _NCO_TEST       | NCO in test     |                 | diagnostics of  |
|                 | mode for        |                 | NCO             |
|                 | diagnostic      |                 | functionality.  |
|                 | purposes.       |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_ADC_BUFF | Sets buffer     | 0x4             | Low power       |
| _CURR_400_UA    | current to 400  |                 | operation with  |
|                 | µA for ADC      |                 | smaller         |
|                 | input stage     |                 | currents.       |
|                 | amplification.  |                 |                 |
+-----------------+-----------------+-----------------+-----------------+
| AD9208_ADC_BUFF | Configures      | 0x32            | Applications    |
| _CURR_1000_UA   | buffer current  |                 | requiring       |
|                 | to 1000 µA,     |                 | higher current  |
|                 | aids in driving |                 | handling.       |
|                 | load for high   |                 |                 |
|                 | input current   |                 |                 |
|                 | conditions.     |                 |                 |
+-----------------+-----------------+-----------------+-----------------+


Device Configuration
---------------------

Device Management
~~~~~~~~~~~~~~~~~~

Device Management includes key functions for the setup, initialization,
and teardown of the AD9208 device. The ``ad9208_initialize`` function sets
up the device’s SPI, GPIO, and configuration parameters, initializes
device components such as memory, handles device interfaces, such as
JESD204, and ensures a successful setup. ``ad9208_setup`` configures
operational settings like clock configurations, input modes, and channel
selection, while verifying device readiness. The ``ad9208_remove``
function handles resource deallocation and cleans up device structures
to ensure a graceful shutdown.

SPI Communication
~~~~~~~~~~~~~~~~~~

In SPI Communication, the ``ad9208_spi_xfer`` function manages SPI data
transfers, performing both write and read operations using specific data
buffers. This function ensures accurate data communication between the
host and the AD9208 device, interfacing with the SPI descriptor, and
managing resources and error logging to facilitate seamless
communication.

Timing and Test Mode
~~~~~~~~~~~~~~~~~~~~~

Timing and Test Mode operations involve functions such as
``ad9208_udelay`` and ``ad9208_testmode_set``. ``ad9208_udelay`` introduces
microsecond delays for precise timing adjustments, whereas
``ad9208_testmode_set`` configures the test mode for specific channels,
supporting various configurations for device validation and performance
testing.

Parameter Setup
~~~~~~~~~~~~~~~~

Parameter Setup covers functions that configure AD9208’s operational
parameters. Tasks like channel selection, decimation modes, data
formats, and gain settings are managed within ``ad9208_setup``. This
function handles comprehensive parameter settings, including digital
down-conversion (DDC) and numerically controlled oscillator (NCO)
configurations, by adjusting frequency, phase, and gain to meet specific
application requirements.

Device Initialization Example
-----------------------------

.. code-block:: C

    #include "no_os_spi.h"
    #include "xilinx_spi.h"
    #include "no_os_gpio.h"
    #include "xilinx_gpio.h"

    /* SPI initialization parameters for AD9208 */
    static struct xil_spi_init_param xil_spi_param = {
        .type = SPI_PL,
        .flags = 0
    };

    static struct no_os_spi_init_param ad9208_0_spi_param = {
        .max_speed_hz = 10000000,
        .mode = NO_OS_SPI_MODE_3,
        .chip_select = SPI_AD9208_0_CS,
        .platform_ops = &xil_spi_ops,
        .extra = &xil_spi_param
    };

    /* GPIO initialization parameters for AD9208 */
    static struct xil_gpio_init_param xilinx_gpio_init_param = {
        .type = GPIO_PL,
        .device_id = GPIO_DEVICE_ID
    };

    int main(void)
    {
        struct ad9208_dev *ad9208_device;
        struct ad9208_init_param ad9208_0_param = {
            .spi_init = &ad9208_0_spi_param,
            .gpio_init = &xilinx_gpio_init_param
        };
        struct ad9208_dev *ad9208_0_device;
        int32_t status;

        Xil_ICacheEnable();
        Xil_DCacheEnable();

        status = ad9208_initialize(&ad9208_0_device, &ad9208_0_param);
        if (status != 0) {
            xil_printf("ad9208_0_init() error: %"PRIi32"\n", status);
            goto error;
        }

    error:
        ad9208_remove(ad9208_0_device);

        return 0;
    }
