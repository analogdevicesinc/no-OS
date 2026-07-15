*******************
Configuration Guide
*******************

no-OS decides *which* drivers, platform code, and framework features go into a
build through a **Kconfig** system — the same mechanism the Linux kernel uses.
The :doc:`getting_started` tutorial referred to build *variants* (the ``.conf``
files), and :doc:`architecture` introduced Kconfig in passing. This page is the
full treatment: what the option system is, how the layered defconfigs combine
into one configuration, how to change options, and how the result reaches your
C code.

This is a companion to the per-platform build guides under :doc:`build_guide`,
which cover toolchain setup. Here we cover configuration itself.

.. contents:: Table of Contents
    :depth: 2

The mental model
================

Every configurable choice in no-OS is a **Kconfig symbol** named
``CONFIG_<SOMETHING>`` — ``CONFIG_SPI``, ``CONFIG_IIO``, ``CONFIG_ADC_DEMO``,
``CONFIG_MAXIM``, and so on. A build's configuration is nothing more than the
set of these symbols and their values.

Symbols come from three places, and a concrete build is produced by **layering**
three sources of *defconfig* (default-configuration) fragments on top of the
Kconfig defaults:

.. code-block::

   ┌─ project board config   projects/<proj>/boards/<variant>/<board>.conf
   │     board tweaks for this project+variant (pin maps, extra options)
   ├─ project variant config projects/<proj>/<variant>.conf
   │     what the application needs: drivers, frameworks, examples
   ├─ board config           board_configs/<platform>/<board>_defconfig
   │     what the MCU provides: platform peripheral drivers
   └─ Kconfig defaults       Kconfig files across the tree
         the baseline (mostly "disabled")

Higher layers override lower ones. The board config turns on the *platform*
peripheral implementations the MCU has; the variant config turns on the
*application* pieces (device drivers, IIO, the chosen example); the optional
per-variant board config adds anything specific to that project on that board.

Where the pieces live
=====================

Kconfig files (the option definitions)
--------------------------------------

Symbols are *declared* in ``Kconfig`` files, which form a tree:

* the **root** ``Kconfig`` — the hardware-interface abstractions
  (``CONFIG_SPI``, ``CONFIG_I2C``, ``CONFIG_UART``, ``CONFIG_GPIO``, ...);
* ``drivers/**/Kconfig`` — one option per driver and platform backend
  (``CONFIG_ADC_DEMO``, ``CONFIG_SPI_MAXIM``, ...);
* ``projects/<proj>/Kconfig`` — options private to a project.

A declaration looks like this (from the root ``Kconfig``):

.. code-block:: kconfig

   config SPI
           bool "Enable SPI"
           default n
           help
             Enable Serial Peripheral Interface (SPI) support.
             API: drivers/api/no_os_spi.c, include/no_os_spi.h

Projects can also declare their own structured choices. The IIO Demo, for
example, uses a ``choice`` so exactly one example variant is selected, plus a
``bool`` that ``select``\ s a dependency:

.. code-block:: kconfig

   choice IIO_DEMO_EXAMPLE
           prompt "Example variant"
           default IIO_DEMO_IIO_EXAMPLE
   config IIO_DEMO_IIO_EXAMPLE
           bool "Basic IIO example"
   config IIO_DEMO_IIO_TIMER_TRIGGER_EXAMPLE
           bool "IIO timer trigger example"
   endchoice

   config IIO_DEMO_NETWORKING
           bool "Enable ESP8266 Wi-Fi networking"
           default n
           select WIFI_NETWORKING

Board configs (what the MCU provides)
-------------------------------------

``board_configs/<platform>/<board>_defconfig`` enables the platform peripheral
drivers a given board's MCU offers. For example
``board_configs/maxim/max32650fthr_defconfig``:

.. code-block::

   CONFIG_PLATFORM_DRIVERS=y
   CONFIG_MAXIM=y
   CONFIG_UART_MAXIM=y
   CONFIG_SPI_MAXIM=y
   CONFIG_I2C_MAXIM=y
   CONFIG_GPIO_MAXIM=y
   CONFIG_TIMER_MAXIM=y
   ...

These are the concrete ``platform_ops`` implementations from
:doc:`architecture` — turning on ``CONFIG_SPI_MAXIM`` is what compiles the
Maxim SPI backend into the build.

Variant configs (what the application needs)
--------------------------------------------

``projects/<proj>/<variant>.conf`` is the defconfig for one build *variant*. The
IIO Demo's ``iio.conf``:

.. code-block::

   CONFIG_UART=y
   CONFIG_IRQ=y
   CONFIG_TIMER=y
   CONFIG_IIO=y
   CONFIG_ADC=y
   CONFIG_ADC_DEMO=y
   CONFIG_ADC_IIO_DEMO=y
   CONFIG_DAC=y
   CONFIG_DAC_DEMO=y
   CONFIG_DAC_IIO_DEMO=y
   CONFIG_IIO_DEMO_IIO_EXAMPLE=y

Note it enables the *abstract* interfaces (``CONFIG_UART``, ``CONFIG_SPI``) and
the device/framework code — but not any platform backend. That is the board
config's job. **This split is the whole point:** the variant config is
platform-independent, so the same ``iio.conf`` builds for Maxim, STM32, ADuCM,
and Pico. Each variant of a project is exactly this — a named defconfig — which
is why ``iio``, ``iio_usb_uart``, and ``iio_wifi`` are separate variants:

.. code-block::

   iio_wifi.conf = iio.conf
                 + CONFIG_IIO_DEMO_NETWORKING=y
                 + CONFIG_WIFI_SSID="..."
                 + CONFIG_WIFI_PWD="..."

Per-variant board configs (project + board specifics)
-----------------------------------------------------

Optionally, ``projects/<proj>/boards/<variant>/<board>.conf`` carries settings
needed only for a specific project on a specific board — for instance a pin-mux
file or an extra peripheral. From the IIO Demo Wi-Fi build on the ADuCM board:

.. code-block::

   CONFIG_ADUCM_PINMUX_PATH="pinmux_config.c"
   CONFIG_TIMER=y

The build utility layers the matching one automatically based on the board you
select; you rarely edit these by hand.

How the layers become one configuration
=======================================

When you run the build utility from :doc:`getting_started`:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio --board max32650fthr

it resolves the three layers for you:

1. ``--board max32650fthr`` selects the CMake **preset** (which points at
   ``board_configs/maxim/max32650fthr_defconfig``);
2. ``--variant iio`` selects ``projects/iio_demo/iio.conf`` as the
   ``PROJECT_DEFCONFIG``;
3. if ``projects/iio_demo/boards/iio/max32650fthr.conf`` exists, it is layered
   on top.

Under the hood, ``tools/scripts/generate_config.py`` loads the Kconfig tree and
merges each defconfig on top of it (later fragments win, mirroring the layering
above). It then emits two artifacts into the build directory:

* a **CMake** file (``config.cmake``) with ``set(CONFIG_FOO <value>)`` lines,
  which the build system uses to decide *which source files to compile*;
* a **C header** (``no_os_config.h``, guarded by ``NO_OS_CONFIG_H``) with
  ``#define CONFIG_FOO <value>`` for every symbol, which your source code can
  test.

Changing the configuration
==========================

There are three ways to adjust options, from most permanent to most transient.

Edit a defconfig
----------------

To make a change part of a variant, edit its ``.conf`` file and rebuild with
``--clean`` so the configuration is regenerated:

.. code-block:: bash

   # e.g. set your network for the Wi-Fi variant
   # projects/iio_demo/iio_wifi.conf
   #   CONFIG_WIFI_SSID="MyNetwork"
   #   CONFIG_WIFI_PWD="MyPassword"

   python tools/scripts/no_os_build.py build \
      --project iio_demo --variant iio_wifi --board eval-adicup3029 --clean

This is the right place for changes you want to keep. To add a whole new variant,
add a new ``<variant>.conf`` (and, if needed, ``boards/<variant>/<board>.conf``
files) — the build utility discovers variants by globbing ``*.conf`` in the
project directory, so it will appear in ``list`` automatically.

Browse and edit interactively (menuconfig)
------------------------------------------

After a project has been configured once, you can explore and toggle options in
the familiar Kconfig terminal UI. Using a manual CMake configure:

.. code-block:: bash

   cmake -B iio_demo --preset max32650fthr \
         -DPROJECT_DEFCONFIG=iio_demo/iio.conf
   cmake --build iio_demo --target menuconfig

``menuconfig`` is the fastest way to *discover* what options exist and read
their ``help`` text and dependencies. Note that it edits the generated
configuration in the build directory — to persist a change, copy it back into
the appropriate ``.conf``.

Override on the CMake command line
----------------------------------

For a one-off manual configure, pass ``-DCONFIG_FOO=...`` directly. The
:doc:`build_guide` CMake pages document the full manual flow (selecting the
preset with ``--preset`` and the variant with ``-DPROJECT_DEFCONFIG=``).

Using configuration in code
===========================

The generated header makes every symbol available as a ``CONFIG_*`` macro, so
source and driver code can compile conditionally:

.. code-block:: c

   #ifdef CONFIG_IIO_DEMO_NETWORKING
           /* set up the ESP8266 Wi-Fi transport */
   #else
           /* use the plain UART transport */
   #endif

Boolean symbols that are disabled are still defined (as empty), which is why the
codebase guards with ``#ifdef CONFIG_FOO`` rather than testing a value. String
and integer symbols (``CONFIG_WIFI_SSID``, buffer sizes, ...) expand to their
literal value.

At the build-system level, the ``CONFIG_*`` values in ``config.cmake`` gate
which files are added to the compile — e.g. ``CONFIG_SPI_MAXIM=y`` is what pulls
``drivers/platform/maxim/.../maxim_spi.c`` into the build. This is how a
disabled option costs nothing: its code is never compiled in.

Troubleshooting
===============

**An option I set in the ``.conf`` did not take effect.**
   The configuration is generated at *configure* time. After editing a
   defconfig, rebuild with ``--clean`` (or delete the ``build-*`` directory) so
   it is regenerated.

**A driver's symbol is on but the platform backend is missing (link errors).**
   You enabled the abstract interface (e.g. ``CONFIG_SPI``) in the variant
   config but the board config does not provide the backend (e.g.
   ``CONFIG_SPI_MAXIM``). Interfaces come from the variant; backends come from
   the board defconfig — check both layers.

**``menuconfig`` changes disappeared.**
   ``menuconfig`` edits the configuration inside the build directory, not the
   source ``.conf``. Copy the change into the appropriate defconfig to keep it.

**A ``select``\ ed option is unexpectedly on.**
   Kconfig ``select`` forces a dependency on. For example
   ``CONFIG_IIO_DEMO_NETWORKING`` ``select``\ s ``CONFIG_WIFI_NETWORKING`` — you
   cannot turn the latter off while the former is on. Follow the ``select``
   chain in the ``Kconfig`` files.

Where to go next
================

* :doc:`getting_started` — the build utility and variants in practice.
* :doc:`architecture` — how ``CONFIG_*`` selection maps to the platform_ops the
  code dispatches through.
* :doc:`build_guide` — per-platform toolchain setup and the manual CMake flow.
