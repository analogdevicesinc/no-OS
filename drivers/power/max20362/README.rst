MAX20362 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

`MAX20362`_

Overview
--------

The MAX20362 is a highly integrated Micro-Battery Power Management IC
(PMIC) designed for compact, power-sensitive applications with high peak load
currents and low power-density batteries, such as wearable medical and
wellness devices.

Key features include an ultra-low quiescent current buck-boost converter and
LDO, an energy buffering system using an external capacitor, a programmable
input current limit (5mA to 50mA), adjustable buck-boost output (1.5V to
5.5V, 50mV steps) and LDO output (0.9V to 4.0V, 100mV steps), Dynamic
Voltage Scaling (DVS) with round-robin support, and comprehensive interrupt
handling. The device is controlled via a 400kHz I2C-compatible serial
interface.

Applications
------------

* Fitness/Wellness Heart Rate Trackers
* Medical Vital Signs Patches
* Wearable and space-constrained battery-powered applications
* Energy harvesting systems requiring buffering

MAX20362 Device Configuration
-----------------------------

Driver Initialization
^^^^^^^^^^^^^^^^^^^^^^

To use the device, you must provide support for the I2C communication
protocol. The first API to be called is **max20362_init()**. A return value
of 0 indicates that the driver was initialized correctly.

Buck-Boost Converter Control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The core of the PMIC is a bidirectional buck-boost converter that regulates
the main output supply. It works with an external capacitor to buffer energy,
protecting the main battery from high peak currents.

* **max20362_enable_buck_boost()**: Enables or disables the buck-boost
  converter.
* **max20362_set_buck_boost_voltage()**: Sets the buck-boost output voltage
  from 1.5V to 5.5V in 50mV steps.
* **max20362_config_buck_boost_mode()**: Configures operating modes (half
  bandwidth, low EMI, fast FPWM).
* **max20362_config_buck_boost_discharge()**: Configures discharge modes
  (active/passive discharge).

Energy Storage Capacitor Control
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The external storage capacitor provides energy buffering to handle peak load
currents without draining the battery.

* **max20362_set_cap_voltage()**: Sets the target voltage for the external
  storage capacitor from 1.6V to 9.5V with configurable step sizes (500mV,
  250mV, or 125mV).

LDO Regulator Control
^^^^^^^^^^^^^^^^^^^^^

A separate low-quiescent-current LDO provides a clean, regulated output
suitable for powering sensitive components.

* **max20362_enable_ldo()**: Enables or disables the LDO.
* **max20362_set_ldo_voltage()**: Sets the LDO output voltage from 0.9V to
  4.0V in 100mV steps.
* **max20362_config_ldo_mode()**: Configures LDO operating modes (low IQ
  mode, discharge modes).
* **max20362_set_ldo_input_source()**: Selects LDO input source (buck-boost
  output, capacitor, or battery).

Dynamic Voltage Scaling (DVS)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The driver supports DVS to adjust the buck-boost output voltage in
real-time, enabling significant power savings.

* **max20362_set_dvs_mode()**: Configures the DVS source (I2C, Pseudo-SPI,
  or Round-Robin mode).
* **max20362_set_dvs_rr_table()**: Programs the Round-Robin voltage
  sequence table.

Power Management
^^^^^^^^^^^^^^^^

Additional power management functions provide fine control over current
limiting and battery protection.

* **max20362_set_input_current_limit()**: Configures the input current limit
  from 5mA to 50mA to protect the battery.
* **max20362_set_bbat_vdrop()**: Sets the battery voltage droop threshold
  (55mV, 100mV, 150mV, or 200mV).

Status and Interrupt Handling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The driver provides comprehensive functions for monitoring device status and
handling faults.

* **max20362_get_status()**: Reads the main status register.
* **max20362_get_int_status()**: Reads main interrupt status.
* **max20362_get_ldo_int_status()**: Reads LDO-specific interrupt status.
* **max20362_get_ingen_int_status()**: Reads Ingenuity interrupt status.
* **max20362_set_int_mask()**: Configures main interrupt mask.
* **max20362_set_ldo_int_mask()**: Configures LDO interrupt mask.
* **max20362_set_ingen_int_mask()**: Configures Ingenuity interrupt mask.
* **max20362_clear_all_interrupts()**: Clears all interrupt flags.

Register Protection
^^^^^^^^^^^^^^^^^^^

The device includes register lock/unlock functionality for protection against
accidental configuration changes.

* **max20362_unlock_registers()**: Unlocks registers for configuration
  changes.
* **max20362_lock_registers()**: Locks registers to prevent accidental
  modifications.

MAX20362 Driver Initialization Example
======================================

.. code-block:: c

    #include "max20362.h"
    #include "no_os_i2c.h"
    #include "no_os_error.h"

    int main()
    {
        int ret;
        struct max20362_dev *max20362_desc;

        /* I2C platform operations driver */
        extern struct no_os_i2c_platform_ops maxim_i2c_ops;

        /* I2C initialization parameter */
        struct no_os_i2c_init_param i2c_ip = {
            .device_id = 0,
            .max_speed_hz = MAX20362_I2C_CLK_SPEED,
            .slave_address = MAX20362_PMIC_I2C_ADDR,
            .platform_ops = I2C_OPS,
            .extra = (void *)&max20362_i2c_extra,
        };

        /* MAX20362 device initialization parameter */
        struct max20362_init_param max20362_ip = {
            .i2c_init = i2c_ip,
            .buck_boost_voltage_uv = 3300000,     /* 3.3V default */
            .cap_voltage_uv = 5000000,            /* 5V default */
            .ldo_voltage_uv = 1800000,            /* 1.8V default */
            .input_current_limit_ma = 20,         /* 20mA default */
            .bbat_vdrop = MAX20362_BBAT_VDROP_100MV,
            .buck_boost_enable = false,     /* Start disabled for testing */
            .ldo_enable = false,          /* Start disabled for testing */
        };

        ret = max20362_init(&max20362_desc, &max20362_ip);
        if (ret != NO_OS_SUCCESS) {
            // Initialization failed
            return ret;
        }

        /* Unlock registers for configuration */
        ret = max20362_unlock_registers(max20362_desc);
        if (ret != NO_OS_SUCCESS) {
            goto cleanup;
        }

        /* Example: Enable buck-boost with 3.3V output */
        ret = max20362_set_buck_boost_voltage(max20362_desc, 3300000);
        if (ret != NO_OS_SUCCESS) {
            goto cleanup;
        }

        ret = max20362_enable_buck_boost(max20362_desc, true);
        if (ret != NO_OS_SUCCESS) {
            goto cleanup;
        }

        /* Example: Enable LDO with 1.8V output */
        ret = max20362_set_ldo_voltage(max20362_desc, 1800000);
        if (ret != NO_OS_SUCCESS) {
            goto cleanup;
        }

        ret = max20362_enable_ldo(max20362_desc, true);
        if (ret != NO_OS_SUCCESS) {
            goto cleanup;
        }

        /* ... application code ... */

    cleanup:
        max20362_remove(max20362_desc);

        return ret;
    }

.. _MAX20362: https://www.analog.com/en/products/max20362.html
