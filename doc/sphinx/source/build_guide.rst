*****************
No-OS Build Guide
*****************

Clone no-OS:

    .. code-block::

        git clone https://github.com/analogdevicesinc/no-OS

External libraries (lwIP, LVGL, mbedTLS, FreeRTOS, Paho MQTT, Azure SDK, esh,
TMC-API, precision-converters-library, pico-sdk) are no longer git submodules.
The CMake build fetches each one automatically at configure time, pinned to the
version selected via Kconfig, into its path under ``libraries/`` so it is cloned
only once and reused by all subsequent builds. These fetched paths are
git-ignored.

Set ``NO_OS_CACHE_DIR`` to fetch into a persistent or shared, version-keyed
store outside the tree instead (useful in CI or to share one clone across
multiple checkouts/worktrees). To point a single library at a local working
copy, set ``NO_OS_<LIB>_DIR`` (e.g. ``NO_OS_LWIP_DIR``, ``NO_OS_MBEDTLS_DIR``);
it takes priority over everything else.


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

WIN
===
.. toctree::
    :maxdepth: 1

    build_guides/build_win

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

