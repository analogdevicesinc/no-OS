#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

/** No error, the operation completed successfully. */
#define DL_SUCCESS                  0x50

/** The requested operation could not complete because the CRC of the selected
 *  image is incorrect.
 */
#define DL_IMAGE_NOT_VALID          0xD0

/** The requested operation could not complete because the target imagae is
 *  already running.
 */
#define DL_IMAGE_RUNNING            0xD1

/** The requested operation could not complete because a download session is
 *  currently active.
 */
#define DL_DOWNLOAD_IN_PROGRESS     0xD2

/** The requested operation could not complete because an active download
 *  session has not been started.
 */
#define DL_DOWNLOAD_NOT_STARTED     0xD3

/** The requested operation could not complete because an error occured while
 *  attempting to modify flash contents.
 */
#define DL_FLASH_ERROR              0xD4

/** The requested operation could not complete because of an unexpected error.
 */
#define DL_UNKNOWN_ERROR            0xFF

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
} dl_image_status_t;

/**
 * @brief   Prepare the module to receive firmware upgrade data.
 *
 * Erases/invalidates the currently non-commited image and prepares that
 * image's area for receiving a new image.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_IMAGE_RUNNING - The non-committed image is currently the
 *                                     active running image.
 *              * #DL_DOWNLOAD_IN_PROGRESS - There is already an active
 *                                            download session.
 *              * #DL_FLASH_ERROR - An error occurred while erasing the flash.
 */
int dl_start_download(void);

/**
 * @brief   Write a data block to the module for firmware upgrade.
 *
 * @param   offset      The offset into the image area to write the data.
 * @param   buff        A pointer to the data to write.
 * @param   length      The length (in bytes) of the data to write.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 *              * #DL_FLASH_ERROR - An error occurred while writing the flash.
 */
int dl_download_image_block(uint32_t offset, uint8_t* buff, uint32_t length);

/**
 * @brief   Abort firmware upgrade process.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 */
int dl_abort_upgrade(void);

/**
 * @brief   Complete the download process of an image.
 *
 * Verifies the complete image was written and its CRC is correct.  If both
 * conditions are satisfied the image will be marked as valid.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_DOWNLOAD_NOT_STARTED - A download session has not been
 *                                            started.
 *              * #DL_IMAGE_NOT_VALID - The CRC of the received image is
 *                                       incorrect.
 */
int dl_complete_download(void);

/**
 * @brief   Execute image A.
 *
 * If image A is valid, this function will not return.  Instead execution will
 * be transferred to image A as if image A was executed from the reset state.
 * If image A is already the active image, this function will restart the image
 * from its reset state.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image A.
 *              * #DL_IMAGE_NOT_VALID - The CRC of image A is incorrect.
 */
int dl_run_image_a(void);

/**
 * @brief   Execute image B.
 *
 * If image B is valid, this function will not return.  Instead execution will
 * be transferred to image B as if image B was executed from the reset state.
 * If image B is already the active image, this function will restart the image
 * from its reset state.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image B.
 *              * #DL_IMAGE_NOT_VALID - The CRC of image B is incorrect.
 */
int dl_run_image_b(void);

/**
 * @brief   Commit image A so that it will automatically run after a reset.
 *
 * The image will be checked for completeness and a correct CRC before the
 * commit is completed.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image A.
 *              * #DL_IMAGE_NOT_VALID - The CRC of image A is incorrect.
 *              * #DL_FLASH_ERROR - An error occurred while writing the flash.
 */
int dl_commit_image_a(void);

/**
 * @brief   Commit image B so that it will automatically run after a reset.
 *
 * The image will be checked for completeness and a correct CRC before the
 * commit is completed.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_DOWNLOAD_IN_PROGRESS - A download session is currently in
 *                                            progress for image B.
 *              * #DL_IMAGE_NOT_VALID - The CRC of image B is incorrect.
 *              * #DL_FLASH_ERROR - An error occurred while writing the flash.
 */
int dl_commit_image_b(void);

/**
 * @brief   Report the state of each image, which image is currently running,
 *          and which image is currently committed.
 *
 * @param   status      Pointer to a structure holding the status information.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 */
int dl_get_image_status(dl_image_status_t* status);

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
uint32_t dl_get_version(void);

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
uint32_t dl_get_build_date(void);


/**
 * @brief   Write flash memory
 *
 * @param   addr       The address to write to.
 * @param   data       Pointer to the data to write.
 * @param   len        The length of the data to write.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_FLASH_ERROR - An error occurred while writing the flash.
 */
int dl_flash_write(uint32_t addr, void *data, uint32_t len);

/**
 * @brief   Read flash memory
 *
 * @param   addr       The address to read from.
 * @param   data       Pointer to the buffer to store the read data.
 * @param   len        The length of the data to read.
 *
 * @return  The result of the operation.  Possible return values:
 *              * #DL_SUCCESS
 *              * #DL_FLASH_ERROR - An error occurred while reading the flash.
 */
int dl_flash_read(uint32_t addr, void *data, uint32_t len);


/** @} end of DL_API group */

#endif /* LOADER_H */
