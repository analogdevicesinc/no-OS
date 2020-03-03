#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "at_driver.h"
#include "error.h"
#include "delay.h"
#include "uart.h"
#include "irq.h"
#include "util.h"
//#include "list.h"

/* Used to remove warnings */
#define USTR(X)			((uint8_t *)(X))
/* 100 miliseconds. Wait for a driver response when sending a cmd*/
#define MODULE_TIMEOUT		20000

struct cmd_desc {
	struct at_buff	cmd;
	uint32_t	type;
};

static const struct cmd_desc g_map[] = {
/*00*/	{{(uint8_t *)"", 0}, AT_EXECUTE_OP},
/*01*/	{{(uint8_t *)"+RST", 4}, AT_EXECUTE_OP},
/*02*/	{{(uint8_t *)"+GSLP", 5}, AT_SET_OP},
/*03*/	{{(uint8_t *)"+GMR", 4}, AT_EXECUTE_OP},
/*04*/	{{(uint8_t *)"+CWMODE", 7}, AT_QUERY_OP | AT_SET_OP | AT_TEST_OP},
/*05*/	{{(uint8_t *)"+CWJAP", 6}, AT_QUERY_OP | AT_SET_OP},
/*06*/	{{(uint8_t *)"+CWLAP", 6}, AT_EXECUTE_OP},
/*07*/	{{(uint8_t *)"+CWQAP", 6}, AT_EXECUTE_OP},
/*08*/	{{(uint8_t *)"+CWSAP", 6}, AT_QUERY_OP | AT_SET_OP},
/*09*/	{{(uint8_t *)"+CWLIF", 6}, AT_EXECUTE_OP},
/*10*/	{{(uint8_t *)"+CIPSTATUS", 10}, AT_EXECUTE_OP},
/*11*/	{{(uint8_t *)"+CIPSTART", 9}, AT_TEST_OP | AT_SET_OP},
/*12*/	{{(uint8_t *)"+CIPSEND", 8}, AT_SET_OP},
/*13*/	{{(uint8_t *)"+CIPCLOSE", 9}, AT_EXECUTE_OP | AT_SET_OP},
/*14*/	{{(uint8_t *)"+CIFSR", 6}, AT_EXECUTE_OP},
/*15*/	{{(uint8_t *)"+CIPMUX", 7}, AT_QUERY_OP | AT_SET_OP},
/*16*/	{{(uint8_t *)"+CIPSERVER", 10}, AT_SET_OP},
/*17*/	{{(uint8_t *)"+CIPMODE", 8}, AT_QUERY_OP | AT_SET_OP},
/*18*/	{{(uint8_t *)"+CIPSTO", 7}, AT_QUERY_OP | AT_SET_OP},
/*19*/	{{(uint8_t *)"+PING", 5}, AT_SET_OP}
};

static void at_sprintf(struct at_buff *dest, uint8_t *fmt, ...);
static void at_sscanf(struct at_buff *src, uint8_t *fmt, ...);
static inline void at_strcpy(struct at_buff *dest, const struct at_buff *src);
static inline void at_strcat(struct at_buff *dest, const struct at_buff *src);
static inline void at_strcatch(struct at_buff *dest, uint8_t ch);
static inline void at_set(struct at_buff *dest, uint8_t *buff, uint32_t len);

/*
 * Get id when running on multiple connection mode.
 * Response: 2,CLOSED -> id = 0
 */
static inline void  get_conn_id(struct at_desc *desc, uint32_t *id)
{
	int32_t i;
	uint8_t	ch;

	*id = -1;
	/* Result.len will be pointing to the C (2,CLOSED) in the result */
	i = desc->result.len - 2;
	if (i < 0)
		return ;

	ch = desc->result.buff[i];
	if (ch < '0' || ch > '9')
		return ;

	desc->result.len -= 2;
	*id = ch - '0';
}

/*
 * For each read character from UART, check with the default responses and
 * update the responses indexes (desc->match_idx)
 */
static inline void refresh_status(struct at_desc *desc)
{
	const static struct at_buff responses[NB_RESPONSES] = {
					{USTR("\r\nERROR\r\n"), 9},
					{USTR("\r\nOK\r\n"), 6},
					{USTR("\r\nSEND OK\r\n"), 11},
					{USTR("CLOSED\r\n"), 8}};
	uint8_t		last_char = desc->result.buff[desc->result.len - 1];
	uint32_t	i;
	uint32_t	id;
	bool		match;

	match = false;
	for (i = 0; i < NB_RESPONSES && !match; i++) {
		if (desc->match_idx[i] > responses[i].len) {
			printf("Driver implementation error \n");
			return ;
		}
		/*
		 * Check if the last read char match the current index on the
		 * response.
		 */
		if (last_char == responses[i].buff[desc->match_idx[i]]) {
			/* Increment matching index if there is a match */
			desc->match_idx[i]++;
			if (desc->match_idx[i] == responses[i].len) {
				/* One response match */
				desc->result.len -= desc->match_idx[i];
				switch (i) {
				case 0: // \r\nERROR\r\n
					desc->result_status = RESULT_ERROR;
					match = true;
					break;
				case 1: // \r\nOK\r\n
				case 2: // \r\nSEND OK\r\n
					desc->result_status = RESULT_AVAILABE;
					match = true;
					break;
				case 3: // CLOSED\r\n
					id = 0;
					if (desc->multiple_conections)
						get_conn_id(desc, &id);
					if (id > 4) //Maximum of 4 connections
						printf("BUG\n");
					desc->connections[id].active = false;
					match = true;
					break;
				default:
					break;
				}
				/* Clear response indexes */
				memset(desc->match_idx, 0,
						sizeof(desc->match_idx));
			}
		}
		else if (last_char == responses[i].buff[0]){
			/* If the first ch is match, set de matching idx to 1 */
			desc->match_idx[i] = 1;
		}
		else {
			desc->match_idx[i] = 0;
		}
	}
}

/*
 * Wait the response for the last command with a timeout of \ref MODULE_TIMEOUT
 * milliseconds.
 */
static inline void wait_for_response(struct at_desc *desc)
{
	uint32_t counter;

	timer_start(desc->timer);
	do {
		if (desc->result_status != WAITING_RESULT)
			break;
		timer_counter_get(desc->timer, &counter);
	} while (counter < MODULE_TIMEOUT);
	timer_stop(desc->timer);
}

/*
 * If a message is received from the module. Get the length of the message
 */
static int32_t	process_ipd_msg(struct at_desc *desc, uint32_t *len) {
	static const struct at_buff at_ipd = {USTR("+IPD,"), 5};
	static const uint32_t	max_ch_search = 13; //+IDP,4,1024: Max code
	struct at_buff	aux;
	uint32_t	i, k;
	bool		cmp = false;
	uint32_t	id;
	uint8_t		*result = desc->result.buff,
			*ipd = at_ipd.buff;
	uint32_t	nb_ch;

	i = desc->result.len;
	nb_ch = 0;
	while (i >= 0 && !cmp && nb_ch < max_ch_search) {
		if (result[i] == ',') {
			/* Compare backwords ipd with response */
			k = 0;
			while (k < at_ipd.len &&
				result[i - k] == ipd[at_ipd.len - k - 1])
				k++;
			if (k == at_ipd.len)
				cmp = true;
		}
		i--;
		nb_ch++;
	}
	if (!cmp)
		return FAILURE;

	/* Calculate the index of + char to get the connection id and length */
	i = (i + 1) - (--k);
	at_set(&aux, result + i , desc->result.len - i);
	if (desc->multiple_conections == 1)
		at_sscanf(&aux, USTR("+IPD,%d,%d"), &id, len);
	else
		at_sscanf(&aux, USTR("+IPD,%d"),len);

	//Reset the match counters to the previous values
	desc->result.len = i - 2;
	for (i = 0; i < NB_RESPONSES; i++)
		if (desc->match_idx[i] >= 2)
			desc->match_idx[i] -= 2;

	return SUCCESS;
}
/* Handle the uart events */
static void	at_callback(void *app_param, enum UART_EVENT event,
								uint8_t	*data)
{
	struct at_desc	*desc = app_param;
	uint32_t	len;

	switch (event)
	{
	case WRITE_DONE:
		/* Reset cmd buffer */
		desc->cmd.len = 0;
		break;
	case READ_DONE:
		if (!desc->ipd_received) {
			if(*desc->read_ch == ':')
				/* Check if message from server is received */
				if (SUCCESS == process_ipd_msg(desc, &len))
				{
					/*
					 * Set ipd_received and wait for buffer
					 * to be filled
					 */
					desc->app_response.len = len;
					uart_read(desc->uart_desc,
						desc->app_response.buff, len);
					desc->ipd_received = true;
					break;
				}
			if (*desc->read_ch == '>' && desc->waiting_send) {
				/* Notify driver that data can be sent */
				desc->waiting_send = false;
			} else if (desc->result.len >= RESULT_BUFF_LEN) {
				desc->result_status = OVERFLOW_ERROR;
			}
			else {
				/* Add received character to result buffer */
				at_strcatch(&desc->result, *desc->read_ch);
				/* Compare buffer with possible responses and
				 * update status */
				refresh_status(desc);
			}
		} else { //The server message filled the buffer
			/* Notify that a message is available */
			desc->ready = true;
			/* Invalidate waiting for server message status */
			desc->ipd_received = false;
		}
		/* Submmit buffer to read the next char */
		uart_read(desc->uart_desc, desc->read_ch, 1);
		break;
	case ERROR:
		/* TODO change to log */
		printf("error 0x%x", (unsigned)data);
		//printf("DataAvailable");
		uart_read(desc->uart_desc, desc->read_ch, 1);
		/* In case is blocked in write_cmd */
		desc->cmd.len = 0;
		break;
	default:
		/* TODO change to log */
		printf("Default");
		uart_read(desc->uart_desc, desc->read_ch, 1);
		/* In case is blocked in write_cmd */
		desc->cmd.len = 0;
		break;
	}
}

/* Blocking uart write */
static void	write_cmd(struct at_desc *desc)
{
	/* Update status */
	desc->result_status = WAITING_RESULT;

	uart_write(desc->uart_desc, desc->cmd.buff, desc->cmd.len);
	while (desc->cmd.len)
		/* Len will be set to 0 when write is done */
		;
}

/* Translate the param to the driver format */
static void	build_cmd_param(struct at_desc *desc, enum at_cmd id,
						union in_param *param)
{
	struct at_buff	aux[1];
	uint32_t	conn_id;

	/* App_response is used as a temporary buffer */
	switch (id){
	case AT_DEEP_SLEEP:
		at_sprintf(&desc->app_response, USTR("d"),
				param->deep_sleep_time_ms);
		break;
	case AT_SET_OPERATION_MODE:
		at_sprintf(&desc->app_response, USTR("d"), param->wifi_mode);
		break;

	case AT_CONNECT_NETWORK:
		at_sprintf(&desc->app_response, USTR("ss"), &param->network.ssid,
				&param->network.pwd);
		break;
	case AT_SET_ACCESS_POINT:
		at_sprintf(&desc->app_response, USTR("ssdd"),
				&param->ap.ssid, &param->ap.pwd,
				param->ap.ch_id, param->ap.encription);
		break;
	case AT_START_CONNECTION:
		if (param->connection.soket_type == SOCKET_TCP) {
			str_to_at(aux, USTR("TCP"));
			if (desc->multiple_conections) {
				at_sprintf(&desc->app_response, USTR("dssd"),
					(int32_t)param->connection.id,
						aux,
						&param->connection.addr,
					(int32_t)param->connection.port);
			} else {
				at_sprintf(&desc->app_response, USTR("ssd"),
						aux,
						&param->connection.addr,
					(int32_t)param->connection.port);
			}
		} else {
			str_to_at(aux, USTR("UDP"));
			if (desc->multiple_conections) {
				at_sprintf(&desc->app_response, USTR("dssddd"),
					(int32_t)param->connection.id,
						aux,
						&param->connection.addr,
					(int32_t)param->connection.port,
					(int32_t)param->connection.local_port,
					(int32_t)param->connection.udp_mode);
			} else {
				at_sprintf(&desc->app_response,
						USTR("ssddd"),
						aux,
						&param->connection.addr,
					(int32_t)param->connection.port,
					(int32_t)param->connection.local_port,
					(int32_t)param->connection.udp_mode);
			}
		}
		break;
	case AT_SEND:
		conn_id = desc->multiple_conections ? param->connection.id : 0;
		if (desc->connections[conn_id].type == SOCKET_TCP) {
			if (desc->multiple_conections)
				at_sprintf(&desc->app_response, USTR("dd"),
					(int32_t)param->send_data.id,
					(int32_t)param->send_data.data.len);
			else
				at_sprintf(&desc->app_response, USTR("d"),
					(int32_t)param->send_data.data.len);
		} else {
			if (desc->multiple_conections)
				at_sprintf(&desc->app_response, USTR("ddsd"),
					(int32_t)param->send_data.id,
					(int32_t)param->send_data.data.len,
						&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
			else
				at_sprintf(&desc->app_response, USTR("dsd"),
					(int32_t)param->send_data.data.len,
						&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
		}
		break;
	case AT_STOP_CONNECTION:
		at_sprintf(&desc->app_response, USTR("d"), param->conn_id);
		break;
	case AT_SET_CONNECTION_TYPE:
		at_sprintf(&desc->app_response, USTR("d"),
				(int32_t)param->conn_type);
		break;
	case AT_SET_SERVER:
		at_sprintf(&desc->app_response, USTR("dd"),
						(int32_t)param->server.action,
						(int32_t)param->server.port);
		break;
	case AT_SET_TRANSPORT_MODE:
		at_sprintf(&desc->app_response, USTR("d"),
			(int32_t)param->transport_mode);
		break;
	case AT_SET_CLIENT_TIMEOUT:
		at_sprintf(&desc->app_response, USTR("d"),
			(int32_t)param->timeout);
		break;
	case AT_PING:
		at_sprintf(&desc->app_response, USTR("s"), &param->ping_ip);
		break;
	default:
		return;
	}
	at_strcat(&desc->cmd, &desc->app_response);
}

/* Creat the driver command with the specified parameters */
static void	build_cmd(struct at_desc *desc, enum at_cmd cmd,
			enum cmd_operation op, union in_out_param *param)
{
	/* Write command in buffer: AT[CMD][OP]<parmas>\r\n*/
	/* AT */
	at_strcatch(&desc->cmd, 'A');
	at_strcatch(&desc->cmd, 'T');
	/* CMD */
	at_strcat(&desc->cmd, &g_map[cmd].cmd);
	/* OP */
	if (op == AT_QUERY_OP)
		at_strcatch(&desc->cmd, '?');
	if (op == AT_TEST_OP) {
		at_strcatch(&desc->cmd, '=');
		at_strcatch(&desc->cmd, '?');
	}
	if (op == AT_SET_OP && param) {
		at_strcatch(&desc->cmd, '=');
		build_cmd_param(desc, cmd, &param->in);
	}
	/* \r\n */
	at_strcatch(&desc->cmd, '\r');
	at_strcatch(&desc->cmd, '\n');
}

/* Special handle of ATE0 command */
static int32_t	stop_echo(struct at_desc *desc)
{
	memcpy(desc->cmd.buff, "ATE0\r\n", 6);
	desc->cmd.len = 6;
	write_cmd(desc);

	wait_for_response(desc);

	desc->result.len = 0;
	if (desc->result_status == RESULT_AVAILABE)
		return SUCCESS;

	/* Timeout or error response */
	return FAILURE;
}

/* Handle special cases */
static int32_t	handle_special(struct at_desc *desc, enum at_cmd cmd)
{
	switch (cmd) {
	case AT_RESET:
		write_cmd(desc);
		mdelay(5000);
		desc->result.len = 0;
		break;
	case AT_DEEP_SLEEP:
		/* TODO */
		return FAILURE;
	default:
		return FAILURE;
	}
	return SUCCESS;

}

/*
 * Copy the result buffer to the user buffer
 * TODO: Parse result to parameters
 */
static int32_t	parse_result(struct at_desc *desc, enum at_cmd cmd,
		union out_param *result)
{
	at_strcpy(&desc->app_response, &desc->result);
	//Clear result
	desc->result.len = 0;

	/* TODO : Parse result to parameters */
	/* Copy result in user buffer */
	at_set(&result->result, desc->app_response.buff,
					desc->app_response.len);

	return SUCCESS;
}

/**
 *
 * @param desc Function that the application use to send commands
 * @param cmd
 * @param result Data will be valid until next call to a driver function
 * @return
 */
int32_t at_run_cmd(struct at_desc *desc, enum at_cmd cmd, enum cmd_operation op,
		union in_out_param *param)
{
	uint32_t id;

	if (!desc)
		return FAILURE;

	if (!(g_map[cmd].type & op))
		return FAILURE;

	build_cmd(desc, cmd, op, param);

	if (cmd == AT_DEEP_SLEEP || cmd == AT_RESET)
		return handle_special(desc, cmd);

	desc->waiting_send = true;
	write_cmd(desc);

	if (cmd == AT_SEND) {
		/* Befor sending the payload, we must receive '>' */
		while (desc->waiting_send)
			;
		/* Send payload */
		at_strcpy(&desc->cmd, &param->in.send_data.data);
		write_cmd(desc);
	}

	/* Wait for OK, SEND OK or ERROR */
	wait_for_response(desc);
	if (desc->result_status != RESULT_AVAILABE)
		return FAILURE;

	/* Update param related with connections */
	if (cmd == AT_SET_CONNECTION_TYPE)
		desc->multiple_conections = param->in.conn_type;
	if (cmd == AT_START_CONNECTION) {
		id = desc->multiple_conections ? param->in.connection.id : 0;
		desc->connections[id].active = true;
		desc->connections[id].type = param->in.connection.soket_type;
	}
	if (param) {
		if (op == AT_SET_OP)
			cmd = AT_GET_VERSION; //Only copy response
		if (SUCCESS != parse_result(desc, cmd, &param->out))
			return FAILURE;
	}
	else
		/* Clear the result*/
		desc->result.len = 0;

	return SUCCESS;
}


struct aducm_uart_init_param aducm_param = {
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS,
		.callback = at_callback,
		.param = NULL
};

struct uart_init_param uart_param = {0, BD_115200, &aducm_param};

struct timer_init_param timer_param = {0, 1000, 0, NULL};

/**
 *
 * @param desc
 * @param param
 */
int32_t at_init(struct at_desc **desc, struct at_init_param *param)
{
	struct at_desc		*ldesc;
	union in_out_param	result;
	uint32_t		conn;

	if (!desc || !param || !param->read_buff || !param->buff_size)
		return FAILURE;

	ldesc = calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	aducm_param.param = ldesc;
	if (SUCCESS != uart_init(&(ldesc->uart_desc), &uart_param))
		goto free_desc;
	/* The read will be handled by the callback */
	uart_read(ldesc->uart_desc, ldesc->read_ch, 1);

	if (SUCCESS != timer_init(&(ldesc->timer), &timer_param))
		goto free_uart;

	ldesc->received_package.buff = param->read_buff;
	ldesc->received_package.len = param->buff_size;
	/* Link buffer structure with static buffers */
	ldesc->app_response.buff = ldesc->buffers.app_response_buff;
	ldesc->app_response.len = 0;
	ldesc->result.buff = ldesc->buffers.result_buff;
	ldesc->result.len = 0;
	ldesc->cmd.buff = ldesc->buffers.cmd_buff;
	ldesc->cmd.len = CMD_BUFF_LEN;

	/* Disable echoing response */
	if (SUCCESS != stop_echo(ldesc))
		goto free_timer;

	/* Test AT */
	if (SUCCESS != at_run_cmd(ldesc, AT_ATTENTION, AT_EXECUTE_OP, NULL))
		goto free_timer;

	/* Get the connection type */
	if (SUCCESS == at_run_cmd(ldesc, AT_SET_CONNECTION_TYPE, AT_QUERY_OP,
			&result)) {
		at_sscanf(&result.out.result, USTR("+CIPMUX:%d\r\n"), &conn);
		ldesc->multiple_conections = conn ? true : false;
	}
	else {
		goto free_timer;
	}

	*desc = ldesc;
	return SUCCESS;

free_timer:
	timer_remove(ldesc->timer);
free_uart:
	uart_remove(ldesc->uart_desc);
free_desc:
	free(ldesc);
	*desc = NULL;
	return FAILURE;
}

/* Free driver resources */
int32_t at_remove(struct at_desc *desc)
{
	if (!desc)
		return FAILURE;
	timer_remove(desc->timer);
	uart_remove(desc->uart_desc);
	free(desc);

	return SUCCESS;
}

/* Fill the at_buff structure from null terminated string */
void	str_to_at(struct at_buff *dest, uint8_t *src)
{
	if (!dest || !src)
		return ;

	dest->buff = (uint8_t *)src;
	dest->len = strlen((char *)src);
}

/* Return Null terminated string from at_buff structure */
uint8_t	*at_to_str(struct at_buff *src)
{
	if (!src)
		return NULL;
	src->buff[src->len] = 0;

	return src->buff;
}

/* At_buff functions */
/* copy src to dest */
static inline void at_strcpy(struct at_buff *dest, const struct at_buff *src)
{
	memcpy(dest->buff, src->buff, src->len);
	dest->len = src->len;
}

/* Set at_buff structure parameters */
static inline void at_set(struct at_buff *dest, uint8_t *buff, uint32_t len)
{
	dest->buff = buff;
	dest->len = len;
}

/* concatenate src to dest */
static inline void at_strcat(struct at_buff *dest, const struct at_buff *src)
{
	memcpy(dest->buff + dest->len, src->buff, src->len);
	dest->len += src->len;
}

/* concatenate ch to dest */
static inline void at_strcatch(struct at_buff *dest, uint8_t ch) {
	dest->buff[dest->len] = ch;
	dest->len++;
}

/**
 * @brief Create formated string in dest according with fmt
 * The parameters ar separated by , and the strings are rounded by '\"' and
 * escaped.
 * @param dest - Where the buffer is written
 * @param fmt - Format: Null terminated string containing the type of the
 * arguments. d for int32_t or s for at_buff. Ex: "dds" meand: 2 integers and a
 * int32_t.
 */
static void at_sprintf(struct at_buff *dest, uint8_t *fmt, ...){
	    va_list		args;
	    uint8_t		buff[12]; //A MAX int32_t fits
	    struct at_buff	aux_str[1];
	    struct at_buff	*str;
	    uint32_t		i;
	    int32_t		nb = 0;

	    dest->len = 0;
	    va_start (args, fmt);
	    while (*fmt)
	    {
		    switch (*fmt)
		    {
		    case 'd':
			    nb = va_arg(args, int32_t);
			    itoa(nb, (char *)buff, 10);
			    str_to_at(aux_str, buff);
			    at_strcat(dest, aux_str);
			    break;
		    case 's':
			    str = va_arg(args, struct at_buff *);
			    at_strcatch(dest, '\"');
			    /* Escape character */
			    for (i = 0; i < str->len; i++) {
				    uint8_t ch = str->buff[i];
				    if (ch == '\"' || ch == ',' || ch == '\\')
					    at_strcatch(dest, '\\');
				    at_strcatch(dest, ch);
			    }
			    at_strcatch(dest, '\"');
			    break;
		    default:
			    break;
		    }
		    fmt++;
		    if (*fmt)
			    at_strcatch(dest, ',');
	    }

	    va_end (args);
}


/*
 * Fill the parameters from a formated string.
 * Ex:	at_buff src = "AA123";
 * 	int n;
 *	at_sscanf(src, "AA%d", &n)
 */
static void at_sscanf(struct at_buff *src, uint8_t *fmt, ...)
{
	char		buff[12]; //A MAX int32_t fits
	char		*aux;
	va_list		args;
	//struct at_buff	*str;
	int32_t		*nb;
	uint32_t	i;

	va_start(args, fmt);
	i = 0;
	while (*fmt)
	{
		if (i >= src->len)
			goto failure;
		if (*fmt != src->buff[i])
		{
			if (*fmt != '%')
				goto failure;
			fmt++;
			if (!*fmt)
				goto failure;
			switch (*fmt) {
			case 'd':
				nb = va_arg(args, int32_t *);
				/* Create null terminate string */
				memcpy(buff, src->buff + i,
						min(src->len - i, 11));
				buff[min(src->len - i, 11)] = 0;
				*nb = strtol(buff, &aux, 10);
				i += aux - buff - 1;
				break;
			case 's':
				/* TODO */
				break;
			default:
				goto failure;
			}
		}
		fmt++;
		i++;
	}

	va_end(args);
	return ;

failure:
	va_end(args);
	printf("Bug found\n");

}

