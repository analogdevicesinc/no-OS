ADF4371 no-OS Driver
====================

.. no-os-doxygen::

Supported Devices
-----------------

- :adi:`ADF4371`
- :adi:`ADF4372`

Overview
--------

The ADF4371 features an RF output frequency range of 62.5 MHz to 32,000
MHz. The ADF4371 includes a fractional-N synthesizer and an integer N
synthesizer with a high resolution 39-bit fractional modulus. The device
implements fractional-N or integer N phase-locked loop (PLL) frequency
synthesizers with an external loop filter and an external reference
frequency. The wideband microwave voltage-controlled oscillator (VCO)
design generates frequencies from 62.5 MHz to 32,000 MHz. The ADF4371
has an integrated VCO with a fundamental output frequency ranging from
4000 MHz to 8000 MHz and connects to divide-by circuits that permit
generation of RF output frequencies as low as 62.5 MHz.

Applications
-------------

- Wireless infrastructure (multicarrier global system for mobile
  communication, 5G) power-down modes
- Test equipment and instrumentation
- Clock generation
- Aerospace and defense

Operation Modes
---------------

+------------------+--------------------------+--------------------------+-----------------------------+
| Mode Name        | Description              | Configuration Bits       | Typical Use                 |
+==================+==========================+==========================+=============================+
| Tristate         | High impedance output    | ADF4371_MUXOUT_TRISTATE  | Disable output when not used|
+------------------+--------------------------+--------------------------+-----------------------------+
| Digital Lock     | Digital lock detect      | ADF4371_MUXOUT_DIG_LOCK  | Monitor PLL lock status     |
+------------------+--------------------------+--------------------------+-----------------------------+
| Charge Pump Up   | Charge pump up signal    | ADF4371_MUXOUT_CH_PUMP_UP| Upward charge pump operation|
+------------------+--------------------------+--------------------------+-----------------------------+
| Charge Pump Down | Charge pump down signal  | ADF4371_MUXOUT_CH        | Downward charge pump        |
|                  |                          | _PUMP_DOWN               | operation                   |
+------------------+--------------------------+--------------------------+-----------------------------+
| RDIV2            | Reference clock ÷2       | ADF4371_MUXOUT_RDIV2     | Divided reference output    |
+------------------+--------------------------+--------------------------+-----------------------------+
| N Divider Output | N divider signal         | ADF4371_MUXOUT_N_DIV_OUT | Debug PLL divider settings  |
+------------------+--------------------------+--------------------------+-----------------------------+
| VCO Test         | VCO test mode            | ADF4371_MUXOUT_VCO_TEST  | Test VCO performance        |
+------------------+--------------------------+--------------------------+-----------------------------+
| High             | Output forced high       | ADF4371_MUXOUT_HIGH      | Override for test           |
+------------------+--------------------------+--------------------------+-----------------------------+
| VCO Cal R Band   | VCO cal for R band       | ADF4371_MUXOUT_VCO       | Calibrate VCO R band        |
|                  |                          | _CALIB_R_B               |                             |
+------------------+--------------------------+--------------------------+-----------------------------+
| VCO Cal N Band   | VCO cal for N band       | ADF4371_MUXOUT           | Calibrate VCO N band        |
|                  |                          |  _VCO_CALIB_N_BAND       |                             |
+------------------+--------------------------+--------------------------+-----------------------------+

Device Configuration
--------------------

Initialization
~~~~~~~~~~~~~~

The function ``adf4371_init()`` configures the device with given
parameters and allocates the necessary resources. The function
``adf4371_remove()`` releases these resources and removes the device
instance from the system.

SPI Communication Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The SPI communication functions for the ADF4371 device serve to enable
efficient data transactions between the device and a controlling system.
The ``adf4371_write`` function allows writing an 8-bit value to a
specified 16-bit register by building a command from the register
address and write operation, which is then transmitted through a 3-byte
buffer over SPI. It confirms success with a return code of 0, or an
appropriate negative error code upon failure.

The ``adf4371_write_bulk`` function extends this capability by
facilitating the writing of multiple bytes from an array to a device
register, using a buffer that incorporates the write command, the
register address, and the data. Like ``adf4371_write``, it returns 0
upon success, with negative codes indicating errors. The
``adf4371_read`` function, on the other hand, reads an 8-bit value from
a defined 16-bit register by issuing a read command and storing the
result from the SPI response.

Register and Frequency Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``adf4371_update`` function operates by reading a register’s current
value, applying a bitmask to clear certain bits, and then writing back
the modified value to update specific bits of the register. It returns 0
to indicate a successful operation or a negative error code if any step
fails. This function is crucial for ensuring precise control over the
ADF4371 device’s configuration and is often employed in modifying
registers to ensure they adhere to desired specifications.

The ``adf4371_pll_fract_n_compute`` function calculates the PLL (Phase
Locked Loop) parameters such as integer and fractional division
components for the VCO (Voltage Controlled Oscillator) based on its
frequency and the PFD (Phase Frequency Detector) frequency.

In contrast, ``adf4371_pll_fract_n_get_rate`` computes the output
frequency for a specific channel, returning it as a 64-bit integer.
Finally, ``adf4371_set_freq`` sets a channel’s frequency by validating
the frequency, adjusting dividers, computing the necessary PLL
parameters, and updating device registers accordingly, returning 0 for
success or a negative error upon failure.

Channel Control
~~~~~~~~~~~~~~~

The ``adf4371_channel_power_down`` function modifies the power state of
a specified channel in the ADF4371 device by altering the register bit
corresponding to the power-down state, effectively enabling or disabling
the channel power as needed. It takes a device pointer, channel number,
and a boolean to indicate power state, and returns 0 on success or a
negative error code upon failure.

Meanwhile, ``adf4371_channel_config`` manages the frequency
configuration and power state of channels, ensuring each channel’s
frequency matches its desired setting. It iterates over channels, sets
frequencies if needed, and utilizes ``adf4371_channel_power_down`` to
power up channels, reporting 0 on success and a negative code if an
error occurs during operations.

Device Setup and Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The setup and operation of the ADF4371 involve several key steps
primarily facilitated through SPI communication. Initially, the SPI
interface is configured by writing specific values to registers, namely
REG0000 and REG0001, to establish a 4-wire SPI operation. This is the
foundation for subsequent initialization and operation.

The initialization sequence requires writing to registers in reverse
order, from address 0x7C to address 0x10, to prepare the device for
frequency updates. Following initialization, the frequency update
sequence involves modifying the R, MOD2, FRAC1, FRAC2, and INT
registers, ensuring accurate phase-frequency detector operation. The
ADF4371’s channel control manages the power state and frequency settings
for each channel.

Clock Control
~~~~~~~~~~~~~

The ``adf4371_clk_recalc_rate`` function recalculates and outputs the
current frequency rate for a specified channel on the ADF4371 device. It
checks if the specified channel number is valid, invokes
``adf4371_pll_fract_n_get_rate`` to obtain the recalculated rate, and
stores it in the provided location. It returns 0 on success or -1 if the
channel is invalid.

The ``adf4371_clk_round_rate`` function sets the desired frequency to
the closest achievable rate. It directly assigns the input rate to the
location pointed to by ``rounded_rate`` and consistently returns 0,
indicating success. Meanwhile, ``adf4371_clk_set_rate`` aims to adjust
the output frequency for a specified channel. It checks for valid
channel inputs before calling ``adf4371_set_freq`` to set the new
frequency, returning the outcome of this operation, which is 0 for
success and -1 if the channel is invalid.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "no_os_spi.h" 
   #include "xilinx_spi.h" 
   #include "adf4371.h" 
   #include "parameters.h" 
    

   #define MULTIDEVICE_INSTANCE_COUNT  1 
   #define ADF4371_CS                  0 
    

   /* ADF4371 channel specifications */ 
   struct adf4371_chan_spec adf_chan_spec[1] = { 
       { 
           .num = 2, 
           .power_up_frequency = 12000000000, 
       } 
   }; 
    

   /* SPI initialization parameters */ 
   struct no_os_spi_init_param clkchip_spi_init_param = { 
       .device_id = 1, 
       .max_speed_hz = 1000000, 
       .mode = NO_OS_SPI_MODE_0, 
       .chip_select = ADF4371_CS, 
       .platform_ops = &xil_spi_ops, 
       .extra = NULL 
   }; 
    

   /* ADF4371 initialization parameters */ 
   struct adf4371_init_param adf4371_param = { 
       .spi_init = &clkchip_spi_init_param, 
       .spi_3wire_enable = true, 
       .clkin_frequency = 50000000, 
       .muxout_select = 1, 
       .num_channels = 1, 
       .channels = adf_chan_spec 
   }; 
    

   int main(void) { 
       int32_t ret; 
       struct adf4371_dev* adf4371_dev[MULTIDEVICE_INSTANCE_COUNT]; 
       struct no_os_clk_hw adf4371_hw[MULTIDEVICE_INSTANCE_COUNT]; 
    

   for (int i = 0; i < MULTIDEVICE_INSTANCE_COUNT; i++) { 
           ret = adf4371_init(&adf4371_dev[i], &adf4371_param); 
           if (ret) { 
               return ret; // Initialization failed 
           } 
           // Additional configuration or operations with adf4371_dev[i] can be performed here 
       } 
    

   return 0; // Successful execution 
   }
