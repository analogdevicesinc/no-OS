/*!
 * \brief     ADI utility functions header file.
 *
 * \version   0.1.x
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*! 
 * \addtogroup __ADI_UTILS__
 * @{
 */
#ifndef __ADI_UTILS_H__
#define __ADI_UTILS_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"

/*============= D E F I N E S ==============*/
#define ADI_UTILS_POW2_32         ((uint64_t)1 << 32)
#define ADI_UTILS_POW2_48         ((uint64_t)1 << 48)
#define ADI_UTILS_MAXUINT24       (0xffffff)
#define ADI_UTILS_MAXUINT32       (ADI_UTILS_POW2_32 - 1)
#define ADI_UTILS_MAXUINT48       (ADI_UTILS_POW2_48 - 1)

#define ADI_UTILS_GET_BYTE(w, p)  (uint8_t)(((w) >> (p)) & 0xff)
#define ADI_UTILS_DIV_U64(x, y)   ((x) / (y))
#define ADI_UTILS_BIT(x)          ((1) << (x))
#define ADI_UTILS_ALL             (-1)
#define ADI_UTILS_ARRAY_SIZE(a)   (sizeof(a) / sizeof((a)[0]))

#define ADI_UTILS_MAX(a,b) ((a) > (b) ? (a) : (b))
#define ADI_UTILS_MIN(a,b) ((a) < (b) ? (a) : (b))

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_api_utils_gcd(int32_t u, int32_t v);

uint64_t adi_api_utils_gcd_64(uint64_t u, uint64_t v);

uint32_t adi_api_utils_lcm(uint32_t p, uint32_t q);

uint64_t adi_api_utils_lcm_64(uint64_t p, uint64_t q);

int32_t adi_api_utils_is_power_of_two(uint64_t x);

void adi_api_utils_mult_64(uint32_t a, uint32_t b, uint32_t *hi, uint32_t *lo);

/**
 * @brief  Convert a ratio M/N to (X+A/B)/(2^bit_size)
 * 
 * @param ratio_num      Ratio numerator
 * @param ratio_den      Ratio denominator
 * @param bit_size       Number of bits
 * @param x              Decomposition integer component
 * @param a              Decomposition fractional numerator
 * @param b              Decomposition fractional denominator
 */
int32_t adi_api_utils_ratio_decomposition(uint64_t ratio_num, uint64_t ratio_den, uint8_t bit_size, uint64_t *x, uint64_t *a, uint64_t *b);

/**
 * @brief  Calculate a ratio M/N = (X+A/B)/(2^bit_size)
 * 
 * @param x              Composition integer component
 * @param a              Composition fractional numerator
 * @param b              Composition fractional denominator
 * @param ratio_num      Ratio numerator
 * @param ratio_den      Ratio denominator
 * @param bit_size       Number of bits
 */
int32_t adi_api_utils_ratio_composition(uint64_t x, uint64_t a, uint64_t b, uint32_t *ratio_num, uint32_t *ratio_den, uint8_t bit_size);

void adi_api_utils_lshift_128(uint64_t *hi, uint64_t *lo);

void adi_api_utils_rshift_128(uint64_t *hi, uint64_t *lo);

void adi_api_utils_mult_128(uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo);

void adi_api_utils_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi, 
    uint64_t b_lo, uint64_t *hi, uint64_t *lo);

void adi_api_utils_mod_128(uint64_t ah, uint64_t al, uint64_t div, uint64_t *mod);

void adi_api_utils_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
    uint64_t *hi, uint64_t *lo);

void adi_api_utils_subt_128(uint64_t ah, uint64_t al, uint64_t bh,uint64_t bl, 
    uint64_t *hi,uint64_t *lo);

uint64_t adi_api_utils_div_floor_u64(uint64_t a, uint64_t b);

uint64_t adi_api_utils_div_ceil_u64(uint64_t a, uint64_t b);

uint8_t adi_api_utils_num_selected(uint32_t select_mask);

uint8_t adi_api_utils_select_lsb_get(uint32_t select_mask);

/**
 * Convert uint64_t to byte array
 */

/**
 * @brief  Convert uint64_t to byte array
 * 
 * @param dest      Destination array
 * @param val       Value to convert
 */
void adi_uint64_to_byte_array(uint8_t dest[], const uint64_t val);

/**
 * @brief  Convert byte array to uint64_t
 * 
 * @param src      Source array
 * @param val      Converted value
 */
void adi_byte_array_to_uint64(uint8_t src[], uint64_t *val);

/**
 * \brief   The decision logic looks for an expected value, i.e. series of consecutive 1’s, within the link table from lower to upper bound indexes. 
 *          If a match is found, calculates the center index from the located series.
 * 
 * 
 * \param link_table            Value to search series of 1's within
 * \param expected_val          Value to look for, which is a series of 1's
 * \param clk_adj_lower_bound   Starting bit index for the search
 * \param clk_adj_upper_bound   Ending bit index for the search
 * \param calc_clk_adj          Center index of matching bit sequence
 * 
 * \return API_CMS_ERROR_OK         API Completed Successfully
 * \return <0                       Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_api_utils_check_consecutive_ones_bounded(uint16_t link_table, uint8_t expected_val, uint8_t clk_adj_lower_bound, uint8_t clk_adj_upper_bound, uint8_t *calc_clk_adj);



#ifdef __cplusplus
}
#endif

#endif /*__ADI_UTILS_H__*/

/*! @} */
