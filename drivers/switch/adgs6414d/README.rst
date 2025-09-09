ADGS6414D Driver
================

Overview
--------

The ADGS6414D is a high-density octal SPST switch with ultra-low on-resistance
and SPI control interface. This driver provides control over the eight
independent switches via SPI communication.

The device features:

* 8 independent SPST switches
* 0.56Ω typical on-resistance
* SPI control interface with robust error detection
* 4mm × 5mm, 30-terminal LGA package
* CRC error detection and SCLK count error detection
* Daisy-chain capability for multiple devices
* Pin-for-pin replacement for ADGS1414D and ADGS2414D

Control Interface
-----------------

The ADGS6414D uses SPI (Serial Peripheral Interface) for digital control:

* **SPI Interface**: Standard 4-wire SPI communication
* **Switch Control**: Each of the 8 switches can be controlled independently
* **Bitmask Control**: All switches can be set simultaneously using an 8-bit mask
* **Daisy Chain**: Multiple devices can be daisy-chained for high channel count systems

Switch Channels
---------------

The ADGS6414D provides 8 independent switch channels:

* **SW1**: Channel 0 (bit 0 in control byte)
* **SW2**: Channel 1 (bit 1 in control byte)
* **SW3**: Channel 2 (bit 2 in control byte)
* **SW4**: Channel 3 (bit 3 in control byte)
* **SW5**: Channel 4 (bit 4 in control byte)
* **SW6**: Channel 5 (bit 5 in control byte)
* **SW7**: Channel 6 (bit 6 in control byte)
* **SW8**: Channel 7 (bit 7 in control byte)

API Functions
-------------

**Initialization**
  ``int adgs6414d_init(struct adgs6414d_dev **device, struct adgs6414d_init_param *init_param)``
  
  Initialize the device and configure SPI interface. Set initial switch states
  as specified in init parameters.

**Individual Switch Control**
  ``int adgs6414d_set_switch(struct adgs6414d_dev *dev, uint8_t channel, bool state)``
  
  Set state of a specific switch channel (0-7). State: true = closed, false = open.

  ``int adgs6414d_get_switch(struct adgs6414d_dev *dev, uint8_t channel, bool *state)``
  
  Get current state of a specific switch channel.

**Multiple Switch Control**
  ``int adgs6414d_set_switches(struct adgs6414d_dev *dev, uint8_t mask)``
  
  Set all switch states using an 8-bit bitmask.

  ``int adgs6414d_get_switches(struct adgs6414d_dev *dev, uint8_t *mask)``
  
  Get all switch states as an 8-bit bitmask.

**Device Reset**
  ``int adgs6414d_soft_reset(struct adgs6414d_dev *dev)``
  
  Perform software reset by writing the sequence 0xA3 followed by 0x05 
  to the SOFT_RESETB register.

**Cleanup**
  ``int adgs6414d_remove(struct adgs6414d_dev *dev)``
  
  Free resources allocated during initialization.

Usage Example
-------------

.. code-block:: c

   struct adgs6414d_dev *dev;
   struct adgs6414d_init_param init_param = {
       .spi_init = &spi_init_param,
       .initial_state = 0x00  // All switches open initially
   };
   
   // Initialize the device
   ret = adgs6414d_init(&dev, &init_param);
   if (ret)
       return ret;
   
   // Perform soft reset
   ret = adgs6414d_soft_reset(dev);
   if (ret)
       goto cleanup;
   
   // Close switch 1 and switch 3
   ret = adgs6414d_set_switch(dev, ADGS6414D_SW1, true);
   if (ret)
       goto cleanup;
   
   ret = adgs6414d_set_switch(dev, ADGS6414D_SW3, true);
   if (ret)
       goto cleanup;
   
   // Set all switches using bitmask (close SW1, SW2, SW5)
   ret = adgs6414d_set_switches(dev, 0x13); // Binary: 00010011
   if (ret)
       goto cleanup;
   
   // Read switch states
   uint8_t switch_mask;
   ret = adgs6414d_get_switches(dev, &switch_mask);
   if (ret)
       goto cleanup;
   
   cleanup:
   adgs6414d_remove(dev);

Dependencies
------------

* no_os_spi.h
* no_os_error.h
* no_os_alloc.h
