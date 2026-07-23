SSD1306 OLED Display no-OS Example Project
==========================================

.. no-os-doxygen::

.. contents::
    :depth: 3

Overview
--------

This project demonstrates the usage of the SSD1306 OLED display with the Maxim
platform. It integrates the LittlevGL (LVGL) graphics library to create a
graphical user interface (GUI) and display animations on the SSD1306.

Features
~~~~~~~~

* **Display Initialization**: Configures the SSD1306 OLED display using I2C
  communication.
* **UART Configuration**: Sets up UART for debugging and communication.
* **Graphics Rendering**: Utilizes the LVGL library to render graphics and
  animations.
* **Startup Screen**: Displays a custom startup screen with a logo.
* **Scrolling Text Animation**: Demonstrates a scrolling text animation
  inspired by the Star Wars intro.

Dependencies
~~~~~~~~~~~~~

* **LVGL Library**: A lightweight graphics library for embedded systems.
* **No-OS Drivers**: Drivers for I2C, UART, and other peripherals.
* **Maxim SDK**: Platform-specific drivers and utilities for the Maxim platform.

How It Works
~~~~~~~~~~~~~

#. **Initialization**:

   * The ``example_main`` function initializes the system using ``no_os_init``.
   * UART is configured for debugging using ``configure_uart``.
   * The SSD1306 display is initialized using ``configure_display``.

#. **Startup Screen**: The ``startup_screen`` function displays a logo on the
   SSD1306 OLED display for 5 seconds.

#. **Scrolling Text Animation**: The ``test_draw`` function creates a label with
   scrolling text and animates it using LVGL's animation API.

#. **Main Loop**: The main loop continuously updates the LVGL timer handler and
   refreshes the display.

Functions Overview
~~~~~~~~~~~~~~~~~~~

* ``configure_uart`` - Configures the UART for debugging and communication.
* ``configure_display`` - Initializes the SSD1306 OLED display using I2C
  communication.
* ``display_task`` - Sets up the LVGL display buffer and flush callback for
  rendering.
* ``my_flush_cb`` - Flushes the LVGL buffer to the SSD1306 display by converting
  row-major data to column-major format.
* ``startup_screen`` - Displays a startup logo on the SSD1306 OLED display.
* ``test_draw`` - Creates a scrolling text animation on the display.
* ``example_main`` - The main entry point of the project. Initializes the
  system, configures peripherals, and starts the display tasks.

Hardware Requirements
~~~~~~~~~~~~~~~~~~~~~~

* **SSD1306 OLED Display**: A 128x64 pixel OLED display.
* **Maxim Platform**: A development board compatible with the Maxim SDK.
* **I2C Interface**: For communication with the SSD1306 display.

No-OS Supported Examples
------------------------

Example
~~~~~~~

This is the only example in this project, so it builds by default. It
initializes the SSD1306 display over I2C, renders a startup logo, and then
runs a scrolling text animation using the LVGL library.

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

**Build Command**

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``example``.
Available boards: ``max78000fthr``.
Replace ``--variant`` / ``--board`` accordingly.

This project depends on LVGL, which the CMake build fetches automatically at
configure time into ``libraries/lvgl`` (no submodule init required).

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (example on the max78000fthr board)
   python tools/scripts/no_os_build.py build \
      --project ssd1306 --variant example --board max78000fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project ssd1306 --variant example --board max78000fthr \
      --probe openocd --flash

.. note::

   Using an RTOS implementation, one must make sure to include the
   ``lv_tick_inc()`` function in the system timer callback, in order to have
   regular updates on the screen. In a baremetal solution, like this project,
   the file ``drivers/platform/maxim/max78000/maxim_delay.c`` needs the
   following change:

   .. code-block:: c

      void SysTick_Handler(void)
      {
         MXC_DelayHandler();
         lv_tick_inc();
         _system_ticks++;
      }
