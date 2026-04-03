XEmacPs no-OS Driver
=====================

Supported Devices
-----------------

- Xilinx Zynq-7000 SoC (GEM v2)
- Xilinx ZynqMP / Versal (GEM v3+)

Overview
--------

The XEmacPs driver is a standalone, bare-metal MAC driver for the
Processing System Gigabit Ethernet MAC (PS GEM) found in the Xilinx
Zynq-7000 and ZynqMP SoC families. It uses polling (no IRQs) and
copy-mode DMA to send and receive raw Ethernet frames, with no
dependency on any network stack. An upper layer such as the no-OS lwIP
glue (``lwip_xemacps``) can be used to connect this driver to a TCP/IP
stack.

Buffer descriptor memory is placed in a dedicated 1 MB uncached region
marked as ``DEVICE_MEMORY`` via ``Xil_SetTlbAttributes()``, eliminating
cache-coherency races between the CPU and the GEM DMA engine. Frame data
buffers are cache-line aligned and use explicit cache maintenance
(flush before TX, invalidate after RX).

PHY Handling
~~~~~~~~~~~~

During initialization the driver scans all 32 MDIO addresses (0-31)
by reading the PHY ID registers (registers 2 and 3). The first address
that returns valid, non-zero, non-0xFFFF values for both registers is
selected as the active PHY. This means the ``phy_addr`` field in the
initialization parameters is unused - the driver always auto-detects
the PHY address.

If the detected PHY is a Marvell 88E151x (matched by OUI), the driver
automatically configures RGMII TX/RX timing delays via page-2 register
21 and issues a soft-reset so the new timing takes effect. Non-Marvell
PHYs skip this step.

After PHY detection and configuration, the driver starts
auto-negotiation. The ``xemacps_poll()`` function monitors the PHY link
state via BMSR: when the link comes up it reads the negotiated speed
(10/100/1000 Mbps) and programs the MAC accordingly. When the link
drops, it restarts auto-negotiation so the PHY re-negotiates
automatically when the cable is reconnected.

GEM v2 Errata Workaround
~~~~~~~~~~~~~~~~~~~~~~~~~

The driver includes an automatic workaround for the Zynq-7000 GEM v2
silicon errata (`AR# 52028
<https://adaptivesupport.amd.com/s/article/52028>`_), where the RX data
path can lock up under heavy traffic of small frames. The workaround
flushes the RX packet buffer on every resource error and resets the RX
path when inactivity is detected. It is gated on runtime GEM version
detection and only activates on GEM v2.

GEM v3+ Queue Parking
~~~~~~~~~~~~~~~~~~~~~~

On ZynqMP (GEM v3+), the MAC exposes multiple priority queues. This
driver uses only queue 0 for RX and queue 1 for TX. Unused queues are
parked with terminated buffer descriptors (WRAP + NEW for RX, WRAP +
USED for TX) so the DMA engine does not access uninitialized memory.

Applications
------------

- Industrial Ethernet networking on Zynq-based boards
- IIO data acquisition over Ethernet
- TCP/UDP server applications
- Bare-metal network connectivity for sensor and control systems

Device Configuration
--------------------

The driver exposes a small public API for initialization, data transfer,
and polling. All functions are declared in ``no_os_xemacps.h``.

Initialization and Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``xemacps_init()`` function allocates the driver descriptor,
configures the GEM controller registers (network configuration, DMA
configuration, buffer descriptors), scans the MDIO bus for a PHY,
configures Marvell RGMII delays if applicable, starts auto-negotiation,
and starts the MAC. The ``xemacps_remove()`` function stops the MAC and
frees the descriptor.

Data Transfer
~~~~~~~~~~~~~

The ``xemacps_send()`` function copies an Ethernet frame into a static
TX buffer, flushes the D-cache, programs the TX buffer descriptor, and
polls until the GEM sets the TxUsed bit. The ``xemacps_recv()`` function
harvests a single completed RX buffer descriptor, copies the frame into
the caller's buffer, and reposts the descriptor to hardware.

Polling and Link State
~~~~~~~~~~~~~~~~~~~~~~

The ``xemacps_poll()`` function must be called once per polling cycle
before the ``xemacps_recv()`` loop. It monitors the PHY link state via
BMSR and configures the MAC operating speed when the link transitions
up. When the link drops, it restarts auto-negotiation. If the link is
down, RX processing is skipped entirely.

On Zynq-7000 (``PLATFORM_ZYNQ``), ``xemacps_poll()`` also runs the
AR# 52028 errata workaround and clears RX status error flags.

The ``xemacps_get_link_state()`` function returns the current link
state without polling the PHY - it returns the cached value from the
last ``xemacps_poll()`` call.

Multicast Filtering
~~~~~~~~~~~~~~~~~~~

The ``xemacps_set_mcast_hash()`` function adds a multicast MAC address
to the GEM hash filter, enabling reception of frames destined for that
multicast group (e.g. mDNS at ``01:00:5E:00:00:FB``).

Build-time Feature Flags
~~~~~~~~~~~~~~~~~~~~~~~~~

The following defines can be set at build time (e.g. via
``CFLAGS=-DGEM_PROMISC``) to toggle GEM features in the network
configuration register:

.. list-table::
   :header-rows: 1

   * - Define
     - Default
     - Effect
   * - ``GEM_PROMISC``
     - off
     - Receive all frames regardless of destination address
   * - ``GEM_HALF_DUPLEX``
     - off (full duplex)
     - Disable full-duplex mode
   * - ``GEM_NO_BROADCAST``
     - off
     - Reject broadcast frames
   * - ``GEM_STRIP_FCS``
     - off
     - Strip FCS from received frames
   * - ``GEM_NO_RX_CHKSUM``
     - off (checksum on)
     - Disable RX checksum offload
   * - ``GEM_NO_PAUSE``
     - off (pause on)
     - Disable pause frame support
   * - ``GEM_NO_MCAST_HASH``
     - off (hash on)
     - Disable multicast hash filter
   * - ``GEM_UCAST_HASH``
     - off
     - Enable unicast hash filter
   * - ``GEM_SGMII``
     - off
     - Enable SGMII / PCS mode (for PS GTR serial Ethernet)

Driver Initialization Example
------------------------------

.. code-block:: C

    #include <stdint.h>
    #include "no_os_xemacps.h"

    int main(void)
    {
        struct xemacps_init_param param = {
            .device_id = 0,
            .hwaddr    = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02},
        };

        struct xemacps_desc *desc;
        uint8_t buf[XEMACPS_MAX_FRAME_SIZE];
        uint32_t len;
        int ret;

        ret = xemacps_init(&desc, &param);
        if (ret)
            return ret;

        /* Main polling loop */
        while (1) {
            xemacps_poll(desc);

            if (!xemacps_get_link_state(desc))
                continue;

            ret = xemacps_recv(desc, buf, &len);
            if (ret)
                break;

            if (len > 0) {
                /* Process received frame in buf[0..len-1] */
            }
        }

        xemacps_remove(desc);
        return ret;
    }
