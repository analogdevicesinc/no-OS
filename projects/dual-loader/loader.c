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
#include <string.h>
#include "loader.h"

/* HAL includes for flash operations */
#include <flc.h>
#include <mxc_sys.h>

#define REV_MAJOR               0
#define REV_MINOR               2

// Flash page size in bytes (8KB for MAX32672)
// Target-specific configuration
#if defined(TARGET_MAX32672)
    #define PAGE_SIZE               0x2000      // 8K Bytes
    #define LOADER_PAGES            1           // 8KB
    #define COMMIT_PAGES            1           // 8KB
    #define IMAGE_A_PAGES           63          // 504KB / 8KB = 63 pages
    #define IMAGE_B_PAGES           63          // 504KB / 8KB = 63 pages
    
    // MAX32672 memory layout:
    // Bank 0 (0x10000000-0x1007FFFF): Bootloader + Image A
    // Bank 1 (0x10080000-0x100FFFFF): Commit + Image B
    #define LOADER_START            (0x10000000)
    #define IMAGE_A_START           (LOADER_START + LOADER_LENGTH)
    #define COMMIT_START            (0x10080000)
    #define IMAGE_B_START           (COMMIT_START + COMMIT_LENGTH)
    
    // Manual flash controller selection
    #define GET_FLC_INSTANCE(addr)  ((addr) < 0x10080000 ? MXC_FLC0 : MXC_FLC1)
    
#elif defined(TARGET_MAX32690)
    #define PAGE_SIZE               0x4000      // 16K Bytes
    #define LOADER_PAGES            1           // 16KB
    #define COMMIT_PAGES            1           // 16KB
    #define IMAGE_A_PAGES           31          // 496KB / 16KB = 31 pages
    #define IMAGE_B_PAGES           31          // 496KB / 16KB = 31 pages
    
    // MAX32690 memory layout (all in Bank 0):
    // BOOT: 1 page (16K) at 0x10000000
    // COMMIT: 1 page (16K) at 0x10004000
    // IMAGE A: 31 pages (496K) at 0x10008000
    // IMAGE B: 31 pages (496K) at 0x10084000
    #define LOADER_START            (0x10000000)
    #define COMMIT_START            (0x10004000)
    #define IMAGE_A_START           (0x10008000)
    #define IMAGE_B_START           (0x10084000)
    
    // MAX32690 has only one flash controller
    #define GET_FLC_INSTANCE(addr)  (MXC_FLC0)
    
#else
    #error "TARGET must be defined as TARGET_MAX32672 or TARGET_MAX32690"
#endif

#define LOADER_FUNCTION_TABLE   (PAGE_SIZE - 128)

// Length of each region in bytes (common calculation)
#define LOADER_LENGTH           (LOADER_PAGES * PAGE_SIZE)
#define COMMIT_LENGTH           (COMMIT_PAGES * PAGE_SIZE)
#define IMAGE_A_LENGTH          (IMAGE_A_PAGES * PAGE_SIZE)
#define IMAGE_B_LENGTH          (IMAGE_B_PAGES * PAGE_SIZE)

// Simple loader state tracking
typedef struct {
    uint8_t running_image;      // 0=A, 1=B
    uint8_t committed_image;    // 0=A, 1=B
    uint8_t download_active;    // 0=idle, 1=active
} dl_state_t;

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
static dl_state_t __attribute__((section (".loaderVariables"))) loaderState;

///////////////////////////////////////////////////////////////////////////////
// Local Function Prototypes
///////////////////////////////////////////////////////////////////////////////

// Write the commit record for image A.  Returns 0 on failure.
static int commit_image_a(void);

// Write the commit record for image B.  Returns 0 on failure.
static int commit_image_b(void);

// Erases and invalidates image A.  Returns 0 on failure.
static int erase_image_a(void);

// Erases and invalidates image B.  Returns 0 on failure.
static int erase_image_b(void);

// Write given data to image A.  Returns 0 on failure.
static int write_image_a(uint32_t offset, uint8_t* buff, uint32_t length);

// Write given data to image B.  Returns 0 on failure.
static int write_image_b(uint32_t offset, uint8_t* buff, uint32_t length);

// Checks the CRC of image A.  Returns 0 on failure.
static int validate_image_a(void);

// Checks the CRC of image B.  Returns 0 on failure.
static int validate_image_b(void);

// Resets and runs image A.
static void start_image_a(void);

// Resets and runs image B.
static void start_image_b(void);

// Checks if image A is empty/erased.  Returns 1 on empty.
static int image_a_empty(void);

// Checks if image B is empty/erased.  Returns 1 on empty.
static int image_b_empty(void);

// Erase the range given - wrapper for HAL
static int erase_pages(uint32_t addr, uint32_t len);

// Writes the given 128-bit word to flash - wrapper for HAL
static int write_flash(uint32_t addr, const uint32_t *val);

// Writes the given array to flash - wrapper for HAL
static int write_bytes(uint32_t addr, uint8_t* buff, uint32_t len);

// Returns the next available address in the commit list.
static uint32_t get_next_record(void);

// Returns the CRC of the given memory range.
static uint32_t do_crc(uint8_t* buff, uint32_t len);

// Helper functions for simplified state management
static int is_image_a_valid(void);
static int is_image_b_valid(void);
static int get_committed_image(void);
static void set_committed_image(int image);

int dl_start_download(void) 
{
    // Check if download already in progress
    if(loaderState.download_active) {
        return DL_DOWNLOAD_IN_PROGRESS;
    }
    
    // Determine which image to update (opposite of running image)
    int target_image = 1 - loaderState.running_image;
    
    // Cannot update the currently running image
    if(target_image == loaderState.running_image) {
        return DL_IMAGE_RUNNING;
    }
    
    // Erase the target image
    int result;
    if(target_image == 0) {
        result = erase_image_a();
    } else {
        result = erase_image_b();
    }
    
    if(!result) {
        return DL_FLASH_ERROR;
    }
    
    // Mark download as active
    loaderState.download_active = 1;
    return DL_SUCCESS;
}

int dl_download_image_block(uint32_t offset, uint8_t* buff, uint32_t length) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return DL_DOWNLOAD_NOT_STARTED;
    }
    
    // Write to the opposite of the running image
    int target_image = 1 - loaderState.running_image;
    int result;
    
    if(target_image == 0) {
        result = write_image_a(offset, buff, length);
    } else {
        result = write_image_b(offset, buff, length);
    }
    
    return result ? DL_SUCCESS : DL_FLASH_ERROR;
}

int dl_abort_upgrade(void) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return DL_DOWNLOAD_NOT_STARTED;
    }
    
    // Mark download as inactive
    loaderState.download_active = 0;
    return DL_SUCCESS;
}

int dl_complete_download(void) 
{
    // Check if download session is active
    if(!loaderState.download_active) {
        return DL_DOWNLOAD_NOT_STARTED;
    }
    
    // Validate the target image (opposite of running)
    int target_image = 1 - loaderState.running_image;
    int valid;
    
    if(target_image == 0) {
        valid = is_image_a_valid();
    } else {
        valid = is_image_b_valid();
    }
    
    // Mark download as inactive
    loaderState.download_active = 0;
    
    if(valid) {
        return DL_SUCCESS;
    } else {
        return DL_IMAGE_NOT_VALID;
    }
}

int dl_run_image_a(void) 
{
    // Check if image A is valid
    if(!is_image_a_valid()) {
        return DL_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress on A
    if(loaderState.download_active && loaderState.running_image == 1) {
        return DL_DOWNLOAD_IN_PROGRESS;
    }
    
    // Update state and start image A
    loaderState.running_image = 0;
    loaderState.download_active = 0;
    start_image_a();
    
    // Should never reach here
    return DL_UNKNOWN_ERROR;
}

int dl_run_image_b(void) 
{
    // Check if image B is valid
    if(!is_image_b_valid()) {
        return DL_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress on B
    if(loaderState.download_active && loaderState.running_image == 0) {
        return DL_DOWNLOAD_IN_PROGRESS;
    }
    
    // Update state and start image B
    loaderState.running_image = 1;
    loaderState.download_active = 0;
    start_image_b();
    
    // Should never reach here
    return DL_UNKNOWN_ERROR;
}

int dl_commit_image_a(void) 
{
    // Check if image A is valid
    if(!is_image_a_valid()) {
        return DL_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress
    if(loaderState.download_active) {
        return DL_DOWNLOAD_IN_PROGRESS;
    }
    
    // Only allow committing A if A is running
    if(loaderState.running_image != 0) {
        return DL_IMAGE_RUNNING;
    }
    
    // Check if A is already committed
    if(loaderState.committed_image == 0) {
        return DL_SUCCESS;
    }
    
    // Commit image A
    if(!commit_image_a()) {
        return DL_FLASH_ERROR;
    }
    
    loaderState.committed_image = 0;
    return DL_SUCCESS;
}

int dl_commit_image_b(void) 
{
    // Check if image B is valid
    if(!is_image_b_valid()) {
        return DL_IMAGE_NOT_VALID;
    }
    
    // Check if download is in progress
    if(loaderState.download_active) {
        return DL_DOWNLOAD_IN_PROGRESS;
    }
    
    // Only allow committing B if B is running
    if(loaderState.running_image != 1) {
        return DL_IMAGE_RUNNING;
    }
    
    // Check if B is already committed
    if(loaderState.committed_image == 1) {
        return DL_SUCCESS;
    }
    
    // Commit image B
    if(!commit_image_b()) {
        return DL_FLASH_ERROR;
    }
    
    loaderState.committed_image = 1;
    return DL_SUCCESS;
}

int dl_get_image_status(dl_image_status_t* status) 
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
    if(is_image_a_valid()) {
        status->image_status |= VALID_A;
    } else if(!image_a_empty()) {
        status->image_status |= INVALID_A;
    }
    
    if(is_image_b_valid()) {
        status->image_status |= VALID_B;
    } else if(!image_b_empty()) {
        status->image_status |= INVALID_B;
    }
    
    return DL_SUCCESS;
}

uint32_t dl_get_version(void)
{
    return (REV_MAJOR << 8) | REV_MINOR;
}

uint32_t dl_get_build_date(void)
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

static int commit_image_a()
{
    uint32_t recordLoc;
    const uint32_t zeros[4] = {0, 0, 0, 0};
    
    // Get location next commit record should be written.
    recordLoc = get_next_record();
    
    // Image A commit always happens on an even address.
    if((recordLoc & 0x10) == 0)
    {
        // Trying to commit A when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return write_flash(recordLoc, zeros);
}

static int commit_image_b()
{
    uint32_t recordLoc;
    const uint32_t zeros[4] = {0, 0, 0, 0};
    
    // Get location next commit record should be written.
    recordLoc = get_next_record();
    
    // Image B commit always happens on an odd address.
    if((recordLoc & 0x10) != 0)
    {
        // Trying to commit B when it is already committed.
        return 0;
    }
    
    // Write the commit record.
    return write_flash(recordLoc, zeros);
}

static int erase_image_a()
{
    return erase_pages(IMAGE_A_START, IMAGE_A_LENGTH);
}

static int erase_image_b()
{
    return erase_pages(IMAGE_B_START, IMAGE_B_LENGTH);
}

static int write_image_a(uint32_t offset, uint8_t* buff, uint32_t length)
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
    return write_bytes(offset, buff, length);
}

static int write_image_b(uint32_t offset, uint8_t* buff, uint32_t length)
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
    return write_bytes(offset, buff, length);
}

static int validate_image_a()
{
    // Get a pointer to flash
    image_footer_t* ftr = (image_footer_t*)(IMAGE_A_START + IMAGE_A_LENGTH - sizeof(image_footer_t));
    
    // See if len is valid.  If not, image cannot be valid.
    if(ftr->len > (IMAGE_A_LENGTH - sizeof(image_footer_t)))
    {
        return 0;
    }
    
    // Compute the CRC of the image
    uint32_t crc = do_crc((uint8_t*)IMAGE_A_START, ftr->len);
    return (crc == ftr->crc);
}

static int validate_image_b()
{
    // Get a pointer to flash
    image_footer_t* ftr = (image_footer_t*)(IMAGE_B_START + IMAGE_B_LENGTH - sizeof(image_footer_t));
    
    // See if len is valid.  If not, image cannot be valid.
    if(ftr->len > (IMAGE_B_LENGTH - sizeof(image_footer_t)))
    {
        return 0;
    }
    
    // Compute the CRC of the image
    uint32_t crc = do_crc((uint8_t*)IMAGE_B_START, ftr->len);
    return (crc == ftr->crc);
}

static void start_image_a()
{
    // Complete interrupt system reset - CRITICAL for FreeRTOS
    __disable_irq();
    
    // Reset SysTick (FreeRTOS scheduler dependency)
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    
    // Reset NVIC completely
    for(int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;  // Disable all interrupts
        NVIC->ICPR[i] = 0xFFFFFFFF;  // Clear pending interrupts
    }
    
    // Reset interrupt priorities to default
    for(int i = 0; i < 240; i++) {
        NVIC->IP[i] = 0;
    }
    
    // Note: Removed dangerous memory clearing - was destroying active task stacks
    
    // Set VTOR first, before any resets
    SCB->VTOR = IMAGE_A_START;
    __DSB(); __ISB();  // Ensure VTOR is set
    
    // Validate vector table before using it
    uint32_t stack_ptr = *(uint32_t*)IMAGE_A_START;
    uint32_t reset_addr = *(uint32_t*)(IMAGE_A_START + 4);
    
    // Basic validation: stack should be in SRAM, reset in flash
    if ((stack_ptr & 0xF0000000) != 0x20000000 || (reset_addr & 0xF0000000) != 0x10000000) {
        // Invalid vector table, halt
        while(1);
    }
    
    // Reset all peripherals and GPIO AFTER setting VTOR
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    
    // Wait for reset to complete (add delay)
    for(volatile int i = 0; i < 1000; i++);
    __DSB(); __ISB();  // Memory barriers
    
    // Load stack pointer and reset handler from Image A vector table
    __set_MSP(stack_ptr);
    
    // Jump to Image A reset handler
    void (*resetHandler)(void) = (void (*)(void))reset_addr;
    resetHandler();
}

static void start_image_b()
{
    // Complete interrupt system reset - CRITICAL for FreeRTOS
    __disable_irq();
    
    // Reset SysTick (FreeRTOS scheduler dependency)
    SysTick->CTRL = 0;
    SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
    
    // Reset NVIC completely
    for(int i = 0; i < 8; i++) {
        NVIC->ICER[i] = 0xFFFFFFFF;  // Disable all interrupts
        NVIC->ICPR[i] = 0xFFFFFFFF;  // Clear pending interrupts
    }
    
    // Reset interrupt priorities to default
    for(int i = 0; i < 240; i++) {
        NVIC->IP[i] = 0;
    }
    
    // Note: Removed dangerous memory clearing - was destroying active task stacks
    
    // Set VTOR first, before any resets
    SCB->VTOR = IMAGE_B_START;
    __DSB(); __ISB();  // Ensure VTOR is set
    
    // Validate vector table before using it
    uint32_t stack_ptr = *(uint32_t*)IMAGE_B_START;
    uint32_t reset_addr = *(uint32_t*)(IMAGE_B_START + 4);
    
    // Basic validation: stack should be in SRAM, reset in flash
    if ((stack_ptr & 0xF0000000) != 0x20000000 || (reset_addr & 0xF0000000) != 0x10000000) {
        // Invalid vector table, halt
        while(1);
    }
    
    // Reset all peripherals and GPIO AFTER setting VTOR
    MXC_GCR->rst0 |= MXC_F_GCR_RST0_SOFT;
    
    // Wait for reset to complete (add delay)
    for(volatile int i = 0; i < 1000; i++);
    __DSB(); __ISB();  // Memory barriers
    
    // Load stack pointer and reset handler from Image B vector table
    __set_MSP(stack_ptr);
    
    // Jump to Image B reset handler
    void (*resetHandler)(void) = (void (*)(void))reset_addr;
    resetHandler();
}

static int image_a_empty()
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

static int image_b_empty()
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

static uint32_t get_next_record()
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
                erase_pages(COMMIT_START, COMMIT_LENGTH);
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


static int write_flash(uint32_t addr, const uint32_t *val)
{
    int result;
    
    // Make sure address is on a 128-bit boundary.
    if(addr & 0xf) 
    {
        return 0;
    }

    // Use HAL to write 128-bit data
    result = MXC_FLC_Write128(addr, val);
    
    return (result == E_NO_ERROR) ? 1 : 0;
}

static int write_bytes(uint32_t addr, uint8_t* buff, uint32_t len)
{
    int result;
    
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
    
    // Use HAL to write the buffer
    result = MXC_FLC_Write(addr, len, (uint32_t*)buff);
    
    if(result != E_NO_ERROR)
    {
        return 0;
    }

    // HAL includes verification, so no need to verify manually
    return 1;
}

static int erase_pages(uint32_t addr, uint32_t len)
{
    int result;
    
    // Align start address on a page boundary.
    addr &= ~(PAGE_SIZE - 1);
    
    while(len > 0)
    {
        // Use HAL to erase one page
        result = MXC_FLC_PageErase(addr);
        
        if(result != E_NO_ERROR)
        {
            return 0;
        }
        
        // Go to next page.
        addr += PAGE_SIZE;
        if (len >= PAGE_SIZE) {
            len -= PAGE_SIZE;
        } else {
            len = 0;
        }
    }
    
    return 1;
}

// CRC-32
#define poly 0xEDB88320UL
static uint32_t do_crc(uint8_t* data, uint32_t len)
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

// Helper function implementations
static int is_image_a_valid(void)
{
    return validate_image_a() && !image_a_empty();
}

static int is_image_b_valid(void)
{
    return validate_image_b() && !image_b_empty();
}

static int get_committed_image(void)
{
    uint32_t commitAddr = get_next_record();
    // If commit area is empty, default to A
    if(commitAddr == (COMMIT_START + COMMIT_LENGTH - 16)) {
        return 0; // A
    }
    // Check if last commit was for A (even address) or B (odd address)
    return (commitAddr & 16) ? 1 : 0; // A=0, B=1
}

static void set_committed_image(int image)
{
    loaderState.committed_image = image;
}

void SystemInit(void)
{
    /* Disable all interrupts */
    __disable_irq();

    /* Turn off watchdog */
    MXC_WDT0->ctrl &= ~MXC_F_WDT_CTRL_EN;

#if defined(TARGET_MAX32672)
    /* Disable ECC on flash - MAX32672 only */
    MXC_TRIMSIR->bb_sir2 &= ~(MXC_F_TRIMSIR_BB_SIR2_FL0ECCEN | MXC_F_TRIMSIR_BB_SIR2_FL1ECCEN);
#endif

    /* Enable FPU on Cortex-M4, which occupies coprocessor slots 10 & 11 */
    /* Grant full access, per "Table B3-24 CPACR bit assignments". */
    /* DDI0403D "ARMv7-M Architecture Reference Manual" */
    SCB->CPACR |= SCB_CPACR_CP10_Msk | SCB_CPACR_CP11_Msk;
    __DSB();
    __ISB();

    /* Initialize clock */
    /* Enable IPO clock */
    MXC_GCR->clkctrl |= MXC_F_GCR_CLKCTRL_IPO_EN;
    
    /* Wait for IPO to be ready */
    while(!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_IPO_RDY));
    
    /* Set IPO as system clock */
    MXC_SETFIELD(MXC_GCR->clkctrl, MXC_F_GCR_CLKCTRL_SYSCLK_SEL, MXC_S_GCR_CLKCTRL_SYSCLK_SEL_IPO);
    
    /* Wait for switch to complete */
    while(!(MXC_GCR->clkctrl & MXC_F_GCR_CLKCTRL_SYSCLK_RDY));

#if defined(TARGET_MAX32672)
    /* Update SystemCoreClock for MAX32672 (100MHz) */
    SystemCoreClock = 100000000;
    /* Configure wait states for 100MHz operation (4 wait states) */
    MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x4UL << MXC_F_GCR_MEMCTRL_FWS_POS);
#elif defined(TARGET_MAX32690)
    /* Update SystemCoreClock for MAX32690 (120MHz) */
    SystemCoreClock = 120000000;
    /* Configure wait states for 120MHz operation (5 wait states) */
    MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x5UL << MXC_F_GCR_MEMCTRL_FWS_POS);
#endif

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
    // Initialize state structure 
    loaderState.download_active = 0;
    
    // Determine which image is committed from flash
    loaderState.committed_image = get_committed_image();
    
    // Check if commit area is empty (very first boot)
    uint32_t commitAddr = get_next_record();
    if(commitAddr == (COMMIT_START + COMMIT_LENGTH - 16))
    {
        const uint32_t zeros[4] = {0, 0, 0, 0};
        
        // First boot - validate and commit image A as default
        if(is_image_a_valid())
        {
            write_flash(commitAddr, zeros);
            loaderState.committed_image = 0; // A
            loaderState.running_image = 0;   // A
            start_image_a();
        }
        else if(is_image_b_valid())
        {
            // A invalid, try B
            write_flash(commitAddr - 16, zeros);
            loaderState.committed_image = 1; // B
            loaderState.running_image = 1;   // B
            start_image_b();
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
        if(is_image_a_valid())
        {
            loaderState.running_image = 0;
            start_image_a();
        }
        else if(is_image_b_valid())
        {
            // Fallback to B
            loaderState.running_image = 1;
            start_image_b();
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
        if(is_image_b_valid())
        {
            loaderState.running_image = 1;
            start_image_b();
        }
        else if(is_image_a_valid())
        {
            // Fallback to A
            loaderState.running_image = 0;
            start_image_a();
        }
        else
        {
            // Both invalid - recovery mode
            while(1); // TODO: Implement recovery mechanism
        }
    }
}
