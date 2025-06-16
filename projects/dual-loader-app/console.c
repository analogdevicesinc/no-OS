#include "console.h"
#include "string.h"
#include "stdio.h"
#include "uart.h"

mxc_uart_regs_t* console_uart = MXC_UART0;

void console_prepare(mxc_uart_regs_t* uart)
{
    // Save the UART to use
    console_uart = uart;

    MXC_UART_Init(uart, 921600, MXC_UART_APB_CLK);
}

void console_str_out(char* str)
{
    // Get the string's length.
    int len = strlen(str);

    // Send each byte in str to the console.
    for(int i = 0; i < len; i++)
    {
        // Line feed characters need to be prefixed with a carriage return.
        if(str[i] == '\n')
        {
            MXC_UART_WriteCharacter(console_uart, '\r');
        }
        MXC_UART_WriteCharacter(console_uart, str[i]);
    }
}

void console_dec_out(int val)
{
    // Format the value as a decimal (base 10) string and output it.
    char str[11];
    sprintf(str, "%d", val);
    console_str_out(str);
}

int console_read_val(uint32_t* val)
{
    char str[16];
    int off = 0;

    do
    {
        // Retreive a byte and echo it.
        str[off] = MXC_UART_ReadCharacter(console_uart);
        MXC_UART_WriteCharacter(console_uart, str[off]);

        // Hande backspace characters specially.
        if(str[off] == 0x08)
        {
            // Make sure to not go back too far.
            if(off)
            {
                off--;
            }
        }
        else
        {
            off++;
        }
    } while((str[off - 1] != '\n') && (str[off -1] != '\r') && (off < 15));

    // Add a NULL character at the end of the string
    str[off] = 0;

    // Parse the value as a base 10 string.
    if(sscanf(str, "%d", val) != 1)
    {
        return 0;
    }

    return 1;
}

int console_read_hex(uint32_t* val)
{
    char str[16];
    int off = 0;

    do
    {
        // Retreive a byte and echo it.
        str[off] = MXC_UART_ReadCharacter(console_uart);
        MXC_UART_WriteCharacter(console_uart, str[off]);

        // Hande backspace characters specially.
        if(str[off] == 0x08)
        {
            // Make sure to not go back too far.
            if(off)
            {
                off--;
            }
        }
        else
        {
            off++;
        }
    } while((str[off - 1] != '\n') && (str[off -1] != '\r') && (off < 15));

    // Add a NULL character at the end of the string
    str[off] = 0;

    // Parse the value as a hexadecimal string.
    if(sscanf(str, "%x", val) != 1)
    {
        return 0;
    }

    return 1;
}

void console_read(uint8_t* buff, uint32_t len, int doEcho)
{
    // Read 'len' bytes.
    while(len)
    {
        // Get a byte.
        buff[0] = MXC_UART_ReadCharacter(console_uart);

        // Echo it if required.
        if(doEcho)
        {
            MXC_UART_WriteCharacter(console_uart, buff[0]);
        }

        buff++;
        len--;
    }
}

int console_char_available()
{
    // Check if the receive FIFO is empty.
    return (console_uart->status & MXC_F_UART_STATUS_RX_EM) == 0;
}

void console_flush()
{
    // Wait until the TX FIFO is empty.
    while((console_uart->status & MXC_F_UART_STATUS_TX_EM) == 0);
}