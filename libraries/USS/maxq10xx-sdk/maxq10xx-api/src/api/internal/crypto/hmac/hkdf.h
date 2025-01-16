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

#ifndef HKDF_H
#define HKDF_H

#define PROCESS_ERROR( p_var, p_func )\
    p_var = p_func; \
    if( (p_var) < 0)\
    {\
        return p_var;\
    }
struct hkdf_sha256_ctx
{
	/** Pseudorandom key, length is restricted to the hash size. */
	unsigned char PRK[SHA256_DIGEST_SIZE];
	/** Additional Info. */
	unsigned char *info;
	/** Info Len. */
	unsigned int infoLen;
	/** Counter. */
	unsigned int counter;
    /** Internal State. */
    unsigned char state[SHA256_DIGEST_SIZE];
};

/** <b>HKDF-Extract Function w/ SHA-256</b>.
 * Inputs a keying material and salt, outputs a pseudorandom key of 'HashLen'
 * octets, using HMAC w/ SHA-256, where HashLen is the output size of SHA-256
 * (32 bytes)'
 *
 * @param[out] PRK              a pseudorandom key (of HashLen octets)
 * @param[in]  salt             Optional salt value (a non-secret random value)
 * @param[in]  saltLen          The byte length of salt
 * @param[in]  IKM              Input keying material
 * @param[in]  IKMLen           The byte length of IKM
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256_extract(unsigned char *p_PRK, unsigned char *p_salt, unsigned int p_saltLen, unsigned char *p_IKM, unsigned int p_IKMLen);
/** <b>HKDF-Expand Function w/ SHA-256</b>.
 * Inputs a (pseuodo random) keying material (usually the output of Extract function) and an optinal info.
 * Outputs the Output keying materal of size "OKMLen". This function can expand the length of the input pseudorandom
 * key up to 255*HashLen where HashLen is the output size of the utilized hash function SHA-256 (32 bytes).
 *
 * @param[out] OKM            Output Keying Material
 * @param[in]  OKMLen         The byte length of OKM, must be less than or equal to 255*HashLen
 * @param[in]  PRK            a pseudorandom key (usually output of Expand)
 * @param[in]  PRKLen         The byte length of PRK, must be greater than or equal to HashLen
 * @param[in]  info           optional context and application specific information
 * @param[in]  infoLen        The byte length of info
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256_expand(unsigned char *p_OKM, unsigned int p_OKMLen, unsigned char *p_PRK, unsigned int p_PRKLen, unsigned char *p_info, unsigned int p_infoLen);


/** <b>The HKDF SHA-256 context</b>.*/
typedef struct hkdf_sha256_ctx hkdf_sha256_ctx_t;
/** <b>The Complete Process of HKDF w/ SHA-256</b>.
 * Calls Extract and Expand, respectively. Outputs PRK from Extract and inputs to Expand.
 *
 * @param[out] OKM            Output Keying Material
 * @param[in]  OKMLen         The byte length of OKM, must be less than or equal to 255*HashLen
 * @param[in]  IKM            Input keying material
 * @param[in]  IKMLen         The byte length of IKM
 * @param[in]  info           optional context and application specific information
 * @param[in]  infoLen        The byte length of info
 * @param[in]  salt           Optional salt value (a non-secret random value)
 * @param[in]  saltLen        The byte length of salt
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256(unsigned char *p_OKM, unsigned int p_OKMLen, unsigned char *p_IKM, unsigned int p_IKMLen, unsigned char *p_info, unsigned int p_infoLen, unsigned char *p_salt, unsigned int p_saltLen);
/** <b>HKDF Initialization w/ SHA-256</b>.
 * Calls HKDF-Extract internally and initializes the HKDF context with the provided input.
 * \warning { info is shallow copied. Caller is responsible for durability of it }
 *
 * @param[out] ctxt           Context to be initialized
 * @param[in]  info           optional context and application specific information
 * @param[in]  infoLen        The byte length of info
 * @param[in]  salt           Optional salt value (a non-secret random value)
 * @param[in]  saltLen        The byte length of salt
 * @param[in]  IKM            Input keying material
 * @param[in]  IKMLen         The byte length of IKM
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256_init(hkdf_sha256_ctx_t *p_ctxt, const unsigned char *p_info, unsigned int p_infoLen, unsigned char *p_salt, unsigned int p_saltLen, unsigned char *p_IKM, unsigned int p_IKMLen);
/** <b>HKDF-Expand Core processing w/ SHA-256</b>.
 * Reads the HKDF context, outputs the keying material of requested length by invoking HKDF Expand partially.
 *
 * @param[out] OKM            Output Keying Material
 * @param[in]  OKMLen         The byte length of OKM, total provided OKMLen to core calls must be less than or equal to 255*HashLen
 * @param[in]  ctxt           HKDF context
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256_core(unsigned char *p_OKM, unsigned int p_OKMLen, hkdf_sha256_ctx_t *p_ctxt);
/** <b>HKDF-Finish w/ SHA-256</b>.
 * Clears the context by washing sensitive data like PRK.
 *
 * @param[in]  p_ctxt           HKDF context
 *
 * @return Error code
 *
 * @retval 0                    If no error occurred
 * @retval -1                   Other errors
 */
int hkdf_sha256_finish(hkdf_sha256_ctx_t *p_ctxt);
int test_hkdf_vectors(void);

#endif /* !HKDF_H */