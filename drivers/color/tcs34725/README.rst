TCS34725 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- `TCS34725 <https://www.mouser.com/datasheet/2/588/TCS3472-1626737.pdf>`_

Overview
--------

The TCS34725 is a color light-to-digital converter that provides a digital
return of red, green, blue (RGB), and clear light sensing values. An IR
blocking filter, integrated on-chip and localized to the color sensing
photodiodes, minimizes the IR spectral component of the incoming light and
allows color measurements to be made accurately.

Each of the four channels has a dedicated 16-bit ADC. The high sensitivity,
3,800,000:1 dynamic range, and IR blocking filter make the device suitable for
use under varying lighting conditions and behind attenuating materials such as
dark glass. Both the analog gain (1x, 4x, 16x, 60x) and the integration time
(2.4 ms to 700 ms in 2.4-ms increments) are programmable, which allows the
full-scale count to be traded against conversion time and noise.

The device communicates over an I2C interface compatible with Fast Mode, at
data rates up to 400 kbit/s, and responds at the 7-bit slave address 0x29. A
maskable interrupt with programmable upper and lower thresholds and a
persistence filter is available on the dedicated INT pin. On-chip power
management provides a 2.5 uA sleep state and a 65 uA wait state with a
programmable wait time, so the device can idle between measurements to reduce
average power consumption.

Applications
------------

* RGB LED backlight control
* Light color temperature measurement
* Ambient light sensing for display backlight control
* Fluid and gas analysis
* Product color verification and sorting

Operation Modes
---------------

+-----------+----------------------------------+---------------------+-----------------------------+
| Mode Name | Description                      | Configuration Bits  | Typical Use Case            |
+===========+==================================+=====================+=============================+
| Sleep     | Low-power state entered after    | PON = 0 in the      | Idling the device between   |
|           | power-on reset. The internal     | ENABLE register     | infrequent measurements to  |
|           | oscillator is stopped and no     | (0x00)              | minimize supply current.    |
|           | conversions take place.          |                     |                             |
+-----------+----------------------------------+---------------------+-----------------------------+
| Idle      | The internal oscillator runs and | PON = 1, AEN = 0    | Configuring gain,           |
|           | the timers are clocked, but the  | in the ENABLE       | integration time and        |
|           | RGBC ADCs are not enabled.       | register (0x00)     | thresholds before starting. |
+-----------+----------------------------------+---------------------+-----------------------------+
| RGBC      | The four-channel ADC converts    | PON = 1, AEN = 1    | Normal color and ambient    |
|           | continuously, repeating the      | in the ENABLE       | light measurement.          |
|           | Wait-RGBC cycle automatically.   | register (0x00)     |                             |
+-----------+----------------------------------+---------------------+-----------------------------+
| Wait      | Low-power state inserted between | WEN = 1 in the      | Reducing average power in   |
|           | RGBC integration cycles, with a  | ENABLE register,    | continuously sampling       |
|           | programmable duration.           | duration set by     | applications.               |
|           |                                  | WTIME (0x03)        |                             |
+-----------+----------------------------------+---------------------+-----------------------------+

TCS34725 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C), by populating a **no_os_i2c_init_param**
structure and referencing it from the **comm_param** field of
**tcs34725_init_param**. The device responds at the 7-bit slave address 0x29.

The first API to be called is **tcs34725_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. This function allocates
the device descriptor and initializes the I2C bus, but it does not write any
device register, so the device is left in the low-power Sleep state it enters
after its internal power-on reset.

A minimum of 2.4 ms must pass after power is applied before the first I2C
access is attempted.

The resources allocated by **tcs34725_init** are released by
**tcs34725_remove**.

Register Access
~~~~~~~~~~~~~~~

All register accesses are performed through **tcs34725_reg_read** and
**tcs34725_reg_write**, which read and write a single 8-bit register. Both APIs
set the command bit (0x80) in the transmitted register address for you, as
required by the device's I2C protocol, so callers pass the plain register
address such as **TCS34725_ENABLE** or **TCS34725_CONTROL**.

The register addresses and the ENABLE register bit masks are provided as macros
in ``tcs34725.h``.

Device Identification
~~~~~~~~~~~~~~~~~~~~~

The read-only ID register (**TCS34725_ID**) holds the part number and can be
read with **tcs34725_reg_read** to verify that the expected device is present
on the bus. It reads 0x44 for the TCS34725 and the TCS34721, and 0x4D for the
TCS34723 and the TCS34727.

Power Management and RGBC Enable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Because **tcs34725_init** leaves the device asleep, the RGBC engine has to be
started explicitly before any color data is available. Write
**TCS34725_ENABLE_PON** to the **TCS34725_ENABLE** register to start the
internal oscillator, wait at least 2.4 ms for the warm-up required before an
RGBC cycle may be initiated, then write
**TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN** to enable the ADC channels.

Once both PON and AEN remain asserted, the device repeats the Wait-RGBC cycle
automatically without further intervention.

Gain and Integration Time Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The analog gain is selected by the AGAIN field (bits 1:0) of the
**TCS34725_CONTROL** register: 0 selects 1x, 1 selects 4x, 2 selects 16x and 3
selects 60x gain.

The integration time is set by the **TCS34725_ATIME** register in 2.4-ms
increments, where the maximum count is ``(256 - ATIME) x 1024``, saturating at
65535. For example, 0xFF gives one 2.4-ms cycle with a maximum count of 1024,
0xF6 gives 24 ms with a maximum count of 10240, and 0xC0 gives 154 ms with a
full-scale count of 65535.

Both registers are written with **tcs34725_reg_write** while the device is in
the Idle state.

Reading Color Data
~~~~~~~~~~~~~~~~~~

The **tcs34725_get_rgbc** API reads the eight RGBC channel data registers
(0x14 to 0x1B) in a single I2C transaction and returns the clear, red, green
and blue channels as 16-bit values, handling the little-endian byte order of
the device internally.

Before relying on a sample, the AVALID bit (bit 0) of the **TCS34725_STATUS**
register may be polled with **tcs34725_reg_read** to confirm that the RGBC
channels have completed an integration cycle.

TCS34725 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	int ret;
	uint8_t color_id;
	uint16_t r, g, b, c;
	struct tcs34725_desc *dev;

	const struct no_os_i2c_init_param tcs34725_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 100000,
		.slave_address = 0x29,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	};

	struct tcs34725_init_param tcs34725_ip = {
		.comm_param = &tcs34725_i2c_ip,
	};

	ret = tcs34725_init(&dev, &tcs34725_ip);
	if (ret)
		goto error;

	/* Wait for the power-on reset to complete before the first access. */
	no_os_mdelay(10);

	ret = tcs34725_reg_read(dev, TCS34725_ID, &color_id);
	if (ret)
		goto error_remove;

	if (color_id != 0x44) {
		ret = -ENODEV;
		goto error_remove;
	}

	/* Start the internal oscillator. */
	ret = tcs34725_reg_write(dev, TCS34725_ENABLE, TCS34725_ENABLE_PON);
	if (ret)
		goto error_remove;

	/* PON requires a 2.4 ms warm-up before AEN can be asserted. */
	no_os_mdelay(3);

	/* Enable the RGBC engine. */
	ret = tcs34725_reg_write(dev, TCS34725_ENABLE,
				 TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
	if (ret)
		goto error_remove;

	ret = tcs34725_get_rgbc(dev, &r, &g, &b, &c);
	if (ret)
		goto error_remove;
