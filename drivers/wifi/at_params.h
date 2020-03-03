#ifndef AT_PARAMS_H
#define AT_PARAMS_H

/*
 * Auxiliar structures
 */

struct at_buff {
	uint8_t		*buff;
	uint32_t	len;
};

enum encryption_type {
	OPEN		= 0u,
	WEP		= 1u,
	WPA_PSK		= 2u,
	WPA2_PSK	= 3u,
	WPA_WPA2_PSK	= 4u
};

enum socket_type {
	SOCKET_TCP,
	SOCKET_UDP
};

/*
 * Parameter structures
 */
enum cwmod_param {
	CLIENT = 1,
	ACCESS_POINT,
	CLIENT_AND_ACCESS_POINT
} mode;

struct cwjap_param {
	struct at_buff	ssid;
	struct at_buff	pwd;
};

struct cwsap_param {
	struct at_buff		ssid;
	struct at_buff		pwd;
	uint32_t		ch_id;
	enum encryption_type	encription;
};

struct cipstart_param {
	uint32_t		id; //if cipmux = 1
	enum socket_type	soket_type; //To be parsed to "TCP or UDP"
	struct at_buff		addr;
	uint16_t		port;
	/* Parameters for udp */
	uint16_t		local_port;
	enum {
		UDP_ENTITY_NO_CHANGE,
		UDP_ENTITY_CHANGE_ONE,
		UDP_ENTITY_CHANGE_ALLOWED
	}			udp_mode;
};

struct cipsend_param {
	uint32_t	id; //if cipmux = 1
	struct at_buff	data;
	struct at_buff	remote_ip;
	uint16_t	remote_port;
};

enum connection_type {
	SINGLE_CONNECTION,
	MULTIPLE_CONNECTION
};


struct cipserver_param {
	enum {
		CLOSE_SOCKET,
		OPEN_SOCKET
	}		action;
	uint16_t	port;
};


enum cipmode_param {
	NORMAL_MODE,
	UNVARNISHED_MODE
};

/* Network search */
struct cwlap_set_param {
	struct at_buff		ssid;
	struct at_buff		mac;
	uint8_t			channel;
};

#if defined(PARSE_RESULT)
/* Search networks. Not used in driver*/
struct cwlap_exec_param {
	struct at_buff		ssid;
	struct at_buff		mac;
	enum encryption_type	encryption;
	/* I think in dB */
	int32_t			strength;
	uint8_t			channel;
};

/* Search networks. Not used in driver*/
struct cipstatus_param {
	enum {
		GOT_IP		= 1u,
		CONNECTED	= 3u,
		DISCONNECTED	= 4u
	}			stat;
	struct {
		uint8_t			connection_id;
		enum socket_type	socket_type;
		struct at_buff		remote_ip;
		uint16_t		*remote_port;
		uint16_t		*local_port;
		enum {
			RUNNING_AS_CLIENT	= 0u,
			RUNNING_AS_SERVER	= 1u
		}			tetype;
	}			connections[4]; //Max 4 connections. To test it.
};

struct cifsr_param {
	struct at_buff		soft_ap_ip;
	struct at_buff		station_ip;
};

struct cwlap_querry_param {
	struct at_buff		ssid;
	struct at_buff		mac;
	uint8_t			channel;
	/* I think in dB */
	int32_t			strength;
};
#endif

#endif /* AT_PARAMS_H */
