ADP20086 no-OS driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADP20086`
- :adi:`ADP20088`

Overview
--------

The :adi:`ADP20086` and :adi:`ADP20088` are quad-channel camera power
protector ICs that deliver up to 800 mA of constant load current on each of
their four output channels. Each output is a high-side isolation and
protection switch with internal current limiting, and is individually
protected against short-to-battery, short-to-ground, and overcurrent
conditions. The input accepts 3 V to 17 V from the camera supply, while the
outputs tolerate up to 17.6 V of short-to-battery isolation (VOUT - VIN). The
input-to-output voltage drop is only 280 mV (typical) at 800 mA, and shutdown
current is 6 µA.

Each channel has an independently adjustable current limit from 52 mA to
832 mA in 52 mA steps, accurate to ±5%. Selectable soft-start current ramps
(0.5 ms to 4 ms) limit inrush current into capacitive loads, and selectable
soft-shutdown ramps (0.25 ms to 2 ms) minimize inductive ringing when a
channel turns off. Channels can be paralleled for higher current. The devices
also provide a differential output overvoltage comparator for short-to-battery
detection, programmable input UV/OV monitoring, load and open-load current
detection, integrated output discharge resistors, overtemperature shutdown,
and autoretry on fault.

The logic and analog control is powered from a separate VCC pin accepting
2.7 V to 5.5 V. An I2C interface configures all thresholds and reads the
diagnostic status, and an on-chip 8-bit ADC reads the current through each
active channel as well as the input and output voltages. An external resistor
from ADDR to GND latches one of eight I2C addresses at power-up, and the I2C
transactions optionally carry a PEC byte. An active-low, open-drain INT output
asserts whenever an unmasked fault is present. The ADC readings are also
mirrored into legacy registers behind a mux setting, for compatibility with
MAX20086-MAX20089 software. Both devices are ASIL B compliant and operate over
the -40°C to +125°C ambient temperature range.

Applications
------------

* Power-over-Coax (PoC) protection for camera systems
* Automotive camera power distribution
* Fault diagnostics
* ASIL B safety applications

ADP20086 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (I2C), alongside the optional EN and INTB GPIO
pins and an IRQ controller if the application needs hardware enable control or
interrupt-driven fault handling.

The first API to be called is **adp20086_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. The initialization
reads the ID register to capture the part ID and revision, and auto-detects
whether the device expects a PEC byte from the ID.PECE bit, so that every
subsequent transfer is framed correctly. If an EN GPIO is supplied, it is
driven high during initialization, because the CONFIG.EN[4:1] bits initialize
from OTP and may already enable outputs.

Use **adp20086_remove** to free the resources allocated by the init function.

Device Identification
~~~~~~~~~~~~~~~~~~~~~

The identity captured during initialization is read back without further bus
traffic using **adp20086_get_part_id** and **adp20086_get_revision**. Whether
the device is operating with PEC enabled is reported by
**adp20086_get_pece**.

Register Access
~~~~~~~~~~~~~~~

Raw register access is provided by **adp20086_read** and **adp20086_write**,
which handle address framing and append or verify the PEC byte when the device
has it enabled. Read-modify-write access to a bitfield is provided by
**adp20086_update_register**, and **adp20086_get_register_field** extracts a
masked field and shifts it down to bit 0.

Output Channel Control
~~~~~~~~~~~~~~~~~~~~~~

The four output channels are enabled and disabled individually through the
CONFIG.EN[4:1] bits using **adp20086_set_channel**, or all at once with
**adp20086_set_all_channels**. The current enable state of every channel is
read back into a ``struct adp20086_enabled_channels`` by
**adp20086_get_enabled_channels**.

Hardware Enable and Interrupt Pins
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When an EN GPIO is wired to the device, **adp20086_set_gpio_en** drives it and
**adp20086_get_gpio_en** reads its current state. Outputs only start their
soft-start ramp when the EN pin is high, all supply voltages are in range, and
the corresponding CONFIG.EN bit is set. The raw state of the active-low INTB
pin is available through **adp20086_get_intb**.

Interrupt Handling
~~~~~~~~~~~~~~~~~~

The driver supports interrupt-driven fault handling on the open-drain INTB
pin. Register a handler with **adp20086_set_interrupt_callback**, then enable
delivery with **adp20086_arm_interrupt**. The interrupt handler itself only
latches that INTB fired; the actual status read and callback dispatch happen
in thread context when the application calls **adp20086_process_interrupt**,
which also re-arms the pin. This keeps I2C traffic and any logging out of
interrupt context. Delivery is stopped with **adp20086_disarm_interrupt**, and
**adp20086_get_interrupt_armed** reports whether INTB is currently armed.

Current Limit
~~~~~~~~~~~~~

Each channel has a 4-bit current limit setting selected from
``enum adp20086_ilim_available``, covering 52 mA to 832 mA in 52 mA steps. Set
one channel with **adp20086_set_ilim** or all four with
**adp20086_set_all_ilim**. Read it back either as the raw register code with
**adp20086_get_ilim_code** or converted to microamperes with
**adp20086_get_ilim**.

Input Voltage Protection
~~~~~~~~~~~~~~~~~~~~~~~~

The window of input voltage allowed through to the outputs is bounded by the
UVIN_SET and OVIN_SET thresholds, configured in microvolts with
**adp20086_set_uvin** and **adp20086_set_ovin**. The outputs are only enabled
while VIN sits between the two thresholds. Crossing OVIN bypasses the
soft-shutdown ramp and turns the outputs off immediately to protect the
downstream circuitry, while crossing UVIN brings them down through the
soft-shutdown ramp. The thresholds are detected by the on-chip ADC and carry
3 LSB of hysteresis, approximately 216 mV. Read the thresholds back with
**adp20086_get_uvin** and **adp20086_get_ovin**, or as raw codes with
**adp20086_get_uvin_code** and **adp20086_get_ovin_code**.

Output Undervoltage Protection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A per-channel output undervoltage threshold is configured in microvolts with
**adp20086_set_uvout** and read back with **adp20086_get_uvout**, or as a raw
code with **adp20086_get_uvout_code**.

Load and Open-Load Detection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Load detection raises an interrupt when a channel's current rises above the
LDET_SET threshold, which can be used to detect a load being connected or as a
current warning. Enable it per channel with **adp20086_set_load_detection** or
on all channels with **adp20086_set_all_load_detection**, and read the enable
state back with **adp20086_get_enabled_load_detection**. The threshold is
configured in microamperes with **adp20086_set_ldet_threshold** and read back
with **adp20086_get_ldet_threshold** or **adp20086_get_ldet_threshold_code**.

Open-load detection works in the opposite direction, asserting when a
channel's current falls below the IOPEN_SET threshold. The threshold is set
with **adp20086_set_iopen** and read back with **adp20086_get_iopen** or
**adp20086_get_iopen_code**. Beyond raising an interrupt, open-load protection
can automatically disable the affected channel and engage a pull-down resistor
to discharge the residual output voltage; enable it per channel with
**adp20086_set_open_protection**, on all channels with
**adp20086_set_all_open_protection**, and read the enable state back with
**adp20086_get_enabled_open_protection**.

Soft-Start and Soft-Shutdown Ramps
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The soft-start ramp duration is selected per channel from
``enum adp20086_soft_startup`` (0.5 ms, 1 ms, 2 ms, or 4 ms) using
**adp20086_set_soft_startup**, or for all channels with
**adp20086_set_all_soft_startup**, and read back with
**adp20086_get_soft_startup**. The soft-shutdown ramp is selected from
``enum adp20086_soft_shutdown`` (0.25 ms, 0.5 ms, 1 ms, or 2 ms) using
**adp20086_set_soft_shutdown** or **adp20086_set_all_soft_shutdown**, and read
back with **adp20086_get_soft_shutdown**.

Interrupt Masks
~~~~~~~~~~~~~~~

Every fault source that can assert INTB is individually maskable. A single
source from ``enum adp20086_interrupt_source`` is masked or unmasked with
**adp20086_set_interrupt_mask** and queried with
**adp20086_get_interrupt_mask**. For bulk configuration, build a
``struct adp20086_masks`` and apply it in one pass with
**adp20086_configure_interrupt_masks**, read the current configuration back
with **adp20086_get_configured_interrupt_masks**, and restore the register
defaults with **adp20086_reset_masks**. **adp20086_get_default_mask** returns
a ``struct adp20086_masks`` matching the device reset state, which is a
convenient starting point to modify.

Status and Diagnostics
~~~~~~~~~~~~~~~~~~~~~~

**adp20086_get_status** reads all five status registers in one pass and
expands them into a ``struct adp20086_status``, covering the device-level
flags (thermal limit, ADC conversion complete, input OV/UV, VDD OV/UV) and the
per-channel flags (thermal shutdown, overcurrent, output OV/UV, load detected,
open load) as well as the self-test and memory integrity flags. A single flag
can be polled with **adp20086_get_status_field** using
``enum adp20086_status_field``. Latched status bits are cleared with
**adp20086_clear_status**.

**adp20086_check_self_test** reports whether any fatal self-test or integrity
fault is latched: ADC frequency calibration, BIST, parity error, or an
uncorrectable ECC error. A corrected ECC error is deliberately excluded,
because it is non-fatal.

The output overvoltage comparators have a dedicated self-test, enabled with
**adp20086_set_ovtst** and queried with **adp20086_get_ovtst**. Setting it
forces the inputs of all four output OV comparators high, so a working
comparator answers by latching its OVOUT status bit. The data sheet requires
every EN[x] to be low to run the diagnostic, which the driver enforces:
writing 1 while any output is enabled returns ``-EINVAL`` rather than running
a test whose result would be meaningless.

Telemetry
~~~~~~~~~

The on-chip 8-bit ADC continuously measures the input voltage, the per-channel
output voltages and currents, and the internal VDD rail. Each reading is
available either as the raw register code or converted to integer micro-units:
**adp20086_read_vin** and **adp20086_read_vin_code** for the input voltage,
**adp20086_read_vout** and **adp20086_read_vout_code** for a channel output
voltage, **adp20086_read_iout** and **adp20086_read_iout_code** for a channel
output current, and **adp20086_read_vdd** and **adp20086_read_vdd_code** for
the internal supply.

An optional IIR filter smooths the ADC results; it is controlled with
**adp20086_set_iir_filter** and **adp20086_get_iir_filter**.

Output Discharge
~~~~~~~~~~~~~~~~

The integrated output discharge resistors are enabled and disabled with
**adp20086_set_discharge_resistors** and read back with
**adp20086_get_discharge_resistors**.

Legacy MAX20086-MAX20089 ADC Path
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For compatibility with MAX20086-MAX20089 software, the ADC results are also
mirrored into four legacy registers whose meaning depends on a mux setting.
The mux selects between switch current, switch voltage, and the input and VDD
supplies; set it with **adp20086_set_mux_config** using
``enum adp20086_legacy_adc_mux_config`` and read it back with
**adp20086_get_mux_config**. The legacy registers are only refreshed while
conversions are enabled, so set ENC first with **adp20086_set_enc_config**
(read back with **adp20086_get_enc_config**); otherwise a read returns whatever
the previous conversion left behind. The registers themselves are read with
**adp20086_read_legacy_adc**.

The CLR bit, controlled by **adp20086_set_clr_config** and read back with
**adp20086_get_clr_config**, selects whether INT responds only to real-time
faults or remains asserted until the status registers are read.

Applications using this driver should prefer the dedicated telemetry APIs
above, which read their own registers and are unaffected by either MUX or ENC.

ADP20086 Driver Initialization Example
--------------------------------------

.. code-block:: bash

	int ret;
	struct adp20086_dev *dev;
	struct no_os_irq_ctrl_desc *gpio_irq_desc;

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
		.max_speed_hz = I2C_MAX_SPEED,
		.slave_address = ADP20086_6P8KOHM_ADDR,
		.platform_ops = I2C_OPS,
		.extra = I2C_EXTRA,
	};

	struct no_os_gpio_init_param adp20086_en_ip = {
		.port = EN_PORT,
		.number = EN_PIN,
		.pull = NO_OS_PULL_DOWN,
		.platform_ops = GPIO_OPS,
		.extra = GPIO_EXTRA,
	};

	/* INTB is open drain and active low, so it idles high. */
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
		.en_gpio_ip = &adp20086_en_ip,
		.intb_gpio_ip = &adp20086_intb_ip,
	};

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

Channels
~~~~~~~~

The IIO driver exposes ten channels: the four output voltages, the input
voltage, and the internal VDD rail as voltage channels, plus the four output
currents as current channels.

* ``vout1, vout2, vout3, vout4 - per-channel output voltage``
* ``vin - input supply voltage``
* ``vdd - internal 1.8 V VDD rail voltage``
* ``iout1, iout2, iout3, iout4 - per-channel output current``

Telemetry Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The voltage channels expose the raw ADC code and its scale.

* ``raw - raw ADC code for the channel``
* ``scale - scale factor converting the raw code to millivolts``

Output Channel Attributes
~~~~~~~~~~~~~~~~~~~~~~~~~

The current channels carry the raw reading plus the whole per-output
configuration surface, since each one maps to a physical output switch. This
includes the per-channel fault status and interrupt masks, which are the
channel-level counterparts of the device-level attributes listed further
below.

* ``raw - raw ADC code for the channel current``
* ``scale - scale factor converting the raw code to milliamperes``
* ``en - enable or disable this output channel``
* ``load_detect_en - enable load current detection on this output``
* ``open_prot_en - enable open-load protection on this output``
* ``en_available - the accepted values for en``
* ``status_thermal_shutdown - this output is in thermal shutdown``
* ``status_overcurrent - this output is in current limit``
* ``status_ovout - this output is above the input voltage``
* ``status_uvout - this output is below its undervoltage threshold``
* ``status_load_detected - this output current rose above ldet_threshold``
* ``status_open_load - this output current fell below iopen``
* ``mask_uvout - mask this output's undervoltage interrupt``
* ``mask_open_load - mask this output's open-load interrupt``
* ``ilim - current limit for this output, in milliamperes``
* ``ilim_available - the selectable current limit values``
* ``ldet_threshold - load detection current threshold, in milliamperes``
* ``iopen - open-load current threshold, in milliamperes``
* ``uvout - output undervoltage threshold, in millivolts``
* ``soft_startup - soft-start ramp duration for this output``
* ``soft_startup_available - the selectable soft-start ramp durations``
* ``soft_shutdown - soft-shutdown ramp duration for this output``
* ``soft_shutdown_available - the selectable soft-shutdown ramp durations``

Global Attributes
~~~~~~~~~~~~~~~~~

* ``device_id - part ID read from the ID register``
* ``revision - revision read from the ID register``
* ``pece - whether the device has the PEC byte enabled``
* ``gpio_en - state of the hardware EN pin``
* ``interrupt_arm - arm or disarm INTB fault delivery``
* ``ovin - input overvoltage threshold, in millivolts``
* ``uvin - input undervoltage threshold, in millivolts``
* ``ovtst_en - run the output overvoltage comparator self-test``
* ``iir_filter - enable the IIR filter on the ADC results``
* ``discharge_resistors - enable the integrated output discharge resistors``
* ``clear_status - write to clear all latched status bits``
* ``mux - legacy ADC mux setting``
* ``mux_available - the selectable legacy ADC mux settings``
* ``enc - enable conversions for the legacy ADC registers``

Status Attributes
~~~~~~~~~~~~~~~~~

Each latched device-level status bit is exposed read-only.

* ``status_tlim_serv - IC thermal limit reached``
* ``status_acc - ADC conversion complete``
* ``status_ovin - input overvoltage``
* ``status_uvin - input undervoltage``
* ``status_ovdd - VDD overvoltage``
* ``status_uvdd - VDD undervoltage``
* ``status_adc_fcal - ADC frequency calibration fault``
* ``status_intbsts - INTB pin status``
* ``status_bist - built-in self-test fault``
* ``status_parerr - register parity error``
* ``status_ecc_cor - corrected ECC error``
* ``status_ecc_det - detected, uncorrectable ECC error``

Mask Attributes
~~~~~~~~~~~~~~~

Each device-level interrupt source can be masked independently. The value is
the raw MASK register bit: 1 means the source is masked, so INTB stays
released when it asserts. The corresponding status bit still latches either
way, because masking suppresses the interrupt, not the fault record.

* ``mask_acc - mask the ADC conversion complete interrupt``
* ``mask_ts - mask the thermal shutdown interrupt``
* ``mask_vdd - mask the VDD OV/UV interrupt``
* ``mask_vin - mask the input OV/UV interrupt``
* ``mask_oc - mask the overcurrent interrupt``
* ``mask_ov - mask the output overvoltage interrupt``
* ``mask_uv - mask the output undervoltage interrupt``
* ``mask_fcal - mask the ADC frequency calibration interrupt``
* ``mask_bist - mask the built-in self-test interrupt``
* ``mask_parerr - mask the parity error interrupt``
* ``mask_ecc_cor - mask the corrected ECC error interrupt``
* ``mask_ecc_det - mask the detected ECC error interrupt``

Debug Attributes
~~~~~~~~~~~~~~~~

The legacy ADC registers are exposed as raw codes only. What a legacy register
holds depends on the ``mux`` attribute, and under the VIN/VDD mux setting it is
not even uniform across the four registers, so no single scale describes them.
Use the telemetry channels above for calibrated readings; these exist to check
that a MAX20086-MAX20089 era host would see the right bytes. Set ``enc`` first,
otherwise the registers are not refreshed.

* ``legacy_adc1_raw - raw legacy ADC1 register code``
* ``legacy_adc2_raw - raw legacy ADC2 register code``
* ``legacy_adc3_raw - raw legacy ADC3 register code``
* ``legacy_adc4_raw - raw legacy ADC4 register code``
* ``direct_reg_access - read or write any device register directly``

ADP20086 IIO Driver Initialization Example
------------------------------------------

.. code-block:: bash

	int ret;

	struct adp20086_iio_desc *adp20086_iio_desc;
	struct adp20086_iio_desc_init_param adp20086_iio_ip = {
		.adp20086_init_param = &adp20086_ip,
	};

	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = {0};

	ret = adp20086_iio_init(&adp20086_iio_desc, &adp20086_iio_ip);
	if (ret)
		goto exit;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adp20086",
			.dev = adp20086_iio_desc,
			.dev_descriptor = adp20086_iio_desc->iio_dev,
		},
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto remove_iio_adp20086;

	return iio_app_run(app);
