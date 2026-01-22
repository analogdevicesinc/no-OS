/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
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
******************************************************************************/

/**
 * @internal
 *
 * @file
 * @date Apr, May 2015
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 */


/**
 *
 *
 * @defgroup API                  Host API
 * The MAXQ10xx API is relatively straightforward as functions defined here almost map to
 * commands defined in [UG67B02].
 *
 * @defgroup Types                Base type definitions
 * This section contains the base types used by the API.
 * @ingroup API
 *
 * @defgroup Administration       MAXQ10xx Management
 * The functions described in this section allow to globally manage the Security module.
 * @ingroup API
 *

 * @defgroup Communication      Communication
 * Functions related to communication with the MAXQ10xx.
 * @ingroup API

 * @defgroup ObjectManagement      Object Management
 * Functions of this section allow to read/write objects in the MAXQ10xx.
 * @ingroup API


 * @defgroup Security       Security management
 * The functions described herein relate to secure channel management, administrator
 * authentication, host boot verification.
 * @ingroup API
 *
 * @defgroup KeyManagement        Key Management and cryptographic functions
 * Functions of this section allow to manage key objects and also perform
 * cryptographic    operations.
 * @ingroup API
 *
 * @defgroup Cipher        Encryption/Decryption functions
 * @ingroup Cryptotools
 *
 * @defgroup MAC        MAC generation/verification functions
 * @ingroup Cryptotools

 * @defgroup MessageDigest        Message Digest functions
 * @ingroup Cryptotools

 * @defgroup Signature        Digital signature functions
 * @ingroup Cryptotools

 * @defgroup TLS        TLS support functions
 * @ingroup API

 * @defgroup Ciphersuites        Supported TLS cipher suites
 * List of the supported TLS cipher suites
 * @ingroup TLS

 * @defgroup Utility      Utility functions
 * Misc. utility functions
 * @ingroup API

 * @defgroup Random Random numbers
 * Functions related to the generation of random numbers.
 * @ingroup Cryptotools
 *
 * @defgroup Platform             Platform dependent functions
 * The functions described in this module may have to be implemented by the platform.

 * @note The note "This function must be implemented by the API user" means that
 * the function needs to be implemented to use some platform specific hardware.
 * For example, sending bytes over the SPI bus has to be implemented.

 *
 * @defgroup Cryptotools Cryptographic toolbox
 * Cryptography related functions.
 * @ingroup API
 * @internal
 
 * @defgroup Internals            MAXQ10xx host library internal functions
 *
 * @defgroup Crypto               Cryptographic toolkit, key management.
 *
 * @defgroup Transport            Transport protocol layer
 * Provides the block oriented transport protocol used to convey APDU commands/responses over I2C or SPI.
 * @ingroup Internals
 *
 */



/**
 * @page Revhist API Specification Revision History
 *
 | Revision |    Date    |              Description              |
 |----------|------------|---------------------------------------|
 | A        | 12/20/2014 | Initial  draft                        |
 | B        | 04/25/2015 | In line with MAXQ Host Library v2.0.0 |
 | C        | 21/01/2017 | In line with MAXQ Host Library v2.1.0 |
 | D        | 05/03/2018 | In line with MAXQ Host Library v3.0.0 |
 | E        | 07/13/2018 | Reorganization of the documentation, in line with API library 5.1.0   |

 * @page REFERENCES References
 *

 * **MAXQ1061/2**

 * @anchor UG6371  [UG6371] MAXQ106x User guide revision 2 - Maxim Integrated

 * @anchor DS67H06 [DS67H06] MAXQ106x EVKIT datasheet - Maxim Integrated

 * @anchor DS1061      [DS8718] MAXQ1061/2 datasheet - Maxim Integrated

 * @anchor ER1061      [ER67H08] MAXQ1061/2 rev C3 errata - Maxim Integrated

 * @anchor WEB1061     [web]  https://www.maximintegrated.com/en/products/microcontrollers/MAXQ1061.html

 * **MAXQ1065**

 * @anchor UG6XXX  [UG6XXX] MAXQ1065 User guide - Maxim Integrated

 * @anchor DS1065      [DSXXXX] MAXQ1065 datasheet - Maxim Integrated

 * @anchor DSXXXX[DSXXXX] MAXQ1065 EVKIT datasheet - Maxim Integrated

 * @anchor WEB1065     [web]  https://www.maximintegrated.com/en/products/microcontrollers/MAXQ1065.html
 * @n


 * @page RELEASE_NOTES Software release notes
 * 
 * # Library Revision History
 | Revision |    Date    |                             Description                                         |
 |----------|------------|---------------------------------------------------------------------------------|
 | 0.1.0    | 12/20/2014 | Initial API draft                                                               |
 | 2.0.0    | 04/25/2015 | In line with MAXQ106x Samples revA3                                             |
 | 2.1.0    | 21/01/2017 | In line with MAXQ106x rev C2                                                    |
 | 3.0.0    | 05/03/2018 | In line with MAXQ106x rev C3                                                    |
 | 5.1.0    | 07/12/2018 | See below                                      |
 | 5.1.1    | 07/14/2018 | See below                                      |
 | 5.1.2    | 07/31/2018 | See below                                      |
 | 5.1.3    | 08/06/2018 | See below                                      |
 | 5.1.4    | 09/01/2018 | See below                                      |
 | 5.1.5    | 12/11/2018 | See below                                      |
 | 5.4.1    | 12/12/2020 | See below                                      |
 *
 *
 *
 * Release details:

 * **5.4.1**
    - MAXQ1065 Support

 * **5.1.5**
    - Fixes AES-CCM usage

 * **5.1.4**
    - add AES SPI support in API and toolbox
    - add ECIES support in API and toolbox
 
 * **5.1.3**
    - Refine max data in/out limits. Add MXQ_Get_MD_OutputSize. Add Totalphase Cheetah support.
 * **5.1.2**
    - Fixes and clarifications in the documentation
    - Compile time verification of the compiler's endianess
    - Get status fixed due to some compilers not respecting the packed attribute of structures

 * **5.1.1**
    - Documentation improved

 * **5.1.0**
    - Addition of doxygen HTML documentation for the API
    - Create object function now accepts access rules with an easy to understand syntax
    - Addition of reset and wakeup functions 
    - Fixed the issue where the first communication attempt with the MAXQ106x failed (raspberry)
 *
 * 
 *    
 * @page Support Support contact
 * Please contact your Maxim representative for further support.
 *

 * @page Legal Legal Information
 *
 * @section Software License
*
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
 *
 * @note This software includes third-party components. Please refer to the file
 * LICENSE for more information.
 *
 *
 *
 *
 * @section Notice
 *
 * MAXIM INTEGRATED PROPRIETARY – CONFIDENTIALITY
 *
 * This document contains confidential information that is the strict ownership
 * of Maxim Integrated, and may be disclosed only under the writing agreement of
 * Maxim Integrated itself. Any copy, reproduction, modification, use or disclose
 * of the whole or only part of this document if not expressly authorized by
 * Maxim Integrated is prohibited. This information is protected under trade
 * secret, unfair competition and copying laws. This information has been
 * provided under a Non Disclosure Agreement. Violations thereof may result
 * in criminalities and fines.
 *
 * Maxim Integrated reserves the right to change the information contained in
 * this document to improve design, description or otherwise. Maxim Integrated
 * does not assume any liability arising out of the use or application of this
 * information, or of any error of omission in such information.
 * Except if expressly provided by Maxim Integrated in any written license
 * agreement, the furnishing of this document does not give recipient any
 * license to any intellectual property rights, including any patent rights
 * covering the information in this document.
 *
 * All trademarks referred to this document are the property of their respective owners.
 *
 * Copyright ©2014-2018 Maxim Integrated. All rights reserved.
 *
 * @section Disclaimer
 * <b>To our valued customers</b>
 *
 * We constantly strive to improve the quality of all our products and documentation. We have spent an exceptional amount of time to ensure that this document is correct. However, we realize that we may have missed a few things. If you find any information that is missing or appears in error, please send us an email via www.maximintegrated.com/support. We appreciate your assistance in making this a better document.
 *
 * <b>Warnings</b>
 *
 * Due to technical requirements components may contain dangerous substances. For information on the types in question please contact your nearest Maxim Integrated Office.
 * Maxim Integrated technologies may only be used in life-support devices or systems with the express written approval of Maxim Integrated, if a failure of such components can reasonably be expected to cause the failure of that life-support device or system, or to affect the safety or effectiveness of that device or system. Life support devices or systems are intended to be implanted in the human body, or to support and/or maintain and sustain and/or protect human life. If they fail, it is reasonable to assume that the health of the user or other persons may be endangered.
 *
 */

#ifndef __MXQ_DOC_H_INCLUDED__
#define __MXQ_DOC_H_INCLUDED__

#endif /* __MXQ_DOC_H_INCLUDED__ */
