/**
 * \file
 * \brief BitField accessors for nvs_regmap_rxb_common emulation
 *
 * \copyright Copyright (c) 2023 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef __ADRV910X_BF_NVS_REGMAP_RXB_COMMON_H__
#define __ADRV910X_BF_NVS_REGMAP_RXB_COMMON_H__

#include "adrv910x_bf_hal.h"
#include "adrv910x_nvs_regmap_rxb.h"
#include "adrv910x_nvs_regmap_rxnb2.h"

typedef enum adrv910x_BfNvsRegmapRxbCommon_e
{
	ADRV910X_BF_RXB1_CORE   = 0,
	ADRV910X_BF_RXB_NB_CORE = 1
} adrv910x_BfNvsRegmapRxbCommon_e;


/**
 * \brief This register is applicable to all the automatic gain control modes (AGC fast attack, AGC slow loop and AGC Hybrid mode). When this register is set to a non-zero value, the Rx1 AGC state machine is not enabled as soon as RXON goes high. The AGC state machine is held inactive for the duration of the value in this register. Each LSB in this register corresponds to a delay of 1s (determined by reference_clock_cycles).
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcAttackDelay_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcAttackDelay_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcAttackDelay_Set(device, value);
	}
}

/**
 * \brief This register is applicable to all the automatic gain control modes (AGC fast attack, AGC slow loop and AGC Hybrid mode). When this register is set to a non-zero value, the Rx1 AGC state machine is not enabled as soon as RXON goes high. The AGC state machine is held inactive for the duration of the value in this register. Each LSB in this register corresponds to a delay of 1s (determined by reference_clock_cycles).
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcAttackDelay_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcAttackDelay_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcAttackDelay_Get(device, value);
	}
}

/**
 * \brief This register is read back only. When read back in Rx mode, it indicates the gain index selected by either the AGC loops or in manual gain mode, the index selected by the BBP for Rx1 at that time. The number of rows in the programmable gain table is 256 (0 through 255). The default gain table has valid entries from index 255 through index 236. When agc_enable_gain_index_update is set, the register must be written to (with some dummy data) before reading. The write latches the current index into the register. When the bit is not set, the register can be read from (without a write to it), but the user must ensure that that index is not being read while it is changing. In the Rx AGC `increment gain on peak threshold only' mode, when the LL blocker triggers a programmable number of times above its configured threshold (see 0x482, LL blocker exceeded counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to LL blocker triggering. Rx1 AGC gain index
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcGainIndex_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcGainIndex_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcGainIndex_Set(device, value);
	}
}

/**
 * \brief This register is read back only. When read back in Rx mode, it indicates the gain index selected by either the AGC loops or in manual gain mode, the index selected by the BBP for Rx1 at that time. The number of rows in the programmable gain table is 256 (0 through 255). The default gain table has valid entries from index 255 through index 236. When agc_enable_gain_index_update is set, the register must be written to (with some dummy data) before reading. The write latches the current index into the register. When the bit is not set, the register can be read from (without a write to it), but the user must ensure that that index is not being read while it is changing. In the Rx AGC `increment gain on peak threshold only' mode, when the LL blocker triggers a programmable number of times above its configured threshold (see 0x482, LL blocker exceeded counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to LL blocker triggering. Rx1 AGC gain index
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcGainIndex_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcGainIndex_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcGainIndex_Get(device, value);
	}
}

/**
 * \brief In the Rx AGC `increment gain on peak threshold only' mode, when the LL blocker triggers a programmable number of times above its configured threshold (agc_llb_threshold_exceeded_counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to LL blocker triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcLlbGainStep_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcLlbGainStep_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcLlbGainStep_Set(device, value);
	}
}

/**
 * \brief In the Rx AGC `increment gain on peak threshold only' mode, when the LL blocker triggers a programmable number of times above its configured threshold (agc_llb_threshold_exceeded_counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to LL blocker triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcLlbGainStep_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcLlbGainStep_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcLlbGainStep_Get(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used): AGC Fast Attack Loop: This register allows the user to configure the power level at which the signal should be held after gain lock. The value in this register is specified as a (negative sign implied)dB value below full scale. Therefore, for an LTE signal with PAR = 10dB, and leaving a 4dB margin for signal variation, the AGC lock level should be written to 0x0E. This gain control closed loop algorithm will attempt to achieve this average power level for the signal during gain lock. AGC Slow Loop: In this mode, there exist four thresholds. There are two inner thresholds and two outer thresholds as shown below. The value in this register specifies the inner High threshold in (negative sign implied) dBFS. If the average power of the signal goes above this level during normal RXON operation, the gain control loop will attempt to correct this situation by reducing the gain. The gain can only be reduced when the gain update counter expires. The gain in this case will be reduced by the value in agc_upper_0_threshold_exceeded_gain_step.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcLockLevel_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcLockLevel_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcLockLevel_Set(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used): AGC Fast Attack Loop: This register allows the user to configure the power level at which the signal should be held after gain lock. The value in this register is specified as a (negative sign implied)dB value below full scale. Therefore, for an LTE signal with PAR = 10dB, and leaving a 4dB margin for signal variation, the AGC lock level should be written to 0x0E. This gain control closed loop algorithm will attempt to achieve this average power level for the signal during gain lock. AGC Slow Loop: In this mode, there exist four thresholds. There are two inner thresholds and two outer thresholds as shown below. The value in this register specifies the inner High threshold in (negative sign implied) dBFS. If the average power of the signal goes above this level during normal RXON operation, the gain control loop will attempt to correct this situation by reducing the gain. The gain can only be reduced when the gain update counter expires. The gain in this case will be reduced by the value in agc_upper_0_threshold_exceeded_gain_step.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcLockLevel_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcLockLevel_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcLockLevel_Get(device, value);
	}
}

/**
 * \brief When SPI control mode is enabled (agc_rx1_manual_gain_pin_control = 0), this register allows the user to directly index the programmable Rx gain table for channel 1. The default gain table can take values between 0xEB and 0xFF, even though every index is accessible from 0x00 to 0xFF. When the index is written to this register, the analog FE gain is changed within 2 clkADC/16 cycles. The digital gain is updated after the count value for digital gain delay reaches 0.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcManualGainIndex_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcManualGainIndex_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcManualGainIndex_Set(device, value);
	}
}

/**
 * \brief When SPI control mode is enabled (agc_rx1_manual_gain_pin_control = 0), this register allows the user to directly index the programmable Rx gain table for channel 1. The default gain table can take values between 0xEB and 0xFF, even though every index is accessible from 0x00 to 0xFF. When the index is written to this register, the analog FE gain is changed within 2 clkADC/16 cycles. The digital gain is updated after the count value for digital gain delay reaches 0.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcManualGainIndex_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcManualGainIndex_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcManualGainIndex_Get(device, value);
	}
}

/**
 * \brief This configures the maximum gain index in the programmable gain table that can be used by the various gain control modes. The maximum gain index register value is the maximum index that can be used by the pin control Manual Gain control mode. The direct SPI control mode is not limited by this register. For the default table, this value should be 0xFF.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcMaximumGainIndex_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcMaximumGainIndex_Set(device, value);
	}
}

/**
 * \brief This configures the maximum gain index in the programmable gain table that can be used by the various gain control modes. The maximum gain index register value is the maximum index that can be used by the pin control Manual Gain control mode. The direct SPI control mode is not limited by this register. For the default table, this value should be 0xFF.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcMaximumGainIndex_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcMaximumGainIndex_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcMaximumGainIndex_Get(device, value);
	}
}

/**
 * \brief This register puts a limit on the lowest index that the gain control algorithms can use.  For the default table, this value should be 0xEC.  The minimum gain index register value is the minium index that can be used by the pin control manual gain control mode.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcMinimumGainIndex_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcMinimumGainIndex_Set(device, value);
	}
}

/**
 * \brief This register puts a limit on the lowest index that the gain control algorithms can use.  For the default table, this value should be 0xEC.  The minimum gain index register value is the minium index that can be used by the pin control manual gain control mode.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcMinimumGainIndex_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcMinimumGainIndex_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcMinimumGainIndex_Get(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used): AGC Fast Attack Loop: The gain control loop reduces the gain by the specified number of indices when either UL blocker triggers or the ADC High Ovrg block triggers requesting a gain change. The gain index in that case is reduced by the value specified in this register. AGC Slow Loop: In this mode, when the ADC High Overrange triggers a programmable number of times (agc_adc_high_ovrg_exceeded_counter), ADC Ovrg Count Threshold) above its configured threshold, the slow loop will decrement gain either immediately (agc_change_gain_if_ulbth_high) or at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be reduced by when a gain step change needs to be made due to ADC High Ovrg triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcOvrgHighGainStep_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcOvrgHighGainStep_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcOvrgHighGainStep_Set(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used): AGC Fast Attack Loop: The gain control loop reduces the gain by the specified number of indices when either UL blocker triggers or the ADC High Ovrg block triggers requesting a gain change. The gain index in that case is reduced by the value specified in this register. AGC Slow Loop: In this mode, when the ADC High Overrange triggers a programmable number of times (agc_adc_high_ovrg_exceeded_counter), ADC Ovrg Count Threshold) above its configured threshold, the slow loop will decrement gain either immediately (agc_change_gain_if_ulbth_high) or at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be reduced by when a gain step change needs to be made due to ADC High Ovrg triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcOvrgHighGainStep_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcOvrgHighGainStep_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcOvrgHighGainStep_Get(device, value);
	}
}

/**
 * \brief In the Rx AGC `increment gain on peak threshold only' mode, when the ADC Low Overrange triggers a programmable number of times above its configured threshold (agc_adc_low_ovrg_exceeded_counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to ADC Low Ovrg triggering. When in multiple time constants in AGC loop mode, the value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to ADC Low Ovrg interval 2 triggering for the 3rd update interval. For details on using this field in this mode, see description in agc_enable_fast_recovery_loop.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcOvrgLowGainStep_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcOvrgLowGainStep_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcOvrgLowGainStep_Set(device, value);
	}
}

/**
 * \brief In the Rx AGC `increment gain on peak threshold only' mode, when the ADC Low Overrange triggers a programmable number of times above its configured threshold (agc_adc_low_ovrg_exceeded_counter), the slow loop will increment gain at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to ADC Low Ovrg triggering. When in multiple time constants in AGC loop mode, the value entered into this register should be the number of index values that the gain should be increased by when a gain step change needs to be made due to ADC Low Ovrg interval 2 triggering for the 3rd update interval. For details on using this field in this mode, see description in agc_enable_fast_recovery_loop.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcOvrgLowGainStep_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcOvrgLowGainStep_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcOvrgLowGainStep_Get(device, value);
	}
}

/**
 * \brief These bits configure the duration (in AGC clock cycles) that the gain control algorithms wait before enabling regular operation of the peak detectors.  These include the analog peak detector, ADC overrange detector and HB2 output peak detectors. If digital gain saturation is enabled, then the peak wait time needs to be increased to account for the location of the digital saturation block in the RX chain (after the PFIR).
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcPeakWaitTime_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcPeakWaitTime_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcPeakWaitTime_Set(device, value);
	}
}

/**
 * \brief These bits configure the duration (in AGC clock cycles) that the gain control algorithms wait before enabling regular operation of the peak detectors.  These include the analog peak detector, ADC overrange detector and HB2 output peak detectors. If digital gain saturation is enabled, then the peak wait time needs to be increased to account for the location of the digital saturation block in the RX chain (after the PFIR).
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcPeakWaitTime_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcPeakWaitTime_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcPeakWaitTime_Get(device, value);
	}
}

/**
 * \brief NvsRegmapRxbCommon_AgcResetOnRxonGainIndex
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcResetOnRxonGainIndex_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcResetOnRxonGainIndex_Set(device, value);
	}
}

/**
 * \brief NvsRegmapRxbCommon_AgcResetOnRxonGainIndex
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcResetOnRxonGainIndex_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcResetOnRxonGainIndex_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcResetOnRxonGainIndex_Get(device, value);
	}
}

/**
 * \brief These bits configure the gain control algorithm that is used on channel 1. The option are as follows: 00      Manual Gain Mode Operation              FDD and TDD modes (but much slower       (SPI control or pin control modes)       than the internal AGC loops), 01      Fast Attack AGC Mode                    TDD mode, 10      Slow Loop AGC Mode                      FDD, TDD modes, 11      Hybrid AGC Mode                         FDD, TDD modes
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcSetup_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcSetup_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcSetup_Set(device, value);
	}
}

/**
 * \brief These bits configure the gain control algorithm that is used on channel 1. The option are as follows: 00      Manual Gain Mode Operation              FDD and TDD modes (but much slower       (SPI control or pin control modes)       than the internal AGC loops), 01      Fast Attack AGC Mode                    TDD mode, 10      Slow Loop AGC Mode                      FDD, TDD modes, 11      Hybrid AGC Mode                         FDD, TDD modes
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcSetup_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcSetup_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcSetup_Get(device, value);
	}
}

/**
 * \brief If this bit is set, pin control mode is enabled on channel 1.  If this bit is clear, then SPI control mode is enabled.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcManualGainPinControl_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcManualGainPinControl_Set(device, value);
	}
}

/**
 * \brief If this bit is set, pin control mode is enabled on channel 1.  If this bit is clear, then SPI control mode is enabled.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcManualGainPinControl_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcManualGainPinControl_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcManualGainPinControl_Get(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used):AGC Fast Attack Loop: The gain control loop reduces the gain by the specified number of indices when both UL blocker triggers and the ADC High Ovrg block triggers simultaneously. The gain index in that case is reduced by the value specified in this register.AGC Slow Loop: In this mode, when the UL blocker triggers a programmable number of times (upper_level_blocker_threshold) above its configured threshold, the slow loop will decrement gain either immediately (agc_change_gain_if_ulbth_high) or at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be reduced by when a gain step change needs to be made due to UL Blocker triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcUlbGainStep_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcUlbGainStep_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcUlbGainStep_Set(device, value);
	}
}

/**
 * \brief This register performs two functions (based on gain control mode used):AGC Fast Attack Loop: The gain control loop reduces the gain by the specified number of indices when both UL blocker triggers and the ADC High Ovrg block triggers simultaneously. The gain index in that case is reduced by the value specified in this register.AGC Slow Loop: In this mode, when the UL blocker triggers a programmable number of times (upper_level_blocker_threshold) above its configured threshold, the slow loop will decrement gain either immediately (agc_change_gain_if_ulbth_high) or at the termination of the gain update counter. The value entered into this register should be the number of index values that the gain should be reduced by when a gain step change needs to be made due to UL Blocker triggering.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcUlbGainStep_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcUlbGainStep_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcUlbGainStep_Get(device, value);
	}
}

/**
 * \brief Setting this bit enables using the blocker, ADC, and digital saturation over-range counters in Rx1 when in manual gain control mode.  The overrange signals can be observed by the user through the GPIO to control the AGC in this mode.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcUseCountersForMgc_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcUseCountersForMgc_Set(device, value);
	}
}

/**
 * \brief Setting this bit enables using the blocker, ADC, and digital saturation over-range counters in Rx1 when in manual gain control mode.  The overrange signals can be observed by the user through the GPIO to control the AGC in this mode.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_AgcUseCountersForMgc_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_AgcUseCountersForMgc_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_AgcUseCountersForMgc_Get(device, value);
	}
}

/**
 * \brief Controls how the decimated power measurement is trigged. When set to 1, the AGC starts the power measurement. When 0, the decimated power measurement is trigged from SPI by setting "dec_power_enable_meas" register.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerEnSpiOrAgcSelect_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerEnSpiOrAgcSelect_Set(device, value);
	}
}

/**
 * \brief Controls how the decimated power measurement is trigged. When set to 1, the AGC starts the power measurement. When 0, the decimated power measurement is trigged from SPI by setting "dec_power_enable_meas" register.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerEnSpiOrAgcSelect_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerEnSpiOrAgcSelect_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerEnSpiOrAgcSelect_Get(device, value);
	}
}

/**
 * \brief If this bit is set, the Rx power measurement block is enabled. If the manual mode for gain control is enabled, the power measurement starts measuring as soon as this bit is set. If in one of the AGC modes, the start of power measurement depends on the current state of the state machine. This bit needs to be set for correct operation of the automatic gain control state machine.  The duration of the measurement can be configured by writing to "dec_power_measurement_duration".
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerEnableMeas_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerEnableMeas_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerEnableMeas_Set(device, value);
	}
}

/**
 * \brief If this bit is set, the Rx power measurement block is enabled. If the manual mode for gain control is enabled, the power measurement starts measuring as soon as this bit is set. If in one of the AGC modes, the start of power measurement depends on the current state of the state machine. This bit needs to be set for correct operation of the automatic gain control state machine.  The duration of the measurement can be configured by writing to "dec_power_measurement_duration".
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerEnableMeas_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerEnableMeas_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerEnableMeas_Get(device, value);
	}
}

/**
 * \brief 00: RFIR, 01: HB1, 10: HB2, 11:HB2
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerInputSelect_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerInputSelect_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerInputSelect_Set(device, value);
	}
}

/**
 * \brief 00: RFIR, 01: HB1, 10: HB2, 11:HB2
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerInputSelect_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerInputSelect_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerInputSelect_Get(device, value);
	}
}

/**
 * \brief When this bit is set, the dynamic range of the power measurement increases from 40dB to ~60dB (that is, all signal levels from 0dBFS to -60dBFS are accurately detected.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerLogShift_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerLogShift_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerLogShift_Set(device, value);
	}
}

/**
 * \brief When this bit is set, the dynamic range of the power measurement increases from 40dB to ~60dB (that is, all signal levels from 0dBFS to -60dBFS are accurately detected.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerLogShift_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerLogShift_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerLogShift_Get(device, value);
	}
}

/**
 * \brief These bits contain the power measurement duration used by the decimated power block. The power measurement duration in Rx sample periods is determined using the following equation: dec power meas duration = 8 x 2^DPMD,  where DPMD  3:0   are the contents of these register bits.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerMeasurementDuration_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerMeasurementDuration_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerMeasurementDuration_Set(device, value);
	}
}

/**
 * \brief These bits contain the power measurement duration used by the decimated power block. The power measurement duration in Rx sample periods is determined using the following equation: dec power meas duration = 8 x 2^DPMD,  where DPMD  3:0   are the contents of these register bits.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerMeasurementDuration_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerMeasurementDuration_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerMeasurementDuration_Get(device, value);
	}
}

/**
 * \brief This register determines the number of cycles to wait between successive decimated power measurements. The number of cycles in Rx sample clocks are determined by: wait duration = dec_power_wait_delay * 32.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[in] value     Value to be set
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerWaitDelay_Set(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	const uint8_t value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerWaitDelay_Set(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerWaitDelay_Set(device, value);
	}
}

/**
 * \brief This register determines the number of cycles to wait between successive decimated power measurements. The number of cycles in Rx sample clocks are determined by: wait duration = dec_power_wait_delay * 32.
 * 
 * \param[in] adrv910x    Context variable
 * \param[in] instance  Module enum
 * \param[out] value     value get
 * 
 * \returns status       0 to indicate sucess
 */
static inline int32_t adrv910x_NvsRegmapRxbCommon_DecPowerWaitDelay_Get(void *const device,
	adrv910x_BfNvsRegmapRxbCommon_e instance,
	uint8_t *const value)
{
	if (ADRV910X_BF_RXB1_CORE == instance)
	{
		return adrv910x_NvsRegmapRxb_DecPowerWaitDelay_Get(device, value);
	}
	else
	{
		return adrv910x_NvsRegmapRxnb2_DecPowerWaitDelay_Get(device, value);
	}
}

#endif // __ADRV910X_BF_NVS_REGMAP_RXB_COMMON_H__