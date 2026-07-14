Evaluating the ADIN1320
=======================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EV-ADIN1320FMCZ <https://www.analog.com/en/products/adin1320.html>`_

Overview
--------

The ADIN1320 is a low-power, single-port Gigabit Ethernet transceiver
that supports copper, fiber, and backplane Ethernet connectivity. Its key
features include low latency and energy-efficient operation, making it an
excellent choice for a wide range of industrial Ethernet applications where
performance and power efficiency are critical.

It integrates a high-performance Energy Efficient Ethernet (EEE) PHY core
along with a robust SerDes subsystem, enabling operation over 10BASE-T,
100BASE-TX, 1000BASE-T, 100BASE-FX, 1000BASE-X, and 1000BASE-KX physical
layers. The device supports multiple MAC-side interfaces including SGMII,
RGMII, RMII, and MII; and enables a variety of operating modes such as
SGMII/RGMII/RMII/MII to Copper, RGMII/RMII/MII to Fiber, RGMII/RMII/MII
to copper-or-fiber automatic media selection Copper to Fiber media
conversion, and SGMII to RGMII bridging. This versatility allows the
ADIN1320 to function as a copper PHY, fiber PHY, mixed-media PHY, media
converter, or MAC-interface bridge depending on system requirements.

The EV-ADIN1320FMCZ is an evaluation platform designed to simplify the
testing and integration of the EV-ADIN1320FMCZ, a low-power, single-port
Gigabit Ethernet PHY optimized for industrial applications. This board
supports both copper and fiber media, which enable the comprehensive
evaluation of Ethernet standards and SERDES-based protocols, which include:

* 10BASE-Te, 10BASE-T, 100BASE-TX, 1000BASE-T (Copper Ethernet)
* 1000BASE-X, 100BASE-FX, 1000BASE-KX (Fiber and SERDES protocols)

It is recommended to configure the EV-ADIN1320FMCZ using hardware switches
first to ensure proper initialization. The No-OS driver provides functions
that can return the current HW configuration and allows further customization
once a stable setup is established.

Full specifications on the ADIN1320 are available in the ADIN1320 data
sheet available from Analog Devices, Inc.

Hardware Specifications
-----------------------

Refer to EV-ADIN1320FMCZ UG available in the `product website <https://www.analog.com/en/products/adin1320.html>`_

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1320/src/common>`__.

The macros used in Common Data are defined in platform-specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/adin1320/src/platform>`__.

basic_init example
~~~~~~~~~~~~~~~~~~

This is a simple example that initializes a single ADIN1320 Ethernet PHY device
via MDIO-over-SPI. It performs a hardware reset, selects the RGMII MAC interface
with configurable Tx/Rx delays, enables auto-negotiation with advertisement for
10/100/1000 Mbps speeds (both half and full duplex), and configures Energy
Efficient Ethernet (EEE) for 100 and 1000 Mbps. The example exits software
power-down and polls the MII status until a link is established with a remote
partner, then reads back the advertised speeds, EEE settings, and the resolved
negotiated speed.

basic_init_2devices example
~~~~~~~~~~~~~~~~~~~~~~~~~~~

This example extends the basic initialization to both ADIN1320 PHY devices on
the EV-ADIN1320FMCZ board (Port 1 and Port 2). It performs the same
initialization, configuration, and link-up procedure for each device
independently, including RGMII configuration, auto-negotiation setup, EEE
configuration, and link status polling.

wake_on_lan example
~~~~~~~~~~~~~~~~~~~

This example demonstrates the Wake-on-LAN (WoL) functionality of the
ADIN1320 Ethernet PHY. It initializes a single ADIN1320 device, enables
the WoL interrupt, and sets up a GPIO interrupt handler on the INT_N pin.
When a Wake-on-LAN event is detected (such as a magic packet match or
link status change), the interrupt callback reads and reports the WoL
status, IRQ status, and IRQ latches via UART. The example runs in a
continuous loop, waiting for WoL events.

cmd example
~~~~~~~~~~~

This example provides an interactive UART terminal interface for the
EV-ADIN1320FMCZ board. It initializes both ADIN1320 PHY devices and
the board peripherals (GPIO switches, LEDs), reads the hardware
configuration switch state (CFG0–CFG3) to determine the firmware mode,
and enters a command processing loop. Supported commands include:

* ``mdioread <phy_addr>,<reg_addr>``
  — Reads a PHY register using MDIO Clause 22
* ``mdiowrite <phy_addr>,<reg_addr>,<value>``
  — Writes a PHY register using MDIO Clause 22
* ``mdiord_cl45 <phy_addr>,<dev_addr>,<reg_addr>``
  — Reads a PHY register using MDIO Clause 45
* ``mdiowr_cl45 <phy_addr>,<dev_addr>,<reg_addr>,<value>``
  — Writes a PHY register using MDIO Clause 45
* ``reset <device>``
  — Performs a hardware reset on a specified device
* ``help`` or ``?``
  — Lists available commands

The firmware mode selected via the CFG switches determines the startup behavior,
including options for GUI mode, SGMII path testing, and MDI path testing.

Maxim Platform
~~~~~~~~~~~~~~

Hardware Used
^^^^^^^^^^^^^

* `EV-ADIN1320FMCZ <https://www.analog.com/en/products/adin1320.html>`_
* `MAX32670 <https://www.analog.com/en/products/max32670.html>`_

Connections
^^^^^^^^^^^

+----------------+-----------------------+
| ADIN1320 Board | MAX32670 Pinouts      |
+----------------+-----------------------+
| SPI0_CLK       | P0.2                  |
+----------------+-----------------------+
| SPI0_MOSI      | P0.3                  |
+----------------+-----------------------+
| SPI0_MISO      | P0.4                  |
+----------------+-----------------------+
| P0_RESET_N     | P0.11                 |
+----------------+-----------------------+
| P1_RESET_N     | P0.10                 |
+----------------+-----------------------+
| P0_INT_N       | P0.19                 |
+----------------+-----------------------+
| P1_INT_N       | P0.21                 |
+----------------+-----------------------+
| GND            | GND                   |
+----------------+-----------------------+

Build Command
^^^^^^^^^^^^^

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``basic_init``, ``basic_init_2devices``, ``cmd``, ``wake_on_lan``.
Available boards: ``max32670evkit``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic_init example on the max32670evkit board)
   python tools/scripts/no_os_build.py build \
      --project adin1320 --variant basic_init --board max32670evkit

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project adin1320 --variant basic_init --board max32670evkit \
      --probe openocd --flash
