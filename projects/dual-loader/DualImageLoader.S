/*******************************************************************************
 * Loader Function Table
 * 
 * This file contains only the loader function table that needs to be placed
 * at a specific address in flash. The rest of the startup code comes from CMSIS.
 ******************************************************************************/

    .syntax unified
    .arch armv7-m
    
    .thumb
    .section .loader_function_table, "a", %progbits
    .align 4
Loader_Function_Table:
    b.w (dl_start_download + 1)
    b.w (dl_download_image_block + 1)
    b.w (dl_abort_upgrade + 1)
    b.w (dl_complete_download + 1)
    b.w (dl_run_image_a + 1)
    b.w (dl_run_image_b + 1)
    b.w (dl_commit_image_a + 1)
    b.w (dl_commit_image_b + 1)
    b.w (dl_get_image_status + 1)
    b.w (dl_get_version + 1)
    b.w (dl_get_build_date + 1)
    b.w (dl_flash_write + 1)
    b.w (dl_flash_read + 1)

    .end
