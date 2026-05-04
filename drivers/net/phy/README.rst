Ethernet PHY Drivers
=====================

Supported Devices
-----------------

- Marvell 88E1510/88E1518/88E1512/88E1514 (Alaska family)

Overview
--------

The ``phy`` component contains Ethernet PHY drivers that implement the
CAPI (Common API) Ethernet PHY interface defined in
``capi/include/capi_eth_phy.h``. Each driver provides an ops table
(``struct capi_eth_phy_ops``) that a MAC driver can use to initialize,
configure, and monitor a PHY without knowing its specific register map.

This decouples PHY-specific logic (register access, auto-negotiation
advertisement, RGMII delay configuration, interrupt handling) from the
MAC driver, making it straightforward to support new PHYs by adding a
new driver directory and ops table.

Marvell 88E1510
~~~~~~~~~~~~~~~

The Marvell 88E1510 driver (``mrvl_88e1510/``) supports the Alaska
88E151x family of Gigabit Ethernet copper PHYs. It communicates over
MDIO Clause 22 and uses the Marvell page-register architecture
(page select via register 22).

Features:

- Auto-negotiation with configurable speed/duplex advertisement
- Forced speed/duplex mode (10/100/1000 Mbps)
- RGMII TX/RX clock delay configuration (Page 2)
- MDI/MDIX auto-crossover and manual override
- Speed downshift on link failure
- Link-change interrupt support
- Power management (full power / power down)

Driver Initialization Example
------------------------------

PHY drivers are not used directly. Instead, pass the ops table and
extra configuration to a MAC driver:

.. code-block:: C

    #include "capi_marvell_88e1510.h"
    #include "no_os_xemacps.h"

    struct mrvl_88e1510_extra_config mrvl_cfg = {
        .rgmii = { .rx_delay_en = true, .tx_delay_en = true },
        .downshift_en = true,
        .downshift_retries = 3,
    };

    struct xemacps_init_param gem_ip = {
        .device_id = 0,
        .phy_ops   = &mrvl_88e1510_ops,
        .phy_extra = &mrvl_cfg,
        .phy_mode  = {
            .speed          = CAPI_ETH_PHY_SPEED_1G,
            .duplex         = CAPI_ETH_PHY_DUPLEX_FULL,
            .auto_negotiate = true,
            .mdix           = CAPI_ETH_MDIX_AUTO,
        },
        .hwaddr = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02},
    };

    struct xemacps_desc *desc;
    xemacps_init(&desc, &gem_ip);
