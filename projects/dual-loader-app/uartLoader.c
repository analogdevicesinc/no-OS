#include "loader.h"
#include "console.h"
#include "stdlib.h"

void print_error(int err)
{
    // Translate the error code to a string and output to the console.
    switch(err)
    {
        case DL_SUCCESS:              console_str_out("SUCCESS\n");              break;
        case DL_IMAGE_NOT_VALID:      console_str_out("IMAGE NOT VALID\n");      break;
        case DL_IMAGE_RUNNING:        console_str_out("IMAGE RUNNING\n");        break;
        case DL_DOWNLOAD_IN_PROGRESS: console_str_out("DOWNLOAD IN PROGRESS\n"); break;
        case DL_DOWNLOAD_NOT_STARTED: console_str_out("DOWNLOAD NOT STARTED\n"); break;
        case DL_FLASH_ERROR:          console_str_out("FLASH ERROR\n");          break;
        case DL_UNKNOWN_ERROR:        
        default:                       console_str_out("UNKNOWN ERROR\n");        break;
    }
}

void start_download()
{
    int res = dl_start_download();

    // Report the result.
    if(res == DL_SUCCESS)
    {
        console_str_out("Success. Ready to accept a new image.\n");
    }
    else
    {
        console_str_out("Failed to start download: ");
        print_error(res);
    }
}

void download_image_block()
{
    uint32_t offset = 0;
    uint32_t length = 0;
    int res;
    uint8_t* buff;

    // Ask the user for the 'offset' value (in hex).
    console_str_out("Ready to receive block.\nEnter offset (hex): ");
    if(!console_read_hex(&offset))
    {
        console_str_out("\n\nInvalid input.\n");
        return;
    }

    // Ask the user for the 'length' of the block.
    console_str_out("\nEnter length: ");
    if(!console_read_val(&length))
    {
        console_str_out("\n\nInvalid input.\n");
        return;
    }

    // Allocate some space to store the block temporarily.
    buff = (uint8_t*)malloc(length);
    if(!buff)
    {
        console_str_out("\n\nMemory allocation error.\n");
        return;
    }

    // Prompt the user to send the block.
    console_str_out("\nSend block: ");

    // Read the block.
    console_read(buff, length, 0);

    // Send the block to the loader.
    res = dl_download_image_block(offset, buff, length);

    // Repor the result
    if(res == DL_SUCCESS)
    {
        console_str_out("\n\nSuccess. Block received.\n");
    }
    else
    {
        console_str_out("\n\nFailed to download block: ");
        print_error(res);
    }

    // Clean up the temp memory.
    free(buff);
}

void abort_upgrade()
{
    int res = dl_abort_upgrade();

    // Report the result.
    if(res == DL_SUCCESS)
    {
        console_str_out("Success. Upgrade aborted.\n");
    }
    else
    {
        console_str_out("Failed to abort upgrade: ");
        print_error(res);
    }
}

void complete_download()
{
    int res = dl_complete_download();

    // Report the result.
    if(res == DL_SUCCESS)
    {
        console_str_out("Success. Download complete.\n");
    }
    else
    {
        console_str_out("Failed to complete download: ");
        print_error(res);
    }
}

void run_image_a()
{
    // Give the new app a clean line to start on.
    console_str_out("\n");
    console_flush();

    int res = dl_run_image_a();
    // If the dl_run_image_a function returns, there was a problem.  When
    //  successful, the new application should take control of the system
    //  instead of returning.
    console_str_out("Failed to run image A: ");
    print_error(res);
}

void run_image_b()
{
    // Give the new app a clean line to start on.
    console_str_out("\n");
    console_flush();
    int res = dl_run_image_b();
    // If the dl_run_image_b function returns, there was a problem.  When
    //  successful, the new application should take control of the system
    //  instead of returning.
    console_str_out("Failed to run image B: ");
    print_error(res);
}

void commit_image_a()
{
    int res = dl_commit_image_a();

    // Report the result.
    if(res == DL_SUCCESS)
    {
        console_str_out("Success. Image A will run automatically on any reset.\n");
    }
    else
    {
        console_str_out("Failed to commit image A: ");
        print_error(res);
    }
}

void commit_image_b()
{
    int res = dl_commit_image_b();

    // Report the result.
    if(res == DL_SUCCESS)
    {
        console_str_out("Success. Image B will run automatically on any reset.\n");
    }
    else
    {
        console_str_out("Failed to commit image B: ");
        print_error(res);
    }
}

void get_image_status()
{
    dl_image_status_t status;
    int res = dl_get_image_status(&status);

    // Report the results.
    if(res == DL_SUCCESS)
    {
        console_str_out("Active Image:    ");
        if(status.active_commit_status & 0x80)
        {
            console_str_out("B\n");
        }
        else
        {
            console_str_out("A\n");
        }

        console_str_out("Committed Image: ");
        if(status.active_commit_status & 0x40)
        {
            console_str_out("B\n");
        }
        else
        {
            console_str_out("A\n");
        }

        console_str_out("Image A Status:  ");
        switch(status.image_status & 0x30)
        {
            case 0x00: console_str_out("No image\n"); break;
            case 0x10: console_str_out("Valid\n");    break;
            case 0x20: console_str_out("Invalid\n");  break;
            case 0x30: console_str_out("Unknown\n");  break;
        }

        console_str_out("Image B Status:  ");
        switch(status.image_status & 0x03)
        {
            case 0x00: console_str_out("No image\n"); break;
            case 0x01: console_str_out("Valid\n");    break;
            case 0x02: console_str_out("Invalid\n");  break;
            case 0x03: console_str_out("Unknown\n");  break;
        }
    }
    else
    {
        console_str_out("Failed to get image status: ");
        print_error(res);
    }
}

void get_version()
{
    // Output version as "Major.Minor"
    uint32_t ver = dl_get_version();
    console_str_out("Loader version = ");
    // Bits 8-15 hold the major version number.
    console_dec_out((ver & 0xFF00) >> 8);
    console_str_out(".");
    // Bits 0-7 hold the minor version number.
    console_dec_out(ver & 0xFF);
    console_str_out("\n");
}

char* months[13] = {"???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
void get_build_date()
{
    // Output date as "DD Mmm YYYY"
    uint32_t date = dl_get_build_date();
    console_str_out("Loader date = ");
    // Bits 0-7 hold the day of the month.
    console_dec_out(date & 0xFF);
    console_str_out(" ");
    // Bits 7-15 hold the month.
    console_str_out(months[(date & 0xFF00) >> 8]);
    console_str_out(" ");
    // Bits 16-23 hold the year
    console_dec_out(2000 + ((date & 0xFF0000) >> 16));
    console_str_out("\n");
}

void receive_image()
{
    int offset = 0;
    uint8_t buff[16];
    int res;

    // Place the loader in download mode.
    res = dl_start_download();
    if(res != DL_SUCCESS)
    {
        console_str_out("\n\nFailed to start download: ");
        print_error(res);
        return;
    }

    // Prompt the user to send the file.
    console_str_out("Send image file: ");

    // Image files will always be 212992 bytes long.
    while(offset < 212992)
    {
        // Read 1 word (128 bits) worth of data.
        console_read(buff, 16, 0);

        // Send it to the loader.
        res = dl_download_image_block(offset, buff, 16);
        if(res != DL_SUCCESS)
        {
            console_str_out("\n\nFailed to download block: ");
            print_error(res);
            dl_abort_upgrade();
            return;
        }

        offset += 16;
    }

    // Tell the loader the entire image has been sent.
    res = dl_complete_download();
    if(res != DL_SUCCESS)
    {
        console_str_out("\n\nFailed to complete download: ");
        print_error(res);
    }

    console_str_out("\n\nDownload complete.\n");
}

void uart_loader()
{
    uint32_t input = 0;

    while(1)
    {
        // Output the menu.
        console_str_out("\n");
        console_str_out("Low-level commands:\n");
        console_str_out("     1. Start Download\n");
        console_str_out("     2. Download Image Block\n");
        console_str_out("     3. Abort Upgrade\n");
        console_str_out("     4. Complete Download\n");
        console_str_out("     5. Run Image A\n");
        console_str_out("     6. Run Image B\n");
        console_str_out("     7. Commit Image A\n");
        console_str_out("     8. Commit Image B\n");
        console_str_out("     9. Get Image Status\n");
        console_str_out("    10. Get Loader Version\n");
        console_str_out("    11. Get Loader Build Date\n");
        console_str_out("\n");
        console_str_out("High-level commands:\n");
        console_str_out("    12. Perform Update (automates commands 1, 2, and 4)\n");
        console_str_out("    13. Exit Loader Mode\n");
        console_str_out("\n");
        console_str_out("Select a loader operation:  ");

        // Read the command given.
        if(!console_read_val(&input))
        {
            console_str_out("\n\nInvalid input.\n");
            continue;
        }
        console_str_out("\n\n");

        // Perform the command.
        switch(input)
        {
            case  1: start_download();      break;  
            case  2: download_image_block(); break;
            case  3: abort_upgrade();       break;
            case  4: complete_download();   break;  
            case  5: run_image_a();          break;
            case  6: run_image_b();          break;
            case  7: commit_image_a();       break;
            case  8: commit_image_b();       break;
            case  9: get_image_status();     break;
            case 10: get_version();         break;
            case 11: get_build_date();       break;
            case 12: receive_image();       break;
            case 13: return;
        }
    }
}
