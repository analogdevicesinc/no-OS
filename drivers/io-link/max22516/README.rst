MAX22516 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`MAX22516`

Overview
--------

The MAX22516 is an IO-Link data link controller with integrated transceiver
designed for industrial automation applications. It combines a 24V C/Q
transceiver, auxiliary digital I/O, integrated DC-DC converter, 5V and 3.3V
linear regulators, and a full-featured IO-Link data link controller in a
single device. The MAX22516 can operate autonomously, managing all master
message requests without external microcontroller intervention during normal
operation. It supports standard IO-Link data rates (COM1, COM2, COM3) and
operates from 11V to 36V supply voltage with integrated short-circuit
protection, thermal shutdown, and comprehensive status monitoring.

**Important Note**: This driver provides hardware control and configuration
for the MAX22516 device. While the MAX22516 includes integrated IO-Link protocol
functionality, users may need to implement or integrate an IO-Link stack for
complete application functionality depending on their specific requirements.

Applications
------------

- Industrial automation systems
- Factory automation equipment
- Process control systems
- IO-Link master implementations
- Industrial sensor networks
- Machine-to-machine communication

Operation Modes
----------------

+------------------------+-----------------+-------------------------------------+-----------------+
| **Mode Name**          | **Description** | **Configuration**                   | **Typical Use   |
|                        |                 |                                     | Case**          |
+------------------------+-----------------+-------------------------------------+-----------------+
| CQ Push-Pull Mode      | Configures CQ   | max22516_setup_cq_pp()              | Standard        |
|                        | output as       | - Enables CQ output                 | IO-Link         |
|                        | push-pull       | - Sets push-pull configuration      | communication   |
|                        | driver          | - Configures 200mA current limit    | with robust     |
|                        |                 | - Sets 500µs current limit blanking | drive           |
|                        |                 | - Enables auto-retry with 200ms     | capability      |
+------------------------+-----------------+-------------------------------------+-----------------+
| CQ PNP Mode            | Configures CQ   | max22516_setup_cq_pnp()             | Interfacing     |
|                        | output for PNP  | - Enables CQ output                 | with PNP        |
|                        | transistor      | - Sets PNP configuration            | transistor      |
|                        | interface       | - Configures 200mA current limit    | based IO-Link   |
|                        |                 | - Sets 500µs current limit blanking | devices         |
|                        |                 | - Enables auto-retry with 200ms     |                 |
+------------------------+-----------------+-------------------------------------+-----------------+
| CQ NPN Mode            | Configures CQ   | max22516_setup_cq_npn()             | Interfacing     |
|                        | output for NPN  | - Enables CQ output                 | with NPN        |
|                        | transistor      | - Sets NPN configuration            | transistor      |
|                        | interface       | - Configures 200mA current limit    | based IO-Link   |
|                        |                 | - Sets 500µs current limit blanking | devices         |
|                        |                 | - Enables auto-retry with 200ms     |                 |
+------------------------+-----------------+-------------------------------------+-----------------+
| CQ Disabled Mode       | Disables CQ     | max22516_setup_cq_dis()             | Power saving    |
|                        | output          | - Powers down CQ output             | mode when CQ    |
|                        |                 |                                     | functionality   |
|                        |                 |                                     | not required    |
+------------------------+-----------------+-------------------------------------+-----------------+
| DO Push-Pull Mode      | Configures DO   | max22516_setup_do_pp()              | Digital output  |
|                        | output as       | - Enables DO output                 | control with    |
|                        | push-pull       | - Sets push-pull configuration      | strong drive    |
|                        | driver          | - Configures 200mA current limit    | capability      |
|                        |                 | - Sets 500µs current limit blanking |                 |
|                        |                 | - Enables auto-retry with 200ms     |                 |
+------------------------+-----------------+-------------------------------------+-----------------+
| DO PNP Mode            | Configures DO   | max22516_setup_do_pnp()             | Digital output  |
|                        | output for PNP  | - Enables DO output                 | interfacing     |
|                        | transistor      | - Sets PNP configuration            | with PNP        |
|                        | interface       | - Configures 200mA current limit    | transistor      |
|                        |                 | - Sets 500µs current limit blanking | loads           |
|                        |                 | - Enables auto-retry with 200ms     |                 |
+------------------------+-----------------+-------------------------------------+-----------------+
| DO NPN Mode            | Configures DO   | max22516_setup_do_npn()             | Digital output  |
|                        | output for NPN  | - Enables DO output                 | interfacing     |
|                        | transistor      | - Sets NPN configuration            | with NPN        |
|                        | interface       | - Configures 200mA current limit    | transistor      |
|                        |                 | - Sets 500µs current limit blanking | loads           |
|                        |                 | - Enables auto-retry with 200ms     |                 |
+------------------------+-----------------+-------------------------------------+-----------------+
| DO Disabled Mode       | Disables DO     | max22516_setup_do_dis()             | Power saving    |
|                        | output          | - Powers down DO output             | mode when DO    |
|                        |                 |                                     | functionality   |
|                        |                 |                                     | not required    |
+------------------------+-----------------+-------------------------------------+-----------------+

Device Configuration
--------------------

Device Initialization
~~~~~~~~~~~~~~~~~~~~~~

The ``max22516_init`` function initializes the MAX22516 by allocating memory
for the device structure and configuring the SPI communication interface.
This prepares the device for operation and enables access to all driver
functions for IO-Link master functionality.

SPI Communication
~~~~~~~~~~~~~~~~~

The driver provides comprehensive SPI communication functions:

- ``max22516_write`` - Single register write operations
- ``max22516_read`` - Single register read operations  
- ``max22516_update`` - Masked register updates
- ``max22516_burst_write_register`` - Multi-byte write operations
- ``max22516_burst_read_register`` - Multi-byte read operations

These functions handle the SPI protocol requirements and provide reliable
communication with the MAX22516 device.

IO-Link Master Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

The driver implements core IO-Link master functionality:

- ``max22516_set_min_ctmr`` - Configure minimum cycle time
- ``max22516_get_mst_ctmr`` - Read master cycle time
- ``max22516_set_id`` - Set device vendor ID, device ID, and function ID
- ``max22516_get_dl_mode`` - Get data link mode
- ``max22516_get_iol_err_cnt`` / ``max22516_clr_iol_err_cnt`` - Monitor/clear IO-Link error counts
- ``max22516_get_frm_err_cnt`` / ``max22516_clr_frm_err_cnt`` - Monitor/clear frame error counts

CQ (Communication/Control) Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CQ output control for IO-Link communication:

- ``max22516_set_cq`` - Set CQ output level (low/high/high-Z)
- ``max22516_get_cq`` - Read current CQ output state
- ``max22516_get_cq_stat`` - Get CQ fault status
- ``max22516_tx_set`` / ``max22516_tx_get`` - Control TX signal level
- ``max22516_txen_set`` - Enable/disable TX output

Digital Output (DO) Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Digital output control for auxiliary functions:

- ``max22516_do_set`` - Set DO output level
- ``max22516_do_get`` - Read current DO output state
- ``max22516_get_do_stat`` - Get DO fault status

Status Monitoring
~~~~~~~~~~~~~~~~~

Comprehensive device status monitoring:

- ``max22516_get_v24`` - Monitor 24V supply status
- ``max22516_get_thd`` - Monitor thermal status (warning/shutdown)

LED Control
~~~~~~~~~~~

Integrated LED control for status indication:

- ``max22516_set_led1`` - Configure LED1 timing control
- ``max22516_set_led2`` - Configure LED2 timing control

Event and Watchdog Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Event handling and watchdog functionality:

- ``max22516_set_event`` - Configure device events with qualification and codes
- ``max22516_setup_watchdog`` - Configure watchdog timer with timeout and event handling

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdio.h>
   #include <stdlib.h>
   #include "no_os_spi.h"
   #include "max22516.h"
   #include "no_os_error.h"
   #include "no_os_delay.h"

   struct max22516_dev *max22516_device;

   // SPI initialization parameters
   struct no_os_spi_init_param max22516_spi_param = {
       .device_id = 0,
       .max_speed_hz = 1000000,  // 1MHz SPI clock
       .chip_select = 0,
       .mode = NO_OS_SPI_MODE_0,
       .platform_ops = &platform_spi_ops,  // Platform-specific SPI ops
       .extra = &platform_spi_extra        // Platform-specific extra parameters
   };

   // MAX22516 initialization parameters
   struct max22516_init_param max22516_init_param = {
       .spi_init = &max22516_spi_param
   };

   int main(void)
   {
       int32_t ret;
       uint8_t device_status;

       // Initialize MAX22516 device
       ret = max22516_init(&max22516_device, &max22516_init_param);
       if (ret) {
           printf("MAX22516 initialization failed: %d\n", ret);
           return ret;
       }

       // Configure CQ output for push-pull operation
       ret = max22516_setup_cq_pp(max22516_device);
       if (ret) {
           printf("CQ configuration failed: %d\n", ret);
           goto cleanup;
       }

       // Configure DO output for push-pull operation
       ret = max22516_setup_do_pp(max22516_device);
       if (ret) {
           printf("DO configuration failed: %d\n", ret);
           goto cleanup;
       }

       // Set device identification
       ret = max22516_set_id(max22516_device, 0x01AB, 0x123456, 0x0001);
       if (ret) {
           printf("Device ID configuration failed: %d\n", ret);
           goto cleanup;
       }

       // Set minimum cycle time to 10ms (100 * 100µs)
       ret = max22516_set_min_ctmr(max22516_device, 100);
       if (ret) {
           printf("Cycle time configuration failed: %d\n", ret);
           goto cleanup;
       }

       // Configure LED1 for status indication
       ret = max22516_set_led1(max22516_device, 0x1000);
       if (ret) {
           printf("LED1 configuration failed: %d\n", ret);
           goto cleanup;
       }

       // Setup watchdog with 5-second timeout
       ret = max22516_setup_watchdog(max22516_device, 50, 0x00, 1, 0x01);
       if (ret) {
           printf("Watchdog configuration failed: %d\n", ret);
           goto cleanup;
       }

       printf("MAX22516 initialized successfully!\n");

       // Main application loop
       while (1) {
           // Monitor device temperature status
           ret = max22516_get_thd(max22516_device, &device_status);
           if (ret == 0) {
               if (device_status == 2) {
                   printf("Warning: Thermal shutdown detected!\n");
               } else if (device_status == 1) {
                   printf("Warning: Thermal warning detected!\n");
               }
           }

           // Monitor 24V supply status
           ret = max22516_get_v24(max22516_device, &device_status);
           if (ret == 0) {
               if (device_status == 2) {
                   printf("Warning: 24V supply error!\n");
               } else if (device_status == 1) {
                   printf("Warning: VM supply error!\n");
               }
           }

           // Set CQ output high for IO-Link communication
           ret = max22516_set_cq(max22516_device, 1);
           if (ret) {
               printf("CQ control failed: %d\n", ret);
           }

           // Delay before next iteration
           no_os_mdelay(1000);
       }

   cleanup:
       // Clean up resources
       max22516_remove(max22516_device);
       return ret;
   }