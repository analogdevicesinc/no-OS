LTC4296 no-OS driver
====================

Supported Devices
-----------------

`LTC4296-1 <www.analog.com/en/products/ltc4296-1.html>`_

Overview
--------

The `LTC4296-1 <www.analog.com/en/products/ltc4296-1.html>`_ is an IEEE
802.3cg-compliant, five port, single-pair power over Ethernet (SPoE), power
sourcing equipment (PSE) controller. SPoE simplifies system design and
installation with standardized power and Ethernet data over a single-pair cable.
The LTC4296-1 is designed for interoperability with 802.3cg powered devices
(PDs) in 24 V or 54 V systems. The LTC4296-1 delivers power using external, low
drain-to-source on resistance (RDS(ON)), N-channel metal-oxide semiconductor
field-effect transistors (MOS-FETs), which minimize voltage drop and ensure
application ruggedness.
High-side circuit breakers with foldback, analog current limit (ACL) provide
controlled inrush and short-circuit protection. An optional low-side circuit
breaker protects the negative output against backfeed faults, and ground faults
in nonisolated applications. PD classification via the serial communication
classification protocol (SCCP)and maintain full voltage signature (MFVS) ensure
full operating voltage is only applied to the cable when a PD is present. The
SWx pins disconnect port power snubbers during detection and classification.
PD initiated sleep and wake-up are supported. The WAKEUP pin supports wake-up
forwarding. Telemetry, status, and software control features are accessed via a
serial peripheral interface (SPI) bus interface with packet error code (PEC)
protection.

Applications
------------

* Operational technology (OT) systems
* Building and factory automation systems
* Field instruments and switches
* Security systems
* Traffic control systems

LTC4296 Device Configuration
----------------------------

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **ltc4296_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function will also
reset the LTC4296 by performing a soft reset, putting the device in a known
state. 


LTC4296 Driver Initialization Example
-------------------------------------

SPI Communication Example
-------------------------

.. code-block:: bash

	struct ltc4296_dev *dev;
	int ret;

	struct no_os_uart_init_param ltc4296_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct no_os_spi_init_param ltc4296_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = SPI_BAUDRATE,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.chip_select = SPI_CS,
		.extra = SPI_EXTRA,
	};

	struct ltc4296_init_param ltc4296_ip = {
		.spi_init = &ltc4296_spi_ip,
	};

	ret = ltc4296_init(&ltc4296_desc, &ltc4296_ip);
	if (ret) {
		pr_info("Initialization error!\n");
		return ret;
	}

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT0, LTC_CFG_APL_MODE);
	if (ret) {
		goto err;
	}

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT0);
	if (ret)
		goto err;
