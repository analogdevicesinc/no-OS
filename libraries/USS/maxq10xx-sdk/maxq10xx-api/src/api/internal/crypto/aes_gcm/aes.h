/*
 * AES functions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef AES_H
#define AES_H

#define AES_BLOCK_SIZE 16

#define _DISABLE_DYNAMIC_MEM_USE_

#ifdef _DISABLE_DYNAMIC_MEM_USE_
void * aes_encrypt_init(const u8 *key, size_t len, u32 *rk);
void * aes_decrypt_init(const u8 *key, size_t len, u32 *rk);
#else
void * aes_encrypt_init(const u8 *key, size_t len);
void * aes_decrypt_init(const u8 *key, size_t len);
#endif

void mxq_aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void aes_encrypt_deinit(void *ctx);
void aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void aes_decrypt_deinit(void *ctx);

#endif /* AES_H */
