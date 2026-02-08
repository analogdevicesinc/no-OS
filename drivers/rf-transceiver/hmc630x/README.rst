HMC630X no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`HMC6300`
- :adi:`HMC6301`

Overview
--------

The HMC630LP3 and HMC630LP3E are high dynamic range Vector Modulator
RFICs designed for RF predistortion and feed-forward cancellation
circuits, as well as RF beam forming and amplitude/phase correction
circuits. The I and Q ports of the HMC630LP3(E) can be used to
continuously vary the phase and amplitude of RF signals by up to 360
degrees and 40 dB, respectively, supporting a 3 dB modulation bandwidth
of 180 MHz. Key features include continuous phase control of 360°,
continuous gain control of 40 dB, and a low output noise floor of -162
dBm/Hz. These characteristics make the device ideal for applications
such as wireless infrastructure HPA and MCPA error correction and
cellular/3G systems. Its high input IP3 of +34 dBm and compact 16-lead
3x3mm SMT format ensure robust performance in advanced RF applications.

Applications
------------

- Wireless infrastructure HPA and MCPA error correction
- Pre-distortion or feed-forward linearization
- Cellular/3G systems
- Beamforming or RF cancellation circuits

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~~

The HMC630X project initializes the devices with ``hmc630x_init``, using
parameters in ``hmc630x_init_param``, and integrates with the IIO
framework via ``hmc630x_iio_init``.

Configuration
~~~~~~~~~~~~~

Device configurations are set with functions like
``hmc630x_set_enable``, ``hmc630x_set_vco``, ``hmc630x_set_if_attn``,
and ``hmc630x_write_regmap``. These handle enabling the device, VCO
frequencies, and register map configurations. ``_hmc630x_generate_vco``
adjusts internal VCO frequencies. Additional specific configurations are
achieved with ``hmc6301_set_lna_gain``, ``hmc6301_set_bb_attn``,
``hmc6301_set_bb_attn_fine``, and ``hmc6301_set_bb_lpc_hpc``.

Reading
~~~~~~~

Data retrieval functions include ``hmc630x_read``, ``hmc630x_read_row``,
and ``hmc630x_read_regmap``, which access individual parameters, rows,
or the full register map. ``hmc630x_get_if_attn`` and
``hmc630x_get_vco`` provide access to intermediate frequency attenuation
settings and VCO frequencies respectively. For specific gain
measurements, use ``hmc6301_get_lna_gain``, ``hmc6301_get_bb_attn``,
``hmc6301_get_bb_attn_fine``, and ``hmc6301_get_bb_lpc_hpc``.

Writing
~~~~~~~

Writing operations utilize ``hmc630x_write``, ``hmc630x_write_row``, and
``hmc630x_write_regmap`` to alter specific parameters, rows, or the full
register map. Gain settings can be adjusted with
``hmc6300_set_rf_attn``, and FM modes with ``hmc6300_set_fm_en``.

Temperature Control
~~~~~~~~~~~~~~~~~~~

Temperature management is facilitated by ``hmc630x_set_temp_en``,
``hmc630x_get_temp_en``, and ``hmc630x_get_temp``. These functions
enable and monitor the temperature reading capabilities of the device,
crucial for maintaining operational stability.

Special Functions
~~~~~~~~~~~~~~~~~~

``hmc630x_remove`` ensures proper shutdown and resource clean-up.
``hmc630x_type`` provides device-specific insights for function calls.
Functions ``hmc6300_get_fm_en`` and ``hmc630x_get_enable`` provide
status readings for FM and enable settings.

Driver Initialization Example
------------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdio.h>
   #include <inttypes.h>
   #include <math.h>
   #include "no_os_error.h"
   #include "no_os_delay.h"
   #include "iio.h"
   #include "iio_hmc630x.h"
   #include "no_os_util.h"
   #include "no_os_alloc.h"
   #include "hmc630x.h"
   #include "parameters.h"

   // Initialization parameters and structures
   static struct hmc630x_iio_dev *iio_tx;

   struct hmc630x_init_param txip = {
       .type = HMC6300,
       .ref_clk = HMC6300_REF_CLK_75MHz,
       .en = NULL,
       .clk = NULL,
       .data = NULL,
       .scanout = NULL,
       .vco = 0,
       .enabled = true,
       .temp_en = true,
       .if_attn = 0,
       .tx.rf_attn = 0
   };

   struct hmc630x_iio_init_param iio_txip = {
       .ip = &txip,
   };

   // Example main function
   int main() {
       if (hmc630x_iio_init(&iio_tx, &iio_txip) < 0) {
           printf("Failed to initialize HMC630x TX\n");
           return EXIT_FAILURE;
       }

       printf("HMC630x TX initialized successfully\n");

       hmc630x_iio_remove(iio_tx);

       return EXIT_SUCCESS;
   }

IIO Support
-----------

Register Management and Attribute Handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``iio_hmc630x.c`` file provides core register management functions
for the HMC6300 and HMC6301 devices, facilitating interactions through
the IIO subsystem. Key functions include ``_hmc630x_read_register2`` and
``_hmc630x_write_register2``, enabling reading and writing operations on
device registers necessary for managing device parameters. This is
extended by the ``hmc630x_iio_read_attr`` and ``hmc630x_iio_write_attr``
functions, which allow for reading and updating device attributes,
supporting operations like configuration value access and device
settings updates.

Initialization and Removal
~~~~~~~~~~~~~~~~~~~~~~~~~~

Initialization and resource management are handled through
``hmc630x_iio_init`` and ``hmc630x_iio_remove``. These functions manage
memory allocation, device attribute setup based on the transceiver type,
and ensure proper cleanup. This ensures resource stability and system
integrity, with initialization focusing on structuring device attributes
via templates, and cleanup freeing allocated resources.

Temperature and Frequency Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Temperature and frequency handling in the driver is crucial for
maintaining device performance. Functions like ``hmc630x_iio_read_raw``
monitor temperature by exposing data through the IIO interface. For
frequency management, ``hmc630x_iio_read_vco_available`` accesses
available VCO frequencies, allowing for dynamic adjustments to optimize
device operation in real-time RF scenarios.

IIO Device Initialization Example
---------------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdio.h>
   #include <inttypes.h>
   #include <math.h>
   #include "no_os_error.h"
   #include "no_os_delay.h"
   #include "iio.h"
   #include "iio_hmc630x.h"
   #include "no_os_util.h"
   #include "no_os_alloc.h"
   #include "hmc630x.h"
   #include <string.h>

   static int32_t _hmc630x_read_register2(struct hmc630x_iio_dev *iiodev,
                                          uint32_t reg,
                                          uint32_t *readval)
   {
       int ret;
       uint8_t val;
       ret = hmc630x_read_row(iiodev->dev, reg, &val);
       if (ret)
           return ret;

       *readval = val;
       return 0;
   }

   static int32_t _hmc630x_write_register2(struct hmc630x_iio_dev *iiodev,
                                           uint32_t reg,
                                           uint32_t writeval)
   {
       return hmc630x_write_row(iiodev->dev, reg, (uint8_t)writeval);
   }

   static int hmc630x_iio_read_attr(void *device, char *buf,
                                    uint32_t len, const struct iio_ch_info *channel,
                                    intptr_t priv)
   {
       int ret;
       struct hmc630x_iio_dev *iiodev = (struct hmc630x_iio_dev *)device;
       struct hmc630x_dev *d = iiodev->dev;
       int32_t val;
       bool enable;
       uint64_t vco;
       uint8_t attn;
       uint8_t band;
       uint8_t lock;
       enum hmc6301_lna_attn lna_attn;
       enum hmc6301_bb_attn attn1, attn2;
       enum hmc6301_bb_attn_fine attni, attnq;
       enum hmc6301_bb_lpc lpc;
       enum hmc6301_bb_hpc hpc;

       switch (priv) {
       case HMC630X_IIO_ATTR_ENABLED:
           ret = hmc630x_get_enable(d, &enable);
           if (ret)
               return ret;

           val = enable;
           break;
       case HMC630X_IIO_ATTR_VCO:
           ret = hmc630x_get_vco(d, &vco);
           if (ret)
               return ret;

           val = vco / 1000;
           break;
       case HMC630X_IIO_ATTR_VCO_BAND:
           ret = hmc630x_read(d, HMC630X_VCO_BANDSEL, &band);
           if (ret)
               return ret;

           val = band;
           break;
       case HMC630X_IIO_ATTR_VCO_LOCK:
           ret = hmc630x_read(d, HMC630X_LOCKDET, &lock);
           if (ret)
               return ret;

           val = lock;
           break;
       case HMC630X_IIO_ATTR_IF_ATTN:
           ret = hmc630x_get_if_attn(d, &attn);
           if (ret)
               return ret;

           val = attn;
           break;
       case HMC630X_IIO_ATTR_TEMP_EN:
           ret = hmc630x_get_temp_en(d, &enable);
           if (ret)
               return ret;

           val = enable;
           break;
       case HMC6300_IIO_ATTR_RF_ATTN:
           ret = hmc6300_get_rf_attn(d, &attn);
           if (ret)
               return ret;

           val = attn;
           break;
       case HMC6301_IIO_ATTR_RF_LNA_GAIN:
           ret = hmc6301_get_lna_gain(d, &lna_attn);
           if (ret)
               return ret;

           val = lna_attn;
           break;
       case HMC6301_IIO_ATTR_BB_ATTN1:
       case HMC6301_IIO_ATTR_BB_ATTN2:
           ret = hmc6301_get_bb_attn(d, &attn1, &attn2);
           if (ret)
               return ret;

           if (priv == HMC6301_IIO_ATTR_BB_ATTN1)
               val = attn1;
           else
               val = attn2;
           break;
       case HMC6301_IIO_ATTR_BB_ATTNI_FINE:
       case HMC6301_IIO_ATTR_BB_ATTNQ_FINE:
           ret = hmc6301_get_bb_attn_fine(d, &attni, &attnq);
           if (ret)
               return ret;

           if (priv == HMC6301_IIO_ATTR_BB_ATTNI_FINE)
               val = attni;
           else
               val = attnq;
           break;
       case HMC6301_IIO_ATTR_BB_LPC:
       case HMC6301_IIO_ATTR_BB_HPC:
           ret = hmc6301_get_bb_lpc_hpc(d, &lpc, &hpc);
           if (ret)
               return ret;

           if (priv == HMC6301_IIO_ATTR_BB_LPC)
               val = lpc;
           else
               val = hpc;
           break;
       default:
           return -EINVAL;
       }

       return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
   }

   int main() {
       // Example code setup for main function initialization
       // Assuming all necessary device initialization is handled properly
       struct hmc630x_iio_dev *iiodev;
       struct hmc630x_iio_init_param init_param;

       // Assign the init_param with suitable parameters

       if (hmc630x_iio_init(&iiodev, &init_param) < 0) {
           printf("Failed to initialize HMC630x IIO device\n");
           return EXIT_FAILURE;
       }

       printf("HMC630x IIO device initialized successfully\n");

       // Other operations with iiodev...

       hmc630x_iio_remove(iiodev);

       return EXIT_SUCCESS;
   }
