/*!
 * \brief     API header file
 *            This file contains all the publicly exposed methods and data
 *            structures to interface with API.
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_GPIO
 * @{
 */
 
#ifndef __ADI_APOLLO_GPIO_TYPES_H__
#define __ADI_APOLLO_GPIO_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_NUM_CMOS_GPIO                 35     /*! [34:00] */
#define ADI_APOLLO_NUM_GPIO                      51     /*! [50:35] */

/*!
* \brief Enumerates Sync Pads 
*/
typedef enum {
    ADI_APOLLO_SYNCINB0_B  = 35,     /*!<SYNCINB0_B. In CMOS mode splits into syncinb0_se1_b, syncinb0_se2_b CMOS GPIOs */
    ADI_APOLLO_SYNCINB1_B  = 37,     /*!<SYNCINB1_B. In CMOS mode splits into syncinb1_se1_b, syncinb1_se2_b CMOS GPIOs */
    ADI_APOLLO_SYNCINB0_A  = 39,     /*!<SYNCINB0_A. In CMOS mode splits into syncinb0_se1_a, syncinb0_se2_a CMOS GPIOs */
    ADI_APOLLO_SYNCINB1_A  = 41,     /*!<SYNCINB1_A. In CMOS mode splits into syncinb1_se1_a, syncinb1_se2_a CMOS GPIOs */
    ADI_APOLLO_SYNCOUTB0_B = 43,     /*!<SYNCOUTB0_B. In CMOS mode splits into syncoutb0_se1_b, syncoutb0_se2_b CMOS GPIOs */
    ADI_APOLLO_SYNCOUTB1_B = 45,     /*!<SYNCOUTB1_B. In CMOS mode splits into syncoutb1_se1_b, syncoutb1_se2_b CMOS GPIOs */
    ADI_APOLLO_SYNCOUTB0_A = 47,     /*!<SYNCOUTB0_A. In CMOS mode splits into syncoutb0_se1_a, syncoutb0_se2_a CMOS GPIOs */
    ADI_APOLLO_SYNCOUTB1_A = 49      /*!<SYNCOUTB1_A. In CMOS mode splits into syncoutb1_se1_a, syncoutb1_se2_a CMOS GPIOs */
} adi_apollo_gpio_sync_pad_e;

/*!
* \brief Enumerates Sync Pad modes 
*/
typedef enum {
    ADI_APOLLO_CMOS_MODE  = 0,             /*!<sets SYNC PAD in CMOS mode. Sync pad will be
                                               available as 2 CMOS GPIO's */
    ADI_APOLLO_LVDS_MODE  = 1              /*!<sets SYNC PAD in LVDS mode. */
} adi_apollo_gpio_sync_pad_mode_e;

/*!
* \brief Enumerates GPIO Direction
*/
typedef enum {
    ADI_APOLLO_GPIO_DIR_INPUT   = 1,        /*!<sets GPIO direction to INPUT */
    ADI_APOLLO_GPIO_DIR_OUTPUT  = 0         /*!<sets GPIO direction to OUTPUT */
} adi_apollo_gpio_dir_e;

/*!
* \brief Enumerates Quick Config Profiles
*/
typedef enum {
    ADI_APOLLO_QUICK_CFG_DISABLE    = 0,     /*!<Disable quick config and allow custom routings */
    ADI_APOLLO_QUICK_CFG_PROFILE_1  = 1,     /*!<GPIO customer quick configuration profile 1 */
    ADI_APOLLO_QUICK_CFG_PROFILE_2  = 2,     /*!<GPIO customer quick configuration profile 2 */
    ADI_APOLLO_QUICK_CFG_PROFILE_3  = 3,     /*!<GPIO customer quick configuration profile 3 */
    ADI_APOLLO_QUICK_CFG_PROFILE_4  = 4,     /*!<GPIO customer quick configuration profile 4 */
    ADI_APOLLO_QUICK_CFG_PROFILE_5  = 5,     /*!<GPIO customer quick configuration profile 5 */
    ADI_APOLLO_QUICK_CFG_PROFILE_6  = 6,     /*!<GPIO customer quick configuration profile 6 */
    ADI_APOLLO_QUICK_CFG_PROFILE_7  = 7,     /*!<GPIO customer quick configuration profile 7 */
    ADI_APOLLO_QUICK_CFG_PROFILE_8  = 8      /*!<GPIO customer quick configuration profile 8 */
} adi_apollo_gpio_quick_cfg_profile_e;

/*!
* \brief Enumerates GPIO Functions
*/
typedef enum {
    ADI_APOLLO_FUNC_SPI1_SDI                   = 1,    /*!< Secondary SPI SDI */
    ADI_APOLLO_FUNC_M_SPI_SDO                  = 2,    /*!< SPI Master SDO */
    ADI_APOLLO_FUNC_M_SPI_CLK                  = 3,    /*!< SPI Master Clocks */
    ADI_APOLLO_FUNC_ARM_TMS_SWDO               = 4,    /*!< Application JTAG Signals */
    ADI_APOLLO_FUNC_SPI1_SCLK                  = 5,    /*!< Secondary SPI Clock */
    ADI_APOLLO_FUNC_SPI1_CSB                   = 6,    /*!< Secondary SPI CSB */
    ADI_APOLLO_FUNC_TXEN_FROM_GPIO_0           = 7,    /*!<Control all the slices of side A Tx(Default),
                                                           SPI programable to control any slices of side A Tx */
    ADI_APOLLO_FUNC_TXEN_FROM_GPIO_1           = 8,    /*!<Control all the slices of side B Tx(Default),
                                                           SPI programable to control any slices of side B Tx */
    ADI_APOLLO_FUNC_TXEN_FROM_GPIO_2           = 9,    /*!<SPI programable to control any slices of side A Tx */
    ADI_APOLLO_FUNC_TXEN_FROM_GPIO_3           = 10,   /*!<SPI programable to control any slices of side B Tx */
    
    ADI_APOLLO_FUNC_RXEN_FROM_GPIO_0           = 11,   /*!<Control all the slices of side A Rx(Default),
                                                           SPI programable to control any slices of side A Rx */
    ADI_APOLLO_FUNC_RXEN_FROM_GPIO_1           = 12,   /*!<Control all the slices of side B Rx(Default),
                                                           SPI programable to control any slices of side B Rx */
    ADI_APOLLO_FUNC_RXEN_FROM_GPIO_2           = 13,   /*!<SPI programable to control any slices of side A Rx */
    ADI_APOLLO_FUNC_RXEN_FROM_GPIO_3           = 14,   /*!<SPI programable to control any slices of side B Rx */
    ADI_APOLLO_FUNC_M_SPI_SDI                  = 15,   /*!<SPI Master SDI */
    ADI_APOLLO_FUNC_PROFILE_TX_RXN_0           = 16,   /*!<profile_tx_rxn[0] */
    ADI_APOLLO_FUNC_PROFILE_TX_RXN_1           = 17,   /*!<profile_tx_rxn[1] */
    ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_0       = 18,   /*!<profile_txrx_slice[0] */
    ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_1       = 19,   /*!<profile_txrx_slice[1] */
    ADI_APOLLO_FUNC_PROFILE_TXRX_SLICE_2       = 20,   /*!<profile_txrx_slice[2] */
    ADI_APOLLO_FUNC_PROFILE_TXRX_BA            = 21,   /*!<0-A; 1-B */
    ADI_APOLLO_FUNC_PROFILE_FCN_SEL_0          = 22,   /*!<Select the function/block to be updated */
    ADI_APOLLO_FUNC_PROFILE_FCN_SEL_1          = 23,   /*!<Select the function/block to be updated */
    ADI_APOLLO_FUNC_PROFILE_FCN_SEL_2          = 24,   /*!<Select the function/block to be updated */
    ADI_APOLLO_FUNC_PROFILE_FCN_SEL_3          = 158,  /*!<Select the function/block to be updated */
    ADI_APOLLO_FUNC_PROFILE_0                  = 25,   /*!<Profiles 0-31 */
    ADI_APOLLO_FUNC_PROFILE_1                  = 26,   /*!<Profiles 0-31 */
    ADI_APOLLO_FUNC_PROFILE_2                  = 27,   /*!<Profiles 0-31 */
    ADI_APOLLO_FUNC_PROFILE_3                  = 28,   /*!<Profiles 0-31 */
    ADI_APOLLO_FUNC_PROFILE_4                  = 29,   /*!<Profiles 0-31 */
    ADI_APOLLO_FUNC_FREEZE                     = 30,   /*!<FREEZE input pin polled by the FW to pause the update of calibration coefficients */
    ADI_APOLLO_FUNC_JTX_SYNC_0                 = 31,   /*!<Sync for JTX side A Link0 */
    ADI_APOLLO_FUNC_JTX_SYNC_1                 = 32,   /*!<Sync for JTX side A Link1 */
    ADI_APOLLO_FUNC_JTX_SYNC_2                 = 33,   /*!<Sync for JTX side B Link0 */
    ADI_APOLLO_FUNC_JTX_SYNC_3                 = 34,   /*!<Sync for JTX side B Link1 */
    ADI_APOLLO_FUNC_ARM_TRST                   = 35,   /*!<Application JTAG Signal */
    ADI_APOLLO_FUNC_ARM_TDI                    = 36,   /*!<Application JTAG Signal */
    ADI_APOLLO_FUNC_ARM_TCK                    = 37,   /*!<Application JTAG Signal */
    ADI_APOLLO_FUNC_UART_PADRXSIN              = 38,   /*!<UART Signal */
    ADI_APOLLO_FUNC_UART_PADCTS                = 39,   /*!<UART Signal */
    ADI_APOLLO_FUNC_FFT_ENABLE_A               = 40,   /*!<fft_enable_A */
    ADI_APOLLO_FUNC_FFT_HOLD_A                 = 41,   /*!<fft_hold_A */
    ADI_APOLLO_FUNC_FFT_ENABLE_B               = 42,   /*!<fft_enable_B */
    ADI_APOLLO_FUNC_FFT_HOLD_B                 = 43,   /*!<fft_hold_B */
    ADI_APOLLO_FUNC_LINEARX_RX0_OFFLINE        = 44,   /*!<LinearX_Rx0_offline */
    ADI_APOLLO_FUNC_LINEARX_RX1_OFFLINE        = 45,   /*!<LinearX_Rx1_offline */
    ADI_APOLLO_FUNC_LINEARX_RX2_OFFLINE        = 46,   /*!<LinearX_Rx2_offline */
    ADI_APOLLO_FUNC_ADC_MODE_SWITCH            = 47,   /*!<ADC fast mode switch */
    ADI_APOLLO_FUNC_SPI1_SDO                   = 48,   /*!<Secondary SPI SDO */
    ADI_APOLLO_FUNC_M_SPI_CS_0                 = 49,   /*!<SPI Master CS M_SPI_CS[0] */
    ADI_APOLLO_FUNC_M_SPI_CS_1                 = 50,   /*!<SPI Master CS M_SPI_CS[1] */
    ADI_APOLLO_FUNC_M_SPI_CS_2                 = 51,   /*!<SPI Master CS M_SPI_CS[2] */
    ADI_APOLLO_FUNC_M_SPI_CS_3                 = 52,   /*!<SPI Master CS M_SPI_CS[3] */
    ADI_APOLLO_FUNC_IRQ_OUT                    = 53,   /*!<FW controlled interrrupt output pin to signal to the user that an error has occurred */
    ADI_APOLLO_FUNC_TRIGOUT_RX_A               = 54,   /*!<TRIG GPIO output pin, generated by the trigger masters  */ 
    ADI_APOLLO_FUNC_TRIGOUT_RX_B               = 55,   /*!<TRIG GPIO output pin, generated by the trigger masters  */
    ADI_APOLLO_FUNC_TRIGOUT_TX_A               = 56,   /*!<TRIG GPIO output pin, generated by the trigger masters  */
    ADI_APOLLO_FUNC_TRIGOUT_TX_B               = 57,   /*!<TRIG GPIO output pin, generated by the trigger masters  */
    ADI_APOLLO_FUNC_JRX_SYNC_0                 = 58,   /*!<Sync for JRX side A Link0 */
    ADI_APOLLO_FUNC_JRX_SYNC_1                 = 59,   /*!<Sync for JRX side A Link1 */
    ADI_APOLLO_FUNC_JRX_SYNC_2                 = 60,   /*!<Sync for JRX side B Link0 */
    ADI_APOLLO_FUNC_JRX_SYNC_3                 = 61,   /*!<Sync for JRX side B Link1 */
    ADI_APOLLO_FUNC_ARM_TDO_SWO_MUX            = 62,   /*!<Application JTAG Signal */
    ADI_APOLLO_FUNC_UART_PADRSTOUT             = 63,   /*!<UART Signal */
    ADI_APOLLO_FUNC_UART_PADTXSOUT             = 64,   /*!<UART Signal */
    ADI_APOLLO_FUNC_ARM_SWO                    = 65,   /*!<arm_swo */
    ADI_APOLLO_FUNC_TRACE_CLK_OUT              = 66,   /*!<trace_clk_out */
    ADI_APOLLO_FUNC_TRACE_DATA_OUT_0           = 67,   /*!<trace_data_out[0] */
    ADI_APOLLO_FUNC_TRACE_DATA_OUT_1           = 68,   /*!<trace_data_out[0] */
    ADI_APOLLO_FUNC_TRACE_DATA_OUT_2           = 69,   /*!<trace_data_out[0] */
    ADI_APOLLO_FUNC_TRACE_DATA_OUT_3           = 70,   /*!<trace_data_out[0] */
    ADI_APOLLO_FUNC_ADC_FD_A0                  = 71,   /*!<Fast detect ADC A0 */
    ADI_APOLLO_FUNC_ADC_FD_A2                  = 72,   /*!<Fast detect ADC A2 (4t4r - A0 duplicate)*/
    ADI_APOLLO_FUNC_ADC_FD_A1                  = 73,   /*!<Fast detect ADC A1 */
    ADI_APOLLO_FUNC_ADC_FD_A3                  = 74,   /*!<Fast detect ADC A3 (4t4r - A1 duplicate)*/
    ADI_APOLLO_FUNC_ADC_FD_B0                  = 75,   /*!<Fast detect ADC B0 */
    ADI_APOLLO_FUNC_ADC_FD_B2                  = 76,   /*!<Fast detect ADC B2 (4t4r - B0 duplicate)*/
    ADI_APOLLO_FUNC_ADC_FD_B1                  = 77,   /*!<Fast detect ADC B1 */
    ADI_APOLLO_FUNC_ADC_FD_B3                  = 78,   /*!<Fast detect ADC B3 (4t4r - B1 duplicate)*/
    ADI_APOLLO_FUNC_PA0_EN                     = 79,   /*!<pa0 _en */
    ADI_APOLLO_FUNC_PA1_EN                     = 80,   /*!<pa1 _en */
    ADI_APOLLO_FUNC_PA2_EN                     = 81,   /*!<pa2 _en */
    ADI_APOLLO_FUNC_PA3_EN                     = 82,   /*!<pa3 _en */
    ADI_APOLLO_FUNC_PA4_EN                     = 83,   /*!<pa4 _en */
    ADI_APOLLO_FUNC_PA5_EN                     = 84,   /*!<pa5 _en */
    ADI_APOLLO_FUNC_PA6_EN                     = 85,   /*!<pa6 _en */
    ADI_APOLLO_FUNC_PA7_EN                     = 86,   /*!<pa7 _en */
    ADI_APOLLO_FUNC_FFT_DONE_A                 = 87,   /*!<fft_done_A */
    ADI_APOLLO_FUNC_FFT_DONE_B                 = 88,   /*!<fft_done_B */
    ADI_APOLLO_FUNC_SYNC_IRQ_A                 = 89,   /*!<sync_irq_a */
    ADI_APOLLO_FUNC_SYNC_IRQ_B                 = 90,   /*!<sync_irq_b */   
    ADI_APOLLO_FUNC_SYNC_IRQ_C                 = 91,   /*!<sync_irq_c */
    ADI_APOLLO_FUNC_GP_INTERRUPT               = 92,   /*!<Pin Interrupt */
    ADI_APOLLO_FUNC_SMON_A_0                   = 93,   /*!<smon_A[0] */
    ADI_APOLLO_FUNC_SMON_A_1                   = 94,   /*!<smon_A[1] */
    ADI_APOLLO_FUNC_SMON_A_2                   = 95,   /*!<smon_A[2] */
    ADI_APOLLO_FUNC_SMON_A_3                   = 96,   /*!<smon_A[3] */
    ADI_APOLLO_FUNC_SMON_A_4                   = 97,   /*!<smon_A[4] */
    ADI_APOLLO_FUNC_SMON_A_5                   = 98,   /*!<smon_A[5] */
    ADI_APOLLO_FUNC_SMON_A_6                   = 99,   /*!<smon_A[6] */
    ADI_APOLLO_FUNC_SMON_A_7                   = 100,  /*!<smon_A[7] */
    ADI_APOLLO_FUNC_SMON_B_0                   = 101,  /*!<smon_B[0] */
    ADI_APOLLO_FUNC_SMON_B_1                   = 102,  /*!<smon_B[1] */
    ADI_APOLLO_FUNC_SMON_B_2                   = 103,  /*!<smon_B[2] */
    ADI_APOLLO_FUNC_SMON_B_3                   = 104,  /*!<smon_B[3] */
    ADI_APOLLO_FUNC_SMON_B_4                   = 105,  /*!<smon_B[4] */
    ADI_APOLLO_FUNC_SMON_B_5                   = 106,  /*!<smon_B[5] */
    ADI_APOLLO_FUNC_SMON_B_6                   = 107,  /*!<smon_B[6] */
    ADI_APOLLO_FUNC_SMON_B_7                   = 108,  /*!<smon_B[7] */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGER0_A      = 109,  /*!<SPI programable for side A DAC0 */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGER1_A      = 110,  /*!<SPI programable for side A DAC1 */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGER0_B      = 111,  /*!<SPI programable for side B DAC0 */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGER1_B      = 112,  /*!<SPI programable for side B DAC1 */
    ADI_APOLLO_FUNC_ALL_IRQ_BAD                = 113,  /*!<all _IRQ_bad */
    ADI_APOLLO_FUNC_LINEARX_SW0_CTL_0          = 114,  /*!<LinearX_sw0_ctl[0] */
    ADI_APOLLO_FUNC_LINEARX_SW0_CTL_1          = 115,  /*!<LinearX_sw0_ctl[1] */
    ADI_APOLLO_FUNC_LINEARX_SW1_CTL_0          = 116,  /*!<LinearX_sw1_ctl[0] */
    ADI_APOLLO_FUNC_LINEARX_SW1_CTL_1          = 117,  /*!<LinearX_sw1_ctl[1] */
    ADI_APOLLO_FUNC_CLOCK_DELADJ_0             = 118,  /*!<clock_deladj_0 */
    ADI_APOLLO_FUNC_CLOCK_DELSTR_0             = 119,  /*!<clock_delstr_0 */
    ADI_APOLLO_FUNC_CLOCK_DELADJ_1             = 120,  /*!<clock_deladj_1 */
    ADI_APOLLO_FUNC_CLOCK_DELSTR_1             = 121,  /*!<clock_delstr_1 */
    ADI_APOLLO_FUNC_I_RECONFIG_DONE_RX_A       = 122,  /*!<I_reconfig_done_rx_a */
    ADI_APOLLO_FUNC_I_RECONFIG_DONE_RX_B       = 123,  /*!<I_reconfig_done_rx_b */
    ADI_APOLLO_FUNC_I_RECONFIG_DONE_TX_A       = 124,  /*!<I_reconfig_done_tx_a */
    ADI_APOLLO_FUNC_I_RECONFIG_DONE_TX_B       = 125,  /*!<I_reconfig_done_tx_b */
    ADI_APOLLO_FUNC_JESD_TX_TPL_PHASE_EST_A    = 126,  /*!<JESD_Tx_TPL_PHASE_ESTABLISHED_A[1:0]. AND of 2 bits */
    ADI_APOLLO_FUNC_JESD_TX_TPL_PHASE_EST_B    = 127,  /*!<JESD_Tx_TPL_PHASE_ESTABLISHED_B[1:0]. AND of 2 bits */
    ADI_APOLLO_FUNC_JESD_PLL_LOCK              = 128,  /*!<Serdes PLL Locked */
    ADI_APOLLO_FUNC_IRQ_JESD_RX0               = 129,  /*!<Side A JRx link0/1 status */
    ADI_APOLLO_FUNC_IRQ_JESD_RX1               = 130,  /*!<Side B JRx link0/1 status */
    ADI_APOLLO_FUNC_IRQ_CLOCKING_LOCK          = 131,  /*!<7G-14G clocking PLL locked */
    ADI_APOLLO_FUNC_IRQ_CLOCKING_LOST          = 132,  /*!<7G-14G clocking PLL lost */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGERS_A      = 133,  /*!<PA_protect_Trigger0_A|PA_protect_Trigger1_A */
    ADI_APOLLO_FUNC_PA_PROTECT_TRIGGERS_B      = 134,  /*!<PA_protect_Trigger0_B|PA_protect_Trigger1_B */
    ADI_APOLLO_FUNC_I_NVM_BOOT_DONE            = 135,  /*!<NVM Boot Done */
    ADI_APOLLO_FUNC_I_TE_UART_TXD              = 136,  /*!<TE UART transmit data */
    ADI_APOLLO_FUNC_O_TE_UART_RXD              = 137,  /*!<TE UART receive data */
    //138, 139 not there in UG
    ADI_APOLLO_FUNC_JESD_PLL_LOST              = 140,  /*!<Serdes PLL Lost */
    ADI_APOLLO_FUNC_IRQ_CLOCK_WARNING          = 141,  /*!<Clock drift warning  */
    ADI_APOLLO_FUNC_ALL_IRQ_GOOD               = 142,  /*!<all_irq_good  */
    ADI_APOLLO_FUNC_IRQ_JRX_DATA_READY_A       = 143,  /*!<IRQ_JRx_DataReady_A  */
    ADI_APOLLO_FUNC_IRQ_JRX_DATA_READY_B       = 144   /*!<IRQ_JRx_DataReady_B  */    
} adi_apollo_gpio_func_e;

/*!
* \brief Enumerates Debug functions
*/
typedef enum {
    ADI_APOLLO_FUNC_I_HSDIN_DEBUG_A               = 0,   /*!<I_hsdin_debug_a signal */
    ADI_APOLLO_FUNC_I_LINX_DEBUG_A                = 1,   /*!<I_linx_debug_a signal */   
    ADI_APOLLO_FUNC_I_PFILT_DEBUG_A               = 2,   /*!<I_pfilt_debug_a */   
    ADI_APOLLO_FUNC_I_CDDC0_DEBUG_A               = 3,   /*!<I_cddc0_debug_a */   
    ADI_APOLLO_FUNC_I_CDDC1_DEBUG_A               = 4,   /*!<I_cddc1_debug_a */
    ADI_APOLLO_FUNC_I_FDDC_DEBUG_A                = 5,   /*!I_fddc_debug_a */
    ADI_APOLLO_FUNC_I_CFIR_DEBUG_A                = 6,   /*!I_cfir_debug_a */
    ADI_APOLLO_FUNC_I_JTX_DEBUG_A                 = 7,   /*!I_jtx_debug_a */
    ADI_APOLLO_FUNC_I_HSDIN_DEBUG_B               = 8,   /*!I_hsdin_debug_b */
    ADI_APOLLO_FUNC_I_LINX_DEBUG_B                = 9,   /*!I_linx_debug_b */
    ADI_APOLLO_FUNC_I_PFILT_DEBUG_B               = 10,  /*!<I_pfilt_debug_b */
    ADI_APOLLO_FUNC_I_CDDC0_DEBUG_B               = 11,  /*!<I_cddc0_debug_b */   
    ADI_APOLLO_FUNC_I_CDDC1_DEBUG_B               = 12,  /*!<I_cddc1_debug_b */
    ADI_APOLLO_FUNC_I_FDDC_DEBUG_B                = 13,  /*!I_fddc_debug_b */
    ADI_APOLLO_FUNC_I_CFIR_DEBUG_B                = 14,  /*!I_cfir_debug_b */
    ADI_APOLLO_FUNC_I_JTX_DEBUG_B                 = 15,  /*!I_jtx_debug_b*/ 
    ADI_APOLLO_FUNC_I_HSDOUT_LVDS_DEBUG_A_0       = 16,  /*!I_hsdout_lvds_debug_a[0]*/ 
    ADI_APOLLO_FUNC_I_HSDOUT_LVDS_DEBUG_A_1       = 17,  /*!I_hsdout_lvds_debug_a[1]*/ 
    ADI_APOLLO_FUNC_I_TX_LNX_DEBUG_A              = 18,  /*!I_tx_lnx_debug_a*/ 
    ADI_APOLLO_FUNC_I_TX_PFILT_DEBUG_A            = 19,  /*!I_tx_pfilt_debug_a*/
    ADI_APOLLO_FUNC_I_CDUC_DEBUG_A_0              = 20,  /*!I_cduc_debug_a[0]*/
    ADI_APOLLO_FUNC_I_CDUC_DEBUG_A_1              = 21,  /*!I_cduc_debug_a[1]*/
    ADI_APOLLO_FUNC_I_FDUC_DEBUG_A                = 22,  /*!I_fduc_debug_a*/
    ADI_APOLLO_FUNC_I_TX_CFIR_FSRC_DEBUG_A        = 23,  /*!I_tx_cfir_fsrc_debug_a*/
    ADI_APOLLO_FUNC_I_HSDOUT_LVDS_DEBUG_B_0       = 24,  /*!I_hsdout_lvds_debug_b[0]*/ 
    ADI_APOLLO_FUNC_I_HSDOUT_LVDS_DEBUG_B_1       = 25,  /*!I_hsdout_lvds_debug_b[1]*/ 
    ADI_APOLLO_FUNC_I_TX_LNX_DEBUG_B              = 26,  /*!I_tx_lnx_debug_b*/ 
    ADI_APOLLO_FUNC_I_TX_PFILT_DEBUG_B            = 27,  /*!I_tx_pfilt_debug_b*/
    ADI_APOLLO_FUNC_I_CDUC_DEBUG_B_0              = 28,  /*!I_cduc_debug_b[0]*/
    ADI_APOLLO_FUNC_I_CDUC_DEBUG_B_1              = 29,  /*!I_cduc_debug_b[1]*/
    ADI_APOLLO_FUNC_I_FDUC_DEBUG_B                = 30,  /*!I_fduc_debug_b*/
    ADI_APOLLO_FUNC_I_TX_CFIR_FSRC_DEBUG_B        = 31,  /*!I_tx_cfir_fsrc_debug_b*/
    ADI_APOLLO_FUNC_OTP_CLK                       = 32,  /*!OTP_CLK*/
    ADI_APOLLO_FUNC_RING_OSC_CLK_DIV2             = 33,  /*!Ring_osc_clk_div2*/
    ADI_APOLLO_FUNC_SD_DBG_CLKOUT_0P8_0           = 34,  /*!Sd_dbg_clkout_0p8[0]*/
    ADI_APOLLO_FUNC_SD_DBG_CLKOUT_0P8_1           = 35,  /*!Sd_dbg_clkout_0p8[1]*/
    ADI_APOLLO_FUNC_SD_DBG_CLKOUT_0P8_2           = 36,  /*!Sd_dbg_clkout_0p8[2]*/
    ADI_APOLLO_FUNC_ARM_CLK_DIV2                  = 37   /*!Arm_clk_div2*/
} adi_apollo_gpio_debug_func_e;

/*!
* \brief Enumerates GPIO Debug stages
*/
typedef enum {
    ADI_APOLLO_DEBUG_STAGE_1  = 1,       /*!CMOS Debug stage 1 */   
    ADI_APOLLO_DEBUG_STAGE_2  = 2,       /*!CMOS Debug stage 2 */
    ADI_APOLLO_DEBUG_STAGE_3  = 3        /*!CMOS Debug stage 3 */
} adi_apollo_gpio_cmos_debug_stage_e;

#endif /* __ADI_APOLLO_GPIO_TYPES_H__ */
/*! @} */
