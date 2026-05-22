ADIN1140 no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`ADIN1140`

Overview
--------

The ADIN1140 is a low-power, single-port 10BASE-T1S MAC/PHY designed for
multi-drop industrial Ethernet applications. It complies with the IEEE
802.3cg-2019 standard for 10 Mbps single-pair Ethernet (SPE) and
implements the Physical Layer Collision Avoidance (PLCA) reconciliation
sublayer so multiple nodes can share a single mixing segment without
classic CSMA/CD overhead. The integrated MAC is accessed through a
4-wire SPI interface using the OPEN Alliance TC6 (OA-TC6) protocol,
allowing direct connectivity with host MCUs that lack an integrated
Ethernet MAC.

Applications
------------

- Building and factory automation
- HVAC and lighting control
- Field instruments and edge sensors
- Multi-drop sensor and actuator networks
- Industrial process monitoring

Driver Architecture
-------------------

The ADIN1140 driver is layered: the device-specific code in
``drivers/net/adin1140`` exposes the public API and translates it into
MMS-encoded register accesses and Ethernet frame transfers, while the
OPEN Alliance TC6 layer in ``drivers/net/oa_tc6`` implements the
chunk-based MAC/host SPI protocol and rides on top of the no-OS SPI
abstraction. Frames and control transactions share the same SPI link;
the OA-TC6 layer multiplexes them into and out of the MAC-PHY's TX/RX
FIFOs. Control register access and PLCA configuration go through the
same protocol as data.

::

  +-----------------------------------------------------------+
  |  Application  (lwIP / iperf / user code)                  |
  +-----------------------------------------------------------+
                            |
                            v
  +-----------------------------------------------------------+
  |  ADIN1140 driver  (drivers/net/adin1140)                  |
  |                                                           |
  |   Control API                 Data path                   |
  |   --------------------------  -------------------------   |
  |   adin1140_init / _remove     adin1140_write_fifo         |
  |   adin1140_mac_set_cfg/_get   adin1140_read_fifo          |
  |   adin1140_plca_set_cfg/_get  adin1140_poll               |
  |   adin1140_plca_reset                                     |
  |   adin1140_plca_get_diag                                  |
  |   adin1140_plca_get_status                                |
  |   adin1140_set_mac_addr                                   |
  |   adin1140_{broadcast,multicast}_filter                   |
  |   adin1140_set_promisc                                    |
  |   adin1140_mdio_{read,write}_c45                          |
  |   adin1140_sw_reset                                       |
  |   adin1140_link_state                                     |
  |   adin1140_set_irq_flag        (called from GPIO ISR)     |
  +-----------------------------------------------------------+
                            |
                            v
  +-----------------------------------------------------------+
  |  OPEN Alliance TC6 layer  (drivers/net/oa_tc6)            |
  |                                                           |
  |  - MMS-encoded register reads/writes (control chunks)     |
  |  - Chunked frame transport (TX/RX over data chunks)       |
  |  - Soft reset with STATUS0.RESETC polling                 |
  |    (oa_tc6_sw_reset)                                      |
  +-----------------------------------------------------------+
                            |
                            v
  +-----------------------------------------------------------+
  |  no_os_spi abstraction                                    |
  +-----------------------------------------------------------+
                            |
                            v
  +-----------------------------------------------------------+
  |  Platform SPI driver  (e.g. MAX32690 SPI, optionally DMA) |
  +-----------------------------------------------------------+
                            |
                            v   4-wire SPI (CS, SCLK, MOSI, MISO)
                            |   + INT  (GPIO IRQ -> set_irq_flag)
  +-----------------------------------------------------------+
  |  ADIN1140 MAC-PHY                                         |
  |  +------------------+    +---------------------------+    |
  |  | MAC + RX filters |<-->| PLCA reconciliation sub-  |    |
  |  |  (CONFIG0/2)     |    |  layer (CTRL0/1, STATUS,  |    |
  |  +------------------+    |   TOTMR, BURST, DIAG)     |    |
  |             |            +---------------------------+    |
  |             v                          |                  |
  |  +-----------------------------------------------------+  |
  |  |  10BASE-T1S PHY  (single-pair, multi-drop)          |  |
  |  +-----------------------------------------------------+  |
  +-----------------------------------------------------------+
                            |
                            v
                   Mixing segment (SPE bus)

Device Configuration
--------------------

The ADIN1140 driver exposes a set of public functions for configuring
and managing the MAC/PHY device. The functions are organized into
several categories.

Initialization and Resource Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``adin1140_init()`` function allocates the device descriptor,
initializes the SPI controller, performs the OA-TC6 software reset
(``oa_tc6_sw_reset()`` polls ``STATUS0.RESETC`` to completion), applies
the user-provided MAC ``CONFIG0`` and PLCA settings, installs the
default broadcast and unicast filters, and asserts ``CONFIG0.SYNC``.
The ``adin1140_remove()`` function tears the device down and releases
all resources.

Basic Register Access
~~~~~~~~~~~~~~~~~~~~~

``adin1140_reg_write()``, ``adin1140_reg_read()``, and
``adin1140_reg_update()`` provide direct 32-bit access to the device’s
MMS-encoded register space, including read-modify-write helpers that
preserve untouched bits.

MDIO Operations
~~~~~~~~~~~~~~~

``adin1140_mdio_read_c22()`` and ``adin1140_mdio_write_c22()`` access
the clause 22 PHY standard register window (32 registers exposed by the
MAC-PHY at MMS 0, offsets 0xFF00..0xFF1F).

``adin1140_mdio_read_c45()`` and ``adin1140_mdio_write_c45()`` provide
clause 45 access to the PHY MMD register space (PMA/PMD, PCS, and
vendor-specific PLCA), letting host code reach PHY-side controls
without driving an external MDIO bus.

MAC Address and Filter Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAC address filter slots are managed by ``adin1140_set_mac_addr()``
(programs the unicast filter), ``adin1140_broadcast_filter()``,
``adin1140_multicast_filter()``, and ``adin1140_set_promisc()`` for
toggling promiscuous mode via ``CONFIG2.FWD_UNK2HOST``.

MAC Configuration
~~~~~~~~~~~~~~~~~

``adin1140_mac_set_cfg()`` and ``adin1140_mac_get_cfg()`` operate on
``struct adin1140_mac_cfg``, which exposes the user-configurable
``CONFIG0`` bitfields: chunk payload size (``cps``), TX cut-through
threshold (``txcthresh``), TX/RX cut-through enables, cut-through
status append (``csarfe``), zero-align RX frame (``zarfe``), and frame
timestamp enable/size (``ftse``/``ftss``). ``TXFCSVE`` is always
cleared by the driver so the MAC appends the FCS on transmit; SYNC,
PROTE, and SEQE are managed internally and not exposed.

PLCA Configuration and Diagnostics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PLCA is configured through ``struct adin1140_plca_cfg``
(``enabled``, ``node_id``, ``node_cnt``, ``to_tmr``, ``burst_cnt``,
``burst_tmr``) via ``adin1140_plca_set_cfg()`` /
``adin1140_plca_get_cfg()``. ``adin1140_plca_get_status()`` reports
whether PLCA is currently active. ``adin1140_plca_reset()`` issues a
PLCA-only reset by setting ``CTRL0.PLCARST``.
``adin1140_plca_get_diag()`` decodes the three ``DIAG`` latches
(``RXINTO``, ``UNEXPB``, ``BCNBFTO``) into ``struct adin1140_plca_diag``
and optionally writes them back to acknowledge (W1C).

FIFO Data Transfer
~~~~~~~~~~~~~~~~~~

``adin1140_write_fifo()`` and ``adin1140_read_fifo()`` move Ethernet
frames in and out of the MAC FIFOs through the underlying OA-TC6
chunked SPI transactions, padding short frames to the 64-byte minimum
on TX.

Reset, Interrupts, and Link Status
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``adin1140_sw_reset()`` performs a full MAC-PHY reset (delegating to
``oa_tc6_sw_reset()``) and re-asserts ``CONFIG0.SYNC``.
``adin1140_set_irq_flag()`` is intended to be called from a GPIO ISR
to record that the MAC-PHY has asserted its INT pin; the main loop
then drains pending work via ``adin1140_poll()``. ``adin1140_link_state()``
reads ``STATUS1`` to report the current link state.

Driver Initialization Example
-----------------------------

.. code-block:: C

    #include <string.h>
    #include "adin1140.h"
    #include "no_os_spi.h"
    #include "no_os_print_log.h"

    int main(void)
    {
        struct adin1140_init_param init_param = {
            .comm_param = {
                .device_id = 1,
                .max_speed_hz = 15000000,
                .chip_select = 0,
                .mode = NO_OS_SPI_MODE_0,
                .platform_ops = &max_spi_ops,
                .extra = &adin1140_spi_extra_ip,
            },
            .mac_cfg = {
                .cps   = 0x6,   /* 64-byte chunk payload */
                .zarfe = true,
            },
            .plca_cfg = {
                .enabled  = true,
                .node_id  = 0,  /* 0 = PLCA coordinator */
                .node_cnt = 2,
            },
            .mac_address = { 0x00, 0x18, 0x80, 0x03, 0x25, 0x80 },
        };

        struct adin1140_desc *desc;
        int ret;

        ret = adin1140_init(&desc, &init_param);
        if (ret) {
            pr_err("ADIN1140 init failed: %d\n", ret);
            return ret;
        }

        /* ... use the device ... */

        adin1140_remove(desc);

        return 0;
    }
