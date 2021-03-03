/*
 * linux_s.c
 *
 *  Created on: Feb 16, 2021
 *      Author: nramona
 */

#include <stdlib.h>
#include "linux_s.h"
#include "error.h"
#include "util.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

static int32_t linux_socket_open(struct linux_desc *desc, uint32_t *sock_id,
				enum socket_protocol prot, uint32_t buff_size);
static int32_t linux_socket_close(struct linux_desc *desc, uint32_t sock_id);
static int32_t linux_socket_connect(struct linux_desc *desc, uint32_t sock_id,
				   struct socket_address *addr);
static int32_t linux_socket_disconnect(struct linux_desc *desc, uint32_t sock_id);
static int32_t linux_socket_send(struct linux_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size);
static int32_t linux_socket_recv(struct linux_desc *desc, uint32_t sock_id,
				void *data, uint32_t size);
static int32_t linux_socket_sendto(struct linux_desc *desc, uint32_t sock_id,
				  const void *data, uint32_t size,
				  const struct socket_address* to);
static int32_t linux_socket_recvfrom(struct linux_desc *desc, uint32_t sock_id,
				    void *data, uint32_t size,
				    struct socket_address *from);
static int32_t linux_socket_bind(struct linux_desc *desc, uint32_t sock_id,
				uint16_t port);
static int32_t linux_socket_listen(struct linux_desc *desc, uint32_t sock_id,
				  uint32_t back_log);
static int32_t linux_socket_accept(struct linux_desc *desc, uint32_t sock_id,
				  uint32_t *client_socket_id);

int32_t linux_init(struct linux_desc** desc, struct linux_init_param* param)
{
	struct linux_desc* adesc;

	if(!desc || !param)
		return -EINVAL;

	adesc = (struct linux_desc*)calloc(1,sizeof(*adesc));

	if(!adesc)
		return -ENOMEM;
	for(int i = 0; i < SOCKETS; i++)
		adesc->sockets[i] = 0;
	adesc->server.fd = 0;
	//adesc->server.back_log_clients = SOCKET_NO;
	adesc->net.net = adesc;
	adesc->net.socket_open = (int32_t (*)(void *, uint32_t *, enum socket_protocol,
		     uint32_t)) linux_socket_open;
	adesc->net.socket_close = (int32_t (*)(void *, uint32_t)) linux_socket_close;
	adesc->net.socket_connect = (int32_t (*)(void *, uint32_t,struct socket_address *))linux_socket_connect;
	adesc->net.socket_disconnect = (int32_t (*)(void *, uint32_t))linux_socket_disconnect;
	adesc->net.socket_send = (int32_t (*)(void *, uint32_t, const void *, uint32_t))linux_socket_send;
	adesc->net.socket_recv = (int32_t (*)(void *, uint32_t, void *, uint32_t))linux_socket_recv;
	adesc->net.socket_sendto = (int32_t (*)(void *, uint32_t, const void *, uint32_t, const struct socket_address* to))linux_socket_sendto;
	adesc->net.socket_recvfrom = (int32_t (*)(void *, uint32_t, void *, uint32_t, struct socket_address* from))linux_socket_recvfrom;
	adesc->net.socket_bind = (int32_t (*)(void *, uint32_t, uint16_t))linux_socket_bind;
	adesc->net.socket_listen = (int32_t (*)(void *, uint32_t, uint32_t))linux_socket_listen;
	adesc->net.socket_accept= (int32_t (*)(void *, uint32_t, uint32_t*))linux_socket_accept;

	*desc = adesc;
	return SUCCESS;
}

int32_t linux_remove(struct linux_desc* desc)
{
	if(!desc)
		return -EINVAL;

	for(int i = 0; i < SOCKET_NO; i++)
		linux_socket_close(desc,i);

	free(desc);
	return SUCCESS;
}

int32_t linux_connect() {
	return -ENOSYS;
}

int32_t linux_get_network_interface(struct linux_desc *desc, struct network_interface** net)
{
	if (!desc || !net)
		return -EINVAL;

	*net = &desc->net;

	return SUCCESS;
}

static int32_t get_unused_linux_socket(struct linux_desc* desc, uint32_t* socket_index)
{
	for(int i = 0; i < SOCKET_NO; i++) {
		if(desc->sockets[i] == 0) {
			socket_index = &i;
			return SUCCESS;
		}
	}

	return -EMLINK;
}

static int32_t linux_socket_open(struct linux_desc *desc, uint32_t *sock_id,
				enum socket_protocol prot, uint32_t buff_size)
{
	printf("se intra in open?\n");
	uint32_t id;
	int32_t ret;

	if(!desc)
		return -EINVAL;

	ret = get_unused_linux_socket(desc,&id);
	printf("wtf cat e id %d\n",id);
	if(ret != SUCCESS)
		return ret;

	if(prot == PROTOCOL_TCP) {
		desc->sockets[id] = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		ret = desc->sockets[id];
		desc->server.fd = desc->sockets[id];
		printf("intra in if tcp cum ar tebui, si ret e %d\n",ret);
	}
	else if(prot == PROTOCOL_UDP) {
		desc->sockets[id] = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		ret = desc->sockets[id];
	}
	if(ret < 0)
		return ret;

	sock_id = &id;
	printf("si sock id ul care s-a setat este %d\n",*sock_id);
	
	return SUCCESS;
}


static int32_t linux_socket_close(struct linux_desc *desc, uint32_t sock_id)
{
	int32_t ret;

	if(!desc || sock_id >= SOCKET_NO) {
		return -EINVAL;
	}

	ret = close(desc->sockets[sock_id]);
	if(ret != SUCCESS)
		return ret;
	desc->sockets[sock_id] = -EINVAL; //temporary

	return SUCCESS;

}

static int32_t linux_socket_connect(struct linux_desc *desc, uint32_t sock_id,
				   struct socket_address *addr)
{
	int32_t ret;

	if(!desc || !addr || sock_id >= SOCKET_NO || desc->server.fd == desc->sockets[sock_id]) {
		return -EINVAL;
	}

	ret = get_unused_linux_socket(desc, &sock_id);
	if(ret != SUCCESS)
		return ret;

	struct sockaddr_in saddr;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(addr->port);
	struct hostent* hptr = gethostbyname(addr->addr);
	saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;

	socklen_t len = sizeof(saddr);
	connect(desc->sockets[sock_id],(struct sockaddr*) &saddr,len);
	return SUCCESS;

}

static int32_t linux_socket_disconnect(struct linux_desc *desc, uint32_t sock_id)
{
	int32_t ret;

	if(desc->sockets[sock_id] == 0)
		return SUCCESS;
	if(desc->sockets[sock_id] == desc->server.fd) {
		for(int i = 0; i < desc->server.back_log_clients; i++) 
			linux_socket_close(desc,desc->server.client_ids[i]);
		ret = linux_socket_close(desc, sock_id);
	}
	else 
		ret = linux_socket_close(desc,sock_id);

	if(ret != SUCCESS)
		return ret;
	return SUCCESS;
}

static int32_t linux_socket_send(struct linux_desc *desc, uint32_t sock_id,
				const void *data, uint32_t size)
{
	int32_t ret;

	if(!desc || !data)
		return -EINVAL;
	
	ret = send(desc->sockets[sock_id], data, size,0);
	if(ret != SUCCESS)
		return ret;

	return SUCCESS;
}

static int32_t linux_socket_recv(struct linux_desc *desc, uint32_t sock_id,
				void *data, uint32_t size)
{
	int32_t ret;

	if(!desc)
		return -EINVAL;
	
	ret = recv(desc->sockets[sock_id], data, size, 0);
	if(ret != SUCCESS)
		return ret;
	
	return -SUCCESS;
}

static int32_t linux_socket_sendto(struct linux_desc *desc, uint32_t sock_id,
				  const void *data, uint32_t size,
				  const struct socket_address* to)
{
	int32_t ret;
	struct sockaddr_in saddr_to;
	socklen_t len;

	if(!desc)
		return -EINVAL;

	memset(&saddr_to,0,sizeof(saddr_to));
	saddr_to.sin_family = AF_INET;
	saddr_to.sin_port = htons(to->port);
	struct hostent* hptr = gethostbyname(to->addr);
	saddr_to.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr_to);
	
	ret = sendto(desc->sockets[sock_id], data, size, 0, (struct sockaddr*) &saddr_to, len);
	if(ret != SUCCESS)
		return ret;
	return SUCCESS;
}

static int32_t linux_socket_recvfrom(struct linux_desc *desc, uint32_t sock_id,
				    void *data, uint32_t size,
				    struct socket_address *from)
{
	int32_t ret;
	struct sockaddr_in saddr_from;
	socklen_t len;
	
	if(!desc)
		return -EINVAL;

	memset(&saddr_from,0,sizeof(saddr_from));
	saddr_from.sin_family = AF_INET;
	saddr_from.sin_port = htons(from->port);
	struct hostent* hptr = gethostbyname(from->addr);
	saddr_from.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr_from);

	ret = recvfrom(desc->sockets[sock_id], data, size, 0,(struct sockaddr*) &saddr_from, &len);
	if(ret != SUCCESS)
		return ret;
	return SUCCESS;
}

static int32_t linux_socket_bind(struct linux_desc *desc, uint32_t sock_id,
				uint16_t port)
{
	int32_t ret;
	struct sockaddr_in saddr;
	socklen_t len;
	if(!desc)
		return -EINVAL;

	printf("in bind the id we receive is %d \n",sock_id);
	// desc->server.fd = socket(AF_INET,SOCK_STREAM,0);
	// desc->socket_fds[sock_id] = desc->server.fd;
	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	struct hostent* hptr = gethostbyname("localhost");
	saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	len = sizeof(saddr);

	ret = bind(desc->sockets[sock_id], (struct sockaddr*) &saddr, len);
	printf("ret de la bind %d \n",ret);
	if(ret != SUCCESS)
		return ret;
	return SUCCESS;
}

static int32_t linux_socket_listen(struct linux_desc *desc, uint32_t sock_id,
				  uint32_t back_log)
{
	int32_t ret;
	if(!desc )
		return -EINVAL;
	ret = listen(desc->sockets[sock_id], back_log);
	if(ret != SUCCESS)
		return ret;
	return SUCCESS;
}


static int32_t linux_socket_accept(struct linux_desc *desc, uint32_t sock_id,
				  uint32_t *client_socket_id)
{
	struct sockaddr_in saddr;
	socklen_t len;
	printf("in the linux socket accept func\n");
	if(!desc || desc->server.fd != desc->sockets[sock_id]){
		return -EINVAL;
	}

	len = sizeof(saddr);
	*client_socket_id = accept(desc->server.fd, (struct sockaddr*) &saddr, &len);
	printf("client sock id %d\n",*client_socket_id);
	if(*client_socket_id < 0)
		return *client_socket_id;
	return SUCCESS;
}
