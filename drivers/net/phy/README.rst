Ethernet PHY Drivers
=====================

Supported Devices
-----------------

- Marvell 88E1510/88E1518/88E1512/88E1514 (Alaska family)
- Texas Instruments DP83867 (used on Xilinx ZCU102)

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

TI DP83867
~~~~~~~~~~

The TI DP83867 driver (``dp83867/``) supports the DP83867 gigabit
copper PHY used on the Xilinx ZCU102. It communicates over MDIO
Clause 22, with vendor extended registers accessed through the standard
MMD-indirect mechanism (REGCR/ADDAR at addresses 0x0D/0x0E).

Features:

- Auto-negotiation with configurable speed/duplex advertisement
- Forced speed/duplex mode (10/100/1000 Mbps)
- RGMII internal TX/RX clock delay with per-step timing codes
  (250 ps .. 4 ns, see ``DP83867_RGMII_DELAY_*``)
- MDI/MDIX auto-crossover and manual override
- Link-change interrupt support
- Power management (full power / power down)
- LED function control (page-less; single LEDCR1 register)

Driver Initialization Example
------------------------------

A PHY driver is bound to its MDIO transport at init time. The typical
flow with a Xilinx GEM MAC is:

.. code-block:: C

    #include "capi_marvell_88e1510.h"
    #include "capi_xemacps.h"
    #include <capi_eth_mac.h>
    #include <capi_mdio.h>
    #include <capi_eth_phy.h>

    struct capi_eth_mac_handle  *mac      = NULL;
    struct capi_mdio_handle *mdio_bus = NULL;
    struct capi_eth_phy_handle  *phy      = NULL;

    struct capi_eth_mac_init_config mac_cfg = {
        .identifier = GEM_DEVICE_ID,
        .ops        = &xemacps_capi_mac_ops,
    };
    capi_eth_mac_init(&mac, &mac_cfg);

    struct xemacps_mdio_init_config xmdio = { .mac = mac };
    struct capi_mdio_init_config mdio_cfg = {
        .ops   = &xemacps_capi_mdio_ops,
        .extra = &xmdio,
    };
    capi_mdio_init(&mdio_bus, &mdio_cfg);

    struct mrvl_88e1510_extra_config phy_extra = {
        .rgmii             = { .rx_delay_en = true, .tx_delay_en = true },
        .downshift_en      = true,
        .downshift_retries = 3,
    };
    struct capi_eth_phy_init_config phy_cfg = {
        .phy_addr = CAPI_ETH_PHY_ADDR_ANY,   /* auto-scan */
        .mdio_bus = mdio_bus,
        .extra    = &phy_extra,
        .ops      = &mrvl_88e1510_ops,
    };
    capi_eth_phy_init(&phy, &phy_cfg);
