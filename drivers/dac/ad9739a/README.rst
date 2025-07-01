AD9739A no-OS Driver
====================

Supported Devices
------------------

- :adi:`AD9739A`

Overview
----------

The AD9739A is a 14-bit RF digital-to-analog converter (DAC), capable of
creating wideband signals from DC to 3 GHz. Featuring an update rate up
to 2.5 GSPS, this device is ideal for broadband communications, military
jamming, instrumentation, and radar systems. It features low-voltage
differential signaling (LVDS) for enhanced data and signal quality,
advanced clock management with delay lock loops (DLL) for optimized
timing, crucial for high-speed data transfer, and minimizes signal
distortion. Its dynamic performance is highlighted by Spurious-Free
Dynamic Range (SFDR) and Intermodulation Distortion (IMD). Integration
is streamlined by flexible initialization via SPI configurations for
reliable operation in complex systems.

Applications
-------------

- Broadband communications systems
     - DOCSIS CMTS systems
- Military jammers
- Instrumentation, automatic test equipment
- Radar, avionics

Operation Modes
----------------

+------------------------+-----------------------------------+-------------------+-----------------+
| Mode Name              | Description                       | Config Bits       | Typical Use     |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_PART_ID        | Identifies the device part ID.    | x & 0xFF          | Device          |
| _PART_ID               |                                   |                   | identification  |
|                        |                                   |                   | in system       |
|                        |                                   |                   | check.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MODE_SDIO_DIR  | Sets the SDIO direction for SPI   | (1 << 7)          | Configuring SPI |
|                        | communication by combining bit 7  | (1 << 0)          | communication   |
|                        | and bit 0.                        |                   | direction.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MODE_LSB       | Sets the least significant bit    | (1 << 6)          | Configuring bit |
|                        | (LSB) mode by combining bit 6 and | (1 << 1)          | order for SPI   |
|                        | bit 1.                            |                   | transactions.   |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MODE_RESET     | Reset mode configuration for the  | (1 << 5)          | Resetting the   |
|                        | device by setting bit 5 and bit 2.| (1 << 2)          | device to its   |
|                        |                                   |                   | default state.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_POWER_DOWN     | Powers down the LVDS driver by    | (1 << 5)          | Power           |
| _LVDS_DRVR_PD          | setting the 5th bit.              |                   | management,     |
|                        |                                   |                   | LVDS driver off.|
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_POWER_DOWN     | Powers down the LVDS receiver     | (1 << 4)          | Power           |
| _LVDS_RCVR_PD          | by setting the 4th bit.           |                   | management when |
|                        |                                   |                   | LVDS receiver   |
|                        |                                   |                   | is not needed.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_POWER_DOWN_    | Powers down the clock receiver    | (1 << 1)          | Conserving      |
| CLK_RCVR_PD            | by setting the 2nd bit.           |                   | power when      |
|                        |                                   |                   | clock receiver  |
|                        |                                   |                   | is inactive.    |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_POWER_DOWN     | Controls the power state of       | (1 << 0)          | Power           |
| _DAC_BIAS_PD           | DAC bias by setting the 0th bit.  |                   | management for  |
|                        |                                   |                   | DAC bias.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CNT_CLK        | Disables the clock generator      | (1 << 3)          | Disable clock   |
| _DIS_CLKGEN_PD         | by setting the 3rd bit.           |                   | generator.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CNT_CLK        | Disables receive counter clock    | (1 << 1)          | Disable receive |
| _DIS_REC_CNT_CLK       | by setting the 1st bit.           |                   | clock.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CNT_CLK_       | Disables MU counter clock by      | (1 << 0)          | Disable MU      |
| _DIS_MU_CNT_CLK        | setting the 0th bit.              |                   | counter.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_EN         | Enables MU Last interrupt by      | (1 << 3)          | MU interrupt    |
| _MU_LST_EN             | setting the 3rd bit.              |                   | enable.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_EN         | Enables MU Lock interrupt by      | (1 << 2)          | MU lock         |
| _MU_LCK_EN             | setting the 2nd bit.              |                   | interrupt.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_EN         | Enables receive list interrupt    | (1 << 1)          | Receive list    |
| _RCV_LST_EN            | by setting the 1st bit.           |                   | interrupt       |
|                        |                                   |                   | handling.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_EN         | Enables receive lock interrupt    | (1 << 0)          | Receive lock    |
| _RCV_LCK_EN            | by setting the 0th bit.           |                   | interrupt.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_REQ        | Requests MU Last interrupt by     | (1 << 3)          | Systems         |
| _MU_LST_IRQ            | setting the 3rd bit.              |                   | response to MU  |
|                        |                                   |                   | Last.           |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_REQ        | Requests MU Lock interrupt by     | (1 << 2)          | Systems         |
| _MU_LCK_IRQ            | setting the 2nd bit.              |                   | response to MU  |
|                        |                                   |                   | lock.           |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_REQ        | Requests receive list interrupt   | (1 << 1)          | Handle receive  |
| _RCV_LST_IRQ           | by setting the 1st bit.           |                   | list interrupt. |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_IRQ_REQ        | Requests receive lock interrupt   | (1 << 0)          | Handle receive  |
| _RCV_LCK_IRQ           | by setting the 0th bit.           |                   | lock interrupt. |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_FSC_1_FSC_1    | Defines full-scale current        | x & 0xFF          | Configure DAC   |
|                        | settings for DAC.                 |                   | output current. |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_FSC_2_FSC_2    | Configures second full-scale      | x & 0x3           | Additional DAC  |
|                        | current setting.                  |                   | current         |
|                        |                                   |                   | configuration.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_FSC_2_Sleep    | Sets DAC to sleep mode by         | (1 << 7)          | Reduce power    |
|                        | setting the 7th bit.              |                   | usage.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_DEC_CNT_DAC_DEC| Sets DAC’s decimation count by    | x & 0x3           | Changing DAC    |
|                        | masking.                          |                   | decimation      |
|                        |                                   |                   | settings.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_DAC_DEC        | Set DAC normal baseband mode.     | 0                 | Normal baseband |
| _NORMAL_BASEBAND       |                                   |                   | mode.           |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_DAC_DEC        | Configures DAC mix-mode using     | 2                 | Operate in      |
| _MIX_MODE              | a specific value.                 |                   | mix-mode.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_STAT1     | DCI pre-phase 0 indicator by      | (1 << 2)          | Monitoring LVDS |
| __DCI_PRE_PH0          | setting bit 2.                    |                   | status.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_STAT1     | DCI post-phase 0 indicator by     | (1 << 0)          | Status check    |
| _DCI_PST_PH0           | setting bit 0.                    |                   | for LVDS post   |
|                        |                                   |                   | phase.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT1  | Resets LVDS receiver flag         | (1 << 2)          | Reset receiver  |
| _RCVR_FLG_RST          | using bit 2.                      |                   | flag.           |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT1  | Enables LVDS receiver loop        | (1 << 1)          | LVDS loop       |
| _RCVR_LOOP_ON          | using bit 1.                      |                   | control.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT1  | Enables LVDS receiver count       | (1 << 0)          | Enable receiver |
| _RCVR_CNT_ENA          | using bit 0.                      |                   | count.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT2  | Sets sample delay in LVDS         | x & 0x3           | LVDS            |
| _SMP_DEL               | receiver count 2.                 |                   | configuration.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT3  | Sets sample delay in LVDS         | x & 0xFF          | Fine-tune LVDS  |
| _SMP_DEL               | receiver count 3.                 |                   | delay.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT4  | Sets DCI delay in receiver        | x & 0xF           | LVDS delay      |
| _DCI_DEL               | count 4.                          |                   | adjustment.     |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_CNT4  | Adjusts fine delay skew in        | x & 0xF           | Skew adjustment |
| _FINE_DEL_SKEW         | LVDS.                             |                   | control.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC       | Sets DCI delay in receiver        | x & 0x3F          | Control         |
| _CNT5_DCI_DEL          | count 5.                          |                   | receiver delay. |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT1 | Sets sample delay in receiver     | x & 0x3           | Receiver status |
| _SMP_DEL               | status 1.                         |                   | sampling.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT2 | Sets sample delay in receiver     | x & 0xFF          | Fine control    |
| _SMP_DEL               | status 2.                         |                   | over sampling.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT3 | Configures DCI delay in           | x & 0x3           | Adjust receiver |
| _DCI_DEL               | receiver status 3.                |                   | phase delay.    |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT4 | Sets DCI delay in receiver        | x & 0xFF          | Control status  |
| _DCI_DEL               | status 4.                         |                   | delays.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT9 | Indicates receiver tracking       | (1 << 3)          | Track receiver  |
| _RCVR_TRK_ON           | by setting bit 3.                 |                   | status.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT9 | Indicates receiver front-end      | (1 << 2)          | Receiver        |
| _RCVR_FE_ON            | ON with bit 2.                    |                   | front-end       |
|                        |                                   |                   | status.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT9 | Indicates receiver lost           | (1 << 1)          | Monitor lost    |
| _RCVR_LST              | state by setting bit 1.           |                   | state.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_LVDS_REC_STAT9 | Indicates receiver locked         | (1 << 0)          | Monitor lock    |
| _RCVR_LCK              | state with bit 0.                 |                   | status.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CROSS_CNT1     | Sets direction for cross          | (1 << 4)          | Configure       |
| _DIR_P                 | counter 1 using bit 4.            |                   | counter         |
|                        |                                   |                   | direction.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CROSS_CNT1     | Sets clock positive offset        | x & 0xF           | Clock offset    |
| _CLKP_OFFSET           | in cross counter 1.               |                   | configuration.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CROSS_CNT2     | Sets negative direction for       | (1 << 4)          | Counter         |
| _DIR_N                 | cross counter 2.                  |                   | direction       |
|                        |                                   |                   | setup.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_CROSS_CNT2     | Sets clock negative offset        | x & 0xF           | Adjust clock    |
| _CLKN_OFFSET           | in cross counter 2.               |                   | spacing.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_PHS_DET        | Sets phase detector               | (1 << 5)          | Boost phase     |
| _CMP_BST               | comparator boost with bit 5.      |                   | detecting.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_PHS_DET        | Enables automatic phase           | (1 << 4)          | Automate phase  |
| _AUTO_EN               | detection with bit 4.             |                   | detect.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_DUTY        | Enables duty cycle                | (1 << 7)          | Adjust duty     |
| _MU_DUTY_AUTO_EN       | adjustment using bit 7.           |                   | cycle.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT1_SLOPE  | Sets slope configuration in       | (1 << 6)          | Modify MU       |
|                        | MU counter using bit 6.           |                   | slope.          |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT1_MODE   | Sets MU counter mode by           | x & 0x3,          | Mode            |
|                        | shifting an input.                | shifted by 4      | configuration.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT1_READ   | Enables reading MU counter        | (1 << 3)          | Read MU         |
|                        | using bit 3.                      |                   | configuration.  |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT1_GAIN   | Sets gain in MU counter           | x & 0x3,          | Gain setting    |
|                        | using shifted input.              | shifted by 1      | for MU.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT1_ENABLE | Enables MU counter with bit 0     | (1 << 0)          | Activate MU     |
|                        |                                   |                   | counter.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT2_MUDEL  | Enable or configure               | (1 << 7)          | Modulation      |
|                        | modulation using bit 7.           |                   | control.        |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT2_SRCH   | Sets MU search mode with          | x & 0x3,          | Coordinate      |
| _MODE                  | shifted input.                    | shifted by 5      | search mode.    |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT2        | Configures phase setting in       | x & 0x1F          | Set phase       |
| _SET_PHS               | MU register.                      |                   | dynamically.    |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT3_MUDEL  | Adjusts modulation field in       | x & 0xFF          | Refine          |
|                        | MU count 3 register.              |                   | modulation      |
|                        |                                   |                   | settings.       |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT4        | Sets search tolerance in MU       | (1 << 7)          | Adjust search   |
| _SEARCH_TOL            | count 4 using bit 7.              |                   | tolerance.      |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT4_RETRY  | Configures retry behavior         | (1 << 6)          | Control retry   |
|                        | using bit 6.                      |                   | operations.     |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT4        | Sets contrast in MU count 4       | (1 << 5)          | Control         |
| _CONTRST               | with bit 5.                       |                   | contrast        |
|                        |                                   |                   | levels.         |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_CNT4_GUARD  | Sets guard value in MU            | x & 0x1F          | Define guard    |
|                        | register.                         |                   | for operations. |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_STAT1       | Detects MU list condition         | (1 << 1)          | List presence   |
| _MU_LST                | using bit 1.                      |                   | monitoring.     |
+------------------------+-----------------------------------+-------------------+-----------------+
| AD9739A_MU_STAT1       | Detects MU lock condition         | (1 << 0)          | Monitor lock    |
| _MU_LKD                | using bit 0.                      |                   | status.         |
+------------------------+-----------------------------------+-------------------+-----------------+

Device Configuration
--------------------

The ``ad9739a_setup`` function initializes the AD9739A device by first
allocating memory using ``no_os_malloc``, then establishing SPI
communication through ``no_os_spi_init``, and confirming the device ID via
ad9739a_read. It proceeds to configure various registers using
ad9739a_write and performs a device reset with ad9739a_reset. To ensure
the MU controller and LVDS receiver are properly locked and operational,
the function repeatedly writes to specific registers and checks their
status, incorporating delay_fdata_cycles for timing control.
Additionally, it sets the DAC’s full-scale current using
ad9739a_dac_fs_current, displays a success message, and returns the
initialized device structure.

Initialization and Cleanup
~~~~~~~~~~~~~~~~~~~~~~~~~~

The initialization and cleanup functions prepare the device for
operation and ensure proper resource management. The ``ad9739a_setup()``
function initializes the AD9739A DAC by configuring its SPI interface,
verifying the device ID, setting up the MU controller, configuring the
LVDS receiver, and establishing the full-scale output current. After the
device has served its purpose, the ``ad9739a_remove()`` function is used
to deallocate resources by removing the SPI descriptor and freeing
allocated memory. Together, these functions ensure that the device is
correctly integrated into a system during initialization and safely
removed when no longer needed.

Register Communication Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For basic register access, the driver provides functions that facilitate
SPI-based communication with the AD9739A DAC registers. The
``ad9739a_write()`` function constructs and sends SPI commands needed to
update register values, while the ``ad9739a_read()`` function retrieves
register values using similarly structured SPI transactions. These two
functions abstract the low-level details of SPI communication to provide
a simple and effective API for register read and write operations.

Reset and Power Management
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Robust operation of the device requires efficient management of its
reset and power states. The ``ad9739a_reset()`` function performs a
software reset by writing commands that restore default settings,
thereby clearing the device’s reset state. Complementing this, the
``ad9739a_power_down()`` function is used to manage the device’s power
configuration by either applying a new power setting or retrieving the
current power status when invalid configurations are provided. These
functions allow high-level control over device resets and power
sequences in a streamlined, consistent manner.

Mode and Output Current Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For fine-tuning the DAC’s operational behavior, the driver includes
functions that adjust both the operational mode and the full-scale
output current. The ``ad_serdes_clk()`` function selects among different
operational modes—such as normal baseband mode or mixed mode—by
configuring the device’s mode register, and it also supports reading the
current mode setting. In addition, the ``ad9739a_dac_fs_current()``
function configures or retrieves the DAC’s full-scale output current.
Together, these functions provide the flexibility needed to tailor the
device’s performance to diverse application requirements.

Timing and Delay Control
~~~~~~~~~~~~~~~~~~~~~~~~

Precise timing control is critical for synchronizing DAC operations, and
the driver addresses this need with a dedicated function. The
``delay_fdata_cycles()`` function introduces a delay based on a
specified number of DAC cycles, effectively translating cycle counts
into controlled timing delays. This functionality is essential for
ensuring proper synchronization across the device’s operations.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include <stdlib.h>
   #include <stdio.h>
   #include "ad9739a.h"
   #include "no_os_alloc.h"

   #define FDATA 2500  // for 2.5 GSPS

   int32_t ad9739a_setup(struct ad9739a_dev **device,
                 struct ad9739a_init_param init_param)
   {
       int32_t ret = 0;
       float fret = 0;
       uint8_t chip_id = 0;
       uint8_t dll_loop_lock_counter = 0;
       uint8_t dll_loop_locked = 0;
       uint8_t ad9739a_reg_mu_stat1_buf = 0;
       uint8_t ad9739a_reg_lvds_rec_stat9_buf;
       struct ad9739a_dev *dev;

       dev = (struct ad9739a_dev *)no_os_malloc(sizeof(*dev));
       if (!dev)
           return -1;

       /* SPI */
       ret = no_os_spi_init(&dev->spi_desc, &init_param.spi_init);

       /* Device ID */
       ad9739a_read(dev, AD9739A_REG_PART_ID, &chip_id);
       if (chip_id != AD9739A_CHIP_ID) {
           printf("Error: Invalid CHIP ID (0x%x).\n", chip_id);
           return -1;
       }

       /* Set 4-wire SPI, MSB first. */
       ret |= ad9739a_write(dev, AD9739A_REG_MODE, 0x00);
       if (ret < 0) {
           return ret;
       }
       ret = ad9739a_reset(dev);
       if (ret < 0) {
           return ret;
       }
       /* Set the common-mode voltage of DACCLK_P and DACCLK_N inputs. */
       ret = ad9739a_write(dev,
                   AD9739A_REG_CROSS_CNT1,
                   AD9739A_CROSS_CNT1_CLKP_OFFSET(init_param.
                           common_mode_voltage_dacclk_p));
       if (ret < 0) {
           return ret;
       }
       ret = ad9739a_write(dev,
                   AD9739A_REG_CROSS_CNT2,
                   AD9739A_CROSS_CNT2_CLKN_OFFSET(init_param.
                           common_mode_voltage_dacclk_n));
       if (ret < 0) {
           return ret;
       }

       /* MU CONTROLLER Setup*/
       /* Phase detector enable and boost bias bits. */
       ret = ad9739a_write(dev,
                   AD9739A_REG_PHS_DET,
                   AD9739A_PHS_DET_CMP_BST |
                   AD9739A_PHS_DET_PHS_DET_AUTO_EN);
       if (ret < 0) {
           return ret;
       }
       ret = ad9739a_write(dev,
                   AD9739A_REG_MU_DUTY,
                   AD9739A_MU_DUTY_MU_DUTY_AUTO_EN);
       if (ret < 0) {
           return ret;
       }
       /* Set the MU Controller search direction to UP and the target phase
        * to 4.
        * */
       ret = ad9739a_write(dev,
                   AD9739A_REG_MU_CNT2,
                   AD9739A_MU_CNT2_SRCH_MODE(1) |
                   AD9739A_MU_CNT2_SET_PHS(4));
       if (ret < 0) {
           return ret;
       }
       /* Set the MU delay value at witch the controller begins its search
        * to 216.
        * */
       ret = ad9739a_write(dev,
                   AD9739A_REG_MU_CNT3,
                   AD9739A_MU_CNT3_MUDEL(0x6C));
       if (ret < 0) {
           return ret;
       }
       do {
           /* Set: find the exact targeted phase, retry the search until
            * correct value is found, continue if desired phase is not
            * found, guard band set to optimal value: 0xB.
            * */
           ret = ad9739a_write(dev,
                       AD9739A_REG_MU_CNT4,
                       AD9739A_MU_CNT4_SEARCH_TOL |
                       AD9739A_MU_CNT4_RETRY |
                       AD9739A_MU_CNT4_GUARD(0xB));
           if (ret < 0) {
               return ret;
           }
           /* Set the MU controller tracking gain to the recommended value
            * of 0x1.
            * */
           ret = ad9739a_write(dev,
                       AD9739A_REG_MU_CNT1,
                       AD9739A_MU_CNT1_GAIN(0x1));
           if (ret < 0) {
               return ret;
           }
           /* Enable the MU controller. */
           ret = ad9739a_write(dev,
                       AD9739A_REG_MU_CNT1,
                       AD9739A_MU_CNT1_GAIN(0x1) |
                       AD9739A_MU_CNT1_ENABLE);
           delay_fdata_cycles(180000);
           dll_loop_lock_counter++;
           ad9739a_read(dev, AD9739A_REG_MU_STAT1,
                    &ad9739a_reg_mu_stat1_buf);
           if (ad9739a_reg_mu_stat1_buf == AD9739A_MU_STAT1_MU_LKD) {
               dll_loop_locked = 1;
           }
       } while ((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
       if (dll_loop_locked == 0) {
           return -1;
       }
       /* Set FINE_DEL_SKEW to 2 for a larger DCI sampling window. */
       ret = ad9739a_write(dev,
                   AD9739A_REG_LVDS_REC_CNT4,
                   AD9739A_LVDS_REC_CNT4_FINE_DEL_SKEW(2) |
                   AD9739A_LVDS_REC_CNT4_DCI_DEL(0x7));
       if (ret < 0) {
           return ret;
       }
       dll_loop_lock_counter = 0;
       dll_loop_locked = 0;
       do {
           /* Disable the data Rx controller before enabling it. */
           ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1, 0x00);
           if (ret < 0) {
               return ret;
           }
           /* Enable the data Rx controller for loop and IRQ. */
           ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1,
                       AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON);
           if (ret < 0) {
               return ret;
           }
           /* Enable the data Rx controller for search and track mode. */
           ret = ad9739a_write(dev, AD9739A_REG_LVDS_REC_CNT1,
                       AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON |
                       AD9739A_LVDS_REC_CNT1_RCVR_CNT_ENA);
           if (ret < 0) {
               return ret;
           }
           delay_fdata_cycles(135000);
           dll_loop_lock_counter++;
           ad9739a_read(dev, AD9739A_REG_LVDS_REC_STAT9,
                    &ad9739a_reg_lvds_rec_stat9_buf);
           if (ad9739a_reg_lvds_rec_stat9_buf ==
               (AD9739A_LVDS_REC_STAT9_RCVR_LCK |
                AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON)) {
               dll_loop_locked = 1;
           }
       } while ((dll_loop_lock_counter <= 3) && (dll_loop_locked == 0));
       if (dll_loop_locked == 0) {
           printf("AD9739A error: DLL unlocked.\n");
           return -1;
       }
       fret = ad9739a_dac_fs_current(dev, init_param.full_scale_current);
       if (fret < 0) {
           return (int32_t)fret;
       }

       printf("AD9739A successfully initialized.\n");

       *device = dev;

       return ret;
   }
