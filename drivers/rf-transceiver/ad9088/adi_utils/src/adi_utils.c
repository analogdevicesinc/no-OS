/*!
 * \brief     API utility helper functions
 *
 * \version   0.1.x
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*! 
 * \addtogroup ADI_APOLLO_UTILS
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_utils.h"

/*============= D E F I N E S ==============*/
#define LOWER_16(A)     (((A) &  0xffff))
#define UPPER_16(A)     (((A) >> 16) & 0xffff)
#define LOWER_32(A)     (((A) & (uint32_t) 0xffffffff))
#define U64MSB          (0x8000000000000000ull)

/*============= C O D E ====================*/
int32_t adi_api_utils_gcd(int32_t u, int32_t v)
{
    int32_t t;
    while (v != 0) {
        t = u; 
        u = v; 
        v = t % v;
    }
    return u < 0 ? -u : u; /* abs(u) */
}

uint64_t adi_api_utils_gcd_64(uint64_t u, uint64_t v)
{
    uint64_t t;
    while (v != 0) {
        t = u; 
        u = v; 
#ifdef __KERNEL__
        div64_u64_rem(t, v, &v);
#else
        v = t % v;
#endif
    }
    return u;
}

uint32_t adi_api_utils_lcm(uint32_t p, uint32_t q)
{
    uint32_t gcd, lcm, i, min_p_q;

    if (p == 0 || q == 0) {
        return 0;
    }

    min_p_q = (p <= q) ? p : q;

    i = min_p_q;
    while (i > 0) {
        if (p % i == 0 && q % i == 0) {
            gcd = i;
            break;
        }
        i--;
    }

    lcm = (p * q) / gcd;
    return lcm;
}


uint64_t adi_api_utils_lcm_64(uint64_t p, uint64_t q)
{
    uint64_t gcd, lcm, i, min_p_q;
#ifdef __KERNEL__
    uint64_t p_mod_i, q_mod_i;
#endif    
    min_p_q = (p <= q) ? p : q;

    i = min_p_q;
    while (i > 0) {
#ifdef __KERNEL__
        div64_u64_rem(p, i, &p_mod_i);
        div64_u64_rem(q, i, &q_mod_i);

        if (p_mod_i == 0 && q_mod_i == 0) {
            gcd = i;
            break;
        }
#else
        if (p % i == 0 && q % i == 0) {
            gcd = i;
            break;
        }
#endif
        i--;
    }

#ifdef __KERNEL__
    lcm = div64_u64(p * q, gcd);
#else
    lcm = (p * q) / gcd;
#endif
    return lcm;
}

int32_t adi_api_utils_is_power_of_two(uint64_t x)
{
    return ((x != 0) && !(x & (x - 1)));
}

void adi_api_utils_mult_64(uint32_t a, uint32_t b, uint32_t *hi, uint32_t *lo)
{
    uint32_t ah   = a >> 16, 
             al   = a & 0xffff, 
             bh   = b >> 16,
             bl   = b & 0xffff,
             rh   = ah * bh,
             rl   = al * bl,
             rm1  = ah * bl,
             rm2  = al * bh,
             rm1h = rm1 >> 16, 
             rm2h = rm2 >> 16, 
             rm1l = rm1 & 0xffff, 
             rm2l = rm2 & 0xffff,
             rmh  = rm1h + rm2h, 
             rml  = rm1l + rm2l, 
             c    = ((rl >> 16) + rml) >> 16;
    rl  = rl + (rml << 16);
    rh  = rh + rmh + c;
    *lo = rl;
    *hi = rh;
}

void adi_api_utils_lshift_128(uint64_t *hi, uint64_t *lo)
{
    *hi <<= 1;
    if (*lo & U64MSB)
    {
        *hi |= 1ul;
    }
    *lo <<= 1;
}

void adi_api_utils_rshift_128(uint64_t *hi, uint64_t *lo)
{
    *lo >>= 1;
    if (*hi & 1u) {
        *lo |= U64MSB;
    }
    *hi >>= 1;
}

void adi_api_utils_mult_128(uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo)
{
    uint64_t ah   = a >> 32,
             al   = a & 0xffffffff, 
             bh   = b >> 32, 
             bl   = b & 0xffffffff, 
             rh   = ah * bh, 
             rl   = al * bl, 
             rm1  = ah * bl, 
             rm2  = al * bh, 
             rm1h = rm1 >> 32,
             rm2h = rm2 >> 32,
             rm1l = rm1 & 0xffffffff, 
             rm2l = rm2 & 0xffffffff,
             rmh  = rm1h + rm2h, 
             rml  = rm1l + rm2l, 
             c    = ((rl >> 32) + rml) >> 32;
    rl  = rl + (rml << 32);
    rh  = rh + rmh + c;
    *lo = rl;
    *hi = rh;
}

void adi_api_utils_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi, uint64_t b_lo,
    uint64_t *hi, uint64_t *lo)
{
    uint64_t remain_lo = a_lo; /* The left-hand side of division, i.e. what is being divided */
    uint64_t remain_hi = a_hi; /* The left-hand side of division, i.e. what is being divided */
    uint64_t part1_lo  = b_lo; /* The right-hand side of division */
    uint64_t part1_hi  = b_hi; /* The right-hand side of division */
    uint64_t result_lo = 0;
    uint64_t result_hi = 0;
    uint64_t mask_lo   = 1;
    uint64_t mask_hi   = 0;
    
    if ((part1_lo == 0) && (part1_hi == 0)) {
        /* Do whatever should happen when dividing by zero. */
        return;
    }

    /* while(part1_lo < remain_lo)
     * Alternative: while(!(part1 & 0x8000)) - For 16-bit, test highest order bit.
     * Alternative: while(not_signed(part1)) - Same as above: As long as sign bit is not set in part1. 
     */
    while (!(part1_hi & U64MSB)) {
        adi_api_utils_lshift_128(&part1_hi, &part1_lo);
        adi_api_utils_lshift_128(&mask_hi, &mask_lo);
    }

    do {
        if ((remain_hi > part1_hi) || ((remain_hi == part1_hi) && (remain_lo >= part1_lo))) {
            /* remain_lo = remain_lo - part1_lo */
            adi_api_utils_subt_128(remain_hi, remain_lo, part1_hi, part1_lo, &remain_hi, &remain_lo);
            /* result = result + mask           */
            adi_api_utils_add_128(result_hi, result_lo, mask_hi, mask_lo, &result_hi, &result_lo);
        }        
        adi_api_utils_rshift_128(&part1_hi, &part1_lo); /* part1 = part1 >> 1 */        
        adi_api_utils_rshift_128(&mask_hi, &mask_lo);   /* mask  = mask  >> 1 */
    } while ((mask_hi != 0) || (mask_lo != 0));
    
    /* result    = division result       (quotient) */
    /* remain_lo = division remain_loder (modulo)   */
    *lo = result_lo;
    *hi = result_hi;
}

void  adi_api_utils_mod_128(uint64_t ah, uint64_t al, uint64_t div, uint64_t *mod)
{
    uint64_t result = 0;
    uint64_t a;

#ifdef __KERNEL__
    div64_u64_rem(~0, div, &a);
    a += 1;
    div64_u64_rem(ah, div, &ah);
#else
    a = (~0 % div) + 1;
    ah %= div;
#endif

    /*modular multiplication of (2^64*upper) % div*/
    while (ah != 0) {
        if ( (ah & 1) == 1) {
            result += a;
            if(result >= div) {
                result -= div;
            }
        }
        a <<= 1;
        if(a >= div) {
            a -= div;
        }
        ah >>= 1;
    }

    /* add up the 2 results and return the modulus*/
    if (al > div) {
        al -= div;
    }

#ifdef __KERNEL__
    div64_u64_rem(al + result, div, mod);
#else
    *mod = (al + result) % div;
#endif
}

void adi_api_utils_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
    uint64_t *hi, uint64_t *lo)
{
    /* r = a + b */
    uint64_t rl, rh;
    rl = al + bl;
    rh = ah + bh;

    if (rl < al)
    {
        rh++;
    }

    *lo = rl;
    *hi = rh;
}

void adi_api_utils_subt_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl, 
    uint64_t *hi, uint64_t *lo)
{
    /* r = a - b */
    uint64_t rl, rh;
    if (bl <= al) {
        rl = al - bl;
        rh = ah - bh;
    } else {
        rl = bl - al - 1;
        rl = 0xFFFFFFFFFFFFFFFFll - rl;
        ah--;
        rh = ah - bh;
    }

    *lo = rl;
    *hi = rh;
}

uint8_t adi_api_utils_num_selected(uint32_t select_mask)
{
    int sel_cnt = 0;
    while (select_mask) {
        sel_cnt += select_mask & 1;
        select_mask = select_mask >> 1;
    }
    return sel_cnt;
}

uint8_t adi_api_utils_select_lsb_get(uint32_t select_mask)
{
    uint8_t index = 0;
    if (select_mask == 0) {
        return 255;
    }
    while ((select_mask & 1) == 0) {
        index++;
        select_mask = select_mask >> 1;
    }
    return index;
}

void adi_uint64_to_byte_array(uint8_t dest[], const uint64_t val)
{
    uint32_t offset = 0;
    uint32_t endian_test_val = 0x11223344;
    uint8_t is_lit_endian = (*(uint8_t *)&endian_test_val == 0x44);
    
    /* Little endian */
    if (is_lit_endian) {
        dest[offset++] = (uint8_t)(val);
        dest[offset++] = (uint8_t)(val >> 8);
        dest[offset++] = (uint8_t)(val >> 16);
        dest[offset++] = (uint8_t)(val >> 24);
        dest[offset++] = (uint8_t)(val >> 32);
        dest[offset++] = (uint8_t)(val >> 40);
        dest[offset++] = (uint8_t)(val >> 48);
        dest[offset++] = (uint8_t)(val >> 56);
    } else {
        offset = 7;
        dest[offset--] = (uint8_t)(val);
        dest[offset--] = (uint8_t)(val >> 8);
        dest[offset--] = (uint8_t)(val >> 16);
        dest[offset--] = (uint8_t)(val >> 24);
        dest[offset--] = (uint8_t)(val >> 32);
        dest[offset--] = (uint8_t)(val >> 40);
        dest[offset--] = (uint8_t)(val >> 48);
        dest[offset--] = (uint8_t)(val >> 56);
    }
}

void adi_byte_array_to_uint64(uint8_t src[], uint64_t *val)
{
    uint32_t offset = 0;
    uint32_t endian_test_val = 0x11223344;
    uint8_t is_lit_endian = (*(uint8_t *)&endian_test_val == 0x44);

    uint8_t *val_ptr = (uint8_t *) val;

    if (is_lit_endian) {
        val_ptr[0] = src[offset++];
        val_ptr[1] = src[offset++];
        val_ptr[2] = src[offset++];
        val_ptr[3] = src[offset++];
        val_ptr[4] = src[offset++];
        val_ptr[5] = src[offset++];
        val_ptr[6] = src[offset++];
        val_ptr[7] = src[offset++];
    } else {
        offset = 7;
        val_ptr[0] = src[offset--];
        val_ptr[1] = src[offset--];
        val_ptr[2] = src[offset--];
        val_ptr[3] = src[offset--];
        val_ptr[4] = src[offset--];
        val_ptr[5] = src[offset--];
        val_ptr[6] = src[offset--];
        val_ptr[7] = src[offset--];
    }
}

int32_t adi_api_utils_ratio_decomposition(uint64_t ratio_num, uint64_t ratio_den, uint8_t bit_size, uint64_t *x, uint64_t *a, uint64_t *b) {
    uint64_t gcd, n, mh, ml, xh, xl; // ratio = m/n
    uint64_t y;

    if (ratio_den == 0  || bit_size >= 64 || ratio_num > ratio_den) {
        return -1;
    }

    gcd = adi_api_utils_gcd_64(ratio_num, ratio_den);

#ifdef __KERNEL__
    n = div64_u64(ratio_den, gcd);
    adi_api_utils_mult_128(div64_u64(ratio_num, gcd),(1ull << bit_size), &mh, &ml);
#else
    n = ratio_den/gcd;
    adi_api_utils_mult_128(ratio_num/gcd,(1ull << bit_size), &mh, &ml);
#endif

    adi_api_utils_div_128(mh, ml, 0, n, &xh, &xl);
    adi_api_utils_mod_128(mh, ml, n, &y);

    if (xh > 0) {
        return -1;
    }

    gcd = adi_api_utils_gcd_64(y, n);

    *x = xl;
#ifdef __KERNEL__
    *a = div64_u64(y, gcd);
    *b = div64_u64(n, gcd);
#else
    *a = y / gcd;
    *b = n / gcd;
#endif
    return 0;
}

int32_t adi_api_utils_ratio_composition(uint64_t x, uint64_t a, uint64_t b, uint32_t *ratio_num, uint32_t *ratio_den, uint8_t bit_size)
{

    uint64_t mh, ml, ah, al, xh, xl, gcd;

    if (b == 0  || bit_size >= 64) {
        return -1;
    }

    adi_api_utils_mult_128(x, b, &mh, &ml);
    adi_api_utils_add_128(mh, ml, 0, a, &ah, &al);
    while ((al & (0xFFFFFFFFFFFFFFFF >> (64 - bit_size))) != 0ull) {
        adi_api_utils_lshift_128(&ah, &al);
        b = b << 1;
    }

    adi_api_utils_div_128(ah, al, 0, (1ull << bit_size), &xh, &xl);

    if (xh > 0) {
        return -1;
    }
 
    gcd = adi_api_utils_gcd_64(xl, b);

#ifdef __KERNEL__
    *ratio_num = div64_u64(xl, gcd);
    *ratio_den = div64_u64(b, gcd);
#else
    *ratio_num = xl / gcd;
    *ratio_den = b / gcd;
#endif

    return 0;

}

uint64_t adi_api_utils_div_floor_u64(uint64_t a, uint64_t b) {

#ifdef __KERNEL__
    return div64_u64(a, b);
#else
    return ((a) / (b));
#endif
}

uint64_t adi_api_utils_div_ceil_u64(uint64_t a, uint64_t b) {

#ifdef __KERNEL__
    return DIV64_U64_ROUND_UP(a, b);
#else
    return ((a) / (b)) + ((a) % (b) != 0);
#endif
}

int32_t adi_api_utils_check_consecutive_ones_bounded(uint16_t link_table, uint8_t expected_val, uint8_t clk_adj_lower_bound, uint8_t clk_adj_upper_bound, uint8_t *calc_clk_adj)
{
    uint16_t link_table_shifted = 0;
    uint8_t bit_shift = 0;
    uint8_t i = 0;

    for (i = clk_adj_lower_bound; i <= clk_adj_upper_bound; ++i) {
        link_table_shifted = (link_table >> bit_shift) & expected_val;

        if (link_table_shifted == expected_val) {
            *calc_clk_adj = i;
            return API_CMS_ERROR_OK;
        }
        bit_shift++;
    }
    return API_CMS_ERROR_ERROR;
}

/*! @} */

