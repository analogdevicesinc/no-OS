/**
 * \file adi_device_profile_pack.h
 *
 * \brief   Contains platform pack define
 *
 * \details Contains platform pack define
 */

#ifndef __ADI_NEVIS_PLATFORM_PACK_H__
#define __ADI_NEVIS_PLATFORM_PACK_H__

/*!
  \def ADI_NEVIS_SAFE_PTR(ptr, sourcePtr, itemType)
  \brief Assign \p sourcePtr to \p ptr if \p sourcePtr is memory aligned, otherwise assign NULL.
  \param ptr The pointer to assign to. This will be set to NULL if /p sourcePtr is not aligned.
  \param sourcePtr The pointer that may be misaligned to assign from.
  \param itemType The type the pointer is pointing to.
*/

#if (!defined(ADI_NEVIS_PACK_START) || !defined(ADI_NEVIS_PACK_FINISH) || !defined(ADI_NEVIS_PACK_ENUM))
#ifdef __GNUC__
#define ADI_NEVIS_SAFE_PTR_GET(ptr, sourcePtr, itemType) do {   \
uint32_t alignMask = (sizeof(itemType) < __alignof__(uint32_t)) \
    ? (0x01u << __alignof__(itemType)) - 1                      \
    : (0x01u << __alignof__(uint32_t)) - 1;                     \
if (((uintptr_t)(const void *)(ptr) & alignMask) == 0)          \
  (ptr) = (sourcePtr);                                          \
else                                                            \
  (ptr) = NULL;                                                 \
} while (0)
//#define ADI_NEVIS_PACK_ENUM __attribute__((packed))
#define ADI_NEVIS_PACK_ENUM
#define ADI_NEVIS_PACK_START     _Pragma("pack(push, 4)")
#define ADI_NEVIS_PACK_FINISH    _Pragma("pack(pop)")
#elif defined  __ICCARM__
#define ADI_NEVIS_SAFE_PTR_GET(ptr, sourcePtr, itemType) do {   \
_Pragma("diag_suppress=Pa039");                                 \
uint32_t alignMask = (sizeof(itemType) < __ALIGNOF__(uint32_t)) \
    ? (0x01u << __ALIGNOF__(itemType)) - 1                      \
    : (0x01u << __ALIGNOF__(uint32_t)) - 1;                     \
if (((uintptr_t)(const void *)(ptr) & alignMask) == 0)          \
  (ptr) = (sourcePtr);                                          \
else                                                            \
  (ptr) = NULL;                                                 \
_Pragma("diag_default=Pa039");                                  \
} while (0)
#define ADI_NEVIS_PACK_ENUM
#define ADI_NEVIS_PACK_START     _Pragma("pack(push, 4)")
#define ADI_NEVIS_PACK_FINISH    _Pragma("pack(pop)")
#elif defined _MSC_VER
#pragma message( "WARNING: Check Enums are packed or force unpacked for all compilers!" )
#define ADI_NEVIS_SAFE_PTR_GET(ptr, sourcePtr, itemType) do {   \
uint32_t alignMask = (sizeof(itemType) < alignof(uint32_t))     \
    ? (0x01u << alignof(itemType)) - 1                          \
    : (0x01u << alignof(uint32_t)) - 1;                         \
if (((uintptr_t)(const void *)(ptr) & alignMask) == 0)          \
  (ptr) = (sourcePtr);                                          \
else                                                            \
  (ptr) = NULL;                                                 \
} while (0)
#define ADI_NEVIS_PACK_ENUM
#define ADI_NEVIS_PACK_START     __pragma(pack(push, 4))
#define ADI_NEVIS_PACK_FINISH    __pragma(pack(pop))
#else
#define ADI_NEVIS_PACK_ENUM
#define ADI_NEVIS_PACK_START
#define ADI_NEVIS_PACK_FINISH
#error ( "Define the ADI_NEVIS_PACK_START and ADI_NEVIS_PACK_FINISH macros for your compiler." )
#endif
#endif

#ifndef ADI_NEVIS_PACKED
#if defined  __ICCARM__
  /*
   * Error[Pm154]: in the definition of a function-like macro, each instance of a
   * parameter shall be enclosed in parenthesis (MISRA C 2004 rule 19.10)
   *
   * ADI_NEVIS_PACKED() is a macro used for structure packing. The parameter
   * for this macro must be a structure definition, which cannot be enclosed in
   * parenthesis (syntatically invalid).
   */
#pragma diag_suppress=Pm154
#define ADI_NEVIS_PACKED(d)    ADI_NEVIS_PACK_START d ADI_NEVIS_PACK_FINISH
#pragma diag_default=Pm154
#else
#define ADI_NEVIS_PACKED(d)    ADI_NEVIS_PACK_START d ADI_NEVIS_PACK_FINISH
#endif
#endif

#endif /* __ADI_NEVIS_PLATFORM_PACK_H__ */
