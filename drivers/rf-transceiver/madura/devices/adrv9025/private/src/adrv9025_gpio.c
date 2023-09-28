// SPDX-License-Identifier: GPL-2.0
#include "adi_adrv9025_user.h"
#include "adi_adrv9025_gpio.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "adi_adrv9025.h"
#include "../../private/include/adrv9025_bf_core.h"
#include "../../private/include/adrv9025_gpio.h"
#include "../../private/include/adrv9025_bf_analog_tx_mem_map.h"
#include "../../private/include/adrv9025_bf_pll_mem_map.h"
#include "../../private/include/adrv9025_reg_addr_macros.h"
#include "../../private/include/adrv9025_bf_orx.h"

#ifdef _RELEASE_BUILD_
#line __LINE__ "adrv9025_gpio.c"
#endif

int32_t adrv9025_GpIntHandler(adi_adrv9025_Device_t*      device,
                              adi_adrv9025_gpIntStatus_t* gpIntStatus)
{
    UNUSED_PARA(device);
    int32_t               recoveryAction                               = ADI_COMMON_ACT_NO_ACTION;
    const char*           errMsg                                       = "No Interrupt Detected";
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_D_WATCHDOG_TIMEOUT  = 0x0010000000000000; /* bit52 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_D_CALIBRATION_ERROR = 0x0008000000000000; /* bit51 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_D_SYSTEM_ERROR      = 0x0004000000000000; /* bit50 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER11_IRQ          = 0x0002000000000000; /* bit49 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER10_IRQ          = 0x0001000000000000; /* bit48 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RF1PLL_UNLOCK           = 0x0000800000000000; /* bit47 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RF2PLL_UNLOCK           = 0x0000400000000000; /* bit46 */
    static const uint64_t ADI_ADRV9025_GP_MASK_AUXPLL_UNLOCK           = 0x0000200000000000; /* bit45 */
    static const uint64_t ADI_ADRV9025_GP_MASK_CLKPLL_UNLOCK           = 0x0000100000000000; /* bit44 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RF1PLL_CP_OVR_RANGE     = 0x0000080000000000; /* bit43 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RF2PLL_CP_OVR_RANGE     = 0x0000040000000000; /* bit42 */
    static const uint64_t ADI_ADRV9025_GP_MASK_AUXPLL_OVR_RANGE        = 0x0000020000000000; /* bit41 */
    static const uint64_t ADI_ADRV9025_GP_MASK_CLKPLL_OVR_RANGE        = 0x0000010000000000; /* bit40 */
    static const uint64_t ADI_ADRV9025_GP_MASK_SERDES_PLL_UNLOCK       = 0x0000008000000000; /* bit39 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER9_IRQ           = 0x0000004000000000; /* bit38 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER8_IRQ           = 0x0000002000000000; /* bit37 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER7_IRQ           = 0x0000001000000000; /* bit36 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER6_IRQ           = 0x0000000800000000; /* bit35 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER5_IRQ           = 0x0000000400000000; /* bit34 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER4_IRQ           = 0x0000000200000000; /* bit33 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER3_IRQ           = 0x0000000100000000; /* bit32 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER2_IRQ           = 0x0000000080000000; /* bit31 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER1_IRQ           = 0x0000000040000000; /* bit30 */
    static const uint64_t ADI_ADRV9025_GP_MASK_DEFRAMER0_IRQ           = 0x0000000020000000; /* bit29 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER8_IRQ             = 0x0000000010000000; /* bit28 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER7_IRQ             = 0x0000000008000000; /* bit27 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER6_IRQ             = 0x0000000004000000; /* bit26 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER5_IRQ             = 0x0000000002000000; /* bit25 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER4_IRQ             = 0x0000000001000000; /* bit24 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER3_IRQ             = 0x0000000000800000; /* bit23 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER2_IRQ             = 0x0000000000400000; /* bit22 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER1_IRQ             = 0x0000000000200000; /* bit21 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FRAMER0_IRQ             = 0x0000000000100000; /* bit20 */
    static const uint64_t ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX3_ERROR = 0x0000000000080000; /* bit19 */
    static const uint64_t ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX2_ERROR = 0x0000000000040000; /* bit18 */
    static const uint64_t ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX1_ERROR = 0x0000000000020000; /* bit17 */
    static const uint64_t ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX0_ERROR = 0x0000000000010000; /* bit16 */
    static const uint64_t ADI_ADRV9025_GP_MASK_FORCE_INTERRUPT         = 0x0000000000008000; /* bit15 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_C_WATCHDOG_TIMEOUT  = 0x0000000000004000; /* bit14 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_C_CALIBRATION_ERROR = 0x0000000000002000; /* bit13 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ARM_C_SYSTEM_ERROR      = 0x0000000000001000; /* bit12 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ORX1_STREAM_ERROR       = 0x0000000000000800; /* bit11 */
    static const uint64_t ADI_ADRV9025_GP_MASK_ORX0_STREAM_ERROR       = 0x0000000000000400; /* bit10 */
    static const uint64_t ADI_ADRV9025_GP_MASK_TX3_STREAM_ERROR        = 0x0000000000000200; /* bit09 */
    static const uint64_t ADI_ADRV9025_GP_MASK_TX2_STREAM_ERROR        = 0x0000000000000100; /* bit08 */
    static const uint64_t ADI_ADRV9025_GP_MASK_TX1_STREAM_ERROR        = 0x0000000000000080; /* bit07 */
    static const uint64_t ADI_ADRV9025_GP_MASK_TX0_STREAM_ERROR        = 0x0000000000000040; /* bit06 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RX3_STREAM_ERROR        = 0x0000000000000020; /* bit05 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RX2_STREAM_ERROR        = 0x0000000000000010; /* bit04 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RX1_STREAM_ERROR        = 0x0000000000000008; /* bit03 */
    static const uint64_t ADI_ADRV9025_GP_MASK_RX0_STREAM_ERROR        = 0x0000000000000004; /* bit02 */
    static const uint64_t ADI_ADRV9025_GP_MASK_CORE_STREAM_ERROR       = 0x0000000000000002; /* bit01 */
    static const uint64_t ADI_ADRV9025_GP_MASK_MEMORY_ECC_ERROR        = 0x0000000000000001; /* bit00 */

    /* test for RF1 PLL Unlock 47 */
    if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RF1PLL_UNLOCK) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_MODULE;
        errMsg         = "GP Interrupt from source RF1PLL Unlock";
    }

        /* test for RF2 PLL Unlock 46 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RF2PLL_UNLOCK) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_MODULE;
        errMsg         = "GP Interrupt from source RF2PLL Unlock";
    }

        /* test for AUX PLL Unlock 45 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_AUXPLL_UNLOCK) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_MODULE;
        errMsg         = "GP Interrupt from source AUXPLL Unlock";
    }

        /* test for ClK PLL Unlock 44 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_CLKPLL_UNLOCK) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source CLKPLL Unlock";
    }

        /* test for SERDES PLL Unlock 39 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_SERDES_PLL_UNLOCK) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source SERDESPLL Unlock";
    }

        /* test for Deframer11 IRQ 49 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER11_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ11 - JESD 204C link 1 CRC error";
    }

        /* test for Deframer10 IRQ 48 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER10_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ10 - JESD 204C link 1 sync loss";
    }

        /* test for Deframer9 IRQ 38 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER9_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ9 - JESD 204B link 1 QBD Error";
    }

        /* test for Deframer8 IRQ 37 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER8_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ8 - JESD Link 1 Sysref at wrong phase";
    }

        /* test for Deframer7 IRQ 36 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER7_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ7 - JESD Link 1 phase shift with respect to global LMFC";
    }

        /* test for Deframer6 IRQ 35 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER6_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ6 - JESD Link 1 Lane FIFO pointer error";
    }

        /* test for Deframer5 IRQ 34 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER5_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ5 - JESD 204C link 0 CRC error";
    }

        /* test for Deframer4 IRQ 33 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER4_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ4 - JESD 204C link 1 sync loss";
    }

        /* test for Deframer3 IRQ 32 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER3_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ3 - JESD 204B link 1 QBD Error";
    }

        /* test for Deframer2 IRQ 31 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER2_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ2 - JESD Link 0 Sysref at wrong phase";
    }

        /* test for Deframer1 IRQ bit 30 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER1_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Deframer IRQ1 - JESD Link 0 phase shift with respect to global LMFC";
    }

        /* test for Deframer0 IRQ bit 29 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_DEFRAMER0_IRQ) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_WARN_RESET_LOG;
        errMsg         = "GP Interrupt from source Deframer IRQ0 - JESD Link 0 Lane FIFO pointer error";
    }

        /* test for framer8 IRQ bit 28 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER8_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ8 - LMFC phase error Link2";
    }

        /* test for framer7 IRQ bit 27 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER7_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ7 - Sysref at wrong phase Link2";
    }

        /* test for framer6 IRQ bit 26 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER6_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ6 - Lane FIFO pointer error Link2";
    }

        /* test for framer5 IRQ bit 25 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER5_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ5 - LMFC phase error Link1";
    }

        /* test for framer4 IRQ bit 24 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER4_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ4 - Sysref at wrong phase Link1";
    }

        /* test for framer3 IRQ bit 23 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER3_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ3 - Lane FIFO pointer error Link1";
    }

        /* test for framer2 IRQ bit 22 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER2_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ2 - LMFC phase error Link0";
    }

        /* test for framer1 IRQ bit 21 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER1_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ1 - Sysref at wrong phase Link0";
    }

        /* test for framer0 IRQ bit 20 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FRAMER0_IRQ) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source Framer IRQ0 - Lane FIFO pointer error Link0";
    }

        /* test for ORX1 Stream Error bit 11 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ORX1_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source ORX1 Stream Error";
    }

        /* test for ORX0 Stream Error bit 10 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ORX0_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source ORX0 Stream Error";
    }

        /* test for TX3 Stream Error bit 9 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_TX3_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source TX3 Stream Error";
    }

        /* test for TX2 Stream Error bit 8 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_TX2_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source TX2 Stream Error";
    }

        /* test for TX1 Stream Error bit 7 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_TX1_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source TX1 Stream Error";
    }

        /* test for TX0 Stream Error bit 6 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_TX0_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source TX0 Stream Error";
    }

        /* test for RX3 Stream Error bit 5 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RX3_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source RX3 Stream Error";
    }

        /* test for RX2 Stream Error bit 4 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RX2_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source RX2 Stream Error";
    }

        /* test for RX1 Stream Error bit 3 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RX1_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source RX1 Stream Error";
    }

        /* test for RX0 Stream Error bit 2 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RX0_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source RX0 Stream Error";
    }

        /* test for CORE Stream Error bit 1 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_CORE_STREAM_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source CORE Stream Error";
    }

        /* test for PA Protection TX3 bit 19 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX3_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source PA Protection TX3 Error";
    }

        /* test for PA Protection TX2 bit 18 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX2_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_WARN_RESET_LOG;
        errMsg         = "GP Interrupt from source PA Protection TX2 Error";
    }

        /* test for PA Protection TX1 bit 17 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX1_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source PA Protection TX1 Error";
    }

        /* test for PA Protection TX0 bit 16 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_PA_PROTECTION_TX0_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source PA Protection TX0 Error";
    }

        /* test for Forced IRQ bit 15 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_FORCE_INTERRUPT) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source Forced IRQ";
    }

        /* test for ARM-C Watchdog Timeout bit 14 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_C_WATCHDOG_TIMEOUT) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source ARM-C WatchDog Time Out";
    }

        /* test for ARM-C Calibration Error bit 13 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_C_CALIBRATION_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_WARN_CHECK_TX_DATA;
        errMsg         = "GP Interrupt from source SRL violation";
    }

        /* test for ARM-C System Error bit 12 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_C_SYSTEM_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source ARM-C System Error";
    }

        /* test for ARM-D Watchdog Timeout bit 52 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_D_WATCHDOG_TIMEOUT) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "GP Interrupt from source ARM-D WatchDog Time Out";
    }

        /* test for ARM-D Calibration Error bit 51 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_D_CALIBRATION_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_WARN_RERUN_TRCK_CAL;
        errMsg         = "GP Interrupt from source ARM-D Calibration Error";
    }

        /* test for ARM-D System Error bit 50 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_ARM_D_SYSTEM_ERROR) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source ARM-D System Error";
    }

        /* test for RF1 PLL OverRange 43 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RF1PLL_CP_OVR_RANGE) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source RF1PLL OverRange";
    }

        /* test for RF2 PLL OverRange 42 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_RF2PLL_CP_OVR_RANGE) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source RF2PLL OverRange";
    }

        /* test for AUX PLL OverRange 41 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_AUXPLL_OVR_RANGE) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source AUXPLL OverRange";
    }

        /* test for CLK PLL OverRange 40 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_CLKPLL_OVR_RANGE) > 0)
    {
        recoveryAction = ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR;
        errMsg         = "GP Interrupt from source CLKPLL OverRange";
    }

        /* test for Memory ECC Error 0 */
    else if ((gpIntStatus->gp_Int_Active_Sources & ADI_ADRV9025_GP_MASK_MEMORY_ECC_ERROR) > 0)
    {
        recoveryAction = ADI_COMMON_ACT_ERR_RESET_FULL;
        errMsg         = "Memory ECC Error";
    }

    gpIntStatus->gp_Int_Recovery_Action = recoveryAction;
    gpIntStatus->gp_Int_Error_Message   = errMsg;

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_AuxDacCfgRangeCheck(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_AuxDacCfg_t* auxDacCfg)
{
    static const uint32_t ALL_AUXDAC_MASK = (uint32_t)ADI_ADRV9025_AUXDAC0 | (uint32_t)ADI_ADRV9025_AUXDAC1 |
    (uint32_t)ADI_ADRV9025_AUXDAC2 | (uint32_t)ADI_ADRV9025_AUXDAC3 |
    (uint32_t)ADI_ADRV9025_AUXDAC4 | (uint32_t)ADI_ADRV9025_AUXDAC5 |
    (uint32_t)ADI_ADRV9025_AUXDAC6 | (uint32_t)ADI_ADRV9025_AUXDAC7;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxDacCfg);

    /*Check that the requested AuxDAC is valid*/
    if (((auxDacCfg->auxDacMask & (~ALL_AUXDAC_MASK)) != 0) || auxDacCfg->auxDacMask == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxDacMask,
                         "Invalid AuxDAC index is selected for AuxDAC. Valid values are between 1-255.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

int32_t adrv9025_AuxDacCfgSet(adi_adrv9025_Device_t* device,
                              adi_adrv9025_AuxDacs_e auxDacSel)
{
    static const uint8_t                AUXDAC_DEFAULT_MODE = 0x01;
    int32_t                             recoveryAction      = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr            = ADRV9025_BF_ANALOG_TX_CH1;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if (((uint8_t)auxDacSel & 0x0F) == 0)
    {
        auxDacSel = (adi_adrv9025_AuxDacs_e)(((uint8_t)auxDacSel & 0xF0) >> 4);
        baseAddr  = ADRV9025_BF_ANALOG_TX_CH2;
    }

    if (ADRV9025_BF_EQUAL(auxDacSel,
                          ADI_ADRV9025_AUXDAC0))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac0ConfigBfSet(device,
                                                                   baseAddr,
                                                                   AUXDAC_DEFAULT_MODE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (ADRV9025_BF_EQUAL(auxDacSel,
                               ADI_ADRV9025_AUXDAC1))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac1ConfigBfSet(device,
                                                                   baseAddr,
                                                                   AUXDAC_DEFAULT_MODE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (ADRV9025_BF_EQUAL(auxDacSel,
                               ADI_ADRV9025_AUXDAC2))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac2ConfigBfSet(device,
                                                                   baseAddr,
                                                                   AUXDAC_DEFAULT_MODE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac3ConfigBfSet(device,
                                                                   baseAddr,
                                                                   AUXDAC_DEFAULT_MODE);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return recoveryAction;
}

int32_t adrv9025_AuxDacValueSet(adi_adrv9025_Device_t* device,
                                adi_adrv9025_AuxDacs_e auxDacSel,
                                uint16_t               dacWord)
{
    static const uint16_t               MAX_DAC_WORD   = 4095;
    int32_t                             recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfAnalogTxMemMapChanAddr_e baseAddr       = ADRV9025_BF_ANALOG_TX_CH1;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if (dacWord > MAX_DAC_WORD)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dacWord,
                         "Requested analog voltage is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (((uint8_t)auxDacSel & 0x0F) == 0)
    {
        auxDacSel = (adi_adrv9025_AuxDacs_e)(((uint8_t)auxDacSel & 0xF0) >> 4);
        baseAddr  = ADRV9025_BF_ANALOG_TX_CH2;
    }

    if (ADRV9025_BF_EQUAL(auxDacSel,
                          ADI_ADRV9025_AUXDAC0))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac0BfSet(device,
                                                             baseAddr,
                                                             dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (ADRV9025_BF_EQUAL(auxDacSel,
                               ADI_ADRV9025_AUXDAC1))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac1BfSet(device,
                                                             baseAddr,
                                                             dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (ADRV9025_BF_EQUAL(auxDacSel,
                               ADI_ADRV9025_AUXDAC2))
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac2BfSet(device,
                                                             baseAddr,
                                                             dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        recoveryAction = adrv9025_AnalogTxMemMapAuxdac3BfSet(device,
                                                             baseAddr,
                                                             dacWord);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         NULL,
                         device->common.error.errormessage);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet(device,
                                                               baseAddr,
                                                               0x01);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while setting latch enable bit");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_AnalogTxMemMapAuxdacLatchEnBfSet(device,
                                                               baseAddr,
                                                               0x00);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while clearing latch enable bit");

    return device->common.error.newAction;
}

int32_t adrv9025_AuxAdcCfgRangeCheck(adi_adrv9025_Device_t*    device,
                                     adi_adrv9025_AuxAdcCfg_t* auxAdcConfig)
{
    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcConfig);

    /* Range check the enable */
    if ((auxAdcConfig->auxAdcEnable < ADI_ADRV9025_AUXADC_DISABLE) ||
        (auxAdcConfig->auxAdcEnable >= ADI_ADRV9025_AUXADC_ENABLE_NUM))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcConfig->auxAdcEnable,
                         "auxAdcEnable is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check the ADC selection */
    if ((auxAdcConfig->auxAdcSelect < ADI_ADRV9025_AUXADC_A) ||
        (auxAdcConfig->auxAdcSelect >= ADI_ADRV9025_AUXADC_NUM))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcConfig->auxAdcSelect,
                         "auxAdcSelect is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check ADC input selection */
    if ((auxAdcConfig->auxAdcInputSelect != ADI_ADRV9025_AUXADC_INPUT_0) &&
        (auxAdcConfig->auxAdcInputSelect != ADI_ADRV9025_AUXADC_INPUT_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcConfig->auxAdcInputSelect,
                         "auxAdcInputSelect is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check the clock divide setting */
    if ((auxAdcConfig->auxAdcClkDivide < ADI_ADRV9025_AUXADC_CLKDIVIDE_32) ||
        (auxAdcConfig->auxAdcClkDivide >= ADI_ADRV9025_AUXADC_CLKDIVIDE_NUM_DIVIDE))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcConfig->auxAdcClkDivide,
                         "auxAdcClkDivide is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    return device->common.error.newAction;
}

int32_t adrv9025_AuxAdcCfgRead(adi_adrv9025_Device_t*      device,
                               adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                               adi_adrv9025_AuxAdcCfg_t*   auxAdcConfig)
{
    int32_t                  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    adrv9025_BfOrxChanAddr_e baseAddr       = ADRV9025_BF_ORX_CH0;
    uint8_t                  readValue      = 0; /* bit field read back */

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcConfig);

    switch (auxAdcSelect)
    {
    case ADI_ADRV9025_AUXADC_A:
        baseAddr = ADRV9025_BF_ORX_CH0;
        break;
    case ADI_ADRV9025_AUXADC_B:
        baseAddr = ADRV9025_BF_ORX_CH1;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcSelect,
                         "auxAdcSelect is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }

    /* update configuration structure with the selected ADC */
    auxAdcConfig->auxAdcSelect = auxAdcSelect;

    /* read the aux ADC input selection */
    recoveryAction = adrv9025_OrxAuxAdcSelBfGet(device,
                                                baseAddr,
                                                &readValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC input selection control register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if ((readValue != ADI_ADRV9025_AUXADC_INPUT_0) &&
        (readValue != ADI_ADRV9025_AUXADC_INPUT_1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         readValue,
                         "Read back invalid or default aux ADC input selection.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        auxAdcConfig->auxAdcInputSelect = (adi_adrv9025_AuxAdcInputSelect_e)readValue;
    }

    /* update enable flag based only on the power down bit */
    recoveryAction = adrv9025_OrxAuxAdcPdBfGet(device,
                                               baseAddr,
                                               &readValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC power down control register");
    ADI_ERROR_RETURN(device->common.error.newAction);
    auxAdcConfig->auxAdcEnable = (adi_adrv9025_AuxAdcEnable_e)(readValue == 0x1)
                                     ? ADI_ADRV9025_AUXADC_DISABLE
                                     : ADI_ADRV9025_AUXADC_ENABLE;

    switch (auxAdcSelect)
    {
    case ADI_ADRV9025_AUXADC_A:
        recoveryAction = adrv9025_CoreAuxAdcClkDivide1BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &readValue);
        break;
    case ADI_ADRV9025_AUXADC_B:
        recoveryAction = adrv9025_CoreAuxAdcClkDivide2BfGet(device,
                                                            ADRV9025_BF_CORE,
                                                            &readValue);
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         auxAdcSelect,
                         "auxAdcSelect is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
        break;
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC clk divide register");
    ADI_ERROR_RETURN(device->common.error.newAction);
    auxAdcConfig->auxAdcClkDivide = (adi_adrv9025_AuxAdcClkDivide_e)readValue;

    return device->common.error.newAction;
}

int32_t adrv9025_AuxAdcCfgWrite(adi_adrv9025_Device_t*    device,
                                adi_adrv9025_AuxAdcCfg_t* auxAdcConfig)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    /* bit field write variable */
    uint8_t                  powerDownValue = 0x0;
    adrv9025_BfOrxChanAddr_e baseAddr       = ADRV9025_BF_ORX_CH0;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcConfig);

    /* auxAdcSelect is already range checked prior to calling this function */
    baseAddr = (auxAdcConfig->auxAdcSelect == ADI_ADRV9025_AUXADC_A)
                   ? ADRV9025_BF_ORX_CH0
                   : ADRV9025_BF_ORX_CH1;

    /* hold ADC in reset while it's being configured */
    recoveryAction = adrv9025_OrxAuxAdcResetBfSet(device,
                                                  baseAddr,
                                                  0x1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux ADC reset register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* configure the ADC input */
    recoveryAction = adrv9025_OrxAuxAdcSelBfSet(device,
                                                baseAddr,
                                                auxAdcConfig->auxAdcInputSelect);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     auxAdcConfig->auxAdcInputSelect,
                     "Error while attempting to write aux ADC input register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* set the decimation rate. Only one mode is available to the user which is
     * mode zero as it's the most accurate */
    recoveryAction = adrv9025_OrxAuxAdcDecimationCtlBfSet(device,
                                                          baseAddr,
                                                          0x0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux ADC decimation register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* configure clock power down */
    powerDownValue = (auxAdcConfig->auxAdcEnable == ADI_ADRV9025_AUXADC_ENABLE)
                         ? 0x0
                         : 0x1;

    if (auxAdcConfig->auxAdcSelect == ADI_ADRV9025_AUXADC_A)
    {
        /* configure clock divide */
        recoveryAction = adrv9025_CoreAuxAdcClkDivide1BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            auxAdcConfig->auxAdcClkDivide);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         auxAdcConfig->auxAdcClkDivide,
                         "Error while attempting to write aux ADC A clk divide register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        recoveryAction = adrv9025_CoreAuxAdcClkPd1BfSet(device,
                                                        ADRV9025_BF_CORE,
                                                        powerDownValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         auxAdcConfig->auxAdcEnable,
                         "Error while attempting to write aux ADC A clk power down register");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else
    {
        /* configure clock divide */
        recoveryAction = adrv9025_CoreAuxAdcClkDivide2BfSet(device,
                                                            ADRV9025_BF_CORE,
                                                            auxAdcConfig->auxAdcClkDivide);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         auxAdcConfig->auxAdcClkDivide,
                         "Error while attempting to write aux ADC B clk divide register");
        ADI_ERROR_RETURN(device->common.error.newAction);

        /* configure clock power down */
        recoveryAction = adrv9025_CoreAuxAdcClkPd2BfSet(device,
                                                        ADRV9025_BF_CORE,
                                                        powerDownValue);
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         device->common.error.errCode,
                         recoveryAction,
                         auxAdcConfig->auxAdcEnable,
                         "Error while attempting to write aux ADC B clk power down register");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* set the aux ADC's reset bit based on the value of the supplied enable */
    recoveryAction = adrv9025_OrxAuxAdcResetBfSet(device,
                                                  baseAddr,
                                                  powerDownValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     auxAdcConfig->auxAdcEnable,
                     "Error while attempting to write aux ADC reset register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* set the aux ADC's power down bit based on the value of the supplied enable */
    recoveryAction = adrv9025_OrxAuxAdcPdBfSet(device,
                                               baseAddr,
                                               powerDownValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     auxAdcConfig->auxAdcEnable,
                     "Error while attempting to write aux ADC power down register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adrv9025_AuxAdcMeasurementStart(adi_adrv9025_Device_t*      device,
                                        adi_adrv9025_AuxAdcSelect_e auxAdcSelect)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    adrv9025_BfOrxChanAddr_e baseAddr = ADRV9025_BF_ORX_CH1;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    if (auxAdcSelect == ADI_ADRV9025_AUXADC_A)
    {
        baseAddr = ADRV9025_BF_ORX_CH0;
    }
    else
    {
        baseAddr = ADRV9025_BF_ORX_CH1;
    }

    /* toggle the reset bit */
    recoveryAction = adrv9025_OrxAuxAdcResetBfSet(device,
                                                  baseAddr,
                                                  0x1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux ADC reset register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    recoveryAction = adrv9025_OrxAuxAdcResetBfSet(device,
                                                  baseAddr,
                                                  0x0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux ADC reset register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    return device->common.error.newAction;
}

int32_t adrv9025_AuxAdcValueGet(adi_adrv9025_Device_t*      device,
                                adi_adrv9025_AuxAdcSelect_e auxAdcSelect,
                                adi_adrv9025_AuxAdcValue_t* auxAdcValue)
{
    int32_t  recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint16_t readValue      = 0u;
    uint8_t  readyValue     = 0u;
    /* auxAdcSelect is already range checked prior to calling this function */
    adrv9025_BfOrxChanAddr_e       baseAddr    = ADRV9025_BF_ORX_CH1;
    adrv9025_BfPllMemMapChanAddr_e pllBaseAddr = ADRV9025_BF_RF2_PLL;

    /* Check device pointer is not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        auxAdcValue);

    if (auxAdcSelect == ADI_ADRV9025_AUXADC_A)
    {
        baseAddr    = ADRV9025_BF_ORX_CH0;
        pllBaseAddr = ADRV9025_BF_RF1_PLL;
    }
    else
    {
        baseAddr    = ADRV9025_BF_ORX_CH1;
        pllBaseAddr = ADRV9025_BF_RF2_PLL;
    }

    /* Clear the readback lock */
    recoveryAction = adrv9025_OrxAuxAdcReadbackLockBfSet(device,
                                                         baseAddr,
                                                         0);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux adc readback lock register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Check the ready bit */
    recoveryAction = adrv9025_PllMemMapAuxAdcReadyBfGet(device,
                                                        pllBaseAddr,
                                                        &readyValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC ready register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    if (readyValue == 0u)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_WARN_RERUN_FEATURE,
                         readyValue,
                         "Aux adc is not ready");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* lock the readback lock */
    recoveryAction = adrv9025_OrxAuxAdcReadbackLockBfSet(device,
                                                         baseAddr,
                                                         1);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to write aux adc readback lock register");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Read the ADC's sample data */
    recoveryAction = adrv9025_OrxAuxAdcReadBfGet(device,
                                                 baseAddr,
                                                 &readValue);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_API,
                     device->common.error.errCode,
                     recoveryAction,
                     NULL,
                     "Error while attempting to read aux ADC sample registers");
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* populate the supplied auxAdcValue structure */
    auxAdcValue->auxAdcSelect = auxAdcSelect;
    auxAdcValue->auxAdcValue  = readValue;

    return device->common.error.newAction;
}
