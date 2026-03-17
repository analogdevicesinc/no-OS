ADAS1000 no-OS Driver
=====================

.. no-os-doxygen::

Supported Devices
-----------------

* `ADAS1000 <https://www.analog.com/ADAS1000>`_
* `ADAS1000-1 <https://www.analog.com/ADAS1000-1>`_
* `ADAS1000-2 <https://www.analog.com/ADAS1000-2>`_

Overview
--------

The ADAS1000/ADAS1000-1/ADAS1000-2 measure electro cardiac (ECG) signals,
thoracic impedance, pacing artifacts, and lead-on/lead-off status and output
this information in the form of a data frame supplying either lead/vector or
electrode data at programmable data rates. Their low power and small size make
them suitable for portable, battery-powered applications, while the high
performance also makes them suitable for higher end diagnostic machines.

The ADAS1000 is a full-featured, 5-channel ECG analog front end including
respiration measurement and pace detection. The ADAS1000-1 offers only ECG
channels with no respiration or pace features. The ADAS1000-2 is a secondary
device with only the ECG channels enabled (no respiration, pace, or right leg
drive), intended for gang mode operation to extend the number of electrodes
beyond five.

The devices feature a power/noise scaling architecture where noise can be
reduced at the expense of increasing power consumption. Signal acquisition
channels can be shut down individually to save power, and data rates can be
reduced for further power savings. Low power operation is achievable from
11 mW (1 lead) to 21 mW (all electrodes).

Key features include ac and dc lead-off detection, an internal pace detection
algorithm on 3 leads, thoracic impedance measurement via internal or external
paths, selectable reference lead, shield driver, and a calibration DAC for test
and calibration purposes. A secondary SPI interface is available for providing
ECG data at 128 kHz for external digital pace algorithm detection. CRC
redundancy checking is provided for data integrity verification.

The devices communicate via a standard SPI interface and support programmable
data rates of 2 kHz, 16 kHz, 128 kHz, and 31.25 Hz. They operate from a 3.3 V
analog supply over the -40 C to +85 C temperature range. The ADAS1000 and
ADAS1000-2 are available in both 56-lead LFCSP and 64-lead LQFP packages, while
the ADAS1000-1 is available in 56-lead LFCSP.

Applications
------------

* ECG monitor and diagnostic
* Bedside patient monitoring
* Portable telemetry
* Holter monitors
* Automated external defibrillators (AED)
* Cardiac defibrillators
* Ambulatory monitors
* Pacemaker programmers
* Patient transport
* Stress testing

ADAS1000 Device Configuration
------------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support for
the SPI communication protocol. The SPI clock frequency depends on the selected
frame rate and can be computed using ``adas1000_compute_spi_freq`` prior to
initialization.

The first API to be called is ``adas1000_init``. Make sure that it returns 0,
which means that the driver was initialized correctly. The initialization
performs a software reset, activates all channel words in the data frame, and
sets the frame rate to the value specified in the initialization parameters.

Frame Rate Configuration
~~~~~~~~~~~~~~~~~~~~~~~~

The data output rate can be configured using ``adas1000_set_frame_rate``. The
supported frame rates are:

* 2 kHz (default) - 24-bit data words, 12-word frames
* 16 kHz - 24-bit data words, 12-word frames
* 128 kHz - 16-bit data words, 15-word frames
* 31.25 Hz - 32-bit data words, 12-word frames

The frame rate affects the word size, frame size, and required SPI clock
frequency. When changing the frame rate, the driver automatically recomputes the
frame size.

Frame Word Selection
~~~~~~~~~~~~~~~~~~~~

Individual words can be excluded from the data frame to reduce the amount of
data transferred over SPI. The ``adas1000_set_inactive_framewords`` function
accepts a bitmask specifying which words to exclude. The available words include
lead/electrode data (LA, LL, RA, V1, V2), pace detection data, respiration
magnitude and phase, leads-off status, GPIO data, and CRC.

Reading ECG Data
~~~~~~~~~~~~~~~~

ECG data is read in frames using ``adas1000_read_data``. The function accepts a
buffer, a frame count, and a ``read_param`` structure controlling the read
sequence:

* ``start_read``: Set to true to initiate the frame read sequence by sending a
  FRAMES command.
* ``stop_read``: Set to true to stop the frame read sequence when exiting.
* ``wait_for_ready``: Set to true to wait for the READY bit in the frame header
  before accepting data.
* ``ready_repeat``: Set to true if the device is configured to repeat the frame
  header until data is ready.

CRC Validation
~~~~~~~~~~~~~~

Data integrity can be verified using ``adas1000_compute_frame_crc``, which
computes the CRC for a received frame. The CRC polynomial depends on the frame
rate: a 24-bit CRC is used at 2 kHz and 16 kHz data rates, while a 16-bit CRC
is used at the 128 kHz data rate.

Register Access
~~~~~~~~~~~~~~~

Low-level register access is provided through ``adas1000_read`` and
``adas1000_write``. All registers use 24-bit data words with an 8-bit address
prefix, resulting in 32-bit SPI transactions.

ADAS1000 Driver Initialization Example
---------------------------------------

.. code-block:: c

   struct adas1000_dev *adas1000;
   uint32_t spi_freq;
   struct adas1000_init_param adas1000_ip = {
   	.spi_init = {
   		.device_id = SPI_DEVICE_ID,
   		.max_speed_hz = 10000000,
   		.mode = NO_OS_SPI_MODE_0,
   		.chip_select = SPI_CS,
   		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
   		.platform_ops = SPI_OPS,
   		.extra = SPI_EXTRA
   	},
   	.frame_rate = ADAS1000_2KHZ_FRAME_RATE
   };
   int ret;

   /* Optionally compute the required SPI frequency for the frame rate */
   ret = adas1000_compute_spi_freq(&adas1000_ip, &spi_freq);
   if (ret)
   	goto error;

   adas1000_ip.spi_init.max_speed_hz = spi_freq;

   ret = adas1000_init(&adas1000, &adas1000_ip);
   if (ret)
   	goto error;
