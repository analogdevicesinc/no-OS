//-----------------------------------------------------------------------
// <summary>
// Describe File
// </summary>
// <author>
// Jason Cockrell
// </author>
// <copyright file="adi_pmag_macros.h" company="Analog Devices Inc">
//     Copyright (c) Analog Devices Inc. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

#define JSON_PARSING 1
#ifdef JSON_PARSING

#ifdef __KERNEL__
#include <linux/kernel.h>

static inline int atoi(const char *str)
{
    int val = 0, ret;

    ret = kstrtoint(str, 10, &val);
    if (ret)
    {
        return ret;
    }

    return val;
}
#endif

#define ADI_ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define ADI_IF_JSON_EQ(jsonBuffer, jsmnTok, token) \
if (jsmnTok.type == JSMN_STRING && (int)strlen(token) == jsmnTok.end - jsmnTok.start && \
    strncmp(jsonBuffer + jsmnTok.start, token, jsmnTok.end - jsmnTok.start) == 0)

#define COPY_TOKEN_TO_BUFFER(tokenArray, tokenIndex, jsonBuffer, copyBuffer) \
snprintf(copyBuffer, \
    ADI_ARRAY_LEN(copyBuffer), \
    "%.*s", tokenArray[tokenIndex + 1].end - tokenArray[tokenIndex + 1].start, \
    jsonBuffer + tokenArray[tokenIndex + 1].start);

#define ADI_STORE_INT(tokenArray, tokenIndex, jsonBuffer, intParsingBuffer, intDestination) \
COPY_TOKEN_TO_BUFFER(tokenArray, tokenIndex, jsonBuffer, intParsingBuffer) \
intDestination = atoi(intParsingBuffer);

#define ADI_STORE_BOOL(tokenArray, tokenIndex, jsonBuffer, boolParsingBuffer, boolDestination) \
COPY_TOKEN_TO_BUFFER(tokenArray, tokenIndex, jsonBuffer, boolParsingBuffer) \
boolDestination = ('0' != boolParsingBuffer[0]) && ('f' != boolParsingBuffer[0]) && ('F' != boolParsingBuffer[0]);

#define ADI_STORE_STR(tokenArray, tokenIndex, jsonBuffer, dummyBuffer, strDestination) \
COPY_TOKEN_TO_BUFFER(tokenArray, tokenIndex, jsonBuffer, strDestination);

#define ADI_PROCESS_X(X, tokenArray, tokenIndex, jsonBuffer, parsingBuffer, varDestination, varName) \
ADI_IF_JSON_EQ(jsonBuffer, tokenArray[tokenIndex], varName) { \
    X(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, varDestination); \
    tokenIndex++; \
    continue; \
}

#define ADI_PROCESS_INT(tokenArray, tokenIndex, jsonBuffer, intParsingBuffer, intDestination, intName) \
ADI_PROCESS_X(ADI_STORE_INT, tokenArray, tokenIndex, jsonBuffer, intParsingBuffer, intDestination, intName)

#define ADI_PROCESS_BOOL(tokenArray, tokenIndex, jsonBuffer, boolParsingBuffer, boolDestination, boolName) \
ADI_PROCESS_X(ADI_STORE_BOOL, tokenArray, tokenIndex, jsonBuffer, boolParsingBuffer, boolDestination, boolName)

#define ADI_PROCESS_STR(tokenArray, tokenIndex, jsonBuffer, dummyBuffer, strDestination, strName) \
ADI_PROCESS_X(ADI_STORE_STR, tokenArray, tokenIndex, jsonBuffer, dummyBuffer, strDestination, strName)

#define ADI_PROCESS_ARRAY_X(X, tokenArray, tokenIndex, jsonBuffer, parsingBuffer, arrayName) \
ADI_IF_JSON_EQ(jsonBuffer, tokenArray[tokenIndex], arrayName) { \
    int16_t size = tokenArray[++tokenIndex].size; \
    int16_t k = 0; \
    for (k = 0; k < size; k++) { \
        X; \
        tokenIndex++; \
    } \
    continue; \
}

#define ARRAY_STRUCT_INDEXER(structName) structName##_k

#define ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, intParsingBuffer, intArrayDestination, intArrayName) \
ADI_PROCESS_ARRAY_X(ADI_STORE_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, intArrayDestination[k]), tokenArray, tokenIndex, jsonBuffer, intParsingBuffer, intArrayName)

#define ADI_PROCESS_STRUCT_X(X, tokenArray, tokenIndex, jsonBuffer, parsingBuffer, structName) \
ADI_IF_JSON_EQ(jsonBuffer, tokenArray[tokenIndex], structName) { \
    int16_t size = tokenArray[++tokenIndex].size; \
    int16_t end = tokenArray[tokenIndex].end; \
    { \
        int16_t j = 0; \
        for (j = 0; j < size; j++) { \
            tokenIndex++; \
            \
            X; \
        } \
    } \
    while (tokenArray[tokenIndex + 1].start <= end) { \
        tokenIndex++; \
    } \
    continue; \
}

#define ADI_PROCESS_ARRAY_STRUCT_X(X, tokenArray, tokenIndex, jsonBuffer, parsingBuffer, structName, indexName) \
ADI_IF_JSON_EQ(jsonBuffer, tokenArray[tokenIndex], structName) { \
    int16_t size = tokenArray[++tokenIndex].size; \
    int16_t indexName = 0; \
    for (indexName = 0; indexName < size; indexName++) { \
        int16_t end, size = tokenArray[++tokenIndex].size; \
        end = tokenArray[tokenIndex].end; \
        { \
            int16_t j = 0; \
            for (j = 0; j < size; j++) { \
                tokenIndex++; \
                X; \
            } \
        } \
        while (tokenArray[tokenIndex + 1].start <= end) { \
            tokenIndex++; \
        } \
    } \
    continue; \
}
#endif
