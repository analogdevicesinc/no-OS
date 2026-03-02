AD4692 no-OS Example Project
============================

.. no-os-doxygen::

Contents
--------

.. contents:: Table of Contents
	:depth: 3

Supported Evaluation Boards
---------------------------

* `EVAL-AD4692-ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4692-ardz.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

Overview
--------

The **EVAL-AD4692-ARDZ** is an Arduino-compatible evaluation board for the
AD4692 family of compact, Easy Drive, 16-bit MUX SAR ADCs. These devices
combine per-channel accumulators, a flexible channel sequencer, and an
internal oscillator with five selectable operating modes, making them well
suited for space-constrained precision measurement applications such as optical
transceiver modules and multi-channel control loops.

The evaluation board connects to a host MCU via the Arduino shield interface.
These examples target the **AD-APARD32690-SL** rapid prototyping platform,
which is built around the MAX32690 microcontroller.

The following devices are supported and share the same project:

.. list-table::
   :header-rows: 1

   * - Device
     - Channels
     - Max Sample Rate
   * - AD4692
     - 16
     - 1 MSPS
   * - AD4691
     - 16
     - 500 kSPS
   * - AD4694
     - 8
     - 1 MSPS
   * - AD4693
     - 8
     - 500 kSPS

To select the target device, change the ``id`` field in ``ad4692_ip`` inside
`common_data.c <src/common/common_data.c>`_.

Applications
------------

* Optical transceiver module monitoring
* Multi-channel industrial control loops
* Precision instrumentation
* Data acquisition systems

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken from:
`Project Common Data Path <src/common>`_

The operating mode is selected via the ``mode`` field in ``ad4692_ip`` in
`common_data.c <src/common/common_data.c>`_. The default is
``AD4692_MANUAL_MODE``, which works for all three examples. Refer to the
AD4692 driver documentation for a description of all five operating modes and
their hardware requirements.

Basic example
^^^^^^^^^^^^^

Initializes the AD4692 and reads two channels using :c:func:`ad4692_get_ch`,
printing the raw values over UART. :c:func:`ad4692_get_ch` handles sequencer
configuration, sampling, and readback internally for every operating mode.

For oscillator-driven modes (CNV Burst, Autonomous, SPI Burst) the example
first sets the internal OSC to 500 kHz. No additional setup is required for
Manual or CNV Clock modes.

To build and run:

.. code-block:: bash

	make EXAMPLE=basic TARGET=max32690
	make run TARGET=max32690

IIO example
^^^^^^^^^^^

Launches an IIOD server on the board so that any LibIIO client can connect,
configure the ADC, and stream data. Data acquisition uses the
``submit_buffer`` path, which is compatible with all five operating modes
and does not require a hardware trigger.

If you are not familiar with ADI IIO Application, please take a look at:
`IIO No-OS <https://wiki.analog.com/resources/tools-software/no-os-software/iio>`_

If you are not familiar with ADI IIO-Oscilloscope Client, please take a look at:
`IIO Oscilloscope <https://wiki.analog.com/resources/tools-software/linux-software/iio_oscilloscope>`_

To connect from a host once the board is running:

.. code-block:: bash

	iio_info -u serial:/dev/ttyUSB0,57600,8n1

To build and run:

.. code-block:: bash

	make EXAMPLE=iio_example TARGET=max32690
	make run TARGET=max32690

IIO trigger example
^^^^^^^^^^^^^^^^^^^

Extends the IIO example with a hardware trigger driven by the GPIO0 output
of the AD4692. In every non-manual operating mode the device asserts GPIO0
at the end of each conversion sequence (as DATA_READYb in CNV Clock, CNV
Burst, and SPI Burst modes; as ADC_BUSY in Autonomous mode). Both signals
produce a falling edge at conversion complete, which fires the MAX32690
GPIO interrupt and invokes the IIO trigger handler to read and push the
accumulated channel data.

.. note::

	``AD4692_MANUAL_MODE`` is not compatible with this example because the
	IIO driver does not register a trigger handler for manual mode. All
	other modes work correctly.

The trigger is configured for a falling-edge interrupt on the GPIO0 pin
(P3.0 on the MAX32690, connected to the DREADY/BUSY signal of the
EVAL-AD4692-ARDZ).

To build and run:

.. code-block:: bash

	make EXAMPLE=iio_trigger_example TARGET=max32690
	make run TARGET=max32690

No-OS Supported Platforms
-------------------------

Maxim Platform
^^^^^^^^^^^^^^

**Used hardware**

* `EVAL-AD4692-ARDZ <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-ad4692-ardz.html>`_
* `AD-APARD32690-SL <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html>`_

**Connections**

The EVAL-AD4692-ARDZ plugs directly into the Arduino-compatible headers of
the AD-APARD32690-SL. The following MAX32690 peripherals are used:

.. list-table::
   :header-rows: 1

   * - Function
     - MAX32690 Peripheral
     - Notes
   * - SPI (MOSI/MISO/SCK/CS)
     - SPI4, CS0
     - Mode 3 (CPOL=1, CPHA=1), up to 1 MHz
   * - RESET
     - P1.6 (GPIO)
     - Active-low hardware reset
   * - DREADY / ADC_BUSY
     - P3.0 (GPIO input / IRQ)
     - Interrupt source for trigger example
   * - CNV (PWM)
     - PWM2
     - Required for CNV Clock and CNV Burst modes only
   * - UART (debug)
     - UART0
     - 57600 baud, 8N1

**Build commands**

.. code-block:: bash

	# Basic example (default)
	make EXAMPLE=basic TARGET=max32690

	# IIO example
	make EXAMPLE=iio_example TARGET=max32690

	# IIO trigger example
	make EXAMPLE=iio_trigger_example TARGET=max32690

	# Flash to board
	make run TARGET=max32690

	# Clean build artifacts
	make reset TARGET=max32690
