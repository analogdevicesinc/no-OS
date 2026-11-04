CAPI Selftest
=============
.. no-os-doxygen::

.. contents::
	:depth: 3

Overview
--------

``capi_selftest`` exercises the CAPI (Common API) platform abstraction layer
against real hardware. Each peripheral is driven through its CAPI contract only
-- never the vendor BSP directly - so the same test sources run unchanged on
every platform that provides a CAPI backend (Xilinx, STM32, ...). The tests are
integration tests, not unit tests: a GPIO edge really fires an interrupt, a SPI
transfer really loops MOSI back to MISO, an I2C initiator really addresses a
second controller wired up as a target, and so on.

The project ships two examples, selected by ``PROJECT_DEFCONFIG``:

* ``basic.conf`` - the **basic** example (``src/examples/basic``). UART only:
  it brings up the console UART and runs the test framework over it. Use it to
  confirm the most fundamental wiring - that the board boots, the UART is
  mapped correctly and characters reach your terminal - before trusting any
  richer result. If ``basic`` does not print, nothing else will.

* ``loopback.conf`` - the **capi_loopback** example
  (``src/examples/capi_loopback``). The full self-test suite: GPIO, IRQ, SPI,
  timer, I2C, UART and DMA, each a table of subtests run back-to-back with a
  summary at the end.

Both examples share ``src/common`` (the test framework and ``common_data``) and
a per-platform ``src/platform/<plat>/`` directory that supplies ``main.c`` and
``parameters.h``. ``parameters.h`` is the only file that knows the board: it
maps each peripheral to a CAPI backend and publishes the capability macros the
tests gate on.

What gets tested
----------------

The loopback suite runs seven groups, in this order, each a table of subtests
driven by ``test_framework_run_cases()``. A subtest is either run, or reported
as a SKIP with a category - it never silently disappears.

Each group below lists what it proves, then which subtests always run and which
are conditional. A conditional subtest runs only when its gate macro is set;
otherwise it reports as a SKIP.

``GPIO`` - output port drives an input port over the wire, read back
	* Always: ``LOOPBACK`` (value), ``PORT_ACTIVE_LOW``, ``PIN_TOGGLE``.
	* ``PORT_TOGGLE`` - gated on ``GPIO_HAS_TOGGLE``.
	* ``PIN_LOOPBACK`` - gated on ``GPIO_HAS_PIN_LOOPBACK``.

``IRQ`` - a real GPIO edge reaches a connected callback
	Whole group gated on ``IRQ_CTRL_IDENTIFIER``.

	* ``FIRE_AND_LISTEN`` (edge delivered), ``ENABLE_GATES`` (enable/disable
	  masks it), ``RECONNECT`` (re-point the callback), ``REPEATED_EDGES``
	  (N pulses deliver N events, scaled by ``GPIO_IRQ_EVENTS_PER_EDGE``).

``SPI`` - external MOSI->MISO loopback
	* Always: ``BASIC``, ``MODES`` (all four), ``LSB_FIRST``, ``DATA``.
	* ``ASYNC_IRQ``, ``MANUAL_ISR``, ``ABORT_IRQ`` - gated on ``SPI_HAS_IRQ``.
	* ``ASYNC_DMA``, ``ABORT_DMA`` - gated on ``SPI_HAS_DMA``.

``TIMER`` - counter advances; rate cross-checks against the uptime clock
	* Always: ``BASIC`` (rate within tolerance).
	* ``COMPARE`` - gated on ``TIMER_HAS_COMPARE``.
	* ``ASYNC_IRQ``, ``IRQ_RATE`` (overflow interrupt fires at expected rate)
	  - gated on ``TIMER_HAS_IRQ``.

``I2C`` - initiator addresses a second controller wired as a target
	Without a target mapped the whole group collapses to one skipping ``BASIC``.

	* Always (target mapped): ``BASIC``, ``DATA``, ``READDRESS``, ``SUBADDR``
	  (register read), ``BUS_SPEED`` (on-wire slow/fast), ``TARGET_TX``.
	* ``MASTER_ASYNC`` - gated on ``I2C_MASTER_ASYNC`` (async initiator).

``UART`` - a second UART strapped TX->RX, never the console
	* Always: ``BASIC`` (sync echo).
	* ``ASYNC_BASIC`` - gated on ``UART_ASYNC_HAS_IRQ``.
	* ``ASYNC_SPEED`` (baud-change timing) - also needs
	  ``UART_ASYNC_HAS_LINE_CONFIG``.
	* ``ASYNC_IRQ`` (per-source IRQ masking) - also needs
	  ``UART_ASYNC_HAS_IRQ_CTL``.

``DMA`` - memory-to-memory
	Whole group gated on ``DMA_OPS``.

	* ``BASIC`` (copy), ``PATTERN`` (fill/verify).

Build
-----

Builds go through ``tools/scripts/no_os_build.py``, which discovers every valid
project/variant/board combination from the CMake presets and the project's
``boards/<variant>/*.conf`` files, then runs configure and build for each. Each
combination gets its own directory under ``build/`` named
``capi_selftest-<variant>-<board>``, so nothing clobbers anything else.

To see what this project can be built for:

.. code-block:: bash

	python tools/scripts/no_os_build.py list --project capi_selftest

.. code-block:: text

	PROJECT        VARIANT   BOARD          PLATFORM
	─────────────  ────────  ─────────────  ────────
	capi_selftest  basic     coraz7s        xilinx
	capi_selftest  basic     max32657evkit  maxim
	capi_selftest  basic     nucleo-f767zi  stm32
	capi_selftest  basic     sdp-ck1z       stm32
	capi_selftest  loopback  coraz7s        xilinx
	capi_selftest  loopback  max32657evkit  maxim
	capi_selftest  loopback  nucleo-f767zi  stm32
	capi_selftest  loopback  sdp-ck1z       stm32

The two variants are:

* ``loopback`` - the full seven-group suite; needs the straps in the wiring
  section for the board.
* ``basic`` - a UART-only smoke test. Build this first on a new board to prove
  the console and the toolchain before wiring anything.

Omitting ``--variant`` or ``--board`` builds every match, which is the quickest
way to check a change did not break another board:

.. code-block:: bash

	# one board, both variants
	python tools/scripts/no_os_build.py build --project capi_selftest --board sdp-ck1z

	# one combination
	python tools/scripts/no_os_build.py build --project capi_selftest \
	    --variant loopback --board nucleo-f767zi

	# build and program the attached board
	python tools/scripts/no_os_build.py build --project capi_selftest \
	    --variant loopback --board nucleo-f767zi --probe openocd --flash

Useful flags: ``-j N`` for parallel compilation, ``--parallel`` to build
different boards concurrently (flashing stays serialized - boards share a
probe), ``--clean`` / ``--fresh`` to discard a stale build directory, and
``--dry-run`` to print the cmake commands without running them.

The suite is designed to compile and link against **any** BSP. A peripheral the
hardware design does not expose simply drops out of the build (see
`How a test is skipped`_), so the same sources cover every board above with no
source edits.

Xilinx builds additionally need a hardware definition, since the ``.xsa``
decides which peripherals and interrupts the BSP exposes. Pass it with
``--hardware``:

.. code-block:: bash

	MODE=gic        # gic | intc | noirq

	python tools/scripts/no_os_build.py build --project capi_selftest \
	    --variant loopback --board coraz7s \
	    --hardware projects/capi_selftest/coraz7s_$MODE.xsa

Three ``.xsa`` files ship with the project, and **the same sources cover all
three with no source edits** (see `Which IP per build`_):

* ``gic`` - interrupts through the GIC.
* ``intc`` - fabric interrupts cascaded through an AXI INTC into the GIC.
* ``noirq`` - no fabric interrupt wired; async paths skip, sync still runs.

Because the build directory name does not include the ``.xsa``, pass
``--fresh`` when switching modes, or give each mode its own base directory with
``--build-dir build-$MODE``.

Other Xilinx presets exist (``zed``, ``zc702``, ``zc706``, ``zcu102``,
``kcu105`` ...); ``zed_{gic,intc,noirq}.xsa`` mirror the three-build split on a
ZedBoard.

Wiring - Cora Z7-07S
--------------------

All three builds use the **same PMOD straps** - only the IP behind each slot
changes (see `Which IP per build`_). Wire the board once, reflash freely.
Console is PS UART0 on the USB-UART; set jumper **JP3 to USB**.

.. list-table::
	:header-rows: 1
	:widths: 22 30 48

	* - Loopback
	  - Strap
	  - Notes
	* - GPIO
	  - JA7 (out) to JA8 (in)
	  - One pin drives, the adjacent one reads back.
	* - SPI
	  - JB7 (MOSI) to JB8 (MISO)
	  - SCLK (JB3) and CS (JB4) are driven, not strapped - only the data pair
	    loops back.
	* - UART async
	  - JA1 (TX) to JA2 (RX)
	  - The second UART, never the console; its own TX feeds its own RX.
	* - I2C
	  - JA9-JB1 (SDA), JA10-JB2 (SCL)
	  - Two controllers on one bus: one is the initiator, the other the target.

Which IP per build
~~~~~~~~~~~~~~~~~~

The build is keyed to its interrupt mode, auto-detected from the BSP (override
with ``-DIRQ_SEL_GIC`` / ``-DIRQ_SEL_CASCADE`` / ``-DIRQ_SEL_NOIRQ``). The
straps above never move; the grid only says which silicon answers at each slot.
Read down a column for one build, across a row to see how a peripheral moves.

.. list-table::
	:header-rows: 1
	:widths: 28 24 24 24

	* - Peripheral
	  - ``gic``
	  - ``intc``
	  - ``noirq``
	* - GPIO
	  - PS
	  - PL
	  - PS
	* - SPI
	  - PL
	  - PS
	  - PL
	* - I2C initiator
	  - PL
	  - PS
	  - PL
	* - I2C target
	  - PS
	  - PL
	  - PS
	* - Timer
	  - TTC
	  - AXI
	  - SCU
	* - UART async
	  - NS550
	  - Lite
	  - PS UART

Reading the columns:

* **gic** - every async path is live through the GIC.
* **intc** - the fabric peripherals' interrupts arrive via an AXI INTC cascaded
  into the GIC. UART async is UART Lite, whose line format is fixed in the IP, so
  ``ASYNC_SPEED`` and ``ASYNC_IRQ`` skip.
* **noirq** - no fabric interrupt is wired, so the fabric peripherals' async
  subtests SKIP (sync still runs). The SCU timer and PS UART keep their own GIC
  lines, so timer and UART async still fire.

The two-way peripherals (GPIO, SPI, I2C) swap PS<->PL between ``gic`` and
``intc``; ``noirq`` mirrors ``gic``. The three-way ones (timer, UART) take a
third option under ``noirq`` - the SCU timer and the PS UART, both of which keep
a GIC line even with no fabric interrupt.

Not physically strapped
~~~~~~~~~~~~~~~~~~~~~~~~

Three groups need no jumper - their loopback is internal to the SoC or the BSP:

* **IRQ** - reuses the GPIO strap. The GPIO output pin is driven, the wired
  input pin is armed as an interrupt source; no separate wiring. The whole
  group gates on ``IRQ_CTRL_IDENTIFIER`` (the GIC, always present on Zynq), and
  each case skips at runtime (``SKIP_IRQ_ABSENT``) when the build wired no
  fabric GPIO interrupt.
* **Timer** - a free-running counter and its overflow interrupt, entirely on
  chip. Nothing external to connect; the rate check times it against the
  independent uptime clock.
* **DMA** - memory-to-memory, no peripheral and no pins. (Not mapped on Cora
  today; present on STM32.)

Wiring - STM32
--------------

Two STM32 boards are mapped, and between them they cover both SPI async
delivery modes: NUCLEO-F767ZI delivers async completion by SPI interrupt,
SDP-K1 delivers it by DMA. ``src/platform/stm32/parameters.h`` selects the
mapping from the CMSIS device macro the CubeMX BSP defines, so the board
follows from ``--board`` and its ``.ioc`` with nothing else to pass down.

.. code-block:: bash

	python tools/scripts/no_os_build.py build --project capi_selftest \
	    --variant loopback --board nucleo-f767zi

	python tools/scripts/no_os_build.py build --project capi_selftest \
	    --variant loopback --board sdp-ck1z

NUCLEO-F767ZI
~~~~~~~~~~~~~

USART3 is the console (ST-LINK virtual COM port) and is never used by a test.
CubeMX only generates MSP code for I2C1, so ``i2c_platform_init.c`` brings up
I2C2's clock, pins and NVIC by hand - the platform hooks here are real, not
no-ops.

.. list-table::
	:header-rows: 1
	:widths: 16 26 58

	* - Group
	  - Peripheral
	  - Strap / notes
	* - GPIO
	  - PE0 -> PC0
	  - Jumper PE0 (CN10/D34) to PC0 (CN9, Arduino A1). No toggle op. Both
	    endpoints are bit 0 of their port, so the port-wide cases run too.
	    PC0 routes to EXTI0 for the IRQ group.
	* - SPI
	  - SPI1
	  - Jumper PA7 (MOSI) to PA6 (MISO); PA5 is SCK. Async delivered by the
	    SPI interrupt (``SPI_HAS_IRQ``), not DMA.
	* - I2C
	  - I2C1 <-> I2C2
	  - Jumper PB6/PB9 (I2C1) to PB10/PB11 (I2C2). Polled (``I2C_HAS_IRQ`` 0).
	* - IRQ / Timer / DMA
	  - NVIC / TIM2 / DMA2
	  - No strap. IRQ reuses the GPIO edge; TIM2 is a 32-bit on-chip counter
	    (1 us resolution); DMA2 Stream0 is memory-to-memory, polled.

SDP-K1 (EVAL-SDP-CK1Z)
~~~~~~~~~~~~~~~~~~~~~~

UART5 is the console (on-board USB virtual COM port) and is never used by a
test. Every strap lands on the **Arduino Uno header**, so no SDP-120 breakout is
needed; the pin names below are the silkscreened Arduino labels with the
STM32F469 port/pin in parentheses.

.. list-table::
	:header-rows: 1
	:widths: 16 26 58

	* - Group
	  - Peripheral
	  - Strap / notes
	* - GPIO
	  - D5 (PA11) -> D2 (PG7)
	  - Jumper D5 to D2. Pin-loopback only (``GPIO_HAS_PORT_LOOPBACK`` 0):
	    neither endpoint is bit 0 of its port, so a port-wide write would
	    reach pins that are not strapped. PG7 routes to EXTI7 (shared
	    EXTI9_5 vector) for the IRQ group.
	* - SPI
	  - SPI1
	  - All three signals are on **DIGI1 (P6)**: jumper pin 4 (D11 / PA7,
	    MOSI) to pin 5 (D12 / PB4, MISO) - they are adjacent, so a single
	    2-pin shunt does it. Pin 6 (D13 / PB3) is SCK and stays open. Async
	    delivered by **DMA** (``SPI_HAS_DMA``) - this is the board that
	    exercises the SPI DMA path.
	* - I2C
	  - not mapped
	  - I2C1 is on DIGI1 (P6) pins 9/10 (SDA/SCL, pulled up 2.2k to
	    MAIN_PWR_SUPPLY), but I2C2's pins are taken by the USB-HS ULPI PHY
	    and I2C3 is reachable only through the SDP-120 connector, so no
	    initiator/target pair can be strapped from the Arduino header. The
	    whole I2C group reports NOT_CONFIGURED.
	* - IRQ / Timer / DMA
	  - NVIC / TIM2 / DMA2
	  - No strap. IRQ reuses the GPIO edge; TIM2 is a 32-bit on-chip counter
	    (1 us resolution). DMA2 is shared: stream 0 serves the
	    memory-to-memory group, streams 2 and 3 carry SPI1 RX/TX.

Wiring - MAX32657 (MAX32657EVKIT)
---------------------------------

Same groups, on-chip peripherals. All async paths are delivered through the
NVIC (IRQ/FIFO); DMA is memory-to-memory only.

.. list-table::
	:header-rows: 1
	:widths: 16 26 58

	* - Group
	  - Peripheral
	  - Strap / notes
	* - GPIO
	  - P0.7 -> P0.8
	  - Jumper P0.7 (output) to P0.8 (input). Remove the on-board JP15 jumpers
	    first so nothing else drives the pair. Pin-loopback only
	    (``GPIO_HAS_PORT_LOOPBACK`` 0); P0.8 is also armed as the IRQ source.
	* - SPI
	  - SPI0
	  - Jumper P0.2 (MOSI) to P0.4 (MISO); P0.6 is SCLK, driven not strapped.
	    IRQ/FIFO delivery, no DMA.
	* - I2C
	  - I2C0
	  - Initiator only - no second controller is wired as a target, so the whole
	    I2C group collapses to one skipping ``BASIC``.
	* - IRQ / Timer / DMA
	  - NVIC / TIM0 / DMA
	  - No strap. IRQ reuses the P0.8 GPIO edge; TIM0 is a 32-bit on-chip counter
	    (1 us resolution); DMA is memory-to-memory.

UART0 is the console only; the second (async) UART is not mapped, so the whole
UART group skips.

How a test is skipped
---------------------

There are three independent skip mechanisms, from coarsest to finest. Prefer
the coarsest one that fits: a peripheral that is absent from the hardware should
disappear at compile time, not fail at runtime.

1. Compile out an entire group - **do not map its ops**
	Each test group is gated on the mapping macros that ``common_data``
	publishes from ``parameters.h``. If the backend is not mapped, the group's
	data and body are ``#ifdef``-ed out and the group's entry point collapses
	to a stub that returns ``0``.

	* Peripheral groups gate on ``<MODULE>_OPS`` - ``GPIO_OUTPUT_OPS``,
	  ``SPI_OPS``, ``TIMER_OPS``, ``I2C_OPS``, ``UART_ASYNC_OPS``, ``DMA_OPS``.
	  Leave the ``*_OPS`` (and the identifier/extra) undefined in
	  ``parameters.h`` and the group is gone.
	* The IRQ group is special: it gates on ``IRQ_CTRL_IDENTIFIER``. If the
	  BSP maps no interrupt controller, leave ``IRQ_CTRL_IDENTIFIER``
	  undefined and ``test_irq.c`` compiles to a skipping stub.

	The recommended pattern in ``parameters.h`` is to auto-detect the backend
	from the BSP's ``XPAR_..._NUM_INSTANCES`` macros and only define the
	mapping when an instance exists, so an absent peripheral leaves its
	``*_OPS`` naturally undefined.

2. Skip a single subtest at compile time - **the** ``skip`` **flag**
	Every entry in a group's ``struct test_case`` table carries a ``skip``
	bool. Set it from a capability macro to drop just that subtest while the
	rest of the group still runs. For example the GPIO table gates its
	pin-level cases on ``GPIO_HAS_PIN_LOOPBACK``::

		static const struct test_case gpio_subtests[] = {
			{ "LOOPBACK", gpio_loopback, !GPIO_HAS_PORT_LOOPBACK },
		#if GPIO_HAS_PIN_LOOPBACK
			{ "PIN_LOOPBACK", gpio_pin_loopback, !GPIO_HAS_PIN_LOOPBACK },
		#endif
		};

	A ``skip == true`` case is reported as ``SKIP_FEATURE_DISABLED``; a case
	whose ``run`` pointer is ``NULL`` is reported as ``SKIP_NOT_IMPLEMENTED``.

	This is the mechanism that makes the polled ("noirq") build behave: the
	fabric peripherals' ``*_HAS_IRQ`` macros come out 0, so their async
	subtests SKIP instead of running against a controller that would reject
	them with ``-ENOTSUP``. **The capability macro must be derived from the
	same** ``XPAR_*_INTERRUPTS`` **macro that decides** ``use_irq``, or the two
	disagree and a SKIP turns into a spurious FAIL.

3. Skip at runtime - ``TEST_SKIP_CAT``
	When absence can only be discovered at runtime (a hook reports the board
	has no path for this test, an optional feature is off), call
	``TEST_SKIP_CAT(category, message)`` and return. The categories live in
	``test_framework.h``: ``SKIP_HW_ABSENT``, ``SKIP_IRQ_ABSENT``,
	``SKIP_BSP_ABSENT``, ``SKIP_UNSAFE``, ``SKIP_NOT_IMPLEMENTED``,
	``SKIP_BOARD_STATE``, ``SKIP_FEATURE_DISABLED``. The IRQ suite uses this:
	on a GIC-only build with no fabric GPIO interrupt the arm hook returns
	``-ENOTSUP`` and each case skips with ``SKIP_IRQ_ABSENT`` rather than
	failing.

.. note::

	The ``skip_all`` field on the **top-level** registry in ``capi_loopback.c``
	(``struct test_entry``) is **not** a hardware-capability switch. It exists
	only for personal, ad-hoc testing - flip it to ``true`` to temporarily
	omit a whole group from the default all-run while you focus on another.
	Leave every entry ``false`` in committed code; real "this hardware does not
	have it" skipping belongs in mechanisms 1-3 above, which are driven by the
	BSP, not by a hand-edited flag.

Extending the suite
-------------------

To add a new peripheral test group:

1. **Write the test** in ``src/examples/capi_loopback/tests/test_<mod>.c`` with
   a matching ``.h`` declaring ``int test_<mod>(void);``. Drive only the CAPI
   ``capi_<mod>_*`` API - no vendor calls in the test body. Put board- or
   vendor-specific setup behind platform hooks (see ``main.c`` and the IRQ
   arm/ack/disarm hooks in ``common_data.h``) so the test stays
   platform-agnostic.

2. **Gate the whole file** on the mapping macro so it compiles everywhere::

	#ifdef <MODULE>_OPS
	... tests, table, test_<mod>() ...
	#else
	int test_<mod>(void) { return 0; }
	#endif

3. **Table-drive the subtests** with ``struct test_case`` and run them through
   ``test_framework_run_cases()``. Use the ``skip`` flag for optional-feature
   subtests and ``TEST_SKIP_CAT`` for runtime-discovered absence.

4. **Publish the mapping** in every ``src/platform/<plat>/parameters.h``:
   ``<MODULE>_IDENTIFIER``, ``<MODULE>_OPS``, and any ``<MODULE>_EXTRA_TYPE`` /
   ``<MODULE>_EXTRA_INIT`` the backend needs. Auto-detect the backend from the
   BSP so an absent peripheral leaves ``*_OPS`` undefined and the group drops
   out. Derive every ``*_HAS_*`` capability macro from the same interrupt macro
   that sets ``use_irq``. Consume it in ``src/common/common_data.h`` under
   ``#ifdef <MODULE>_OPS``, giving each tunable an ``#ifndef`` default there so a
   platform overrides only what it must in its ``parameters.h``.

5. **Register the group** in ``capi_loopback.c``: add ``#include
   "tests/test_<mod>.h"`` and a ``{ 0, "<MOD>", test_<mod>, false }`` row in the
   ``tests[]`` table. Keep ``skip_all`` ``false``.

Rules
-----

* CAPI only in test bodies. Anything vendor-specific goes behind a platform hook.
* Absent hardware must never fail. Compile it out (undefined ``*_OPS`` /
  ``IRQ_CTRL_IDENTIFIER``), gate the subtest, or skip at runtime with a category.
  A FAIL on a board that lacks the peripheral means the gate is wrong - fix the
  gate, not the test.
* Every subtest is visited even if an earlier one fails; the first non-zero
  return is propagated so the end-of-run summary stays complete.
* ``skip_all`` in the registry is a developer convenience, not a capability gate
  - committed code keeps it ``false``.
