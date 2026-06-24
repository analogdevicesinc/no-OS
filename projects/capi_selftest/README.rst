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

Project Layout
--------------

``src/common/common_data.*``
	Defines the CAPI UART and GPIO configuration objects used by examples.
	The values are platform-neutral at the call site; ``parameters.h``
	from the active platform directory supplies the actual macros.

``src/common/test_framework.*``
	Provides structured test output, result counters, skip categories,
	assertion helpers and optional timeout hooks.

``src/platform/<platform>/parameters.h``
	Maps BSP or SDK identifiers to the common macro names consumed by
	``common_data.c``.

``src/platform/<platform>/main.c``
	Program entry point. Performs any platform-specific initialization
	(cache enable, clock setup, etc.) then calls ``example_main()``.

``src/examples/capi_loopback/capi_loopback.c``
	Defines ``example_main()`` for the loopback runner.

``src/examples/capi_loopback/tests/test_gpio.c``
	Platform-agnostic GPIO test cases.

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

+-----------------+---------------------------+
| Resource        | Use                       |
+=================+===========================+
| Zynq PS UART1   | Test log output, 115200   |
+-----------------+---------------------------+
| AXI GPIO LEDs   | Output/readback GPIO test |
+-----------------+---------------------------+
| AXI GPIO SWS    | Input GPIO test           |
+-----------------+---------------------------+

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
	#define GPIO_OUTPUT_EXTRA_TYPE  struct my_gpio_config
	#define GPIO_OUTPUT_EXTRA_INIT  { /* initializer for GPIO_OUTPUT_EXTRA_TYPE */ }

**Input GPIO port** (sampled by the test):

.. code-block:: c

	#define GPIO_INPUT_NAME        "MY_INPUT_PORT"
	#define GPIO_INPUT_IDENTIFIER  /* device ID or base address */
	#define GPIO_INPUT_NUM_PINS    /* number of pins on this port */
	#define GPIO_INPUT_OPS         /* pointer to struct capi_gpio_port_ops */
	#define GPIO_INPUT_EXTRA_TYPE  struct my_gpio_config
	#define GPIO_INPUT_EXTRA_INIT  { /* initializer for GPIO_INPUT_EXTRA_TYPE */ }

The ``_EXTRA_TYPE`` and ``_EXTRA_INIT`` macros are used by ``common_data.c``
to declare and initialize the private config structs for each port. If your
platform's GPIO driver does not require extra configuration, define them as
an empty struct and empty initializer:

.. code-block:: c

	#define GPIO_OUTPUT_EXTRA_TYPE  struct { int unused; }
	#define GPIO_OUTPUT_EXTRA_INIT  { 0 }

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

The ``capi_loopback`` example requires two GPIO ports that can be wired
together so the output port can be driven and the result sampled on the input
port. On boards with a physical loopback header or two separate port peripherals
you can connect them directly; on boards with a single port, a logical split
(two channels of the same peripheral) works equally well.

The UART port only needs to be accessible from a host terminal at the baud
rate specified by ``UART_BAUDRATE``.
