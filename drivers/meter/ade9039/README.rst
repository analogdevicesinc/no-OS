ADE9039 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* :adi:`ADE9039`

Overview
--------

The ADE9039 is a high performance, polyphase energy and power quality
measurement IC intended for use with di/dt (Rogowski) current sensors. It
provides total and fundamental active, reactive, and apparent energy
measurements, as well as current and voltage RMS, power factor, total harmonic
distortion (THD), line period, phase angle, and zero-crossing measurements on a
per-phase and total-system basis.

The device supports Class 0.2 accuracy metrology, with less than 0.1% error in
active energy over a 2000:1 dynamic range and less than 0.1% error in the
voltage and current RMS measurements over a 1000:1 range. In addition to the
filter-based RMS values, it computes fast half-cycle RMS (VRMS½ and IRMS½)
values refreshed every half line cycle, which are used by the on-chip dip,
swell, and overcurrent monitors for power quality event detection. A flexible
waveform buffer can continuously resample the input waveforms to a fixed number
of points per line cycle, simplifying harmonic analysis in an external
processor.

The ADE9039 communicates over a 10 MHz SPI port and operates from a single
3.3 V supply. Power mode selection (normal, tamper, current peak detect, and
idle) is controlled through the PSM0 and PSM1 pins, and an on-chip temperature
sensor with factory calibrated gain and offset trim is available. It is offered
in a 40-lead LFCSP package.

Applications
------------

* Advanced metering infrastructure (AMI)
* Data center power distribution units (PDU)
* Sub-meters
* Smart circuit breakers
* Home energy management devices

ADE9039 Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol, together with the GPIOs used for the PSM0,
PSM1, and reset lines.

The first API to be called is **ade9039_init**. Make sure that it returns 0,
which means that the driver was initialized correctly. The initialization
applies the selected power mode through **ade9039_set_power_mode**, performs a
hardware reset of the device, waits for the reset recovery time, verifies the
chip and part identity by reading the ``VERSION`` and ``PART_ID`` registers,
and optionally enables the on-chip temperature sensor based on the ``temp_en``
initialization parameter.

Device Setup
~~~~~~~~~~~~~

After initialization, **ade9039_setup** writes the default measurement
configuration to the device. It programs the PGA gains, the ``CONFIG0`` through
``CONFIG3`` registers, the accumulation mode (``ACCMODE``), the zero-crossing
and line-period selection (``ZX_LP_SEL``), the interrupt and event masks, the
waveform buffer configuration, and the energy/power accumulation settings,
then starts the DSP by writing the ``RUN`` register. The default values are
defined in ``ade9039.h`` and target a 50 Hz, 3-phase, 4-wire Wye configuration.

Power Mode Selection
~~~~~~~~~~~~~~~~~~~~~

The ADE9039 supports four power modes selected through the PSM0 and PSM1 pins:
normal mode (``NORMAL_MODE``), tamper measurement mode (``TAMPER_MODE``),
current peak detect mode (``CURRENT_PEAK_DETECT_MODE``), and idle mode
(``IDLE_MODE``). The mode stored in the ``power_mode`` field is applied by
**ade9039_set_power_mode**, which drives the PSM0/PSM1 GPIOs accordingly. Note
that the SPI interface is only available in normal and tamper modes.

Measurement Readout
~~~~~~~~~~~~~~~~~~~

Per-phase measurement data is read from the device with a set of helper
functions, each taking a phase selector (``ADE9039_PHASE_A``,
``ADE9039_PHASE_B``, or ``ADE9039_PHASE_C``) and storing the raw register
values in the device structure:

* **ade9039_read_rms_ph** - reads the filter-based current and voltage RMS
  values (``irms_val``, ``vrms_val``).
* **ade9039_read_power_ph** - reads total active, total apparent, fundamental
  active, fundamental reactive, and fundamental apparent power.
* **ade9039_read_energy_ph** - reads the accumulated total active, total
  apparent, fundamental active, fundamental reactive, and fundamental apparent
  energy.
* **ade9039_read_power_factor_ph** - reads the total and fundamental power
  factor.

Energy Accumulation Model
~~~~~~~~~~~~~~~~~~~~~~~~~~

The energy accumulation behavior is configured with **ade9039_set_egy_model**.
Three models are available:

* ``ADE9039_EGY_WITH_RESET`` - energy registers are read and automatically
  reset. The ``value`` parameter must be set to 1.
* ``ADE9039_EGY_HALF_LINE_CYCLES`` - energy is accumulated over a specified
  number of half line cycles.
* ``ADE9039_EGY_NR_SAMPLES`` - energy is accumulated over a specified number of
  samples.

The function stops the DSP, configures the ``EP_CFG`` and ``EGY_TIME``
registers according to the selected model, and restarts the DSP.

Temperature Measurement
~~~~~~~~~~~~~~~~~~~~~~~~

The ADE9039 includes an on-chip temperature sensor. When enabled through the
``temp_en`` initialization parameter, a conversion is started by setting the
``TEMP_START`` bit in the ``TEMP_CFG`` register. **ade9039_read_temp** reads the
raw result from ``TEMP_RSLT`` and the factory calibrated gain and offset from
``TEMP_TRIM``, storing them in the ``temp_raw``, ``temp_gain``, and
``temp_offset`` fields so the caller can compute the temperature in degrees
Celsius.

Register Access
~~~~~~~~~~~~~~~

Low-level register access is provided through **ade9039_read**,
**ade9039_write**, and **ade9039_update_bits**. The driver automatically
handles the different register widths: 16-bit registers (addresses between
``RUN`` and ``VERSION``) and 32-bit registers (all other addresses).

Status and Interrupt Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The interrupt and event status of the device can be polled with
**ade9039_get_int_status0**, which returns the state of a masked bit in the
``STATUS0`` register, and with the more general **ade9039_check_bit_status**,
which returns the state of a masked bit in any register.

ADE9039 Driver Initialization Example
-------------------------------------

.. code-block:: c

   struct ade9039_dev *ade9039;
   struct no_os_spi_init_param ade9039_spi_ip = {
   	.device_id = SPI_DEVICE_ID,
   	.max_speed_hz = 1000000,
   	.mode = NO_OS_SPI_MODE_0,
   	.chip_select = SPI_CS,
   	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   	.platform_ops = SPI_OPS,
   	.extra = SPI_EXTRA
   };
   struct ade9039_init_param ade9039_ip = {
   	.spi_init = &ade9039_spi_ip,
   	.psm0_desc = psm0_desc,
   	.psm1_desc = psm1_desc,
   	.reset_desc = reset_desc,
   	.power_mode = NORMAL_MODE,
   	.freq = 50,
   	.temp_en = true
   };
   int ret;

   ret = ade9039_init(&ade9039, ade9039_ip);
   if (ret)
   	goto error;

   ret = ade9039_setup(ade9039);
   if (ret)
   	goto error;
