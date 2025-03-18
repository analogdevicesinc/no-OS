#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__ 

#include "no_os_delay.h"
#include "no_os_alloc.h" 
#include "no_os_error.h"
#include "no_os_gpio.h"			
#include "no_os_print_log.h" 
#include "no_os_irq.h "          
#include "no_os_list.h "      	
#include "no_os_uart.h"    
#include "no_os_lf256fifo.h"  	
#include "no_os_util.h "      	
#include "no_os_units.h"	  	
#include "no_os_mutex.h"	  	
#include "stm32_hal.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h" 


#define UART_DEVICE_ID                  3
#define UART_BAUDRATE                   1000000
#define UART_OPS                        &stm32_uart_ops
extern UART_HandleTypeDef               huart3;
#define UART_INSTANCE                   (&huart3)

extern struct stm32_uart_init_param stm32_uart_extra_ip;
extern struct no_os_uart_init_param uart_ip;


#endif /* __PARAMETERS_H_ */#define UART_OPS        &stm32_uart_ops