.. Project documentation template for no-OS example projects.
..
.. HOW TO USE THIS TEMPLATE
.. ------------------------
.. 1. Copy this file to ``projects/<your-project>/README.rst``.
.. 2. Replace every ``<...>`` placeholder and delete the guidance comments
..    (lines starting with ``..``) as you fill each section in.
.. 3. Keep the section order and heading underline styles below. RST underline
..    characters used here, in order of depth:
..       ``=`` title       (level 1)
..       ``-`` section      (level 2)
..       ``~`` subsection   (level 3)
..       ``^`` sub-sub      (level 4)
..    Underlines must be at least as long as the text above them.
.. 4. Drop any section that genuinely does not apply (e.g. omit "Applications"
..    for a pure interface/carrier board), but prefer completeness.

<PROJECT NAME> no-OS Example Project
====================================
.. Underline the title with ``=`` for the full width of the text above.

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

.. One bullet per evaluation board this project targets. Link to the ADI
.. product page for each.

* `<EVAL-BOARD> <https://www.analog.com/<EVAL-BOARD>>`_

Overview
--------

.. Two or three short paragraphs describing the part and the evaluation
.. board: what it is, key specs (resolution, channels, interfaces), supply
.. requirements, and notable on-chip features. Pull the numbers from the
.. device data sheet.

<Describe the evaluation board and the device it carries. Summarize the
key electrical characteristics and the primary function of the part.>

Applications
------------

.. Bullet list of typical end applications for the device. Usually lifted
.. from the "Applications" section of the data sheet.

* <Application 1>
* <Application 2>
* <Application 3>

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. Describe how the board is powered (USB, external supply, via the carrier
.. connector) and list the required supply rails and their voltage ranges.

<Describe the power supply requirements and how the board is powered.>

On-board Connectors
~~~~~~~~~~~~~~~~~~~~

.. Document the connectors, headers, and default jumper/solder-link settings
.. the user must be aware of. Use a simple RST table where it helps.

========= =====================================================
Connector Function
========= =====================================================
<Jx>      <What it carries / how it is used>
========= =====================================================

<Note any jumpers or solder links that affect the default configuration.>

No-OS Supported Examples
------------------------

.. Describe the variant-based build flow. The platform ``main()`` is a thin
.. dispatcher that calls ``example_main()`` from the selected variant. Point
.. at the shared ``src/common`` init data and the platform-specific
.. ``src/platform`` macros/params for this project.

This project is organized around the no-OS variant based build flow.
Selecting a variant at build time (``--variant <name>``) chooses which
application is compiled. The platform ``main()`` is a thin dispatcher that
calls ``example_main()``, provided by the selected example. Shared
initialization data is defined in
`src/common <https://github.com/analogdevicesinc/no-OS/tree/main/projects/<your-project>/src/common>`__,
and platform-specific macros and extra init parameters are in
`src/platform <https://github.com/analogdevicesinc/no-OS/tree/main/projects/<your-project>/src/platform>`__.

.. Add one subsection per variant. Repeat the block below for each.

<Variant Name> Example
~~~~~~~~~~~~~~~~~~~~~~~

.. Describe what the variant does and, if relevant, which source file holds
.. the tunable parameters the user may want to change.

<Describe what this example does.>

.. For an IIO variant, keep these client pointers:
.. If you are not familiar with ADI IIO Application, please take a look at:
.. :dokuwiki:`IIO No-OS </resources/tools-software/no-os-software/iio>`
..
.. If you are not familiar with ADI IIO-Oscilloscope Client, please take a
.. look at:
.. :dokuwiki:`IIO Oscilloscope </resources/tools-software/linux-software/iio_oscilloscope>`

No-OS Supported Platforms
-------------------------

.. Add one subsection per supported platform (e.g. Maxim, ADuCM3029, STM32,
.. Xilinx). Repeat the block below for each platform.

<Platform>
~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* `<Carrier / MCU board> <https://www.analog.com/<board>>`_
* `<EVAL-BOARD> <https://www.analog.com/<EVAL-BOARD>>`_

Connections
^^^^^^^^^^^

.. Describe the physical wiring between the eval board and the carrier: the
.. bus (SPI/I2C), the relevant pins, the UART console port and baud rate.
.. A pin-mapping RST table is encouraged for hand-wired connections.

<Describe how to connect the boards and where the UART console appears.>

Build Command
^^^^^^^^^^^^^

.. State the build system, the available variants and boards, and any
.. environment variable the toolchain needs (e.g. MAXIM_LIBRARIES,
.. CCES_HOME). Then give copy-pasteable commands: a plain build and a
.. build-and-flash. Show the env var with Unix `export` first, then the
.. PowerShell equivalent right after for Windows users.
..
.. Link to the platform's CMake build guide on GitHub Pages for toolchain
.. setup and prerequisites. Use the guide matching this platform, e.g.:
..    Maxim     -> build_maxim_cmake.html
..    ADuCM3029 -> build_aducm3029_cmake.html
..    STM32     -> build_stm32_cmake.html
..    Xilinx    -> build_xilinx_cmake.html

The <Platform> platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``<variant>``.
Available boards: ``<board>``.

For toolchain setup and prerequisites, see the
`<Platform> CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_<platform>_cmake.html>`__.

.. code-block:: bash

   # set any required toolchain environment variable, e.g.:
   export <TOOLCHAIN_ENV>=</path/to/toolchain>
   # PowerShell (Windows) equivalent:
   #   $env:<TOOLCHAIN_ENV> = "<path\to\toolchain>"

   cd no-OS

   # build the example on the target board
   python tools/scripts/no_os_build.py build \
      --project <your-project> --variant <variant> --board <board>

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project <your-project> --variant <variant> --board <board> \
      --probe openocd --flash
