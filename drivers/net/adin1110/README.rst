ADIN1110 no-OS Driver
======================

Supported Devices
-----------------

- :adi:`ADIN1110`
- :adi:`ADIN2111`

Overview
--------

The ADIN1110 is an ultra-low-power, single-port, 10BASE-T1L transceiver
designed for industrial Ethernet applications. It complies with the IEEE
802.3cg-2019 Ethernet standard for long-reach, 10 Mbps single-pair
Ethernet (SPE). It features an integrated media access control (MAC)
interface with a 4-wire serial peripheral interface (SPI) that can be
configured to use the Open Alliance SPI protocol or a generic SPI
protocol, enabling direct connectivity with lower-power processors that
lack an integrated MAC. The device supports programmable transmit
levels, external termination resistors, and independent receive and
transmit pins, making it ideally suited for intrinsic safety
applications. Additionally, the ADIN1110 boasts integrated voltage
supply monitoring, power-on reset circuitry, and is available in a
compact 40-lead, 6 mm × 6 mm LFCSP, ensuring robust operation in
industrial environments while maintaining low overall system-level power
consumption.

Applications
------------

- Field instruments
- Building automation and fire safety
- Factory automation
- Edge sensors and actuators
- Condition monitoring and machine connectivity

Device Configuration
--------------------

The ADIN1110 driver’s header file exposes a comprehensive set of public
functions that enable complete configuration and management of the
Ethernet device. The public functions are organized into several
categories to ensure clarity and ease of use.

Initialization and Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The initialization-related functions include ``adin1110_init()`` and
``adin1110_remove()``. The ``adin1110_init()`` function allocates and
configures the device descriptor, initializes the SPI (and optionally,
GPIO) interfaces, and executes all required reset sequences and
configuration setups. The ``adin1110_remove()`` function deallocates
resources and cleans up the device descriptor, ensuring a safe removal
of the device from the system.

Basic Register Access
~~~~~~~~~~~~~~~~~~~~~

The basic register access functions provided in the header include
``adin1110_reg_write()``, ``adin1110_reg_read()``, and
``adin1110_reg_update()``. These functions allow for 32-bit register
writes, register reads, and streamlined register modifications by
reading a register’s current value, applying bitmasks, and writing the
updated value back to the register.

MDIO Operations
~~~~~~~~~~~~~~~

For accessing PHY registers using MDIO protocols, the driver offers a
set of functions: ``adin1110_mdio_read()``, ``adin1110_mdio_write()``,
``adin1110_mdio_write_c45()``, and ``adin1110_mdio_read_c45()``. These
functions support both standard clause 22 and extended clause 45 MDIO
transactions, facilitating complete PHY register operations.

MAC Address and Filter Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAC address and filtering capabilities are managed by
``adin1110_set_mac_addr()``, ``adin1110_clear_mac_addr()``, and
``adin1110_broadcast_filter()``. These functions respectively program a
MAC address filter, remove an existing MAC address filter, and toggle
the broadcast address filter on the device.

FIFO Data Transfer Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For managing data exchange via the device’s FIFO channels, the driver
provides ``adin1110_write_fifo()`` and ``adin1110_read_fifo()``. The
``adin1110_write_fifo()`` function is used to write Ethernet frame data
into the TX FIFO with proper handling of frame length padding and
alignment, while the ``adin1110_read_fifo()`` function handles reading
from the RX FIFO and processing received data.

Reset and Link Status Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ensuring reliable operation includes robust reset and status monitoring
functions. This group comprises ``adin1110_mac_reset()``,
``adin1110_sw_reset()``, ``adin1110_link_state()``, and
``adin1110_phy_reset()``. These functions perform hardware and software
resets, verify link status, and reset the PHY component while ensuring
correct operation per hardware specifications.

Device Feature Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Additional device-specific configuration is enabled through
``adin1110_set_promisc()``, ``adin1110_setup_phy()``, and
``adin1110_setup_mac()``. The ``adin1110_set_promisc()`` function
toggles promiscuous mode, the ``adin1110_setup_phy()`` function handles
PHY power management and autonegotiation, and the
``adin1110_setup_mac()`` function configures the MAC settings including
CRC operations, interrupt setup, and MAC address filtering.

Driver Initialization Example
-----------------------------

.. code-block:: C

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include <stdbool.h>
    #include "adin1110.h"
    #include "no_os_spi.h"
    #include "no_os_gpio.h"
    #include "no_os_delay.h"
    #include "no_os_print_log.h"
    #include "no_os_alloc.h"
    #include "no_os_crc8.h"
    #include "no_os_util.h"

    int main(void)
    {
        struct adin1110_init_param init_param = {
            .chip_type = ADIN1110,
            .comm_param = {
                .device_id = 1,
                .max_speed_hz = 15000000,
                .chip_select = 0,
                .mode = NO_OS_SPI_MODE_0,
                .platform_ops = &max_spi_ops,
                .extra = &adin1110_spi_extra_ip
            },
            .reset_param = {
                .number = 19,
                .platform_ops = &max_gpio_ops,
                .extra = &adin1110_reset_gpio_extra
            },
            .mac_address = {0x00, 0x1A, 0x11, 0x22, 0x33, 0x44},
            .append_crc = true
        };

        struct adin1110_desc *desc;
        int32_t ret;

        no_os_print_log("Starting ADIN1110 initialization...\n");
        ret = adin1110_init(&desc, &init_param);
        if (ret) {
            no_os_print_log("ADIN1110 initialization failed with error: %d\n", ret);
            goto error_exit;
        }

        no_os_print_log("ADIN1110 initialization successful\n");
        goto end;

    error_exit:
        if (desc) {
            int32_t remove_ret = adin1110_remove(desc);
            if (remove_ret) {
                no_os_print_log("Error during resource cleanup: %d\n", remove_ret);
            }
        }
        no_os_print_log("Exiting due to initialization error\n");

    end:
        return ret;
    }
