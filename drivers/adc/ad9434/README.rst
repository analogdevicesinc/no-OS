AD9434 no-OS Driver
===================

.. no-os-doxygen::

Supported Devices
-----------------

* `AD9434 <https://www.analog.com/AD9434>`_

Overview
--------

The AD9434 is a 12-bit, 370 MSPS/500 MSPS analog-to-digital converter (ADC)
designed for applications demanding high dynamic range and wide input bandwidth.
Operating from a 1.8 V supply, it achieves an SNR of 65 dBFS and SFDR of
78 dBc at input frequencies up to 250 MHz at 500 MSPS. The device integrates
a sample-and-hold circuit, an input buffer, and a voltage reference on-chip,
requiring no external decoupling components. It features a 1 GHz full power
analog bandwidth and a programmable input voltage range from 1.18 V p-p to
1.6 V p-p. Digital outputs are LVDS-compatible, supporting offset binary,
twos complement, or Gray code formats. The AD9434 also includes a clock duty
cycle stabilizer and an integrated data clock output with programmable clock
and data alignment. Available in a 56-lead LFCSP, it is rated for the
industrial temperature range of -40°C to +85°C.

Applications
------------

* Wireless and wired broadband communications
* Cable reverse path
* Communications test equipment
* Radar and satellite subsystems
* Power amplifier linearization

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

The ``ad9434_setup`` function initializes the AD9434 device. It allocates
memory for an ``ad9434_dev`` structure, initializes SPI communication using
the settings provided in the ``ad9434_init_param`` structure, and verifies
the chip ID by reading the ``AD9434_REG_CHIP_ID`` register. If the ID does
not match the expected ``AD9434_CHIP_ID``, the function returns an error.
Upon successful verification, the default output mode is configured using
``ad9434_outputmode_set``.

SPI Communication
~~~~~~~~~~~~~~~~~

The driver communicates with the AD9434 over SPI using ``ad9434_spi_read``
and ``ad9434_spi_write``. These functions handle low-level reading and
writing of the ADC registers, constructing 3-byte SPI transactions with
the register address and data.

Output Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

The output data format can be configured using ``ad9434_outputmode_set``,
which writes the desired mode to the output mode register and triggers a
transfer synchronization. Supported output formats include offset binary
(``OUTPUT_MODE_OFFSET_BINARY``), twos complement
(``OUTPUT_MODE_TWOS_COMPLEMENT``), and Gray code
(``OUTPUT_MODE_GRAY_CODE``).

Test Mode Configuration
~~~~~~~~~~~~~~~~~~~~~~~

The ``ad9434_testmode_set`` function configures the ADC into various test
modes for verification and debugging. Available test modes include midscale
short (``TESTMODE_MIDSCALE_SHORT``), positive full-scale
(``TESTMODE_POS_FULLSCALE``), negative full-scale
(``TESTMODE_NEG_FULLSCALE``), alternating checkerboard
(``TESTMODE_ALT_CHECKERBOARD``), PN23 sequence (``TESTMODE_PN23_SEQ``),
PN9 sequence (``TESTMODE_PN9_SEQ``), one-zero toggle
(``TESTMODE_ONE_ZERO_TOGGLE``), and user-defined
(``TESTMODE_USER_DEFINED``).

Data Delay Adjustment
~~~~~~~~~~~~~~~~~~~~~

The ``ad9434_data_delay`` function adjusts the output data delay by writing
to the ``AD9434_REG_OUTPUT_DELAY`` register and triggering a transfer
synchronization. This enables fine-tuning of data timing to synchronize
with other system components.

Driver Initialization Example
-----------------------------

.. code-block:: c

   #include "ad9434.h"
   #include "no_os_spi.h"

   struct ad9434_dev *device;
   int32_t ret;

   struct ad9434_init_param init_param = {
       .spi_init = {
           .device_id    = SPI_DEVICE_ID,
           .max_speed_hz = 10000000,
           .chip_select  = SPI_CS,
           .mode         = NO_OS_SPI_MODE_0,
           .platform_ops = SPI_OPS,
           .extra        = SPI_EXTRA,
       }
   };

   ret = ad9434_setup(&device, init_param);
   if (ret)
       goto error;
