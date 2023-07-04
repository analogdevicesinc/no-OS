/*!
 * @brief     device configuration header
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup ADI_AD9081_INTERNAL_CONFIG
 * @{
 */
#ifndef __ADI_AD9081_CONFIG_H__
#define __ADI_AD9081_CONFIG_H__

/*============= D E F I N E S ==============*/

/*============= I N C L U D E S ============*/
#include "adi_ad9081.h"
#include "adi_ad9081_bf_impala_tc.h"
#include "adi_ad9081_bf_jrxa_des.h"
#include "adi_ad9081_bf_jtx_dual_link.h"
#include "adi_ad9081_bf_jtx_qbf_ad9081.h"
#include "adi_ad9081_bf_lcpll_28nm.h"
#include "adi_ad9081_bf_main.h"
#include "adi_ad9081_bf_nb_coarse_nco.h"
#include "adi_ad9081_bf_nb_ddc_dformat.h"
#include "adi_ad9081_bf_nb_fine_nco.h"
#include "adi_ad9081_bf_rx_paging.h"
#include "adi_ad9081_bf_ser_phy.h"
#include "adi_ad9081_bf_spi_only_up.h"
#include "adi_ad9081_bf_ad9081.h"

/*============= D E F I N E S ==============*/
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ == 199901L)
#define __FUNCTION_NAME__ __func__
#else
#define __FUNCTION_NAME__ __FUNCTION__
#endif

#define AD9081_API_REV 0x00010500
#define AD9081_API_HW_RESET_LOW 600000
#define AD9081_API_RESET_WAIT 500000
#define AD9081_PLL_LOCK_TRY 75
#define AD9081_PLL_LOCK_WAIT 20000
#define AD9081_JESD_CAL_BOOT_WAIT 250000
#define AD9081_JESD_MAN_CAL_WAIT 200000
#define AD9081_JESD_RX_204C_CAL_WAIT 500000
#define AD9081_JESD_FG_CAL_WAIT 200000
#define AD9081_JESD_BG_CAL_WAIT 10000
#define AD9081_SERDES_RST_WAIT 50000
#define AD9081_DESER_MODE_204B_BR_TRESH 8000000000ULL
#define AD9081_DESER_MODE_204C_BR_TRESH 16000000000ULL
#define AD9081_IL_CTLE_UPPER_DB_THRESH 10

/* var error report */
#define AD9081_MSG_REPORT(var, comment)                                        \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_MSG, API_CMS_ERROR_OK, \
				    __FILE__, __FUNCTION_NAME__, __LINE__,     \
				    #var, comment)
#define AD9081_WARN_REPORT(var, comment)                                       \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_WARN,                  \
				    API_CMS_ERROR_OK, __FILE__,                \
				    __FUNCTION_NAME__, __LINE__, #var,         \
				    comment)
#define AD9081_ERROR_REPORT(error, var, comment)                               \
	adi_ad9081_hal_error_report(device, ADI_CMS_LOG_ERR, error, __FILE__,  \
				    __FUNCTION_NAME__, __LINE__, #var,         \
				    comment)

/* log report */
#define AD9081_LOG_FUNC()                                                      \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_API, "%s(...)",           \
				 __FUNCTION_NAME__)
#define AD9081_LOG_SPIR(addr, data)                                            \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: r@%.4x = %.2x", addr, data)
#define AD9081_LOG_SPIW(addr, data)                                            \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: w@%.4x = %.2x", addr, data)
#define AD9081_LOG_SPIR32(addr, data)                                          \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: r32@%.4x = %.8x", addr, data)
#define AD9081_LOG_SPIW32(addr, data)                                          \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_SPI,                      \
				 "ad9081: w32@%.4x = %.8x", addr, data)
#define AD9081_LOG_VAR(type, msg, ...)                                         \
	adi_ad9081_hal_log_write(device, type, msg, ##__VA_ARGS__)
#define AD9081_LOG_MSG(msg)                                                    \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_MSG, msg)
#define AD9081_LOG_WARN(msg)                                                   \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_WARN, msg)
#define AD9081_LOG_ERR(msg)                                                    \
	adi_ad9081_hal_log_write(device, ADI_CMS_LOG_ERR, msg)

/* var error check */
#define AD9081_ERROR_RETURN(r)                                                 \
	{                                                                      \
		if (r != API_CMS_ERROR_OK) {                                   \
			return r;                                              \
		}                                                              \
	}
#define AD9081_NULL_POINTER_RETURN(p)                                          \
	{                                                                      \
		if (p == NULL) {                                               \
			AD9081_ERROR_REPORT(API_CMS_ERROR_NULL_PARAM, p,       \
					    "Null pointer passed.");           \
			return API_CMS_ERROR_NULL_PARAM;                       \
		}                                                              \
	}
#define AD9081_INVALID_PARAM_RETURN(r)                                         \
	{                                                                      \
		if (r) {                                                       \
			AD9081_ERROR_REPORT(API_CMS_ERROR_INVALID_PARAM, r,    \
					    "Invalid param passed.");          \
			return API_CMS_ERROR_INVALID_PARAM;                    \
		}                                                              \
	}
#define AD9081_INVALID_PARAM_WARN(r)                                           \
	{                                                                      \
		if (r) {                                                       \
			AD9081_WARN_REPORT(r, "Invalid param passed.");        \
		}                                                              \
	}

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Enables or disables on-chip PLL.
 *
 *  If set, the device clocks will be generated from the PLL. Otherwise
 *  the device clocks will be generated from the reference clock supplied.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  pll_en            Enable signal for PLL
 *                                  0 – Disable PLL
 *                                  1 – Enable PLL
 *
 * \returns API_CMS_ERROR_OK returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_clk_pll_enable_set(adi_ad9081_device_t *device,
					     uint8_t pll_en);

/**
 * \brief Configures PLL divider settings.
 *
 *   Set up the on-chip pll dividers which can generate the device
 *   clocks for dac and adc cores from a provided reference clock.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  ref_div           Input reference clock divider. Valid range 0-0x3.
 * \param[in]  m_div             PLL M-Divider.
 * \param[in]  pll_div           PLL output divider. Valid range 0-0x3.
 * \param[in]  fb_div            PLL Feedback divider. Valid range 0-0x3F.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_clk_pll_div_set(adi_ad9081_device_t *device,
					  uint8_t ref_div, uint8_t m_div,
					  uint8_t pll_div, uint8_t fb_div);

/**
 * \brief Configures PLL input and output frequencies.
 *
 *  Configure the PLL as per the desired DAC and ADC clock based on the
 *  supplied reference clock set by ref_clk_hz parameter.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dac_clk_hz        Desired DAC Clock Frequency in Hz. Valid Range 1.5GHz to 12GHz.
 * \param[in]  adc_clk_hz        Desired ADC Clock Frequency in Hz. Valid Range 2GHz to 6GHz.
 * \param[in]  ref_clk_hz        Reference Clock Frequecny provided in Hz. Valid Range 100 MHz to 2GHz.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_clk_pll_startup(adi_ad9081_device_t *device,
					  uint64_t dac_clk_hz,
					  uint64_t adc_clk_hz,
					  uint64_t ref_clk_hz);

/**
 * \brief Configures up dividers.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dac_clk_hz        Desired DAC Clock Frequency in Hz. Valid Range 1.5GHz to 12GHz.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_clk_up_div_set(adi_ad9081_device_t *device,
					 uint64_t dac_clk_hz);

/**
 * \brief Gets laminate ID.
 *
 * Gets laminate ID from register 0x1e0d and puts it where input parameter 'id' points.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  id                Pointer to where you want to store laminate ID.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_laminate_id_get(adi_ad9081_device_t *device,
					  uint8_t *id);

/**
 * \brief Gets die ID.
 *
 *  Gets die ID from register 0x1e0e and puts it where input parameter 'id' points.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  id                Pointer to where you want to store die ID.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_die_id_get(adi_ad9081_device_t *device, uint8_t *id);

/**
 * \brief Checks status of power supplies.
 *
 *  Checks power supply status for: DAC, Clock, ADC0, and ADC1.
 *
 * \param[in]  device	Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned if all power supplies are on. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_power_status_check(adi_ad9081_device_t *device);

/**
 * \brief Performs 8-bit register read/write test.
 *
 * Tries writing/reading register REG_PAGEINDX_DAC_CHAN_ADDR (0x0000001C).
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_reg8_access_check(adi_ad9081_device_t *device);

/**
 * \brief Performs 32-bit register read/write test.
 *
 * Tries writing/reading register 0x01001300.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_reg32_access_check(adi_ad9081_device_t *device);

/**
 * \brief Verifies and logs pre-clock boot activity.
 *
 * Tracks core status through register 0x3472 to ensure boot reaches expected spot in boot_pre_clock().
 * Next, checks that device revision is supported and logs chip ID, laminate ID, and die ID.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_boot_pre_clock(adi_ad9081_device_t *device);

/**
 * \brief Verifies and logs post-clock boot activity.
 *
 * Checks device revision to determine if the boot loader power up sequence should be bypassed.
 * Verifies up_spi_edge_interrupt bit is cleared. Checks that boot is complete.
 * Checks that core is waiting to run application code. Verifies that clock switch is done.
 * Completes additional writes to ADC SPI enable registers. Finally, logs firmware revision.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_boot_post_clock(adi_ad9081_device_t *device);

/**
 * \brief Control master-slave mode for NCO sync.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  mode              0: disable, 1: set as master, 2: set as slave, 3: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_nco_sync_mode_set(adi_ad9081_device_t *device,
					    uint8_t mode);

/**
 * \brief Select which source triggers Master-slave mode for the master device.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  source            0: sysref, 1: lmfc rising edge, 2: lmfc falling edge.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_device_nco_sync_trigger_source_set(adi_ad9081_device_t *device,
					      uint8_t source);

/**
 * \brief Set GPIOs related to NCO sync.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  gpio_index        GPIO identifier.
 * \param[in]  output            (output > 0): 10 written to bitfield (output < 0): 11 written to bitfield.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_nco_sync_gpio_set(adi_ad9081_device_t *device,
					    uint8_t gpio_index, uint8_t output);

/**
 * \brief In NCO Master-Slave sync mode, set how many extra lmfc cycles to delay before an NCO reset is issued.
 *
 *  Only valid when 'nco_sync_ms_mode'==1 & 'nco_sync_ms_trig_source'!=0.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  num               Number of extra lmfc.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_device_nco_sync_extra_lmfc_num_set(adi_ad9081_device_t *device,
					      uint8_t num);

/**
 * \brief Control how RX & TX NCOs are synced by sysref.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  mode              0: immediately by sysref, 1: by next lmfc rising edge, 2: by next lmfc falling edge.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_nco_sync_sysref_mode_set(adi_ad9081_device_t *device,
						   uint8_t mode);

/**
 * \brief Resets NCO
 *
 * Transition low to high triggers an internal NCO reset signal, on the next received SYSREF pulse.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  enable            1: reset NCO, 0: clear reset bit.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_device_nco_sync_reset_via_sysref_set(adi_ad9081_device_t *device,
						uint8_t enable);

/**
 * \brief Triggers master-slave NCO synchronization.
 *
 * Self-clearing.
 *
 * \param[in]  device	         Pointer to device handler structure.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_nco_sync_trigger_set(adi_ad9081_device_t *device);

/**
 * \brief Enables digital logic.
 *
 * This includes JESD digital, digital clock gen., digital data path.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  enable            1: enable digital logic.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_device_digital_logic_enable_set(adi_ad9081_device_t *device,
						   uint8_t enable);

/**
 * \brief  Enable dual SPI mode
 *
 * Selecting dual0 will enable access to control registers in ranges 0x180-0x194, 0x60-0x7E, and 0x140-0x178.
 * This will only access these registers for DAC0, DAC1, ADC0, and ADC2. Select dual1 for the same ranges on
 * the DAC2, DAC3, ADC1, ADC3 side of the device.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  duals             0x1: select dual0, 0x2: select dual1, 0x3: select both.
 * \param[in]  enable            1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_d2a_dual_spi_enable_set(adi_ad9081_device_t *device,
					       uint8_t duals, uint8_t enable);

/**
 * \brief Starts up specified DACs
 *
 * Enables DLL clock control, SWD clock control, and mushi decoder/control for each
 * specified DAC. Overrides manual DCC code.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dacs              0bXXXX, set X==1 to start-up corresponding DAC :
 *                                  Bit 3: DAC 3  (MSB)
 *                                  Bit 2: DAC 2
 *                                  Bit 1: DAC 1
 *                                  Bit 0: DAC 0  (LSB)
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_dll_startup(adi_ad9081_device_t *device, uint8_t dacs);

/**
 * \brief Sets frequency tuning word for specified DACs and channels.
 *
 * If DDSM modulus disabled, main datapath NCO frequency =  FDAC * (FTW/2^48).
 * If DDSM modulus enabled, main datapath NCO frequency = FDAC * (FTW + REG_DDSM_ACC_DELTA/REG_DDSM_ACC_MODULUS) / 2^48.
 *
 *      Note: ACC_DELTA and ACC_MODULUS are set via adi_ad9081_dac_duc_nco_ftw_set call.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dacs              0bXXXX, set X==1 to specify DACs you wish to affect:
 *                                  Bit 3: DAC 3  (MSB)
 *                                  Bit 2: DAC 2
 *                                  Bit 1: DAC 1
 *                                  Bit 0: DAC 0  (LSB)
 * \param[in]  channels         0bXXXXXXXX, (8 bits) set X==1 to specifiy channels you wish to affect:
 *                                  Bit 7: Channel 7 (MSB)
 *                                  Bit 6: Channel 6
 *                                  .....
 *                                  Bit 0: Channel 0 (LSB)
 * \param[in]  ftw              Frequency tuning word.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_duc_nco_ftw0_set(adi_ad9081_device_t *device,
					uint8_t dacs, uint8_t channels,
					uint64_t ftw);

/**
 * \brief Enables soft off gain block for specified DACs.
 *
 * The ramp up/down process is achieved by data * gain which controls gain up
 * and down, so soft off gain block must be enabled to use soft off/on.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dacs              0bXXXX, set X==1 to specify DACs you wish to affect:
 *                                  Bit 3: DAC 3  (MSB)
 *                                  Bit 2: DAC 2
 *                                  Bit 1: DAC 1
 *                                  Bit 0: DAC 0  (LSB)
 * \param[in] enable             1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_soft_off_gain_enable_set(adi_ad9081_device_t *device,
						uint8_t dacs, uint8_t enable);

/**
 * \brief Enables MSB and ISB shuffling.
 *
 * Enable shuffling (randomly selecting) active MSB & ISB segments for each new DAC sampling cycle.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dacs              0bXXXX, set X==1 to specify DACs you wish to affect:
 *                                  Bit 3: DAC 3  (MSB)
 *                                  Bit 2: DAC 2
 *                                  Bit 1: DAC 1
 *                                  Bit 0: DAC 0  (LSB)
 * \param[in] enable             1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_shuffle_enable_set(adi_ad9081_device_t *device,
					  uint8_t dacs, uint8_t enable);

/**
 * \brief Enables both:
 *           - DAC data scrambling in sync and re-timer block (xor)
 *           - DAC data de-scramble in decoder (de-xor)
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  dacs              0bXXXX, set X==1 to specify DACs you wish to affect:
 *                                  Bit 3: DAC 3  (MSB)
 *                                  Bit 2: DAC 2
 *                                  Bit 1: DAC 1
 *                                  Bit 0: DAC 0  (LSB)
 * \param[in] enable             1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_dac_data_xor_set(adi_ad9081_device_t *device, uint8_t dacs,
				    uint8_t enable);

/**
 * \brief Selects ADCs to affect with next ADC setting change by updating coarse paging register.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  adcs              0bXXXX, set X==1 to specify ADCs you wish to affect:
 *                                  Bit 3: ADC 3  (MSB)
 *                                  Bit 2: ADC 2
 *                                  Bit 1: ADC 1
 *                                  Bit 0: ADC 0  (LSB)
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_select_set(adi_ad9081_device_t *device, uint8_t adcs);

/**
 * \brief Enables ADC0 and ADC1 analog register SPI access through the 8bit and 32bit SPI registers.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  adc_cores         0x01: ADC0 enable, 0x02: ADC1 enable, 0x03: both enable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_core_analog_regs_enable_set(adi_ad9081_device_t *device,
						   uint8_t adc_cores,
						   uint8_t enable);

/**
 * \brief Boots up specified ADC cores.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  adc_cores         0x01: boot ADC0, 0x02: boot ADC1, 0x03: boot both.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_core_setup(adi_ad9081_device_t *device,
				  uint8_t adc_cores);

/**
 * \brief Configures SPI registers for ADC Buffer
 *
 * \param[in]  device               Pointer to device handler structure.
 * \param[in]  adc_cores            0x01: boot ADC0, 0x02: boot ADC1, 0x03: boot both.
 * \param[in]  coupling             AC_COUPLED or DC_COUPLED
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_adc_analog_input_buffer_set(adi_ad9081_device_t *device,
				       uint8_t adc_cores,
				       adi_cms_signal_coupling_e coupling);

/**
 * \brief Powers up selected ADCs by booting up their respective cores.
 *
 *  Essentially a wrapper for adi_ad9081_adc_core_setup.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  adcs              0bXXXX, set X==1 to specify ADCs you wish to power up:
 *                                  Bit 3: ADC 3  (MSB)
 *                                  Bit 2: ADC 2
 *                                  Bit 1: ADC 1
 *                                  Bit 0: ADC 0  (LSB)
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_power_up_set(adi_ad9081_device_t *device, uint8_t adcs,
				    uint8_t enable);

/**
 * \brief Decodes ADC coarse DDC decimation values.
 *
 *
 * \param[in]  cddc_dcm	         Value to be decoded.
 *
 * \param[out] cddc_dcm_value    Decoded value.
 *
 * \returns cddc_dcm_value - Decoded decimation value.
 */
uint8_t
adi_ad9081_adc_ddc_coarse_dcm_decode(adi_ad9081_adc_coarse_ddc_dcm_e cddc_dcm);

/**
 * \brief Decodes ADC fine DDC decimation values.
 *
 *
 * \param[in]  fddc_dcm	         Value to be decoded.
 *
 * \param[out] fddc_dcm_value    Decoded value.
 *
 * \returns fddc_dcm_value - Decoded decimation value.
 */
uint8_t
adi_ad9081_adc_ddc_fine_dcm_decode(adi_ad9081_adc_fine_ddc_dcm_e fddc_dcm);

/**
 * \brief Selects the GPIOs to which PERI_I_SEL21/22 are connected.
 *
 * PERI_I_SEL21/22 are drive bits 0 and 1, respectively, of a 2-bit selector which
 * chooses among four profiles of Programmable Filter/Fractional Delay/Cycle Delay
 * (see datasheet).
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  sel0              Map a GPIO to PERI_I_SEL21.
 *
 *                               Pin selection:
 *
 *                                  TxFE GPIO[6] 	 0x02
 *                                  TxFE GPIO[7] 	 0x03
 *                                  TxFE GPIO[8] 	 0x04
 *                                  TxFE GPIO[9] 	 0x05
 *                                  TxFE GPIO[10]    0x06
 *                                  TxFE SYNCINB1_P	 0x07
 *                                  TxFE SYNCINB1_N  0x08
 *
 * \param[in]  sel1             Map a GPIO to PERI_I_SEL22. Same pin selection as sel0.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_fdelay_cdelay_pfir_sel_to_gpio_mapping_set(
	adi_ad9081_device_t *device, uint8_t sel0, uint8_t sel1);

/**
 * \brief Enables common, GPIO based frequency hopping for all coarse DDC NCOs.
 *
 * When enabled while GPIO based hopping is selected, freq. hopping is done at the same time for all the Coarse DDC NCOs, bypassing the profile_pins[5:4].
 * When disabled while GPIO based hopping is selected, freq. hopping is done for the Coarse DDC NCO selected by profile_pins[5:4].
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  enable            1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_common_hop_en_set(adi_ad9081_device_t *device,
					 uint8_t enable);

/**
 * \brief Changes Profile Update Mode/ Phase Update Mode for specified coarse DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  cddcs             0bXXXX, set X==1 to specify cddcs you wish to affect:
 *                                  Bit 3: cddc 3  (MSB)
 *                                  Bit 2: cddc 2
 *                                  Bit 1: cddc 1
 *                                  Bit 0: cddc 0  (LSB)
 * \param[in]  mode              0: Instantaneous/Continuous Update. Phase increment and phase offset values are updated immediately.
 *                               1: Phase increment and phase offset values are updated synchronously either when the chip_transfer
 *                                  bit is set high or based on the GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy)
 *                                  pin low to high transition. The chip transfer bit will be cleared once the transfer is complete.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_coarse_nco_channel_update_mode_set(
	adi_ad9081_device_t *device, uint8_t cddcs, uint8_t mode);

/**
 * \brief Changes GPIO Chip Transfer Mode for specified coarse DDCs.
 *
 * Used when ddc0_phase_update_mode is '1'.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  cddcs             0bXXXX, set X==1 to specify cddcs you wish to affect:
 *                                  Bit 3: cddc 3  (MSB)
 *                                  Bit 2: cddc 2
 *                                  Bit 1: cddc 1
 *                                  Bit 0: cddc 0  (LSB)
 * \param[in]  mode              0: Phase increment and phase offset values are updated synchronously when the chip_transfer bit is set
 *                                  high. The chip transfer bit will be cleared once the transfer is complete.
 *                               1: Phase increment and phase offset values are updated based on the GPIO (pin ddc_chip_gpio_transfer
 *                                  at nova_dig_dp_top hierarchy) pin low to high transition.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_adc_ddc_coarse_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						  uint8_t cddcs, uint8_t mode);

/**
 * \brief Enable trig signal frequency hopping for specified coarse DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  cddcs             0bXXXX, set X==1 to specify cddcs you wish to affect:
 *                                  Bit 3: cddc 3  (MSB)
 *                                  Bit 2: cddc 2
 *                                  Bit 1: cddc 1
 *                                  Bit 0: cddc 0  (LSB)
 * \param[in]  enable            0: Frequency hopping is independent of trig signal.
 *                               1: Trig signal is used for frequency hopping.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_coarse_trig_hop_en_set(adi_ad9081_device_t *device,
						  uint8_t cddcs,
						  uint8_t enable);

/**
 * \brief Enables amplitude dither and phase dither for specified coarse DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  cddcs             0bXXXX, set X==1 to specify cddcs you wish to affect:
 *                                  Bit 3: cddc 3  (MSB)
 *                                  Bit 2: cddc 2
 *                                  Bit 1: cddc 1
 *                                  Bit 0: cddc 0  (LSB)
 * \param[in]  amp_dither_en     0: enable, 1: disable
 * \param[in]  phase_dither_en   0: enable, 1: disable
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_coarse_dither_en_set(adi_ad9081_device_t *device,
						uint8_t cddcs,
						uint8_t amp_dither_en,
						uint8_t phase_dither_en);

/**
 * \brief Sets Profile Select Word (PSW) for specified coarse DDCs.
 *
 * The PSW specifies the rollover point (in encode samples) for the Profile Select Timer (PST).  Whenever the Profile Select Timer
 * rolls over to zero, channel selection counter increments when channel selection is through Profile Select Timer.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  cddcs             0bXXXX, set X==1 to specify cddcs you wish to affect:
 *                                  Bit 3: cddc 3  (MSB)
 *                                  Bit 2: cddc 2
 *                                  Bit 1: cddc 1
 *                                  Bit 0: cddc 0  (LSB)
 * \param[in]  psw               Profile Select Word.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_coarse_psw_set(adi_ad9081_device_t *device,
					  uint8_t cddcs, uint64_t psw);

/**
 * \brief Enables trig NCO reset for specified fine DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  fddcs             0bXXXXXXXX, (8 bits) set X==1 to specify fddcs you wish to affect:
 *                                  Bit 7: fddc 7  (MSB)
 *                                  Bit 6: fddc 6
 *                                  .....
 *                                  Bit 0: fddc 0  (LSB)
 * \param[in]  enable            1: enable, 0: disable.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_fine_trig_nco_reset_enable_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t enable);

/**
 * \brief Changes Profile Update Mode/ Phase Update Mode for specified fine DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  fddcs             0bXXXXXXXX, (8 bits) set X==1 to specify fddcs you wish to affect:
 *                                  Bit 7: fddc 7  (MSB)
 *                                  Bit 6: fddc 6
 *                                  .....
 *                                  Bit 0: fddc 0  (LSB)
 * \param[in]  mode              0: Instantaneous/Continuous Update. Phase increment and phase offset values are updated immediately.
 *                               1: Phase increment and phase offset values are updated synchronously either when the chip_transfer
 *                                  bit is set high or based on the GPIO (pin ddc_chip_gpio_transfer at nova_dig_dp_top hierarchy)
 *                                  pin low to high transition. The chip transfer bit will be cleared once the transfer is complete.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_fine_nco_channel_update_mode_set(
	adi_ad9081_device_t *device, uint8_t fddcs, uint8_t mode);

/**
 * \brief Changes GPIO Chip Transfer Mode for specified fine DDCs.
 *
 * Used when ddc0_phase_update_mode is '1'.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  fddcs             0bXXXXXXXX, (8 bits) set X==1 to specify fddcs you wish to affect:
 *                                  Bit 7: fddc 7  (MSB)
 *                                  Bit 6: fddc 6
 *                                  .....
 *                                  Bit 0: fddc 0  (LSB)
 * \param[in]  mode              0: Phase increment and phase offset values are updated synchronously when the chip_transfer bit is set
 *                                  high. The chip transfer bit will be cleared once the transfer is complete.
 *                               1: Phase increment and phase offset values are updated based on the GPIO (pin ddc_chip_gpio_transfer
 *                                  at nova_dig_dp_top hierarchy) pin low to high transition.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t
adi_ad9081_adc_ddc_fine_gpio_chip_xfer_mode_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t mode);

/**
 * \brief Enable trig signal frequency hopping for specified fine DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  fddcs             0bXXXXXXXX, (8 bits) set X==1 to specify fddcs you wish to affect:
 *                                  Bit 7: fddc 7  (MSB)
 *                                  Bit 6: fddc 6
 *                                  .....
 *                                  Bit 0: fddc 0  (LSB)
 * \param[in]  enable            0: Frequency hopping is independent of trig signal.
 *                               1: Trig signal is used for frequency hopping.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_fine_trig_hop_en_set(adi_ad9081_device_t *device,
						uint8_t fddcs, uint8_t enable);

/**
 * \brief Enables amplitude dither and phase dither for specified fine DDCs.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  fddcs             0bXXXXXXXX, (8 bits) set X==1 to specify fddcs you wish to affect:
 *                                  Bit 7: fddc 7  (MSB)
 *                                  Bit 6: fddc 6
 *                                  .....
 *                                  Bit 0: fddc 0  (LSB)
 * \param[in]  amp_dither_en     0: enable, 1: disable
 * \param[in]  phase_dither_en   0: enable, 1: disable
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_adc_ddc_fine_dither_en_set(adi_ad9081_device_t *device,
					      uint8_t fddcs,
					      uint8_t amp_dither_en,
					      uint8_t phase_dither_en);

/**
 * \brief  Sets masking for unused channels.
 *
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  links             Choose AD9081 link(s) to set mask for:
 *                                  0x1 - AD9081_LINK_0
 *                                  0x2 - AD9081_LINK_1
 *                                  0x3 - Both
 * \param[in]  conv_index        Choose converter to set mask for:
 *                                  0x0 - CONV0
 *                                  0x1 - CONV1
 *                                  0x2 - CONV2
 *                                  .....
 *                                  0xF - CONV15
 * \param[in]  val               0: unmask, 1: mask.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_tx_conv_mask_set(adi_ad9081_device_t *device,
					 adi_ad9081_jesd_link_select_e links,
					 uint8_t conv_index, uint8_t val);

/**
 * \brief Sets virtual converters for specified links.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  links             Choose AD9081 link(s) to set mask for:
 *                                  0x1 - AD9081_LINK_0
 *                                  0x2 - AD9081_LINK_1
 *                                  0x3 - Both
 * \param[in]  jesd_conv_sel     jesd_conv_sel[0] - Virtual converter selection struct for AD9081_LINK_0.
 *                               jesd_conv_sel[1] - Virtual converter selection struct for AD9081_LINK_1.
 * \param[in]  jesd_m            jesd_m[0] - Number of used virtual converters for AD9081_LINK_0.
 *                               jesd_m[1] - Number of used virtual converters for AD9081_LINK_1.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_tx_link_conv_sel_set(
	adi_ad9081_device_t *device, adi_ad9081_jesd_link_select_e links,
	adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2], uint8_t jesd_m[2]);

/**
 * \brief Get status of PLL.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  pll_locked        Pointer to location for storing pll locked status.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_tx_pll_status_get(adi_ad9081_device_t *device,
					  uint8_t *pll_locked);

/**
 * \brief Startup JESD deserializers.
 *
 * \param[in]  device	         Pointer to device handler structure.
 * \param[in]  deser_mode        0: Full rate operation, 1: Half rate operation, 2: Quarter rate operation.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_rx_startup_des(adi_ad9081_device_t *device,
				       adi_ad9081_deser_mode_e deser_mode);

/**
 * \brief Finds unused DFormat output to assign to unused virtual converters.
 *
 * \param[in]  jesd_conv_sel     Pointer to virtual converter selection struct.
 * \param[in]  jesd_m            Number of used virtual converters.
 *
 * \param[out] df_out            Unused DFormat output.
 *
 * \returns df_out: Unused DFormat output.
 */
uint16_t adi_ad9081_jesd_find_dformat_out_nc(
	adi_ad9081_jtx_conv_sel_t const *jesd_conv_sel, uint8_t jesd_m);

/**
 * \brief Finds the highest common unused DFormat output between both AD9081 links.
 *
 * \param[in]  links             0x1 - Outputs first unused DFormat output for AD9081_LINK_0.
 *                               0x2 - Outputs first unused DFormat output for AD9081_LINK_1.
 *                               0x3 - Outputs highest common unused DFormat output between both.
 * \param[in]  jesd_conv_sel     jesd_conv_sel[0] - Virtual converter selection struct for AD9081_LINK_0.
 *                               jesd_conv_sel[1] - Virtual converter selection struct for AD9081_LINK_1.
 * \param[in]  jesd_m            jesd_m[0] - Number of used virtual converters for AD9081_LINK_0.
 *                               jesd_m[1] - Number of used virtual converters for AD9081_LINK_1.
 *
 * \param[out] nc                DFormat output.
 *
 * \returns nc - DFormat output.
 */
uint8_t
adi_ad9081_jesd_determine_common_nc(adi_ad9081_jesd_link_select_e links,
				    adi_ad9081_jtx_conv_sel_t jesd_conv_sel[2],
				    uint8_t jesd_m[2]);

/**
 * \brief  Set d2acenter enable
 *
 * \param[in]  device     Pointer to the device structure
 * \param[in]  enable     1:Enable, 0:Disable
 *
 * \return API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_sysref_d2acenter_enable_set(adi_ad9081_device_t *device,
						    uint8_t enable);

/**
 * \brief Set hardware sysref control
 *
 * \param[in] device      Pointer to the device structure
 *
 * \return API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_sync_sysref_ctrl(adi_ad9081_device_t *device);

/**
 * \brief Sets JTX_BR_LOG2_RATIO for specified links.
 *
 * \param[in]   device           Pointer to device handler structure.
 * \param[in]   chip_op_mode     Chip operating mode, 1 : Tx Only, 2 : Rx Only, 3 : Tx + Rx Only.
 * \param[in]   jesd_param       @see adi_cms_jesd_param_t, pass array with 2 elements for dual link.
 * \param[in]   links            Choose AD9081 link(s) to set mask for:
 *                                  0x1 - AD9081_LINK_0
 *                                  0x2 - AD9081_LINK_1
 *                                  0x3 - Both
 * \param[in]   jtx_lane_rate    jtx_lane_rate[0] - lane rate in bps for AD9081_LINK_0.
 *                               jtx_lane_rate[1] - lane rate in bps for AD9081_LINK_1.
 * \param[out]  jtx_brr          jtx_brr[0] - JTX_BR_LOG2_RATIO value for AD9081_LINK_0.
 *                               jtx_brr[1] - JTX_BR_LOG2_RATIO value for AD9081_LINK_1.
 *
 * \returns API_CMS_ERROR_OK is returned upon success. Otherwise, a failure code.
 */
int32_t adi_ad9081_jesd_tx_calc_br_ratio(adi_ad9081_device_t *device,
					 adi_cms_chip_op_mode_t chip_op_mode,
					 adi_cms_jesd_param_t *jesd_param,
					 adi_ad9081_jesd_link_select_e links,
					 uint64_t jtx_lane_rate[2],
					 uint8_t jtx_brr[2]);

#if AD9081_USE_FLOATING_TYPE > 0
int32_t adi_ad9081_hal_calc_nco_ftw_f(adi_ad9081_device_t *device, double freq,
				      double nco_shift, uint64_t *ftw,
				      uint64_t *a, uint64_t *b);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ADI_AD9081_CONFIG_H__ */

/*! @} */