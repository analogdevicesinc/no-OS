MDIO SPI Driver
===============

.. no-os-doxygen::

Overview
--------

The MDIO SPI driver implements the MDIO (Management Data Input/Output)
interface over SPI, allowing communication with Ethernet PHY devices through
standard MDIO frames transmitted on an SPI bus. This provides an alternative
to dedicated MDIO hardware peripherals.

Both IEEE 802.3 Clause 22 and Clause 45 MDIO protocols are supported.
Protocol selection is handled automatically based on the device configuration
and register address.

The driver plugs into the No-OS MDIO abstraction layer (``no_os_mdio.h``)
via the ``mdio_spi_ops`` operations structure, so all user interaction goes
through the ``no_os_mdio_*`` public API.

Applications
------------

- Ethernet PHY management via SPI
- Systems without dedicated MDIO hardware interfaces
- Multi-PHY configurations using SPI multiplexing
- Isolated MDIO communication using SPI isolators

MDIO Frame Structure
--------------------

Each SPI transaction transfers 8 bytes (64 bits):

* **Bytes 0-3** — 32-bit preamble (all ones)
* **Bytes 4-7** — 32-bit MDIO frame:

  - Start bits (2 bits)
  - Operation code (2 bits): Address, Write, or Read
  - PHY address (5 bits)
  - Register / device address (5 bits)
  - Turnaround bits (2 bits)
  - Data field (16 bits)

Clause 22 and Clause 45
-----------------------

* **Clause 22** — Direct register access for registers 0–31.
* **Clause 45** — Extended addressing for registers ≥ 32. The driver
  automatically performs a two-phase transaction (address phase followed by
  read/write phase).

Clause 45 is used only when both ``c45 = true`` in the init parameters and the
target register address is ≥ ``NO_OS_MDIO_C22_REGS`` (32).

Frame Validation
----------------

On every transaction the driver validates the response frame by checking:

* Turnaround bits match the expected value
* PHY address in the response matches the requested address

A mismatch returns ``-EBADMSG``.

SPI Configuration
-----------------

The SPI interface should be configured with:

* MSB-first bit order
* SPI Mode 0 (CPOL = 0, CPHA = 0)
* Full-duplex operation (``no_os_spi_write_and_read``)
* Clock frequency appropriate for the target PHY

API
---

The driver is accessed exclusively through the No-OS MDIO abstraction layer.
Pass ``&mdio_spi_ops`` as the ``ops`` field in
``struct no_os_mdio_init_param``.

* **no_os_mdio_init** — Initialise the MDIO-over-SPI interface. Internally
  calls ``mdio_spi_init``, which allocates the MDIO descriptor and initialises
  the underlying SPI peripheral.
* **no_os_mdio_write** — Write a 16-bit value to a PHY register.
* **no_os_mdio_read** — Read a 16-bit value from a PHY register.
* **no_os_mdio_remove** — Release all resources (SPI descriptor, MDIO
  descriptor).

Error Codes
-----------

* ``-EINVAL`` — Invalid argument (NULL pointer or invalid operation)
* ``-ENOMEM`` — Memory allocation failure
* ``-EBADMSG`` — Invalid MDIO frame (turnaround or PHY address mismatch)
* Any error code propagated from the SPI subsystem

Driver Initialization Example
-----------------------------

.. code-block:: C

    #include "no_os_spi.h"
    #include "no_os_mdio.h"
    #include "mdio_spi.h"

    int main(void)
    {
        struct no_os_mdio_desc *mdio_desc;
        int ret;
        uint16_t reg_value;

        struct no_os_spi_init_param spi_ip = {
            .device_id = SPI_DEVICE_ID,
            .max_speed_hz = SPI_BAUDRATE,
            .chip_select = SPI_CS,
            .mode = NO_OS_SPI_MODE_0,
            .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
            .platform_ops = SPI_OPS,
            .extra = SPI_EXTRA_IP,
        };

        struct no_os_mdio_init_param mdio_ip = {
            .addr = 0,
            .c45 = true,
            .ops = &mdio_spi_ops,
            .extra = &(struct mdio_spi_init_param) {
                .mdio = spi_ip
            }
        };

        ret = no_os_mdio_init(&mdio_desc, &mdio_ip);
        if (ret)
            return ret;

        /* Read PHY Control Register (register 0) */
        ret = no_os_mdio_read(mdio_desc, 0, &reg_value);
        if (ret)
            goto cleanup;

        /* Write PHY Control Register */
        ret = no_os_mdio_write(mdio_desc, 0, reg_value | 0x8000);
        if (ret)
            goto cleanup;

    cleanup:
        no_os_mdio_remove(mdio_desc);
        return ret;
    }
