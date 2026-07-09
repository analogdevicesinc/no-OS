Dual-Core Example (MAX78000 ARM + RISC-V)
=========================================

Overview
--------

This example demonstrates the no-OS coprocessor API and inter-core
communication (IPC) framework for the MAX78000's dual-core architecture:

- **CPU0**: ARM Cortex-M4F (primary core) -- boots, manages lifecycle,
  publishes callbacks
- **CPU1**: RISC-V RV32 IMC (coprocessor) -- performs work, signals
  completion via doorbell

Architecture
------------

.. code-block:: text

   ┌───────────────────────────────────────────────────────────────────┐
   │         MAX78000 SoC (Unified 0x10000000–0x1007FFFF Flash)        │
   ├───────────────────────────────────────────────────────────────────┤
   │ CPU0 (ARM)       │      Shared Resources        │  CPU1 (RISC-V)  │
   │ ┌─────────────┐  │  ┌─────────────────────────┐ │  ┌──────────┐   │
   │ │  Cortex-M4F │  │  │ SEMA (0x4003E000)       │ │  │  RV32    │   │
   │ │             │  │  │ ├─ Semaphores           │ │  │          │   │
   │ │ - Boots     │  │  │ ├─ Doorbells (irq0/1)   │ │  │ - Waits  │   │
   │ │ - Publishes │  │  │ └─ Mailboxes            │ │  │ - Reads  │   │
   │ │   callback  │──┼──┤ ┌──────────────────────┐│ ├──┤ - Echoes │   │
   │ │ - Sleeps    │  │  │ │ IPC Shared Memory    ││ │  │ - Notif. │   │
   │ │   on WFI    │  │  │ │ (SRAM2 0x20011000)   ││ │  │          │   │
   │ └─────────────┘  │  │ └──────────────────────┘│ │  └──────────┘   │
   │                  │  └─────────────────────────┘ │                 │
   └───────────────────────────────────────────────────────────────────┘

Key Files
---------

- ``dual_core_example.c`` -- ARM side: initialization, boot, callback
  publishing, wait loop
- ``dual_core_example_riscv.c`` -- RISC-V side: boot, IPC table read,
  doorbell notify
- ``dual_core_example.h`` -- ARM example header

Usage
-----

Configuration
~~~~~~~~~~~~~

Enable the dual-core example (run from the no-OS repository root):

.. code-block:: bash

   cmake --preset max78000fthr \
     -DPROJECT_DEFCONFIG=max78000/dual_core.conf

   cmake --build out/build/max78000fthr

Or manually enable the options in any defconfig:

.. code-block:: bash

   CONFIG_COPROCESSOR=y
   CONFIG_SEMA_MAXIM=y
   CONFIG_COPROCESSOR_MAXIM=y
   CONFIG_MAX78000_DUAL_CORE_EXAMPLE=y

Expected Output (ARM Console)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

See the `Smoke Test`_ section below for the full, verified console output.

Flow
----

Initialization Phase
~~~~~~~~~~~~~~~~~~~~~

1. **ARM boots** (main.c -> common_data.c -> dual_core_example.c), selects
   the 100 MHz IPO system clock
2. **ARM initializes SEMA** -> enables hardware semaphores and doorbells
3. **ARM initializes coprocessor** -> allocates CPU1 descriptor
4. **ARM boots RISC-V** -> clears ``FCR->urvctrl``, sets
   ``FCR->urvbootaddr = &_riscv_boot`` (a flash address), pulses CPU1 reset.
   The RISC-V image is already resident in flash (embedded in the ARM ELF);
   it runs in place -- no copy to SRAM.
5. **RISC-V starts executing** -> the RISC-V startup sets mtvec/gp/sp, copies
   .data / zeroes .bss (into SRAM2); ``main()`` enables the RISC-V
   instruction cache, then reads the IPC table and spins until ready

IPC Handshake
~~~~~~~~~~~~~

.. code-block:: c

   /* ARM side publishes callback (fixed-address pointer to SRAM2 0x20011000) */
   g_ipc_table->magic = 0;                        // invalidate
   g_ipc_table->fn[0] = (uint32_t)callback_fn;    // write address
   g_ipc_table->magic = MAXIM_IPC_MAGIC;          // validate
   g_ipc_table->ready = 1;                        // release
   __DSB();                                       // ARM dsb sy (fine on Cortex-M)

   /* RISC-V side waits and reads (same fixed-address pointer). No fence: see the
    * "No Data Cache" note -- the RV core's `fence` is avoided here. */
   while (g_ipc_table->magic != MAXIM_IPC_MAGIC || !g_ipc_table->ready)
       delay_ms(10);                              // spin until ready
   uint32_t addr = g_ipc_table->fn[0];

Event Loop (10-round 1:1 handshake)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. **RISC-V** -> echoes callback address to ``MXC_SEMA->mail1``, rings
   doorbell (sets ``CM4_IRQ`` in ``MXC_SEMA->irq0``)
2. **ARM** -> polls ``CM4_IRQ``, reads ``mail1``, acknowledges (clears
   ``CM4_IRQ``), dispatches the echoed callback, and counts the event
3. **RISC-V** -> waits for the ARM to clear ``CM4_IRQ`` before ringing again
   (guarantees every notification is seen exactly once), then repeats
4. Both finish after 10 acknowledged events

Implementation Notes
--------------------

No Data Cache
~~~~~~~~~~~~~

Both cores have **no data cache** (ICC0/ICC1 are instruction-only) -> shared
memory is automatically coherent; synchronization is ``volatile`` + the SEMA
doorbell.

- ARM's ``__DSB()`` / ``no_os_barrier_full()`` (``dsb sy``) is used for
  explicit ordering.
- The RISC-V side issues **no** memory barriers. On this PULP-derived RV32
  core a ``fence iorw,iorw`` corrupts the immediately-following load, so
  ``no_os_barrier_full()`` is intentionally not called from the RISC-V side --
  ``volatile`` accesses plus the doorbell handshake order the exchange
  correctly.

Function Pointer Communication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The IPC table stores function addresses as ``uint32_t`` (not typed C
pointers):

- **Why**: Avoids cross-arch ABI issues (Thumb bit on ARM, different calling
  conventions)
- **Pattern**: ARM publishes address -> RISC-V reads it -> RISC-V echoes back
  via doorbell -> ARM dispatches from its own ISR (stays on ARM core, avoids
  remote function calls)

Memory Layout & IPC Table Placement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- RISC-V **code** runs in place from **flash** (at ``_riscv_boot``); its
  **data/bss/stack** live in SRAM2 (per the stock ``max78000_riscv.ld``).
  SRAM3 is unused by this build.
- The IPC table lives at the fixed SRAM2 address **``0x20011000``**, reached
  by **both** cores through an identical fixed-address pointer
  (``(maxim_ipc_table_t *)0x20011000``) rather than a linker-placed object --
  this is what makes the two cores agree on the table location.
- It must be in SRAM2, **not** SRAM3: when the RISC-V runs from flash, SRAM3
  (the "RV Code" region) is not accessible to the RV core as data.
  ``0x20011000`` sits above the RV ``.data``/``.bss`` (~``0x20010004``) and
  well below the RV stack limit (``0x2001B000``).

Build Integration
-----------------

The RISC-V firmware is cross-compiled and embedded into the ARM ELF by the
no-OS platform helper ``maxim_add_riscv_coprocessor()`` (in
``cmake/maxim/maxim_coprocessor.cmake``). This project just names the RISC-V
source:

.. code-block:: cmake

   include(maxim_coprocessor)
   maxim_add_riscv_coprocessor(max78000
       SOURCES  .../dual_core_example_riscv.c
       INCLUDES ...)

The helper runs a **two-pass** build (the RISC-V must be linked at its flash
load address, which is only known after the ARM is linked):

1. **Premap:** link a twin of the ARM target (same sources, no RISC-V blob)
   and read the ``_riscv_boot`` flash address from its map.
2. Derive ``buildrv/common_riscv.ld`` (``__FlashStart``/``__FlashLength``)
   from it.
3. Compile the RISC-V app + the stock RISC-V startup/system, link at
   ``_riscv_boot`` -> ``riscv.bin``.
4. Wrap ``riscv.bin`` (``.incbin``) into ``riscv.o``.
5. **Final:** link the ARM ELF with ``riscv.o``. The platform's dual-core ARM
   linker script (``drivers/platform/maxim/max78000/max78000_dualcore_arm.ld``)
   ``KEEP``s ``riscv.o`` at ``.riscv_flash``/``_riscv_boot``, excludes it from
   ARM ``.text``, and confines the ARM stack to SRAM0/1 so it cannot trample
   the RISC-V's SRAM2.

The image rides inside the single ARM ELF that gets flashed -- no separate
flash invocation.

Future Enhancements
-------------------

Real Work Examples
~~~~~~~~~~~~~~~~~~

In production, the RISC-V side would do useful work:

- **CNN inference**: run inference on the neural network accelerator while ARM
  sleeps
- **DSP processing**: audio/signal processing in the background
- **Autonomous tasks**: RTC alarms, periodic sensors, power-down sequencing

Replace the ``delay_ms()`` + doorbell loop with your actual workload.

Testing
-------

Smoke Test
~~~~~~~~~~

Build and flash to MAX78000FTHR:

.. code-block:: bash

   python tools/scripts/no_os_build.py build \
     --project max78000 --variant dual_core --board max78000fthr \
     --probe openocd --flash --clean

Then open the USB-UART console (e.g. ``/dev/ttyACM0``, 115200 8N1). Expected
output:

.. code-block:: text

   ========================================
   MAX78000 Dual-Core Example (ARM Side)
   ========================================
   [ARM] RISC-V firmware embedded in flash at 0x1000d200
   [ARM] Initializing IPC (doorbells + mailboxes)...
   [ARM] IPC initialized
   [ARM] Initializing coprocessor (CPU1)...
   [ARM] Booting coprocessor (CPU1)...
   [ARM] Coprocessor booted successfully
   [ARM] Callback address published to RISC-V (0x...)

   [ARM] Waiting for RISC-V events...

     [ARM] RISC-V event callback invoked
   [ARM] Wakeup 1/10 (callback 0x...)
     ...
   [ARM] Wakeup 10/10 (callback 0x...)
   [ARM] Example complete (10 wakeups received)
   [ARM] Halting coprocessor...
   [ARM] Dual-core example finished

This is a **verified, working** dual-core flow: the RISC-V image is embedded
in flash inside the ARM ELF, the ARM boots CPU1 to run in place from flash,
and the two cores complete a 10-round doorbell handshake through the shared
IPC table.

How the RISC-V image is embedded and booted
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. The RISC-V source + the RISC-V startup/system are cross-compiled with
   ``riscv-none-elf-gcc`` and linked with ``max78000_riscv.ld`` to run in
   place from **flash** at ``_riscv_boot`` (see the two-pass Build
   Integration).
2. The built ``.bin`` is ``.incbin``-wrapped into ``riscv.o``, which the ARM
   linker ``KEEP``s in the ``.riscv_flash`` section at ``_riscv_boot``.
3. At boot the coprocessor driver clears ``FCR->urvctrl`` (select flash), sets
   ``FCR.urvbootaddr = &_riscv_boot``, and pulses ``GCR.rst1.CPU1`` to release
   CPU1.
4. The RISC-V startup sets mtvec/gp/sp, copies ``.data`` / zeroes ``.bss``
   into SRAM2; ``main()`` then enables the RISC-V instruction cache and runs.

Debug Hints
~~~~~~~~~~~

- **RISC-V not waking ARM**: Check the IPC table at ``0x20011000`` -- offset 0
  (``magic``) must read ``0xCAFEFEED`` and offset 68 (``ready``) must be 1.
  Both cores must use the fixed ``0x20011000`` (SRAM2), not a linker-placed
  section, and not SRAM3 (inaccessible to the RV when it runs from flash).
- **SEMA registers**: Check ``MXC_SEMA->irq0`` -- the ``CM4_IRQ`` bit is the
  doorbell from RISC-V; ``mail1`` carries the echoed callback address.
- **RISC-V code not running**: Verify ``FCR->urvctrl == 0`` and
  ``FCR->urvbootaddr == &_riscv_boot``, and that the bytes at ``_riscv_boot``
  in flash match ``riscv.bin``. A RISC-V trap with no ``mtvec`` (not an issue
  with the stock startup) wedges the AHB bus -- recover by racing
  ``reset halt`` + ``flash erase_sector 0 0 last`` before the bad image runs.
- **RISC-V reads garbage from the table**: do not issue
  ``fence``/``no_os_barrier_full()`` on the RISC-V side -- on this core the
  fence corrupts the following load.

References
----------

- ``include/no_os_coprocessor.h`` -- Generic coprocessor API
- ``include/no_os_ipc.h`` -- Generic IPC (doorbell + mailbox) API
- ``drivers/platform/maxim/max78000/maxim_coprocessor.c`` -- CPU1 boot/halt
- ``drivers/platform/maxim/max78000/maxim_ipc.{c,h}`` -- SEMA doorbell/mailbox
  IPC
- ``drivers/platform/maxim/max78000/maxim_ipc_shared.h`` -- Shared table layout
