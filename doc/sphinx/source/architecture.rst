************
Architecture
************

This page explains how no-OS is put together: the layers a no-OS application is
built from, how the hardware-abstraction API stays portable across every
supported MCU, and where each piece lives in the source tree. Read it after the
:doc:`getting_started` tutorial, when you want to understand *why* the build you
ran is structured the way it is — and before you write a driver
(:doc:`drivers_guide`) or port to a new platform.

.. contents:: Table of Contents
    :depth: 2

The big picture
===============

A no-OS application is a stack of four cooperating layers. Each layer depends
only on the one below it through a stable interface, which is what lets the same
driver run unchanged on a Maxim MCU, an STM32, an FPGA soft-core, or even on
Linux for testing.

.. code-block::

   ┌────────────────────────────────────────────────────────────┐
   │  Application  (projects/<name>/src/main.c)                 │
   │  wires a driver to a platform and runs the program         │
   ├────────────────────────────────────────────────────────────┤
   │  Frameworks   (iio/, network/, jesd204/, ...)  [optional]  │
   │  reusable subsystems built on the no_os_* API              │
   ├────────────────────────────────────────────────────────────┤
   │  Device drivers  (drivers/<class>/<part>/)                 │
   │  device logic, hardware-agnostic                           │
   ├────────────────────────────────────────────────────────────┤
   │  no_os_* HAL API  (include/no_os_*.h, drivers/api/)        │
   │  SPI / I2C / GPIO / UART / IRQ / timer / ... abstractions  │
   ├────────────────────────────────────────────────────────────┤
   │  Platform layer  (drivers/platform/<mcu>/)                 │
   │  vendor-SDK-backed implementation of the HAL API           │
   └────────────────────────────────────────────────────────────┘

The rule that makes this work: **a driver never calls a vendor SDK directly.**
It calls ``no_os_spi_write_and_read()``, not ``MXC_SPI_MasterTransaction()`` or
``HAL_SPI_Transmit()``. Only the platform layer touches vendor code. Move a
driver to a new MCU and nothing in the driver changes — only the platform
implementation underneath it does.

The layers
==========

Platform layer
--------------

Lives in ``drivers/platform/<mcu>/``. There is one directory per supported
backend:

.. code-block::

   drivers/platform/
   ├── aducm3029/   maxim/      stm32/     xilinx/
   ├── altera/      pico/       lattice/   linux/
   ├── ftd2xx/      freeRTOS/   chibios/   generic/  ...

Each platform provides the vendor-SDK-backed implementation of the HAL: the
code that actually configures a peripheral and moves bytes.

HAL API layer
-------------

The public API is declared in ``include/no_os_*.h`` — one header per peripheral
class or utility:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Header
     - Abstraction
   * - ``no_os_spi.h``
     - SPI bus master transfers
   * - ``no_os_i2c.h`` / ``no_os_i3c.h``
     - I2C / I3C transfers
   * - ``no_os_gpio.h``
     - GPIO configuration and level control
   * - ``no_os_uart.h``
     - UART / serial I/O
   * - ``no_os_irq.h``
     - Interrupt controller and handler registration
   * - ``no_os_timer.h`` / ``no_os_pwm.h`` / ``no_os_rtc.h``
     - Timers, PWM outputs, real-time clock
   * - ``no_os_dma.h`` / ``no_os_flash.h`` / ``no_os_eeprom.h``
     - DMA, on-chip flash, EEPROM
   * - ``no_os_delay.h`` / ``no_os_alloc.h`` / ``no_os_util.h``
     - Delays, allocation, helpers (bit ops, endianness, ...)

The generic, platform-independent half of the API lives in ``drivers/api/``
(``no_os_spi.c``, ``no_os_gpio.c``, ...). These files contain no vendor code;
they validate arguments, take bus locks, and forward the call to the platform
implementation (see `How dispatch works`_ below).

Device drivers
--------------

Lives in ``drivers/<class>/<part>/``, grouped by device class — ``adc``,
``dac``, ``accel``, ``imu``, ``temperature``, ``power``, ``rf-transceiver``,
and so on. A driver contains the logic for one ADI part (register maps,
conversion sequences, calibration) and reaches hardware exclusively through the
``no_os_*`` API. See :doc:`drivers_guide` for the structures and functions a
driver must define.

Frameworks (optional)
--------------------

Higher-level subsystems built on top of the HAL that many projects reuse:

* ``iio/`` — the Industrial I/O server (IIOD), device/attribute/buffer model,
  and triggers. This is what the :doc:`getting_started` IIO Demo exposes.
* ``network/`` — TCP/IP and network transports.
* ``jesd204/`` — the JESD204 link-layer state machine for high-speed
  converters.

A project uses these only if it needs them; a minimal driver-plus-``main.c``
application does not.

Application (projects)
--------------------

Lives in ``projects/<name>/``. A project is the top of the stack: it selects a
driver, a platform, and any frameworks, supplies the initialization data that
binds them together, and provides ``main()``. Its layout follows a consistent
pattern:

.. code-block::

   projects/<name>/
   ├── src/
   │   ├── common/     application code + init data (platform-independent)
   │   └── platform/   per-platform macros and pin/peripheral configuration
   ├── *.conf          defconfig files — one per build variant
   ├── Kconfig         project configuration options
   └── CMakeLists.txt  build entry point

The ``src/common`` vs ``src/platform`` split mirrors the whole architecture in
miniature: portable logic on one side, per-target configuration on the other.

.. _how-dispatch-works:

How dispatch works: the ``platform_ops`` pattern
================================================

Portability hinges on one mechanism repeated across every peripheral class: a
**function-pointer table** (``*_platform_ops``) that a project selects at
configuration time and the HAL calls through at run time. It is C's version of
a virtual interface. SPI is the canonical example.

1. Each platform defines an ops table pointing at its own functions
------------------------------------------------------------------

.. code-block:: c

   /* drivers/platform/maxim/max32660/maxim_spi.c */
   const struct no_os_spi_platform_ops max_spi_ops = {
           .init           = &max_spi_init,
           .write_and_read = &max_spi_write_and_read,
           .transfer       = &max_spi_transfer,
           .transfer_dma   = &max_spi_transfer_dma,
           .remove         = &max_spi_remove,
   };

The STM32 platform defines ``stm32_spi_ops``, ADuCM defines its own, and so on
— each with the same members pointing at that platform's implementation.

2. The project points its init_param at the platform's ops
---------------------------------------------------------

The application fills in a ``no_os_spi_init_param`` and sets its
``platform_ops`` field to the table for the target it is building for. This is
the single line that binds a portable driver to a concrete MCU:

.. code-block:: c

   struct no_os_spi_init_param spi_ip = {
           .device_id     = 1,
           .max_speed_hz  = 1000000,
           .platform_ops  = &max_spi_ops,   /* the target-specific table */
           /* ... */
   };

3. The driver calls the generic API — never the platform directly
----------------------------------------------------------------

.. code-block:: c

   /* inside a device driver — identical on every platform */
   ret = no_os_spi_write_and_read(dev->spi_desc, buf, len);

4. The generic API forwards through the ops table
------------------------------------------------

``no_os_spi_init()`` copies ``platform_ops`` from the init_param into the
descriptor, and every subsequent call forwards through that pointer:

.. code-block:: c

   /* drivers/api/no_os_spi.c (simplified) */
   int32_t no_os_spi_init(struct no_os_spi_desc **desc,
                          const struct no_os_spi_init_param *param)
   {
           if (!param || !param->platform_ops)
                   return -EINVAL;
           if (!param->platform_ops->init)
                   return -ENOSYS;

           ret = param->platform_ops->init(desc, param);   /* -> max_spi_init */
           if (ret)
                   return ret;

           (*desc)->platform_ops = param->platform_ops;     /* remember it */
           return 0;
   }

   int32_t no_os_spi_write_and_read(struct no_os_spi_desc *desc, ...)
   {
           if (!desc->platform_ops->write_and_read)
                   return -ENOSYS;

           no_os_mutex_lock(desc->bus->mutex);
           ret = desc->platform_ops->write_and_read(desc, data, bytes_number);
           no_os_mutex_unlock(desc->bus->mutex);
           return ret;
   }

The payoff: the driver's ``no_os_spi_write_and_read()`` call lands in
``max_spi_write_and_read()`` on a Maxim build and in ``stm32_spi_write_and_read()``
on an STM32 build — with no change to the driver. Swapping platforms means
swapping one ``platform_ops`` pointer at configuration time. The same pattern
governs ``no_os_gpio_platform_ops``, ``no_os_uart_platform_ops``,
``no_os_i2c_platform_ops``, and the rest.

The init_param / descriptor idiom
=================================

Every abstraction — and every driver — follows the same two-structure
convention:

* **init_param** (``*_init_param``) — an input, filled in by the caller,
  describing *what to create*: bus IDs, speeds, pin numbers, device settings,
  and the ``platform_ops`` pointer.
* **descriptor** (``*_desc``) — an opaque output, allocated by the ``_init()``
  function, representing *a live instance*. The caller passes it to every
  subsequent call and finally to ``_remove()``.

.. code-block:: c

   struct no_os_spi_desc *spi;
   ret = no_os_spi_init(&spi, &spi_ip);   /* allocates + configures */
   /* ... use spi ... */
   no_os_spi_remove(spi);                 /* frees everything it owns */

The same shape scales up: a device driver's init_param embeds the bus and GPIO
init params it needs — for example ``adxl355_init_param`` carries a
``no_os_spi_init_param`` (inside a ``comm_init`` union that also allows I2C) —
and its ``adxl355_dev`` descriptor holds the resulting communication descriptor
returned by the lower layer. Initialization composes downward; ``_remove()``
tears down in reverse. Because init allocates on the heap and remove frees it, mind how often
you call them in a long-running application (see the notes on heap fragmentation
in :doc:`drivers_guide`).

Configuration: Kconfig and defconfigs
=====================================

Which drivers, platforms, and framework features a build includes is chosen
through a **Kconfig** system, the same mechanism the Linux kernel uses. Options
are declared in ``Kconfig`` files (at the repo root, under ``drivers/``, and in
each project) and selected values are captured in per-variant **defconfig**
files — the ``.conf`` files you saw in the project directory during the
:doc:`getting_started` tutorial. Each build *variant* is essentially a named
defconfig, which is why ``iio``, ``iio_usb_uart``, and ``iio_wifi`` are separate
variants of the same project: they enable different options.

Source tree map
===============

.. code-block::

   no-OS/
   ├── include/            no_os_*.h — the public HAL API
   ├── drivers/
   │   ├── api/            generic, platform-independent HAL implementation
   │   ├── platform/       per-MCU HAL implementations (the only vendor code)
   │   └── <class>/<part>/ device drivers, grouped by device class
   ├── iio/                IIO framework (IIOD server, buffers, triggers)
   ├── network/            networking stack and transports
   ├── jesd204/            JESD204 link layer
   ├── projects/           example applications (project + variants + boards)
   ├── util/               shared utilities (buffers, CRC, lists, ...)
   ├── tools/scripts/      build utility (no_os_build.py) and helpers
   └── doc/                this documentation

Where to go next
================

* :doc:`getting_started` — build and run an application if you have not yet.
* :doc:`drivers_guide` — the structures, functions, and conventions for writing
  a driver on top of the HAL.
* :doc:`build_guide` — per-platform toolchain setup and build mechanics.
* `No-OS Doxygen Documentation <https://analogdevicesinc.github.io/no-OS/doxygen>`_
  — the exact signature and fields of every ``no_os_*`` function and structure.
