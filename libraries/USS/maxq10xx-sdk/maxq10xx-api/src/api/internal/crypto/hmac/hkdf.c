// Implements the HKDF algorithm (HMAC-based Extract-and-Expand Key
// Derivation Function, RFC 5869).

/* Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#include <string.h>
#include <stdio.h>
#include "sha2.h"
#include "hmac_sha2.h"
#include "hkdf.h"





int hkdf_abstract_extract(unsigned char *p_PRK,
                              unsigned char *p_salt,
                              unsigned int p_saltLen,
                              unsigned char *p_IKM,
                              unsigned int p_IKMLen)
{
    if( p_PRK == NULL)
    {
        return -1;
    }
    else if( p_saltLen != 0 && p_salt == NULL )
    {
        return -1;
    }
    if( p_saltLen != 0 || p_salt != NULL)
    {
        hmac_sha256(p_salt, p_saltLen, p_IKM, p_IKMLen, p_PRK, SHA256_DIGEST_SIZE);
    }
    else
    {
        unsigned char l_dummy[1] = {0}; //as ucl hmac does not accept null input
        hmac_sha256(l_dummy, 0, p_IKM, p_IKMLen, p_PRK, SHA256_DIGEST_SIZE);
    }
    return 0;
}


int hkdf_abstract_init(unsigned char *p_PRK,
                           unsigned char *p_salt,
                           unsigned int p_saltLen,
                           unsigned char *p_IKM,
                           unsigned int p_IKMLen,
                           unsigned char **p_info_ctxt,
                           unsigned int *p_infoLen_ctxt,
                           const unsigned char *p_info,
                           unsigned int p_infoLen,
                           unsigned int *p_counter)
{
    int l_ret;
    PROCESS_ERROR( l_ret, hkdf_abstract_extract( p_PRK, p_salt, p_saltLen, p_IKM, p_IKMLen) )
    *p_counter = 0;
    *p_info_ctxt = (unsigned char*) p_info; //info is shallow copied, this is okay because the original data pointed by info would be also required if expand() is used at once
    *p_infoLen_ctxt = p_infoLen;
    return 0;
}


int hkdf_abstract_core(unsigned char *p_OKM,
                           unsigned int p_OKMLen,
                           unsigned char *p_PRK,
                           unsigned int p_PRKLen,
                           unsigned char *p_info,
                           unsigned int p_infoLen,
                           unsigned int *p_counter,
                           unsigned char *p_state)
{

    unsigned int l_N, i;
    hmac_sha256_ctx ctx;
    const unsigned int l_hashLen = SHA256_DIGEST_SIZE;
    l_N = p_OKMLen / l_hashLen;
    unsigned int l_finalHashLen = l_hashLen;
    if ( p_OKMLen % l_hashLen != 0)
    {
        l_N++;
        l_finalHashLen = p_OKMLen % l_hashLen;
    }

    if ( ( l_N + *p_counter) > ( 0xFF * l_hashLen ) ) // l_N is the number of chunks to be processed now, p_counter is the number of chunkes processed before
    {
        return -1;
    }
    if ( p_PRKLen < l_hashLen )
    {
        return -1;
    }
    if ( p_info == NULL && p_infoLen != 0 )
    {
        return -1;
    }
    if ( *p_counter != 0 && p_state == NULL)
    {
        return -1;
    }

    for ( i = 1; i <= l_N; i++ )
    {
        unsigned char l_num = (unsigned char) ++(*p_counter);
         hmac_sha256_init(&ctx, p_PRK, p_PRKLen);
        if ( i != 1 )
        {   // process T(i-1)
            hmac_sha256_update(&ctx, p_OKM + ( i - 2 ) * l_hashLen, l_hashLen );
        }
        else if ( i == 1 && *p_counter != 1 )
        {
            // from last calls
            hmac_sha256_update(&ctx, p_state, l_hashLen);
        }

        if ( p_info != NULL && p_infoLen != 0 )
        {
            hmac_sha256_update(&ctx, p_info, p_infoLen);
        }

        hmac_sha256_update(&ctx, &l_num, 1);

        if ( i == l_N )
        {
            if ( p_state == NULL )
            {
                hmac_sha256_final(&ctx, p_OKM + ( i - 1 ) * l_hashLen, l_finalHashLen);
            }
            else
            {
                hmac_sha256_final(&ctx, p_state, l_hashLen);
                memcpy( p_OKM + ( l_N - 1 ) * l_hashLen, p_state, l_finalHashLen );
            }
        }
        else
        {
            hmac_sha256_final(&ctx, p_OKM + ( i - 1 ) * l_hashLen, l_hashLen);
        }
    }
    return 0;

}

int hkdf_abstract_finish(unsigned char *p_PRK,
                             unsigned char *p_state,
                             unsigned int *p_counter)
{
    const unsigned int l_hashLen = SHA256_DIGEST_SIZE;
    memset(p_PRK, 0, l_hashLen);
    memset(p_state, 0, l_hashLen);
    *p_counter = 0;
    return 0;
}


int hkdf_abstract_expand(unsigned char *p_OKM,
                             unsigned int p_OKMLen,
                             unsigned char *p_PRK,
                             unsigned int p_PRKLen,
                             unsigned char *p_info,
                             unsigned int p_infoLen)
{
    unsigned int l_zero = 0;
    return hkdf_abstract_core(p_OKM, p_OKMLen, p_PRK, p_PRKLen, p_info, p_infoLen, &l_zero, NULL);
}




int hkdf_abstract(unsigned char *p_OKM,
                      unsigned int p_OKMLen,
                      unsigned char *p_IKM,
                      unsigned int p_IKMLen,
                      unsigned char *p_info,
                      unsigned int p_infoLen,
                      unsigned char *p_salt,
                      unsigned int p_saltLen)
{
    int l_ret;
    unsigned char l_PRK[SHA256_DIGEST_SIZE];
    const unsigned int l_hashLen = SHA256_DIGEST_SIZE;

    PROCESS_ERROR( l_ret, hkdf_abstract_extract( l_PRK, p_salt, p_saltLen, p_IKM, p_IKMLen ) )
    PROCESS_ERROR( l_ret, hkdf_abstract_expand( p_OKM, p_OKMLen, l_PRK, l_hashLen, p_info, p_infoLen ) )

    return l_ret;
}

// --------------------------------------- SHA-256 ------------------------------------------------------------------------------------------------------------------------------------

int hkdf_sha256(unsigned char *p_OKM, unsigned int p_OKMLen, unsigned char *p_IKM, unsigned int p_IKMLen, unsigned char *p_info, unsigned int p_infoLen, unsigned char *p_salt, unsigned int p_saltLen)
{
    return hkdf_abstract(p_OKM, p_OKMLen, p_IKM, p_IKMLen, p_info, p_infoLen, p_salt, p_saltLen);
}

int hkdf_sha256_extract(unsigned char *p_PRK, unsigned char *p_salt, unsigned int p_saltLen, unsigned char *p_IKM, unsigned int p_IKMLen)
{
    return hkdf_abstract_extract(p_PRK, p_salt, p_saltLen, p_IKM, p_IKMLen);
}

int hkdf_sha256_expand(unsigned char *p_OKM, unsigned int p_OKMLen, unsigned char *p_PRK, unsigned int p_PRKLen, unsigned char *p_info, unsigned int p_infoLen)
{
    return hkdf_abstract_expand(p_OKM, p_OKMLen, p_PRK, p_PRKLen, p_info, p_infoLen);
}

int hkdf_sha256_init(hkdf_sha256_ctx_t *p_ctxt, const unsigned char *p_info, unsigned int p_infoLen, unsigned char *p_salt, unsigned int p_saltLen, unsigned char *p_IKM, unsigned int p_IKMLen)
{
    return hkdf_abstract_init(p_ctxt->PRK, p_salt, p_saltLen, p_IKM, p_IKMLen, &(p_ctxt->info), &(p_ctxt->infoLen), p_info, p_infoLen, &(p_ctxt->counter));
}

int hkdf_sha256_core(unsigned char *p_OKM, unsigned int p_OKMLen, hkdf_sha256_ctx_t *p_ctxt)
{
    return hkdf_abstract_core(p_OKM, p_OKMLen, p_ctxt->PRK, SHA256_DIGEST_SIZE, p_ctxt->info, p_ctxt->infoLen, &(p_ctxt->counter), p_ctxt->state);
}

int hkdf_sha256_finish(hkdf_sha256_ctx_t *p_ctxt)
{
    return hkdf_abstract_finish(p_ctxt->PRK, p_ctxt->state, &(p_ctxt->counter));
}
//#define VERBOSE

int test_hkdf_helper_sha256(unsigned char *IKM, unsigned int IKMLen, unsigned char *salt, unsigned int saltLen, unsigned char *info, unsigned int infoLen, unsigned char *PRK_exp, unsigned int PRKLen, unsigned char *OKM_exp, unsigned int OKMLen, int test_id)
{
    unsigned char temp[120];
    (void) test_id;
    int ret;
    hkdf_sha256_ctx_t sha256_hkdf_ctxt;
    int loop, i;
    unsigned int infoLen_internal = infoLen;
    unsigned int saltLen_internal = saltLen;
    int last_core_block = SHA256_DIGEST_SIZE;
    loop = OKMLen / SHA256_DIGEST_SIZE;
    if (OKMLen % SHA256_DIGEST_SIZE != 0)
    {
        last_core_block = OKMLen % SHA256_DIGEST_SIZE;
        loop++;
    }
    if (info == NULL)
    {
        infoLen_internal = 0;
    }
    if (salt == NULL)
    {
        saltLen_internal = 0;
    }

    ret = hkdf_sha256_extract(temp, salt, saltLen_internal, IKM, IKMLen);
    if (ret != 0)
    {
#ifdef VERBOSE
        printf("Error in hkdf-sha256 extract, test %d: %d\n", test_id, ret);
#endif
        return -1;
    }
    else if (memcmp(temp, PRK_exp, sizeof(PRKLen)))
    {
#ifdef VERBOSE
        printf("hkdf-sha256 extract, test %d, wrong output\n", test_id);
#endif
        return -1;
    }

    ret = hkdf_sha256_expand(temp, OKMLen, PRK_exp, PRKLen, info, infoLen_internal);

    if (ret != 0)
    {
#ifdef VERBOSE
        printf("Error in hkdf-sha256 expand, test %d: %d\n", test_id, ret);
#endif
        return -1;
    }
    else if (memcmp(temp, OKM_exp, OKMLen))
    {
#ifdef VERBOSE
        printf("hkdf-sha256 expand, test %d, wrong output\n", test_id);
#endif
        return -1;
    }
    // direct call
    ret = hkdf_sha256(temp, OKMLen, IKM, IKMLen, info, infoLen_internal, salt, saltLen_internal);
    if (ret != 0)
    {
#ifdef VERBOSE
        printf("Error in direct call to hkdf-sha256, test %d: %d\n", test_id, ret);
#endif
        return -1;
    }
    else if (memcmp(temp, OKM_exp, OKMLen))
    {
#ifdef VERBOSE
        printf("direct call to hkdf-sha256, test %d, wrong output\n", test_id);
#endif
        return -1;
    }
    // init core finish
    ret = hkdf_sha256_init(&sha256_hkdf_ctxt, info, infoLen_internal, salt, saltLen_internal, IKM, IKMLen);
    if (ret != 0)
    {
#ifdef VERBOSE
        printf("Error in hkdf-sha256 init, test %d: %d\n", test_id, ret);
#endif
        return -1;
    }
    memset(temp, 0x00, OKMLen);
    for (i = 0; i < loop; i++)
    {
        if (i != (loop - 1))
        {
            ret = hkdf_sha256_core(temp + i * SHA256_DIGEST_SIZE, SHA256_DIGEST_SIZE, &sha256_hkdf_ctxt);
        }
        else
        {
            ret = hkdf_sha256_core(temp + i * SHA256_DIGEST_SIZE, last_core_block, &sha256_hkdf_ctxt);
        }
        if (ret != 0)
        {
#ifdef VERBOSE
            printf("Error in hkdf-sha256 core, test %d: %d\n", test_id, ret);
#endif
            return -1;
        }
    }
    if (memcmp(temp, OKM_exp, OKMLen))
    {
#ifdef VERBOSE
        printf("init-core-finish hkdf-sha256, test %d, wrong output\n", test_id);
#endif
        return -1;
    }
    ret = hkdf_sha256_finish(&sha256_hkdf_ctxt);
    if (ret != 0)
    {
#ifdef VERBOSE
        printf("Error in hkdf-sha256 finish, test %d: %d\n", test_id, ret);
#endif
    }
    if (!memcmp(sha256_hkdf_ctxt.state, OKM_exp + (loop - 1) * SHA256_DIGEST_SIZE, last_core_block) || !memcmp(sha256_hkdf_ctxt.PRK, PRK_exp, PRKLen))
    {
#ifdef VERBOSE
        printf("init-core-finish hkdf-sha256, test %d, data not washed\n", test_id);
#endif
        return -1;
    }
#ifdef VERBOSE
    printf("test %d OK()\n", test_id);
#endif

    return 0;

}

int test_hkdf_vectors(void)
{
    // AUTO GENERATED CODE
    static const unsigned char test_sha256_0_IKM[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };
    static const unsigned char test_sha256_0_SALT[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };
    static const unsigned char test_sha256_0_INFO[] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9 };
    static const unsigned char test_sha256_0_PRK[] = { 0x07, 0x77, 0x09, 0x36, 0x2c, 0x2e, 0x32, 0xdf, 0x0d, 0xdc, 0x3f, 0x0d, 0xc4, 0x7b, 0xba, 0x63, 0x90, 0xb6, 0xc7, 0x3b, 0xb5, 0x0f, 0x9c, 0x31, 0x22, 0xec, 0x84, 0x4a, 0xd7, 0xc2, 0xb3, 0xe5 };
    static const unsigned char test_sha256_0_OKM[] = { 0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf, 0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65 };
    static const unsigned char test_sha256_1_IKM[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
    static const unsigned char test_sha256_1_SALT[] = { 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf };
    static const unsigned char test_sha256_1_INFO[] = { 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
    static const unsigned char test_sha256_1_PRK[] = { 0x06, 0xa6, 0xb8, 0x8c, 0x58, 0x53, 0x36, 0x1a, 0x06, 0x10, 0x4c, 0x9c, 0xeb, 0x35, 0xb4, 0x5c, 0xef, 0x76, 0x00, 0x14, 0x90, 0x46, 0x71, 0x01, 0x4a, 0x19, 0x3f, 0x40, 0xc1, 0x5f, 0xc2, 0x44 };
    static const unsigned char test_sha256_1_OKM[] = { 0xb1, 0x1e, 0x39, 0x8d, 0xc8, 0x03, 0x27, 0xa1, 0xc8, 0xe7, 0xf7, 0x8c, 0x59, 0x6a, 0x49, 0x34, 0x4f, 0x01, 0x2e, 0xda, 0x2d, 0x4e, 0xfa, 0xd8, 0xa0, 0x50, 0xcc, 0x4c, 0x19, 0xaf, 0xa9, 0x7c, 0x59, 0x04, 0x5a, 0x99, 0xca, 0xc7, 0x82, 0x72, 0x71, 0xcb, 0x41, 0xc6, 0x5e, 0x59, 0x0e, 0x09, 0xda, 0x32, 0x75, 0x60, 0x0c, 0x2f, 0x09, 0xb8, 0x36, 0x77, 0x93, 0xa9, 0xac, 0xa3, 0xdb, 0x71, 0xcc, 0x30, 0xc5, 0x81, 0x79, 0xec, 0x3e, 0x87, 0xc1, 0x4c, 0x01, 0xd5, 0xc1, 0xf3, 0x43, 0x4f, 0x1d, 0x87 };
    static const unsigned char test_sha256_2_IKM[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };
    static const unsigned char *test_sha256_2_SALT = NULL;
    static const unsigned char *test_sha256_2_INFO = NULL;
    static const unsigned char test_sha256_2_PRK[] = { 0x19, 0xef, 0x24, 0xa3, 0x2c, 0x71, 0x7b, 0x16, 0x7f, 0x33, 0xa9, 0x1d, 0x6f, 0x64, 0x8b, 0xdf, 0x96, 0x59, 0x67, 0x76, 0xaf, 0xdb, 0x63, 0x77, 0xac, 0x43, 0x4c, 0x1c, 0x29, 0x3c, 0xcb, 0x04 };
    static const unsigned char test_sha256_2_OKM[] = { 0x8d, 0xa4, 0xe7, 0x75, 0xa5, 0x63, 0xc1, 0x8f, 0x71, 0x5f, 0x80, 0x2a, 0x06, 0x3c, 0x5a, 0x31, 0xb8, 0xa1, 0x1f, 0x5c, 0x5e, 0xe1, 0x87, 0x9e, 0xc3, 0x45, 0x4e, 0x5f, 0x3c, 0x73, 0x8d, 0x2d, 0x9d, 0x20, 0x13, 0x95, 0xfa, 0xa4, 0xb6, 0x1a, 0x96, 0xc8 };
    // AUTO GENERATED CODE END

    printf("HKDF Test\n");

    if (test_hkdf_helper_sha256((unsigned char*) test_sha256_0_IKM, sizeof(test_sha256_0_IKM), (unsigned char*) test_sha256_0_SALT, sizeof(test_sha256_0_SALT), (unsigned char*) test_sha256_0_INFO, sizeof(test_sha256_0_INFO), (unsigned char*) test_sha256_0_PRK, sizeof(test_sha256_0_PRK), (unsigned char*) test_sha256_0_OKM, sizeof(test_sha256_0_OKM), 1) != 0)
    {
        return -1;
    }

    if (test_hkdf_helper_sha256((unsigned char*) test_sha256_1_IKM, sizeof(test_sha256_1_IKM), (unsigned char*) test_sha256_1_SALT, sizeof(test_sha256_1_SALT), (unsigned char*) test_sha256_1_INFO, sizeof(test_sha256_1_INFO), (unsigned char*) test_sha256_1_PRK, sizeof(test_sha256_1_PRK), (unsigned char*) test_sha256_1_OKM, sizeof(test_sha256_1_OKM), 2) != 0)
    {
        return -1;
    }

    if (test_hkdf_helper_sha256((unsigned char*) test_sha256_2_IKM, sizeof(test_sha256_2_IKM), (unsigned char*) test_sha256_2_SALT, sizeof(test_sha256_2_SALT), (unsigned char*) test_sha256_2_INFO, sizeof(test_sha256_2_INFO), (unsigned char*) test_sha256_2_PRK, sizeof(test_sha256_2_PRK), (unsigned char*) test_sha256_2_OKM, sizeof(test_sha256_2_OKM), 3) != 0)
    {
        return -1;
    }


    return 0;
}
