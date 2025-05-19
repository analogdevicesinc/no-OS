/*!
 * \brief     SMON Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SMON
 * @{
 */
#ifndef __ADI_APOLLO_SMON_TYPES_H__
#define __ADI_APOLLO_SMON_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "adi_apollo_fddc_types.h"

/*============= D E F I N E S ==============*/
#define ADI_APOLLO_SMON_NUM 8
#define ADI_APOLLO_SMON_STATUS_SIZE 3

/*!
* \brief Enumerates SMON selection
*/
typedef enum {
    ADI_APOLLO_SMON_NONE   = 0x00,                        /*!< No SMON */
    ADI_APOLLO_SMON_A0     = 0x01,                        /*!< SMON 0 of SIDE A */
    ADI_APOLLO_SMON_A1     = 0x02,                        /*!< SMON 1 of SIDE A */
    ADI_APOLLO_SMON_A2     = 0x04,                        /*!< SMON 2 of SIDE A */
    ADI_APOLLO_SMON_A3     = 0x08,                        /*!< SMON 3 of SIDE A */
    ADI_APOLLO_SMON_B0     = 0x10,                        /*!< SMON 0 of SIDE B */
    ADI_APOLLO_SMON_B1     = 0x20,                        /*!< SMON 1 of SIDE B */
    ADI_APOLLO_SMON_B2     = 0x40,                        /*!< SMON 2 of SIDE B */
    ADI_APOLLO_SMON_B3     = 0x80,                        /*!< SMON 3 of SIDE B */
    ADI_APOLLO_SMON_ALL    = 0xFF,                        /*!< All SMON */
    ADI_APOLLO_SMON_ALL_4T4R = 0x33,                      /*!< All SMON (4t4r) */
} adi_apollo_smon_sel_e;

/*!
* \brief Enumerates SMON SFRAMER MODE selection
*/
typedef enum {
    ADI_APOLLO_SFRAMER_TEN_BIT_DISABLE   = 0x0,      /*!< 10 Bit Framer Disabled */
    ADI_APOLLO_SFRAMER_TEN_BIT_ENABLE    = 0x1,      /*!< 10 Bit Framer Enabled */
    ADI_APOLLO_SFRAMER_FIVE_BIT_DISABLE  = 0x2,      /*!< 5 Bit Framer Disabled */
    ADI_APOLLO_SFRAMER_FIVE_BIT_ENABLE   = 0x3,      /*!< 5 Bit Framer Enabled */
} adi_apollo_smon_sframer_sel_e;

/*!
* \brief SMON programming data
*/
typedef struct {
    adi_apollo_smon_sframer_sel_e sframer_mode_en;    /*!< \ref adi_apollo_smon_sframer_sel_e */
    uint32_t smon_period;                             /*!< Signal Monitor Period */
    uint16_t thresh_low;                              /*!< 11 bit lower threshold for the absolute value of peak detected */
    uint16_t thresh_high;                             /*!< 11 bit upper threshold for the absolute value of peak detected */
    uint8_t sync_en;                                  /*!< SMON Synchronization Enable.
                                                           0: Synchronization Disabled; 
                                                           1: Synchronization Enabled.
                                                           If smon_sync_next == 1, only the next valide edge of the SYSREF pin will be used to synchronize the SMON block.
                                                           Subsequent edges of the SYSREF pin will be ignored. Once the next SYSREF has been received, this bit will be cleared */
    uint8_t sync_next;                                /*!< 0: Continuous mode, 1: Next Synchronization Mode */
    uint8_t sframer_en;                               /*!< Signal Monitor Serial Framer Enable. 0: Serial Framer is disabled; 1: Serial Framer is enabled. */
    uint8_t sframer_mode;                             /*!< Signal Monitor Serial Framer Mode Selection. 0: 10-bit Framer Selected; 1: 5-bit Framer Selected. */
    uint8_t sframer_insel;                            /*!< Signal Monitor Serial Framer Input Selection
                                                           When each individual bit is a 1, the corresponding signal statistics information is sent within the frame.
                                                           [0]: Reserved for thermometer detect
                                                           [1]: Peak Detector Data Inserted in Serial Frame.
                                                           [2]: Reserved RMS Magnitude
                                                           [3]: Reserved MS Power
                                                           [4]: Reserved for threshold crossing
                                                           [5]: Reserved */
    uint8_t peak_en;                                  /*!< Signal Monitor Peak Detector Enable. 0: Peak Detector Disabled, 1: Peak Detector Enabled */
    uint8_t status_rdsel;                             /*!< Signal Monitor Status Read-back Selection
                                                           0x0: Reserved for thermometer detect
                                                           0x1: Peak Detector placed on Status read-back signals
                                                           0x2: Reserved for RMS  Magnitude
                                                           0x3: Reserved for MS Power
                                                           0x4: Reserved for threshold crossing
                                                           0x5: Reserved 
                                                           0x6-0x7: Reserved */
    uint8_t jlink_sel;                                /*!< 0: JESD Link0, 1: JESD LInk1 */
    uint8_t gpio_en;                                  /*!< If set, allows GPIO output of peak indication. Peak_en also has to be enabled for this feature. */
} adi_apollo_smon_pgm_t;

/*!
* \brief SMON DFOR programming data
*/
typedef struct {
    uint8_t ctrl_bit_0;                                    /*!< Control Bit 0 Mux Selection \ref adi_apollo_dfor_ctrl_sel_e */
    uint8_t ctrl_bit_1;                                    /*!< Control Bit 1 Mux Selection \ref adi_apollo_dfor_ctrl_sel_e */
    uint8_t ctrl_bit_2;                                    /*!< Control Bit 2 Mux Selection \ref adi_apollo_dfor_ctrl_sel_e */
    uint8_t fine_adc_i_sel[ADI_APOLLO_FDDC_NUM];        /*!< FDDC ADC I Path Selection \ref adi_apollo_fine_adc_sel_e */
    uint8_t fine_adc_q_sel[ADI_APOLLO_FDDC_NUM];        /*!< FDDC ADC Q Path Selection \ref adi_apollo_fine_adc_sel_e */ 
} adi_apollo_smon_dfor_pgm_t;

/*!
* \brief SMON Status read data
*/
typedef struct {
    uint8_t status_fcnt;      /*!< Increments whenever period counter expires */
    uint32_t status;          /*!< 20 bits Signal Monitor Serial Data Output bits */
} adi_apollo_smon_read_t;

#endif /* __ADI_APOLLO_SMON_TYPES_H__ */
/*! @} */
