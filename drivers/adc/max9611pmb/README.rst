MAX9611 no-OS driver
=====================

Supported Devices
-----------------

- :adi:`MAX9611`
- :adi:`MAX9612`

Overview
---------

The MAX9611 and MAX9612 are high-side current-sense amplifiers featuring
a 12-bit ADC and an op-amp/comparator, ideal for industrial and
automotive applications. They support a wide input common-mode voltage
range from 0V to 60V and a power supply range from 2.7V to 5.5V. These
components allow for programmable full-scale voltage, enhancing
measurement precision and sense resistor flexibility. An I2C interface
compatible with 1.8V and 3.3V logic levels ensures seamless integration
with modern microcontrollers. The MAX9611 provides a noninverting
input-output configuration, while the MAX9612 offers an inverting
configuration. Both are suitable for automotive temperature ranges of
-40°C to +125°C and come in a 3mm x 5mm, 10-pin µMAX® package. Their
applications include hybrid automotive power supplies, server
backplanes, and telecom cards.

Applications
-------------

- Hybrid Automotive Power Supplies
- Server Backplanes
- Base-Station PA Control
- Base-Station Feeder Cable Bias-T
- Telecom Cards
- Battery-Operated Equipment

Operation Modes
----------------

+--------------------------+-----------------+-----------------+-----------------+
| Mode Name                | Description     | Configuration   | Typical Use     |
|                          |                 | Bits            | Case            |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_NORMAL_MODE      | Normal          | 0x0             | Used for        |
|                          | operating mode  |                 | standard        |
|                          | for MAX9611     |                 | operations of   |
|                          |                 |                 | the device      |
|                          |                 |                 | without         |
|                          |                 |                 | additional      |
|                          |                 |                 | processing      |
|                          |                 |                 | modes.          |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_OPAMP_MODE       | Configures the  | 0x3             | When additional |
|                          | device to       |                 | signal          |
|                          | operate as an   |                 | amplification   |
|                          | operational     |                 | is required in  |
|                          | amplifier       |                 | the current     |
|                          |                 |                 | sensing path.   |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_COMPARATOR_MODE  | Sets device to  | 0x7             | Applications    |
|                          | function as a   |                 | needing voltage |
|                          | comparator,     |                 | threshold       |
|                          | including latch |                 | detection and   |
|                          | behaviors       |                 | high-speed      |
|                          |                 |                 | signal          |
|                          |                 |                 | comparison      |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_SENSE_1X    | MUX             | Uses MUX bits   | Basic current   |
|                          | configuration   |                 | sensing         |
|                          | for 1x sensing  |                 | operation with  |
|                          | gain            |                 | standard gain   |
|                          |                 |                 | in typical      |
|                          |                 |                 | applications.   |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_SENSE_4X    | MUX             | Uses MUX bits   | Enhance         |
|                          | configuration   |                 | sensitivity for |
|                          | for 4x sensing  |                 | precise current |
|                          | gain            |                 | measurement     |
|                          |                 |                 | requiring       |
|                          |                 |                 | moderate gain.  |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_SENSE_8X    | MUX             | Uses MUX bits   | High gain       |
|                          | configuration   |                 | scenario for    |
|                          | for 8x sensing  |                 | weak signal     |
|                          | gain            |                 | amplification   |
|                          |                 |                 | in sensitive    |
|                          |                 |                 | current paths.  |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_IN_COM_MODE | Configures for  | Uses MUX bits   | Situations      |
|                          | input common    |                 | requiring       |
|                          | mode            |                 | common-mode     |
|                          |                 |                 | signal          |
|                          |                 |                 | rejection.      |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_OUT_ADC     | Configures to   | Uses MUX bits   | Applications    |
|                          | output the ADC  |                 | focusing on     |
|                          | value           |                 | direct ADC      |
|                          |                 |                 | output values   |
|                          |                 |                 | for further     |
|                          |                 |                 | processing.     |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_SET_ADC     | Sets ADC        | Uses MUX bits   | Setting ADC     |
|                          | configuration   |                 | parameters for  |
|                          |                 |                 | particular      |
|                          |                 |                 | operational     |
|                          |                 |                 | needs.          |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_CONF_TEMP        | Temperature     | Uses MUX bits   | Applications    |
|                          | sensing         |                 | need            |
|                          | configuration   |                 | temperature     |
|                          |                 |                 | monitoring      |
|                          |                 |                 | alongside       |
|                          |                 |                 | current         |
|                          |                 |                 | sensing.        |
+--------------------------+-----------------+-----------------+-----------------+
| MAX9611_FAST_MODE        | Fast data       | Uses MUX bits   | Critical        |
|                          | capturing and   |                 | high-speed data |
|                          | processing      |                 | acquisition     |
|                          | configuration   |                 | scenarios where |
|                          |                 |                 | time is of      |
|                          |                 |                 | essence.        |
+--------------------------+-----------------+-----------------+-----------------+

Device Configuration
---------------------

Initialization and Addressing
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In MAX9611PMB I2C communication, a bus master initiates communication
with a slave device by issuing a START condition followed by a slave
address. The MAX9611/MAX9612 wait for this condition and, upon
recognizing their address, are ready to exchange data, offering 16
different slave addresses using two address inputs, A1 and A0, with the
least significant bit determining the read/write mode. After receiving
the address, the MAX9611/MAX9612 acknowledge by pulling SDA low for one
clock cycle. The ``max9611_init`` function initializes
a ``max9611_dev``device by allocating memory, setting up the I2C interface,
and handling errors. It starts by allocating memory for the device
structure and returns an error if the allocation fails. The I2C
interface is then initialized, and if this fails, the allocated memory
is freed, and the error is returned. If successful, the device structure
is assigned to the provided pointer, and the function returns 0. The
MAX9611 driver uses functions like ``max9611_addr_gen`` to generate the
I2C slave address, ``max9611_init`` to establish communication and
allocate resources, and ``max9611_remove`` to deallocate resources and clean
up.

Data Operations
~~~~~~~~~~~~~~~

To manipulate device registers, the driver offers various functions.
``max9611_read`` reads from a specified register, ensuring compliance with
valid boundaries, while ``max9611_write`` writes data to registers,
preventing modifications to read-only areas. ``max9611_reg_update``
enables selective update of register contents using a mask and new
values.

Mode Control
~~~~~~~~~~~~

Operational mode configuration is performed with ``max9611_set_mode`` and
``max9611_get_mode``, which set and retrieve the device mode, supporting
normal, operational amplifier, and comparator modes. ``max9611_set_mux``
and ``max9611_get_mux`` configure and query internal multiplexer settings
for adaptable data capture configurations. ``max9611_set_lr`` configures
the latch/retry (LR) bit in the MAX9611 control register to manage
operational modes. By setting the boolean ‘is_normal’ to true, the
function sets the LR bit to 0, enabling normal operation. Otherwise, it
configures for an alternative mode as detailed in the device datasheet
while the ``max9611_get_lr`` function reads the LR bit state from the
MAX9611’s control register, storing the value in a boolean pointer. This
allows verification of the current operational mode of the device,
ensuring correct mode settings for intended applications.

Delay and Retry
~~~~~~~~~~~~~~~

Timing control is essential, and the driver manages this through
``max9611_set_delay`` and ``max9611_get_delay`` for delay settings, and
``max9611_set_retry`` and ``max9611_get_retry`` for retry intervals,
ensuring robust I2C communication by allowing operation retries within
specific intervals.

Raw Data Access
~~~~~~~~~~~~~~~

Access to raw ADC data is provided by ``max9611_get_raw``, which retrieves
12-bit data according to the device’s current configuration and capture
mode. This function supports applications requiring precise ADC readings
and in-depth current measurement necessary for high-side current-sensing
tasks.

Shutdown
~~~~~~~~

``max9611_shutdown`` enables toggling the device’s power state, crucial
for controlling power consumption, by updating control registers based
on a given parameter.

Driver Initialization Example
-----------------------------

.. code-block:: C

   int ret;

   uint8_t mux_conf[] = {
       MAX9611_CONF_SENSE_1X,
       MAX9611_CONF_SENSE_4X,
       MAX9611_CONF_SENSE_8X,
       MAX9611_CONF_IN_COM_MODE,
       MAX9611_CONF_OUT_ADC,
       MAX9611_CONF_SET_ADC,
       MAX9611_CONF_TEMP,
       MAX9611_FAST_MODE
   };

   uint8_t mode_conf[] = {
       MAX9611_NORMAL_MODE,
       MAX9611_OPAMP_MODE,
       MAX9611_COMPARATOR_MODE
   };

   uint8_t delays[] = {
       MAX9611_1MS,
       MAX9611_100US
   };

   uint8_t retries[] = {
       MAX9611_50MS,
       MAX9611_10MS
   };

   struct max_i2c_init_param max9611_extra = {
       .vssel = MXC_GPIO_VSSEL_VDDIOH
   };

   struct max9611_init_param max9611_ip = {
       .i2c_init.device_id     = 0,
       .i2c_init.max_speed_hz  = MAX_I2C_STD_MODE,
       .i2c_init.extra         = &max9611_extra,
       .i2c_init.platform_ops  = &max_i2c_ops
   };

   ret = max9611_addr_gen(&max9611_ip, MAX9611_ZERO_VCC, MAX9611_ZERO_VCC);
   if (ret)
       return ret;

   struct max9611_dev *max9611_device;
   ret = max9611_init(&max9611_device, max9611_ip);
   if (ret)
       return ret;

   while (1) {
       uint16_t raw_val;
       int mux_cnt = 0, mode_cnt = 0, dcnt = 0, rcnt = 0;

       for (int i = 0; i < 8; i++) {
           enum max9611_mux_conf mux, mux_new;

           ret = max9611_get_mux(max9611_device, &mux);
           if (ret)
               return ret;

           ret = max9611_set_mux(max9611_device, mux_conf[i]);
           if (ret)
               return ret;

           ret = max9611_get_mux(max9611_device, &mux_new);
           if (ret)
               return ret;

           ret = max9611_get_raw(max9611_device, &raw_val);
           if (ret)
               return ret;

           if (mux_conf[i] != mux_new)
               mux_cnt++;
       }

       for (int i = 0; i < 3; i++) {
           enum max9611_mode_conf mode, mode_new;

           ret = max9611_get_mode(max9611_device, &mode);
           if (ret)
               return ret;

           ret = max9611_set_mode(max9611_device, mode_conf[i]);
           if (ret)
               return ret;

           ret = max9611_get_mode(max9611_device, &mode_new);
           if (ret)
               return ret;

           if (mode_conf[i] != mode_new)
               mode_cnt++;
       }

       for (int i = 0; i < 2; i++) {
           enum max9611_delay_time dtime, dtime_new;

           ret = max9611_get_delay(max9611_device, &dtime);
           if (ret)
               return ret;

           ret = max9611_set_delay(max9611_device, delays[i]);
           if (ret)
               return ret;

           ret = max9611_get_delay(max9611_device, &dtime_new);
           if (ret)
               return ret;

           if (delays[i] != dtime_new)
               dcnt++;
       }

       for (int i = 0; i < 2; i++) {
           enum max9611_retry_time rtime, rtime_new;

           ret = max9611_get_retry(max9611_device, &rtime);
           if (ret)
               return ret;

           ret = max9611_set_retry(max9611_device, retries[i]);
           if (ret)
               return ret;

           ret = max9611_get_retry(max9611_device, &rtime_new);
           if (ret)
               return ret;

           if (retries[i] != rtime_new)
               rcnt++;
       }

       ret = max9611_shutdown(max9611_device, true);
       if (ret)
           return ret;

       ret = max9611_shutdown(max9611_device, false);
       if (ret)
           return ret;
   }
