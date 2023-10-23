#ifndef _USE_MACRO_AND_CONST_H_
#define _USE_MACRO_AND_CONST_H_

#include <stdio.h>	
#include <stdint.h>	
#include <stdbool.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define BIT(__N)         ((uint8_t)(1 << __N)) 

#define enable_irq()   __enable_irq();
#define disable_irq()  __disable_irq();

//! \brief The safe ATOM code section macro
#define SAFE_ATOM_CODE(...) {\
        disable_irq();\
        __VA_ARGS__;\
        enable_irq();\
    }

typedef struct {
    unsigned Bit0:1;
    unsigned Bit1:1;
    unsigned Bit2:1;
    unsigned Bit3:1;
    unsigned Bit4:1;
    unsigned Bit5:1;
    unsigned Bit6:1;
    unsigned Bit7:1;
}byte_bit_t;

typedef union{
	byte_bit_t tBit;
	uint8_t chByte;
}bit_operate_t;


#endif

/*-------------------------------END OF FILE-------------------------------*/

