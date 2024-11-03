====================
AD5293 no-OS driver
====================

Supported Devices
------------------

`ad5293 <https://www.analog.com/en/products/ad5293.html>`_

Overview
---------
The AD5293 is a single-channel, 1024-position digital potentiometer with 
a <1% end-to-end resistor tolerance error. The AD5293 performs the same 
electronic adjustment function as a mechanical potentiometer with enhanced 
resolution, solid state reliability, and superior low temperature coefficient 
performance. This device is capable of operating at high voltages and supporting 
both dual-supply operation at ±10.5 V to ±15 V and single-supply operation at 21 V to 30 V.
(in this documentation, the terms digital potentiometer and RDAC are used interchangeably)

The AD5293 contains a serial interface (SYNC, SCLK, DIN, and
SDO) that is compatible with SPI standards, as well as most DSPs.
The device allows data to be written to every register via the SPI.

Applications
------------
- Mechanical potentiometer replacement
- Instrumentation: gain and offset adjustment
- Programmable voltage-to-current conversion
- Programmable filters, delays, and time constants
- Programmable power supply
- Low resolution DAC replacements
- Sensor calibration

Device Configuration
--------------------
Single Chips Use
^^^^^^^^^^^^^^^^^^
Single chip use is compatible for normal SPI device hardware topology. 
For this configuration set chip num to 1 in initilazation struct.

Multiple Chips Use
^^^^^^^^^^^^^^^^^^
Multiple chips could be used in application, daisy chaining topology can 
minimizes the number of port pins required from the controlling IC. For 
this driver, if more than 1 chip are used, daisy-chain hardware topology 
is required. All RESET pins are required to connect together. For this 
configuration set chip num greater than 1 in initilazation struct.

Device Operation
----------------

Write Operation
^^^^^^^^^^^^^^^
In order to write to the RDAC calibration mode, you will need to
call the **ad5293_update_cali** to update calibration on specific 
chip struct, then call the **ad5293_write_cali** to perform a write performance.

In order to write to the RDAC wiper value, you will need to
call the **ad5293_update_wiper** to update wiper value on specific 
chip struct, then call the **ad5293_write_cali** to perform a write performance.
**RDAC register write protect register** is only unlocked in **ad5293_write_cali**
to pervent undesired wiper write.

Each write perform will write all chips, for multiple chips application, 
update all relevant data before perform write.

Read Operation
^^^^^^^^^^^^^^
In order to read the RDAC calibration mode, you will need to
call the **ad5293_read_reg_cali**, calibration mode will be updated 
to chip struct.

In order to read the RDAC wiper value, you will need to
call the **ad5293_read_reg_wiper**, wiper value will be updated 
to chip struct.

Reset
^^^^^
In order to read the RDAC chips, you will need to
call the **ad5293_hard_reset** to implement hardware reset 
with a low-to-high transition of the hardware RESET pin,
or call the **ad5293_soft_reset** to implement software reset 
through SPI interface. After reset, RDAC registers are loaded 
with midscale, the control registers are restored with default bits.

Minimize The SDO Power Dissipation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The SDO pin contains an open-drain N-channel FET that requires a pull-up 
resistor if this pin is used. This pin could be place in by calling **ad5293_sdo_setfloat** 
high impedence to minimize power dissipation. ( User should be careful with this operation, 
SPI comminications integrity may be affected)

Get Data
^^^^^^^^
In order to get the RDAC calibration mode or wiper value from chips struct, 
you will need to call the **ad5293_get_cali** or **ad5293_get_wiper**, which
returns data of specified chip.

Shutdown
^^^^^^^^
RDAC can be placed in a special state in which Terminal A is open-circuited 
and Wiper W is connected to Terminal B, this is shutdown mode. Contents in 
RDAC are not changed and all command through SPI are supported in this mode.
In order to RDAC to shut down mode, call **ad5293_shutdown** and input parameter
from enmu type **shutdown_t**.

Driver Initialization
---------------------
In order to be able to use the device, you will have to provide the support for
the communication protocol (SPI) as mentioned above.

Device Struct Description
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: C

        struct ad5293_dev {
        /* SPI */
        struct no_os_spi_desc	*spi_desc;
        /* GPIO */
        struct no_os_gpio_desc	*gpio_reset;
        /* number of chips */
        uint16_t chip_num;
        /* pointer of chip struct  */
        struct ad5293_chip_info* chip; //point to chip 0
    };

ad5293_dev
    overall device information holder, multiple chips are regards as one device
no_os_spi_desc
    no-os lib definded spi device instance handler
no_os_gpio_desc
    no-os lib definded gpio instance handler for reset pin
chip_num
    number of chip
ad5293_chip_info
    pointer to allocated memory for **ad5293_chip_info** structs, total amount 
    is chip_num*sizeof(ad5293_chip_info), different chip information is accessed 
    by different pointer offset(array operation) 

Driver Initialization Example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
An initialization and test example on stm32 could be

.. code-block:: C

    #define RESET_PORT 0     //PORT A
    #define RESET_PIN  11
    #define RDAC_CS_PORT 1   //PORT B
    #define RDAC_CS_PIN  12

    //function
    uint32_t get_spi2_clock(void);

    //adi device pointer define
    struct ad5293_dev* pad5293_dev;

    uint32_t get_spi2_clock(void)
    {
        return LL_RCC_GetSPIClockFreq(LL_RCC_SPI123_CLKSOURCE);
    }

    void rdac_init(void)
    {
        int32_t ret = 0;
        struct ad5293_init_param rdac_init;

        //start timer 5 as delay counter
        delay_init();

        struct no_os_gpio_init_param RESET_init;
        struct stm32_gpio_init_param RESET_init_stm;

        RESET_init.port = RESET_PORT;
        RESET_init.number = RESET_PIN;
        RESET_init.pull = NO_OS_PULL_NONE;
        RESET_init.platform_ops = & stm32_gpio_ops;
        RESET_init.extra = (void*) & RESET_init_stm;
        RESET_init_stm.mode = GPIO_MODE_OUTPUT_PP;
        RESET_init_stm.speed = GPIO_SPEED_FREQ_HIGH;
        RESET_init_stm.alternate = 0;

        rdac_init.gpio_reset = & RESET_init;
        //spi GPIO init struct preparation
        struct stm32_spi_init_param stm32_spi_init;

        stm32_spi_init.chip_select_port = RDAC_CS_PORT;   //GPIO CS PORT
        stm32_spi_init.get_input_clock = & get_spi2_clock;
        stm32_spi_init.alternate = 0;
        
        rdac_init.spi_init.extra = (void*) & stm32_spi_init;
        rdac_init.spi_init.platform_ops = & stm32_spi_ops;
        
        rdac_init.spi_init.device_id = 2;    //spi device 1
        rdac_init.spi_init.max_speed_hz = 200 * 1000;  //3mHz
        rdac_init.spi_init.chip_select = RDAC_CS_PIN;     //GPIO CS PIN
        rdac_init.spi_init.mode = NO_OS_SPI_MODE_1;
        rdac_init.spi_init.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST;
        
        rdac_init.spi_init.platform_delays.cs_delay_first = 0;        //delay before transfer
        rdac_init.spi_init.platform_delays.cs_delay_last = 0;         //delay after transfer

        rdac_init.chip_num = 6;

        ret = ad5293_init( & pad5293_dev, & rdac_init);
        if(ret)
            printf("rdac init error. \r\n");

    }

    void rdac_test(void)
    {
        int i, cali_val[pad5293_dev->chip_num];
        int16_t wp_value[pad5293_dev->chip_num];


        ad5293_hard_reset(pad5293_dev);

        no_os_mdelay(1);

        for(i = 0;i < pad5293_dev->chip_num;i++)
        {
            ad5293_update_cali(pad5293_dev, CALI_NORMAL, i);
            ad5293_update_wiper(pad5293_dev, 768, i);
        }

        ad5293_write_cali(pad5293_dev);
        ad5293_write_wiper(pad5293_dev);

        ad5293_read_reg_cali(pad5293_dev);
        ad5293_read_reg_wiper(pad5293_dev);

        no_os_mdelay(1);

        for(i = 0;i < pad5293_dev->chip_num;i++)
        {
            cali_val[i] = ad5293_get_cali(pad5293_dev, i);
            wp_value[i] = ad5293_get_wiper(pad5293_dev, i);
        }

        ad5293_shutdown(pad5293_dev, SHUTDOWN);

        no_os_mdelay(1);

        ad5293_shutdown(pad5293_dev, NORMAL);

        no_os_mdelay(1);

        ad5293_soft_reset(pad5293_dev);

        no_os_mdelay(1);

        for(i = 0;i < pad5293_dev->chip_num;i++)
        {
            ad5293_update_cali(pad5293_dev, CALI_NORMAL, i);
            ad5293_update_wiper(pad5293_dev, 256, i);
        }

        ad5293_write_cali(pad5293_dev);
        ad5293_write_wiper(pad5293_dev);

        ad5293_read_reg_cali(pad5293_dev);
        ad5293_read_reg_wiper(pad5293_dev);

        no_os_mdelay(1);
    }