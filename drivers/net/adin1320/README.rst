ADIN1320 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`ADIN1320 <https://www.analog.com/ADIN1320>`_

Overview
--------

The ADIN1320 driver provides an interface for controlling the ADIN1320 Ethernet 
PHY device within the ADI No-OS framework. It supports standard Ethernet 
functionality for both Copper and Fiber media modes, MAC interface control, and 
Wake-on-LAN (WoL).

Features
^^^^^^^^

* Standard IEEE PHY Functions (Clause 22/45)
* MAC Interface Control (RGMII, RMII, MII, and SGMII)
* Wake-on-LAN (WoL) 
* Copper & Fiber Media Support 
* Auto Media Detection 
* Low-level MDIO Access APIs 

Documentation
^^^^^^^^^^^^^

* Inline API descriptions in source code. 
* User Guide available on the product webpage (TBD).

Reset Options
^^^^^^^^^^^^^

* Hardware Reset : Use **adin1320_hard_reset** if GPIO for hardware reset is initialized.
* Software Reset : Use **adin1320_soft_reset** to reset specific register groups.

Refer to the ADIN1320 datasheet for details on affected registers.

MDIO Communication APIs
^^^^^^^^^^^^^^^^^^^^^^^

The driver provides low-level MDIO access for custom feature implementation:

* **adin1320_read** - Read a 16-bit PHY register. 
* **adin1320_write** - Write a 16-bit PHY register. 
* **adin1320_write_bits** - Perform masked bit writes without overwriting other bits. 

Note: The driver covers standard Ethernet features and selected advanced features. Users can implement additional functionality using these MDIO APIs.

API Categories
--------------

Setup & Teardown
^^^^^^^^^^^^^^^^

* **adin1320_init** - Initializes the ADIN1320 Ethernet PHY device by setting up the MDIO interface, performing a hardware or software reset, and validating the PHY ID. This function ensures the PHY is ready for operation and communication over the MDIO bus.
* **adin1320_remove** - Cleans up and deallocates resources associated with the ADIN1320 device

Device Control
^^^^^^^^^^^^^^

* **adin1320_hard_reset** - Performs a hardware reset of the device using the GPIO reset pin
* **adin1320_soft_reset** - Performs a software reset of the device for specific register groups
* **adin1320_get_device_id** - Read the PHY device ID
* **adin1320_set_software_powerdown** - Enables or Disables Software power-down mode
* **adin1320_get_software_powerdown** - Determine if the PHY is in software power-down or not.
* **adin1320_config_gp_clk** - Sets GP clock to use
* **adin1320_config_clk25_ref** - Enables/disables 25 MHz reference clock
* **adin1320_led_cfg** - Configures LED settings in general.

MAC Interface Control
^^^^^^^^^^^^^^^^^^^^^

* **adin1320_select_mac_interface** - Sets the MAC interface
* **adin1320_config_rgmii** - Configures the Tx and Rx delays
* **adin1320_reset_rmii_fifo** - Resets the RMII FIFO

Wake-on-LAN (WoL)
^^^^^^^^^^^^^^^^^

* **adin1320_wol_en_cfg** - Enables/disables the wake-on-LAN feature
* **adin1320_wol_address** - Sets the station address for wake-on-LAN
* **adin1320_wol_sys_cfg** - Configures the wake-on-LAN in the system
* **adin1320_wol_sig_cfg** - Configures the signaling of wake-on-LAN events
* **adin1320_wol_wake_cfg** - Configures the wake-on-LAN wake behavior
* **adin1320_mgc_match_cfg** - Configures the magic packet matchings to enable
* **adin1320_mgc_check_cfg** - Configures the magic packet checks to enable
* **adin1320_mgc_key_cfg** - Configures the magic packet SecureOn key

Copper Mode APIs
^^^^^^^^^^^^^^^^

Auto-negotiation
""""""""""""""""

* **adin1320_cu_autoneg** - Enable/disable auto-negotiation. Also performs restart on auto-negotiation upon enabling.
* **adin1320_cu_autoneg_adv_cfg** - Configures which auto-negotiation advertisement copper speeds are to be enabled.
* **adin1320_cu_get_autoneg_adv** - Gets all enabled auto-negotiation advertisement copper speeds.

Energy Efficient Ethernet (EEE)
"""""""""""""""""""""""""""""""

* **adin1320_set_eee** - Configures which energy efficient ethernet speeds are to be enabled.
* **adin1320_get_eee** - Gets all enabled energy efficient ethernet speeds.

Forced Speed Control
""""""""""""""""""""

* **adin1320_cu_forced_speed** - Sets the forced copper speed

Speed and Link Status
"""""""""""""""""""""

* **adin1320_cu_resolved_speed** - Gets the resolved/negotiated copper speed with the link partner
* **adin1320_cu_get_mii_status** - Allows reading of link status, auto-negotiation completion, link drop, jabber detection and remote fault. Can be configured which of these are to be returned as output. Reading all status bits handles concerns for status bit latches.

LED Control
"""""""""""

* **adin1320_cu_led_ctrl** - Configures LED settings for copper PHY.

Other Config Controls and Status
""""""""""""""""""""""""""""""""

* **adin1320_downspeed_cfg** - Configures which down-speeds are to be enabled.
* **adin1320_set_downspeed_retries** - If down-speed is enabled, this register bit specifies the number of retries the PHY must attempt to bring up a link at, and the advertised speed before advertising a lower speed
* **adin1320_cu_link_cfg** - Enables or Disables the Link.
* **adin1320_auto_mdix_cfg** - Configures the auto MDIX
* **adin1320_energy_detect_pwd_cfg** - Configures the energy detect power-down mode
* **adin1320_get_energy_detect_pwd_stat** - Gets the status of energy detect power-down mode
* **adin1320_master_slave_config** - Configure PHY as Master or slave only for 1 Gb speed
* **adin1320_get_master_slave_status** - Get Master Slave status. 
* **adin1320_cu_config_interrupt** - Configures the interrupt mask for the copper PHY. Enables or disables specific interrupt events.
* **adin1320_cu_read_irq_status** - Read interrupt status.

SerDes Mode APIs
^^^^^^^^^^^^^^^^

Auto-negotiation
""""""""""""""""

* **adin1320_sd_autoneg** - Enable/disable auto-negotiation. Also performs restart on auto-negotiation upon enabling.
* **adin1320_sd_autoneg_adv_cfg** - Configures which auto-negotiation advertisement serdes speeds are to be enabled.
* **adin1320_sd_get_autoneg_adv** - Gets all enabled auto-negotiation advertisement serdes speeds.

Forced Speed Control
""""""""""""""""""""

* **adin1320_sd_forced_speed** - Sets the forced fiber speed

Speed and Link Status
"""""""""""""""""""""

* **adin1320_sd_resolved_speed** - Gets the resolved/negotiated fiber speed with the link partner
* **adin1320_sd_get_mii_status** - Allows reading of link status, auto-negotiation completion, link drop, jabber detection and remote fault. Can be configured which of these are to be returned as output. Reading all status bits handles concerns for status bit latches.

LED Control
"""""""""""

* **adin1320_sd_led_ctrl** - Configures LED settings for serdes PHY.

Interrupt
"""""""""

* **adin1320_sd_config_interrupt** - Configures the interrupt mask for the serdes PHY. Enables or disables specific interrupt events.
* **adin1320_sd_read_irq_status** - Read interrupt status.

Auto Media Detection
^^^^^^^^^^^^^^^^^^^^

* **adin1320_get_active_media** - Returns PHY active media (Cu or Fi)

Development & Validation
------------------------

- Software : Follows ADI No-OS framework guidelines. See this link for build guide: `<https://wiki.analog.com/resources/no-os/build>`_
- Hardware : Validated on EVAL-ADIN1320EBZ and EVAL-ADIN1320FMCZ with MAX32670 MCU.

ADIN1320 Driver Initialization Example
--------------------------------------

.. code-block:: C

	struct adin1320_desc *dev;

	struct no_os_uart_init_param uart_ip = {
		.device_id = UART_DEV_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA_IP,
	};

	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.extra = SPI_EXTRA_IP,
		.max_speed_hz = SPI_BAUDRATE,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
	};

	struct no_os_gpio_init_param gpio_adin1320_reset_ip = {
		.port = PORT_NUMBER,
		.pull = NO_OS_PULL_NONE,
		.number = ADIN1320_RESET_BUTTON,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA_IP
	};

	struct adin1320_init_param adin1320_ip = {
		.reset_param = &gpio_adin1320_reset_ip,
		.mdio_param = {
			.c45 = true,
			.addr = 0,
			.ops = MDIO_OPS, 
			.extra = &(struct mdio_spi_init_param)
			{
				.mdio = spi_ip
			}
		}
	};

	ret = adin1320_init(&dev, &adin1320_ip);
	if (ret)
		return ret;