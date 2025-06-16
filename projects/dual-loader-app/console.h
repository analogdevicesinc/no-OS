#include "uart.h"

/* Perform any initialization necessary for console input/output. */
void console_prepare(mxc_uart_regs_t* uart);

/* Output the given string to the console. */
void console_str_out(char* str);

/* Output the given value (in base 10) to the console. */
void console_dec_out(int val);

/* Read a value (base 10) from the console.  This function will read from the
   console until a new line character is received or 15 digits have been 
   entered.  This function returns 1 if successful, otherwise it returns 0. */
int console_read_val(uint32_t* val);

/* Read a value (base 16) from the console.  This function will read from the
   console until a new line character is received or 15 characters have been 
   entered.  This function returns 1 if successful, otherwise it returns 0. */
int console_read_hex(uint32_t* val);

/* Reads 'len' number of bytes from the console and places them into 'buff'.  
   Set 'doEcho' to a non-zero value to echo back each byte as it is received.
   This function will block until all bytes are received. */
void console_read(uint8_t* buff, uint32_t len, int doEcho);

/* Determines if there are any characters that have been entered in the console
   but not yet consumed.  Returns non-zero if characters are available. */
int console_char_available(void);

/* Makes sure all characters that have been sent to the console have been
   output. */
void console_flush(void);
