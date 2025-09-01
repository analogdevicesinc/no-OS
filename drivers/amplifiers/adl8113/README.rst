ADL8113 Driver
==============

Overview
--------

The ADL8113 is a 10MHz to 12GHz Low Noise Amplifier with integrated bypass
switches. This driver provides digital control over the amplifier mode
selection and enable/disable functionality.

The device features:

* Low noise amplifier: 10MHz to 12GHz operation
* Bypass switches: 10MHz to 14GHz operation
* Noise figure: 3.8dB
* Small signal gain: 14.0dB
* Output 3rd order intercept point (OIP3): 35.5dBm
* Digital control interface via GPIO pins

Digital Control Interface
-------------------------

The ADL8113 uses two GPIO control pins:

* **VA**: Mode control pin A
* **VB**: Mode control pin B

Operation Modes
---------------

The ADL8113 supports four operation modes controlled by VA and VB pins:

* **Internal Amplifier**: VA=0, VB=0 - Signal passes through the internal low noise amplifier
* **Internal Bypass**: VA=1, VB=1 - Signal bypasses through internal bypass path
* **External Bypass A**: VA=0, VB=1 - Signal routes through external bypass path A
* **External Bypass B**: VA=1, VB=0 - Signal routes through external bypass path B

API Functions
-------------

**Initialization**
  ``int adl8113_init(struct adl8113_dev **device, struct adl8113_init_param *init_param)``
  
  Initialize the device and configure GPIO pins. Set initial mode as
  specified in init parameters.

**Mode Control**
  ``int adl8113_set_mode(struct adl8113_dev *dev, enum adl8113_mode mode)``
  
  Set operation mode to ADL8113_INTERNAL_AMPLIFIER, ADL8113_INTERNAL_BYPASS, 
  ADL8113_EXTERNAL_BYPASS_A, or ADL8113_EXTERNAL_BYPASS_B.

  ``int adl8113_get_mode(struct adl8113_dev *dev, enum adl8113_mode *mode)``
  
  Get current operation mode.

**Cleanup**
  ``int adl8113_remove(struct adl8113_dev *dev)``
  
  Free resources allocated during initialization.

Usage Example
-------------

.. code-block:: c

   struct adl8113_dev *dev;
   struct adl8113_init_param init_param = {
       .gpio_va = { /* VA GPIO configuration */ },
       .gpio_vb = { /* VB GPIO configuration */ },
       .initial_mode = ADL8113_INTERNAL_AMPLIFIER
   };
   
   // Initialize the device
   ret = adl8113_init(&dev, &init_param);
   if (ret)
       return ret;
   
   // Switch to external bypass A mode
   ret = adl8113_set_mode(dev, ADL8113_EXTERNAL_BYPASS_A);
   if (ret)
       goto cleanup;
   
   cleanup:
   adl8113_remove(dev);

Dependencies
------------

* no_os_gpio.h
* no_os_error.h
* no_os_alloc.h