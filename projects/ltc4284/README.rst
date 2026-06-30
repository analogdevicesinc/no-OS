LTC4284 Project
===============

Overview
--------

This project demonstrates the LTC4284 High Power Negative Voltage Hot Swap
Controller driver running on the Maxim MAX32665 platform, interfacing with
the DC2470A evaluation kit.

The LTC4284 is designed for -48V distributed power systems commonly used in
telecom infrastructure and data centers. It provides comprehensive hot swap
control, power monitoring, and fault protection.

Supported Evaluation Boards
----------------------------

* `DC2470A <https://www.analog.com/DC2470A>`_ - LTC4284 Evaluation Kit

Hardware Setup
--------------

**Required Hardware:**

* DC2470A Evaluation Kit (LTC4284 demo board)
* MAX32665 Feather Board (or compatible Maxim MCU)
* I2C cable/connections
* -48V power supply (for DC2470A)
* USB cable for UART console

**Connections:**

=================  =================  ===========================
DC2470A            MAX32665           Description
=================  =================  ===========================
SDA                P0.16 (I2C0_SDA)   I2C data line
SCL                P0.17 (I2C0_SCL)   I2C clock line
ALERT# (optional)  GPIO pin           Alert interrupt (optional)
GND                GND                Common ground
=================  =================  ===========================

**Power Configuration:**

* DC2470A: Connect -48V input power supply
* MAX32665: Connect USB for power and UART console
* Ensure common ground connection between boards

**I2C Address Configuration:**

The LTC4284 I2C address is configured via ADR0 and ADR1 pins:

======  ======  ==============
ADR0    ADR1    Address
======  ======  ==============
GND     GND     0x28
GND     INTVCC  0x29
INTVCC  GND     0x2A (default)
INTVCC  INTVCC  0x2B
======  ======  ==============

Software Setup
--------------

**Build Configuration:**

The project supports multiple build configurations defined in ``builds.json``:

.. code-block:: json

   {
       "maxim": {
           "basic_max32665": {
               "flags": "EXAMPLE=basic TARGET=max32665"
           },
           "iio_max32665": {
               "flags": "EXAMPLE=iio TARGET=max32665"
           }
       }
   }

**Build Commands:**

Basic monitoring example:

.. code-block:: bash

   cd projects/ltc4284
   make EXAMPLE=basic TARGET=max32665

IIO integration example:

.. code-block:: bash

   cd projects/ltc4284
   make EXAMPLE=iio TARGET=max32665

**Programming the MAX32665:**

Using OpenOCD:

.. code-block:: bash

   openocd -f interface/cmsis-dap.cfg -f target/max32665.cfg \
       -c "program build/ltc4284.elf verify reset exit"

Using pyOCD:

.. code-block:: bash

   pyocd flash -t max32665 build/ltc4284.elf

Running the Examples
--------------------

Basic Example
~~~~~~~~~~~~~

The basic example demonstrates continuous monitoring of the LTC4284:

1. Flash the basic example firmware
2. Connect UART console (115200 baud, 8N1)
3. Reset the MAX32665 board
4. Observe monitoring output

**Expected Output:**

.. code-block:: text

   LTC4284 Basic Example
   =====================

   LTC4284 initialized successfully
   I2C Address: 0x2A
   RSENSE: 500 mohm
   VSENSE: 25 mV

   Initial Status: 0x00
   Enabling FET drivers...

   Continuous Monitoring (Ctrl+C to stop):
   ------------------------------------------
   VIN: 48120 mV | IIN:  1250 mA | VOUT: 47850 mV | Power:  60150 mW | Energy:  125000000 mJ | Temp: 25.375 C
   VIN: 48115 mV | IIN:  1248 mA | VOUT: 47845 mV | Power:  60100 mW | Energy:  125060150 mJ | Temp: 25.500 C

**Features:**

* Real-time monitoring of VIN, IIN, VOUT, Power, Energy, Temperature
* Automatic fault detection and clearing
* FET enable/disable control
* 1 Hz update rate

IIO Example
~~~~~~~~~~~

The IIO example provides network-accessible telemetry via the Industrial I/O
framework:

1. Flash the IIO example firmware
2. Connect UART console (115200 baud, 8N1)
3. Reset the MAX32665 board
4. Use pyadi-iio or libiio to access telemetry

**Expected Output:**

.. code-block:: text

   LTC4284 IIO Example
   ===================

   LTC4284 IIO initialized successfully
   IIO device: ltc4284
   Channels: VIN, IIN, VOUT, Power, Energy

   IIO app initialized successfully

   Access via:
     - pyadi-iio: uri='serial:/dev/ttyUSB0,115200'
     - libiio: iiod -F ttyUSB0,115200

   Running IIO app (Ctrl+C to stop)...

**Using pyadi-iio:**

.. code-block:: python

   import adi

   # Connect to LTC4284 via UART
   dev = adi.ltc4284(uri="serial:/dev/ttyUSB0,115200")

   # Read voltage measurements
   vin = dev.voltage_in       # Input voltage in volts
   vout = dev.voltage_out     # Output voltage in volts

   # Read current
   iin = dev.current_in       # Input current in amps

   # Read power and energy
   power = dev.power          # Power in watts
   energy = dev.energy        # Cumulative energy in joules

   print(f"VIN: {vin:.3f} V, IIN: {iin:.3f} A, Power: {power:.3f} W")

**Using libiio:**

.. code-block:: bash

   # List devices
   iio_info -u serial:/dev/ttyUSB0,115200

   # Read channel attributes
   iio_attr -u serial:/dev/ttyUSB0,115200 -d ltc4284 -c voltage0 raw
   iio_attr -u serial:/dev/ttyUSB0,115200 -d ltc4284 -c current0 raw
   iio_attr -u serial:/dev/ttyUSB0,115200 -d ltc4284 -c power0 raw

Configuration
-------------

**Current Sense Resistor (RSENSE):**

The current sensing configuration is defined in ``common_data.c``:

.. code-block:: c

   struct ltc4284_init_param ltc4284_ip = {
       .rsense_mohm = 500,  // 0.5 mΩ sense resistor
       .vsense_mv = 25,     // 25 mV sense voltage threshold
   };

Supported RSENSE values: 100 - 10000 mΩ (0.1 mΩ to 10 mΩ)

**Sense Voltage Threshold (VSENSE):**

Supported values: 15 mV, 20 mV, 25 mV, 30 mV

**Current Limit Calculation:**

.. code-block:: text

   I_LIMIT = V_SENSE / R_SENSE

   Example: 25 mV / 0.5 mΩ = 50 A maximum current

Troubleshooting
---------------

**I2C Communication Failures:**

* Verify I2C connections (SDA, SCL, GND)
* Check I2C address configuration (ADR0, ADR1 pins)
* Ensure pull-up resistors on I2C lines (typically 2.2kΩ to 10kΩ)
* Verify DC2470A has power applied

**Device Not Responding:**

* Check that DC2470A has -48V power supply connected
* Verify LTC4284 is not in shutdown mode
* Check ALERT# pin if fault condition is latched
* Try power cycling both boards

**Incorrect Measurements:**

* Verify RSENSE value in ``common_data.c`` matches hardware
* Check VSENSE configuration
* Ensure load is connected to DC2470A output
* Verify ADC_CONTROL register configuration

**Build Errors:**

* Ensure correct TARGET specified (max32665)
* Verify EXAMPLE variable (basic or iio)
* Check that all dependencies are included in ``src.mk``
* Review platform-specific includes in ``platform_src.mk``

**UART Console Not Working:**

* Verify baud rate: 115200, 8N1, no flow control
* Check USB cable connection
* Ensure correct COM port or /dev/ttyUSB device
* Verify UART pins on MAX32665 (default UART0)

Support
-------

For additional support:

* `LTC4284 Product Page <https://www.analog.com/ltc4284>`_
* `DC2470A Evaluation Kit <https://www.analog.com/DC2470A>`_
* `LTC4284 Datasheet <https://www.analog.com/media/en/technical-documentation/data-sheets/ltc4284.pdf>`_
* `no-OS Driver Documentation <https://wiki.analog.com/resources/tools-software/uc-drivers/no-os>`_
