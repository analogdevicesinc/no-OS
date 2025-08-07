AD9553 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9553 <https://www.analog.com/AD9553>`_

Overview
--------

The AD9553 is a PLL-based clock translator designed for passive optical
networks (PON) and base stations. It employs an integer-N PLL for
specific frequency translation, allowing up to two single-ended or one
differential input via the REFA and REFB inputs. The device supports
holdover applications with a 25 MHz crystal attached to the XTAL input.
It is pin programmable, offering frequency translations from 15 input
frequencies to 52 output frequency pairs (OUT1 and OUT2). The AD9553
includes a 3-wire SPI for custom frequency translations and supports
LVPECL, LVDS, or CMOS logic levels. It operates within an industrial
temperature range of -40 degrees C to +85 degrees C.

Applications
------------

* Cost effective replacement of high frequency VCXO, OCXO, and SAW resonators
* Flexible frequency translation for SONET/SDH, Ethernet, Fiber Channel,
  DRFI/DOCSIS, and PON/EPON/GPON
* Wireless infrastructure
* Test and measurement

AD9553 Device Configuration
---------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (SPI).

The first API to be called is **ad9553_setup**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function resets
the device, configures the PLL charge pump control, activates the lock
detector, and sets the division values for frequency translation through
SPI transactions.

SPI Communication
~~~~~~~~~~~~~~~~~

**ad9553_reg_read** and **ad9553_reg_write** manage SPI protocol
transactions for reading and writing to the AD9553 registers.
**ad9553_reg_read** retrieves data from specific registers, while
**ad9553_reg_write** configures registers by sending data. The SPI
protocol uses a 3-byte transaction format where the MSB of byte 0
indicates a read/write operation.

Resource Management
~~~~~~~~~~~~~~~~~~~

**ad9553_remove** handles resource deallocation by removing SPI
descriptors used by the AD9553 driver, ensuring efficient use of
system resources and preventing memory leaks.

AD9553 Driver Initialization Example
------------------------------------

.. code-block:: c

	struct ad9553_dev *ad9553_device;
	struct no_os_spi_init_param ad9553_spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 2000000u,
		.chip_select = SPI_CS,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = SPI_OPS,
		.extra = SPI_EXTRA,
	};
	struct ad9553_init_param ad9553_ip = {
		.spi_init = ad9553_spi_ip,
	};

	ret = ad9553_setup(&ad9553_device, &ad9553_ip);
	if (ret)
		goto error;
