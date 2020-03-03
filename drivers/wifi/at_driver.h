/** https://cdn.sparkfun.com/datasheets/Wireless/WiFi/Command%20Doc.pdf */
/** https://github.com/espressif/ESP8266_AT/wiki/basic_at_0019000902
 * think about add at+GSLP
 * */
#ifndef AT_DRIVER_H
# define AT_DRIVER_H

#include  <stdint.h>
#include "uart.h"
#include "uart_extra.h"
#include "timer.h"
#include "at_params.h"

#define NB_RESPONSES 4
#define MAX_CONNECTIONS 4

/* TODO */
//#define PARSE_RESULT

enum at_cmd{
			/** Attention */
/*00 "" 	  */	AT_ATTENTION,
			/** Reset module */
/*01 "+RST"	  */	AT_RESET,
			/** Enter in deep sleep mode */
/*02 "+GSLP"	  */	AT_DEEP_SLEEP,
			/** View version info */
/*03 "+GMR"	  */	AT_GET_VERSION,
			/** Setting operation mode.
			 * - Client
			 * - Access Point
			 * - Client and Access Point
			 * The parameter to configure this must be \ref cwmod_param
			 * */
/*04 "+CWMODE"	  */	AT_SET_OPERATION_MODE,
			/** Connect to network */
/*05 "+CWJAP"	  */	AT_CONNECT_NETWORK,
			/** List available networks */
/*06 "+CWLAP"	  */	AT_GET_AVAILABLE_NETWORKS,
			/** Disconnect from network */
/*07 "+CWQAP"	  */	AT_DISCONNECT_NETWORK,
			/** Set access point when running as Hotspot*/
/*08 "+CWSAP"	  */	AT_SET_ACCESS_POINT,
			/** Get ips of connected clients to hotspot */
/*09 "+CWLIF"	  */	AT_GET_CONNECTED_IPS,
			/** Get module status */
/*10 "+CIPSTATUS" */	AT_GET_STATUS,
			/** Start new connection */
/*11 "+CIPSTART"  */	AT_START_CONNECTION,
			/** Send data over connection */
/*12 "+CIPSEND"	  */	AT_SEND,
			/** Stop connection */
/*13 "+CIPCLOSE"  */	AT_STOP_CONNECTION,
			/** Get ip */
/*14 "+CIFSR"	  */	AT_GET_IP,
			/** Set multiple or single connection type */
/*15 "+CIPMUX"	  */	AT_SET_CONNECTION_TYPE,
			/** Configure a TCP server */
/*16 "+CIPSERVER" */	AT_SET_SERVER,
			/** Set transport mode */
/*17 "+CIPMODE"	  */	AT_SET_TRANSPORT_MODE,
			/** Set client timeout */
/*18 "+CIPSTO"	  */	AT_SET_CLIENT_TIMEOUT,
			/** Ping */
/*19 "+PING"	  */	AT_PING
};

/** Command type. Each command cand be executed as one of this types */
enum cmd_operation {
	/* Test command */
	AT_TEST_OP	= 0x1u,
	/* Query current values of the associated command */
	AT_QUERY_OP	= 0x2u,
	/* Set current values of the associated command */
	AT_SET_OP	= 0x4u,
	/* Execute command */
	AT_EXECUTE_OP	= 0x8u
};

/* Parameters returned by the command */
union out_param {
/* TODO : Implement parsing response to parameters */
#if defined(PARSE_RESULT)
	/** Result for TEST_OPERATIONS */
	struct at_buff		test_result;
	/** Result for \ref AT_GET_VERSION */
	struct at_buff		version;
	/** Result for \ref AT_GET_IP */
	struct cifsr_param	local_ip;
	/** Result for \ref AT_GET_AVAILABLE_NETWORKS */
	struct cwlap_exec_param	available_networks;
	/** Result for \ref AT_GET_STATUS */
	struct cipstatus_param	status;
	/** Result form AT_SET_OPERATION_MODE */
	enum cwmod_param	wifi_mode;
	/** Result form AT_CONNECT_NETWORK */
	struct cwlap_querry_param wifi_info;
	/** Result form AT_SET_ACCESS_POINT */
	struct cwsap_param	ap_info;
	/** Result form AT_SET_CONNECTION_TYPE */
	struct cipmux_param	con_type;
	/** Result form AT_SET_TRANSPORT_MODE */
	struct cipmode_param	transp_mode;
	/** Result form AT_SET_CLIENT_TIMEOUT */
	struct cipsto_param	timeout;
#else
	/* The result of the executing command is writtend in result */
	struct at_buff		result;
#endif
};

/* Parameters used to set a command */
union in_param {
	/** Param for \ref AT_DEEP_SLEEP */
	uint32_t		deep_sleep_time_ms;
	/** Param for \ref AT_SET_OPERATION_MODE */
	enum cwmod_param	wifi_mode;
	/** Param for \ref AT_CONNECT_NETWORK */
	struct cwjap_param	network;
	/** Param for \ref AT_SET_ACCESS_POINT */
	struct cwsap_param	ap;
	/** Param for \ref AT_START_CONNECTION */
	struct cipstart_param	connection;
	/** Param for \ref AT_SEND */
	struct cipsend_param	send_data;
	/** Param for \ref AT_STOP_CONNECTION */
	uint32_t		conn_id;
	/** Param for \ref AT_SET_CONNECTION_TYPE */
	enum connection_type	conn_type;
	/** Param for \ref AT_SET_SERVER */
	struct cipserver_param	server;
	/** Param for \ref AT_SET_TRANSPORT_MODE */
	enum cipmode_param	transport_mode;
	/** Param for \ref AT_SET_CLIENT_TIMEOUT . Range 0~7200 seconds */
	uint32_t		timeout;
	/** Param for \ref AT_PING */
	struct at_buff		ping_ip;
};

/* Parameter to initialize driver */
struct at_init_param {
	uint8_t		*read_buff;
	uint32_t	buff_size;
};

/* Parameter used to send and receive data from a command */
union in_out_param {
	union out_param out;
	union in_param	in;
};

#define CMD_BUFF_LEN	2000u //? Is enaught
#define RESULT_BUFF_LEN	2000u //? To think about this

/** If ok on list, move descriptor to .c file */
struct at_desc {
	/** Buffers */
	struct {
		uint8_t	app_response_buff[RESULT_BUFF_LEN];
		uint8_t	result_buff[RESULT_BUFF_LEN];
		uint8_t	cmd_buff[CMD_BUFF_LEN];
	} 			buffers;
	/** Buffer where is stored the output parameter for the application */
	struct at_buff		app_response;
	/** Buffer where data received from the module is stored */
	struct at_buff		result;
	/** Buffer to build the command */
	struct at_buff		cmd;
	/** Given by the user. TODO: Buffer to stor the incoming messages */
	//struct at_buff		received_package;
	/** Buffer to read one char */
	uint8_t			read_ch[1];
	/** States during the execution of at_run_cmd  */
	volatile enum {
		WAITING_RESULT,
		RESULT_AVAILABE,
		RESULT_ERROR,
		OVERFLOW_ERROR
	}			result_status;
	/** Event used when sending payload */
	volatile bool		waiting_send;
	/** Event used when reading payload */
	volatile bool		ipd_received;
	/** Indexes for in the possibles response given by the driver */
	uint32_t		match_idx[NB_RESPONSES];
	/** Timer handler */
	struct timer_desc	*timer;
	/** UART handler */
	struct uart_desc	*uart_desc;
	/** Structure used to store the connection statuses */
	struct {
		bool			active;
		enum socket_type	type;
	}			connections[MAX_CONNECTIONS];
	/** Stores if running as single or multiple connection */
	bool			multiple_conections;
	/** Used to notify app that payload have been received. Will be used
	 * other mechanism */
	volatile bool		ready;

};

/** Initialize wifi driver */
int32_t at_init(struct at_desc **desc, struct at_init_param *param);
/** Free resources used by wifi driver */
int32_t at_remove(struct at_desc *desc);

/** Read payload (TODO) */
int32_t get_last_message(struct at_desc *desc, struct at_buff *msg);

/** Execute AT command */
int32_t at_run_cmd(struct at_desc *desc, enum at_cmd cmd, enum cmd_operation op,
			union in_out_param *param);

/** Convert null terminated string to at_buff */
void	str_to_at(struct at_buff *dest, uint8_t *src);
/** Convert at_buff to null terminated string */
uint8_t	*at_to_str(struct at_buff *src);

// TODO
/*
void enter_unvernished_mode();
void send_unvarnished();
void exit_unvernished_mode();
*/
#endif
