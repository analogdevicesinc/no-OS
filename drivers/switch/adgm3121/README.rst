ADGM3144/ADGM3121 MEMS Switch Driver
====================================

Supported Devices
-----------------

* `ADGM3144 <https://www.analog.com/ADGM3144>`_ - SP4T Switch
* `ADGM3121 <https://www.analog.com/ADGM3121>`_ - DPDT Switch

Overview
--------

The ADGM3144 and ADGM3121 are 3mm x 3mm MEMS switches operating from DC to 23 GHz:

* **ADGM3144**: Single-Pole Four-Throw (SP4T) switch with RF1-4 ports connected to RFC
* **ADGM3121**: Dual-Pole Dual-Throw (DPDT) switch with RF1A/1B, RF2A/2B ports connected to RFCA/RFCB

This driver supports both devices and control modes:

1. **Parallel GPIO Mode**: Direct control via IN1-IN4 GPIO pins
2. **SPI Mode**: Control via SPI interface with register access

Key Features:
- DC to 23 GHz frequency range
- Low insertion loss: -0.5 dB (typical) at 6 GHz  
- High isolation: -22 dB (typical) at 6 GHz
- Fast switching time: 200 µs (typical)
- On resistance: 1.9 Ω (typical)
- High cycle count: 100 million cycles minimum at +85°C

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The driver supports two operating modes, selected via the PIN/SPI control pin:

**Parallel GPIO Mode Example (ADGM3144 SP4T):**

.. code-block:: c

    struct adgm3121_dev *dev;
    struct adgm3121_init_param init_param = {
        .mode = ADGM3121_MODE_PARALLEL,
        .gpio_pin_spi = {
            .number = PIN_SPI_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        },
        .gpio_in1 = {
            .number = IN1_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        },
        .gpio_in2 = {
            .number = IN2_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        },
        .gpio_in3 = {
            .number = IN3_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        },
        .gpio_in4 = {
            .number = IN4_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        }
    };
    
    ret = adgm3121_init(&dev, &init_param);

**SPI Mode Example (ADGM3121 DPDT):**

.. code-block:: c

    struct adgm3121_dev *dev;
    struct adgm3121_init_param init_param = {
        .mode = ADGM3121_MODE_SPI,
        .gpio_pin_spi = {
            .number = PIN_SPI_GPIO_NUM,
            .platform_ops = &gpio_ops,
            .extra = &gpio_extra
        },
        .spi_init = {
            .device_id = SPI_DEVICE_ID,
            .mode = NO_OS_SPI_MODE_0,
            .bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
            .max_speed_hz = 10000000, // 10 MHz max
            .chip_select = SPI_CS,
            .platform_ops = &spi_ops,
            .extra = &spi_extra
        }
    };
    
    ret = adgm3121_init(&dev, &init_param);

Basic Usage
~~~~~~~~~~~

**Individual Switch Control:**

.. code-block:: c

    // ADGM3144: Enable switch 1 (RF1 to RFC)
    // ADGM3121: Enable switch 1 (RF1A to RFCA) 
    ret = adgm3121_set_switch_state(dev, ADGM3121_SW1, ADGM3121_ENABLE);
    
    // ADGM3144: Disable switch 2 (RF2 to RFC)
    // ADGM3121: Disable switch 2 (RF1B to RFCA)
    ret = adgm3121_set_switch_state(dev, ADGM3121_SW2, ADGM3121_DISABLE);
    
    // Check switch state
    enum adgm3121_state state;
    ret = adgm3121_get_switch_state(dev, ADGM3121_SW1, &state);

**Multiple Switch Control:**

.. code-block:: c

    // Enable switches 1 and 3 simultaneously
    ret = adgm3121_set_switches(dev, 0x05); // Binary: 0101
    
    // Get all switch states
    uint8_t switch_mask;
    ret = adgm3121_get_switches(dev, &switch_mask);
    
    // Reset all switches to off
    ret = adgm3121_reset_switches(dev);

SPI Mode Advanced Features
~~~~~~~~~~~~~~~~~~~~~~~~~~

**Daisy Chain Mode:**

.. code-block:: c

    // Enter daisy chain mode for multiple devices
    ret = adgm3121_enter_daisy_chain_mode(dev);
    
    // In daisy chain mode, use adgm3121_set_switches() with shift register data

**Error Detection:**

.. code-block:: c

    uint8_t error_status;
    ret = adgm3121_check_internal_error(dev, &error_status);
    if (error_status != 0) {
        // Handle internal error
        printf("Internal error detected: 0x%02X\\n", error_status);
    }

**Direct Register Access:**

.. code-block:: c

    uint8_t switch_data = 0x05; // Enable SW1 and SW3
    ret = adgm3121_write_register(dev, ADGM3121_REG_SWITCH_DATA, switch_data);
    
    uint8_t read_data;
    ret = adgm3121_read_register(dev, ADGM3121_REG_SWITCH_DATA, &read_data);

Hardware Configuration
~~~~~~~~~~~~~~~~~~~~~~

**Pin Connections:**

+----------+----------------------------------+------------------------------------+
| Pin      | ADGM3144                         | ADGM3121                           |
+==========+==================================+====================================+
| VDD      | 3.0V to 3.6V power supply       | 3.0V to 3.6V power supply           |
+----------+----------------------------------+------------------------------------+
| VCP      | 80V charge pump output           | 80V charge pump output             |
+----------+----------------------------------+------------------------------------+
| GND      | Ground                           | Ground                             |
+----------+----------------------------------+------------------------------------+
| RF Ports | RF1-RF4 (SP4T)                   | RF1A, RF1B, RF2A, RF2B (DPDT)      |
+----------+----------------------------------+------------------------------------+
| RFC Port | RFC (common)                     | RFCA, RFCB (dual common)           |
+----------+----------------------------------+------------------------------------+
| PIN/SPI  | Mode select (Parallel, SPI)      | Mode select (Parallel, SPI)        |
+----------+----------------------------------+------------------------------------+
| IN1-IN4  | Parallel control pins            | Parallel control pins              |
+----------+----------------------------------+------------------------------------+
| SDI      | SPI data input                   | SPI data input                     |
+----------+----------------------------------+------------------------------------+
| SDO      | SPI data output                  | SPI data output                    |
+----------+----------------------------------+------------------------------------+
| SCLK     | SPI clock                        | SPI clock                          |
+----------+----------------------------------+------------------------------------+
| CS       | SPI chip select                  | SPI chip select                    |
+----------+----------------------------------+------------------------------------+

**External Components:**

- 0.1 µF decoupling capacitor on VDD (rated ≥16V)
- 0.1 nF decoupling capacitor on VCP (rated ≥100V)
- 50Ω termination on unused RF ports

Performance Specifications
~~~~~~~~~~~~~~~~~~~~~~~~~~

+---------------------------+------------------+----------+
| Parameter                 | Typical          | Unit     |
+===========================+==================+==========+
| Frequency Range           | DC to 23         | GHz      |
+---------------------------+------------------+----------+
| Insertion Loss @ 6GHz     | -0.5             | dB       |
+---------------------------+------------------+----------+
| Isolation @ 6GHz          | -22              | dB       |
+---------------------------+------------------+----------+
| Return Loss @ 6GHz        | -24.5            | dB       |
+---------------------------+------------------+----------+
| On Resistance             | 1.9              | Ω        |
+---------------------------+------------------+----------+
| Switching Time            | 200              | µs       |
+---------------------------+------------------+----------+
| Power-up Time             | 45               | ms       |
+---------------------------+------------------+----------+
| Max RF Power              | 33               | dBm      |
+---------------------------+------------------+----------+
| Max DC Current            | 200              | mA       |
+---------------------------+------------------+----------+
| Cycle Lifetime            | 100M             | cycles   |
+---------------------------+------------------+----------+

API Reference
~~~~~~~~~~~~~

**Initialization Functions:**

- ``adgm3121_init()`` - Initialize device
- ``adgm3121_remove()`` - Free resources

**Switch Control Functions:**

- ``adgm3121_set_switch_state()`` - Set individual switch state
- ``adgm3121_get_switch_state()`` - Get individual switch state  
- ``adgm3121_set_switches()`` - Set multiple switches with bitmask
- ``adgm3121_get_switches()`` - Get all switch states as bitmask
- ``adgm3121_reset_switches()`` - Turn off all switches

**SPI Mode Functions:**

- ``adgm3121_write_register()`` - Write to device register
- ``adgm3121_read_register()`` - Read from device register
- ``adgm3121_enter_daisy_chain_mode()`` - Enable daisy chain mode
- ``adgm3121_check_internal_error()`` - Check for internal errors

Troubleshooting
~~~~~~~~~~~~~~~

**Common Issues:**

1. **Switch not responding:** Check VDD supply (3.0-3.6V) and wait 45ms after power-up
2. **SPI communication failure:** Verify SPI mode, speed (≤10MHz), and CS timing
3. **High insertion loss:** Check for proper grounding and 50Ω terminations
4. **Internal errors:** Use ``adgm3121_check_internal_error()`` to diagnose

**ESD Precautions:**

- RF pins: 150V HBM, 1kV FICDM rating
- Use proper ESD protection during handling
- Avoid hot switching (applying RF power during switching transitions)