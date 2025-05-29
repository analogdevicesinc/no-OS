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

#include <mxc_device.h>
#include <flc_regs.h>
#include <gcr_regs.h>
#include <wdt_regs.h>
#include <pwrseq_regs.h>
#include "loader.h"

///////////////////////////////////////////////////////////////////////////////
// Local Definitions
///////////////////////////////////////////////////////////////////////////////

#define REV_MAJOR               0
#define REV_MINOR               1

// Flash page size in bytes
#define PAGE_SIZE               0x4000      // 16K Bytes
#define LOADER_FUNCTION_TABLE   (PAGE_SIZE - 128) // enough for 16 wide instruction entries

// Number of pages used by each region
#define LOADER_PAGES            1
#define COMMIT_PAGES            1
#define IMAGE_A_PAGES           8   // 128KB / 16KB = 8 pages
#define IMAGE_B_PAGES           8   // 128KB / 16KB = 8 pages
#define NVS_PAGES               4

// Length of each region in bytes
#define LOADER_LENGTH           (LOADER_PAGES * PAGE_SIZE)
#define COMMIT_LENGTH           (COMMIT_PAGES * PAGE_SIZE)
#define IMAGE_A_LENGTH          (IMAGE_A_PAGES * PAGE_SIZE)
#define IMAGE_B_LENGTH          (IMAGE_B_PAGES * PAGE_SIZE)
#define NVS_LENGTH              (NVS_PAGES * PAGE_SIZE)

// Starting address of each region in bytes
#define LOADER_START            (0x10000000)
#define COMMIT_START            (LOADER_START + LOADER_LENGTH)
#define IMAGE_A_START           (COMMIT_START + COMMIT_LENGTH)
#define IMAGE_B_START           (IMAGE_A_START + IMAGE_A_LENGTH)
#define NVS_START               (IMAGE_B_START + IMAGE_B_LENGTH)
 
// Simple loader state tracking
typedef struct {
    uint8_t running_image;      // 0=A, 1=B
    uint8_t committed_image;    // 0=A, 1=B
    uint8_t download_active;    // 0=idle, 1=active
} ldr_state_t;

// Defines used to fill in the status bytes of the loader state
#define RUN_A       0x00
#define RUN_B       0x80
#define COMMIT_A    0x00
#define COMMIT_B    0x40
#define NO_A        0x00
#define VALID_A     0x10
#define INVALID_A   0x20
#define NO_B        0x00
#define VALID_B     0x01
#define INVALID_B   0x02

// Defines header that is found at the beginning of each image
typedef struct
{
    uint32_t reserved[2];
    uint32_t crc;
    uint32_t len;
} image_footer_t;




///////////////////////////////////////////////////////////////////////////////
// Local Variables
///////////////////////////////////////////////////////////////////////////////

// Variable holding the current state.
static ldr_state_t __attribute__((section (".loaderVariables"))) loaderState;




///////////////////////////////////////////////////////////////////////////////
// Local Function Prototypes
///////////////////////////////////////////////////////////////////////////////

// Write the commit record for image A.  Returns 0 on failure.
static int commitImageA(void);

// Write the commit record for image B.  Returns 0 on failure.
static int commitImageB(void);

// Erases and invalidates image A.  Returns 0 on failure.
static int eraseImageA(void);

// Erases and invalidates image B.  Returns 0 on failure.
static int eraseImageB(void);

// Write given data to image A.  Returns 0 on failure.
static int writeImageA(uint32_t offset, uint8_t* buff, uint32_t length);

// Write given data to image B.  Returns 0 on failure.
static int writeImageB(uint32_t offset, uint8_t* buff, uint32_t length);

// Checks the CRC of image A.  Returns 0 on failure.
static int validateImageA(void);

// Checks the CRC of image B.  Returns 0 on failure.
static int validateImageB(void);

// Resets and runs image A.
static void startImageA(void);

// Resets and runs image B.
static void startImageB(void);

// Checks if image A is empty/erased.  Returns 1 on empty.
static int imageAEmpty(void);

// Checks if image B is empty/erased.  Returns 1 on empty.
static int imageBEmpty(void);

// Erase the range given.
static int erasePages(uint32_t addr, uint32_t len);

// Writes the given 32-bit word to flash.
static int writeFlash(uint32_t addr, uint32_t val[4]);

// Writes the given 32-bit word to flash.  This is a helper function to
//   write128 and writeBytes.  It does not prepare the flash or lock it back
//   upon completion.
static int write128(uint32_t addr, uint32_t val[4]);

// Writes the given array to flash.
static int writeBytes(uint32_t addr, uint8_t* buff, uint32_t len);

// Returns the next available address in the commit list.
static uint32_t getNextRecord(void);

// Returns the CRC of the given memory range.
static uint32_t doCRC(uint8_t* buff, uint32_t len);

// Prepares flash for an operation.
static int startFlashOp(void);

// Prepares flash for an operation.
static int endFlashOp(void);

// Helper functions for simplified state management
static int isImageAValid(void);
static int isImageBValid(void);
static int getCommittedImage(void);
static void setCommittedImage(int image);




///////////////////////////////////////////////////////////////////////////////
// API Function Implementations
///////////////////////////////////////////////////////////////////////////////

int LDR_StartDownload(void) 
{
    // Check if download already in progress
    if(loaderState.download_active) {
        return LDR_DOWNLOAD_IN_PROGRESS;
    }
    
    // Determine which image to update (opposite of running image)
    int target_image = 1 - loaderState.running_image;
    
    // Cannot update the currently running image
    if(target_image == loaderState.running_image) {
        return LDR_IMAGE_RUNNING;
    }
    
    // Erase the target image
    int result;
    if(target_image == 0) {
        result = eraseImageA();
    } else {
        result = eraseImageB();
    }
    
    if(!result) {
        return LDR_FLASH_ERROR;
    }
    
    // Mark download as active
    loaderState.download_active = 1;
    return LDR_SUCCESS;
}

int LDR_DownloadImageBlock(uint32_t offset, uint8_t* buff, uint32_t length) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return LDR_DOWNLOAD_NOT_STARTED;
    }
    
    // Write to the opposite of the running image
    int target_image = 1 - loaderState.running_image;
    int result;
    
    if(target_image == 0) {
        result = writeImageA(offset, buff, length);
    } else {
        result = writeImageB(offset, buff, length);
    }
    
    return result ? LDR_SUCCESS : LDR_FLASH_ERROR;
}

int LDR_AbortUpgrade(void) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return LDR_DOWNLOAD_NOT_STARTED;
    }
    
    // Mark download as inactive
    loaderState.download_active = 0;
    return LDR_SUCCESS;
}

int LDR_CompleteDownload(void) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return LDR_DOWNLOAD_NOT_STARTED;
    }
    
    // Validate the target image (opposite of running)
    int target_image = 1 - loaderState.running_image;
    int valid;
    
    if(target_image == 0) {
        valid = isImageAValid();
    } else {
        valid = isImageBValid();
    }
    
    // Mark download as inactive
    loaderState.download_active = 0;
    
    if(valid) {
        return LDR_SUCCESS;
    } else {
        return LDR_IMAGE_NOT_VALID;
    }
}

int LDR_RunImageA(void) 
{
    // Check if image A is valid
    if(!isImageAValid()) {
        return LDR_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress on A
    if(loaderState.download_active && loaderState.running_image == 1) {
        return LDR_DOWNLOAD_IN_PROGRESS;
    }
    
    // Update state and start image A
    loaderState.running_image = 0;
    loaderState.download_active = 0;
    startImageA();
    
    // Should never reach here
    return LDR_UNKNOWN_ERROR;
}

int LDR_RunImageB(void) 
{
    // Check if image B is valid
    if(!isImageBValid()) {
        return LDR_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress on B
    if(loaderState.download_active && loaderState.running_image == 0) {
        return LDR_DOWNLOAD_IN_PROGRESS;
    }
    
    // Update state and start image B
    loaderState.running_image = 1;
    loaderState.download_active = 0;
    startImageB();
    
    // Should never reach here
    return LDR_UNKNOWN_ERROR;
}

int LDR_CommitImageA() 
{
    // Check if image A is valid
    if(!isImageAValid()) {
        return LDR_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress
    if(loaderState.download_active) {
        return LDR_DOWNLOAD_IN_PROGRESS;
    }
    
    // Only allow committing A if A is running
    if(loaderState.running_image != 0) {
        return LDR_IMAGE_RUNNING;
    }
    
    // Check if A is already committed
    if(loaderState.committed_image == 0) {
        return LDR_SUCCESS;
    }
    
    // Commit image A
    if(!commitImageA()) {
        return LDR_FLASH_ERROR;
    }
    
    loaderState.committed_image = 0;
    return LDR_SUCCESS;
}

int LDR_CommitImageB() 
{
    // Check if image B is valid
    if(!isImageBValid()) {
        return LDR_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress
    if(loaderState.download_active) {
        return LDR_DOWNLOAD_IN_PROGRESS;
    }
    
    // Only allow committing B if B is running
    if(loaderState.running_image != 1) {
        return LDR_IMAGE_RUNNING;
    }
    
    // Check if B is already committed
    if(loaderState.committed_image == 1) {
        return LDR_SUCCESS;
    }
    
    // Commit image B
    if(!commitImageB()) {
        return LDR_FLASH_ERROR;
    }
    
    loaderState.committed_image = 1;
    return LDR_SUCCESS;
}

int LDR_GetImageStatus(ldr_image_status_t* status) 
{
    // Build active/commit status
    status->active_commit_status = 0;
    if(loaderState.running_image == 0) {
        status->active_commit_status |= RUN_A;
    } else {
        status->active_commit_status |= RUN_B;
    }
    
    if(loaderState.committed_image == 0) {
        status->active_commit_status |= COMMIT_A;
    } else {
        status->active_commit_status |= COMMIT_B;
    }
    
    // Build image validity status
    status->image_status = 0;
    if(isImageAValid()) {
        status->image_status |= VALID_A;
    } else if(!imageAEmpty()) {
        status->image_status |= INVALID_A;
    }
    
    if(isImageBValid()) {
        status->image_status |= VALID_B;
    } else if(!imageBEmpty()) {
        status->image_status |= INVALID_B;
    }
    
    return LDR_SUCCESS;
}

uint32_t LDR_GetVersion()
{
    return (REV_MAJOR << 8) | REV_MINOR;
}

uint32_t LDR_GetBuildDate()
{
    uint32_t ret = 0;

    char* date = __DATE__;

    // Fill in the 'year' byte (converting string to binary value).
    ret = (((date[9] - '0') * 10) + (date[10] - '0')) << 16;

    // Fill in the 'month' byte
    switch (date[0])
    {
        // Jan, Jun, or Jul
        case 'J':
        {
            if(date[1] == 'a')
            {
                ret |= 0x100;
            }
            else if(date[2] == 'n')
            {
                ret |= 0x600;
            }
            else
            {
                ret |= 0x700;
            }
            break;
        }

        // Feb
        case 'F':
        {
            ret |= 0x200;
            break;
        }

        // Mar or May
        case 'M':
        {
            if(date[2] == 'r')
            {
                ret |= 0x300;
            }
            else
            {
                ret |= 0x500;
            }
            break;
        }

        // Apr or Aug
        case 'A':
        {
            if(date[1] == 'p')
            {
                ret |= 0x400;
            }
            else
            {
                ret |= 0x800;
            }
            break;
        }

        // Sep
        case 'S':
        {
            ret |= 0x900;
            break;
        }

        // Oct
        case 'O':
        {
            ret |= 0xA00;
            break;
        }

        // Nov
        case 'N':
        {
            ret |= 0xB00;
            break;
        }

        // Dec
        case 'D':
        {
            ret |= 0xC00;
            break;
        }

        default:
        case '?':
        {
            // Leave the value at 0
            break;
        }

    }

    // Fill in the 'day' byte (converting string to binary value).
    if(date[4] == ' ')
    {
        ret |= (date[5] - '0');
    }
    else
    {
        ret |= (((date[4] - '0') * 10) + (date[5] - '0'));
    }

    return ret;
}





///////////////////////////////////////////////////////////////////////////////
// Local Function Implementations
///////////////////////////////////////////////////////////////////////////////

static int commitImageA()
{
    uint32_t recordLoc;
    
    // Get location next commit record should be written.
    recordLoc = getNextRecord();
    
    // Image A commit always happens on an even address.
    // Code should never pass this check.  
    if((recordLoc & 0x10) == 0)
    {
        // Trying to commit A when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return writeFlash(recordLoc, (uint32_t[]){0, 0, 0, 0});
}

static int commitImageB()
{
    uint32_t recordLoc;
    
    // Get location next commit record should be written.
    recordLoc = getNextRecord();
    
    // Image B commit always happens on an odd address.
    // Code should never pass this check.  

    if((recordLoc & 0x10) != 0)
    {
        // Trying to commit B when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return writeFlash(recordLoc, (uint32_t[]){0, 0, 0, 0});
}

static int eraseImageA()
{
    return erasePages(IMAGE_A_START, IMAGE_A_LENGTH);
}

static int eraseImageB()
{
    return erasePages(IMAGE_B_START, IMAGE_B_LENGTH);
}

static int writeImageA(uint32_t offset, uint8_t* buff, uint32_t length)
{
    // Check if image A is currently running
    if(loaderState.running_image == 0)
    {
        return 0;  // Cannot write to running image
    }

    // Check if offset is in the valid range for Image A
    if(offset < IMAGE_A_START || offset >= (IMAGE_A_START + IMAGE_A_LENGTH))
    {
        return 0;
    }

    // Convert absolute address to relative offset
    uint32_t relative_offset = offset - IMAGE_A_START;

    // Is the length in range?
    if(length > IMAGE_A_LENGTH)
    {
        return 0;
    }
    
    // Are the two together in range?
    if((relative_offset + length) > IMAGE_A_LENGTH)
    {
        return 0;
    }
    
    // Write the bytes to image A using the absolute address
    return writeBytes(offset, buff, length);
}

static int writeImageB(uint32_t offset, uint8_t* buff, uint32_t length)
{
    // Check if image B is currently running
    if(loaderState.running_image == 1)
    {
        return 0;  // Cannot write to running image
    }

    // Check if offset is in the valid range for Image B
    if(offset < IMAGE_B_START || offset >= (IMAGE_B_START + IMAGE_B_LENGTH))
    {
        return 0;
    }

    // Convert absolute address to relative offset
    uint32_t relative_offset = offset - IMAGE_B_START;

    // Is the length in range?
    if(length > IMAGE_B_LENGTH)
    {
        return 0;
    }
    
    // Are the two together in range?
    if((relative_offset + length) > IMAGE_B_LENGTH)
    {
        return 0;
    }
    
    // Write the bytes to image B using the absolute address
    return writeBytes(offset, buff, length);
}

static int validateImageA()
{
    // Get a pointer to flash
    image_footer_t* ftr = (image_footer_t*)(IMAGE_A_START + IMAGE_A_LENGTH - sizeof(image_footer_t));
    
    // See if len is valid.  If not, image cannot be valid.
    if(ftr->len > (IMAGE_A_LENGTH - sizeof(image_footer_t)))
    {
        return 0;
    }
    
    // Compute the CRC of the image
    uint32_t crc = doCRC((uint8_t*)IMAGE_A_START, ftr->len);
    return (crc == ftr->crc);
}

static int validateImageB()
{
    // Get a pointer to flash
    image_footer_t* ftr = (image_footer_t*)(IMAGE_B_START + IMAGE_B_LENGTH - sizeof(image_footer_t));
    
    // See if len is valid.  If not, image cannot be valid.
    if(ftr->len > (IMAGE_B_LENGTH - sizeof(image_footer_t)))
    {
        return 0;
    }
    
    // Compute the CRC of the image
    uint32_t crc = doCRC((uint8_t*)IMAGE_B_START, ftr->len);
    return (crc == ftr->crc);
}

static void startImageA()
{
    // Reset all peripherals and GPIO
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    // TODO - set vtor?
    __set_MSP(IMAGE_A_START);
    __asm volatile ("ldr lr, [sp, #4]");
    __asm volatile ("ldr sp, [sp, #0]");
    __asm volatile ("bx lr");
}

static void startImageB()
{
    // Reset all peripherals and GPIO
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    // TODO - set vtor?
    __set_MSP(IMAGE_B_START);
    __asm volatile ("ldr lr, [sp, #4]");
    __asm volatile ("ldr sp, [sp, #0]");
    __asm volatile ("bx lr");
}

static int imageAEmpty()
{
    int i;
    
    // Get a pointer to the flash memory.
    uint32_t* mem = (uint32_t*)IMAGE_A_START;
    
    // Look at the flash 4 bytes at a time.
    for(i = 0; i < (IMAGE_A_LENGTH >> 2); i++)
    {
        // If any word is found to be not-erased, return not empty.
        if(mem[i] != 0xFFFFFFFFUL)
        {
            return 0;
        }
    }
    return 1;
}

static int imageBEmpty()
{
    int i;
    
    // Get a pointer to the flash memory.
    uint32_t* mem = (uint32_t*)IMAGE_B_START;
    
    // Look at the flash 4 bytes at a time.
    for(i = 0; i < (IMAGE_B_LENGTH >> 2); i++)
    {
        // If any word is found to be not-erased, return not empty.
        if(mem[i] != 0xFFFFFFFFUL)
        {
            return 0;
        }
    }
    return 1;
}

static uint32_t getNextRecord()
{
    int i;
    
    // Get a pointer to the flash memory.
    uint32_t* mem = (uint32_t*)COMMIT_START;
    
    // Look at the flash 4 bytes at a time.
    for(i = 0; i < COMMIT_LENGTH / 4; i+=4)
    {
        // If any word is found to be not-erased, return previous address.
        if((mem[i] == 0) && (mem[i+1] == 0) && (mem[i+2] == 0) && (mem[i+3] == 0))
        {
            // See if commit region is full.
            if(i == 0)
            {
                // TODO - error check - what to do on error though?
                erasePages(COMMIT_START, COMMIT_LENGTH);
                break;
            }
            else
            {
                // Return byte address of previous empty 32-bit word.
                return (i - 4) * 4 + COMMIT_START;
            }
        }
    }
    
    // Commit area is empty, return byte address of the last 32-bit word.
    return (COMMIT_START + COMMIT_LENGTH - 16);
}

static int write128(uint32_t addr, uint32_t val[4])
{
    // write the data
    MXC_FLC->addr = addr;
    MXC_FLC->data[0] = val[0];
    MXC_FLC->data[1] = val[1];
    MXC_FLC->data[2] = val[2];
    MXC_FLC->data[3] = val[3];
    MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WR;

    // TODO - timeout
    // Wait until flash operation is complete.
    while (MXC_FLC->ctrl & MXC_F_FLC_CTRL_WR);
    
    return 1;
}

static int writeFlash(uint32_t addr, uint32_t val[4])
{
    // Make sure address is on a 128-bit boundary.
    if(addr & 0xf) 
    {
        return 0;
    }

    // Prepare flash controller for writes.
    if(!startFlashOp())
    {
        return 0;
    }

    // Do the write.
    if(!write128(addr, val))
    {
        return 0;
    }

    // Lock the flash and flush the caches.
    if(!endFlashOp())
    {
        return 0;
    }

    return 1;
}

static int writeBytes(uint32_t addr, uint8_t* buff, uint32_t len)
{
    uint32_t i;
    uint32_t* mem = (uint32_t*)buff;
    
    // Make sure address is on a 128-bit boundary.
    if(addr & 0xf) 
    {
        return 0;
    }

    // Make sure length is multiple of 16.  Can only write 128 bits at a time.
    if(len & 0xf) 
    {
        return 0;
    }
    
    // Prepare flash controller for writes.
    if(!startFlashOp())
    {
        return 0;
    }

    // Process 16 bytes (4 words) at a time
    for(i = 0; i < len; i += 16)
    {
        uint32_t word_idx = i / 4;  // Convert byte index to word index
        if(!write128(addr, &mem[word_idx]))
        {
            return 0;
        }
        addr += 16;
    }

    // Lock the flash and flush the caches.
    if(!endFlashOp())
    {
        return 0;
    }

    return 1;
}

static int erasePages(uint32_t addr, uint32_t len)
{
    // Prepare flash controller for the erase.
    if(!startFlashOp())
    {
        return 0;
    }
    
    // Align start address on a page boundary.
    addr &= ~(PAGE_SIZE - 1);
    while(len)
    {
        // Write page erase code
        MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_ERASE_CODE) | MXC_S_FLC_CTRL_ERASE_CODE_ERASEPAGE;

        // Issue page erase command
        MXC_FLC->addr = addr;
        MXC_FLC->ctrl |= MXC_F_FLC_CTRL_PGE;

        // TODO - timeout?
        // Wait until flash operation is complete.
        while(MXC_FLC->ctrl & MXC_F_FLC_CTRL_PGE);

        // Go to next page.
        addr += PAGE_SIZE;
        len -= PAGE_SIZE;
    }
    
    // Lock the flash and flush the caches.
    if(!endFlashOp())
    {
        return 0;
    }
    
    // All necessary data erased.
    return 1;
}

// CRC-32
// x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x^1 + 1
#define poly 0xEDB88320UL
static uint32_t doCRC(uint8_t* data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFUL;
    unsigned int i;
    int j;
    for(i = 0; i < len; i++)
    {
        crc = crc ^ data[i];
        for (j=0; j<8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
        }
    }
    return(crc ^ 0xFFFFFFFFUL);
}

static int startFlashOp()
{
    // The flash controller requires a 1MHz internal clock for low-power wakeup, write, and erase operations.
    MXC_FLC->clkdiv = 120;

    // TODO - timeout??
    // Check if the flash controller is busy.
    while(MXC_FLC->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));

    // Unlock flash.
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_UNLOCK) | MXC_S_FLC_CTRL_UNLOCK_UNLOCKED;

    return 1;
}

static int endFlashOp()
{
    // Lock flash.
    MXC_FLC->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;

    // Flush the instruction cache.
    MXC_GCR->sysctrl |= MXC_F_GCR_SYSCTRL_ICC0_FLUSH;
    
    // TODO - timeout??
    // Wait for flush to complete.
    while(MXC_GCR->sysctrl & MXC_F_GCR_SYSCTRL_ICC0_FLUSH);
    
    return 1;
}

// Helper function implementations
static int isImageAValid(void)
{
    return validateImageA() && !imageAEmpty();
}

static int isImageBValid(void)
{
    return validateImageB() && !imageBEmpty();
}

static int getCommittedImage(void)
{
    uint32_t commitAddr = getNextRecord();
    // If commit area is empty, default to A
    if(commitAddr == (COMMIT_START + COMMIT_LENGTH - 16)) {
        return 0; // A
    }
    // Check if last commit was for A (even address) or B (odd address)
    return (commitAddr & 16) ? 1 : 0; // A=0, B=1
}

static void setCommittedImage(int image)
{
    loaderState.committed_image = image;
}




///////////////////////////////////////////////////////////////////////////////
// CMSIS Startup Function Overrides
///////////////////////////////////////////////////////////////////////////////

void SystemInit(void)
{
    uint32_t div;

    MXC_WDT0->ctrl &= ~MXC_F_WDT_CTRL_EN;  /* Turn off watchdog. Application can re-enable as needed. */

    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

    // Set flash wait states higher than what the minimum for the fastest clock is
    MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x5UL << MXC_F_GCR_MEMCTRL_FWS_POS);

    // Enable 120 MHz IPO Clock
    MXC_GCR->clkctrl |=MXC_F_GCR_CLKCTRL_IPO_EN;

    // TODO - timeout???
    // Wait for clock to be ready
    while((MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_IPO_RDY) == 0);
    
    // Set 120MHz IPO clock as System Clock
    MXC_SETFIELD(MXC_GCR->clkctrl, MXC_F_GCR_CLKCTRL_SYSCLK_SEL, MXC_S_GCR_CLKCTRL_SYSCLK_SEL_IPO);

    // TODO - timeout???
    // Wait for system clock to be ready
    while((MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_SYSCLK_RDY) == 0);

    // Get the clock divider
    div = (MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_SYSCLK_DIV) >> MXC_F_GCR_CLKCTRL_SYSCLK_DIV_POS;

    if(div == 0)
    {
        MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x4UL << MXC_F_GCR_MEMCTRL_FWS_POS);
    }
    else if(div == 1)
    {
        MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x2UL << MXC_F_GCR_MEMCTRL_FWS_POS);
    } 
    else
    {
        MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x1UL << MXC_F_GCR_MEMCTRL_FWS_POS);
    }

    /* Disable clocks to peripherals by default to reduce power */
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_DMA;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_SPI0;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_SPI1;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_UART0;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_UART1;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_I2C0;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_TMR0;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_TMR1;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_TMR2;
    MXC_GCR->pclkdis0 |= MXC_F_GCR_PCLKDIS0_I2C1;
}




///////////////////////////////////////////////////////////////////////////////
// Loader Entry Point
///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // Initialize state structure 
    loaderState.download_active = 0;
    
    // Determine which image is committed from flash
    loaderState.committed_image = getCommittedImage();
    
    // Check if commit area is empty (very first boot)
    uint32_t commitAddr = getNextRecord();
    if(commitAddr == (COMMIT_START + COMMIT_LENGTH - 16))
    {
        // First boot - validate and commit image A as default
        if(isImageAValid())
        {
            writeFlash(commitAddr, (uint32_t[]){0, 0, 0, 0});
            loaderState.committed_image = 0; // A
            loaderState.running_image = 0;   // A
            startImageA();
        }
        else if(isImageBValid())
        {
            // A invalid, try B
            writeFlash(commitAddr - 16, (uint32_t[]){0, 0, 0, 0});
            loaderState.committed_image = 1; // B
            loaderState.running_image = 1;   // B
            startImageB();
        }
        else
        {
            // Both invalid - enter recovery mode
            while(1); // TODO: Implement recovery mechanism
        }
    }
    
    // Normal boot - try committed image first
    if(loaderState.committed_image == 0)
    {
        // A is committed
        if(isImageAValid())
        {
            loaderState.running_image = 0;
            startImageA();
        }
        else if(isImageBValid())
        {
            // Fallback to B
            loaderState.running_image = 1;
            startImageB();
        }
        else
        {
            // Both invalid - recovery mode
            while(1); // TODO: Implement recovery mechanism
        }
    }
    else
    {
        // B is committed
        if(isImageBValid())
        {
            loaderState.running_image = 1;
            startImageB();
        }
        else if(isImageAValid())
        {
            // Fallback to A
            loaderState.running_image = 0;
            startImageA();
        }
        else
        {
            // Both invalid - recovery mode
            while(1); // TODO: Implement recovery mechanism
        }
    }
}