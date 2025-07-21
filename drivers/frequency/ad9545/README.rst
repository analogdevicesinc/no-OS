AD9545 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9545 <https://www.analog.com/AD9545>`_

Overview
--------

The AD9545 is a quad-input, 10-output, dual DPLL/IEEE 1588 synchronizer
and jitter cleaner designed for telecommunications and networking
applications. It supports existing and emerging ITU standards for the
delivery of frequency, phase, and time of day over service provider
packet networks, including ITU-G.8262, ITU-T G.812, ITU-T G.813, and
ITU-T G.8273.2. The 10 clock outputs are synchronized to any one of up
to four input references, with the digital phase-locked loops reducing
timing jitter associated with the external references. The digitally
controlled loop and holdover circuitry continuously generate a low jitter
output signal, even when all reference inputs fail. Key features include
programmable reference switching, fast DPLL locking, dual independent
auxiliary NCOs for IEEE 1588 servo feedback, and optional use of a crystal
resonator or oscillator for frequency stability. The AD9545 operates from
a single 1.8 V power supply with internal regulation and is available
in a 48-lead LFCSP package, operating over the -40°C to +85°C temperature
range.

Applications
------------

* Global positioning system (GPS), PTP (IEEE 1588), and synchronous
  Ethernet (SyncE) jitter cleanup and synchronization
* Optical transport networks (OTN), synchronous digital hierarchy (SDH),
  and macro and small cell base stations
* Small base station clocking, including baseband and radio Stratum 2,
  Stratum 3e, and Stratum 3 holdover, jitter cleanup, and phase
  transient control
* JESD204B support for analog-to-digital converter (ADC) and
  digital-to-analog converter (DAC) clocking
* Cable infrastructures
* Carrier Ethernet

AD9545 Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the
support for the communication protocol (SPI or I2C) via the
``comm_type`` field in the initialization parameter structure.

The first API to be called is ``ad9545_init``. Make sure that it returns
0, which means that the driver was initialized correctly. After
initialization, ``ad9545_setup`` must be called to configure system
clocks, input references, auxiliary NCOs, TDCs, and DPLLs.

PLL Configuration
~~~~~~~~~~~~~~~~~

The AD9545 features two independent DPLLs that can be configured for
different operating modes. The driver supports active closed-loop
operation, holdover, and free-run modes via ``ad9545_set_pll_mode``.
Translation profiles define the relationship between input references
and output frequencies, with automatic or manual profile selection modes
available.

Clock Management
~~~~~~~~~~~~~~~~

Clock management functions, including ``ad9545_in_clk_recalc_rate`` and
``ad9545_get_nco_freq_hz``, manage recalibration of clock rates and
frequency settings. The R dividers and Q dividers can be configured via
``ad9545_set_r_div`` and ``ad9545_set_q_div`` respectively, while NCO
center frequencies are set with ``ad9545_set_nco_center_freq``.

Auxiliary Features
~~~~~~~~~~~~~~~~~~

Auxiliary setup functions such as ``ad9545_aux_dpll_setup`` and
``ad9545_aux_tdcs_setup`` configure additional components like DPLLs and
TDCs. Two independent auxiliary NCOs provide frequency output from 1 Hz
to 65,535 Hz, suitable for IEEE 1588 Version 2 servo feedback in PTP
applications.

Communication Protocol
~~~~~~~~~~~~~~~~~~~~~~

The driver supports both SPI and I2C communication interfaces. Key
functions such as ``ad9545_spi_reg_read``, ``ad9545_spi_reg_write``,
``ad9545_i2c_reg_read``, and ``ad9545_i2c_reg_write`` handle register
access. Low-level register operations via ``ad9545_read_reg``,
``ad9545_write_reg``, and ``ad9545_write_mask`` enable precise control
over device registers.

Driver Initialization Example
------------------------------

.. code-block:: c

   #include "ad9545.h"

   struct no_os_spi_init_param ad9545_spi_ip = {
       .device_id = 0,
       .max_speed_hz = 1000000,
       .mode = NO_OS_SPI_MODE_0,
       .platform_ops = SPI_OPS,
       .chip_select = 0,
       .extra = SPI_EXTRA
   };

   struct ad9545_init_param ad9545_ip = {
       .spi_init = &ad9545_spi_ip,
       .comm_type = SPI,
   };

   int main(void)
   {
       int32_t ret;
       struct ad9545_dev *dev;

       ret = ad9545_init(&dev, &ad9545_ip);
       if (ret)
           goto error;

       ret = ad9545_setup(dev);
       if (ret)
           goto error_remove;

       /* Device is ready for use. */

       ad9545_remove(dev);
       return 0;

   error_remove:
       ad9545_remove(dev);
   error:
       return ret;
   }
