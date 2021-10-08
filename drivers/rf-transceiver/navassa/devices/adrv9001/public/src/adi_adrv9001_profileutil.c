/** 
* \file 
* \brief Functions for working with ADRV9001 profiles
* 
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$ 
*/ 
 
/** 
* Copyright 2020 Analog Devices Inc. 
* Released under the ADRV9001 API license, for more information 
* see the "LICENSE.txt" file in this zip file. 
*/

#ifdef __KERNEL__ 
#include <linux/kernel.h> 
#include <linux/slab.h> 
 
#ifndef free 
#define free kfree 
#endif 
 
#ifndef calloc 
#define calloc(n, s) kcalloc(n, s, GFP_KERNEL) 
#endif 
 
#else 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#endif 

#include "jsmn.h"

#include "adi_adrv9001_profileutil.h"
#include "adrv9001_Init_t_parser.h"

#ifdef ADI_DYNAMIC_PROFILE_LOAD

#define ADI_ADRV9001_TOKEN_MAX_LENGTH	32

int32_t adi_adrv9001_profileutil_Parse(adi_adrv9001_Device_t *device, adi_adrv9001_Init_t *init, char * jsonBuffer, uint32_t length) 
{
    uint16_t ii = 0; 
    int16_t numTokens = 0; 
    jsmn_parser parser = { 0 }; 
    jsmntok_t * tokens = NULL; 
    char parsingBuffer[ADI_ADRV9001_TOKEN_MAX_LENGTH]; /* This buffer only needs to hold a stringified number like '123.4567'. */ 
 
 
    /* initialize the JSMN parser and determine the number of JSON tokens */ 
    jsmn_init(&parser); 
    numTokens = jsmn_parse(&parser, jsonBuffer, length, NULL, 0); 
 
    /* The JSON file must be tokenized successfully. */ 
    if (numTokens < 1) 
    { 
        ADI_ERROR_REPORT(&device->common, 
                         ADI_COMMON_ERRSRC_API, 
                         ADI_COMMON_ERR_INV_PARAM, 
                         ADI_COMMON_ACT_ERR_CHECK_PARAM, 
                         NULL, 
                         "Fatal error while parsing profile file. The JSON may be invalid, or the token buffer may be too small."); 
        ADI_ERROR_RETURN(device->common.error.newAction); 
    } 
 
    /* allocate space for tokens */ 
    tokens = (jsmntok_t*)calloc(numTokens, sizeof(jsmntok_t)); 
 
    if (NULL == tokens) 
    { 
        ADI_ERROR_REPORT(&device->common, 
                         ADI_COMMON_ERRSRC_API, 
                         ADI_COMMON_ERR_MEM_ALLOC_FAIL, 
                         ADI_COMMON_ACT_ERR_RESET_FULL, 
                         NULL, 
                         "Fatal error while reading profile file. Possible memory shortage."); 
        ADI_ERROR_RETURN(device->common.error.newAction); 
    } 
 
    /* initialize the JSMN parser and parse the profile file into the tokens array */ 
    jsmn_init(&parser); 
    numTokens = jsmn_parse(&parser, jsonBuffer, length, tokens, numTokens); 
 
    /* The top-level element must be an object. */ 
    if (numTokens < 1 || tokens[0].type != JSMN_OBJECT) 
    { 
        free(tokens); 
        ADI_ERROR_REPORT(&device->common, 
                         ADI_COMMON_ERRSRC_API, 
                         ADI_COMMON_ERR_INV_PARAM, 
                         ADI_COMMON_ACT_ERR_CHECK_PARAM, 
                         NULL, 
                         "Fatal error while parsing profile file. The JSON may be invalid, or the token buffer may be too small."); 
        ADI_ERROR_RETURN(device->common.error.newAction); 
    } 
 
    /* Loop over all keys of the root object, searching for matching fields. */ 
    for (ii = 1; ii < numTokens; ii++) 
    { 
        ADRV9001_INIT_T(tokens, ii, jsonBuffer, parsingBuffer, (*init)); 
    } 
 
    free(tokens); 
    tokens = NULL; 
 
    ADI_API_RETURN(device); 
} 

#endif 
 
