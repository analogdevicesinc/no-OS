LTC4284 - High Power Negative Voltage Hot Swap Controller
==========================================================

Supported Devices
-----------------

* `LTC4284 <https://www.analog.com/ltc4284>`_

Overview
--------

The LTC4284 is a high power negative voltage hot swap controller with integrated
I²C energy monitor and EEPROM. It provides comprehensive protection and monitoring
for -48V distributed power systems commonly used in telecom infrastructure, data
centers, and server applications.

The device drives external N-channel MOSFETs to allow a board to be safely
inserted and removed from a live backplane. The dual-gate, multi-mode drivers
optimize the MOSFET safe operating area (SOA) for a variety of power levels,
with an SOA timer limiting MOSFET temperature rise for reliable protection
against overstresses.

Features
--------

* Dual-gate N-channel MOSFET driver for high power applications
* Configurable operating modes: Parallel, Staged Start, or Single modes
* MOSFET SOA timer protection
* Programmable 15mV to 30mV current limit sense voltage with <3.3% accuracy
* Adjustable current limit foldback
* 8-bit to 16-bit gear-shift ADC with 0.7% accuracy
* Monitors voltages, currents, power, and energy
* Nonvolatile configuration and fault recording via EEPROM
* Floating topology for rugged high voltage operation
* Selectable inrush control: dV/dt or current limit modes
* I²C/SMBus or single-wire broadcast interfaces
* Min/max ADC measurement logging with programmable alerts
* Reboots on I²C command with programmable delay
* Adjustable input UV/OV thresholds and hysteresis
* 44-Pin 5mm × 8mm QFN Package

Applications
------------

* -48V Telecom and Datacom Infrastructure
* -48V Distributed Power Systems
* Hot-swappable Power Supplies
* Servers and Data Centers
* Network Equipment
* Blade Server Systems

LTC4284 Device Configuration
-----------------------------

Driver Initialization
^^^^^^^^^^^^^^^^^^^^^

The LTC4284 driver is initialized using the following structure:

.. code-block:: c

	#include "ltc4284.h"

	struct ltc4284_dev *ltc4284_dev;
	struct ltc4284_init_param init_param = {
		.i2c_init = {
			.device_id = 0,
			.max_speed_hz = 400000,  /* 400kHz I2C */
			.platform_ops = &max32_i2c_ops,
			.extra = &i2c_extra
		},
		.i2c_addr = LTC4284_I2C_ADDR_2,  /* Default 0x2A */
		.rsense_mohm = 500,              /* 0.5mΩ sense resistor */
		.vsense_mv = 25,                 /* 25mV sense voltage */
		.alert_gpio = NULL               /* Optional ALERT# pin */
	};

	ret = ltc4284_init(&ltc4284_dev, &init_param);

I²C Address Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^

The LTC4284 I²C address is configurable via ADR0 and ADR1 pins:

* **0x28**: ADR0=0, ADR1=0
* **0x29**: ADR0=1, ADR1=0
* **0x2A**: ADR0=0, ADR1=1 (default)
* **0x2B**: ADR0=1, ADR1=1

Current Limiting Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The LTC4284 supports programmable current limiting based on external sense
resistor (RSENSE) and sense voltage (VSENSE):

**Sense Voltage Options**: 15mV, 20mV, 25mV, 30mV

**Current Limit Calculation**:

.. code-block:: c

	/* Current limit = VSENSE / RSENSE
	 * Example: 25mV / 0.5mΩ = 50A limit
	 */
	uint8_t ilim_code = 0x08;  /* Application-specific */
	ret = ltc4284_set_current_limit(ltc4284_dev, ilim_code);

Monitoring Functions
^^^^^^^^^^^^^^^^^^^^

The driver provides monitoring functions for voltage, current, power, and energy:

.. code-block:: c

	uint16_t vin_mv, iin_ma, vout_mv;
	uint32_t power_mw;
	uint64_t energy_mj;

	/* Read input voltage */
	ret = ltc4284_read_vin(ltc4284_dev, &vin_mv);

	/* Read input current */
	ret = ltc4284_read_iin(ltc4284_dev, &iin_ma);

	/* Read output voltage */
	ret = ltc4284_read_vout(ltc4284_dev, &vout_mv);

	/* Read power consumption */
	ret = ltc4284_read_power(ltc4284_dev, &power_mw);

	/* Read cumulative energy */
	ret = ltc4284_read_energy(ltc4284_dev, &energy_mj);

FET Control
^^^^^^^^^^^

Control MOSFET gate drivers:

.. code-block:: c

	/* Enable FET drivers for hot swap turn-on */
	ret = ltc4284_enable_fet(ltc4284_dev, true);

	/* Disable FET drivers for turn-off */
	ret = ltc4284_enable_fet(ltc4284_dev, false);

Fault Management
^^^^^^^^^^^^^^^^

Read fault status and clear faults:

.. code-block:: c

	uint8_t status, faults;

	/* Read system status */
	ret = ltc4284_read_status(ltc4284_dev, &status);

	/* Get fault conditions */
	ret = ltc4284_get_fault(ltc4284_dev, &faults);

	/* Check individual fault bits */
	if (faults & LTC4284_FAULT_OV_FAULT_BIT)
		printf("Overvoltage fault detected\\n");

	if (faults & LTC4284_FAULT_OC_FAULT_BIT)
		printf("Overcurrent fault detected\\n");

	/* Clear all faults */
	ret = ltc4284_clear_faults(ltc4284_dev);

EEPROM Configuration
^^^^^^^^^^^^^^^^^^^^

Store and restore configuration to/from EEPROM:

.. code-block:: c

	/* Store current configuration to EEPROM */
	ret = ltc4284_store_config(ltc4284_dev);

	/* Restore configuration from EEPROM */
	ret = ltc4284_restore_config(ltc4284_dev);

LTC4284 no-OS IIO Support
--------------------------

The LTC4284 driver provides comprehensive IIO (Industrial I/O) subsystem
integration for real-time telemetry and monitoring.

IIO Channels
^^^^^^^^^^^^

The driver exposes the following IIO channels:

* **vin** (voltage_in channel 0): Input voltage monitoring
* **iin** (current_in channel 0): Input current monitoring
* **vout** (voltage_out channel 1): Output voltage monitoring
* **power** (power channel 0): Power consumption
* **energy** (energy channel 0): Cumulative energy accumulation

Each channel provides **raw** and **scale** attributes:

* **raw**: Returns integer value in millivolts/milliamps/milliwatts/millijoules
* **scale**: Returns conversion factor (1/1000) to base units (V/A/W/J)

IIO Initialization
^^^^^^^^^^^^^^^^^^

.. code-block:: c

	#include "iio_ltc4284.h"

	struct ltc4284_iio_desc *ltc4284_iio_desc;
	struct ltc4284_iio_init_param iio_init = {
		.ltc4284_init = &ltc4284_init_param
	};

	ret = ltc4284_iio_init(&ltc4284_iio_desc, &iio_init);

Using IIO with pyadi-iio
^^^^^^^^^^^^^^^^^^^^^^^^^

Remote monitoring via pyadi-iio:

.. code-block:: python

	import adi

	# Connect to IIO device
	dev = adi.ltc4284(uri="serial:/dev/ttyUSB0,115200")

	# Read telemetry data
	vin = dev.voltage_in  # Input voltage (V)
	iin = dev.current_in  # Input current (A)
	power = dev.power     # Power consumption (W)
	energy = dev.energy   # Cumulative energy (J)

	print(f"Input: {vin:.2f}V, {iin:.2f}A")
	print(f"Power: {power:.2f}W, Energy: {energy:.2f}J")

LTC4284 Driver Architecture
----------------------------

Register Interface
^^^^^^^^^^^^^^^^^^

The driver provides a complete register map based on the LTC4284 datasheet:

* **System Status Registers** (0x00-0x04): Status, faults, ADC status
* **Control Registers** (0x0A-0x0F): System control, configuration
* **GPIO Configuration** (0x10-0x12): GPIO and ADC I/O configuration
* **ADC Data Registers** (0x40-0x6C): Voltage, current, power, energy readings
* **Threshold Registers** (0x1E-0x3D): Min/max thresholds for alarm generation
* **EEPROM Registers** (0xA2-0xDC): Nonvolatile configuration storage

Multi-Mode Operation
^^^^^^^^^^^^^^^^^^^^^

The LTC4284 supports four operating modes for dual-gate MOSFET control:

1. **Single Driver Mode (Mode 1)**: Single MOSFET for lower power
2. **Parallel Mode (Mode 2)**: Dual parallel MOSFETs for high power
3. **High Stress Staged Start (Mode 3)**: Sequential turn-on for stress management
4. **Low Stress Staged Start (Mode 4)**: Optimized sequential turn-on

Mode selection is configured via CONFIG_1 register and external pin configuration.

Fault Protection
^^^^^^^^^^^^^^^^

The driver supports comprehensive fault detection and auto-retry:

* **Overcurrent (OC)**: Programmable current limit with foldback
* **Overvoltage (OV)**: Adjustable OV threshold with hysteresis
* **Undervoltage (UV)**: Adjustable UV threshold with hysteresis
* **FET Bad**: MOSFET failure detection
* **FET Short**: MOSFET short circuit detection
* **External Fault**: Integration with external fault monitors
* **Power Failed**: Power good monitoring with timeout

All faults support configurable auto-retry with programmable cooling delays.

LTC4284 Support
---------------

Hardware Platforms
^^^^^^^^^^^^^^^^^^

The driver has been tested on:

* **Maxim MAX32665** feather board (primary platform)
* **DC2470A** evaluation kit (LTC4284 demo board)

Additional platform support can be added by implementing the no-OS I²C platform
drivers for the target MCU.

Documentation
^^^^^^^^^^^^^

* **Datasheet**: https://www.analog.com/ltc4284
* **Evaluation Kit**: https://www.analog.com/DC2470A
* **Application Note**: Hot Swap Controller Design Guide

Support and Contact
^^^^^^^^^^^^^^^^^^^

For questions or support:

* **no-OS Repository**: https://github.com/analogdevicesinc/no-OS
* **IIO Support**: https://wiki.analog.com/resources/tools-software/linux-software/libiio
* **Technical Support**: https://www.analog.com/support
