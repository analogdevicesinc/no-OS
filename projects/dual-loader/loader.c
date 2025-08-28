/*******************************************************************************
 * MAX32672 Dual Bank Flash Bootloader
 * 
 * MEMORY LAYOUT:
 * 
 * MAX32672 (Dual Bank Configuration):
 * Bank 0 (0x10000000-0x1007FFFF): Bootloader (8KB) + Image A (504KB)
 * Bank 1 (0x10080000-0x100FFFFF): Commit (8KB) + Protected (8KB) + Image B (496KB)
 * 
 * 
 * MEMORY REGIONS:
 * - BOOTLOADER: Contains this bootloader code and function table
 * - COMMIT: Stores commit records to track which image should run on boot
 * - PROTECTED: Hardware configuration storage, preserved across all operations
 * - IMAGE A/B: Application firmware images with CRC validation
 * 
 * PROTECTED AREA FEATURES:
 * - One dedicated page for hardware configuration data
 * - NEVER erased by commit operations or image management functions
 * - Only accessible via dl_flash_write() and dl_flash_read()
 * - Survives all bootloader operations, image swaps, and firmware updates
 * - Contains persistent configuration that applications can read/write
 ******************************************************************************/

#include <mxc_device.h>
#include <gcr_regs.h>
#include <wdt_regs.h>
#include <trimsir_regs.h>
#include <string.h>
#include "loader.h"
#include "icc.h"
#include <flc.h>
#include <mxc_sys.h>

#define REV_MAJOR               0
#define REV_MINOR               3

// Flash page size in bytes (8KB for MAX32672)
// Target-specific configuration
#if defined(TARGET_MAX32672)
    #define PAGE_SIZE               0x2000      // 8K Bytes
    #define LOADER_PAGES            2           // 16KB
    #define COMMIT_PAGES            1           // 8KB (for commit records only)
    #define PROTECTED_A_PAGES       1           // 8KB (extra protected page)
    #define PROTECTED_B_PAGES       2           // 16KB (extra protected page)
    #define IMAGE_A_PAGES           61          // 488KB / 8KB = 61 pages
    #define IMAGE_B_PAGES           61          // 488KB / 8KB = 61 pages

    // MAX32672 memory layout:
    // Bank 0 (0x10000000-0x1007FFFF): Bootloader + Protected_A + Image A
    // Bank 1 (0x10080000-0x100FFFFF): Commit + Protected_B + Image B
    #define LOADER_START            (0x10000000)
    #define PROTECTED_A_START       (LOADER_START + LOADER_LENGTH)
    #define IMAGE_A_START           (PROTECTED_A_START + PROTECTED_A_LENGTH)
    #define COMMIT_START            (0x10080000)
    #define PROTECTED_B_START       (COMMIT_START + COMMIT_LENGTH)
    #define IMAGE_B_START           (PROTECTED_B_START + PROTECTED_B_LENGTH)

    // Manual flash controller selection
    #define GET_FLC_INSTANCE(addr)  ((addr) < 0x10080000 ? MXC_FLC0 : MXC_FLC1)

#else
    #error "TARGET must be defined as TARGET_MAX32672"
#endif

#define LOADER_FUNCTION_TABLE   (PAGE_SIZE - 128)

// Length of each region in bytes (common calculation)
#define LOADER_LENGTH           (LOADER_PAGES * PAGE_SIZE)
#define COMMIT_LENGTH           (COMMIT_PAGES * PAGE_SIZE)
#define PROTECTED_A_LENGTH      (PROTECTED_A_PAGES * PAGE_SIZE)  // Protected from all operations except config access
#define PROTECTED_B_LENGTH      (PROTECTED_B_PAGES * PAGE_SIZE)  // Protected from all operations except config access
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

// Image footer structure located at the end of each firmware image
// Contains metadata for image validation and integrity checking
typedef struct
{
    uint32_t reserved[2];   // Reserved fields for future use
    uint32_t crc;          // CRC-32 checksum of the image data
    uint32_t len;          // Length of image data in bytes (excluding footer)
} image_footer_t;

///////////////////////////////////////////////////////////////////////////////
// Local Variables
///////////////////////////////////////////////////////////////////////////////

// Bootloader state structure stored in dedicated SRAM section
// Tracks current running image, committed image, and download status
static dl_state_t __attribute__((section (".loaderVariables"))) loaderState;

///////////////////////////////////////////////////////////////////////////////
// Local Function Prototypes
///////////////////////////////////////////////////////////////////////////////

// Write the commit record for image A. Returns 0 on failure.
static int commit_image_a(void);

// Write the commit record for image B. Returns 0 on failure.
static int commit_image_b(void);

// Erases and invalidates image A. Returns 0 on failure.
static int erase_image_a(void);

// Erases and invalidates image B. Returns 0 on failure.
static int erase_image_b(void);

// Write given data to image A using relative offset. Returns 0 on failure.
// offset: Relative offset from start of Image A (0-based)
static int write_image_a(uint32_t offset, uint8_t* buff, uint32_t length);

// Write given data to image B using relative offset. Returns 0 on failure.
// offset: Relative offset from start of Image B (0-based)
static int write_image_b(uint32_t offset, uint8_t* buff, uint32_t length);

// Validates the CRC of image A. Returns 0 on failure.
static int validate_image_a(void);

// Validates the CRC of image B. Returns 0 on failure.
static int validate_image_b(void);

// Resets and transfers control to image A.
static void start_image_a(void);

// Resets and transfers control to image B.
static void start_image_b(void);

// Checks if image A is empty/erased. Returns 1 if empty.
static int image_a_empty(void);

// Checks if image B is empty/erased. Returns 1 if empty.
static int image_b_empty(void);

// Erase flash pages at given address and length. Returns 0 on failure.
static int erase_pages(uint32_t addr, uint32_t len);

// Writes 128-bit aligned data to flash. Returns 0 on failure.
static int write_flash(uint32_t addr, const uint32_t *val);

// Writes byte array to flash with proper alignment. Returns 0 on failure.
static int write_bytes(uint32_t addr, uint8_t* buff, uint32_t len);

// Returns the next available address in the commit area for new records.
static uint32_t get_next_record(void);

// Calculates CRC-32 of given memory buffer.
static uint32_t do_crc(uint8_t* buff, uint32_t len);

// Helper functions for image validation and state management
static int is_image_a_valid(void);
static int is_image_b_valid(void);
static int get_committed_image(void);

// Protected area access validation
static uint8_t is_address_in_protected_area(uint32_t addr, uint32_t len);

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
    // NOTE: offset parameter is RELATIVE to image start (0-based)
    // The write_image_x functions convert this to absolute addresses
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

    // Convert relative offset to absolute address
    uint32_t absolute_addr = offset;

    // Check if absolute address is in the valid range for Image A
    if(absolute_addr < IMAGE_A_START || absolute_addr >= (IMAGE_A_START + IMAGE_A_LENGTH))
    {
        return 0;
    }

    // Is the length in range?
    if(length > IMAGE_A_LENGTH)
    {
        return 0;
    }
    
    // Are the offset and length together in range?
    if((length) > IMAGE_A_LENGTH)
    {
        return 0;
    }
    
    // Write the bytes to image A using the absolute address
    return write_bytes(absolute_addr, buff, length);
}

static int write_image_b(uint32_t offset, uint8_t* buff, uint32_t length)
{
    // Check if image B is currently running
    if(loaderState.running_image == 1)
    {
        return 0;  // Cannot write to running image
    }

    // Convert relative offset to absolute address
    // uint32_t absolute_addr = IMAGE_B_START + offset;
    uint32_t absolute_addr = offset;

    // Check if absolute address is in the valid range for Image B
    if(absolute_addr < IMAGE_B_START || absolute_addr >= (IMAGE_B_START + IMAGE_B_LENGTH))
    {
        return 0;
    }

    // Is the length in range?
    if(length > IMAGE_B_LENGTH)
    {
        return 0;
    }
    
    // Are the offset and length together in range?
    if( length > IMAGE_B_LENGTH)
    {
        return 0;
    }
    
    // Write the bytes to image B using the absolute address
    return write_bytes(absolute_addr, buff, length);
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
    
    // Get a pointer to the flash memory - ONLY scan the commit area
    uint32_t* mem = (uint32_t*)COMMIT_START;
    
    // Look at the flash 4 bytes at a time - ONLY within commit area
    for(i = 0; i < COMMIT_LENGTH / 4; i+=4)
    {
        // Check if this 128-bit block is a commit record (all zeros)
        if((mem[i] == 0) && (mem[i+1] == 0) && (mem[i+2] == 0) && 
           ((mem[i+3] == 0)))
        {
            // See if commit region is full.
            if(i == 0)
            {
                // Erase ONLY commit area, NOT protected area
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
    
    // Commit area is empty, return byte address of the last 32-bit word in COMMIT area only.
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

    // Disable ICC before flash operations
    MXC_ICC_Disable();

    // Use HAL to write 128-bit data
    result = MXC_FLC_Write128(addr, (uint32_t*)val);
    
    MXC_ICC_Enable();
    
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
    
    // Disable ICC before flash operations
    MXC_ICC_Disable();
    
    // Use HAL to write the buffer
    result = MXC_FLC_Write(addr, len, (uint32_t*)buff);
    
    MXC_ICC_Enable();
    
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
    
    // Disable ICC before flash operations
    MXC_ICC_Disable();
    
    // Align start address on a page boundary.
    addr &= ~(PAGE_SIZE - 1);
    
    while(len > 0)
    {
        // Use HAL to erase one page
        result = MXC_FLC_PageErase(addr);
        
        if(result != E_NO_ERROR)
        {
            MXC_ICC_Enable();
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
    
    MXC_ICC_Enable();
    return 1;
}

// CRC-32 implementation using standard polynomial 0xEDB88320
// Used for validating firmware image integrity
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

///////////////////////////////////////////////////////////////////////////////
// Hardware Configuration Management
///////////////////////////////////////////////////////////////////////////////

// Protected area access validation
static uint8_t is_address_in_protected_area(uint32_t addr, uint32_t len)
{
    // Check if address range is completely within protected area
    if (!((addr >= PROTECTED_A_START && addr < (PROTECTED_A_START + PROTECTED_A_LENGTH)) || (addr >= PROTECTED_B_START && addr < (PROTECTED_B_START + PROTECTED_B_LENGTH)))) {
        return 0;  // Address starts outside of protected area
    }
   
    if (len == 0 || len > PAGE_SIZE) {
        return 0;  // Invalid size
    }
    
    return 1;  // Address range is valid
}


// Hardware configuration storage address - uses the entire protected page
// This area is isolated from all other bootloader operations and survives
// firmware updates, image swaps, and commit operations
#define HW_CONFIG_ADDR      (PROTECTED_A_START)  // Use the entire protected page

int dl_flash_write(uint32_t addr, void *data, uint32_t len)
{
    if(is_address_in_protected_area(addr, len))
    {
        // Erase the protected area (this is the ONLY function allowed to do this)
        // Disable ICC for the erase operation
        MXC_ICC_Disable();
        int erase_result = MXC_FLC_PageErase(addr);
        MXC_ICC_Enable();
        
        if (erase_result != E_NO_ERROR) {
            return DL_FLASH_ERROR;
        }

        // Write the configuration data to protected area using aligned buffer
        // Use the write_bytes function which has proper ICC protection
        if (!write_bytes(addr, (uint8_t*)data, len)) {
            return DL_FLASH_ERROR;
        }

        return DL_SUCCESS;
    }
    return DL_FLASH_ERROR;
}

int dl_flash_read(uint32_t addr, void *data, uint32_t len)
{
    if(is_address_in_protected_area(addr, len))
    {
        // Read from protected area
        // Note: Flash reads typically don't require ICC disable
        MXC_FLC_Read(addr, (uint8_t*)data, len);
        return DL_SUCCESS;
    }
    return DL_FLASH_ERROR;
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

    /* Update SystemCoreClock for MAX32672 (100MHz) */
    SystemCoreClock = 100000000;
    /* Configure wait states for 100MHz operation (4 wait states) */
    MXC_GCR->memctrl = (MXC_GCR->memctrl & ~(MXC_F_GCR_MEMCTRL_FWS)) | (0x4UL << MXC_F_GCR_MEMCTRL_FWS_POS);

    /* Enable GPIO clocks */
    MXC_GCR->pclkdis0 &= ~(MXC_F_GCR_PCLKDIS0_GPIO0 | MXC_F_GCR_PCLKDIS0_GPIO1);

    /* Make sure interrupts are enabled */
    __enable_irq();
}


///////////////////////////////////////////////////////////////////////////////
// Syscall Stubs
///////////////////////////////////////////////////////////////////////////////

// These functions provide stubs for newlib syscalls to eliminate linker warnings
// Bootloader doesn't use file operations, so these just return appropriate values
int _close(int file) { return -1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _read(int file, char *ptr, int len) { return 0; }
int _write(int file, char *ptr, int len) { return len; }

///////////////////////////////////////////////////////////////////////////////
// Bootloader Entry Point
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Main bootloader entry point
 * 
 * Determines which image to boot based on:
 * 1. Commit records in flash (which image is committed)
 * 2. Image validation (CRC checks)
 * 3. Fallback logic if primary image is invalid
 * 
 * Boot sequence:
 * - Initialize bootloader state
 * - Read commit area to determine committed image
 * - Validate images and boot the appropriate one
 * - Handle first boot and recovery scenarios
 */
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
        const uint8_t global_config_data[40] = {0x33, 0x03, 0x28, 0x00, 0xC0, 0x00, 0x00, 0x80, 0x90, 0x05, 0x10, 0x00, 0x64, 0x55, 0x0C, 0x03, 0xA0, 0x0F, 0xE8, 0x03, 0x04, 0x01, 0xEA, 0x01, 0x5A, 0x00, 0x00, 0x00, 0x00, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x32, 0x00};
            (HW_CONFIG_ADDR, global_config_data, sizeof(global_config_data));
        const uint16_t current_lut[1024] = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 256, 260, 264, 268, 272, 276, 280, 284, 288, 292, 296, 300, 304, 308, 312, 316, 320, 324, 328, 332, 336, 340, 344, 348, 352, 356, 360, 364, 368, 372, 376, 380, 384, 388, 392, 396, 400, 404, 408, 412, 416, 420, 424, 428, 432, 436, 440, 444, 448, 452, 456, 460, 464, 468, 472, 476, 480, 484, 488, 492, 496, 500, 504, 508, 512, 516, 520, 524, 528, 532, 536, 540, 544, 548, 552, 556, 560, 564, 568, 572, 576, 580, 584, 588, 592, 596, 600, 604, 608, 612, 616, 620, 624, 628, 632, 636, 640, 644, 648, 652, 656, 660, 664, 668, 672, 676, 680, 684, 688, 692, 696, 700, 704, 708, 712, 716, 720, 724, 728, 732, 736, 740, 744, 748, 752, 756, 760, 764, 768, 772, 776, 780, 784, 788, 792, 796, 800, 804, 808, 812, 816, 820, 824, 828, 832, 836, 840, 844, 848, 852, 856, 860, 864, 868, 872, 876, 880, 884, 888, 892, 896, 900, 904, 908, 912, 916, 920, 924, 928, 932, 936, 940, 944, 948, 952, 956, 960, 964, 968, 972, 976, 980, 984, 988, 992, 996, 1000, 1004, 1008, 1012, 1016, 1020, 1024, 1028, 1032, 1036, 1040, 1044, 1048, 1052, 1056, 1060, 1064, 1068, 1072, 1076, 1080, 1084, 1088, 1092, 1096, 1100, 1104, 1108, 1112, 1116, 1120, 1124, 1128, 1132, 1136, 1140, 1144, 1148, 1152, 1156, 1160, 1164, 1168, 1172, 1176, 1180, 1184, 1188, 1192, 1196, 1200, 1204, 1208, 1212, 1216, 1220, 1224, 1228, 1232, 1236, 1240, 1244, 1248, 1252, 1256, 1260, 1264, 1268, 1272, 1276, 1280, 1284, 1288, 1292, 1296, 1300, 1304, 1308, 1312, 1316, 1320, 1324, 1328, 1332, 1336, 1340, 1344, 1348, 1352, 1356, 1360, 1364, 1368, 1372, 1376, 1380, 1384, 1388, 1392, 1396, 1400, 1404, 1408, 1412, 1416, 1420, 1424, 1428, 1432, 1436, 1440, 1444, 1448, 1452, 1456, 1460, 1464, 1468, 1472, 1476, 1480, 1484, 1488, 1492, 1496, 1500, 1504, 1508, 1512, 1516, 1520, 1524, 1528, 1532, 1536, 1540, 1544, 1548, 1552, 1556, 1560, 1564, 1568, 1572, 1576, 1580, 1584, 1588, 1592, 1596, 1600, 1604, 1608, 1612, 1616, 1620, 1624, 1628, 1632, 1636, 1640, 1644, 1648, 1652, 1656, 1660, 1664, 1668, 1672, 1676, 1680, 1684, 1688, 1692, 1696, 1700, 1704, 1708, 1712, 1716, 1720, 1724, 1728, 1732, 1736, 1740, 1744, 1748, 1752, 1756, 1760, 1764, 1768, 1772, 1776, 1780, 1784, 1788, 1792, 1796, 1800, 1804, 1808, 1812, 1816, 1820, 1824, 1828, 1832, 1836, 1840, 1844, 1848, 1852, 1856, 1860, 1864, 1868, 1872, 1876, 1880, 1884, 1888, 1892, 1896, 1900, 1904, 1908, 1912, 1916, 1920, 1924, 1928, 1932, 1936, 1940, 1944, 1948, 1952, 1956, 1960, 1964, 1968, 1972, 1976, 1980, 1984, 1988, 1992, 1996, 2000, 2004, 2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036, 2040, 2044, 2048, 2052, 2056, 2060, 2064, 2068, 2072, 2076, 2080, 2084, 2088, 2092, 2096, 2100, 2104, 2108, 2112, 2116, 2120, 2124, 2128, 2132, 2136, 2140, 2144, 2148, 2152, 2156, 2160, 2164, 2168, 2172, 2176, 2180, 2184, 2188, 2192, 2196, 2200, 2204, 2208, 2212, 2216, 2220, 2224, 2228, 2232, 2236, 2240, 2244, 2248, 2252, 2256, 2260, 2264, 2268, 2272, 2276, 2280, 2284, 2288, 2292, 2296, 2300, 2304, 2308, 2312, 2316, 2320, 2324, 2328, 2332, 2336, 2340, 2344, 2348, 2352, 2356, 2360, 2364, 2368, 2372, 2376, 2380, 2384, 2388, 2392, 2396, 2400, 2404, 2408, 2412, 2416, 2420, 2424, 2428, 2432, 2436, 2440, 2444, 2448, 2452, 2456, 2460, 2464, 2468, 2472, 2476, 2480, 2484, 2488, 2492, 2496, 2500, 2504, 2508, 2512, 2516, 2520, 2524, 2528, 2532, 2536, 2540, 2544, 2548, 2552, 2556, 2560, 2564, 2568, 2572, 2576, 2580, 2584, 2588, 2592, 2596, 2600, 2604, 2608, 2612, 2616, 2620, 2624, 2628, 2632, 2636, 2640, 2644, 2648, 2652, 2656, 2660, 2664, 2668, 2672, 2676, 2680, 2684, 2688, 2692, 2696, 2700, 2704, 2708, 2712, 2716, 2720, 2724, 2728, 2732, 2736, 2740, 2744, 2748, 2752, 2756, 2760, 2764, 2768, 2772, 2776, 2780, 2784, 2788, 2792, 2796, 2800, 2804, 2808, 2812, 2816, 2820, 2824, 2828, 2832, 2836, 2840, 2844, 2848, 2852, 2856, 2860, 2864, 2868, 2872, 2876, 2880, 2884, 2888, 2892, 2896, 2900, 2904, 2908, 2912, 2916, 2920, 2924, 2928, 2932, 2936, 2940, 2944, 2948, 2952, 2956, 2960, 2964, 2968, 2972, 2976, 2980, 2984, 2988, 2992, 2996, 3000, 3004, 3008, 3012, 3016, 3020, 3024, 3028, 3032, 3036, 3040, 3044, 3048, 3052, 3056, 3060, 3064, 3068, 3072, 3076, 3080, 3084, 3088, 3092, 3096, 3100, 3104, 3108, 3112, 3116, 3120, 3124, 3128, 3132, 3136, 3140, 3144, 3148, 3152, 3156, 3160, 3164, 3168, 3172, 3176, 3180, 3184, 3188, 3192, 3196, 3200, 3204, 3208, 3212, 3216, 3220, 3224, 3228, 3232, 3236, 3240, 3244, 3248, 3252, 3256, 3260, 3264, 3268, 3272, 3276, 3280, 3284, 3288, 3292, 3296, 3300, 3304, 3308, 3312, 3316, 3320, 3324, 3328, 3332, 3336, 3340, 3344, 3348, 3352, 3356, 3360, 3364, 3368, 3372, 3376, 3380, 3384, 3388, 3392, 3396, 3400, 3404, 3408, 3412, 3416, 3420, 3424, 3428, 3432, 3436, 3440, 3444, 3448, 3452, 3456, 3460, 3464, 3468, 3472, 3476, 3480, 3484, 3488, 3492, 3496, 3500, 3504, 3508, 3512, 3516, 3520, 3524, 3528, 3532, 3536, 3540, 3544, 3548, 3552, 3556, 3560, 3564, 3568, 3572, 3576, 3580, 3584, 3588, 3592, 3596, 3600, 3604, 3608, 3612, 3616, 3620, 3624, 3628, 3632, 3636, 3640, 3644, 3648, 3652, 3656, 3660, 3664, 3668, 3672, 3676, 3680, 3684, 3688, 3692, 3696, 3700, 3704, 3708, 3712, 3716, 3720, 3724, 3728, 3732, 3736, 3740, 3744, 3748, 3752, 3756, 3760, 3764, 3768, 3772, 3776, 3780, 3784, 3788, 3792, 3796, 3800, 3804, 3808, 3812, 3816, 3820, 3824, 3828, 3832, 3836, 3840, 3844, 3848, 3852, 3856, 3860, 3864, 3868, 3872, 3876, 3880, 3884, 3888, 3892, 3896, 3900, 3904, 3908, 3912, 3916, 3920, 3924, 3928, 3932, 3936, 3940, 3944, 3948, 3952, 3956, 3960, 3964, 3968, 3972, 3976, 3980, 3984, 3988, 3992, 3996, 4000, 4004, 4008, 4012, 4016, 4020, 4024, 4028, 4032, 4036, 4040, 4044, 4048, 4052, 4056, 4060, 4064, 4068, 4072, 4076, 4080, 4084, 4088, 4092};
        write_bytes(HW_CONFIG_ADDR + (PAGE_SIZE / 2), (void *)current_lut, sizeof(current_lut));

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
