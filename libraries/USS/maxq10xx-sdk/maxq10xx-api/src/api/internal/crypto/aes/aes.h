/*
* ---------------------------------------------------------------------------
* OpenAES License
* ---------------------------------------------------------------------------
* Copyright (c) 2013, Nabil S. Al Ramli, www.nalramli.com
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* - Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
* ---------------------------------------------------------------------------
*/

#ifndef AES_H_
#define AES_H_

#include <stdint.h>

#ifdef __cplusplus

extern "C" {

#endif

#ifdef _WIN32
#  ifdef OAES_SHARED
#    ifdef oaes_lib_EXPORTS
#      define OAES_API __declspec(dllexport)
#    else
#      define OAES_API __declspec(dllimport)
#    endif
#  else
#    define OAES_API
#  endif
#else
#  define OAES_API
#endif /*  WIN32 */

#define OAES_VERSION "0.10.0"

typedef enum
{
	OAES_RET_FIRST = 0,
	OAES_RET_SUCCESS = 0,
	OAES_RET_ERROR,
	OAES_RET_ARG1,
	OAES_RET_ARG2,
	OAES_RET_ARG3,
	OAES_RET_ARG4,
	OAES_RET_ARG5,
	OAES_RET_ARG6,
	OAES_RET_ARG7,
	OAES_RET_NOKEY,
	OAES_RET_MEM,
	OAES_RET_BUF,
	OAES_RET_HEADER,
	OAES_RET_COUNT
} OAES_RET;

#define OAES_BLOCK_SIZE 16

typedef void OAES_CTX;

/*
 * oaes_set_option() takes one of these values for its [option] parameter
 * some options accept either an optional or a required [value] parameter
 */

/*  no option */
#define OAES_OPTION_NONE 0

/*  enable ECB mode, disable CBC mode */
#define OAES_OPTION_ECB 1

/*  enable CBC mode, disable ECB mode */
/*  value is optional, may pass uint8_t iv[OAES_BLOCK_SIZE] to specify */
/*  the value of the initialization vector, iv */
#define OAES_OPTION_CBC 2

#ifdef OAES_DEBUG

typedef int ( * oaes_step_cb ) (
    const uint8_t state[OAES_BLOCK_SIZE],
    const char * step_name,
    int step_count,
    void * user_data );

/*  enable state stepping mode */
/*  value is required, must pass oaes_step_cb to receive the state at each step */
#define OAES_OPTION_STEP_ON 4

/*  disable state stepping mode */
#define OAES_OPTION_STEP_OFF 8

#endif /*  OAES_DEBUG */

typedef uint16_t OAES_OPTION;

#ifdef __cplusplus
}
#endif

#endif /* AES_H_ */
