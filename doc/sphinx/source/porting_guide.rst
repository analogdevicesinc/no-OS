*************
Porting Guide
*************

no-OS runs on a new MCU by adding a **platform** — an implementation of the
``no_os_*`` hardware-abstraction API backed by that MCU's vendor SDK. Because
drivers depend only on the abstraction layer (see :doc:`architecture`), once a
platform exists every existing driver and project can target the new MCU without
change.

This guide is for contributors bringing up a new platform (or a new chip within
an existing platform family). It assumes you have read :doc:`architecture` (the
``platform_ops`` dispatch model) and :doc:`configuration_guide` (Kconfig and
defconfigs), and that you can already build an existing project per
:doc:`getting_started`.

.. contents:: Table of Contents
    :depth: 2

What "porting" means here
=========================

There are two distinct tasks people call "porting", at different scales:

* **Adding a platform** — bringing up a new MCU family (or a new chip in an
  existing family) by implementing the HAL peripheral by peripheral. This is the
  bulk of this guide.
* **Adding a board** — targeting a new evaluation board on an *already
  supported* platform. This is just configuration; see `Bringing up a new
  board`_ at the end.

The contract a platform implements
==================================

A platform provides, for each peripheral class it supports, three things:

1. an **ops table** — a ``const struct no_os_<periph>_platform_ops`` whose
   members point at the platform's functions;
2. the **functions** those members point at, each matching the signature the
   abstraction expects;
3. a **header** that exports the ops table (and any platform-specific extra
   parameters) so projects can reference it.

The generic ``no_os_<periph>_*`` API in ``drivers/api/`` calls through the ops
table; your job is to fill the table with working implementations. You do **not**
modify ``drivers/api/`` or ``include/no_os_*.h`` — those are the stable
interface you are implementing against.

Anatomy of an existing platform
===============================

Study a working platform before writing one. Each lives under
``drivers/platform/<mcu>/`` with one file pair per peripheral. For Maxim
(``drivers/platform/maxim/max32660/``):

.. code-block::

   drivers/platform/maxim/max32660/
   ├── maxim_spi.c   / maxim_spi.h      SPI  -> max_spi_ops
   ├── maxim_i2c.c   / maxim_i2c.h      I2C  -> max_i2c_ops
   ├── maxim_gpio.c  / maxim_gpio.h     GPIO -> max_gpio_ops
   ├── maxim_uart.c  / maxim_uart.h     UART -> max_uart_ops
   ├── maxim_irq.c   / maxim_irq.h      IRQ  -> max_irq_ops
   ├── maxim_timer.c / maxim_timer.h    timer, pwm, rtc, ...
   ├── maxim_delay.c                    no_os_mdelay/udelay
   ├── maxim_init.c                     early platform init
   └── CMakeLists.txt                   conditional source inclusion

Some families (like Maxim) keep a per-chip subdirectory under the family
directory; a single-chip platform can be flat. Follow the closest existing
platform to the hardware you are porting to.

Implementing a peripheral: the SPI example
==========================================

SPI is the canonical case. The three pieces:

1. The ops table
----------------

Define a ``const`` ops table pointing at your functions (from
``drivers/platform/maxim/max32660/maxim_spi.c``):

.. code-block:: c

   const struct no_os_spi_platform_ops max_spi_ops = {
           .init           = &max_spi_init,
           .write_and_read = &max_spi_write_and_read,
           .transfer       = &max_spi_transfer,
           .transfer_dma   = &max_spi_transfer_dma,
           .remove         = &max_spi_remove,
   };

Implement at least ``init``, ``write_and_read``, and ``remove``; the optional
members (DMA transfers, abort) may be omitted — the generic API returns
``-ENOSYS`` when a member is ``NULL``, so leave out what your hardware does not
support rather than stubbing it.

2. The functions
----------------

Each function matches the signature declared in the ops struct in
``include/no_os_spi.h``. The ``init`` function allocates and configures the
descriptor; note how it reads platform-specific settings from the init_param's
``extra`` field:

.. code-block:: c

   int32_t max_spi_init(struct no_os_spi_desc **desc,
                        const struct no_os_spi_init_param *param)
   {
           struct max_spi_init_param *eparam;
           /* ... */
           if (!param || !param->extra)
                   return -EINVAL;

           eparam = param->extra;          /* platform-specific params */
           /* allocate the descriptor, configure the vendor SPI block
              using eparam, store handles for later calls ... */
   }

   int32_t max_spi_write_and_read(struct no_os_spi_desc *desc,
                                  uint8_t *data, uint16_t bytes_number)
   {
           /* drive the vendor SDK to do the transfer */
   }

This is the **only** layer allowed to ``#include`` and call vendor SDK
functions. Everything above it stays vendor-agnostic.

3. The header
-------------

The header exports the ops table and declares the platform's *extra* parameter
structure (from ``maxim_spi.h``):

.. code-block:: c

   /* the table a project points its init_param at */
   extern const struct no_os_spi_platform_ops max_spi_ops;

   /* platform-specific settings carried via init_param->extra */
   struct max_spi_init_param {
           uint32_t                       num_slaves;
           enum spi_ss_polarity           polarity;
           struct no_os_dma_init_param   *dma_param;
           /* ... */
   };

The ``extra`` field is the escape hatch for hardware-specific configuration that
does not belong in the generic init_param. A project fills a
``max_spi_init_param``, points ``no_os_spi_init_param.extra`` at it, and points
``no_os_spi_init_param.platform_ops`` at ``max_spi_ops``.

Repeat this pattern for each peripheral the platform supports: GPIO, UART, I2C,
IRQ, timer, and so on. UART (and usually a ``*_uart_stdio`` for ``printf``),
GPIO, IRQ, and a delay implementation are the practical minimum to run something
useful like the IIO Demo.

Wiring it into Kconfig
======================

Every platform option is a Kconfig symbol so projects can select it (see
:doc:`configuration_guide`). Two edits:

1. Create ``drivers/platform/<mcu>/Kconfig`` declaring a family menu and one
   option per peripheral, each depending on the abstract interface. Model it on
   ``drivers/platform/maxim/Kconfig``:

   .. code-block:: kconfig

      menuconfig MYMCU
              bool "MyMCU Platform Drivers"
              default n
      if MYMCU
      config SPI_MYMCU
              bool "Enable MyMCU SPI driver"
              depends on SPI
              default n
      config UART_MYMCU
              bool "Enable MyMCU UART driver"
              depends on UART
              default n
      # ... one per peripheral
      endif # MYMCU

   The ``depends on SPI`` ties the backend to the abstract interface — you
   cannot enable ``SPI_MYMCU`` unless ``SPI`` is on. This is the split described
   in :doc:`configuration_guide`: the variant config enables ``SPI``, the board
   config enables ``SPI_MYMCU``.

2. ``source`` your new Kconfig from ``drivers/platform/Kconfig`` alongside the
   existing platforms:

   .. code-block:: kconfig

      source "drivers/platform/mymcu/Kconfig"

Wiring it into the build
========================

Add a ``CMakeLists.txt`` in the platform directory that includes each source
file **only when its option is set**, using the ``no_os_sources_ifdef`` helper
(from ``drivers/platform/maxim/CMakeLists.txt``):

.. code-block:: cmake

   target_compile_definitions(no-os PUBLIC -DMYMCU_PLATFORM)

   no_os_sources_ifdef(CONFIG_SPI_MYMCU  ${CMAKE_CURRENT_SOURCE_DIR}/mymcu_spi.c)
   no_os_sources_ifdef(CONFIG_UART_MYMCU ${CMAKE_CURRENT_SOURCE_DIR}/mymcu_uart.c)
   no_os_sources_ifdef(CONFIG_GPIO_MYMCU ${CMAKE_CURRENT_SOURCE_DIR}/mymcu_gpio.c)
   # ... and any vendor-SDK sources each peripheral needs

This is what makes a disabled option cost nothing: ``CONFIG_SPI_MYMCU=n`` means
``mymcu_spi.c`` is never compiled. Vendor SDK source files the peripheral
depends on are added the same way, guarded by the same option.

You will also need the platform's build plumbing — a toolchain file, startup
code, and linker script — following the existing platform that most resembles
your target. If the new MCU uses a toolchain no-OS does not yet drive, a new
per-platform build guide under :doc:`build_guide` should accompany the port.

Bringing up a new board
=======================

Once a platform exists, targeting a new evaluation board on it is pure
configuration — no C code:

1. **Board defconfig.** Add ``board_configs/<platform>/<board>_defconfig``
   enabling the platform backends the board's MCU provides
   (``CONFIG_PLATFORM_DRIVERS=y``, ``CONFIG_MYMCU=y``, ``CONFIG_SPI_MYMCU=y``,
   ...). Copy the closest existing board's defconfig and adjust.

2. **CMake preset.** Register the board in the platform's
   ``board_configs/<platform>/CMakePresets.json`` so ``no_os_build.py``
   discovers it (it finds the repo root by ``CMakePresets.json`` and builds the
   valid combination list from the presets).

3. **Per-project board config (optional).** If a specific project needs
   board-specific settings (a pin-mux file, an extra peripheral), add
   ``projects/<proj>/boards/<variant>/<board>.conf`` — it is layered on top
   automatically (see :doc:`configuration_guide`).

Verify it appears and builds:

.. code-block:: bash

   python tools/scripts/no_os_build.py list --board <board>
   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board <board>

Bring-up strategy and verification
==================================

A pragmatic order that lets you test early:

1. **Delay + UART stdio first.** With just a working delay and UART ``printf``
   you can confirm the toolchain, startup, clocks, and linker script by printing
   from ``main()``.
2. **GPIO and IRQ next.** These underpin most higher-level drivers and the IIO
   transport.
3. **The bus you need (SPI/I2C) and timer.** Enough to talk to a real device.
4. **Run the IIO Demo.** Because its demo devices are software loopbacks, the
   ``iio`` variant exercises UART, IRQ, timer, and the IIO stack **without any
   external sensor** — making it the standard smoke test for a new port (see
   :doc:`getting_started`). If ``iio_readdev`` can read the loopback buffer, the
   platform's core is sound.
5. **Then a real driver.** Pick a project for a part you have and build it for
   your board.

Use the :doc:`troubleshooting` guide when a stage misbehaves — the "interface
enabled but backend missing" and "stale build" entries are common during
bring-up.

Checklist
=========

Adding a platform:

* ``drivers/platform/<mcu>/`` with ``<mcu>_<periph>.c`` / ``.h`` per peripheral.
* Each peripheral: an ops table, its functions (matching the API signatures),
  and a header exporting the table and any ``extra`` param struct.
* Vendor SDK calls confined to these files only.
* ``drivers/platform/<mcu>/Kconfig`` with per-peripheral options
  (``depends on`` the abstract interface), ``source``\ d from
  ``drivers/platform/Kconfig``.
* ``CMakeLists.txt`` using ``no_os_sources_ifdef(CONFIG_<PERIPH>_<MCU> ...)``.
* Toolchain, startup, linker plumbing; a build guide if the toolchain is new.

Adding a board (on an existing platform):

* ``board_configs/<platform>/<board>_defconfig``.
* An entry in the platform's ``CMakePresets.json``.
* Optional ``projects/<proj>/boards/<variant>/<board>.conf``.
* Confirm with ``no_os_build.py list`` / ``build``.

Where to go next
================

* :doc:`architecture` — the layer model and ``platform_ops`` dispatch you are
  implementing.
* :doc:`configuration_guide` — how board and variant defconfigs select your
  platform.
* :doc:`drivers_guide` — the driver-side conventions (init_param/descriptor)
  your platform mirrors.
* :doc:`build_guide` — per-platform toolchain setup; add a page for a new
  toolchain.
* `No-OS Doxygen Documentation <https://analogdevicesinc.github.io/no-OS/doxygen>`_
  — the exact signatures of every ``no_os_<periph>_platform_ops`` member.
