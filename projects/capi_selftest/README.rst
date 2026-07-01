CAPI Self-Test no-OS Project
============================

.. no-os-doxygen::

.. contents::
	:depth: 3

Overview
--------

``capi_selftest`` is a platform-agnostic no-OS validation project for CAPI
drivers. It builds normal no-OS examples, runs them on the target, and prints
structured test results through the configured UART.

The project is split into three layers so that platform differences stay
isolated from the tests themselves:

* ``src/common`` — platform-independent test data and the lightweight test
  framework.
* ``src/platform/<platform>`` — maps board/BSP identifiers to CAPI
  configuration objects.
* ``src/examples/<example>`` — test suites that exercise the CAPI APIs using
  only the objects from ``src/common``.

Supported Evaluation Boards
---------------------------

* ZedBoard (Xilinx Zynq-7000)

Supported Examples
------------------

``basic``
^^^^^^^^^

Proves that the CAPI UART path and the test framework are alive. It
initializes the platform UART, emits a hello message, checks a few
non-destructive UART calls, exercises framework pass/range/skip reporting, and
then deinitializes the UART.

``capi_loopback``
^^^^^^^^^^^^^^^^^

The full self-test runner. It initializes the console UART and (when the
build enables it) the IRQ controller, then runs each registered test group in
turn: GPIO, SPI and timer. Groups are listed in a table in
``capi_loopback.c`` and executed by ``run_all()``; a failing group is recorded
but does not stop the others, so a single run always produces a complete tally.

How the Framework Works
-----------------------

The framework is a small, allocation-free reporting and counting layer around
plain C functions. There are three nesting levels:

* **Group** — a ``test_<peripheral>()`` entry point (e.g. ``test_spi``),
  registered in the ``capi_loopback.c`` table.
* **Subtest** — one ``static int`` function per behavior, returning ``0`` on
  pass and negative on failure. A group runs its subtests through a
  ``struct test_case`` table and ``test_framework_run_cases()``.
* **Assertion** — ``TEST_ASSERT_*`` macros that record a pass/fail line and
  ``return`` from the subtest on failure.

A group is just::

    static const struct test_case spi_subtests[] = {
        { "BASIC",       spi_basic,       false        },
        { "ASYNC_IRQ",   spi_async_irq,   !SPI_HAS_IRQ },  /* gated */
        { "ERROR_PATHS", spi_error_paths, false        },
    };

    int test_spi(void)
    {
        return test_framework_run_cases("SPI", spi_subtests,
                                        sizeof(spi_subtests) /
                                        sizeof(spi_subtests[0]));
    }

``test_framework_run_cases()`` visits every entry:

* ``skip == true`` → reported as a ``FEATURE_DISABLED`` skip.
* ``run == NULL``  → reported as a ``NOT_IMPLEMENTED`` skip.
* otherwise        → ``run()`` is called.

The third ``struct test_case`` field is the **capability gate**. It is wired to
a ``*_HAS_*`` macro from ``parameters.h`` (e.g. ``!SPI_HAS_IRQ``,
``!TIMER_HAS_CAPTURE``), so the same test source covers builds and boards that
lack a feature: the case is honestly reported as skipped instead of being
omitted or failing. Every case is still visited on failure, so the final
``[RUN:SUMMARY]`` line is always complete.

Useful assertion macros (all record a line and return ``-1`` on failure):

* ``TEST_ASSERT_EQ(actual, expected, name)`` — exact match / return code.
* ``TEST_ASSERT_NEAR(actual, expected, tolerance, name)`` — value within an
  absolute tolerance (used by the timer rate check).
* ``TEST_ASSERT_RANGE`` / ``TEST_ASSERT_GT`` / ``TEST_ASSERT_COUNTER_DIR`` —
  bounds and counter-direction checks.
* ``TEST_VALUE(name, value)`` — emit a diagnostic ``[VAL:...]`` line.
* ``TEST_SKIP_CAT(category, message)`` — skip the current subtest at runtime.

Subtests never touch a vendor header: they use only ``capi_*`` calls plus the
macros from ``parameters.h``, which is what keeps them board- and
platform-agnostic.

Project Layout
--------------

``src/common/common_data.*``
	Declares the CAPI configuration objects (UART, GPIO, SPI, timer, IRQ)
	used by the examples. The values are platform-neutral at the call site;
	``parameters.h`` from the active platform directory supplies the actual
	board macros.

``src/common/test_framework.*``
	Structured test output, result counters, skip categories, assertion
	helpers, ``test_framework_run_cases()`` and optional watchdog hooks.

``src/platform/<platform>/parameters.h``
	The only file that knows BSP/SDK identifiers. Maps them to the common
	macro names and defines the per-peripheral capability flags.

``src/platform/<platform>/main.c``
	Program entry point. Performs platform init (cache, clocks, IRQ) then
	calls ``example_main()``.

``src/examples/capi_loopback/capi_loopback.c``
	Defines ``example_main()`` and the top-level group table.

``src/examples/capi_loopback/tests/test_<peripheral>.c``
	Platform-agnostic test groups (``test_gpio``, ``test_spi``,
	``test_timer``).

Build Setup
-----------

Please see the no-OS build guide:
https://wiki.analog.com/resources/no-os/build

Build and run the default example:

.. code-block:: bash

	make PLATFORM=<platform> HARDWARE=<hardware>
	make run PLATFORM=<platform> HARDWARE=<hardware>

Build and run the loopback self-test:

.. code-block:: bash

	make clean PLATFORM=<platform> HARDWARE=<hardware> EXAMPLE=capi_loopback
	make PLATFORM=<platform> HARDWARE=<hardware> EXAMPLE=capi_loopback
	make run PLATFORM=<platform> HARDWARE=<hardware> EXAMPLE=capi_loopback

Xilinx-specific notes
^^^^^^^^^^^^^^^^^^^^^

Source the Vitis environment before building. Adjust the path to match your
local install:

.. code-block:: bash

	source ~/Xilinx/2025.2/Vitis/settings64.sh

The Xilinx platform configuration requires a BSP generated from
``zed_gic.xsa``. The XSA is not tracked by Git; keep it next to the project
when building locally.

+------------------+------------------------------------------------+
| Resource         | Use                                            |
+==================+================================================+
| Zynq PS UART1    | Test log output, 115200 8N1                    |
+------------------+------------------------------------------------+
| Zynq PS GIC      | IRQ controller for SPI/timer async paths       |
+------------------+------------------------------------------------+
| PS GPIO EMIO     | GPIO loopback: drive JA2 (pin 55), read JA1    |
|                  | (pin 54); the two must be wired together       |
+------------------+------------------------------------------------+
| PS SPI0 (EMIO)   | SPI loopback: MOSI (JC2) wired to MISO (JC3)   |
+------------------+------------------------------------------------+
| PS TTC0          | Timer counter/compare/overflow tests           |
+------------------+------------------------------------------------+

Example build invocation for ZedBoard:

.. code-block:: bash

	make PLATFORM=xilinx HARDWARE=zed_gic.xsa EXAMPLE=capi_loopback

Open the board's USB-UART serial port at 115200 baud before running.

Test Log Format
---------------

The test log uses stable tags so scripts can parse it:

.. code-block:: text

	[RUN:START] <PLATFORM_NAME>
	[TEST:<module>.<name>] PASS
	[TEST:<module>.<name>] FAIL: <details>
	[TEST:<module>.<name>] SKIP: <reason> [SKIP_CAT:<category>]
	[VAL:<module>.<name>.<value>] 0x...
	[RUN:SUMMARY] PASS=<n> FAIL=<n> SKIP=<n> TOTAL=<n>
	[RUN:END]

Adding a Test Group
-------------------

To test another CAPI peripheral, add a group under ``tests/`` and register it.
No framework or platform code needs to change beyond ``parameters.h``.

Step 1 — Write the group
^^^^^^^^^^^^^^^^^^^^^^^^^

Create ``tests/test_<peripheral>.c`` / ``.h``. Write one ``static int`` subtest
per behavior, drive it through a ``struct test_case`` table, and expose a
single ``int test_<peripheral>(void)`` entry point:

.. code-block:: c

	static int foo_basic(void)
	{
		struct capi_foo_handle *foo = NULL;

		TEST_SECTION("BASIC");
		TEST_ASSERT_EQ(capi_foo_init(&foo, &foo_config), 0, "INIT");
		/* ... exercise the API, assert results ... */
		TEST_ASSERT_EQ(capi_foo_deinit(foo), 0, "DEINIT");
		return 0;
	}

	static const struct test_case foo_subtests[] = {
		{ "BASIC",       foo_basic,       false        },
		{ "ASYNC",       foo_async,       !FOO_HAS_IRQ },
		{ "ERROR_PATHS", foo_error_paths, false        },
	};

	int test_foo(void)
	{
		return test_framework_run_cases("FOO", foo_subtests,
						sizeof(foo_subtests) /
						sizeof(foo_subtests[0]));
	}

Rules that keep the group portable:

* Use only ``capi_*`` calls and macros from ``parameters.h``; never include a
  vendor header.
* Gate optional behavior on a ``FOO_HAS_*`` flag in the table's skip field, so
  boards without the feature skip rather than fail.
* On a failing assertion the subtest returns immediately, so release any handle
  (``deinit``) *before* the assertions that might fail, or an early return will
  leak it into the next subtest.

Step 2 — Add the CAPI config object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Declare the peripheral's ``capi_foo_config`` in ``common_data.h`` and define it
in ``common_data.c`` from the ``FOO_*`` macros. Keep ``common_data.h``
platform-neutral (it must not include ``parameters.h``).

Step 3 — Provide the board macros
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In each platform's ``parameters.h`` add the identifier/ops/capability macros
the config and gates reference (``FOO_IDENTIFIER``, ``FOO_OPS``,
``FOO_HAS_IRQ``, ...).

Step 4 — Register and build
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Add the group to the table in ``capi_loopback.c``:

.. code-block:: c

	{ 0, "FOO", test_foo, false },

then list the CAPI source in ``src.mk`` and any platform driver in
``platform_src.mk``.

Adding Another Platform
-----------------------

Porting ``capi_selftest`` to a new platform requires three things: a
``parameters.h`` that maps BSP symbols to the common macro names, a ``main.c``
that handles platform init and calls ``example_main()``, and a
``platform_src.mk`` that lists any platform-specific source files. Nothing in
``src/common`` or ``src/examples`` needs to change.

Step 1 — Create the platform directory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create ``src/platform/<yourplatform>/`` and add the files described below.

Step 2 — Write ``parameters.h``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This file is the only place that knows about BSP or SDK-specific identifiers.
It must define the following macros:

**UART (test output transport):**

.. code-block:: c

	#define UART_IDENTIFIER   /* device ID or base address */
	#define UART_OPS          /* pointer to struct capi_uart_ops */
	#define UART_BAUDRATE     115200U

**Platform label** (printed in ``[RUN:START]``):

.. code-block:: c

	#define PLATFORM_NAME     "MYPLATFORM"

**Output/readback GPIO port** (driven by the test, then read back):

.. code-block:: c

	#define GPIO_OUTPUT_NAME        "MY_OUTPUT_PORT"
	#define GPIO_OUTPUT_IDENTIFIER  /* device ID or base address */
	#define GPIO_OUTPUT_NUM_PINS    /* number of pins on this port */
	#define GPIO_OUTPUT_OPS         /* pointer to struct capi_gpio_port_ops */
	#define GPIO_OUTPUT_EXTRA  struct my_gpio_config
	#define GPIO_OUTPUT_EXTRA_INIT  { /* initializer for GPIO_OUTPUT_EXTRA */ }

**Input GPIO port** (sampled by the test):

.. code-block:: c

	#define GPIO_INPUT_NAME        "MY_INPUT_PORT"
	#define GPIO_INPUT_IDENTIFIER  /* device ID or base address */
	#define GPIO_INPUT_NUM_PINS    /* number of pins on this port */
	#define GPIO_INPUT_OPS         /* pointer to struct capi_gpio_port_ops */
	#define GPIO_INPUT_EXTRA  struct my_gpio_config
	#define GPIO_INPUT_EXTRA_INIT  { /* initializer for GPIO_INPUT_EXTRA */ }

The ``_EXTRA_TYPE`` and ``_EXTRA_INIT`` macros are used by ``common_data.c``
to declare and initialize the private config structs for each port. If your
platform's GPIO driver does not require extra configuration, define them as
an empty struct and empty initializer:

.. code-block:: c

	#define GPIO_OUTPUT_EXTRA  struct { int unused; }
	#define GPIO_OUTPUT_EXTRA_INIT  { 0 }

**SPI, timer and IRQ:** the same pattern applies. Each peripheral needs its
identifier/ops macros, its ``_EXTRA_TYPE`` / ``_EXTRA_INIT`` pair where the
driver takes private config, and a ``*_HAS_*`` capability flag for every gated
subtest (``SPI_HAS_IRQ``, ``SPI_HAS_DMA``, ``TIMER_HAS_IRQ``,
``TIMER_HAS_CAPTURE``, ...). The timer additionally needs its counter shape
(``TIMER_DIRECTION``, ``TIMER_COUNTER_MAX``) and rate-check parameters
(``TIMER_RATE_WINDOW_US``, ``TIMER_RATE_COUNTER_MASK``,
``TIMER_RATE_TOLERANCE_PCT``); pick the window well below the counter rollover
period and the mask equal to the counter width. See the Xilinx
``parameters.h`` for a complete, commented reference set.

Step 3 — Write ``main.c``
^^^^^^^^^^^^^^^^^^^^^^^^^^

Perform any platform-specific startup (enable caches, configure clocks, etc.)
then call ``example_main()``. The function is declared in each example:

.. code-block:: c

	extern int example_main(void);

	int main(void)
	{
	    /* platform init */
	    return example_main();
	}

Step 4 — Write ``platform_src.mk``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

List any source files that must be compiled only for this platform. At minimum
this is ``main.c``:

.. code-block:: makefile

	PLATFORM_SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

Step 5 — Wire in the hardware
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``capi_loopback`` example uses external loopback where the CAPI API cannot
otherwise observe its own effect:

* **GPIO** — two ports wired together, so the driven output is sampled on the
  input. On boards with a single port, a logical split (two channels of the
  same peripheral) works equally well.
* **SPI** — MOSI wired to MISO, so every transfer reads back what it wrote.
* **Timer** — no wiring; the counter-overflow interrupt is self-generated and
  the rate is checked against the independent CAPI uptime clock.

The UART port only needs to be reachable from a host terminal at
``UART_BAUDRATE``. Any peripheral whose ``*_HAS_*`` flags are all disabled, or
whose group is absent, simply does not run — the framework reports the gap
rather than failing.
