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

#define CORE_CLOCK_MHZ          96

#define LOADER_STATE_ADDR       0x20017FFC  // Last 4 bytes of SRAM

// Flash page size in bytes
#define PAGE_SIZE               0x2000      // 8K Bytes

// Number of pages used by each region
#define LOADER_PAGES            1
#define COMMIT_PAGES            1
#define IMAGE_A_PAGES           13
#define IMAGE_B_PAGES           13
#define NVS_PAGES               4

// Length of each region in bytes
#define LOADER_LENGTH           (LOADER_PAGES * PAGE_SIZE)
#define COMMIT_LENGTH           (COMMIT_PAGES * PAGE_SIZE)
#define IMAGE_A_LENGTH          (IMAGE_A_PAGES * PAGE_SIZE)
#define IMAGE_B_LENGTH          (IMAGE_B_PAGES * PAGE_SIZE)
#define NVS_LENGTH              (NVS_PAGES * PAGE_SIZE)

// Starting address of each region in bytes
#define LOADER_START            (0)
#define COMMIT_START            (LOADER_START + LOADER_LENGTH)
#define IMAGE_A_START           (COMMIT_START + COMMIT_LENGTH)
#define IMAGE_B_START           (IMAGE_A_START + IMAGE_A_LENGTH)
#define NVS_START               (IMAGE_B_START + IMAGE_B_LENGTH)
 
// Possible combinataion of states of the current images.
//      RUN_X         - which image is currently running.
//      COMMIT_X      - which image is currently committed.
//      VALID_XY      - which images are detected to be valid.
//      IDLE/ACTIVE   - whether a download is in progress.
typedef enum 
{
    STATE0_RUN_A_COMMIT_A_VALID_A_IDLE,
    STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE,
    STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE,
    STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE,
    STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE,
    STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE,
    STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE,
    STATE7_RUN_B_COMMIT_B_VALID_B_IDLE
} e_ldr_state;

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
    uint32_t crc;
    uint32_t len;
} image_footer_t;




///////////////////////////////////////////////////////////////////////////////
// Local Variables
///////////////////////////////////////////////////////////////////////////////

// Variable holding the current state.
static e_ldr_state __attribute__((section (".loaderVariables"))) loaderState;




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
static int writeFlash(uint32_t addr, uint32_t val);

// Writes the given 32-bit word to flash.  This is a helper function to
//   writeWord and writeBytes.  It does not prepare the flash or lock it back
//   upon completion.
static int writeWord(uint32_t addr, uint32_t val);

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




///////////////////////////////////////////////////////////////////////////////
// API Function Implementations
///////////////////////////////////////////////////////////////////////////////

int LDR_StartDownload(void) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        {
            // Write new commit record
            if(!eraseImageB()) 
            {
                return LDR_FLASH_ERROR;
            }

            // Image B is now invalid, download session active.
            loaderState = STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE;
            return LDR_SUCCESS;
        }
        
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Image B is currently being updated.
            return LDR_DOWNLOAD_IN_PROGRESS;
        }

        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        {
            // Cannot load to the image because it is currrently running.
            return LDR_IMAGE_RUNNING;
        }
        
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // Write new commit record
            if(!eraseImageA()) 
            {
                return LDR_FLASH_ERROR;
            }

            // Image A becomes invalid, loader becomes active
            loaderState = STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE;
            return LDR_SUCCESS;
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_DownloadImageBlock(uint32_t offset, uint8_t* buff, uint32_t length) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // A downloading session has not been started.
            return LDR_DOWNLOAD_NOT_STARTED;
        }
        
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Image A is running and commited.  Write to image B.
            if(!writeImageB(offset, buff, length)) 
            {
                return LDR_FLASH_ERROR;
            }
            return LDR_SUCCESS;
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Image B is running and commited.  Write to image A.
            if(!writeImageA(offset, buff, length)) 
            {
                return LDR_FLASH_ERROR;
            }
            return LDR_SUCCESS;
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_AbortUpgrade(void) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // A downloading session has not been started.
            return LDR_DOWNLOAD_NOT_STARTED;
        }
        
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Download session is now idle.
            loaderState = STATE0_RUN_A_COMMIT_A_VALID_A_IDLE;
            return LDR_SUCCESS;
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Download session is now idle.
            loaderState = STATE7_RUN_B_COMMIT_B_VALID_B_IDLE;
            return LDR_SUCCESS;
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_CompleteDownload(void) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // A downloading session has not been started.
            return LDR_DOWNLOAD_NOT_STARTED;
        }
        
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Check that image B is correct.
            if(validateImageB()) 
            {
                // B is now valid, download session idle.
                loaderState = STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE;
                return LDR_SUCCESS;
            }
            else
            {
                // Abort the download session.
                loaderState = STATE0_RUN_A_COMMIT_A_VALID_A_IDLE;
                return LDR_IMAGE_NOT_VALID;
            }
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Check that image A is correct.
            if(validateImageA()) 
            {
                // A is now valid, download session idle.
                loaderState = STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE;
                return LDR_SUCCESS;
            }
            else
            {
                // Abort the download session.
                loaderState = STATE7_RUN_B_COMMIT_B_VALID_B_IDLE;
                return LDR_IMAGE_NOT_VALID;
            }
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_RunImageA(void) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // A is now running.
            loaderState = STATE0_RUN_A_COMMIT_A_VALID_A_IDLE;
            // Restart A - function will not return.
            startImageA();
            break;
        }
        
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        {
            // A is now running.
            loaderState = STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE;
            // Restart A - function will not return.
            startImageA();
            break;
        }
        
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        {
            // A is now running.
            loaderState = STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE;
            // Restart A - function will not return.
            startImageA();
            break;
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Image A is currently being updated.
            return LDR_DOWNLOAD_IN_PROGRESS;
        }
        
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // Image A is not valid.
            return LDR_IMAGE_NOT_VALID;
        }
    }

    // Should never reach here.  If code does reach here, something has
    //   incorreclty modified the 'loaderState' variable.
    return LDR_UNKNOWN_ERROR;
}

int LDR_RunImageB(void) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        {
            // Image A is not valid.
            return LDR_IMAGE_NOT_VALID;
        }

        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Image A is currently being updated.
            return LDR_DOWNLOAD_IN_PROGRESS;
        }

        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        {
            // B is now running.
            loaderState = STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE;
            // Restart B - function will not return.
            startImageB();
            break;
        }
        
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        {
            // B is now running.
            loaderState = STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE;
            // Restart B - function will not return.
            startImageB();
            break;
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // B is now running.
            loaderState = STATE7_RUN_B_COMMIT_B_VALID_B_IDLE;
            // Restart B - function will not return.
            startImageB();
            break;
        }
    }

    // Should never reach here.  If code does reach here, something has
    //   incorreclty modified the 'loaderState' variable.
    return LDR_UNKNOWN_ERROR;
}

int LDR_CommitImageA() 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        {
            return LDR_SUCCESS;     // Image A is already committed.
        }
               
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        {
            // Write new commit record
            if(!commitImageA()) 
            {
                return LDR_FLASH_ERROR;
            }

            // A is now the committed image.
            loaderState = STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE;
            return LDR_SUCCESS;
        }
            
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        {
            // Write new commit record
            if(!commitImageA()) 
            {
                return LDR_FLASH_ERROR;
            }

            // A is now the committed image.
            loaderState = STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE;
            return LDR_SUCCESS;
        }
        
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        {
            // Image A is currently being updated.
            return LDR_DOWNLOAD_IN_PROGRESS;
        }
        
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // Image A is not currently valid.
            return LDR_IMAGE_NOT_VALID;
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_CommitImageB() 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        {
            // Image B is not currently valid.
            return LDR_IMAGE_NOT_VALID;
        }
        
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Image B is currently being updated.
            return LDR_DOWNLOAD_IN_PROGRESS;
        }
        
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        {
            // Write new commit record
            if(!commitImageB()) 
            {
                return LDR_FLASH_ERROR;
            }

            // B is now the committed image.
            loaderState = STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE;
            return LDR_SUCCESS;
        }

        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        {
            // Write new commit record
            if(!commitImageB()) 
            {
                return LDR_FLASH_ERROR;
            }

            // B is now the committed image.
            loaderState = STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE;
            return LDR_SUCCESS;
        }

        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            return LDR_SUCCESS;     // Image B is already committed.
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
}

int LDR_GetImageStatus(ldr_image_status_t* status) 
{
    switch(loaderState)
    {
        case STATE0_RUN_A_COMMIT_A_VALID_A_IDLE:
        case STATE1_RUN_A_COMMIT_A_VALID_A_ACTIVE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_A | COMMIT_A;
            status->image_status = VALID_A;
            // Discover and fill in other possible states.
            if(!imageBEmpty())
            {
                status->image_status |= INVALID_B;
            }
            return LDR_SUCCESS;
        }
        case STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_A | COMMIT_A;
            status->image_status = VALID_A | VALID_B;
            return LDR_SUCCESS;
        }
        case STATE3_RUN_B_COMMIT_A_VALID_AB_IDLE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_B | COMMIT_A;
            status->image_status = VALID_A | VALID_B;
            return LDR_SUCCESS;
        }
        case STATE4_RUN_A_COMMIT_B_VALID_AB_IDLE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_A | COMMIT_B;
            status->image_status = VALID_A | VALID_B;
            return LDR_SUCCESS;
        }
        case STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_B | COMMIT_B;
            status->image_status = VALID_A | VALID_B;
            return LDR_SUCCESS;
        }
        case STATE6_RUN_B_COMMIT_B_VALID_B_ACTIVE:
        case STATE7_RUN_B_COMMIT_B_VALID_B_IDLE:
        {
            // Fill in known status.
            status->active_commit_status = RUN_B | COMMIT_B;
            status->image_status = VALID_B;
            // Discover and fill in other possible states.
            if(!imageAEmpty())
            {
                status->image_status |= INVALID_A;
            }
            return LDR_SUCCESS;
        }
        
        default:
        {
            // Should never reach here.  If code does reach here, something has
            //   incorreclty modified the 'loaderState' variable.
            return LDR_UNKNOWN_ERROR;
        }
    }
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
    if((recordLoc & 4) == 0)
    {
        // Trying to commit A when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return writeFlash(recordLoc, 0);
}

static int commitImageB()
{
    uint32_t recordLoc;
    
    // Get location next commit record should be written.
    recordLoc = getNextRecord();
    
    // Image B commit always happens on an odd address.
    // Code should never pass this check.  

    if((recordLoc & 4) != 0)
    {
        // Trying to commit B when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return writeFlash(recordLoc, 0);
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
    // Is the offset in range?
    if(offset > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }

    // Is the length in range?
    if(length > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }
    
    // Are the two together in range?
    if((offset + length) > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }
    
    if(offset < IMAGE_A_LENGTH)
    {
        // Does this write go past the end of image A?
        if(offset + length > (IMAGE_A_LENGTH))
        {
            // Truncate the length so it only goes up to the end of A.
            length = IMAGE_A_LENGTH - offset;
        }
        // Write the bytes to image A.
        return writeBytes(IMAGE_A_START + offset, buff, length);
    }

    // Received data for image B.  Just ignore it and return success.
    return 1;
}

static int writeImageB(uint32_t offset, uint8_t* buff, uint32_t length)
{
    // Is the offset in range?
    if(offset > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }

    // Is the length in range?
    if(length > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }
    
    // Are the two together in range?
    if((offset + length) > (IMAGE_A_LENGTH + IMAGE_B_LENGTH))
    {
        return 0;
    }
    
    // Any image A data included here?
    if(offset < IMAGE_A_LENGTH)
    {
        // Does this write go past the end of image A?
        if(offset + length > (IMAGE_A_LENGTH))
        {
            // Move buffer past image A data.
            buff += (IMAGE_A_LENGTH - offset);
            // Adjust length to account for the amount of image A data.
            length = length - (IMAGE_A_LENGTH - offset);
            // Move offset to the beginning of image B data.
            offset = IMAGE_A_LENGTH;
        }
        else
        {
            // Received data for image A.  Just ignore it and return success.
            return 1;
        }
    }

    // Translate offset to be relative to start of B.
    offset -= IMAGE_A_LENGTH;

    // Write the bytes to image A.
    return writeBytes(IMAGE_B_START + offset, buff, length);
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
    for(i = 0; i < (COMMIT_LENGTH >> 2); i++)
    {
        // If any word is found to be not-erased, return previous address.
        if(mem[i] != 0xFFFFFFFFUL)
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
                return ((i - 1) << 2) + COMMIT_START;
            }
        }
    }
    
    // Commit area is empty, return byte address of the last 32-bit word.
    return (COMMIT_START + COMMIT_LENGTH - 4);
}

static int writeWord(uint32_t addr, uint32_t val)
{
    // write the data
    MXC_FLC->addr = addr;
    MXC_FLC->data[0] = val;
    MXC_FLC->ctrl |= MXC_F_FLC_CTRL_WRITE;

    // TODO - timeout
    // Wait until flash operation is complete.
    while (MXC_FLC->ctrl & MXC_F_FLC_CTRL_WRITE);
    
    return 1;
}

static int writeFlash(uint32_t addr, uint32_t val)
{
    // Make sure address is on a 32-bit boundary.
    if(addr & 0x3) 
    {
        return 0;
    }

    // Prepare flash controller for writes.
    if(!startFlashOp())
    {
        return 0;
    }

    // Do the write.
    if(!writeWord(addr, val))
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
    
    // Make sure address is on a 32-bit boundary.
    if(addr & 0x3) 
    {
        return 0;
    }

    // Make sure length is multiple of 4.  Can only write 32 bits at a time.
    if(len & 0x3) 
    {
        return 0;
    }
    
    // Convert to number of 32-bit words.
    len >>= 2;
    
    // Prepare flash controller for writes.
    if(!startFlashOp())
    {
        return 0;
    }

    for(i = 0; i < len; i++)
    {
        if(!writeWord(addr, mem[i]))
        {
            return 0;
        }
        addr += 4;
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
        MXC_FLC->ctrl |= MXC_S_FLC_CTRL_ERASE_CODE_ERASEPAGE;

        // TODO - timeout?
        // Wait until flash operation is complete.
        while(MXC_FLC->ctrl & MXC_S_FLC_CTRL_ERASE_CODE_ERASEPAGE);

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
    // Set flash clock divider to generate a 1MHz clock from the APB clock.
    MXC_FLC->clkdiv = CORE_CLOCK_MHZ;

    // TODO - timeout??
    // Check if the flash controller is busy.
    while(MXC_FLC->ctrl & (MXC_F_FLC_CTRL_WRITE | MXC_F_FLC_CTRL_MASS_ERASE | MXC_F_FLC_CTRL_PAGE_ERASE));

    // Unlock flash.
    MXC_FLC->ctrl = (MXC_FLC->ctrl & ~MXC_F_FLC_CTRL_UNLOCK_CODE) | MXC_S_FLC_CTRL_UNLOCK_CODE_UNLOCKED;

    return 1;
}

static int endFlashOp()
{
    // Lock flash.
    MXC_FLC->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK_CODE;

    // Flush the instruction cache.
    MXC_GCR->scon |= MXC_F_GCR_SCON_ICC0_FLUSH;
    
    // TODO - timeout??
    // Wait for flush to complete.
    while(MXC_GCR->scon & MXC_F_GCR_SCON_ICC0_FLUSH);
    
    return 1;
}




///////////////////////////////////////////////////////////////////////////////
// CMSIS Startup Function Overrides
///////////////////////////////////////////////////////////////////////////////

void SystemInit(void)
{
    uint32_t ovr, div;

    MXC_WDT0->ctrl &= ~MXC_F_WDT_CTRL_WDT_EN;  /* Turn off watchdog. Application can re-enable as needed. */

    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

    // Set flash wait states higher than what the minimum for the fastest clock is
    MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x5UL << MXC_F_GCR_MEM_CTRL_FWS_POS);

    // Enable 96MHz Clock
    MXC_GCR->clk_ctrl |=MXC_F_GCR_CLK_CTRL_HIRC_EN;

    // TODO - timeout???
    // Wait for clock to be ready
    while((MXC_GCR->clk_ctrl & MXC_F_GCR_CLK_CTRL_HIRC_RDY) == 0);
    
    // Set 96MHz clock as System Clock
    MXC_SETFIELD(MXC_GCR->clk_ctrl, MXC_F_GCR_CLK_CTRL_CLKSEL, MXC_S_GCR_CLK_CTRL_CLKSEL_HIRC);

    // TODO - timeout???
    // Wait for system clock to be ready
    while((MXC_GCR->clk_ctrl & MXC_F_GCR_CLK_CTRL_CLKRDY) == 0);

    // Get the clock divider
    div = (MXC_GCR->clk_ctrl & MXC_F_GCR_CLK_CTRL_PSC) >> MXC_F_GCR_CLK_CTRL_PSC_POS;

    // Get OVR setting
    ovr = (MXC_PWRSEQ->lp_ctrl & MXC_F_PWRSEQ_LP_CTRL_OVR);

    // Set flash wait settings
    if(ovr == MXC_S_PWRSEQ_LP_CTRL_OVR_0_9V)
    {
        if(div == 0)
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x2UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        } 
        else
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x1UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        }
    }
    else if(ovr == MXC_S_PWRSEQ_LP_CTRL_OVR_1_0V)
    {
        if(div == 0)
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x2UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        } 
        else
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x1UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        }
    }
    else 
    {
        if(div == 0)
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x4UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        }
        else if(div == 1)
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x2UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        } 
        else
        {
            MXC_GCR->mem_ctrl = (MXC_GCR->mem_ctrl & ~(MXC_F_GCR_MEM_CTRL_FWS)) | (0x1UL << MXC_F_GCR_MEM_CTRL_FWS_POS);
        }
    }

    /* Disable clocks to peripherals by default to reduce power */
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_DMAD;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_SPI0D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_SPI1D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_UART0D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_UART1D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_I2C0D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_TIMER0D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_TIMER1D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_TIMER2D;
    MXC_GCR->pclk_dis0 |= MXC_F_GCR_PCLK_DIS0_I2C1D;
}




///////////////////////////////////////////////////////////////////////////////
// Loader Entry Point
///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    uint32_t commitAddr;
    
    // Get the state of the commit list.
    commitAddr = getNextRecord();
    
    // Check if commit area is empty.
    if(commitAddr == (COMMIT_START + COMMIT_LENGTH - 4))
    {
        // Write the commit record to indicate A is committed.
        // TODO - check return
        writeFlash(commitAddr, 0);
        commitAddr -= 4;
    }
    
    // See if A or B is the currently commited image
    if((commitAddr & 0x4) == 0)
    {
        // A is committed.  Assume it is valid and check the validity of B.
        if(validateImageB())
        {
            // Set the loader state to reflect the discovered state.
            loaderState = STATE2_RUN_A_COMMIT_A_VALID_AB_IDLE;
        }
        else
        {
            // Set the loader state to reflect the discovered state.
            loaderState = STATE0_RUN_A_COMMIT_A_VALID_A_IDLE;
        }
        // Launch image A.
        startImageA();
    }
    else
    {
        // B is committed.  Assume it is valid and check the validity of A.
        if(validateImageA())
        {
            // Set the loader state to reflect the discovered state.
            loaderState = STATE5_RUN_B_COMMIT_B_VALID_AB_IDLE;
        }
        else
        {
            // Set the loader state to reflect the discovered state.
            loaderState = STATE7_RUN_B_COMMIT_B_VALID_B_IDLE;
        }
        // Launch image B.
        startImageB();
    }
}


