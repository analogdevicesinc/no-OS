Xilinx Zynq lwIP Ethernet Demo
===============================

Overview
--------

This project demonstrates bare-metal Ethernet networking on Xilinx
Zynq-7000 and ZynqMP boards using the PS GEM (XEmacPs) and the lwIP
raw API (NO_SYS=1). Two example applications are available:

- **IIO example** (default) - exposes virtual ADC/DAC devices over the
  iiod TCP protocol on port 30431.
- **Echo server example** - runs a TCP echo server on port 7

Board Compatibility
-------------------

The project is generic for any Xilinx board with a PS GEM. The driver
automatically scans MDIO addresses 0-31 to find the on-board PHY, so
no board-specific PHY address configuration is required.

If the detected PHY is a Marvell 88E151x, the driver configures RGMII
TX/RX timing delays automatically.

Known Errata
------------

The Zynq-7000 GEM v2 has a silicon bug (`AR# 52028
<https://adaptivesupport.amd.com/s/article/52028>`_) where the RX data
path can lock up under heavy traffic of small frames when a large number
of receive resource errors are generated.

The XEmacPs driver includes an automatic workaround that:

1. Flushes a packet from the RX buffer on every resource error to reduce
   the error rate.
2. Monitors the received-frame counter - if it stops incrementing for
   two consecutive polling cycles while resource errors are present, the
   driver toggles the RX enable bit to reset the receive path.

This workaround only activates on GEM v2 (Zynq-7000) and is skipped on
GEM v3+ (ZynqMP / Versal).

Prerequisites
-------------

- Xilinx Vitis / Vivado (``xsct`` and ``vitis`` must be in PATH)
- A hardware description file (``*.xsa``) generated from a Vivado project
  targeting your board
- ``libiio`` installed on the host (for the IIO example)
- ``netcat`` / ``nc`` installed on the host (for the echo example)

Building
--------

Place the ``.xsa`` file inside ``projects/xilinx_lwip/`` before building.
The build system detects it automatically and generates the BSP.

**IIO example (default):**

.. code-block:: bash

   cd projects/xilinx_lwip
   make PLATFORM=xilinx

**Echo server example:**

.. code-block:: bash

   cd projects/xilinx_lwip
   make PLATFORM=xilinx ECHO_EXAMPLE=y

Build outputs are written to ``projects/xilinx_lwip/build/``.

Flashing
--------

.. code-block:: bash

   make run

This calls ``xsct`` to program the bitstream and ELF over JTAG.

Build Flags
-----------

The following flags can be passed to ``make`` or set in the Makefile:

.. list-table::
   :header-rows: 1

   * - Flag
     - Default
     - Description
   * - ``ECHO_EXAMPLE=y``
     - unset (IIO example)
     - Build the TCP echo server instead of the IIO server
   * - ``NO_OS_IP``
     - unset (DHCP/AutoIP)
     - Static IP address, e.g. ``192.168.1.100``
   * - ``NO_OS_NETMASK``
     - unset
     - Subnet mask, e.g. ``255.255.255.0`` (required with ``NO_OS_IP``)
   * - ``NO_OS_GATEWAY``
     - unset
     - Default gateway, e.g. ``192.168.1.1`` (required with ``NO_OS_IP``)
   * - ``DEBUG=1``
     - unset
     - Disable ``-O2``, enable ``-O0 -g3`` for debugging
   * - ``VERBOSE=y``
     - unset
     - Print each compiler and linker invocation

GEM Feature Flags
~~~~~~~~~~~~~~~~~~

The following defines toggle GEM hardware features via
``CFLAGS=-D<flag>``:

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
     - off (RX checksum on)
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
     - Enable SGMII / PCS mode (for boards using PS GTR serial Ethernet)

Example with static IP and promiscuous mode:

.. code-block:: bash

   make PLATFORM=xilinx NO_OS_IP=169.254.3.1 NO_OS_NETMASK=255.255.0.0 NO_OS_GATEWAY=0.0.0.0 CFLAGS="-DGEM_PROMISC"

Network Configuration
---------------------

By default the board uses DHCP, falling back to AutoIP (``169.254.x.x``)
if no DHCP server is present. The assigned IP address and gateway are
printed over the UART console at startup (115200 baud, 8N1).

The MAC address is set in the example source files
(``src/examples/iio_lwip/iio_lwip_example.c`` and
``src/examples/echo_lwip/echo_lwip_example.c``). Change the last three
bytes to ensure uniqueness on your network:

.. code-block:: c

   static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

Connecting - IIO Example
------------------------

The board exposes two virtual devices: ``adc_demo`` (input) and
``dac_demo`` (output), served over the iiod protocol on **TCP port 30431**.

Connecting - Echo Example
--------------------------

The echo server listens on **TCP port 7** and reflects every byte back
to the sender.
