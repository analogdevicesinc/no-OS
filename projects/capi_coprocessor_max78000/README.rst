CAPI Coprocessor no-OS Example Project
======================================

.. no-os-doxygen::

.. contents:: Table of Contents
    :depth: 3

Supported Evaluation Boards
---------------------------

* `MAX78000EVKIT <https://www.analog.com/MAX78000EVKIT>`_

Overview
--------

The MAX78000 carries two cores: an Arm Cortex-M4 and a RISC-V (RV32)
coprocessor, referred to as CPU1 in the hardware documentation. This
project runs on the Cortex-M4 and drives the RV32 entirely through the
Common HAL coprocessor API (``capi_coprocessor.h``), narrating every step
over the board's stdio UART.

Where the plain lifecycle API only lets a caller start and stop a
coprocessor, the primitives exercised here let it be *configured*: where
the core begins executing, what activity or power state it sits in, and
which of its sub-blocks are clocked.

Demo Steps
----------

The demo runs the following sequence, printing the outcome of each call:

1. **Capability negotiation.** ``capi_coprocessor_get_boot_caps()`` is
   decoded into a readable list. The MAX78000 driver advertises
   ``VECTOR_BASE``, ``ENTRY_PC`` and ``VECTOR_MODE``; the RV32 has no way
   to preload a stack pointer or boot argument registers, so those bits
   are deliberately absent.

2. **Boot vector round-trip.** A known flash address is written with
   ``capi_coprocessor_set_vector_base()`` and read back with
   ``capi_coprocessor_get_vector_base()``. This is ``MXC_FCR->urvbootaddr``
   round-tripping on real silicon.

3. **Refused field.** ``capi_coprocessor_set_boot_config()`` is called with
   a ``stack_pointer`` set. The expected result is ``-ENOTSUP``: an
   unsupported field is rejected rather than accepted and silently
   dropped.

4. **Mode walk.** A single-instruction RV32 idle loop in flash provides a
   known-valid reset target. The demo then walks
   ``RUN -> PARKED -> STANDBY -> LOW_POWER -> RUN -> OFF``, with
   ``capi_coprocessor_get_mode()`` read back after each transition.
   These modes are **coprocessor-scoped**: the host core keeps running and
   printing throughout. SoC-wide power modes are a different API
   (``capi_pmu_aon.h``).

5. **Resources.** Each resource class is enumerated with
   ``capi_coprocessor_resource_get_count()``, then every member is
   disabled and re-enabled with a read-back after each change. One
   past-the-end index is also requested, to show it is refused
   (``-EINVAL``) rather than clamped.

Hardware Mapping
----------------

============================  ==========================================
CAPI primitive                MAX78000 hardware
============================  ==========================================
``set/get_vector_base``       ``MXC_FCR->urvbootaddr``
``MODE_RUN``                  ``pclkdis1.CPU1`` cleared, reset pulsed
``MODE_PARKED``               ``pclkdis1.CPU1`` set, state retained
``MODE_STANDBY``              clock gated; next ``RUN`` restarts at vector
``MODE_LOW_POWER``            clocked with the RV32 I-cache disabled
``MODE_OFF``                  clock disabled; equivalent of a shutdown
``RESOURCE_DATA_LANE`` 0      ``MXC_SYS_PERIPH_CLOCK_SMPHR`` (host<->RV32)
``RESOURCE_CACHE`` 0          ``MXC_ICC1`` (RV32 instruction cache)
``RESOURCE_ACCELERATOR`` 0    ``MXC_SYS_PERIPH_CLOCK_CNN``
``RESOURCE_POWER_DOMAIN`` 0   ``MXC_SYS_PERIPH_CLOCK_CPU1``
============================  ==========================================

Two hardware facts shape the driver and are worth knowing when reading
it. There is exactly one CPU1 clock gate — ``MXC_SYS_ClockEnable(CPU1)``
and clearing ``pclkdis1.CPU1`` are the same bit. And
``MXC_F_GCR_RST1_CPU1`` is a self-clearing pulse, not a level, so a
"held in reset" state is not expressible; ``STANDBY`` gates the clock and
records that the next ``RUN`` must restart from the vector.

Building
--------

.. code-block:: bash

    cmake --preset max78000evkit -B build-max78000evkit \
        -DPROJECT_DEFCONFIG=capi_coprocessor_max78000/demo.conf
    cmake --build build-max78000evkit --target capi_coprocessor_max78000

``MAXIM_LIBRARIES`` must point at the MSDK ``Libraries`` directory.

Flash with the generated flash target, then open the stdio UART at
115200 8N1 to read the narration.
