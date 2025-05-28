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

#include "loader.h"
#include "console.h"
#include "stdlib.h"

void printError(int err)
{
    // Translate the error code to a string and output to the console.
    switch(err)
    {
        case LDR_SUCCESS:              consoleStrOut("SUCCESS\n");              break;
        case LDR_IMAGE_NOT_VALID:      consoleStrOut("IMAGE NOT VALID\n");      break;
        case LDR_IMAGE_RUNNING:        consoleStrOut("IMAGE RUNNING\n");        break;
        case LDR_DOWNLOAD_IN_PROGRESS: consoleStrOut("DOWNLOAD IN PROGRESS\n"); break;
        case LDR_DOWNLOAD_NOT_STARTED: consoleStrOut("DOWNLOAD NOT STARTED\n"); break;
        case LDR_FLASH_ERROR:          consoleStrOut("FLASH ERROR\n");          break;
        case LDR_UNKNOWN_ERROR:        
        default:                       consoleStrOut("UNKNOWN ERROR\n");        break;
    }
}

void startDownload()
{
    int res = LDR_StartDownload();

    // Report the result.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Success. Ready to accept a new image.\n");
    }
    else
    {
        consoleStrOut("Failed to start download: ");
        printError(res);
    }
}

void downloadImageBlock()
{
    uint32_t offset = 0;
    uint32_t length = 0;
    int res;
    uint8_t* buff;

    // Ask the user for the 'offset' value (in hex).
    consoleStrOut("Ready to receive block.\nEnter offset (hex): ");
    if(!consoleReadHex(&offset))
    {
        consoleStrOut("\n\nInvalid input.\n");
        return;
    }

    // Ask the user for the 'length' of the block.
    consoleStrOut("\nEnter length: ");
    if(!consoleReadVal(&length))
    {
        consoleStrOut("\n\nInvalid input.\n");
        return;
    }

    // Allocate some space to store the block temporarily.
    buff = (uint8_t*)malloc(length);
    if(!buff)
    {
        consoleStrOut("\n\nMemory allocation error.\n");
        return;
    }

    // Prompt the user to send the block.
    consoleStrOut("\nSend block: ");

    // Read the block.
    consoleRead(buff, length, 0);

    // Send the block to the loader.
    res = LDR_DownloadImageBlock(offset, buff, length);

    // Repor the result
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("\n\nSuccess. Block received.\n");
    }
    else
    {
        consoleStrOut("\n\nFailed to download block: ");
        printError(res);
    }

    // Clean up the temp memory.
    free(buff);
}

void abortUpgrade()
{
    int res = LDR_AbortUpgrade();

    // Report the result.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Success. Upgrade aborted.\n");
    }
    else
    {
        consoleStrOut("Failed to abort upgrade: ");
        printError(res);
    }
}

void completeDownload()
{
    int res = LDR_CompleteDownload();

    // Report the result.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Success. Download complete.\n");
    }
    else
    {
        consoleStrOut("Failed to complete download: ");
        printError(res);
    }
}

void runImageA()
{
    // Give the new app a clean line to start on.
    consoleStrOut("\n");
    consoleFlush();

    int res = LDR_RunImageA();
    // If the LDR_RunImageA function returns, there was a problem.  When
    //  successful, the new application should take control of the system
    //  instead of returning.
    consoleStrOut("Failed to run image A: ");
    printError(res);
}

void runImageB()
{
    // Give the new app a clean line to start on.
    consoleStrOut("\n");
    consoleFlush();
    int res = LDR_RunImageB();
    // If the LDR_RunImageB function returns, there was a problem.  When
    //  successful, the new application should take control of the system
    //  instead of returning.
    consoleStrOut("Failed to run image B: ");
    printError(res);
}

void commitImageA()
{
    int res = LDR_CommitImageA();

    // Report the result.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Success. Image A will run automatically on any reset.\n");
    }
    else
    {
        consoleStrOut("Failed to commit image A: ");
        printError(res);
    }
}

void commitImageB()
{
    int res = LDR_CommitImageB();

    // Report the result.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Success. Image B will run automatically on any reset.\n");
    }
    else
    {
        consoleStrOut("Failed to commit image B: ");
        printError(res);
    }
}

void getImageStatus()
{
    ldr_image_status_t status;
    int res = LDR_GetImageStatus(&status);

    // Report the results.
    if(res == LDR_SUCCESS)
    {
        consoleStrOut("Active Image:    ");
        if(status.active_commit_status & 0x80)
        {
            consoleStrOut("B\n");
        }
        else
        {
            consoleStrOut("A\n");
        }

        consoleStrOut("Committed Image: ");
        if(status.active_commit_status & 0x40)
        {
            consoleStrOut("B\n");
        }
        else
        {
            consoleStrOut("A\n");
        }

        consoleStrOut("Image A Status:  ");
        switch(status.image_status & 0x30)
        {
            case 0x00: consoleStrOut("No image\n"); break;
            case 0x10: consoleStrOut("Valid\n");    break;
            case 0x20: consoleStrOut("Invalid\n");  break;
            case 0x30: consoleStrOut("Unknown\n");  break;
        }

        consoleStrOut("Image B Status:  ");
        switch(status.image_status & 0x03)
        {
            case 0x00: consoleStrOut("No image\n"); break;
            case 0x01: consoleStrOut("Valid\n");    break;
            case 0x02: consoleStrOut("Invalid\n");  break;
            case 0x03: consoleStrOut("Unknown\n");  break;
        }
    }
    else
    {
        consoleStrOut("Failed to get image status: ");
        printError(res);
    }
}

void getVersion()
{
    // Output version as "Major.Minor"
    uint32_t ver = LDR_GetVersion();
    consoleStrOut("Loader version = ");
    // Bits 8-15 hold the major version number.
    consoleDecOut((ver & 0xFF00) >> 8);
    consoleStrOut(".");
    // Bits 0-7 hold the minor version number.
    consoleDecOut(ver & 0xFF);
    consoleStrOut("\n");
}

char* months[13] = {"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
void getBuildDate()
{
    // Output date as "DD Mmm YYYY"
    uint32_t date = LDR_GetBuildDate();
    consoleStrOut("Loader date = ");
    // Bits 0-7 hold the day of the month.
    consoleDecOut(date & 0xFF);
    consoleStrOut(" ");
    // Bits 7-15 hold the month.
    consoleStrOut(months[(date & 0xFF00) >> 8]);
    consoleStrOut(" ");
    // Bits 16-23 hold the year
    consoleDecOut(2000 + ((date & 0xFF0000) >> 16));
    consoleStrOut("\n");
}

void receiveImage()
{
    int offset = 0;
    uint8_t buff[16];
    int res;

    // Place the loader in download mode.
    res = LDR_StartDownload();
    if(res != LDR_SUCCESS)
    {
        consoleStrOut("\n\nFailed to start download: ");
        printError(res);
        return;
    }

    // Prompt the user to send the file.
    consoleStrOut("Send image file: ");

    // Image files will always be 212992 bytes long.
    while(offset < 212992)
    {
        // Read 1 word (128 bits) worth of data.
        consoleRead(buff, 16, 0);

        // Send it to the loader.
        res = LDR_DownloadImageBlock(offset, buff, 16);
        if(res != LDR_SUCCESS)
        {
            consoleStrOut("\n\nFailed to download block: ");
            printError(res);
            LDR_AbortUpgrade();
            return;
        }

        offset += 16;
    }

    // Tell the loader the entire image has been sent.
    res = LDR_CompleteDownload();
    if(res != LDR_SUCCESS)
    {
        consoleStrOut("\n\nFailed to complete download: ");
        printError(res);
    }

    consoleStrOut("\n\nDownload complete.\n");
}

void uartLoader()
{
    uint32_t input = 0;

    while(1)
    {
        // Output the menu.
        consoleStrOut("\n");
        consoleStrOut("Low-level commands:\n");
        consoleStrOut("     1. Start Download\n");
        consoleStrOut("     2. Download Image Block\n");
        consoleStrOut("     3. Abort Upgrade\n");
        consoleStrOut("     4. Complete Download\n");
        consoleStrOut("     5. Run Image A\n");
        consoleStrOut("     6. Run Image B\n");
        consoleStrOut("     7. Commit Image A\n");
        consoleStrOut("     8. Commit Image B\n");
        consoleStrOut("     9. Get Image Status\n");
        consoleStrOut("    10. Get Loader Version\n");
        consoleStrOut("    11. Get Loader Build Date\n");
        consoleStrOut("\n");
        consoleStrOut("High-level commands:\n");
        consoleStrOut("    12. Perform Update (automates commands 1, 2, and 4)\n");
        consoleStrOut("    13. Exit Loader Mode\n");
        consoleStrOut("\n");
        consoleStrOut("Select a loader operation:  ");

        // Read the command given.
        if(!consoleReadVal(&input))
        {
            consoleStrOut("\n\nInvalid input.\n");
            continue;
        }
        consoleStrOut("\n\n");

        // Perform the command.
        switch(input)
        {
            case  1: startDownload();      break;  
            case  2: downloadImageBlock(); break;
            case  3: abortUpgrade();       break;
            case  4: completeDownload();   break;  
            case  5: runImageA();          break;
            case  6: runImageB();          break;
            case  7: commitImageA();       break;
            case  8: commitImageB();       break;
            case  9: getImageStatus();     break;
            case 10: getVersion();         break;
            case 11: getBuildDate();       break;
            case 12: receiveImage();       break;
            case 13: return;
        }
    }
}
