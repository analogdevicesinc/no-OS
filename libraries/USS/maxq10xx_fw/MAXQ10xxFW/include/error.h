/**
 * File system services error status.
 * @ingroup File_System
 */
/*******************************************************************************
 * Copyright (C) Maxim Integrated Products, Inc., All rights Reserved.
 *
 * This software is protected by copyright laws of the United States and
 * of foreign countries. This material may also be protected by patent laws
 * and technology transfer regulations of the United States and of foreign
 * countries. This software is furnished under a license agreement and/or a
 * nondisclosure agreement and may only be used or reproduced in accordance
 * with the terms of those agreements. Dissemination of this information to
 * any party or parties not specified in the license agreement and/or
 * nondisclosure agreement is expressly prohibited.
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *******************************************************************************
 */

#ifndef INCLUDE_ERROR_H_
#define INCLUDE_ERROR_H_

/*
 * TODO: This should be refactored to comply with
 *       naming convention
 */
typedef enum _PACKED {

    eCmdRespOk                  = 0x0000,
    eCmdRespErrorUnknown        = 0x1000,
    eCmdRespSecCondNotSatisfied = 0x2000,
    eCmdRespNoHashAvail         = 0x2001,
    eCmdRespNoRandAvail         = 0x2002,
    eCmdRespWrongMAC            = 0x2002,

    eCmdRespAccDenied           = 0xfffa,
    eCmdRespWrongParam          = 0xfffb,
    eCmdRespWrongLength         = eCmdRespErrorUnknown,


    eMaxQInternalBufferOver     = ( 0xA000 ),

    eMaxQBadParam               = ( 0x0001 ),

    // FS errors
    eErrFsIdAlreadyExist        = (u2) 0x0001U,
    eErrFsWrongParamAccessCond  = (u2) 0x0002U,
    eErrFsWrongObjType          = (u2) 0x0003U,
    eErrFsNotFound              = (u2) 0x0004U,
    eErrFsOutOfBound            = (u2) 0x0005U,
    eErrFsUnknownError          = (u2) 0x1000U,
    eErrFsMemoryFull            = (u2) 0x1001U,
    eErrMemError                = (u2) 0x3000U,
    eErrFsNvmAccessError        = (u2) 0x8004,
    eErrFsNvmAlignError         = (u2) 0x8004,
    eErrNvrPageDisabled,
    eErrNvrPadeLocked,
    eErrFsWrongParam            = (u2) 0x8006,
    eErrFsAccessDenied          = (u2) 0x2001,
    eErrFsWrongSize             = (u2) 0x800D,
    eErrFsNameAlreadyExists     = (u2) 0x8016,
    eErrFsInvalidObject         = (u2) 0x8017,
    eErrFsConsistencyError      = (u2) 0x9000,
    eErrFsObjectEmpty           = (u2) 0x9001U,
    eErrBufferOverflow          = (u2) 0xC000,
    eErrNotImplemented          = 0x5000,
    eErrUnspecified             = (u2) 0x2000U,
    eErrOk                      = (u2) 0x0000U,
    eErrScError                 = (u2) 0x4000U,
    eErrNotAllowed              = (u2) 0x6000U,
    eErrCounterOverflow         = (u2) 0x8030U,
    eErrOkWrongParam            = (u2) 0xC030U,

    eErrCrBadKeyType            = 0x8001U,
    eErrCrCryptoFailure         = 0x8002U,
    eErrCrAlgoNotSupported      = 0x8004U,
    eErrCrSignatureIsWrong      = (u2) 0x3000U,
    eErrCrPaddingError          = 0x800AU,
    eErrCrBufferTooSmall        = 0x8011U,
    eErrCrHashOutputTooShort    = 0x8012U,
    eErrCrCurveNotSupported     = 0x8013U,
    eErrCrCipherBadInput        = 0x6100U,

    eErrI2CError,
    eErrSystemNoAESKey,
    eErrSetAESKeyFailed,

    eCmdRxError                 = (s2) 0x8001, //!< APDU_RX_ERROR
    eCmdTxError                 = (s2) 0x8005, //!< APDU_TX_ERROR
    eCmdBadApiCall              = (s2) 0x8002, //!< APDU_BAD_API_CALL
    eErrRxTimeout,
    eErrTxTimeout,

    eLlError                    = 0x8001,
    eLlBuffOverflow             = 0x8002,

    eCmdRespUnknownCmd          = 0xffff

// UCL: 0K, -1..-99 other errors see ucl_error.h

} FN_STATUS;

#endif /* INCLUDE_ERROR_H_ */
