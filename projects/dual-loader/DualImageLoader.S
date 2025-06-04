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
    b.w (LDR_StartDownload + 1)
    b.w (LDR_DownloadImageBlock + 1)
    b.w (LDR_AbortUpgrade + 1)
    b.w (LDR_CompleteDownload + 1)
    b.w (LDR_RunImageA + 1)
    b.w (LDR_RunImageB + 1)
    b.w (LDR_CommitImageA + 1)
    b.w (LDR_CommitImageB + 1)
    b.w (LDR_GetImageStatus + 1)
    b.w (LDR_GetVersion + 1)
    b.w (LDR_GetBuildDate + 1)

    .end
