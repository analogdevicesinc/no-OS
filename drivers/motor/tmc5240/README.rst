TMC5240 no-OS driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`TMC5240`

Overview
--------
The TMC5240 is a high-performance stepper motor driver IC from Trinamic (now Analog Devices).
It integrates motion control and driver stages for two-phase bipolar stepper motors.

Applications
------------
- Textile, Sewing Machines, Knitting Machines 
- Lab and Factory Automation 
- 3D Printers, ID Printers/Card Printers 
- Liquid Handling, Medical Applications 
- Office Automation and Paper Handling 
- Point-of-Sale (POS), Massage Chairs 
- Automated Teller Machine (ATM), Cash Recycler, Bill Validators, Cash Machines 
- Closed-Circuit Television (CCTV), Security 
- Pumps and Valve Control 
- Heliostat and Antenna Positioning 

Driver Source Code
------------------------
The TMC5240 driver provides wrappers to the TMC5240 stepper motor controller driver from the TMC-API,
offering register-level access and basic configuration functions for no-OS environments.

Functionality
~~~~~~~~~~~~~
* SPI communication with TMC5240 IC
* Register read/write operations
* Configuration of motor parameters (current, velocity, acceleration)
* Motion control (position, velocity modes)

Driver Configuration
--------------------

Multi-Instance Support
~~~~~~~~~~~~~~~~~~~~~~
The driver supports multiple TMC5240 devices via an internal device lookup table. By default,
up to 4 devices are supported. To increase this limit, define ``TMC5240_MAX_DEVICES`` before
including the driver header:

.. code-block:: c

	#define TMC5240_MAX_DEVICES 8
	#include "tmc5240.h"

Or set it in the build system:

.. code-block:: make

	CFLAGS += -DTMC5240_MAX_DEVICES=8

Initialization
~~~~~~~~~~~~~~
The TMC5240 driver is initialized using the ``tmc5240_init()`` function, which requires a properly configured
``tmc5240_init_param`` structure. The initialization parameters include:

**SPI Configuration:**

* ``spi_init_param``: SPI interface settings (device ID, speed, chip select, mode)

**Motion Profile Parameters:**

The TMC5240 supports a sophisticated multi-phase motion profile with separate acceleration and deceleration stages:

* ``vstart``: Start velocity threshold in internal units
* ``a1``: First acceleration phase value in internal units  
* ``v1``: First velocity threshold in internal units (transition from a1 to a2)
* ``a2``: Second acceleration phase value in internal units
* ``v2``: Second velocity threshold in internal units (transition from a2 to amax)
* ``amax``: Maximum acceleration in internal units
* ``vmax``: Maximum velocity in internal units
* ``dmax``: Maximum deceleration in internal units
* ``d2``: Second deceleration phase value in internal units
* ``d1``: First deceleration phase value in internal units (transition from d2 to d1)
* ``vstop``: Stop velocity threshold in internal units
* ``clock``: Internal clock frequency in Hz (typically 12.5 MHz)

The multi-phase profile allows for smooth motion control with different acceleration/deceleration 
rates at different velocity ranges, providing better control over motor dynamics and mechanical stress.

**Motor Mechanical Parameters:**

* ``step_angle_millidegrees``: Motor step angle in millidegrees (e.g., 900 for 0.9°)
* ``microsteps_res``: Microstepping resolution (0 = 256 microsteps, 1 = 128, 2 = 64, etc.)

**Current Control:**

* ``current_run``: Run current (0-31, scaled by 1/32)
* ``current_hold``: Hold current (0-31, scaled by 1/32)
* ``irundelay``: Power-down delay for run current (multiplied by 512 clocks)
* ``iholddelay``: Power-down delay for hold current (multiplied by 2^18 clocks)
* ``global_scaler``: Global current scaling factor (0-255)

**Chopper Configuration:**

* ``toff``: Off-time setting (determines chopper frequency)
* ``tbl``: Blank time setting (noise suppression)
* ``slope_control``: Slope control for voltage ramping
* ``current_range``: Current range selection

Register Access
~~~~~~~~~~~~~~~
The driver provides low-level register access through wrapper functions to the TMC-API:

**tmc5240_reg_read()**

Reads a 32-bit value from a specified TMC5240 register.

* **Parameters:**

  * ``device``: Pointer to TMC5240 device structure
  * ``reg_addr``: 8-bit register address
  * ``reg_data``: Pointer to store the 32-bit register value

* **Returns:** 0 on success, negative error code otherwise

**tmc5240_reg_write()**

Writes a 32-bit value to a specified TMC5240 register.

* **Parameters:**

  * ``device``: Pointer to TMC5240 device structure
  * ``reg_addr``: 8-bit register address
  * ``reg_data``: 32-bit value to write

* **Returns:** 0 on success, negative error code otherwise

**tmc5240_reg_update()**

Performs a read-modify-write operation on a specific register field.

* **Parameters:**

  * ``device``: Pointer to TMC5240 device structure
  * ``reg_addr``: 8-bit register address
  * ``mask``: Bitmask for the field to update
  * ``val``: New value for the field (already shifted to the correct position)

* **Returns:** 0 on success, negative error code otherwise


These functions wrap the TMC-API register access functions and handle the SPI communication
transparently. Direct register access is useful for advanced configurations and debugging.

Motion Control
~~~~~~~~~~~~~~
The driver provides comprehensive motion control APIs for position and velocity modes:

**Position Control**

* ``tmc5240_set_target_pos()``: Set target position in microsteps. Automatically configures ramp mode to POSITION and ensures AMAX/VMAX are set. The motor will move to the target using the configured motion profile.

* ``tmc5240_get_target_pos()``: Read the current target position from XTARGET register.

* ``tmc5240_set_current_pos()``: Override the current position counter. Switches to HOLD mode first to safely update XACTUAL register. Useful for homing or synchronization.

* ``tmc5240_get_current_pos()``: Read the actual motor position from XACTUAL register.

**Velocity Control**

* ``tmc5240_set_target_vel()``: Set target velocity in internal units. Positive values use POSITIVE_VELOCITY mode, negative values use NEGATIVE_VELOCITY mode. The absolute value is written to VMAX.

* ``tmc5240_get_current_vel()``: Read the actual velocity from VACTUAL register. Returns sign-extended 24-bit value as 32-bit signed integer.

**Acceleration Control**

* ``tmc5240_set_amax()``: Set maximum acceleration value.
* ``tmc5240_get_amax()``: Read current maximum acceleration.
* ``tmc5240_get_current_accel()``: Read actual acceleration from AACTUAL register.

**Motion Profile Parameters**

The TMC5240 uses a multi-phase motion profile for smooth acceleration and deceleration:

*Basic Motion Profile:*

* ``tmc5240_set_vmax()`` / ``tmc5240_get_vmax()``: Set/get maximum velocity.
* ``tmc5240_set_amax()`` / ``tmc5240_get_amax()``: Set/get maximum acceleration.
* ``tmc5240_set_dmax()`` / ``tmc5240_get_dmax()``: Set/get maximum deceleration.

*Advanced Multi-Phase Profile Parameters:*

* ``tmc5240_set_vstart()`` / ``tmc5240_get_vstart()``: Set/get start velocity threshold.
* ``tmc5240_set_a1()`` / ``tmc5240_get_a1()``: Set/get first acceleration phase (VSTART to V1).
* ``tmc5240_set_v1()`` / ``tmc5240_get_v1()``: Set/get first velocity threshold (transition from A1 to A2).
* ``tmc5240_set_a2()`` / ``tmc5240_get_a2()``: Set/get second acceleration phase (V1 to V2).
* ``tmc5240_set_v2()`` / ``tmc5240_get_v2()``: Set/get second velocity threshold (transition from A2 to AMAX).
* ``tmc5240_set_d1()`` / ``tmc5240_get_d1()``: Set/get first deceleration phase (V1 to VSTOP).
* ``tmc5240_set_d2()`` / ``tmc5240_get_d2()``: Set/get second deceleration phase (V2 to V1).
* ``tmc5240_set_vstop()`` / ``tmc5240_get_vstop()``: Set/get stop velocity threshold.

**Ramp Mode Control**

* ``tmc5240_set_rampmode()`` / ``tmc5240_get_rampmode()``: Configure operating mode:

  * ``TMC5240_RAMPMODE_POSITION``: Position mode (servo)
  * ``TMC5240_RAMPMODE_POSITIVE_VELOCITY``: Velocity mode, positive direction
  * ``TMC5240_RAMPMODE_NEGATIVE_VELOCITY``: Velocity mode, negative direction
  * ``TMC5240_RAMPMODE_HOLD``: Hold mode (motor stopped)

**Direction and Stop Control**

* ``tmc5240_set_shaft_dir()`` / ``tmc5240_get_shaft_dir()``: Set/get shaft direction (0 = normal, 1 = inverted). Allows reversing motor direction without rewiring.

* ``tmc5240_stop()``: Stop the motor by setting VMAX to 0. Motor will decelerate using configured profile.

Understanding Multi-Phase Motion Profiles
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The TMC5240 supports a sophisticated multi-phase motion profile that provides fine-grained control 
over acceleration and deceleration phases. This allows for smoother motion and reduced mechanical 
stress on the motor and driven system.

**Acceleration Profile:**

The motor accelerates through multiple phases:

1. **vstart → v1**: Accelerates using ``a1`` (first acceleration rate)
2. **v1 → v2**: Accelerates using ``a2`` (second acceleration rate)
3. **v2 → vmax**: Accelerates using ``amax`` (maximum acceleration rate)

**Deceleration Profile:**

Similarly, the motor decelerates through mirrored phases:

1. **vmax → v2**: Decelerates using ``dmax`` (maximum deceleration rate)
2. **v2 → v1**: Decelerates using ``d2`` (second deceleration rate)
3. **v1 → vstop**: Decelerates using ``d1`` (first deceleration rate)

**Benefits:**

* Reduced mechanical shock and vibration
* Better control over resonance frequencies
* Smoother transitions at low speeds
* Optimized performance across the full speed range

For simple applications, you can set a1=a2=amax and d1=d2=dmax to use a single-phase profile.

Driver Initialization Example
-----------------------------
The following example demonstrates how to initialize the TMC5240 driver and perform basic motion control operations:

.. code-block:: c

	struct tmc5240_dev *tmc5240_dev;
	int ret;

	/* SPI initialization parameters */
	const struct no_os_spi_init_param tmc_spi_ip = {
		.device_id = 1,
		.max_speed_hz = 1000000,	/* 1 MHz SPI clock */
		.chip_select = 0,
		.mode = NO_OS_SPI_MODE_3,	/* CPOL=1, CPHA=1 */
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &stm32_spi_ops,	/* Platform-specific ops */
		.extra = &spi_extra,	/* Platform-specific config */
	};

	/* TMC5240 initialization parameters */
	struct tmc5240_init_param tmc5240_ip = {
		.spi_init_param = tmc_spi_ip,

		/* Motion profile parameters - multi-phase configuration */
		.vstart = 1,			/* Start velocity threshold */
		.a1 = 65536,			/* First acceleration phase */
		.v1 = 100000,			/* First velocity threshold */
		.a2 = 98304,			/* Second acceleration phase */
		.v2 = 200000,			/* Second velocity threshold */
		.amax = 131072,			/* Maximum acceleration */
		.vmax = 254876,			/* Maximum velocity */
		.dmax = 131072,			/* Maximum deceleration */
		.d2 = 98304,			/* Second deceleration phase */
		.d1 = 65536,			/* First deceleration phase */
		.vstop = 10,			/* Stop velocity threshold */

		/* Hardware configuration */
		.clock = 12500000,		/* 12.5 MHz internal clock */
		.step_angle_millidegrees = 900, /* 0.9 degrees per full step */
		.microsteps_res = 0,		/* 0 = 256 microsteps (native) */

		/* Current control */
		.current_run = 16,		/* Run current = 16/32 = 50% */
		.current_hold = 0,		/* Standstill current = 0/32 = 0% */
		.irundelay = 4,			/* 4 * 512 clocks delay */
		.iholddelay = 4,		/* 4 * 2^18 clocks delay */
		.global_scaler = 0xFB,		/* Global current scaler */

		/* Chopper configuration */
		.toff = 3,			/* 24 + 3*32 = 120 clocks off-time */
		.tbl = 2,			/* 36 clocks blank time */

		/* Driver configuration */
		.slope_control = 0,		/* 100V/us slew rate */
		.current_range = 3,		/* 3A current range */
	};

	/* Initialize the TMC5240 driver */
	ret = tmc5240_init(&tmc5240_dev, &tmc5240_ip);
	if (ret) {
		/* Handle initialization error */
		return ret;
	}

TMC5240 no-OS IIO Support
-------------------------
The iio_tmc5240 driver provides a TinyIIO interface for the TMC5240 stepper motor
controller, enabling remote configuration and monitoring through the IIO protocol
in no-OS embedded systems.

Functionality
~~~~~~~~~~~~~
* Exposes TMC5240 registers and parameters as IIO attributes
* Real-time monitoring and control of motor via IIO channels
* Remote configuration through TinyIIO protocol
* Integration with no-OS framework

Exposed IIO ABI
~~~~~~~~~~~~~~~
The iio_tmc5240 driver exposes the following IIO channels and attributes:

**IIO Channels**

The driver exposes three output channels for motor control:

1. **anglvel**: angle_velocity (output)

   * ``raw``: Read/write target velocity in internal units

     - **Read:** Returns current velocity from VACTUAL register (sign-extended 24-bit value)
     - **Write:** Sets target velocity. Positive values rotate in positive direction, negative in negative direction

   * ``scale``: Read-only velocity scale factor. Converts internal velocity units to full-steps per second

2. **accel**: angular_acceleration (output)

   * ``raw``: Read acceleration

     - **Read:** Returns current acceleration from AACTUAL register
     - **Write:** Sets maximum acceleration in AMAX register

   * ``scale``: Read-only acceleration scale factor. Converts internal acceleration units to full-steps per second²

3. **angl**: angular_position (output)

   * ``raw``: Read/write target position

     - **Read:** Returns current position from XACTUAL register in microsteps
     - **Write:** Sets target position in microsteps. Motor moves to this position using configured motion profile

   * ``scale``: Read-only position scale factor. Converts microsteps to full-steps.

   * ``preset``: Read/write position preset

     - **Read:** Returns current position from XACTUAL register
     - **Write:** Overrides current position counter (motor enters HOLD mode first). Useful for homing/zeroing operations

**Shared Channel Attributes**

* ``calibscale``: Read/write motor step angle calibration (shared by all channels). Can be used for converting motor full-step to degrees gear scaling.

**Device Attributes**

* ``direction``: Read/write shaft direction

  - **Format:** Integer (0 or 1)
  - **Values:**

    * ``0`` = Normal direction
    * ``1`` = Inverted direction

  - Reverses motor rotation direction without changing wiring

**Debug Attributes**

These attributes are accessible through the IIO debug interface:

* ``stop``: Write-only motor stop command

  - **Write:** Must write ``1`` to trigger stop
  - **Effect:** Sets VMAX to 0, motor decelerates according to configured deceleration profile

* ``amax``: Read/write maximum acceleration

  - **Format:** Integer (internal units)
  - Direct access to AMAX register

* ``vmax``: Read/write maximum velocity

  - **Format:** Integer (internal units)
  - Direct access to VMAX register

* ``dmax``: Read/write maximum deceleration

  - **Format:** Integer (internal units)
  - Direct access to DMAX register

*Advanced Multi-Phase Profile Parameters:*

* ``vstart``: Read/write start velocity threshold

  - **Format:** Integer (internal units)
  - Direct access to VSTART register

* ``a1``: Read/write first acceleration phase

  - **Format:** Integer (internal units)
  - Direct access to A1 register

* ``v1``: Read/write first velocity threshold

  - **Format:** Integer (internal units)
  - Direct access to V1 register

* ``a2``: Read/write second acceleration phase

  - **Format:** Integer (internal units)
  - Direct access to A2 register

* ``v2``: Read/write second velocity threshold

  - **Format:** Integer (internal units)
  - Direct access to V2 register

* ``d1``: Read/write first deceleration phase

  - **Format:** Integer (internal units)
  - Direct access to D1 register

* ``d2``: Read/write second deceleration phase

  - **Format:** Integer (internal units)
  - Direct access to D2 register

* ``vstop``: Read/write stop velocity threshold

  - **Format:** Integer (internal units)
  - Direct access to VSTOP register

**Register Access**

The driver provides direct register read/write access through the IIO debug interface:

* ``direct_reg_access``: Read/write any TMC5240 register by address.

Dependencies
~~~~~~~~~~~~
* no-OS framework
* TinyIIO library
* SPI bus support
* TMC-API

TMC5240 IIO Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

	struct tmc5240_iio_dev *tmc5240_iio_dev;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	int ret;

	/* SPI initialization parameters */
	struct tmc5240_init_param tmc5240_ip_iio = {
		.spi_init_param = tmc_spi_ip,
		.vstart = 1,
		.a1 = 65536,
		.v1 = 100000,
		.a2 = 98304,
		.v2 = 200000,
		.amax = 131072,
		.vmax = 254876,
		.dmax = 131072,
		.d2 = 98304,
		.d1 = 65536,
		.vstop = 10,
		.clock = 12500000,
		.step_angle_millidegrees = 900,
		.microsteps_res = 0,
		.current_run = 16,
		.current_hold = 0,
		.irundelay = 4,
		.iholddelay = 4,
		.global_scaler = 0xFB,
		.toff = 3,
		.tbl = 2,
		.slope_control = 0,
		.current_range = 3,
	};

	/* IIO TMC5240 initialization parameters */
	struct tmc5240_iio_dev_init_param tmc5240_iio_ip = {
		.tmc5240_init_param = &tmc5240_ip_iio,
	};

	/* Initialize the IIO TMC5240 driver */
	ret = tmc5240_iio_init(&tmc5240_iio_dev, &tmc5240_iio_ip);
	if (ret) {
		pr_err("tmc5240_iio_init failed: %d\n", ret);
		return ret;
	}

	/* IIO device can now be added to IIO application device configuration */
