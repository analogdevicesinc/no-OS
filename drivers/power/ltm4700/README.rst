LTM4700 no-OS Driver
====================

Supported Devices
-----------------

* `LTM4700 <https://www.analog.com/LTM4700>`_
* `LTM4777 <https://www.analog.com/LTM4777>`_

Overview
--------

The LTM4700 is a dual 50A or single 100A step-down µModule regulator featuring
remote configurability and telemetry-monitoring of power management parameters
over PMBus—an open standard I2C-based digital interface protocol.

The LTM4700 is comprised of fast analog control loops, precision mixed-signal
circuitry, EEPROM, power MOSFETs, inductors and supporting components.

Key Features
------------

* Dual 50A or Single 100A Digitally Adjustable Outputs
* Wide Input Voltage: 4.5V to 16V
* Output Voltage Range: 0.5V to 1.8V
* ±0.5% Maximum DC Output Error Over Temperature
* ±3% Current Readback Accuracy (25°C to 125°C)
* 400kHz PMBus-Compliant I2C Serial Interface
* Integrated 16-Bit ∆Σ ADC for telemetry
* Constant Frequency Current Mode Control
* Internal EEPROM with Fault Logging and ECC

Supported Features
------------------

* Device identification (LTM4700/LTM4777 variants)
* Dual-channel voltage and current monitoring
* LINEAR11/LINEAR16 data format conversion
* Comprehensive status monitoring
* PMBus command interface
* NVM operations for configuration storage
* Peak value tracking
* IIO framework integration

Communication Interface
-----------------------

The device uses PMBus 1.3 compliant I2C interface:

* **Bus Speed**: Up to 400kHz
* **Default Address**: 0x5A (7-bit)
* **Data Formats**: LINEAR11, LINEAR16
* **CRC Support**: Optional polynomial 0x7

Device Variants
---------------

+----------+----------------+-------------------------------------------+
| Device   | Special ID     | Description                               |
+==========+================+===========================================+
| LTM4700  | 0x4130         | Dual 50A or Single 100A µModule           |
+----------+----------------+-------------------------------------------+
| LTM4777  | 0x4131         | Variant with similar architecture         |
+----------+----------------+-------------------------------------------+

PMBus Commands
--------------

The driver supports standard PMBus commands and manufacturer-specific commands:

**Standard Commands:**
  - PAGE, OPERATION, CLEAR_FAULTS
  - VOUT_COMMAND, VOUT_MAX, VOUT_MARGIN_HIGH/LOW
  - STATUS_BYTE, STATUS_WORD, STATUS_VOUT, STATUS_IOUT
  - READ_VIN, READ_IIN, READ_VOUT, READ_IOUT
  - READ_TEMPERATURE_1/2, READ_POUT, READ_PIN

**Manufacturer Commands:**
  - MFR_SPECIAL_ID (device identification)
  - MFR_VOUT_PEAK, MFR_IOUT_PEAK, MFR_VIN_PEAK
  - MFR_CLEAR_PEAKS, MFR_ADC_CONTROL
  - MFR_PWM_MODE, MFR_PWM_COMP
  - Store/restore user settings

Data Formats
------------

**LINEAR11 Format:**
  - 5-bit signed exponent + 11-bit signed mantissa
  - Used for: current, power, temperature, time values

**LINEAR16 Format:**
  - Fixed exponent (-12) + 16-bit mantissa
  - Used for: voltage values (VOUT_COMMAND, READ_VOUT)

Channel Configuration
---------------------

The LTM4700 supports dual-channel operation:

* **Channel 0**: VOUT0, IOUT0, POUT0
* **Channel 1**: VOUT1, IOUT1, POUT1
* **Global**: VIN, IIN, PIN, temperatures

Pages are used to select channels for PMBus commands.

IIO Integration
---------------

The driver provides comprehensive IIO (Industrial I/O) support:

**IIO Channels:**
  - ``vin``: Input voltage monitoring
  - ``vout0/vout1``: Output voltage monitoring and control
  - ``iin``: Input current monitoring
  - ``iout0/iout1``: Output current monitoring
  - ``pin``: Input power monitoring
  - ``pout0/pout1``: Output power monitoring
  - ``temp0/temp1``: Temperature monitoring
  - ``frequency0/frequency1``: Switching frequency

**IIO Attributes:**
  - ``raw``: Read telemetry values
  - ``command``: Set output voltage
  - ``operation``: Control channel operation mode
  - ``clear_peaks``: Reset peak value tracking
  - ``device_id``: Device variant identification

Usage Example
-------------

Basic device initialization:

.. code-block:: c

    struct ltm4700_init_param init_param = {
        .i2c_init = &i2c_init_param,
        .crc_en = false,
        .id = ID_LTM4700
    };

    struct ltm4700_dev *dev;
    int ret = ltm4700_init(&dev, &init_param);

Reading telemetry:

.. code-block:: c

    int vin_mv, vout0_mv, iout0_ma;

    // Read input voltage in millivolts
    ltm4700_read_value(dev, 0, LTM4700_VIN, &vin_mv);

    // Read output voltage for channel 0
    ltm4700_read_value(dev, 0, LTM4700_VOUT, &vout0_mv);

    // Read output current for channel 0
    ltm4700_read_value(dev, 0, LTM4700_IOUT, &iout0_ma);

Setting output voltage:

.. code-block:: c

    // Set VOUT0 to 1.2V (1200mV)
    ltm4700_vout_value(dev, 0, 1200000, 1800000); // command, max

    // Set operation mode
    ltm4700_set_operation(dev, 0, LTM4700_OPERATION_ON);

IIO Usage:

.. code-block:: c

    struct ltm4700_iio_desc *iio_desc;
    struct ltm4700_iio_desc_init_param iio_init_param = {
        .ltm4700_init_param = &ltm4700_init_param
    };

    ltm4700_iio_init(&iio_desc, &iio_init_param);

Status monitoring:

.. code-block:: c

    struct ltm4700_status status;
    ltm4700_read_status(dev, 0, LTM4700_STATUS_ALL_TYPE, &status);

    if (status.vout & VOUT_OV_FAULT)
        printf("Output overvoltage fault detected\n");

Design Considerations
---------------------

**Power Supply Design:**
  - Provide adequate input capacitance for transient response
  - Follow recommended PCB layout guidelines
  - Consider thermal management for high current operation

**I2C Interface:**
  - Use appropriate pull-up resistors (1kΩ - 10kΩ)
  - Keep trace lengths short for high-speed operation
  - Consider CRC protection for noisy environments

**Dual Channel Operation:**
  - Channels can operate independently or in current sharing mode
  - Use PAGE command to select channel for configuration
  - Monitor both channels for fault conditions

**Fault Management:**
  - Implement appropriate fault handling in application
  - Use status registers to identify fault conditions
  - Consider automated recovery strategies

Build Configuration
-------------------

Add the LTM4700 driver to your project by including in ``src.mk``:

.. code-block:: make

    SRCS += $(DRIVERS)/power/ltm4700/ltm4700.c

For IIO support, also include:

.. code-block:: make

    SRCS += $(DRIVERS)/power/ltm4700/iio_ltm4700.c \
            $(LIBRARIES)/iio/iio.c

Dependencies:
  - I2C platform driver
  - no-OS utility functions
  - CRC8 library (if CRC enabled)
  - IIO framework (for IIO support)

Links
-----

* `LTM4700 Product Page <https://www.analog.com/LTM4700>`_
* `LTM4700 Datasheet <https://www.analog.com/media/en/technical-documentation/data-sheets/ltm4700.pdf>`_
* `PMBus Specification <https://pmbus.org/specification/>`_
* `no-OS Framework <https://github.com/analogdevicesinc/no-OS>`_