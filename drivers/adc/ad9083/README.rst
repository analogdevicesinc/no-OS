AD9083 no-OS Driver
===================

Supported Devices
-----------------

- :adi:`AD9083`

Overview
--------

The AD9083 is a 16-channel, 125MHz bandwidth continuous-time sigma-delta (CTSD)
analog-to-digital converter (ADC) that incorporates an integrated programmable
single-pole antialiasing filter and termination resistor. It is designed to
offer low power consumption and a compact form factor, ensuring ease of use
for a wide range of applications. Each ADC core is composed of a first-order
CTSD modulator, embedded background nonlinearity correction, and self-cancelling
dither to accommodate wide bandwidth differential inputs. The device features
a built-in voltage reference, cascaded integrator comb (CIC) filter, and a
quadrature digital downconverter (DDC) with FIR decimation filters. It delivers
high-speed Subclass 1 JESD204B serialized output, configurable in various lane
arrangements, and supports synchronization across multiple devices. Additionally,
the AD9083 provides flexible power-down options and is programmable via a 1.8V
compatible 3-wire SPI interface. It operates within a temperature range of −40°C
to +85°C and is available in a 100-ball CSP_BGA package.

Applications
------------

- Millimeter wave imaging
- Electronic beam forming and phased arrays
- Multichannel wireless receivers
- Electronic support measures

Operation Modes
---------------

+--------------------------------------+-------------------------+-------------------------+
| Mode Name                            | Description             | Typical Use Case        |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_DISABLED    | Disable Test Mode for   | Use when no test mode   |
|                                      | the JESD transmitter.   | is necessary.           |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_CHECKER_BOARD | Checkerboard Test Mode | Testing of data        |
|                                      | for sending checkerboard| integrity in JESD       |
|                                      | pattern data.           | interface.              |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_WORD_TOGGLE | Word Toggle Test Mode   | Verification of JESD    |
|                                      | for toggling word       | interface by toggling   |
|                                      | patterns.               | words.                  |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_PN31        | PN31 Test Mode for      | Performance testing     |
|                                      | sending pseudo-random   | using pseudo-random     |
|                                      | binary sequence data.   | data.                   |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_PN15        | PN15 Test Mode based on | Used in scenarios       |
|                                      | pseudo-random patterns. | requiring less          |
|                                      |                         | complexity than PN31.   |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_PN7         | PN7 Test Mode operating | For less extensive      |
|                                      | with simplified         | testing environments.   |
|                                      | pseudo-random patterns. |                         |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_RAMP        | Ramp Test Mode          | Simulation of           |
|                                      | simulating ramping data | continuous increase in  |
|                                      | stream.                 | data which aids in      |
|                                      |                         | buffer testing.         |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_USER_REPEAT | Repeated User Data Test | Validation with known   |
|                                      | Mode for sending same   | static patterns         |
|                                      | data patterns repeated  | repeated over time.     |
|                                      | over time.              |                         |
+--------------------------------------+-------------------------+-------------------------+
| AD9083_JESD_TX_TEST_MODE_USER_SINGLE | Single Time User Data   | Used for transient data |
|                                      | Test Mode for one-off   | verification.           |
|                                      | pattern testing.        |                         |
+--------------------------------------+-------------------------+-------------------------+

Device Configuration
--------------------

Initialization and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9083.c`` file encompasses critical functions necessary for the
initialization and configuration of the AD9083 device. The
``ad9083_init`` function facilitates device preparation by allocating
required resources, configuring GPIOs and SPI communication, and
validating the chip ID against expected values. Subsequently, it invokes
the ``ad9083_setup`` function to establish parameters such as clock
settings and ADC configurations based on a specified configuration
index. During the setup process, the function executes a software reset
and device initialization, while also adjusting JESD204 subclass
settings as required. The ``ad9083_remove`` function ensures proper
resource deallocation by releasing memory and removing SPI and GPIO
descriptors when the device is no longer in use.

Register and SPI Communication
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

SPI communication is facilitated through functions like
``ad9083_reg_get`` and ``ad9083_reg_set``, which handle reading and
writing to device registers. ``ad9083_reg_get`` uses the device’s SPI
descriptor to read a specified register, while ``ad9083_reg_set`` writes
a value to a register. Both ensure valid addresses and handle errors.
The ``ad9083_spi_xfer`` function manages data flow during SPI transfers,
checking data sizes and ensuring communication success.

Utility
~~~~~~~

Utility functions in the file support the AD9083’s management and
operation. ``ad9083_log_write`` logs messages by severity levels,
including warnings and errors. ``ad9083_udelay`` provides
microsecond-level delays for synchronizing operations, and
``ad9083_reset_pin_ctrl`` manages the device’s reset state via GPIOs,
enabling programmatic resets. These utilities support robust operation
and debugging.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "ad9083.h"
   #include "no_os_spi.h"
   #include "no_os_gpio.h"

   // Define the initialization parameters
   struct ad9083_init_param init_param = {
       .spi_init = {
           .max_speed_hz = 1000000,
           .chip_select = 0,
           .mode = NO_OS_SPI_MODE_0,
           .extra = NULL,
       },
       .gpio_reset = {
           .number = 0,
           .extra = NULL,
       },
       .some_settings = 0, // Add any device-specific settings here
   };

   int main(void) {
       struct ad9083_dev *dev;
       int ret;

       // Initialize the device
       ret = ad9083_init(&dev, &init_param);
       if (ret) {
           printf("Initialization failed!\n");
           return ret;
       }

       // Use the device (e.g., read data, configure settings)
       // ...

       // Clean up
       ad9083_remove(dev);

       return 0;
   }
