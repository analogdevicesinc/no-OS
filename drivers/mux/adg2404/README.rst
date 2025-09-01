ADG2404/ADG6404 Driver
======================

Supported Devices
-----------------

* `ADG2404 <https://www.analog.com/adg2404>`_
* `ADG6404 <https://www.analog.com/adg6404>`_

Overview
--------

The ADG2404/ADG6404 are low voltage, CMOS analog multiplexers. This driver provides
control over the multiplexer channel selection and enable/disable functionality
for both device variants.

Both devices feature:

* 4:1 single-ended multiplexer
* 0.62Ω on-resistance
* Fast switching times
* Break-before-make switching
* Identical control interface (A0, A1, EN)

**ADG2404 specifications:**
* ±16.5V dual supply or +16.5V single supply
* 1.8V/3.3V/5V logic compatible

**ADG6404 specifications:**
* ±20V dual supply or +36V single supply
* Higher voltage capability for demanding applications

Hardware Configuration
----------------------

The driver requires three GPIO connections:

* **A0**: Address bit 0 for channel selection
* **A1**: Address bit 1 for channel selection  
* **EN**: Enable/disable control (active high)

Channel Selection
-----------------

The multiplexer supports 4 channels (S1-S4) plus an "all off" state using the following enum:

* ``ADG2404_CH_OFF``: All channels off (disables the multiplexer)
* ``ADG2404_S1``: Channel S1 selected (A1=0, A0=0)
* ``ADG2404_S2``: Channel S2 selected (A1=0, A0=1)
* ``ADG2404_S3``: Channel S3 selected (A1=1, A0=0)
* ``ADG2404_S4``: Channel S4 selected (A1=1, A0=1)

API Functions
-------------

**Initialization**
  ``int adg2404_init(struct adg2404_dev **device, struct adg2404_init_param *init_param)``
  
  Initialize the device and configure GPIO pins.

**Channel Selection**
  ``int adg2404_select_channel(struct adg2404_dev *dev, enum adg2404_channel channel)``
  
  Select one of the multiplexer channels or turn all channels off. Use ``ADG2404_CH_OFF`` 
  to disable all channels, or ``ADG2404_S1`` to ``ADG2404_S4`` to select a specific channel.

**Enable/Disable**
  ``int adg2404_enable(struct adg2404_dev *dev, bool enable)``
  
  Enable or disable the multiplexer using ``true`` to enable or ``false`` to disable.

**Cleanup**
  ``int adg2404_remove(struct adg2404_dev *dev)``
  
  Free resources allocated during initialization.

Usage Example
-------------

.. code-block:: c

   struct adg2404_dev *dev;
   struct adg2404_init_param init_param = {
       .gpio_a0 = { /* A0 GPIO configuration */ },
       .gpio_a1 = { /* A1 GPIO configuration */ },
       .gpio_en = { /* EN GPIO configuration */ },
   };
   
   // Initialize the device
   ret = adg2404_init(&dev, &init_param);
   if (ret)
       return ret;
   
   // Select channel S2 (automatically enables the multiplexer)
   ret = adg2404_select_channel(dev, ADG2404_S2);
   if (ret)
       goto cleanup;
   
   // Turn off all channels (disables the multiplexer)
   ret = adg2404_select_channel(dev, ADG2404_CH_OFF);
   if (ret)
       goto cleanup;
   
   cleanup:
   adg2404_remove(dev);
