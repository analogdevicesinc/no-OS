/*!  
 * \brief     BMEM Block definition headers 
 * 
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_BMEM
 * @{
 */
#ifndef __ADI_APOLLO_BMEM_TYPES_H__
#define __ADI_APOLLO_BMEM_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_BMEM_NUM 8
#define ADI_APOLLO_BMEM_PER_SIDE 4
#define ADI_APOLLO_BMEM_HOP_PROFILES 4
#define ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_BYTES (128*1024)
#define ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_WORDS (ADI_APOLLO_BMEM_HSDIN_MEM_SIZE_BYTES/4)
#define ADI_APOLLO_BMEM_CDDC_MEM_SIZE_BYTES (16*1024)
#define ADI_APOLLO_BMEM_CDDC_MEM_SIZE_WORDS (ADI_APOLLO_BMEM_CDDC_MEM_SIZE_BYTES/4)
#define ADI_APOLLO_BMEM_FDDC_MEM_SIZE_BYTES (2*1024)
#define ADI_APOLLO_BMEM_FDDC_MEM_SIZE_WORDS (ADI_APOLLO_BMEM_FDDC_MEM_SIZE_BYTES/4)

/*!
* \brief Enumerates BMEM selection
*/

typedef enum {
    ADI_APOLLO_BMEM_NONE   = 0x00,                        /*!< No BMEM */
    ADI_APOLLO_BMEM_A0     = 0x01,                        /*!< BMEM 0 of SIDE A */
    ADI_APOLLO_BMEM_A1     = 0x02,                        /*!< BMEM 1 of SIDE A */
    ADI_APOLLO_BMEM_A2     = 0x04,                        /*!< BMEM 2 of SIDE A */
    ADI_APOLLO_BMEM_A3     = 0x08,                        /*!< BMEM 3 of SIDE A */
    ADI_APOLLO_BMEM_B0     = 0x10,                        /*!< BMEM 0 of SIDE B */
    ADI_APOLLO_BMEM_B1     = 0x20,                        /*!< BMEM 1 of SIDE B */
    ADI_APOLLO_BMEM_B2     = 0x40,                        /*!< BMEM 2 of SIDE B */
    ADI_APOLLO_BMEM_B3     = 0x80,                        /*!< BMEM 3 of SIDE B */
    ADI_APOLLO_BMEM_ALL    = 0xFF,                        /*!< All BMEM */
    ADI_APOLLO_BMEM_ALL_4T4R = 0xFF,                      /*!< All BMEM (4T4R) */
} adi_apollo_bmem_sel_e;

/*!
* \brief Enumerates BMEM locations
*/

typedef enum {
    ADI_APOLLO_BMEM_HSDIN  = 0, /*!< HSDIN/ADC BMEM */
    ADI_APOLLO_BMEM_CDDC = 1, /*!< Coarse BMEM */
    ADI_APOLLO_BMEM_FDDC   = 2, /*!< Fine BMEM */
} adi_apollo_bmem_loc_e;

/*!
* \brief BMEM Capture mode configuration data
*/
typedef struct {
    uint8_t sample_size;                       /*!< 0: NORMAL 16b, 1: DEBUG 32b */
    uint8_t ramclk_ph_dis;                     /*!< Set 1 to Disable SRAM Clock Phasing. */
    uint16_t st_addr_cpt;                      /*!< Start Address of SRAM in Capture Mode */
    uint16_t end_addr_cpt;                     /*!< End Address of SRAM in Capture Mode */
    uint8_t parity_check_en;                   /*!< Set 1 to Enable Parity Check on Data Read from SRAM. */
} adi_apollo_bmem_capture_t;

/*!
* \brief BMEM AWG mode configuration data
*/
typedef struct {
    uint8_t sample_size;                       /*!< 0: NORMAL 16b, 1: DEBUG 32b */
    uint8_t ramclk_ph_dis;                     /*!< Set 1 to Disable SRAM Clock Phasing. */
    uint8_t parity_check_en;                   /*!< Set 1 to Enable Parity Check on Data Read from SRAM. */
} adi_apollo_bmem_awg_t;

/*!
* \brief BMEM delay mode configuration data
*/
typedef struct {
    uint8_t sample_size;                                    /*!< 0: NORMAL 16b, 1: DEBUG 32b */
    uint8_t ramclk_ph_dis;                                  /*!< Set 1 to Disable SRAM Clock Phasing. */
	uint16_t sample_delay;                                  /*!< Only used if trig_mode == 0 */       
	uint8_t parity_check_en;                                /*!< Set 1 to Enable Parity Check on Data Read from SRAM. */
} adi_apollo_bmem_delay_sample_t;

/*!
* \brief BMEM delay mode with hopping configuration data
*/
typedef struct {
    uint8_t sample_size;                                    /*!< 0: NORMAL 16b, 1: DEBUG 32b */
    uint8_t ramclk_ph_dis;                                  /*!< Set 1 to Disable SRAM Clock Phasing. */
	uint16_t hop_delay[ADI_APOLLO_BMEM_HOP_PROFILES];       /*!< Sample delay profiles */
	uint8_t hop_delay_sel_mode;                             /*!< 0 to cycle 0-1-2-3-0 1 for gpio select next profile */
	uint8_t trig_mode_sclr_en;                              /*!< Set to 1 to self clear after a trigger */
	uint8_t parity_check_en;                                /*!< Set 1 to Enable Parity Check on Data Read from SRAM. */
} adi_apollo_bmem_delay_hop_t;


#endif /* __ADI_APOLLO_BMEM_TYPES_H__ */
/*! @} */
