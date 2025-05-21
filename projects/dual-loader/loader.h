/*******************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
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
 *
 ******************************************************************************/

 /**
 * @defgroup  LDR_API ACMIS loader functions.  
 * @brief     API functions of an ACMIS style dual image loader.
 * @{
 */
 
#include <stdint.h>

/** No error, the operation completed successfully. */
#define LDR_SUCCESS                 0x50 

/** The requested operation could not complete because the CRC of the selected
 *  image is incorrect.
 */
#define LDR_IMAGE_NOT_VALID         0xD0

/** The requested operation could not complete because the target imagae is
 *  already running. 
 */
#define LDR_IMAGE_RUNNING           0xD1

/** The requested operation could not complete because a download session is
 *  currently active. 
 */
#define LDR_DOWNLOAD_IN_PROGRESS    0xD2

/** The requested operation could not complete because an active download
 *  session has not been started. 
 */
#define LDR_DOWNLOAD_NOT_STARTED    0xD3

/** The requested operation could not complete because an error occured while
 *  attempting to modify flash contents.
 */
#define LDR_FLASH_ERROR             0xD4

/** The requested operation could not complete because of an unexpected error.
 */
#define LDR_UNKNOWN_ERROR           0xFF

 /**
 * @brief   Structure holding the state of each image, which image is currently
 *          running, and which image is currently committed.
 */
typedef struct {
    /**
     * @brief   Indication of which image is active and which is committed.
     *
     *  * Bit 7 holds the currently active image: 0 = image A, 1 = image B.
     *  * Bit 6 holds the currently committed image: 0 = image A, 1 = image B.
     *  * Bits 5:0 are unused.
     */
    uint8_t active_commit_status;
    
    /**
     * @brief   Indication of each image's status.
     *
     *  * Bits 7:6 are unused.
     *  * Bits 5:4 hold the status of image A.
     *  * Bits 3:2 are unused.
     *  * Bits 1:0 hold the status of image B.
     *
     * The possible values for the status of each image are:
     *      * 0: No image
     *      * 1: Image valid
     *      * 2: Image invalid (bad CRC)
     *      * 3: Reserved
     */
    uint8_t image_status;
} ldr_image_status_t;

/** 
 * @brief   Prepare the module to receive firmware upgrade data.  
 *
 * Erases/invalidates the currently non-commited image and prepares that 
 * image's area for receiving a new image.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_IMAGE_RUNNING - The non-committed image is currently the 
 *                                     active running image.
 *              * #LDR_DOWNLOAD_IN_PROGRESS - There is already an active
 *                                            download session.
 *              * #LDR_FLASH_ERROR - An error occurred while erasing the flash.
 */
int LDR_StartDownload(void);

/**
 * @brief   Write a data block to the module for firmware upgrade.
 *
 * @param   offset      The offset into the image area to write the data.
 * @param   buff        A pointer to the data to write.
 * @param   length      The length (in bytes) of the data to write.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 *              * #LDR_FLASH_ERROR - An error occurred while writing the flash.
 */
int LDR_DownloadImageBlock(uint32_t offset, uint8_t* buff, uint32_t length);

/**
 * @brief   Abort firmware upgrade process.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 */
int LDR_AbortUpgrade(void);

/**
 * @brief   Complete the download process of an image.  
 *
 * Verifies the complete image was written and its CRC is correct.  If both
 * conditions are satisfied the image will be marked as valid.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 *              * #LDR_IMAGE_NOT_VALID - The CRC of the received image is 
 *                                       incorrect.
 */
int LDR_CompleteDownload(void);

/**
 * @brief   Execute image A.
 *
 * If image A is valid, this function will not return.  Instead execution will
 * be transferred to image A as if image A was executed from the reset state.
 * If image A is already the active image, this function will restart the image
 * from its reset state.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image A.
 *              * #LDR_IMAGE_NOT_VALID - The CRC of image A is incorrect.
 */
int LDR_RunImageA(void);

/**
 * @brief   Execute image B.
 *
 * If image B is valid, this function will not return.  Instead execution will
 * be transferred to image B as if image B was executed from the reset state.
 * If image B is already the active image, this function will restart the image
 * from its reset state.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image B.
 *              * #LDR_IMAGE_NOT_VALID - The CRC of image B is incorrect.
 */
int LDR_RunImageB(void);

/**
 * @brief   Commit image A so that it will automatically run after a reset.
 *
 * The image will be checked for completeness and a correct CRC before the
 * commit is completed.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image A.
 *              * #LDR_IMAGE_NOT_VALID - The CRC of image A is incorrect.
 *              * #LDR_FLASH_ERROR - An error occurred while writing the flash.
 */
int LDR_CommitImageA();

/**
 * @brief   Commit image B so that it will automatically run after a reset.
 *
 * The image will be checked for completeness and a correct CRC before the
 * commit is completed.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 *              * #LDR_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image B.
 *              * #LDR_IMAGE_NOT_VALID - The CRC of image B is incorrect.
 *              * #LDR_FLASH_ERROR - An error occurred while writing the flash.
 */
int LDR_CommitImageB();

/**
 * @brief   Report the state of each image, which image is currently running,
 *          and which image is currently committed.
 *
 * @param   status      Pointer to a structure holding the status information.
 * 
 * @return  The result of the operation.  Possible return values:
 *              * #LDR_SUCCESS
 */
int LDR_GetImageStatus(ldr_image_status_t* status);

/**
 * @brief   Retreives the version number of the loader.
 *
 * * Byte 3: Reserved (will read 0)
 * * Byte 2: Reserved (will read 0)
 * * Byte 1: Major version number
 * * Byte 0: Minor version number
 *
 * @return  The version number.
 */
uint32_t LDR_GetVersion(void);

/**
 * @brief   Retreives the build date of the loader.
 *
 * * Byte 3: Reserved (will read 0)
 * * Byte 2: The year of the release (19 represents the year 2019)
 * * Byte 1: The month of the release (1 = Jan, 2 = Feb, etc.)
 * * Byte 0: The day of the release
 *
 * @return  The date.
 */
uint32_t LDR_GetBuildDate(void);

/** @} end of LDR_API group */
