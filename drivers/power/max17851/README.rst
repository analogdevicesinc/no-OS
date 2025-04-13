MAX17851 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX17851 <https://www.analog.com/MAX17851>`_

Overview
--------

The MAX17851 SPI to UART safety monitoring bridge translates communication from
SPI format to universal asynchronous receiver transmitter (UART) format
specially designed to interface with Maxim's Battery Management Data
Acquisition System.

The safety monitoring bridge enables robust communication with baud rates
extending up to 4Mbps in both single and dual daisy-chain system architectures.
High throughput, dual UART systems can read 96 cells of battery voltages within
1173us.

THIS Uart Device is meant to be used alongside devices using Maxim's Battery
Management System UART Protocol!

Applications
------------
* Battery Management Systems (BMS)
* Electric and Hybrid Electric Vehicles (EV/HEV)
* Energy Storage Systems (ESS)

Maxim Battery Management System UART Protocol
---------------------------------------------
A message is defined as a sequence of UART characters.
The message starts with a preamble character, followed by
data characters, and ending with a stop character.

Each character consists of the following 12 bits:
* One START bit
* Eight data bits (LSB first)
* One parity bit (even)
* Two STOP bits

More than this, every data byte is transmitted and received as two separate
characters (separated from one byte to two bytes), since every nibble is
Manchester Encoded. Therefore each  nibble becomes a byte (the bit number
doubles), so a data byte splits into two characters.

Driver Initialization
---------------------

In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI).

The first API to be called is **max17851_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Usually, this function is to be called from **no_os_uart_init** function, since
max17851 can also be seen as a UART peripheral inside no-OS infrastructure.

For more details regarding UART API, please see
`No-OS UART API <https://wiki.analog.com/resources/no-os/drivers/uart>`_.

General Configuration
---------------------
Baud Rate, Single/Dual UART and Operation Mode are properties that should be set
at the initialization of the device, therefore they are found in the
**max17851_init** function, although other configurations can be found there
such as the :
* Alert Debounce (**max17851_set_alert_debounce**).
* GPIO Recovery Delay (**max17851_reg_write** to CONFIG_SAFEMON0 Register).
* Contact Timer Delay (**max17851_reg_write** to CONFIG_SAFEMON1 Register).
* Communication Timeout Retry Count (**max17851_set_comm**).
* Safemon Delay (**max17851_set_safemon_dly**).

But more than this, the MAX17851 no-OS driver also provides access to UART
related general configuration, that can be performed at run-time, such as :
* MAX17851_TX_NOPREAMBLE_SEL - Transmits Message with no Preamble Byte.
* MAX17851_TX_NOSTOP_SEL - Disables Stop Character from UART TX packages.
* MAX17851_TX_PAUSE_SEL - Puts TX in idle state.
* MAX17851_TX_ODDPARITY_SEL - Selects ODD parity for UART bits instead of EVEN.
* MAX17851_TX_QUEUE_SEL - Enables transmission of the message from the queue.
* MAX17851_TX_PREAMBLES_SEL - Transmits preambles continuously.
* MAX17851_TX_RAW_DATA_SEL - Disables Manchester encoding from transmitted data.
* MAX17851_RX_RAW_DATA_SEL - Disables Manchester encoding from received data.

Alert Packet Configuration
--------------------------

MAX17851 also comes with the Alert Packet feature, therefore ALERT pins can
be initialized from the Slave BMS chip used alongside MAX17851 and receive
an alert packet. MAX17851 gives the chance to monitor the specific alerts
or enable/disable them, so an interrupt inside the MCU host unit can be
performed in case of failures.

**max17851_set_alrtpckt_time** Sets the time for generating Alert Packets or
sending keep-alive (stop) characters throughout UART.

**max17851_set_alert** and **max17851_get_alert** enable/disable specific
alerts, respectively checks if a specific alert is triggered.

Read/Writing Messages Configuration
-----------------------------------

In order to send/receive UART messages using the MAX17851, a specific routine
has to be performed for each operation.

Transmitting Messages:
----------------------

For transmitting messages there are 4 queues, each one of them consisting of 32
bytes, first byte being the length of the message, rest of them being the
data bytes need to be sent over UART (can or cannot be encoded, default
settings require them not to be encoded, since the IC is taking care of
the Mancheste Encoding).

Therefore the user can select wich queue is to be loaded (written to inside
the IC memory) and which one to be sent over UART using the
**max17851_select_queue**, each one being able to be seleted as the
transmitt/load queue in case of manual message sending.

**max17851_write_msg** handles the whole transmitting of the message, using the
first queue, therefore in this case the message can not be grater than 31 bytes.

Receving Messages:
------------------
**max17851_read_msg** is the API to be used in order to read the received
buffer over UART.

For both transmitting and receiving messages, the read/write message API's
handle the buffer clearance after the buffers are properly written/read.

Register Access
---------------

In case the user wants to use even more specific functionalities of the
MAX17851 such as using the Continuously Preambles mode, or no STOP
characters mode, etc. in order for a Hardware-In-The-Loop test or such,
register access is provided as well as the whole register map being defined
in the header file.

**max17851_reg_write** - Is to be used for register writing.
**max17851_reg_read** - Is to be used for register reading.

No-OS platform operations
-------------------------

Since the MAX17851 is to be used as a UART peripheral in order to communicate
with other BMS chips, it can be used as such inside the No-OS infrastructure
and having specific platform ops, therefore a no-OS uart descriptor can be
used and parsed for initialization in other BMS device drivers.

**max17851_uart_init** - Initializes the UART descriptor.
**max17851_uart_read** - Reads data over the UART.
**max17851_uart_write** - Writes data over the UART.
**max17851_uart_get_errors** - Gets errors from the Alert Loop.
**max17851_uart_remove** - Removes the UART descriptor.

MAX17851 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct max17851_desc *max17851_desc;
	struct no_os_spi_init_param max17851_spi_ip = {
		.device_id = 4,
		.extra = &max17851_spi_extra,
		.mode = NO_OS_SPI_MODE_0,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct max17851_init_param max17851_ip = {
		.spi_param = &max17851_spi_ip,
		.gpio1_param = NULL,
		.gpio2_param = NULL,
		.op_mode = MAX17851_MASTER_DUAL_UART,
		.baud_rate = MAX17851_UART_BAUD_500K,
		.no_dev = 1,
		.single = false,
		.contact_tmr_dly_4xmin = MAX17851_CONTACT_TIMER_DELAY_INFINITE,
		.gpio_rec_dly_csec = MAX17851_GPIO_RECOVERY_DELAY_DISABLED,
		.safemon_dly = MAX17851_SAFEMON_DLY_500MS,
	};

	ret = max17851_init(&max17851_desc, &max17851_ip);
	if (ret)
		return ret;

MAX17851 UART Descriptor Initialization Example
-----------------------------------------------

.. code-block:: bash

	struct no_os_uart_desc *uart_desc;
	struct max_spi_init_param max17851_spi_extra = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};
	struct no_os_spi_init_param max17851_spi_ip = {
		.device_id = 4,
		.extra = &max17851_spi_extra,
		.mode = NO_OS_SPI_MODE_0,
		.max_speed_hz = 100000,
		.platform_ops = &max_spi_ops,
		.chip_select = 0,
	};
	struct max17851_init_param uart_extra = {
		.spi_param = &max17851_spi_ip,
		.gpio1_param = NULL,
		.gpio2_param = NULL,
		.op_mode = MAX17851_MASTER_DUAL_UART,
		.baud_rate = MAX17851_UART_BAUD_500K,
		.no_dev = 1,
		.single = false,
		.contact_tmr_dly_4xmin = MAX17851_CONTACT_TIMER_DELAY_INFINITE,
		.gpio_rec_dly_csec = MAX17851_GPIO_RECOVERY_DELAY_DISABLED,
		.safemon_dly = MAX17851_SAFEMON_DLY_500MS,
	};
	struct no_os_uart_init_param uart_ip = {
		.device_id = 0,
		.irq_id = NULL,
		.baud_rate = 500000,
		.size = NO_OS_UART_CS_8,
		.platform_ops = &max17851_uart_ops,
		.parity = NO_OS_UART_PAR_EVEN,
		.stop = NO_OS_UART_STOP_2_BIT,
		.extra = &uart_extra,
	};
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;
