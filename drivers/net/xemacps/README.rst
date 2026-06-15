XEmacPs CAPI MAC Driver
========================

Supported Devices
-----------------

- Xilinx Zynq-7000 SoC (GEM v2)
- Xilinx ZynqMP / Versal (GEM v3+)

Overview
--------

This driver implements the no-OS Common API (CAPI) MAC contract
(``capi/include/capi_eth_mac.h``) for the Processing System Gigabit
Ethernet MAC (PS GEM) found in the Xilinx Zynq-7000 and ZynqMP SoC
families. It uses polling (no IRQs) and copy-mode DMA to send and
receive raw Ethernet frames, with no dependency on any network stack
or PHY driver. An upper layer such as the no-OS lwIP glue
(``lwip_xemacps``) is responsible for orchestrating both this MAC and
an external CAPI PHY driver, and for stitching link-state changes
between them.

The driver exports a single ``capi_eth_mac_ops`` table —
``xemacps_capi_mac_ops`` — and a small set of XEmacPs-specific helpers
(``xemacps_mdio_read``, ``xemacps_mdio_write``, ``xemacps_mdio_scan_phy``,
``xemacps_poll_rx_errata``) for tasks not covered by the CAPI contract.

Buffer descriptor memory is placed in a dedicated 1 MB uncached region
marked as ``DEVICE_MEMORY`` via ``Xil_SetTlbAttributes()``, eliminating
cache-coherency races between the CPU and the GEM DMA engine. Frame
data buffers are cache-line aligned and use explicit cache maintenance
(flush before TX, invalidate after RX).

The driver includes an automatic workaround for the Zynq-7000 GEM v2
silicon errata (`AR# 52028
<https://adaptivesupport.amd.com/s/article/52028>`_), where the RX data
path can lock up under heavy traffic of small frames. The workaround
flushes the RX packet buffer on every resource error and resets the RX
path when inactivity is detected. It is gated on runtime GEM version
detection and only activates on GEM v2.

Applications
------------

- Industrial Ethernet networking on Zynq-based boards
- IIO data acquisition over Ethernet
- TCP/UDP server applications
- Bare-metal network connectivity for sensor and control systems

Architecture
------------

This driver implements only the MAC half of the Ethernet stack. PHY
drivers are separate (``drivers/net/phy/...``) and use the CAPI PHY
contract. The MAC and PHY meet in the application or lwIP glue, never
in the driver. The MDIO bus, which is physically a peripheral of the
GEM, is exposed via ``xemacps_mdio_read`` / ``xemacps_mdio_write`` so
the application can wire it into a CAPI PHY's
``capi_eth_phy_init_config.fn_read`` / ``fn_write`` callbacks.

Driver Initialization Example
------------------------------

.. code-block:: C

    #include "capi_xemacps.h"
    #include <capi_eth_mac.h>

    int main(void)
    {
        uint8_t hwaddr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

        struct capi_eth_mac_init_config cfg = {
            .identifier  = XPAR_XEMACPS_0_DEVICE_ID,
            .mac_address = (capi_eth_mac_addr *)hwaddr,
            .ops         = &xemacps_capi_mac_ops,
        };

        struct capi_eth_mac_handle *mac = NULL;
        if (capi_eth_mac_init(&mac, &cfg))
            return -1;

        /* Wire a CAPI PHY here using xemacps_mdio_read/write */

        /* Configure speed/duplex once the PHY reports link up */
        struct capi_eth_mac_config run = {
            .speed = CAPI_ETH_MAC_SPEED_1G,
            .duplex = CAPI_ETH_MAC_DUPLEX_FULL,
            .address_broadcast = true,
            .address_multicast = true,
        };
        capi_eth_mac_configure(mac, &run);
        capi_eth_mac_power_control(mac, CAPI_ETH_MAC_POWER_FULL);

        uint8_t buf[XEMACPS_MAX_FRAME_SIZE];
        uint32_t size;
        while (1) {
            xemacps_poll_rx_errata(mac);
            if (capi_eth_mac_get_rx_frame_size(mac, &size) == 0 && size) {
                int n = capi_eth_mac_read_frame(mac, buf, sizeof(buf), NULL);
                if (n > 0) {
                    /* Process received frame in buf[0..n-1] */
                }
            }
        }

        capi_eth_mac_deinit(mac);
        return 0;
    }
