Dual-Core IMU Example (MAX78000 ARM + RISC-V, shared bit-banged SPI)
====================================================================

Overview
--------

A two-sensor dual-core workload built on the `dual_core <../dual_core>`_
doorbell example. Both cores run **real no-OS drivers** over one **shared,
bit-banged SPI bus**:

- **CPU1 (RISC-V RV32)** -- drives an **ADIS16577-2** IMU with the no-OS
  ``no_os_spi`` + ``adis``/``adis1657x`` drivers (cross-compiled into the
  coprocessor image), reads raw gyro/accel/data-counter samples as fast as the
  bus allows, and publishes them to the ARM.
- **CPU0 (ARM Cortex-M4F)** -- drives an **ADXL345** on the same pins with the
  no-OS ``adxl345`` driver, integrates the RISC-V's raw gyro samples into
  roll/pitch/yaw, and logs both sensors at ~10 Hz.

.. code-block:: text

   ┌──────────────────────────────────────────────────────────────────────┐
   │                          MAX78000 SoC                                  │
   │  CPU0 (ARM)            Shared resources            CPU1 (RISC-V)       │
   │  ┌───────────┐    ┌────────────────────────┐    ┌──────────────┐      │
   │  │ adxl345   │    │ SEMA 0x4003E000        │    │ adis / no_os │      │
   │  │ (no-OS)   │    │  ├ doorbell irq0/irq1  │    │ _spi (no-OS, │      │
   │  │ integrate │◀──▶│  ├ semaphores[3]=SPI   │◀──▶│ RV-compiled) │      │
   │  │ gyro,     │    │  │   bus lock          │    │ raw samples  │      │
   │  │ logs both │    │ IPC table SRAM2        │    │              │      │
   │  └─────┬─────┘    └────────────────────────┘    └──────┬───────┘      │
   │        │       bit-banged SPI: P0.7/5/6 + CS            │              │
   │        └───────────────── shared bus ──────────────────┘              │
   └──────────────────────────────────────────────────────────────────────┘

Why a bit-banged SPI bus
------------------------

The MAX78000's only header-accessible hardware SPI (SPI0 on the FTHR) is gated
out of RISC-V builds by the SDK, and driving it from the RISC-V proved
impractical. Instead both cores share a **software SPI master** implemented as
a reusable no-OS platform driver,
`maxim_gpio_spi <../../../../../drivers/platform/maxim/max78000/maxim_gpio_spi.c>`_
(``CONFIG_SPI_GPIO_MAXIM``). It plugs into ``no_os_spi`` like any other
backend, so the stock ``adis`` and ``adxl345`` drivers run unchanged.

The subtlety of sharing one bus between two independent CPUs is **driver
contention**, solved by two rules the driver enforces:

- **Single-driver bus ownership.** Each core owns and drives **only its own
  CS** (ADIS->P0.11 on the RISC-V, ADXL->P0.19 on the ARM). The shared
  **SCLK+MOSI are driven push-pull only for the duration of a transfer** and
  returned to high-Z inputs afterwards; MISO is always an input. So the idle
  core never fights the active core on any shared net. (An earlier version
  drove all pins from both cores continuously, which corrupted MOSI --
  transition-heavy command bytes got garbled while ``0x00``/``0x80``
  survived.)
- **3.3 V rail (VDDIOH).** The sensors are powered from 3V3, so every pin is
  set to ``vssel = VDDIOH``; at the reset default (1.8 V) driven highs and
  sampled inputs use the wrong reference.

- **Serialized by a hardware semaphore.** The RISC-V is ``rv32imc`` (no atomic
  extension), so a software lock in SRAM is unsafe. The two cores serialize
  every transaction with ``MXC_SEMA->semaphores[3]`` (atomic test-and-set) --
  the same SEMA block used for the doorbells, on a distinct index. The driver
  itself is lock-agnostic; the application brackets each device access with
  ``imu_spi_lock/unlock`` (see ``imu_ipc_shared.h``), and the driver
  takes/releases the shared pins inside that window.

Both sensors run **SPI mode 3**, MSB-first, at the bit-bang loop's native
rate.

Wiring (MAX78000FTHR)
---------------------

=========== ======= ============ =======
Signal      MCU pin ADIS16577-2  ADXL345
=========== ======= ============ =======
SCLK        P0.7    SCLK         SCLK
MOSI (COPI) P0.5    DIN          MOSI
MISO (CIPO) P0.6    DOUT         MISO
CS (ADIS)   P0.11   /CS          --
CS (ADXL)   P0.19   --           /CS
VDD / GND   3V3/GND VDD/GND      VCC/GND
=========== ======= ============ =======

This matches the `single-core example <../imu_single_core>`_ wiring exactly, so
a board that works there works here.

Data path
---------

RISC-V (ADIS16577-2, no-OS driver)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Configures ``max_gpio_spi_ops`` (pins from ``imu_ipc_shared.h``, CS = P0.11,
  VDDIOH), then ``adis_init`` (no reset GPIO -> software reset,
  ``dev_id = ADIS16577_2``).
- Loop, under the SPI bus lock: ``adis_read_{x,y,z}_gyro`` + ``_accl`` +
  ``adis_read_data_cntr``, published to the shared table. Each iteration ends
  with a ``IMU_RV_LOOP_PERIOD_US`` (500 us) delay taken **outside** the lock,
  matched to the ADIS 2 kHz ODR: reading faster only re-reads the same sample,
  and back-to-back bus-locked reads with no idle gap would starve the ARM on
  the shared (unfair) semaphore lock. On a read error it backs off 10 ms
  instead. Publishes only once the previous doorbell is acked so no
  notification is coalesced away; never blocks on the ARM.

ARM (integration + ADXL345 + logging)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Waits the doorbell, snapshots the raw ADIS payload, and integrates gyro on
  **every** sample using the **``DATA_CNTR`` delta** (zero-order hold), so the
  result is correct regardless of poll rate. The 32-bit gyro (2 kHz ODR)
  converts to milli-degrees via ``accum / 5242880``.
- Reads the ADXL345 once per event (bus-locked) and logs at ~10 Hz. Output is
  decimated by **ADIS sample count** (one line per 200 samples = 10 Hz in
  sensor time), so the log rate tracks sensor time rather than the doorbell
  rate. ``seq`` reports how many raw samples the coprocessor pushed for each
  line.

Shared table (``imu_ipc_shared.h``, SRAM2 @ 0x20011000)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Integer-only payload (raw 32-bit sensor counts) so the exchange never depends
on matching float ABIs. Magic ``0xAD152016``; ``state`` tracks RISC-V bring-up
(BOOT -> SPI_OK -> IMU_DETECTED -> RUNNING, or ERROR/FAULT). Detection truth is
the ``prod_id`` field (ARM compares it), not the transient state.

Reusable platform pieces
------------------------

This example pulled its previously example-local platform code into no-OS so
other projects can reuse it:

- **``maxim_gpio_spi.{c,h}``** (``CONFIG_SPI_GPIO_MAXIM``) -- the bit-banged
  SPI master described above, pin-parameterized via
  ``struct max_gpio_spi_init_param``.
- **``maxim_sys.{c,h}``** (``CONFIG_SYS_MAXIM``) -- the shared
  ``__wrap_MXC_SYS_Reset_Periph()`` read-modify-write replacement for the SDK's
  destructive peripheral-reset store (which would otherwise clobber the SEMA
  bus lock / the SIMO rail). Enable with ``-Wl,--wrap=MXC_SYS_Reset_Periph``;
  used by both this example and the single-core one.

``maxim_add_riscv_coprocessor()`` (in ``cmake/maxim/maxim_coprocessor.cmake``)
links a ``NOOS_DRIVERS`` list (no-OS + Maxim-platform + MXC + ADIS sources)
into the coprocessor image. ``maxim_delay.c``'s ``no_os_get_time()`` is
``#ifndef __riscv``-guarded (the RISC-V has no SysTick) and its
``no_os_udelay/mdelay`` use a cycle-count path.

Build & flash
-------------

.. code-block:: bash

   cmake --preset max78000fthr -B out/build/max78000fthr \
     -DPROJECT_DEFCONFIG=max78000/imu_dual_core.conf
   cmake --build out/build/max78000fthr --target max78000

The RISC-V image (no-OS + ADIS + MXC) is ``.incbin``-embedded into the single
ARM ELF; there is no separate flash step. Flash the ARM ELF with your usual
OpenOCD/probe flow, then open the USB-UART console at 115200 8N1.

Expected console
----------------

.. code-block:: text

   ========================================
   MAX78000 Dual-Core IMU Example (ARM Side)
   ========================================
   [ARM] CS mapping: ADIS=P0.11 ADXL=P0.19
   [ARM] Waiting for RISC-V to detect the ADIS16577-2...
   [ARM] ADIS16577-2 detected: PROD_ID=16577 DIAG=0x0000

   [ARM] Streaming (ADIS reads on RISC-V, integration on ARM, ADXL accel on ARM)...

   [IMU] rpy=+0.012,-0.004,+0.031 deg  adis_a=+0.001,-0.002,+0.998 g  pmod=+0.011,-0.019,+0.992 g (n=2000, seq=..., +...)
   ...

Static board -> gyro angles drift slowly (bias), both ``az`` ~ +1 g. Rotate an
axis -> its angle ramps and returns; the corresponding accel axis responds.

Debug hints
-----------

- **Garbled bytes from either sensor** -- a bus-ownership or lock violation.
  Every transaction on both cores must bracket with ``imu_spi_lock/unlock``,
  and only the lock holder may drive SCLK/MOSI. If MISO is clean but MOSI is
  corrupted, two cores are driving the bus at once.
- **``PROD_ID != 16577``** -- MISO/CS/mode wiring, the VDDIOH rail, or the
  bit-bang clock rate. Detection is reported via the ``prod_id`` field, not
  ``state``.
- **``state`` stuck at ``SPI_OK`` (1)** -- SPI up but ADIS bring-up (reset +
  PROD_ID) never completed; check ADIS wiring/CS (P0.11). Inspect the shared
  table at ``0x20011000`` (magic ``0xAD152016``).
- **ADXL init failed** -- ADXL CS (P0.19) or MISO wiring.

Key files
---------

- ``maxim_gpio_spi.{c,h}`` (in ``drivers/platform/maxim/max78000/``) --
  reusable bit-banged SPI master.
- ``maxim_sys.{c,h}`` (same dir) -- shared reset-wrapper helper.
- ``imu_ipc_shared.h`` -- shared table layout + SPI hardware-semaphore bus lock
  + pin map.
- ``imu_dual_core_example_riscv.c`` -- RISC-V: no-OS SPI + ADIS driver, raw
  sample publisher.
- ``imu_dual_core_example.c`` / ``.h`` -- ARM: IPC, coprocessor boot, gyro
  integration, ADXL345, logging.

References
----------

- `../dual_core/README.rst <../dual_core/README.rst>`_ -- base doorbell
  example, RISC-V embed/boot mechanics.
- `../imu_single_core <../imu_single_core>`_ -- same two sensors on the ARM
  alone (hardware SPI0), the known-good wiring reference.
- ``drivers/imu/adis.c``, ``drivers/imu/adis1657x.c`` -- ADIS driver +
  ADIS16577 config.
- ``drivers/accel/adxl345/adxl345.c`` -- ADXL345 no-OS driver.
- ``drivers/platform/maxim/max78000/maxim_ipc.{c,h}`` -- SEMA doorbell/mailbox
  IPC.
