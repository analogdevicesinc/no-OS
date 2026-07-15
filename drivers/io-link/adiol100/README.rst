ADIOL100 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADIOL100`
- :adi:`ADIOL101`

Overview
--------

The ADIOL100/ADIOL101 are dual-channel IO-Link Master transceivers.
Each channel integrates an L+ sensor supply controller, a C/Q line
driver and receiver, and a digital IO-Link framer that handles
EstablishCommunication, M-sequence framing, and cyclic process data exchange
autonomously.

The device communicates with the host MCU over SPI at up to 26 MHz (CPOL=0,
CPHA=0). All registers are 16 bits wide, except the TxRxData FIFO registers.
IO-Link message data is exchanged through a per-channel FIFO accessed via SPI
burst transfers. The device address is selected by the ADRSEL pin at power-up (4
possible addresses), allowing up to 4 chips on one SPI bus.

The ADIOL101 is the same die in a smaller 40-pin package, without the
external high-side FET controller pins. The register map and driver are
identical for both variants.

Key specifications:

* V24 supply: 9 V to 36 V (typ 24 V)
* VL logic supply: 1.62 V to 5.5 V
* CQ current limit: 10 mA to 500 mA (programmable)
* Operating temperature: -40 C to +125 C
* Thermal warning at 135 C, thermal shutdown at 150 C

**Important note:**

In order for IO-Link communication to take place, the master needs to go
through several protocol phases:

.. code-block:: text

                            ┌────────────┐
                            │STANDARD I/O│
                            └─────┬──────┘
                                  │
                              ┌───▼───┐            ┌──────────────┐
                              │STARTUP├───────────►│ Transceiver  │
                              └───┬───┘            │Configuration │
                                  │                │- CQ, LP      │
                                  │                │- Protections │
                                  │                │- Interrupts  │
                                  │                │- ...         │
                                  │                └──────┬───────┘
                                  │                       │
                                  │              ┌────────▼─────────────┐
                                  │              │EstablishCommunication│
                                  │              │  (probing the slave  │
                                  │              │   at every ComRate)  │
                                  │              └──────────────────────┘
                                  │                       │
   ┌─────────────────┐      ┌─────▼────┐      ┌───────────▼───────────────┐
   │   Master sets   │◄─────┤PREOPERATE│      │          Slave            │
   │ MasterCycleTime │      └─────┬────┘      │      Identification       │
   └────────┬────────┘            │           │ (reading slave parameters │
            │                     │           │required for communication │
   ┌────────▼────────┐            │           │   and identification)     │
   │   (Optional)    │            │           └────────────┬──────────────┘
   │Master reads ISDU│            │                        │
   │     pages       │            │             ┌──────────▼───────────┐
   └─────────────────┘            │             │       Sending        │
                                  │             │ MasterIdent command  │
                                  │             └──────────────────────┘
                                  │
                                  │
                              ┌───▼───┐
                              │OPERATE│
                              └───────┘
                       (live data every cycle)

This driver provides hardware control and configuration up through the
EstablishCommunication step. It provides the building blocks for transceiver
configuration, FIFO messaging, and cycle timer control. The higher-level IO-Link
protocol state machine (STARTUP to PREOPERATE to OPERATE transitions, M-sequence
framing, ISDU handling, process data management) must be implemented by the
application or by integrating an external IO-Link stack. Examples for both cases
can be found in the `adiol100 project <projects/adiol100>`_.

Applications
------------

* IO-Link master ports
* Factory automation sensor hubs
* Industrial fieldbus gateways
* Process instrumentation

ADIOL100 Device Configuration
------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI), the device address set by the ADRSEL
pin, the clock source (crystal, external, or internal), and the clock
divider.

The first API to be called is **adiol100_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. The init sequence
reads the RevisionID and DeviceID to verify the chip is present, configures
the clock source and divider, performs a global reset (disabling interrupts
and clearing latched flags), and resets both channels.

Register Access
~~~~~~~~~~~~~~~

Use **adiol100_read** and **adiol100_write** to access any 16-bit register.
Both functions handle the paged register addressing automatically: registers
in the ``0x1Fxx`` range are accessed through the ExtraPage gateway register
transparently to the caller.

Use **adiol100_update** for read-modify-write operations on individual bit
fields within a register.

CQ Driver Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

Use **adiol100_config_cq** to set the CQ output mode (push-pull or NPN),
enable or disable the driver, select the input sink current, and set the
output slew rate. This modifies only the specified fields in the CQSet
register, preserving other settings.

Use **adiol100_config_cq_protection** to configure the CQ current limit,
blanking time, TX/RX voltage thresholds, and autoretry behavior. This writes
the full CQCurLim register.

L+ Supply Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

Use **adiol100_config_lp** to enable the L+ sensor supply and the reverse
polarity protection FET. This modifies only the LPEN and LPFETREVEN fields
in the LPCfg register.

Use **adiol100_config_lp_protection** to configure the L+ FET protection
parameters across six registers: nominal current limit (LPCfg), power limit
(LPFetPwrCfg), current min/max (LPCurrentCfg), open-load and current-limit
timeouts (LPFETProtect), slope control (LPSlope), and autoretry settings
(LPRetry).

Framer Configuration
~~~~~~~~~~~~~~~~~~~~

Use **adiol100_config_framer** to enable the IO-Link framer and checksum
insertion. Both must be enabled before starting IO-Link communication. This
modifies only the INSCHKS and FRAMEREN fields in the FramCtrl1 register.

IO-Link Communication
~~~~~~~~~~~~~~~~~~~~~

Use **adiol100_estcom** to run EstablishCommunication. The function clears
CQSend, disables the cycle timer, resets both FIFOs, then sets the EstCom
bit. It polls the channel interrupt register at 1 ms intervals for up to
100 ms. Returns 0 on success, ``-ENODEV`` if no device responded,
``-ETIMEDOUT`` if the chip did not finish in time.

Use **adiol100_get_comrt** to read the detected baud rate after a successful
EstCom (1 = COM1, 2 = COM2, 3 = COM3).

Use **adiol100_send_msg** to load IO-Link payload bytes into the TxFIFO and
trigger transmission. The caller provides only the IO-Link payload (MC + CKT
+ OD bytes). The driver prepends the FIFO header (MsgID, RxBytes, TxBytes),
sets the SPI burst length, and asserts CQSend internally.

Use **adiol100_read_msg** to read the device response from the RxFIFO. The
driver strips the MsgID and RxBytesAct header and returns only the IO-Link
response bytes. A sanity check is performed: if RxBytesAct does not match the
FIFO level, the FIFO is reset and ``-EIO`` is returned.

Cycle Timer
~~~~~~~~~~~

Use **adiol100_set_cycle_tmr** to write the IO-Link encoded cycle time byte
(time base in bits [7:6], multiplier in bits [5:0]).

Use **adiol100_enable_cycle_timer** and **adiol100_disable_cycle_timer** to
start and stop autonomous cyclic operation. When enabled, the chip
automatically re-transmits the TxFIFO contents at the configured cycle
interval. For cyclic process data exchange, call **adiol100_send_msg** with
``ADIOL100_KEEP_MSG`` to retain the master message in the TxFIFO for
re-transmission.

Interrupts
~~~~~~~~~~

Use **adiol100_get_channel_irq** and **adiol100_get_global_irq** to read
and clear the interrupt status registers. Channel interrupts include
EstCom success/fail, RX data ready, L+ errors, CQ driver errors, and more.
Global interrupts include thermal warning/shutdown, V24 errors, watchdog,
and SPI CRC errors.

Use **adiol100_enable_channel_irq** and **adiol100_enable_global_irq** to
set the interrupt enable masks. Note that interrupt flags latch regardless
of the enable mask - the mask only controls whether the IRQ pin is asserted.

Watchdog
~~~~~~~~

Use **adiol100_config_watchdog** to configure the watchdog timer: time base
(100 us, 500 us, or 2 ms), timeout multiplier (6-bit, 0 to 63), mode (SPI
activity or explicit clear), enable, and lock. Once locked, the watchdog
configuration cannot be changed until the next power cycle.

Use **adiol100_clear_watchdog** to clear the watchdog in explicit-clear mode.

Reset
~~~~~

Use **adiol100_global_reset** to disable all global interrupts, clear the
trigger register, and read-clear all interrupt and status registers for both
channels. This does not reset register values to defaults.

Use **adiol100_reset_channel** to assert CHANRST, which resets all
channel-specific registers to their default values.

Use **adiol100_reset_tx_fifo** and **adiol100_reset_rx_fifo** to clear the
transmit or receive FIFO for a channel without resetting other channel state.

ADIOL100 Driver Initialization Example
---------------------------------------

.. code-block:: c

	#include <stdio.h>
	#include "no_os_spi.h"
	#include "no_os_delay.h"
	#include "no_os_print_log.h"
	#include "adiol100.h"

	struct adiol100_dev *dev;

	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 10000000,
		.mode = NO_OS_SPI_MODE_0,
		.chip_select = SPI_CS,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};

	struct adiol100_init_param ip = {
		.spi_ip = &spi_ip,
		.chip_addr = 0,
		.clock_src = ADIOL100_CLK_CRYSTAL,
		.clk_div = 0,
	};

	int main(void)
	{
		int ret;
		uint8_t comrt;
		uint16_t status;

		/* Initialize the ADIOL100 */
		ret = adiol100_init(&dev, &ip);
		if (ret) {
			pr_info("ADIOL100 init failed: %d\n", ret);
			return ret;
		}

		/* Enable L+ sensor supply on channel A */
		ret = adiol100_config_lp(dev, ADIOL100_CH_A,
					 ADIOL100_LP_EN,
					 ADIOL100_LP_REV_EN);
		if (ret)
			goto cleanup;

		/* Wait for L+ to stabilize */
		no_os_mdelay(3000);

		/* Configure CQ driver: push-pull, 5mA sink, 250ns slew */
		ret = adiol100_config_cq(dev, ADIOL100_CH_A,
					 ADIOL100_CQ_PUSHPULL,
					 ADIOL100_CQ_DRV_EN,
					 ADIOL100_SINKSEL_5MA,
					 ADIOL100_CQSLEW_250NS);
		if (ret)
			goto cleanup;

		/* Enable framer with checksum insertion */
		ret = adiol100_config_framer(dev, ADIOL100_CH_A,
					     ADIOL100_CHKS_EN,
					     ADIOL100_FRAMER_EN);
		if (ret)
			goto cleanup;

		/* Establish communication with the IO-Link device */
		ret = adiol100_estcom(dev, ADIOL100_CH_A);
		if (ret) {
			pr_info("EstCom failed: %d\n", ret);
			goto cleanup;
		}

		/* Read the detected baud rate */
		adiol100_get_comrt(dev, ADIOL100_CH_A, &comrt);
		pr_info("Device responded at COM%d\n", comrt);

		/* Read channel status */
		adiol100_get_status(dev, ADIOL100_CH_A, &status);
		pr_info("Channel A status: 0x%04X\n", status);

		pr_info("ADIOL100 initialized successfully!\n");

		/*
		 * Application code goes here:
		 * - Read DPP parameters using send_msg/read_msg
		 * - Transition through STARTUP -> PREOPERATE -> OPERATE
		 * - Exchange cyclic process data
		 * These steps require IO-Link protocol handling above
		 * the driver level.
		 */

	cleanup:
		adiol100_remove(dev);
		return ret;
	}
