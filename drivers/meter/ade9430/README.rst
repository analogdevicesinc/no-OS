ADE9430 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADE9430 <https://www.analog.com/ADE9430>`_

Overview
--------

The ADE9430 is a highly accurate, fully integrated, polyphase energy and power
quality monitoring device. It features seven high performance, 24-bit
sigma-delta ADCs with 101 dB SNR at 8 kSPS, a wide input voltage range of
+/-1 V (707 mV rms full scale at gain = 1), and differential inputs. The
integrated high end reference ensures low drift over temperature with a combined
drift of less than +/-25 ppm/C maximum for the entire signal chain including the
programmable gain amplifier (PGA) and ADC, enabling a 10,000:1 dynamic input
range and Class 0.2 metrology with standard external components.

The device provides total and fundamental active, reactive, and apparent energy
measurements, as well as VRMS, IRMS, power factor, and period measurements per
phase. It supports VRMS and IRMS one-cycle values refreshed each cycle, filtered
RMS values updated every sample, and 10/12-cycle RMS calculations. Phase angle
measurements and zero-crossing detection are also included.

The ADE9430 offers a resampled waveform of 1024 points per 10 cycles or 12
cycles, simplifying FFT calculation of at least 40 harmonics in an external
processor. Combined with the optional ADSW-PQ-CLS Power Quality Library and a
host microcontroller, it enables the design of standalone monitoring or
protection systems compliant with IEC 61000-4-30 Class S.

The device communicates via SPI and operates from a 2.97 V to 3.63 V supply with
a 24.576 MHz crystal, over the -40 C to +85 C temperature range.

Applications
------------

* Energy and power monitoring
* Standards compliant power quality monitoring
* Protective devices
* Machine health
* Smart power distribution units
* Polyphase energy meters

ADE9430 Device Configuration
-----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol. The first API to be called is ``ade9430_init``.
Make sure that it returns 0, which means that the driver was initialized
correctly. The initialization performs a software reset, verifies the chip
identity, and optionally enables the on-chip temperature sensor based on the
``temp_en`` parameter.

Temperature Measurement
~~~~~~~~~~~~~~~~~~~~~~~

The ADE9430 includes an on-chip temperature sensor. To enable it, set the
``temp_en`` field in the initialization parameters to ``true``. Once enabled,
temperature readings can be obtained by calling ``ade9430_read_temp``, which
triggers a measurement, reads the raw ADC result, and applies the factory
calibrated gain and offset trim values to compute the temperature in degrees
Celsius. The result is stored in the ``temp_deg`` field of the device structure.

Phase Measurement
~~~~~~~~~~~~~~~~~

The ADE9430 supports three-phase measurement for current, voltage, and power.
The ``ade9430_read_data_ph`` function reads IRMS, VRMS, and active power (WATT)
values for a specified phase (A, B, or C). The raw register values are converted
to physical units using the device resolution constants and stored in the device
structure fields ``irms_val``, ``vrms_val``, and ``watt_val``.

Energy Accumulation
~~~~~~~~~~~~~~~~~~~

The energy accumulation model can be configured using ``ade9430_set_egy_model``.
Three models are available:

* ``ADE9430_EGY_WITH_RESET``: Energy registers are read and automatically reset.
  The value parameter must be set to 1.
* ``ADE9430_EGY_HALF_LINE_CYCLES``: Energy is accumulated over a specified
  number of half line cycles.
* ``ADE9430_EGY_NR_SAMPLES``: Energy is accumulated over a specified number of
  samples.

The function configures the EP_CFG and EGY_TIME registers accordingly and
starts the DSP by writing to the RUN register.

Register Access
~~~~~~~~~~~~~~~

Low-level register access is provided through ``ade9430_read``,
``ade9430_write``, and ``ade9430_update_bits``. The driver automatically handles
the different register widths: 16-bit registers (addresses 0x0480 to 0x04FE)
and 32-bit registers (all other addresses).

ADE9430 Driver Initialization Example
--------------------------------------

.. code-block:: c

   struct ade9430_dev *ade9430;
   struct no_os_spi_init_param ade9430_spi_ip = {
   	.device_id = SPI_DEVICE_ID,
   	.max_speed_hz = 1000000,
   	.mode = NO_OS_SPI_MODE_0,
   	.chip_select = SPI_CS,
   	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   	.platform_ops = SPI_OPS,
   	.extra = SPI_EXTRA
   };
   struct ade9430_init_param ade9430_ip = {
   	.spi_init = &ade9430_spi_ip,
   	.temp_en = true
   };
   int ret;

   ret = ade9430_init(&ade9430, ade9430_ip);
   if (ret)
   	goto error;
