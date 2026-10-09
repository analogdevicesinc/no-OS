ADP20086 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADP20086`
- :adi:`ADP20088`

Overview
--------

The ADP20086/ADP20088 are quad-channel, camera power protector ICs that deliver
up to 800 mA of constant load current to each of their four output channels.
Each output is individually protected against short-to-battery, short-to-ground,
and overcurrent conditions. The devices operate from a 3 V to 17 V camera supply
with input undervoltage/overvoltage (UV/OV) protection, and the input-to-output
voltage drop is only 280 mV (typ) at 800 mA.

An on-chip 8-bit ADC provides voltage and current readings for each active
channel as well as the input (VIN) and internal supply (VDD) voltages, all
accessible over the I2C interface. The I2C interface is also used to configure
per-channel current limits (adjustable from 52 mA to 832 mA in ±5% steps),
protection thresholds, soft-start/soft-shutdown current ramps, and to read the
diagnostic status of the device. Eight selectable I2C addresses are available,
set by an external resistor on the ADDR pin, and an optional Packet Error
Checking (PEC) mode protects the I2C traffic with a CRC-8 checksum.

The devices integrate overtemperature shutdown, differential output overvoltage
protection, open-load detection and protection, output discharge resistors, and
autoretry-on-fault recovery. All parts operate over the -40 °C to +125 °C ambient
temperature range and are ASIL B compliant, making them suitable for automotive
functional-safety designs. The ADP20086/ADP20088 also provide a MAX20087-compatible
legacy ADC mux read path for drop-in compatibility with existing designs.

Applications
------------

* Power-Over-Coax (PoC) protection for camera systems
* Automotive ADAS and surround-view camera power distribution
* Fault diagnostics and protection
* ASIL B safety applications

ADP20086 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the communication protocol (I2C).

The first API to be called is **adp20086_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. During initialization the
driver populates the CRC-8 table, brings up the I2C bus, reads the **ID**
register to auto-detect the PEC-enable (OTP factory-programmed) state, part ID
and silicon revision, and runs a self-test check. The device is released with
**adp20086_remove**.

Register Access
~~~~~~~~~~~~~~~

Raw register access is provided by **adp20086_read** and **adp20086_write**,
which transparently frame each transfer with a PEC byte when PEC mode is enabled.
Read-modify-write of individual fields is available through
**adp20086_update_register**, and a masked field read through
**adp20086_get_register_field**.

Device Identity
~~~~~~~~~~~~~~~

The device identity captured at init can be queried with **adp20086_get_pece**
(PEC-enable state), **adp20086_get_part_id** (ID[5:4]) and
**adp20086_get_revision** (Rev[3:0]).

CONFIG Register
~~~~~~~~~~~~~~~

The CONFIG register controls the global behavior of the device. The legacy ADC
mux selection is set and read with **adp20086_configure_mux** and
**adp20086_get_mux_config**. The ADC enable/conversion control is handled by
**adp20086_configure_enc** / **adp20086_get_enc_config**, and the
clear-faults-on-read behavior by **adp20086_configure_clr** /
**adp20086_get_clr_config**.

Channel and Protection Enable
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each of the four output channels is individually enabled or disabled with
**adp20086_enable_channel** and **adp20086_disable_channel** (pass
``ADP20086_ALL_CHANNELS`` to act on every channel at once). Per-channel load
detection is toggled with **adp20086_enable_load_detection** /
**adp20086_disable_load_detection**, and open-load protection with
**adp20086_enable_open_protection** / **adp20086_disable_open_protection**.

Current Limit Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The per-channel current limit is selected from the discrete
``enum adp20086_ilim_available`` steps (52 mA to 832 mA) using
**adp20086_set_ilim**. The programmed limit can be read back as a raw code with
**adp20086_get_ilim_code** or as a value in microamps with
**adp20086_get_ilim_ua**.

Protection Thresholds
~~~~~~~~~~~~~~~~~~~~~

The device exposes a set of programmable protection thresholds. Each threshold
has a setter that accepts an engineering value and getters that return either the
raw code or the value in engineering units:

* Load-detection threshold (per channel) - **adp20086_set_ldet_threshold**,
  **adp20086_get_ldet_threshold_code**, **adp20086_get_ldet_threshold_ua**
* Input overvoltage threshold - **adp20086_set_ovin**,
  **adp20086_get_ovin_code**, **adp20086_get_ovin_uv**
* Input undervoltage threshold - **adp20086_set_uvin**,
  **adp20086_get_uvin_code**, **adp20086_get_uvin_uv**
* Open-load current threshold (per channel) - **adp20086_set_iopen**,
  **adp20086_get_iopen_code**, **adp20086_get_iopen_ua**
* Output undervoltage threshold (per channel) - **adp20086_set_uvout**,
  **adp20086_get_uvout_code**, **adp20086_get_uvout_uv**

Soft-Start / Soft-Shutdown Ramps
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The current slew rate at turn-on and turn-off is configured per channel. The
soft-start ramp (``enum adp20086_soft_start``) is set and read with
**adp20086_set_soft_start** / **adp20086_get_soft_start**, and the
soft-shutdown ramp (``enum adp20086_soft_shutdown``) with
**adp20086_set_soft_shutdown** / **adp20086_get_soft_shutdown**.

Interrupt Masking and Feature Bits
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Individual interrupt sources (``enum adp20086_interrupt_source``) are masked
from the INT pin using **adp20086_set_interrupt_mask** and read back with
**adp20086_get_interrupt_mask**. **adp20086_reset_masks** reverts
all three mask registers to their datasheet defaults in one call.

Additional feature bits are controlled by
**adp20086_set_ovtst** (output overvoltage self-test),
**adp20086_set_iir_filter** (ADC IIR filter) and
**adp20086_set_discharge_resistors** (integrated output discharge resistors),
each with a matching **adp20086_get_*** accessor.

INTB Fault Interrupt
~~~~~~~~~~~~~~~~~~~~

INT (INTB) is an open-drain, active-low output that asserts whenever any
unmasked fault is present. It requires an external pull-up to the system I/O
supply; the datasheet calls for at least 2 kΩ so the device can pull the line
down to its specified low level.

The pin is optional. Supplying only ``intb_gpio_ip`` makes it readable as a
plain input with **adp20086_get_intb** (which returns ``true`` when a fault is
being signalled, since the line is active low). Supplying ``irq_ctrl`` as well
— an interrupt controller the caller has already initialized, and which the
driver borrows rather than owns — additionally registers a falling-edge
interrupt on the pin.

Interrupt handling is split in two. The interrupt handler itself performs no
bus traffic: reading the fault cause requires I2C, which must not run in
interrupt context, so the handler only masks the pin and records that a fault
is pending. The work is done by **adp20086_process_interrupt**, which the
application must call from its main loop:

.. code-block:: c

        static void fault_cb(struct adp20086_dev *dev,
                             const struct adp20086_device_status *dev_status,
                             const struct adp20086_channel_status *ch_status,
                             void *ctx)
        {
                if (adp20086_channel_flagged(ch_status->overcurrent,
                                             ADP20086_CHANNEL1))
                        pr_info("CH1 overcurrent\n");
        }

        adp20086_set_int_callback(dev, fault_cb, NULL);
        adp20086_int_enable(dev);

        while (1) {
                adp20086_process_interrupt(dev);    /* no-op unless pending */
                ...
        }

**adp20086_process_interrupt** decodes both status structures, passes them to
the registered callback, and re-arms the interrupt. It returns immediately when
nothing is pending, so it is cheap to call unconditionally.

Two details are worth knowing. The interrupt is left **disabled** by
**adp20086_init**, so that a fault cannot be delivered before the application
has registered a callback; arm it with **adp20086_int_enable** once the
callback is in place. And because reading the status registers is what clears
the latched faults (``CONFIG.CLR`` is set out of reset),
**adp20086_process_interrupt** must not be paired with a separate
**adp20086_clear_device_status** call — the status read already does it.

Status and Diagnostics
~~~~~~~~~~~~~~~~~~~~~~

Per-channel fault status (thermal shutdown, overcurrent, overvoltage,
undervoltage, load-detected, open-load) for all four channels is retrieved into a
``struct adp20086_channel_status`` with **adp20086_get_channel_status**. Each
fault field is a 4-bit mask with the LSB = channel 1; test one of them with
**adp20086_channel_flagged**, which takes a mask field and a channel and spares
the caller the bit layout. Passing ``ADP20086_ALL_CHANNELS`` asks whether any
channel is flagged. The call reads STAT2 (CH1/CH2), STAT3 (CH3/CH4) and STAT4
(all four) in one pass; because the STAT registers are clear-on-read while
``CONFIG.CLR`` is set and each byte packs several channels, reading them together
is the only way to capture every channel's latched faults without one read
clearing another's.

Device-wide status is read into a ``struct adp20086_device_status`` with
**adp20086_get_device_status**, and the device-level latched faults can be
flushed with **adp20086_clear_device_status**. The self-test / integrity fault
check performed at init can be re-run at any time with
**adp20086_check_self_test**.

ADC Telemetry
~~~~~~~~~~~~~

The on-chip 8-bit ADC readings are available both as raw codes and as scaled
engineering values:

* Input voltage - **adp20086_read_vin_code**, **adp20086_read_vin_uv**
* Output voltage (per channel) - **adp20086_read_vout_code**,
  **adp20086_read_vout_uv**
* Output current (per channel) - **adp20086_read_iout_code**,
  **adp20086_read_iout_ua**
* Internal supply voltage - **adp20086_read_vdd_code**,
  **adp20086_read_vdd_uv**

Legacy ADC Read
~~~~~~~~~~~~~~~

For drop-in compatibility with the MAX20087, the legacy ADC mux read path is
provided by **adp20086_read_legacy_adc**, which returns the raw ADC byte for the
selected legacy channel (``enum adp20086_legacy_adc``) according to the mux
configuration set through **adp20086_configure_mux**.

ADP20086 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	struct adp20086_dev *dev;

	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
		.extra = UART_EXTRA,
	};

	struct no_os_i2c_init_param adp20086_i2c_ip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = 400000,
		.platform_ops = I2C_OPS,
		.slave_address = ADP20086_BASE_ADDR,
		.extra = I2C_EXTRA,
	};

	/* Optional: INTB, open drain and active low, with an external pull-up. */
	struct no_os_gpio_init_param adp20086_intb_ip = {
		.port = INTB_PORT,
		.number = INTB_PIN,
		.pull = NO_OS_PULL_UP,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	struct no_os_irq_init_param adp20086_gpio_irq_ip = {
		.irq_ctrl_id = GPIO_IRQ_ID,
		.platform_ops = GPIO_IRQ_OPS,
		.extra = GPIO_IRQ_EXTRA,
	};

	struct adp20086_init_param adp20086_ip = {
		.i2c_ip = &adp20086_i2c_ip,
		.intb_gpio_ip = &adp20086_intb_ip,
	};

	/*
	 * The driver borrows the interrupt controller, so it has to be
	 * initialized first and removed by the caller afterwards.
	 */
	ret = no_os_irq_ctrl_init(&gpio_irq_desc, &adp20086_gpio_irq_ip);
	if (ret)
		goto error;

	adp20086_ip.irq_ctrl = gpio_irq_desc;

	ret = adp20086_init(&dev, &adp20086_ip);
	if (ret)
		goto error;

ADP20086 no-OS IIO support
--------------------------

The ADP20086 IIO driver comes on top of the ADP20086 driver and offers support
for interfacing IIO clients through libiio.

ADP20086 IIO Device Configuration
---------------------------------

Input Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``vin``, ``vout1``..``vout4`` and ``vdd`` voltage channels and the
``iout1``..``iout4`` current channels expose the on-chip ADC telemetry. Every
measurement channel provides:

* ``raw - the raw 8-bit ADC code of the channel``
* ``scale - the per-code scale; raw x scale yields mV for voltage channels and mA for current channels (VIN/VOUT = 72, VDD = 9, IOUT = 3.5)``

Output Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The four ``iout`` current channels map 1:1 to the device outputs and therefore
also carry the per-output configuration attributes:

* ``enable - enable/disable the output channel``
* ``enable_available - list of available enable states ("0 1")``
* ``current_limit - per-channel current limit in mA``
* ``iopen - open-load current threshold in mA``
* ``load_detect - load-detection current threshold in mA``
* ``uvout - output undervoltage threshold in mV``
* ``soft_start - turn-on current ramp in ms``
* ``soft_start_available - list of available soft-start ramps ("0.5 1 2 4")``
* ``soft_shutdown - turn-off current ramp in ms``
* ``soft_shutdown_available - list of available soft-shutdown ramps ("0.25 0.5 1 2")``
* ``open_prot - enable/disable open-load protection``
* ``open_prot_available - list of available open-protection states ("0 1")``
* ``load_detect_en - enable/disable load detection``
* ``load_detect_en_available - list of available load-detect states ("0 1")``

Global Attributes
~~~~~~~~~~~~~~~~~

The device has the following global attributes:

* ``vin_uv_threshold - input undervoltage threshold in mV``
* ``vin_ov_threshold - input overvoltage threshold in mV``
* ``pece - PEC-enable state (read-only)``
* ``part_id - device part ID (read-only)``
* ``revision - silicon revision (read-only)``
* ``int_mask - interrupt mask register (MASK)``
* ``int_mask2 - interrupt mask register (MASK2)``
* ``int_mask3 - interrupt mask register (MASK3)``

Two further attributes depend on how the part is wired, and are present only
when the corresponding pin is supplied at init:

* ``gpio_en - drive the EN pin (present only when en_gpio_ip is supplied)``
* ``gpio_en_available - list of available EN states ("0 1")``
* ``intb - INTB pin state, 1 while a fault is being signalled (read-only;
  present only when intb_gpio_ip is supplied)``

Debug Attributes
~~~~~~~~~~~~~~~~

* ``status - decoded, human-readable device and per-channel fault status``
* ``bist - latched built-in self-test result``

Arbitrary registers can be read and written through the standard IIO
``direct_reg_access`` debug interface, which is wired to ``adp20086_read`` /
``adp20086_write``.

ADP20086 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	struct adp20086_iio_desc *adp20086_iio_desc;
	struct adp20086_iio_desc_init_param adp20086_iio_ip = {
		.adp20086_init_param = &adp20086_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	ret = adp20086_iio_init(&adp20086_iio_desc, &adp20086_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp20086",
			.dev = adp20086_iio_desc,
			.dev_descriptor = adp20086_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adp20086_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp20086;

	ret = iio_app_run(app);

	iio_app_remove(app);

remove_iio_adp20086:
	adp20086_iio_remove(adp20086_iio_desc);
