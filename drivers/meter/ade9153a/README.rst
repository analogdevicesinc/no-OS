ADE9153A no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADE9153A <https://www.analog.com/ADE9153A>`_

Overview
--------

The ADE9153A is a highly accurate, single-phase, energy metering IC with
autocalibration. The mSure autocalibration feature allows a meter to
automatically calibrate the current and voltage channels without using an
accurate source or an accurate reference meter when a shunt resistor is used
as a current sensor. Class 1 and Class 2 meters are supported by mSure
autocalibration.

The ADE9153A incorporates three high performance analog-to-digital converters
(ADCs), providing an 88 dB signal-to-noise ratio (SNR). The device offers an
advanced metrology feature set including line voltage and current, active
energy, fundamental reactive energy, apparent energy calculations, and current
and voltage RMS calculations. Power quality measurements such as zero crossing
detection, line period calculation, angle measurement, dip and swell, peak and
overcurrent detection, and power factor measurements are also supported.

Each input channel supports independent and flexible gain stages. Current
Channel A is ideal for shunts, having a flexible gain stage and providing
full-scale input ranges from 62.5 mV peak down to 26.04 mV peak. Current
Channel B has gain stages of 1x, 2x, and 4x for use with current transformers
(CTs). A high speed, 10 MHz, SPI port allows access to the ADE9153A registers.

The ADE9153A operates from a 3.3 V supply and is available in a 32-lead LFCSP
package.

Applications
------------

* Single-phase energy meters
* Energy and power measurement
* Street lighting
* Smart power distribution systems
* Machine health

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The driver communicates with the device via SPI interface.
The ``ade9153a_init`` function initializes the device by configuring the SPI
interface, setting up GPIO pins (reset, data ready, SS, SCK), and optionally
configuring a data ready interrupt. After initialization,
``ade9153a_setup`` configures the device operating parameters such as PGA gain,
high-pass filter corner frequency, line frequency selection (50/60 Hz),
voltage level, and energy accumulation settings.

Autocalibration
~~~~~~~~~~~~~~~

The mSure autocalibration feature allows automatic calibration of current and
voltage channels. Functions ``ade9153a_start_autocal_ai``,
``ade9153a_start_autocal_bi``, and ``ade9153a_start_autocal_av`` initiate
autocalibration on the respective channels. The calibration mode can be set
to normal or turbo using the ``ade9153a_acalmode_e`` enum. Calibration results
can be read using ``ade9153a_read_autocal_vals``.

Energy and Power Measurement
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``ade9153a_energy_vals`` function reads active, fundamental reactive, and
apparent energy register values. The ``ade9153a_power_vals`` function reads
active, fundamental reactive, and apparent power values. Energy accumulation
modes (signed, absolute, positive, negative) are configured via
``ade9153a_wattacc`` and ``ade9153a_varacc``. The CF pin output can be
configured using ``ade9153a_cf1sel`` and ``ade9153a_cf2sel``.

RMS Measurements
~~~~~~~~~~~~~~~~

Current and voltage RMS values are obtained using ``ade9153a_rms_vals``.
Half-cycle RMS values are available through ``ade9153a_half_rms_vals``.
Power quality values including power factor, period, and angle measurements
are read using ``ade9153a_power_quality_vals``.

Temperature Measurement
~~~~~~~~~~~~~~~~~~~~~~~

The on-chip temperature sensor is enabled with ``ade9153a_temp_en`` and
started with ``ade9153a_temp_start``. The sampling interval is configured
using ``ade9153a_temp_time``. Temperature results are read using
``ade9153a_temp_val``.

Interrupt Configuration
~~~~~~~~~~~~~~~~~~~~~~~

The driver provides granular interrupt control through enable/disable function
pairs for each status flag (e.g., ``ade9153a_enable_dready_int`` /
``ade9153a_disable_dready_int``). Status flags can be read with the
corresponding ``ade9153a_get_*`` functions and cleared with the
``ade9153a_clear_*`` functions.

Driver Initialization Example
------------------------------

.. code-block:: c

   #include "ade9153a.h"

   struct ade9153a_dev *ade9153a_dev;
   struct no_os_irq_ctrl_desc *irq_ctrl;

   // IRQ controller must be initialized before ade9153a_init
   ret = no_os_irq_ctrl_init(&irq_ctrl, &irq_ip);
   if (ret)
       return ret;

   struct ade9153a_init_param ade9153a_ip = {
       .spi_init = &spi_ip,
       .gpio_rdy = &gpio_rdy_ip,
       .gpio_reset = &gpio_reset_ip,
       .spi_en = 1,
       .irq_ctrl = irq_ctrl,
       .ai_pga_gain = ADE9153A_AI_GAIN_16,
       .hpf_crn = ADE9153A_HPF_CORNER_0_625_HZ,
       .freq = F_50_HZ,
       .vlevel = 0x002C11E8,
       .rsmall = 0x03E8,
       .no_samples = 0x0F9F,
       .ai_gain = -(1 << 28),
   };

   int ret;

   // Initialize the device (internally resets, starts DSP, and verifies product ID)
   ret = ade9153a_init(&ade9153a_dev, ade9153a_ip);
   if (ret)
       goto free_irq;

   // Configure device parameters (gain, HPF, frequency, energy accumulation)
   ret = ade9153a_setup(ade9153a_dev, ade9153a_ip);
   if (ret)
       goto free_dev;

   free_dev:
       ade9153a_remove(ade9153a_dev);
   free_irq:
       no_os_irq_ctrl_remove(irq_ctrl);
       return ret;
