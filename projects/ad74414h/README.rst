AD74414H no-OS Project
======================

Supported Evaluation Boards
---------------------------

- :adi:`EVAL-AD74414H`

Overview
---------

The AD74414H is a quad-channel, software-configurable device designed
for robust industrial control applications. It offers versatile analog
and digital I/O capabilities for measuring voltage, current, RTD, and
thermocouples while incorporating a 16-bit DAC and a 24-bit sigma-delta
ADC with optional 50Hz/60Hz rejection. Each channel features an
integrated HART modem that provides both HART and SPI communications
using configurable pin assignments that support multiple operation
modes. This is complemented by adaptive power switching, built-in
diagnostics for open-circuit/short-circuit detection, a high-accuracy
2.5V reference, and auxiliary channels for sensing and power supply
measurements.

Applications
------------

- Industrial control systems
- Process control
- Factory automation

No-OS Build Setup
-----------------

Please see: :dokuwiki:`No-OS Build Setup </resources/no-os/build>`

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from the
:git-no-OS:`Project Common Data Path </projects/ad74414h/src/common>`.
The macros used in Common Data are defined in platform-specific files found in:
:git-no-OS:`Platform Configuration Path </projects/ad74414h/src/platform>`.

Basic Example
~~~~~~~~~~~~~

The Basic Example demonstrates the fundamental initialization and
configuration of the AD74414H device by setting up communication
interfaces such as UART and SPI using common data structures in a
no-OS environment. To enable the Basic Example and disable other
examples, update the :git-no-OS:`Makefile </projects/ad74414h/Makefile>`
with the following settings:

.. code-block:: bash

    # Enable Basic Example
    EXAMPLE = basic

Current Input External Power Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Current Input External Power Example demonstrates channel
configuration in externally powered current input mode. It reads the
ADC value and computes the input current. To enable this example, update
the :git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the
following settings:

.. code-block:: bash

    # Enable Current Input External Power Example
    EXAMPLE = current_input_ext

Current Input Loop Power Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Current Input Loop Power Example demonstrates channel configuration
in loop-powered current input mode. It reads the ADC value and computes
the input current. To enable this example, update the
:git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the following
settings:

.. code-block:: bash

    # Enable Current Input Loop Power Example
    EXAMPLE = current_input_loop

Current Output Example
~~~~~~~~~~~~~~~~~~~~~~

The Current Output Example demonstrates channel configuration in
current output mode. It sets a DAC code to drive a specified output
current on the configured channel. To enable this example, update the
:git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the following
settings:

.. code-block:: bash

    # Enable Current Output Example
    EXAMPLE = current_output

Digital Input Logic Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Digital Input Logic Example demonstrates channel configuration in
digital input mode with a fixed comparator threshold for logic-level
detection. To enable this example, update the
:git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the following
settings:

.. code-block:: bash

    # Enable Digital Input Logic Example
    EXAMPLE = digital_input_logic

Digital Input Loop Example
~~~~~~~~~~~~~~~~~~~~~~~~~~

The Digital Input Loop Example demonstrates channel configuration in
digital input mode with loop-powered operation. To enable this example,
update the :git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the
following settings:

.. code-block:: bash

    # Enable Digital Input Loop Example
    EXAMPLE = digital_input_loop

Digital Output Example
~~~~~~~~~~~~~~~~~~~~~~

The Digital Output Example demonstrates channel configuration in
digital output mode with GPIO-based control of the output state. To
enable this example, update the
:git-no-OS:`Makefile </projects/ad74414h/Makefile>` with the following
settings:

.. code-block:: bash

    # Enable Digital Output Example
    EXAMPLE = digital_output

No-OS Supported Platforms
-------------------------

Mbed Platform
~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-AD74414H`
- :adi:`EVAL-SDP-CK1Z`

Build Command
^^^^^^^^^^^^^

.. code-block:: bash

    # Clean previous build artifacts
    make clean

    # Build the project for the Mbed platform
    make PLATFORM=mbed

    # Flash the firmware
    make flash

    # Start a debugging session
    make debug
