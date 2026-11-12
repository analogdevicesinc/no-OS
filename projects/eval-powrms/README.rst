EVAL-POWRMS Evaluation Board
============================

.. contents::
    :depth: 3

Supported Parts
---------------

* `ADL5920 <https://www.analog.com/en/products/adl5920.html>`_
* `AD7091R5 <https://www.analog.com/en/products/ad7091r-5.html>`_
* `MAX77986 <https://www.analog.com/en/products/max77986.html>`_

Supported Evaluation Boards
---------------------------

* `EVAL-POWRMS <https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/eval-powrms.html>`_

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

The system supports calibration across 11 frequency ranges:

* 10 MHz
* 300 MHz
* 1000 MHz (1 GHz)
* 2000 MHz (2 GHz)
* 3000 MHz (3 GHz)
* 4000 MHz (4 GHz)
* 5000 MHz (5 GHz)
* 5250 MHz (5.25 GHz)
* 5500 MHz (5.5 GHz)
* 5750 MHz (5.75 GHz)
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

   # Available IIO channels (id -- name)
   iio_attr -c powrms voltage0     # v_in_cor   : corrected input voltage
   iio_attr -c powrms voltage1     # v_out_cor  : corrected output voltage
   iio_attr -c powrms voltage2     # tmp        : temperature
   iio_attr -c powrms voltage3     # v_in       : raw input voltage
   iio_attr -c powrms voltage4     # v_out      : raw output voltage
   iio_attr -c powrms power5       # p_fwd      : forward power
   iio_attr -c powrms power6       # p_rev      : reverse power
   iio_attr -c powrms voltage7     # precision_array : calibration attributes
   iio_attr -c powrms voltage8     # poly_calib_5000MHz
   iio_attr -c powrms voltage9     # poly_calib_5000MHz_reverse
   iio_attr -c powrms voltage10    # v_diff     : raw differential voltage

.. note::

   Channel indices come from a single shared enum in ``iio_powrms.h``, not from a
   per-type counter, so the power channels are ``power5``/``power6`` -- not
   ``power0``/``power1``. The examples below address channels by name
   (e.g. ``precision_array``), which is stable regardless of index.

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
   iio_attr -c powrms power5 raw       # Forward power raw (p_fwd)
   iio_attr -c powrms power6 raw       # Reverse power raw (p_rev)

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
* **EEPROM Storage**: Persistent storage of the temperature compensation value

**Temperature Coefficient Configuration:**

Each frequency range has three temperature compensation coefficients (A1, A2, A3).
These are build-time constants held in ``temperature_precision_values[]`` in
``src/examples/example/powrms_data_processing.c``. They are **not** exposed as IIO
attributes and are not read back from EEPROM, so changing them requires rebuilding
and reflashing the firmware.

The only temperature setting adjustable at runtime is the global compensation value:

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

Each frequency range requires 28 precision values -- 14 (X, Y) interpolation points
written as ``X1,Y1,X2,Y2,...,X14,Y14``. A write with any other number of values is
rejected with ``-EINVAL``.

.. code-block:: bash

   # Frequency-specific precision calibration values (forward power)
   iio_attr -c powrms precision_array calib_10MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_300MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_1000MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_2000MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_3000MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_4000MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_5000MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_5250MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_5500MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_5750MHz "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_6000MHz "X1,Y1,...,X14,Y14"

Each frequency range has a matching reverse-power attribute taking the same 28 values:

.. code-block:: bash

   # Frequency-specific precision calibration values (reverse power)
   iio_attr -c powrms precision_array calib_10MHz_reverse "X1,Y1,...,X14,Y14"
   iio_attr -c powrms precision_array calib_300MHz_reverse "X1,Y1,...,X14,Y14"
   # ... for all 11 frequency ranges

**Calibration Data Organization:**

* **Precision Values**: 28 values × 11 frequency ranges = 308 total precision coefficients
* **Temperature Coefficients**: 3 coefficients × 11 frequency ranges = 33 temperature correction values
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

   # Example: Upload custom precision values for the 10MHz range.
   # All 28 values (14 X,Y pairs) must be supplied in a single write.
   iio_attr -c powrms precision_array calib_10MHz "X1,Y1,X2,Y2,...,X14,Y14"

   # Same for the matching reverse-power range
   iio_attr -c powrms precision_array calib_10MHz_reverse "X1,Y1,X2,Y2,...,X14,Y14"

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
   iio_attr -c powrms precision_array calib_10MHz "X1,Y1,X2,Y2,...,X14,Y14"
   iio_attr -c powrms precision_array calib_10MHz_reverse "X1,Y1,X2,Y2,...,X14,Y14"
   # ... repeat for all 11 frequency ranges
   
   # Step 2: Set factory temperature compensation
   iio_attr -c powrms precision_array temperature_compensation_value 1.0
   
   # Step 3: **FACTORY OVERRIDE** - Save current values as factory defaults
   iio_attr -c powrms precision_array dev_mode_overwrite_def_calib 1

**Post-Factory Setup Workflow:**

After factory calibration is complete, end users should focus on the ``use_default_calibration`` attribute for normal operation:

.. code-block:: bash

   # Normal user operation - toggle between factory and custom calibration
   iio_attr -c powrms use_default_calibration 1  # Use factory defaults
   iio_attr -c powrms use_default_calibration 0  # Use custom user calibration

**Best Practices:**

* **Factory Use Only**: The ``dev_mode_overwrite_def_calib`` should only be used during initial factory setup
* **Documentation**: Always document factory calibration procedures and reference standards used
* **User Focus**: End users should only modify ``use_default_calibration`` and update user calibration values via standard IIO attributes
* **Backup**: Maintain backup copies of factory calibration data before any override operations

Building and Running
====================

Prerequisites
-------------

* Maxim SDK installed and configured
* GCC ARM toolchain
* CMake and Ninja

The project is built through the CMake/Kconfig flow driven by
``tools/scripts/no_os_build.py``. Point ``MAXIM_LIBRARIES`` at the Maxim SDK
``Libraries`` directory, then select the variant and board. The LVGL graphics
library is pulled in automatically by the build (matching the project's
``lv_conf.h``). For toolchain setup and prerequisites, see the
:doc:`Maxim CMake build guide </build_guides/build_maxim_cmake>`.

Available variants: ``eval_powrms``. Available boards: ``max32662evkit``.

**Build Commands:**

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>

   cd no-OS

   # build the project (eval_powrms variant on the max32662evkit board)
   python3 tools/scripts/no_os_build.py build \
       --project eval-powrms --variant eval_powrms --board max32662evkit

   # build and flash (requires a connected debug probe)
   python3 tools/scripts/no_os_build.py build \
       --project eval-powrms --variant eval_powrms --board max32662evkit \
       --probe openocd --flash

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
   ├── boards/              # Per-board Kconfig overlays
   ├── CMakeLists.txt       # Project build description
   ├── Kconfig              # Project configuration options
   ├── eval_powrms.conf     # Variant defconfig (drivers/libraries selected)
   ├── lv_conf.h            # LVGL configuration
   └── README.rst           # This documentation

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