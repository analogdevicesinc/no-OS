EVAL-POWRMS Evaluation Board
============================

.. contents::
    :depth: 3

Overview
========

Eval-powrms is an evaluation kit built around ADL5920 which is an ultrawideband, bidirectional detector that simultaneously measures forward and reverse rms power levels in a signal path, along with the return loss. The firmware allows user to interact with setting and calibration via IIO or buttons on the PCB and can produce result both via embedded display or IIO.

Main Functionality
==================

The EVAL-POWRMS board offers the following core capabilities:

Power Measurement
-----------------

* **Forward Power (P_FWD)**: Real-time measurement of forward RF power with high precision
* **Reverse Power (P_REV)**: Accurate reverse power monitoring for VSWR analysis
* **Voltage Monitoring**: Multi-channel voltage measurement (VIN0, VIN1, VIN2) with both raw and temperature-corrected values
* **Temperature Compensation**: Advanced temperature correction algorithms for improved accuracy across operating conditions

ADC Channels
------------

The board utilizes multiple ADC channels for comprehensive signal acquisition:

* **Channel 0**: P_FWD (Power Forward) / VIN0 measurement
* **Channel 1**: P_REV (Power Reverse) / VIN1 measurement  
* **Channel 2**: VIN2 used for temperature compensation
* **4.096V Reference**: High-precision voltage reference for accurate measurements
* **12-bit Resolution**: 4096 count ADC for detailed signal digitization

Frequency Range Support
-----------------------

The system supports calibration across 8 frequency ranges:

* 10 MHz
* 100 MHz
* 1000 MHz (1 GHz)
* 2000 MHz (2 GHz)
* 3000 MHz (3 GHz)
* 4000 MHz (4 GHz)
* 5000 MHz (5 GHz)
* 6000 MHz (6 GHz)

User Interface Options
======================

The EVAL-POWRMS provides two primary interfaces for data access and configuration:

Button Interface with Display
-----------------------------

Interactive OLED Display Navigation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board features a comprehensive button-based navigation system with a high-resolution OLED display:

**Main Menu Navigation:**

* **'S2' key**: Switch between menu options (Show measurements / Settings)
* **'S3' key**: Select and enter the highlighted menu option

**Measurement Display Screen:**

* Real-time display of all measurement values:
  
  - V_in: Input voltage (corrected)
  - V_out: Output voltage (corrected) 
  - P_fwd: Forward power
  - P_rev: Reverse power

* **'S1' key**: Return to main menu

**Settings Configuration Screen:**

Interactive parameter adjustment interface:

* **'S2' key**: Navigate between digits and parameters
* **'S1' key**: Increment selected digit (edit mode) / Go back (navigation mode)  
* **'S3' key**: Enter/exit edit mode for precise value adjustment

**Configurable Parameters:**

* **Signal Frequency**: Set operating frequency for optimal calibration
* **Input Impedance**: Configure input impedance matching
* **Output Impedance**: Set output impedance parameters

IIO (Industrial I/O) Interface
------------------------------

Advanced Remote Control and Data Access
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board implements a comprehensive IIO interface for remote control and automated testing:

**Device Channels:**

.. code-block:: bash

   # Available IIO channels
   iio_attr -c powrms voltage0    # v_in_corrected
   iio_attr -c powrms voltage1    # v_out_corrected  
   iio_attr -c powrms voltage2    # temperature
   iio_attr -c powrms voltage3    # v_in_raw
   iio_attr -c powrms voltage4    # v_out_raw
   iio_attr -c powrms power0      # p_forward
   iio_attr -c powrms power1      # p_reverse

**Global Configuration Attributes:**

.. code-block:: bash

   # Set operating frequency (MHz)
   iio_attr -d powrms frequency_MHz 2400

   # Configure impedance values
   iio_attr -d powrms input_impedance 50.0
   iio_attr -d powrms output_impedance 50.0

   # Control calibration data usage
   iio_attr -d powrms use_default_calibration 1

**Reading Measurement Data:**

.. code-block:: bash

   # Read channel attributes
   iio_attr -c powrms voltage0 raw     # Raw ADC value
   iio_attr -c powrms voltage0 scale   # Scaling factor
   iio_attr -c powrms voltage0 offset  # Offset value

   # Read power measurements
   iio_attr -c powrms power0 raw       # Forward power raw
   iio_attr -c powrms power1 raw       # Reverse power raw

Calibration System
==================

Temperature Calibration
-----------------------

Advanced Temperature Compensation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-POWRMS implements a sophisticated temperature compensation system to maintain measurement accuracy across varying environmental conditions.

**Temperature Compensation Features:**

* **Temperature Sensing**: On-board temperature monitoring for real-time compensation
* **Multi-Coefficient Correction**: Three temperature coefficients per frequency range for precise modeling
* **Automatic Correction**: Real-time application of temperature compensation to all voltage measurements
* **EEPROM Storage**: Persistent storage of temperature calibration data

**Temperature Coefficient Configuration:**

Each frequency range supports three temperature compensation coefficients stored as calibration data:

.. code-block:: bash

   # Temperature calibration for each frequency range
   iio_attr -c powrms precision_array calib_temp_10MHz_values "coeff1,coeff2,coeff3"
   iio_attr -c powrms precision_array calib_temp_100MHz_values "coeff1,coeff2,coeff3"
   # ... for all 8 frequency ranges

**Temperature Compensation Value:**

.. code-block:: bash

   # Set global temperature compensation factor
   iio_attr -c powrms precision_array temperature_compensation_value 1.0

Frequency Calibration
---------------------

Frequency-Dependent Precision Correction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The system provides frequency-specific calibration to account for component variations and frequency response characteristics across the supported frequency ranges.

**Precision Value Configuration:**

Each frequency range requires six precision values for comprehensive calibration:

.. code-block:: bash

   # Frequency-specific precision calibration values
   iio_attr -c powrms precision_array calib_10MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_100MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_1000MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_2000MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_3000MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_4000MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_5000MHz_values "val1,val2,val3,val4,val5,val6"
   iio_attr -c powrms precision_array calib_6000MHz_values "val1,val2,val3,val4,val5,val6"

**Calibration Data Organization:**

* **48 Precision Values**: 6 values × 8 frequency ranges = 48 total precision coefficients
* **24 Temperature Coefficients**: 3 coefficients × 8 frequency ranges = 24 temperature correction values
* **EEPROM Persistence**: All calibration data stored in non-volatile memory

Custom Calibration Data Sets
=============================

Advanced Calibration Management
-------------------------------

The EVAL-POWRMS supports sophisticated calibration data management with separate storage areas for default factory calibration and user-configurable custom calibration sets.

**Calibration Data Structure:**

.. code-block:: text

   EEPROM Memory Layout:
   ┌─────────────────────────────────────────────────┐
   │ User Configurable Area                          │
   ├─────────────────────────────────────────────────┤
   │ 0x0002: use_default_calibration flag (1 byte)   │
   │ 0x0004: User precision array (192 bytes)        │
   │ 0x00C4: User temperature coeffs (96 bytes)      │
   │ 0x0114: User temp compensation value (4 bytes)  │
   ├─────────────────────────────────────────────────┤
   │ Default Factory Calibration Area                │
   ├─────────────────────────────────────────────────┤
   │ 0x0118: Default precision array (192 bytes)     │
   │ 0x01D8: Default temperature coeffs (96 bytes)   │
   │ 0x0238: Default temp compensation (4 bytes)     │
   └─────────────────────────────────────────────────┘

**Loading Custom Calibration Data:**

1. **Create Calibration File**: Prepare your custom calibration values in comma-separated format
2. **Upload via IIO**: Use IIO attributes to upload custom calibration data
3. **Enable Custom Mode**: Set use_default_calibration to 0

.. code-block:: bash

   # Example: Upload custom precision values for 10MHz range
   iio_attr -c powrms precision_array calib_10MHz_values "1.0000,0.9995,1.0005,0.9998,1.0002,0.9997"
   
   # Upload custom temperature coefficients for 10MHz range  
   iio_attr -c powrms precision_array calib_temp_10MHz_values "0.001,-0.0005,0.0008"
   
   # Set custom temperature compensation value
   iio_attr -c powrms precision_array temperature_compensation_value 1.0234

   # Enable custom calibration mode
   iio_attr -c powrms use_default_calibration 0

**Switching Between Calibration Sets:**

.. code-block:: bash

   # Use factory default calibration
   iio_attr -d powrms use_default_calibration 1
   
   # Use custom user calibration  
   iio_attr -d powrms use_default_calibration 0

**Reading Current Calibration Mode:**

.. code-block:: bash

   # Check which calibration set is active
   iio_attr -c powrms use_default_calibration

Factory Calibration Override (First-Time Setup)
------------------------------------------------

**IMPORTANT**: Factory Calibration Override for Manufacturing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The EVAL-POWRMS includes a special development mode attribute for **factory calibration and first-time setup only**. This feature allows manufacturers to establish the default factory calibration values.

**⚠️ WARNING**: This operation overwrites the factory default calibration data permanently and should only be used during initial factory calibration or by authorized personnel.

**Factory Calibration Procedure:**

1. **Load Optimal Calibration Values**: Use IIO to load the desired factory calibration values into the user area
2. **Verify Calibration Accuracy**: Test and validate the calibration across all frequency ranges and temperature conditions
3. **Execute Factory Override**: Trigger the factory calibration override to save as defaults

.. code-block:: bash

   # Step 1: Load factory-grade calibration values (example)
   iio_attr -c powrms precision_array calib_10MHz_values "1.0000,1.0000,1.0000,1.0000,1.0000,1.0000"
   iio_attr -c powrms precision_array calib_temp_10MHz_values "0.0,0.0,0.0"
   # ... repeat for all frequency ranges
   
   # Step 2: Set factory temperature compensation
   iio_attr -c powrms precision_array temperature_compensation_value 1.0
   
   # Step 3: **FACTORY OVERRIDE** - Save current values as factory defaults
   iio_attr -c powrms precision_array dev_mode_overwrite_def_calib_values 1

**Post-Factory Setup Workflow:**

After factory calibration is complete, end users should focus on the ``use_default_calibration`` attribute for normal operation:

.. code-block:: bash

   # Normal user operation - toggle between factory and custom calibration
   iio_attr -c powrms use_default_calibration 1  # Use factory defaults
   iio_attr -c powrms use_default_calibration 0  # Use custom user calibration

**Best Practices:**

* **Factory Use Only**: The ``dev_mode_overwrite_def_calib_values`` should only be used during initial factory setup
* **Documentation**: Always document factory calibration procedures and reference standards used
* **User Focus**: End users should only modify ``use_default_calibration`` and update user calibration values via standard IIO attributes
* **Backup**: Maintain backup copies of factory calibration data before any override operations

Building and Running
====================

Prerequisites
-------------

* Maxim SDK installed and configured
* GCC ARM toolchain
* Make build system

**Build Commands:**

.. code-block:: bash

   # Build the project
   make PLATFORM=maxim TARGET=max32662

   # Build and run
   make run PLATFORM=maxim TARGET=max32662

   # Clean build artifacts
   make clean

**Development Tasks:**

The project includes predefined VS Code tasks for development:

* **maxim Build**: Compile the project for MAX32662 target
* **maxim Run**: Build and execute the firmware on target hardware

**Project Structure:**

.. code-block:: text

   eval-powrms/
   ├── src/
   │   ├── common/          # Common data structures and syscalls  
   │   ├── examples/        # Main application logic
   │   │   ├── example/     # Core measurement and IIO functionality
   │   │   └── screens/     # OLED display interface screens
   │   └── platform/        # Platform-specific implementation
   │       └── maxim/       # MAX32662 platform support
   ├── build/               # Build artifacts and object files
   ├── Makefile            # Main build configuration
   ├── src.mk              # Source file definitions
   └── README.rst          # This documentation

**Firmware Version:**

The firmware reports version information through IIO context attributes, providing traceability for deployed systems.

Support and Documentation
=========================

For additional support, technical documentation, and application examples, please refer to:

* Analog Devices no-OS framework documentation
* MAX32662 microcontroller reference manual  
* IIO subsystem documentation for advanced interface usage
* EVAL-POWRMS hardware user guide and schematics

**Contact Information:**

* Documentation: https://github.com/analogdevicesinc/no-OS