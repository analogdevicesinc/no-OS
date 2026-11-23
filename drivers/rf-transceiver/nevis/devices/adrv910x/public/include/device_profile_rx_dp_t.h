/*!
 * @brief   Rx datapath data types.
 *
 * @details This file provides data type definitions for Rx datapath.
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_RX_DP_T_HEADER__
#define __DEVICE_PROFILE_RX_DP_T_HEADER__

#include "adi_device_profile_pack.h"
// #include "nvs_ahb_addr_redf.h"
#include "device_profile_nco_drv_t.h"
#include "device_profile_phase_sync_nco_t.h"
#include "device_profile_rxnb_dem_t.h"

/*!
 *  @addtogroup rx_datapath
 *  @ingroup drivers
 *  @{
 */



/*! enumerations of the SINC_MUX options */
typedef enum  {

    /* DO NOT modify the value of the enum as it matches the actual
     * register value */

    RX_SINC_MUX5_OUTPUT_ZERO     = 0u,     /*!< Output zero   */
    RX_SINC_MUX5_OUTPUT_SINC3    = 1u,     /*!< Select SINC3 for the output     */
    RX_SINC_MUX5_OUTPUT_SINC2    = 2u,     /*!< Select SINC4 for the output     */
} ADI_NEVIS_PACK_ENUM rxSincMux5Output_e;

/*! enumerations of the SINC_MUX options */
typedef enum  {

    /* DO NOT modify the value of the enum as it matches the actual
     * register value */

    RXNB_SINC_MUX5_OUTPUT_ZERO     = 0u,     /*!< Output zero   */
    RXNB_SINC_MUX5_OUTPUT_SINC2    = 1u,     /*!< Select SINC2 for the output     */
    RXNB_SINC_MUX5_OUTPUT_SINC3    = 2u,     /*!< Select SINC3 for the output     */
    RXNB_SINC_MUX5_OUTPUT_SINC4    = 4u,     /*!< Select SINC4 for the output     */
} ADI_NEVIS_PACK_ENUM rxNbSincMux4Output_e;

/*! enumerations of the SINC gain settings */
typedef enum {

    /* DO NOT modify the value of the enum as it matches the actual
     * register value */

    RX_SINC_GAIN_MUX_0_DB        = 0u,     /*!< 0 dB   */
    RX_SINC_GAIN_MUX_6_DB        = 1u,     /*!< 6 dB   */
    RX_SINC_GAIN_MUX_12_DB       = 2u,     /*!< 12 dB   */
    RX_SINC_GAIN_MUX_NEG_6_DB    = 3u,     /*!< -6 dB   */
    RX_SINC_GAIN_MUX_NEG_12_DB   = 4u,     /*!< -12 dB   */
} ADI_NEVIS_PACK_ENUM rxSincGainMuxOutput_e;

/*! enumerations of the HB_MUX options */
typedef enum  {

    /* DO NOT modify the value of the enum as it matches the actual
     * register value */

    RX_HB_MUX_BYPASS       = 0u,     /*!< BYPASS   */
//    RX_HB_MUX_OUTPUT_HB0        = 1u,     /*!< Nevis does not have?. Select HB0 for the output     */
    RX_HB_MUX_OUTPUT_HB2        = 2u,     /*!< Select HB2 for the output     */
    RX_HB_MUX_OUTPUT_HB1        = 4u,     /*!< Select HB1 for the toutput    */

} ADI_NEVIS_PACK_ENUM rxHBMuxOutput_e;

/*! enumerations of the NB HB_MUX options */
typedef enum  {

    /* DO NOT modify the value of the enum as it matches the actual
     * register value */

    RXNB_HB_MUX_BYPASS       = 0u,     /*!< Output zero   */
    RXNB_HB_MUX_OUTPUT_HB1        = 1u,     /*!<  HB1 for the output     */
} ADI_NEVIS_PACK_ENUM rxNbHBMuxOutput_e;

/*! enumerations for RX/ORX profile activation */
//typedef enum {
//    RX_SELECT  = 0u,
//    ORX_SELECT = 1u,
//} ADI_NEVIS_PACK_ENUM rxDpProfileSelect_e;
//
//typedef enum {
//    ORX_PROFILE_SELECT = 0u,
//    ILB_PROFILE_SELECT = 1u,
//    ELB_PROFILE_SELECT = 2u,
//} ADI_NEVIS_PACK_ENUM orxDpProfileSelect_e;

/**
*  \brief Enum to select the type of gain table loaded during ADRV910X initialization
*/
typedef enum {
    RX_GAIN_CORRECTION_TABLE   = 0,   /*!< Gain table to use digital gain to adjust for coarse analog gain steps and maintain a constant gain */
    RX_GAIN_COMPENSATION_TABLE = 1    /*!< Gain table to adjust digital gain when analog gain changes to maintain a constant gain*/
} ADI_NEVIS_PACK_ENUM rxGainTableType_e;


#define NUM_GAIN_COMP9_VALUES  (6u)
   
                                 

/*! Contains decimation settings for rxnb_dec_top block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t scicBlk23En;          /*!< Setting for block #23. RSCIC */
    uint8_t scicBlk23DivFactor;   /*!< RSCIC division factor. This parameter records
                                    the actual division factor in the hardware which
                                    is 2-10. When writing to the SPI register, the 
                                    value is mapped to 1-9, and the mapping is taken 
                                    care of by the driver */ 
    uint8_t scicBlk23LowRippleEn; /*!< Enable low ripple mode */

    uint8_t decBy2Blk35En;     /*!< Setting for decimator block #35, decimation factor by 2 */
    uint8_t decBy2Blk37En;     /*!< Setting for decimator block #37, decimation factor by 2 */
    uint8_t decBy2Blk39En;     /*!< Setting for decimator block #39, decimation factor by 2 */
    uint8_t decBy2Blk41En;     /*!< Setting for decimator block #41, decimation factor by 2 */
    uint8_t decBy2Blk43En;     /*!< Setting for decimator block #43, decimation factor by 2 */
    uint8_t decBy3Blk45En;     /*!< Setting for decimator block #45, decimation factor by 3 */
    uint8_t decBy2Blk47En;     /*!< Setting for decimator block #47, decimation factor by 2 */
    uint8_t padding[2u];
} rxNbDecTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains decimation settings for rxnb_dec_top block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t scicEn;          /*!< Setting for block #23. RSCIC */
    uint8_t scicDivFactor;   /*!< RSCIC division factor. This parameter records
                                    the actual division factor in the hardware which
                                    is 2-10. When writing to the SPI register, the 
                                    value is mapped to 1-9, and the mapping is taken 
                                    care of by the driver */ 
    uint8_t rxDpScicControl; /*!< Enable low ripple mode */
    
    uint8_t decBy5Blk14En;     /*!< Setting for decimator block #14, decimation factor by 5 */
    uint8_t decBy5Blk16En;     /*!< Setting for decimator block #16, decimation factor by 5 */
    uint8_t decBy3Blk18En;     /*!< Setting for decimator block #18, decimation factor by 3 */
    uint8_t decBy2Blk20En;     /*!< Setting for decimator block #20, decimation factor by 2 */
    uint8_t decBy2Blk22En;     /*!< Setting for decimator block #22, decimation factor by 2 */
    uint8_t decBy2Blk24En;     /*!< Setting for decimator block #24, decimation factor by 2 */
    uint8_t decBy2Blk26En;     /*!< Setting for decimator block #26, decimation factor by 2 */
    uint8_t decBy2Blk27En;     /*!< Setting for decimator block #27, decimation factor by 2 */
    uint8_t decBy2Blk29En;     /*!< Setting for decimator block #29, decimation factor by 2 */

//    uint8_t padding[2u];
} rxNbDecTopNb_t;
ADI_NEVIS_PACK_FINISH

/*! Contains decimation settings for rxwb_dec_top block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t decBy2Blk25En;     /*!< Setting for decimator block #25. decimation factor by 2*/
    uint8_t decBy2Blk27En;     /*!< Setting for decimator block #27. decimation factor by 2 */
    uint8_t decBy2Blk29En;     /*!< Setting for decimator block #29. decimation factor by 2 */
    uint8_t decBy2Blk31En;     /*!< Setting for decimator block #31. decimation factor by 2 */
    uint8_t decBy2Blk33En;     /*!< Setting for decimator block #33. decimation factor by 2 */
    uint8_t wbLpfBlk33p1En;    /*!< Setting for decimator block #33.1. WBLPF decimation factor by 2 */
    uint8_t padding[2u];
} rxWbDecTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains decimation settings for rx_dec_top block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t decBy3Blk15En;         /*!< DEC3_Mux16: 0=bypass, 1=DEC3_15_out */
    uint8_t decBy3Blk15_1En;       /*!< DEC3_Mux16: 0=bypass, 1=DEC3_15_1_out (applicable if rx_dp_dec3_15_en=0 ) */    
    uint8_t decBy2Hb3Blk17p1En;    /*!< Setting for block #17.1. HB3 */
    uint8_t decBy2Hb4Blk17p2En;    /*!< Setting for block #17.2. HB4 */
    uint8_t decBy2Hb5Blk19p1En;    /*!< Setting for block #19.1. HB5 */
    uint8_t decBy2Hb6Blk19p2En;    /*!< Setting for block #19.2. HB6 */  
    uint8_t padding[2u];    
} rxDecTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for txnb_int_top block */
ADI_NEVIS_PACK_START
typedef struct{

    rxSincGainMuxOutput_e sincGainMux;        /*!< Sinc gain setting          */
    rxSincMux5Output_e    sincMux;            /*!< Sinc MUX selection         */
    rxHBMuxOutput_e       hbMux;              /*!< HB MUX selection           */
    uint8_t               isGainCompEnabled;  /*!< Gain compensation          */
    int16_t               gainComp9GainI[NUM_GAIN_COMP9_VALUES]; /*!< Gain compensation value in S1.14 fixed point format */
    int16_t               gainComp9GainQ[NUM_GAIN_COMP9_VALUES]; /*!< Gain compensation value in S1.14 fixed point format */

} rxSincHbTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for txnb_int_top block */
ADI_NEVIS_PACK_START
typedef struct{
    rxSincGainMuxOutput_e sincGainMux;        /*!< Sinc gain setting          */
    rxNbSincMux4Output_e    sincMux;            /*!< Sinc MUX selection         */
    rxNbHBMuxOutput_e       hbMux;              /*!< HB MUX selection           */
    uint8_t               isGainCompEnabled;  /*!< Gain compensation          */
    int16_t               gainComp9GainI[NUM_GAIN_COMP9_VALUES]; /*!< Gain compensation value in S1.14 fixed point format */
    int16_t               gainComp9GainQ[NUM_GAIN_COMP9_VALUES]; /*!< Gain compensation value in S1.14 fixed point format */
} rxNbSincHbTop_t;
ADI_NEVIS_PACK_FINISH

/*! RX Channel profile data structure */
ADI_NEVIS_PACK_START
typedef struct {
    rxNbDecTop_t  nbDecTop;    /*!< rxnb_dec_top block */
    rxWbDecTop_t  wbDecTop;    /*!< rxwb_dec_top block */
    rxDecTop_t    decTop;      /*!< rx_dec_top block   */
    rxSincHbTop_t sincHbTop;   /*!< rx sinc hb block   */
    rxnbDemConfig_t rxNbDem;   /*!< rx nb dem block    */ 
}rxDpProfile_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct {
    rxNbDecTopNb_t      nbDecTopNb;    /*!< rxnb_dec_top new block for NB path*/
    rxNbSincHbTop_t     sincHbTopNb;   /*!< rx sinc hb block   */
    rxnbDemConfigNb_t   rxNbDemNb;   /*!< rx nb dem block in NB path   */ 
}rxNbDpProfile_t;
ADI_NEVIS_PACK_FINISH

/*! @} */
#endif
