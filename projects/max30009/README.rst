MAX30009 no-OS Example Project
===============================

Supported Evaluation Boards
----------------------------

* `MAX30009 Evaluation Board <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/max30009evkit.html>`_

Overview
--------

This project demonstrates the MAX30009 Low-Power, High-Performance Bioimpedance
Analog Front-End (AFE) integration with the no-OS framework. The MAX30009 enables
bioimpedance (BioZ) measurement for applications such as body composition analysis,
impedance cardiography (ICG), and respiration monitoring.

The MAX30009 is a low-power, high-performance analog front-end (AFE) for bioimpedance
(BioZ) measurement applications. It features:

* Bioimpedance measurement with programmable excitation
* Dual-channel (I and Q) data acquisition
* Frequency and phase lock detection
* 256-word FIFO for data buffering
* Lead-off detection for electrode monitoring
* I2C interface with configurable address
* Low power consumption with shutdown mode
* Programmable measurement gain (10 V/V to 100 V/V)
* Configurable drive current (32 μA to 96 μA peak)
* Adjustable amplifier bandwidth (32 kHz to 256 kHz)

Applications
------------

* Body composition analysis
* Impedance cardiography (ICG)
* Respiration monitoring
* Wearable medical devices
* Vital signs monitoring
* Hydration measurement
* Muscle monitoring

Hardware Specifications
-----------------------

Power Supply Requirements
~~~~~~~~~~~~~~~~~~~~~~~~~

The MAX30009 requires a 1.8V power supply. Both SPI and I2C
interfaces operate at 1.8V logic levels.

I2C Interface
~~~~~~~~~~~~~

* Default address: 0x69 (configurable to 0x54 via hardware pin)
* Supported speeds: Standard (100 kHz), Fast (400 kHz)
* 7-bit addressing

No-OS Build Setup
-----------------

Please see: https://wiki.analog.com/resources/no-os/build

No-OS Supported Examples
------------------------

The initialization data used in the examples is taken out from:
`Project Common Data Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max30009/src/common>`_

The macros used in Common data are defined in platform-specific files found in:
`Project Platform Configuration Path <https://github.com/analogdevicesinc/no-OS/tree/main/projects/max30009/src/platform>`_

Basic Example
~~~~~~~~~~~~~

The basic example initializes the MAX30009 and demonstrates fundamental functionality:

- Device initialization and hardware setup
- Interrupt configuration for FIFO data ready
- PLL configuration for 50 kHz stimulus frequency @ 100 sps sample rate
- BioZ measurement configuration (sine-wave current drive, 10 V/V gain)
- 4-electrode (tetrapolar) BIA electrode MUX configuration
- Lead-off detection setup
- FIFO data acquisition and sample decoding (I and Q channels)
- Status monitoring (frequency lock, phase lock, power ready)

In order to build the basic example make sure you are using this command:

.. code-block:: bash

	python tools/scripts/no_os_build.py build \
	   --project max30009 --variant basic --board max32655fthr

No-OS Supported Platforms
-------------------------

Maxim Platform
~~~~~~~~~~~~~~

Used Hardware
^^^^^^^^^^^^^

* MAX30009 Evaluation Board
* `MAX32655FTHR <https://www.analog.com/MAX32655FTHR>`_

Connections
^^^^^^^^^^^

The MAX30009 supports both I2C and SPI interfaces. The basic example uses **I2C by default**.

**I2C Interface:**

+-------------+------------------+--------------------------------------------------+
| MAX30009    | MAX32655FTHR     | Function                                         |
+-------------+------------------+--------------------------------------------------+
| VDD         | 1V8              | Power Supply                                     |
+-------------+------------------+--------------------------------------------------+
| GND         | GND              | Ground                                           |
+-------------+------------------+--------------------------------------------------+
| SDA         | P0_6             | I2C Data                                         |
+-------------+------------------+--------------------------------------------------+
| SCL         | P0_7             | I2C Clock                                        |
+-------------+------------------+--------------------------------------------------+
| CSB/I2C_SEL | P0_20            | Select I²C mode                                  |
+-------------+------------------+--------------------------------------------------+
| INTB        | P0_25            | Interrupt (FIFO ready, optional)                 |
+-------------+------------------+--------------------------------------------------+

**SPI Interface:**

+-------------+------------------+----------------------------------+
| MAX30009    | MAX32655FTHR     | Function                         |
+-------------+------------------+----------------------------------+
| VDD         | 1V8              | Power Supply                     |
+-------------+------------------+----------------------------------+
| GND         | GND              | Ground                           |
+-------------+------------------+----------------------------------+
| SCLK        | P0_11            | SPI Clock                        |
+-------------+------------------+----------------------------------+
| MOSI        | P0_5             | SPI Data Out (MOSI)              |
+-------------+------------------+----------------------------------+
| MISO        | P0_4             | SPI Data In (MISO)               |
+-------------+------------------+----------------------------------+
| CSB/I2C_SEL | P0_20            | SPI Chip Select (active-low)     |
+-------------+------------------+----------------------------------+
| INTB        | P0_25            | Interrupt (FIFO ready, optional) |
+-------------+------------------+----------------------------------+

Build Command
^^^^^^^^^^^^^

Available variants: ``basic``.
Available boards: ``max32655fthr``.

.. code-block:: bash

   export MAXIM_LIBRARIES=/path/to/MaximSDK/Libraries
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the max32655fthr board)
   python tools/scripts/no_os_build.py build \
      --project max30009 --variant basic --board max32655fthr

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project max30009 --variant basic --board max32655fthr \
      --probe openocd --flash
