// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI ADRV9102 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_ADRV9102_TYPES_H
#define AXI_ADRV9102_TYPES_H

/**
 * \brief mcs parameters, all parameters are based off the device clock,
 *  the offsets define the value of the period counter at which mcs is
 *  asserted high. parameters must be set before request.
 */

typedef struct axi_adrv9102_mcs_params {

    uint32_t period;  /*!< period, total duration HIGH_TIME + LOW_TIME */
    uint32_t width; /*!< width, HIGH_TIME */
    uint32_t count; /*!< number of pulses in a sequence. */
    uint32_t edge; /*!< mcs is driven at falling edge (1) or rising edge (0) of device clock */
    uint32_t select; /*!< select, internal (0x0), external (0x1) or none (0x3) */

} axi_adrv9102_mcs_params_t;

/**
 * \brief mcs receive reporting, all parameters are based off the axilite clock,
 *  the delta define the offset between the two interfaces, with a direction.
 */

typedef struct axi_adrv9102_mcs_rx_info {

    uint32_t rx0_latency;  /*!< rx0 latency */
    uint32_t rx0_strobe;  /*!< rx0 strobe */
    uint32_t rx0_data;  /*!< rx0 data */
    uint32_t rx1_latency;  /*!< rx1 latency */
    uint32_t rx1_strobe;  /*!< rx1 strobe */
    uint32_t rx1_data;  /*!< rx1 data */

} axi_adrv9102_mcs_rx_info_t;

/**
 * \brief clock parameters,  hdl supports two reference clock sources.
 *  normally one from a global clock source and another from device itself.
 *  internally one of them is selected depending on the frequencies set here.
 *  if a clock is not present, set the frequency to 0.
 *  device clock frequency is the closest match to reference clock frequency.
 *  tdd/gpio clock frequency is the closest match to 200MHz.
 */

typedef struct axi_adrv9102_clk_params {

    uint32_t ref0_clk_freq_hz; /*!< reference clock 0, default */
    uint32_t ref1_clk_freq_hz; /*!< reference clock 1, device clock output */
    uint32_t gen_dev_clk_freq_hz; /*!< generated device clock frequency */
    uint32_t gen_tdd_clk_freq_hz; /*!< generated tdd clock frequency */
    uint32_t gen_gpio_clk_freq_hz; /*!< generated gpio clock frequency */
    uint32_t gen_uart_clk_freq_hz; /*!< generated uart clock frequency */
    uint32_t gen_qspi_clk_freq_hz; /*!< generated qspi clock frequency */

} axi_adrv9102_clk_params_t;

/**
 * \brief ssi parameters, these controls the ssi mode of the interface core.
 *  most of the parameters are self-explanatory and are complementary to corresponding
 *  device functions. two of the fields (cmos1_lvds0 and gpio1_ssi0) are special.
 *  if reconfiguration is NOT allowed, these fields are ignored (software may read
 *  these fields, and reinterpret rest of the fields).
 *  if reconfiguration is allowed, setting these fields changes the base configuration
 *  of the interface.
 *  the following table lists the configuration.
 *  gpio1_ssi0 == 0, cmos1_lvds0 = 0 indicates lvds ssi interface.
 *  gpio1_ssi0 == 0, cmos1_lvds0 = 1 indicates cmos ssi interface.
 *  gpio1_ssi0 == 1, cmos1_lvds0 = x indicates cmos gpio interface.
 *  see hdl documentation for further details about these fields.
 */

typedef struct axi_adrv9102_ssi_params {

    uint32_t gpio; /*!< gpio (1) or ssi (0) select */
    uint32_t mode; /*!< cmos (1) or lvds (0) select */
    uint32_t num_of_bits; /*!< numbert of bits in sample (2, 8, 16, 24, 32, or 64) */
    uint32_t strb_type; /*!< level (1) or pulse (0) strobe */
    uint32_t num_of_lanes; /*!< 4 (cmos only) (2), 2 (1) or single lane (0) mode */
    uint32_t data_rate; /*!< sdr (1) or ddr (0) mode */
    uint32_t bit_order; /*!< lsb first (1) or msb first (0) mode */
    uint32_t iq_order; /*!< q first (1) or i first (0) mode */
    uint32_t sign_extension; /*!< if set, sign extend to nearest byte boundary */
    uint32_t edge; /*!< sdr, falling edge (1) or rising edge (0) sampling select */
    uint32_t ss_ratio; /*!< sub/super sampling ratio 0, 2, 4, 8, 16, or 32 */
    uint32_t orx_ss_ratio; /*!< sub/super sampling ratio of orx channel */
    uint32_t tx_clk_sel; /*!< if set, uses rx clock, otherwise tx ref clock */
    uint32_t tx_clk_msb; /*!< if set, inverts tx clock output */
    uint32_t tx_buf_enable; /*!< if set, enables all tx output buffers */
    uint32_t tx_ss_mode; /*!< sub/super sampling mode, repeated strobe or single strobe */

} axi_adrv9102_ssi_params_t;

/**
 * \brief tdd frame parameters, must be set before tdd start.
 *  triggers may be set using the macro defines, multiple triggers are allowed and
 *  they behave like an 'and' function. the following is an example:
 *  params->triggers = AXI*TDD_TRIGGERS_SET(0, AXI*TDD_TRIGGER_RISING_EDGE) |
 *      AXI*TDD_TRIGGERS_SET(3, AXI*TDD_TRIGGER_FALLING_EDGE);
 *  this allows tdd to start only after a rising edge on trigger source 0, and
 *  a falling edge on trigger source 3, note that they do not have to happen
 *  at the same time, but must happen for tdd to start.
 */

typedef struct axi_adrv9102_tdd_frame_params {

    uint32_t period; /*!< period of a single tdd frame */
    uint32_t num_of_frames; /*!< number of frames in a sequence (continous == 0) */
    uint32_t frame_switch_period; /*!< period count to switch the frame */
    uint32_t frame_switch_num; /*!< frame number to switch the frame */
    uint32_t frame_switch_enable; /*!< frame switch enable */
    uint32_t continous; /*!< continous mode, requires sw to issue a stop request */
    uint32_t triggers[4]; /*!< triggers for starting a tdd frame sequence */

} axi_adrv9102_tdd_frame_params_t;

/**
 * \brief tdd enable parameters, may be modified at run time.
 *  see hdl documentation for more details.
 */

typedef struct axi_adrv9102_tdd_enable_params {

    uint32_t primary_assert; /*!< primary assert (set to 1) value */
    uint32_t primary_deassert; /*!< primary deassert (set to 0) value */
    uint32_t secondary_assert; /*!< secondary assert (set to 1) value */
    uint32_t secondary_deassert; /*!< secondary deassert (set to 0) value */
    uint32_t primary_frame_assert; /*!< primary assertion frame count */
    uint32_t primary_frame_deassert; /*!< primary deassertion frame count */
    uint32_t secondary_frame_assert; /*!< secondary assertion frame count */
    uint32_t secondary_frame_deassert; /*!< secondary deassertion frame count */

} axi_adrv9102_tdd_enable_params_t;

#endif

// **********************************************************************************
// **********************************************************************************
