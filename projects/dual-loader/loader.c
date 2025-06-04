/*******************************************************************************
 * MAX32672 Dual Bank Flash Bootloader
 * 
 * Bank 0 (FLC0): Bootloader (8KB) + Image A
 * Bank 1 (FLC1): Commit area (8KB) + Image B
 ******************************************************************************/

#include <mxc_device.h>
#include <flc_regs.h>
#include <gcr_regs.h>
#include <icc_regs.h>
#include <wdt_regs.h>
#include <pwrseq_regs.h>
#include <trimsir_regs.h>
#include "loader.h"

/* HAL includes */
#include "flc.h"
#include "icc.h"
#include "mxc_sys.h"

///////////////////////////////////////////////////////////////////////////////
// Local Definitions
///////////////////////////////////////////////////////////////////////////////

#define REV_MAJOR               0
#define REV_MINOR               2

// Flash page size in bytes (8KB for MAX32672)
#define PAGE_SIZE               0x2000      // 8K Bytes
#define LOADER_FUNCTION_TABLE   (PAGE_SIZE - 128)

// Number of pages used by each region
#define LOADER_PAGES            1   // 8KB
#define COMMIT_PAGES            1   // 8KB
#define IMAGE_A_PAGES           16  // 128KB / 8KB = 16 pages
#define IMAGE_B_PAGES           16  // 128KB / 8KB = 16 pages

// Length of each region in bytes
#define LOADER_LENGTH           (LOADER_PAGES * PAGE_SIZE)
#define COMMIT_LENGTH           (COMMIT_PAGES * PAGE_SIZE)
#define IMAGE_A_LENGTH          (IMAGE_A_PAGES * PAGE_SIZE)
#define IMAGE_B_LENGTH          (IMAGE_B_PAGES * PAGE_SIZE)

// Starting address of each region
// Bank 0 (0x10000000-0x1007FFFF): Bootloader + Image A
#define LOADER_START            (0x10000000)
#define IMAGE_A_START           (LOADER_START + LOADER_LENGTH)

// Bank 1 (0x10080000-0x100FFFFF): Commit + Image B
#define COMMIT_START            (0x10080000)
#define IMAGE_B_START           (COMMIT_START + COMMIT_LENGTH)

// Manual flash controller selection to avoid HAL issues
#define GET_FLC_INSTANCE(addr)  ((addr) < 0x10080000 ? MXC_FLC0 : MXC_FLC1)

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

// Writes the given 128-bit word to flash.
static int writeFlash(uint32_t addr, uint32_t val[4]);

// Writes the given array to flash.
static int writeBytes(uint32_t addr, uint8_t* buff, uint32_t len);

// Returns the next available address in the commit list.
static uint32_t getNextRecord(void);

// Returns the CRC of the given memory range.
static uint32_t doCRC(uint8_t* buff, uint32_t len);

// HAL flash functions handle RAM execution internally

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
    // if(loaderState.running_image != 0) {
    //     return LDR_IMAGE_RUNNING;
    // }
    
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
    // if(loaderState.running_image != 1) {
    //     return LDR_IMAGE_RUNNING;
    // }
    
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
    // Ensure flash controllers are idle before jumping
    while(MXC_FLC0->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));
    while(MXC_FLC1->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));
    
    // Lock both flash controllers
    MXC_FLC0->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
    MXC_FLC1->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
    
    // Invalidate instruction cache
    if (MXC_ICC->ctrl & MXC_F_ICC_CTRL_EN) {
        MXC_ICC->invalidate = 1;
        while (MXC_ICC->invalidate & 1);
    }
    
    // Reset all peripherals and GPIO
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    
    // Set VTOR to point to Image A
    SCB->VTOR = IMAGE_A_START;
    
    // Load stack pointer and reset handler from Image A vector table
    __set_MSP(*(uint32_t*)IMAGE_A_START);
    
    // Jump to Image A reset handler
    void (*resetHandler)(void) = (void (*)(void))(*(uint32_t*)(IMAGE_A_START + 4));
    resetHandler();
}

static void startImageB()
{
    // Ensure flash controllers are idle before jumping
    while(MXC_FLC0->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));
    while(MXC_FLC1->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));
    
    // Lock both flash controllers
    MXC_FLC0->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
    MXC_FLC1->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;
    
    // Invalidate instruction cache
    if (MXC_ICC->ctrl & MXC_F_ICC_CTRL_EN) {
        MXC_ICC->invalidate = 1;
        while (MXC_ICC->invalidate & 1);
    }
    
    // Reset all peripherals and GPIO
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    
    // Set VTOR to point to Image B
    SCB->VTOR = IMAGE_B_START;
    
    // Load stack pointer and reset handler from Image B vector table
    __set_MSP(*(uint32_t*)IMAGE_B_START);
    
    // Jump to Image B reset handler
    void (*resetHandler)(void) = (void (*)(void))(*(uint32_t*)(IMAGE_B_START + 4));
    resetHandler();
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
        // Check if this 128-bit block is a commit record (all zeros)
        if((mem[i] == 0) && (mem[i+1] == 0) && (mem[i+2] == 0) && 
           ((mem[i+3] == 0)))
        {
            // See if commit region is full.
            if(i == 0)
            {
                // Erase commit area and start over
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


static int writeFlash(uint32_t addr, uint32_t val[4])
{
    int ret = 1;
    int err;
    
    // Make sure address is on a 128-bit boundary.
    if(addr & 0xf) 
    {
        return 0;
    }

    // Use HAL function to write 128-bit word
    err = MXC_FLC_Write128(addr, val);
    if(err != E_NO_ERROR)
    {
        ret = 0;
    }

    return ret;
}

static int writeBytes(uint32_t addr, uint8_t* buff, uint32_t len)
{
    int ret = 1;
    int err;
    
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
    
    // Use HAL function to write data
    err = MXC_FLC_Write(addr, len, (uint32_t*)buff);
    if(err != E_NO_ERROR)
    {
        ret = 0;
    }

    return ret;
}

static int erasePages(uint32_t addr, uint32_t len)
{
    int ret = 1;
    int err;
    
    // Align start address on a page boundary.
    addr &= ~(PAGE_SIZE - 1);
    
    while(len > 0)
    {
        // Use HAL function to erase page
        err = MXC_FLC_PageErase(addr);
        if(err != E_NO_ERROR)
        {
            ret = 0;
            break;
        }
        
        // Go to next page.
        addr += PAGE_SIZE;
        if (len >= PAGE_SIZE) {
            len -= PAGE_SIZE;
        } else {
            len = 0;
        }
    }
    
    return ret;
}

// CRC-32
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

static int startFlashOp(mxc_flc_regs_t* flc)
{
    // The flash controller requires a 1MHz internal clock for write and erase operations.
    flc->clkdiv = SystemCoreClock / 1000000;

    // Check if the flash controller is busy.
    while(flc->ctrl & (MXC_F_FLC_CTRL_WR | MXC_F_FLC_CTRL_ME | MXC_F_FLC_CTRL_PGE));

    // Unlock flash.
    flc->ctrl = (flc->ctrl & ~MXC_F_FLC_CTRL_UNLOCK) | MXC_S_FLC_CTRL_UNLOCK_UNLOCKED;

    return 1;
}

static int endFlashOp(mxc_flc_regs_t* flc)
{
    // Lock flash.
    flc->ctrl &= ~MXC_F_FLC_CTRL_UNLOCK;

    // Flush the instruction cache if it's enabled
    if (MXC_ICC->ctrl & MXC_F_ICC_CTRL_EN) {
        MXC_ICC->invalidate = 1;
        while (MXC_ICC->invalidate & 1);
    }
    
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
    /* Disable all interrupts */
    __disable_irq();

    /* Turn off watchdog */
    MXC_WDT0->ctrl &= ~MXC_F_WDT_CTRL_EN;

    /* Disable ECC on flash. */
    MXC_TRIMSIR->bb_sir2 &= ~(MXC_F_TRIMSIR_BB_SIR2_FL0ECCEN | MXC_F_TRIMSIR_BB_SIR2_FL1ECCEN);

    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

    /* Initialize clock - switch to 100MHz IPO */
    /* Enable 100MHz IPO clock */
    MXC_GCR->clkctrl |= MXC_F_GCR_CLKCTRL_IPO_EN;
    
    /* Wait for IPO to be ready */
    while(!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_IPO_RDY));
    
    /* Set IPO as system clock */
    MXC_SETFIELD(MXC_GCR->clkctrl, MXC_F_GCR_CLKCTRL_SYSCLK_SEL, MXC_S_GCR_CLKCTRL_SYSCLK_SEL_IPO);
    
    /* Wait for switch to complete */
    while(!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_SYSCLK_RDY));

    /* Update SystemCoreClock */
    SystemCoreClock = 100000000;

    /* Configure wait states for 100MHz operation */
    MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x4UL << MXC_F_GCR_MEMCTRL_FWS_POS);

    /* Enable GPIO clocks */
    MXC_GCR->pclkdis0 &= ~(MXC_F_GCR_PCLKDIS0_GPIO0 | MXC_F_GCR_PCLKDIS0_GPIO1);

    /* Make sure interrupts are enabled */
    __enable_irq();
}

///////////////////////////////////////////////////////////////////////////////
// Loader Entry Point
///////////////////////////////////////////////////////////////////////////////

int main(void)
{    
     //erasePages(COMMIT_START, COMMIT_LENGTH);
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

// Custom flash functions removed - using HAL functions instead
///////////////////////////////////////////////////////////////////////////////
// PreInit function - called by CMSIS startup before RAM initialization
///////////////////////////////////////////////////////////////////////////////

// Weak function that can be overridden
__attribute__((weak))
int PreInit(void)
{
    // Return 0 to indicate RAM should be initialized normally
    // Return non-zero to skip RAM initialization
    return 0;
}

// Dummy implementation of libc_init_array for bootloader
// We don't need C++ constructor support in the bootloader
void __libc_init_array(void)
{
    // Empty - no constructors to call
}
