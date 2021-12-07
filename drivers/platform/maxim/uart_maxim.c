#include "mxc_sys.h"
#include <stdio.h>
#include <stdlib.h>
#include "mxc_sys.h"
#include <errno.h>
//#include "irq_x.h"
#include "uart_maxim.h"
#include "uart.h"

#define BUFFER_SIZE 32000

static void cb(uart_req_t *req, int error){
	printf("done\n");   
	fflush(stdout); 
}


/**
 * @brief Read data from UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number)
{
	if(!desc || !data || !bytes_number)
  		return -EINVAL;
	
	uint32_t err = 0;
	uint32_t bytes_read = 0;
	
	int32_t len = (bytes_number < BUFFER_SIZE) ? bytes_number : BUFFER_SIZE;
	err = UART_Read(MXC_UART_GET_UART(desc->device_id), data, len, &bytes_read);

	if(err < 0)
		return -EIO;

	return bytes_read;
}

/**
 * @brief Write data to UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,	uint32_t bytes_number)
{
	if(!desc || !data || !bytes_number)
		return -EINVAL;
	
	uint32_t err = 0;
 
	int32_t len = (bytes_number < BUFFER_SIZE) ? bytes_number : BUFFER_SIZE;
	err = UART_Write(MXC_UART_GET_UART(desc->device_id), data, len);
	
	if(err < 0)
		return -EIO;

	return 0;
}

/**
 * @brief Read data from UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t uart_read_nonblocking(struct uart_desc *desc, uint8_t *data,  uint32_t bytes_number)
{
	if(!desc || !data || !bytes_number)
		return -EINVAL;

	uint32_t err = 0;
	uart_req_t read_req; 
	
	read_req.data	   = data;
	read_req.len	= (bytes_number < BUFFER_SIZE) ? bytes_number : BUFFER_SIZE;
	read_req.callback   = cb;
	err = UART_ReadAsync(MXC_UART_GET_UART(desc->device_id), &read_req);	

	return err;
}

/**
 * @brief Write data to UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */

int32_t uart_write_nonblocking(struct uart_desc *desc, const uint8_t *data, uint32_t bytes_number){
	if(!desc || !data || !bytes_number)
		return -EINVAL;

	uint32_t err = 0;
	uart_req_t write_req; 

	write_req.data	   = data;
	write_req.len		= (bytes_number < BUFFER_SIZE) ? bytes_number : BUFFER_SIZE;
	write_req.callback   = cb;
	err = UART_WriteAsync(MXC_UART_GET_UART(desc->device_id), &write_req);

	return err;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return SUCCESS in case of success, error code otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param)
{
	if(!param || !desc)
		return -EINVAL;
	
	int ret = 0;
	uint32_t did = param->device_id;
 
	struct maxim_uart_desc *maxim_uart = calloc(1, sizeof(struct maxim_uart_desc));
	struct uart_desc *descriptor = calloc(1, sizeof(struct uart_desc));
	//uart_cfg_t *maxim_desc = calloc(1, sizeof(uart_cfg_t));
	//sys_cfg_uart_t *maxim_desc_sys = calloc(1, sizeof(sys_cfg_uart_t));
	//uart_req_t *request_desc = calloc(1, sizeof(uart_req_t));	
	uart_cfg_t maxim_desc;
	sys_cfg_uart_t maxim_desc_sys;

	if(!maxim_uart || !descriptor){
		ret = -ENOMEM;   
		goto error;
	}

	NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(did));
	NVIC_DisableIRQ(MXC_UART_GET_IRQ(did));
	NVIC_SetPriority(MXC_UART_GET_IRQ(did), 1);
	NVIC_EnableIRQ(MXC_UART_GET_IRQ(did));

	//maxim_desc = (uart_cfg_t *)param->extra;
	struct maxim_uart_init_param *extra_param = (struct maxim_uart_init_param *)param->extra;
	maxim_desc.parity = extra_param->parity;
	maxim_desc.size = 768;
	maxim_desc.stop = extra_param->stop;
	maxim_desc.flow = extra_param->flow;
	maxim_desc.pol = extra_param->pol;
	maxim_desc.baud = param->baud_rate;

	//maxim_uart->maxim_desc = maxim_desc;
	//maxim_uart->request_descriptor = request_desc;
	//maxim_uart->mode = extra_param->mode;

	maxim_desc_sys.map = MAP_A;
	maxim_desc_sys.flow_flag = UART_FLOW_DISABLE;

	descriptor->extra = maxim_uart;
	descriptor->baud_rate = param->baud_rate;
	descriptor->device_id = param->device_id;

	uint32_t err = UART_Init(MXC_UART_GET_UART(did), &maxim_desc, &maxim_desc_sys);
	if(err != E_NO_ERROR){
		ret = -EIO;
		goto error;
	}
	
	*desc = descriptor;
	return 0;
error:
	free(descriptor);
	//free(maxim_desc);
	//free((sys_cfg_uart_t *)maxim_desc_sys);	
	//free(request_desc);
	free(maxim_uart);

	return ret;
}

/**
 * @brief Free the resources allocated by uart_init().
 * @param desc - The UART descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_remove(struct uart_desc *desc)
{
	if(!desc)
		return -EINVAL;

	struct maxim_uart_desc *maxim_uart = (struct maxim_uart_desc *)desc->extra;
	free(maxim_uart->maxim_desc);
	free(maxim_uart->request_descriptor);
	free(desc->extra);
	free(desc->callback);
	free(desc);

	return 0;
}

/* Check if UART errors occurred. */
uint32_t uart_get_errors(struct uart_desc *desc)
{

	return 0;
}


