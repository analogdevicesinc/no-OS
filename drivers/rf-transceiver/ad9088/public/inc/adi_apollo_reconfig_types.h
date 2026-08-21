/*!
 * \brief     RECONFIG Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_RECONFIG
 * @{
 */
#ifndef __ADI_APOLLO_RECONFIG_TYPES_H__
#define __ADI_APOLLO_RECONFIG_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/

/*!
* \brief Enumerates dynamic config selection
*/
typedef enum {
    ADI_APOLLO_RECONFIG_A0     = 0x1,    /*!< 4t4r FDRC A0*/
    ADI_APOLLO_RECONFIG_A1     = 0x2,    /*!< 4t4r FDRC A1*/
    ADI_APOLLO_RECONFIG_A2     = 0x4,    /*!< 4t4r FDRC A2*/
    ADI_APOLLO_RECONFIG_A3     = 0x8,    /*!< 4t4r FDRC A3*/
    ADI_APOLLO_RECONFIG_B0     = 0x10,   /*!< 4t4r FDRC B0*/
    ADI_APOLLO_RECONFIG_B1     = 0x20,   /*!< 4t4r FDRC B1*/
    ADI_APOLLO_RECONFIG_B2     = 0x40,   /*!< 4t4r FDRC B2*/
    ADI_APOLLO_RECONFIG_B3     = 0x80,   /*!< 4t4r FDRC B3*/
    ADI_APOLLO_RECONFIG_A0_A4  = 0x1,    /*!< 8t8r FDRC A0 + A4*/
    ADI_APOLLO_RECONFIG_A1_A5  = 0x2,    /*!< 8t8r FDRC A1 + A5*/
    ADI_APOLLO_RECONFIG_A2_A6  = 0x4,    /*!< 8t8r FDRC A2 + A6*/
    ADI_APOLLO_RECONFIG_A3_A7  = 0x8,    /*!< 8t8r FDRC A3 + A7*/
    ADI_APOLLO_RECONFIG_B0_B4  = 0x10,   /*!< 8t8r FDRC B0 + B4*/
    ADI_APOLLO_RECONFIG_B1_B5  = 0x20,   /*!< 8t8r FDRC B1 + B5*/
    ADI_APOLLO_RECONFIG_B2_B6  = 0x40,   /*!< 8t8r FDRC B2 + B6*/
    ADI_APOLLO_RECONFIG_B3_B7  = 0x80,   /*!< 8t8r FDRC B3 + B7*/
} adi_apollo_reconfig_select_e;

/*!
 * \brief Enumerates the reconfig controller input counter types
 */
typedef enum  {
    ADI_APOLLO_RECONFIG_EXTERNAL = 0,           /*!< External generated reconfig count (e.g. ext trig sync)*/
    ADI_APOLLO_RECONFIG_INTERNAL = 1,           /*!< Internal generated reconfig count (e.g. time stamp) */
    ADI_APOLLO_RECONFIG_TYPE_NUM = 2            /*!< No. of types */
} adi_apollo_reconfig_type_e;


/*!
* \brief RECONFIG CONTROL programming data
*/
typedef struct {
    uint8_t trig_reconfig_mode;         /*!< Trigger based Reconfiguration Mode
                                             1=> choose internal triggers  for reconfiguration after one sync event
                                             0=> Use external trigger (retimed to internal-sysref boundary) for resynchronization. >*/
    uint8_t cnco_reset;                 /*!< 1=>Reset CNCO  with next sync/trigger. Design Needs a risedge on this bit to enable the CNCO reset if in Tzero coherence mode. 0 =>Do not reset CNCO >*/
    uint8_t timestamp_reset_en;         /*!< 1=>Reset timestamp with next sync/trigger. Design Needs a risedge on this bit to enable the timestamp reset. 0 =>Do not reset timestamp >*/
    uint8_t resync_en;                  /*!< 1=> Forcefully enable resynchronization of all clocks at next sync/trigger(debug feature). 0 =>Resynchronization based on inputs >*/
    uint8_t tzero_coherence_en;         /*!< 1=> Coherence w.r.t Time0 enabled. 0 => Coherence w.r.t immediately preceding trigger/sync >*/
    uint8_t fnco_reset_en;              /*!< 1=>Reset FNCO  with next sync/trigger if tzero_coherence_en is disabled. 0 =>Do not reset FNCO >*/
    uint16_t prefsrc_lcm;               /*!< Decides the LCM value used in counter for PreFSRC Reconfiguration controller. This decides the time taken for reconfiguration,
                                             but needs to be programmed according to the decimation/interpolation modes. >*/
    uint16_t postfsrc_lcm;              /*!< Decides the LCM value used in counter for PostFSRC Reconfiguration controller. This decides the time taken for reconfiguration,
                                             but needs to be programmed according to the decimation/interpolation modes. >*/
} adi_apollo_reconfig_ctrl_pgm_t;

#endif /* __ADI_APOLLO_RECONFIG_TYPES_H__ */
/*! @} */
