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
no-OS environment. In order to build this example make sure you are
using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant basic --board sdp-ck1z

Current Input External Power Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Current Input External Power Example demonstrates channel
configuration in externally powered current input mode. It reads the
ADC value and computes the input current. In order to build this example
make sure you are using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant current_input_ext --board sdp-ck1z

Current Input Loop Power Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Current Input Loop Power Example demonstrates channel configuration
in loop-powered current input mode. It reads the ADC value and computes
the input current. In order to build this example make sure you are
using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant current_input_loop --board sdp-ck1z

Current Output Example
~~~~~~~~~~~~~~~~~~~~~~

The Current Output Example demonstrates channel configuration in
current output mode. It sets a DAC code to drive a specified output
current on the configured channel. In order to build this example make
sure you are using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant current_output --board sdp-ck1z

Digital Input Logic Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Digital Input Logic Example demonstrates channel configuration in
digital input mode with a fixed comparator threshold for logic-level
detection. In order to build this example make sure you are using this
command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant digital_input_logic --board sdp-ck1z

Digital Input Loop Example
~~~~~~~~~~~~~~~~~~~~~~~~~~

The Digital Input Loop Example demonstrates channel configuration in
digital input mode with loop-powered operation. In order to build this
example make sure you are using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant digital_input_loop --board sdp-ck1z

Digital Output Example
~~~~~~~~~~~~~~~~~~~~~~

The Digital Output Example demonstrates channel configuration in
digital output mode with GPIO-based control of the output state. In
order to build this example make sure you are using this command:

.. code-block:: bash

    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant digital_output --board sdp-ck1z

No-OS Supported Platforms
-------------------------

STM32
~~~~~

Used Hardware
^^^^^^^^^^^^^

- :adi:`EVAL-AD74414H`
- :adi:`EVAL-SDP-CK1Z`

Connections
^^^^^^^^^^^

Connect the EVAL-AD74414H to the EVAL-SDP-CK1Z (SDP-K1) through the SDP
connector. The SDP-K1 is powered and communicates with the host over its
USB-C port, which also exposes the UART console.

Build Command
^^^^^^^^^^^^^

The STM32 platform uses the CMake/Ninja build system via the
``no_os_build.py`` helper script. Available variants: ``basic``,
``current_input_ext``, ``current_input_loop``, ``current_output``,
``digital_input_logic``, ``digital_input_loop``, ``digital_output``.
Available boards: ``sdp-ck1z``.

For toolchain setup and prerequisites, see the
`STM32 CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_stm32_cmake.html>`__.

.. code-block:: bash

    # set the path to STM32CubeMX and STM32CubeIDE (only if they are not
    # in a default install location)
    export STM32CUBEMX=</path/to/stm32cubemx>
    export STM32CUBEIDE=</path/to/stm32cubeide>
    # PowerShell (Windows) equivalent:
    #   $env:STM32CUBEMX = "C:\ST\STM32CubeMX"
    #   $env:STM32CUBEIDE = "C:\ST\STM32CubeIDE"

    cd no-OS

    # build the example on the SDP-K1 board (replace --variant as needed)
    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant basic --board sdp-ck1z

    # build and flash (requires a connected debug probe)
    python tools/scripts/no_os_build.py build \
        --project ad74414h --variant basic --board sdp-ck1z \
        --probe openocd --flash
