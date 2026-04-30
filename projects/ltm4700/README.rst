LTM4700 no-OS Project
=====================

Overview
--------

This project demonstrates the LTM4700 dual-channel µModule regulator driver for the no-OS framework. The LTM4700 is a high-performance, dual 50A or single 100A step-down regulator featuring digital power management through PMBus interface.

The project supports both basic telemetry monitoring and advanced IIO (Industrial I/O) integration for real-time data acquisition and control.

Supported Hardware
------------------

**Device Support:**
* `LTM4700 <https://www.analog.com/LTM4700>`_ - Dual 50A or Single 100A µModule Regulator
* `LTM4777 <https://www.analog.com/LTM4777>`_ - Compatible variant with similar architecture

**Evaluation Platforms:**
* Maxim MAX32655 Feather Board
* STM32 platforms (with appropriate SDP-K1 setup)
* Any platform supporting I2C/PMBus communication

Hardware Setup
--------------

**Connections for MAX32655:**

===========  ================  ===========================================
Function     MAX32655 Pin      Description
===========  ================  ===========================================
PMBus SDA    P0.6 (I2C0_SDA)   Data line for PMBus communication
PMBus SCL    P0.7 (I2C0_SCL)   Clock line for PMBus communication
UART TX      P0.1 (UART1_TX)   Console output for data logging
UART RX      P0.0 (UART1_RX)   Console input (optional)
VDD          3.3V              Power supply for MAX32655
GND          GND               Ground connection
===========  ================  ===========================================

**PMBus Configuration:**
* Bus Speed: 400kHz (PMBus 1.3 compliant)
* Device Address: 0x5A (7-bit, configurable via ADDR pin)
* Pull-up Resistors: 1kΩ on SDA/SCL lines

Build Instructions
------------------

**Prerequisites:**
* ARM GCC toolchain for embedded development
* no-OS framework dependencies
* Make build system

**Build Commands:**

.. code-block:: bash

    # Build for MAX32655 platform
    cd projects/ltm4700
    make PLATFORM=maxim TARGET=max32655

    # Build with basic example (telemetry monitoring)
    make PLATFORM=maxim TARGET=max32655 BASIC_EXAMPLE=y

    # Build with IIO example (Industrial I/O integration)
    make PLATFORM=maxim TARGET=max32655 IIO_EXAMPLE=y

**Build Targets:**

=========  ===========  ============================================
Platform   Target       Description
=========  ===========  ============================================
maxim      max32655     Maxim MAX32655 Feather Board
stm32      stm32f469    STM32F469 Discovery Board
=========  ===========  ============================================

Project Examples
----------------

Two examples are provided to demonstrate different usage patterns:

Basic Example
~~~~~~~~~~~~~

The basic example demonstrates fundamental LTM4700 operations:

* Device initialization and variant detection
* Dual-channel voltage and current monitoring
* Real-time telemetry logging via UART console
* Temperature monitoring (IC and external sensors)
* Simple loop-based data acquisition

**Features:**
- Automatic device detection (LTM4700/LTM4777)
- 500ms update interval for telemetry
- Channel-specific monitoring (VOUT0, VOUT1, IOUT0, IOUT1)
- Input power monitoring (VIN, IIN)
- Temperature readings in Celsius

**Sample Output:**

.. code-block:: text

    CH0: vin = 12000 mV | vout = 1200 mV | iout = 15000 mA | temp_ic = 45 C
    CH1: vin = 12000 mV | vout = 3300 mV | iout = 8500 mA | temp_ic = 45 C

IIO Example
~~~~~~~~~~~

The IIO example provides advanced integration capabilities:

* Industrial I/O framework integration
* Real-time data streaming via IIO channels
* Remote configuration and control
* Attribute-based device interaction
* Integration with Linux IIO subsystem

**IIO Channels:**
- ``vin``: Input voltage monitoring
- ``vout0``, ``vout1``: Output voltage monitoring/control
- ``iin``: Input current monitoring
- ``iout0``, ``iout1``: Output current monitoring
- ``pin``: Input power monitoring
- ``pout0``, ``pout1``: Output power monitoring
- ``temp0``, ``temp1``: Temperature monitoring
- Device identification and status attributes

**Usage with IIO Tools:**

.. code-block:: bash

    # List available IIO devices
    iio_info

    # Read input voltage
    iio_attr -c ltm4700 vin raw

    # Set output voltage for channel 0 (in mV)
    iio_attr -c ltm4700 vout0 command 1200

Configuration Options
---------------------

**Makefile Variables:**

=================  ========  ==============================================
Variable           Default   Description
=================  ========  ==============================================
BASIC_EXAMPLE      n         Enable basic telemetry monitoring example
IIO_EXAMPLE        y         Enable IIO integration example
PLATFORM           maxim     Target platform (maxim, stm32, etc.)
TARGET             max32655  Specific target board configuration
=================  ========  ==============================================

**Device Configuration:**

The LTM4700 supports extensive configuration options:

* **Voltage Output**: 0.5V to 1.8V range with 1mV resolution
* **Current Monitoring**: ±3% accuracy from 25°C to 125°C
* **PMBus Commands**: Standard and manufacturer-specific commands
* **Data Formats**: LINEAR11, LINEAR16 automatic conversion
* **Multi-Channel**: Independent or synchronized operation

Troubleshooting
---------------

**Common Issues:**

**Device Not Detected:**

.. code-block:: text

    Error: Device initialization failed (-5)

* Verify I2C connections (SDA/SCL)
* Check power supply to LTM4700
* Confirm device address (default 0x5A)
* Ensure pull-up resistors on I2C lines

**Communication Errors:**

.. code-block:: text

    Error: PMBus read failed (-110)

* Check I2C bus speed (400kHz max)
* Verify ground connections
* Test with basic I2C scan tools
* Check for bus conflicts

**Build Errors:**

.. code-block:: text

    Error: platform_includes.h not found

* Ensure PLATFORM variable is set correctly
* Verify no-OS framework installation
* Check PATH environment variables
* Clean and rebuild project

**Voltage Reading Issues:**

* Verify LINEAR format conversion
* Check reference voltage configuration
* Confirm channel selection (PAGE commands)
* Test with known voltage sources

Performance Characteristics
---------------------------

**Data Acquisition Rates:**
* Basic Example: 2Hz update rate (500ms interval)
* IIO Example: Up to 100Hz (limited by PMBus speed)

**Accuracy:**
* Voltage: ±0.5% maximum error over temperature
* Current: ±3% accuracy from 25°C to 125°C
* Temperature: ±5°C accuracy for IC temperature

**Resource Usage:**
* Flash: ~15KB for basic functionality
* RAM: ~2KB for driver data structures
* Stack: ~1KB for operation functions

Integration Notes
-----------------

**no-OS Framework Integration:**
* Uses standard no-OS I2C platform drivers
* Compatible with all supported platforms
* Integrated error handling and logging
* Consistent API across platforms

**Linux IIO Integration:**
* Seamless integration with Linux IIO subsystem
* Support for buffered data acquisition
* Real-time attribute updates
* Compatible with standard IIO tools

**Multi-Platform Support:**
* Abstracted platform layer for portability
* Platform-specific optimizations
* Consistent behavior across targets
* Easy porting to new platforms

Further Information
-------------------

**Related Documentation:**
* `LTM4700 Product Page <https://www.analog.com/LTM4700>`_
* `LTM4700 Datasheet <https://www.analog.com/media/en/technical-documentation/data-sheets/ltm4700.pdf>`_
* `PMBus Specification <https://pmbus.org/specification/>`_
* `no-OS Framework Documentation <https://analogdevicesinc.github.io/no-OS/>`_

**Driver Documentation:**
* ``drivers/power/ltm4700/README.rst`` - Complete driver documentation
* ``drivers/power/ltm4700/ltm4700.h`` - API reference
* ``drivers/power/ltm4700/iio_ltm4700.h`` - IIO integration details

**Support:**
* For hardware questions: Contact ADI Customer Support
* For software issues: Submit issues to no-OS GitHub repository
* For design consultation: Contact ADI Field Applications Engineers