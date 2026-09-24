/*!
 * \brief     Trigger and Timestamp functional block type definitions header
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TRIGGER_TS
 * @{
 */
#ifndef __ADI_APOLLO_TRIGGER_TS_TYPES_H__
#define __ADI_APOLLO_TRIGGER_TS_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
/*!
* \brief Enumerates TRIGGER MASTER selection for both Side A and B
*/
typedef enum {
    ADI_APOLLO_TRIG_MST_CNCO_0       = 0x00000001,      
    ADI_APOLLO_TRIG_MST_CNCO_2       = 0x00000002,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_CNCO_1       = 0x00000004,
    ADI_APOLLO_TRIG_MST_CNCO_3       = 0x00000008,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_FNCO_0       = 0x00000010,
    ADI_APOLLO_TRIG_MST_FNCO_4       = 0x00000020,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_FNCO_1       = 0x00000040,
    ADI_APOLLO_TRIG_MST_FNCO_5       = 0x00000080,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_FNCO_2       = 0x00000100,
    ADI_APOLLO_TRIG_MST_FNCO_6       = 0x00000200,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_FNCO_3       = 0x00000400,
    ADI_APOLLO_TRIG_MST_FNCO_7       = 0x00000800,      /*! 8T8R mode */
    ADI_APOLLO_TRIG_MST_BMEM0        = 0x00001000,      /*! Rx only */
    ADI_APOLLO_TRIG_MST_BMEM1        = 0x00004000,      /*! Rx only */
    ADI_APOLLO_TRIG_MST_LINEARX      = 0x00010000,
    ADI_APOLLO_TRIG_MST_RECONFIG     = 0x00040000,
    ADI_APOLLO_TRIG_MST_PFILT0       = 0x00080000,
    ADI_APOLLO_TRIG_MST_PFILT1       = 0x00100000,
    ADI_APOLLO_TRIG_MST_CFIR0        = 0x00200000,
    ADI_APOLLO_TRIG_MST_CFIR1        = 0x00400000
} adi_apollo_trig_mst_sel_e; 

/*!
* \brief Enumerates the output of trigger selection mux
*/
typedef enum {
    ADI_APOLLO_TRIG_INPUT    =  0,  
    ADI_APOLLO_TRIG_SPI      =  1,
    ADI_APOLLO_TRIG_MASTER   =  2
} adi_apollo_trigger_sel_mux_e;

/*!
* \brief Enumerates trigger mute selection
*/
typedef enum {
    ADI_APOLLO_TRIG_MUTE_ENABLE    =  1,              /*! Mute out the Trigger Master output */
    ADI_APOLLO_TRIG_MUTE_DISABLE   =  0
} adi_apollo_trig_mute_sel_e;

/*!
* \brief Enumerates trigger mute selection
*/
typedef enum {
    ADI_APOLLO_TRIG_MUTE_MASK_DISABLE    =  0,   /*! Disable mute counter mask */
    ADI_APOLLO_TRIG_MUTE_MASK_0          =  1,   /*! Mute trigger after count in mask 0 */
    ADI_APOLLO_TRIG_MUTE_MASK_1          =  2    /*! Mute trigger after count in mask 1 */
} adi_apollo_trig_mute_mask_e;

/*!
* \brief Enumerates trigger Enable / Disable selection
*/
typedef enum {
    ADI_APOLLO_TRIG_ENABLE    =  1,                  /*! Enable trigger */
    ADI_APOLLO_TRIG_DISABLE   =  0                   /*! Disable trigger */
} adi_apollo_trig_enable_sel_e;

/*!
* \brief Enumerates timestamp reset mode
*/
typedef enum
{
    ADI_APOLLO_TRIG_TS_RESET_MODE_SYSREF = 0,       /*! SYSREF based reset */
    ADI_APOLLO_TRIG_TS_RESET_MODE_SPI    = 1        /*! SPI based reset */
} adi_apollo_trig_ts_reset_mode_e;

/*!
 * \brief Trigger Master Config Structure
 */
typedef struct {
    uint64_t trig_period;                        /*! Cycles at sampling frequency between triggers */
    uint64_t trig_offset;                        /*! Min. of 31 for sysref reset, 32 for SPI reset */
    adi_apollo_trig_enable_sel_e trig_enable;
} adi_apollo_trig_mst_config_t; 

#endif /* __ADI_APOLLO_TRIGGER_TS_TYPES_H__ */
/*! @} */
