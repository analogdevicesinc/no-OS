*****************
No-OS Build Guide
*****************

Clone no-OS:

    .. code-block::

        git clone https://github.com/analogdevicesinc/no-OS

The CMake build fetches external libraries (lwIP, LVGL, mbedTLS, FreeRTOS, Paho
MQTT, Azure SDK, esh, TMC-API, precision-converters-library, pico-sdk)
automatically at configure time, pinned to the version selected via Kconfig,
into its path under ``libraries/`` so it is cloned only once and reused by all
subsequent builds. These fetched paths are git-ignored.

Set ``NO_OS_CACHE_DIR`` to fetch into a persistent or shared, version-keyed
store outside the tree instead (useful in CI or to share one clone across
multiple checkouts/worktrees). To point a single library at a local working
copy, set ``NO_OS_DEP_<LIB>_PATH`` (e.g. ``NO_OS_DEP_LWIP_PATH``,
``NO_OS_DEP_MBEDTLS_PATH``); it takes priority over everything else.

The default pinned version of every dependency is its ``CONFIG_<LIB>_VERSION``
Kconfig default (a git tag, branch or commit); set that config to fetch a
different version.


Common Prerequisites
====================

The following tools are required by all CMake-based platform builds.

**CMake 3.28 or newer** (presets v7 are used):

    .. code-block:: bash

        # Linux (Debian/Ubuntu)
        $ sudo apt install cmake

    .. code-block:: bat

        :: Windows
        > winget install Kitware.CMake

**Ninja** (the CMake presets use the Ninja generator):

    .. code-block:: bash

        # Linux (Debian/Ubuntu)
        $ sudo apt install ninja-build

    .. code-block:: bat

        :: Windows
        > winget install Ninja-build.Ninja

XILINX
======
.. toctree::
    :maxdepth: 1

    build_guides/build_xilinx

INTEL
=====
.. toctree::
    :maxdepth: 1

    build_guides/build_intel

MAXIM
======
.. toctree::
   :maxdepth: 1

   build_guides/build_maxim_cmake

FTD2XX
======
.. toctree::
    :maxdepth: 1

    build_guides/build_ftd2xx

MAC
======
.. toctree::
    :maxdepth: 1

    build_guides/build_mac

STM32
=====
.. toctree::
    :maxdepth: 1

    build_guides/build_stm32_cmake

ADUCM3029
=========
.. toctree::
    :maxdepth: 1

    build_guides/build_aducm3029_cmake

RASPBERRY PI PICO
=================
.. toctree::
    :maxdepth: 1

    build_guides/build_pico_cmake

LATTICE PROPEL SDK
===================
.. toctree::
    :maxdepth: 1

    build_guides/build_lattice

WSL2
====
.. toctree::
    :maxdepth: 1

    build_guides/build_wsl

