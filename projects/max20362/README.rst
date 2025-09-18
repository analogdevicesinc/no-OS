Evaluating the MAX20362
========================

.. no-os-doxygen::

.. contents::
	:depth: 3

Supported Evaluation Boards
---------------------------

* `MAX20362EVKIT`_

Overview
--------

This project is a comprehensive test framework and validation suite for the
MAX20362 no-OS driver. It executes an extensive series of automated tests
covering all major driver functionalities including communication validation,
register access, voltage/current control, LDO operations, DVS configuration,
and interrupt handling. The test suite provides detailed pass/fail reporting
with register-level diagnostics to ensure the driver operates correctly with
the hardware.

The test framework includes:
* **9 comprehensive test functions** covering all driver features
* **750+ lines of test code** with extensive validation
* **Automated register read-back verification** for all settings
* **Detailed error reporting** with register dumps on failures
* **Comprehensive status monitoring** throughout test execution

Hardware Specifications
-----------------------

Power Supply Requirements
^^^^^^^^^^^^^^^^^^^^^^^^^

The MAX20362 evaluation kit can be powered directly from a host PC's USB
port, which supplies 5V to the board's BAT input. Alternatively, an external
power supply can be connected to the BAT and GND test points.

**Hardware Bringup**

For reference, follow the Quick Start Procedure section of the evaluation
kit's user guide. The setup requires the MAX20362EVKIT# and a MAXPICO2PMB#
adapter board for I2C communication with the host.

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

Build System Overview
^^^^^^^^^^^^^^^^^^^^^

The MAX20362 project uses the standard no-OS build system with the following
structure:

**Core Build Files**:

* ``Makefile``: Main makefile that includes the generic no-OS build system
* ``src.mk``: Project-specific source file definitions including the
  MAX20362 driver
* ``builds.json``: Build configuration for CI/CD systems

**Source Organization**:

* Platform abstraction layer (I2C, UART, GPIO, Timer, IRQ, DMA drivers)
* no-OS framework APIs and utilities (FIFO, List, Memory allocation, Mutex)
* Application code (common data structures, platform main, example code)

**Build Outputs**:

* ``max20362.elf``: Executable ELF file
* ``max20362.hex``: Intel HEX file for flashing

No-OS Supported Examples
------------------------

The initialization data used in the examples is based on the
platform-specific default configurations.

Basic Example (Comprehensive Test Suite)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example is a comprehensive test suite that initializes the MAX20362 and
validates all major driver functionalities through automated testing. The
test suite includes:

**Communication and Device Identification Tests**:

* **test_communication()**: Verifies I2C communication and reads device
  Chip ID (expected: 0x00)
* **test_register_access()**: Performs write-read-verify sequence on
  writable registers

**Power Supply Control Tests**:

* **test_buck_boost_voltage()**: Tests buck-boost voltage setting from 1.5V
  to 5.5V with range validation
* **test_ldo_voltage()**: Tests LDO voltage setting from 0.9V to 4.0V with
  range validation and clamping
* **test_input_current_limit()**: Validates input current limit setting from
  5mA to 50mA

**Functional Feature Tests**:

* **test_buck_boost_enable()**: Verifies buck-boost enable/disable
  functionality
* **test_ldo_enable()**: Verifies LDO enable/disable functionality
* **test_cap_voltage()**: Tests capacitor voltage setting with different
  step size configurations

**Advanced Feature Tests**:

* **test_dvs_configuration()**: Tests Dynamic Voltage Scaling mode
  configuration (I2C and Round-Robin)
* **test_interrupt_handling()**: Tests interrupt status reading, masking, and
  clearing functionality
* **test_register_protection()**: Tests register lock/unlock functionality
  for configuration protection

**System Integration Tests**:

* **test_power_sequencing()**: Comprehensive power-up sequence test with
  voltage and status monitoring

Each test provides detailed diagnostic output including register values,
expected vs. actual results, and comprehensive error reporting. The test
suite concludes with a summary showing the total number of tests passed and
failed.

Example test output:

.. code-block:: text

    ========================================
    TEST: Basic Communication
    ========================================
    [PASS] Chip ID Verification - Chip ID: 0x00 (expected 0x00)

    ========================================
    TEST: Register Read/Write Operations
    ========================================
    [PASS] Write/Read Verification - Wrote 0x0A, Read 0x0A
    [PASS] Update Bits Verification - Masked update result: 0x05

    ...

    ========================================
    TEST SUMMARY
    ========================================
    Tests Passed: 28
    Tests Failed: 0
    Total Tests:  28

    RESULT: ALL TESTS PASSED
    ========================================

Building the Project
---------------------

In order to build the basic example make sure you are using this command:

.. code-block:: bash

    cd no-OS
    python tools/scripts/no_os_build.py build \
      --project max20362 --variant basic --board max32690evkit

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `MAX20362EVKIT`_
* `MAX32690EVKIT <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32690evkit.html>`_
* `MAX32625PICO <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max32625pico.html>`_

**Connections**:

The MAX20362EVKIT connects to the MAX32625PICO board, which then connects to
the MAX32690EVKit.

+--------------------------+-------------------+-----------------------------+
| MAX20362 EVKIT Connector | Function          | MAX32690 EVKIT Pin          |
+==========================+===================+=============================+
| J38 (PMOD) Pin 10        | Ground            | GND                         |
+--------------------------+-------------------+-----------------------------+
| J38 (PMOD) Pin 4         | Serial Data Line  | P2_7 (I2C2_SDA)             |
+--------------------------+-------------------+-----------------------------+
| J38 (PMOD) Pin 3         | Serial Clock Line | P2_8 (I2C2_SCL)             |
+--------------------------+-------------------+-----------------------------+

**Build Command**

.. code-block:: bash

	cd no-OS
	# to delete current build
	python tools/scripts/no_os_build.py build \
	    --project max20362 --variant basic --board max32690evkit --clean
	# to build the project for running basic example
	python tools/scripts/no_os_build.py build \
	    --project max20362 --variant basic --board max32690evkit
	# to flash the code
	python tools/scripts/no_os_build.py build \
      --project max20362 --variant basic --board max32690evkit --flash --probe opencd

.. _MAX20362EVKIT: https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max20362evkit.html
