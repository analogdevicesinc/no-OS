#include "uart_maxim_pdriver.h"

struct maxim_uart_desc{
	uint32_t mode;	// rx or tx
	uart_cfg_t *maxim_desc;
	uart_req_t *request_descriptor;
};

struct maxim_uart_init_param {
	uint32_t mode;
	uart_parity_t parity;
	uart_size_t size;
	uart_stop_t stop;
	uart_flow_ctrl_t flow;	
	uart_flow_pol_t pol;
};
