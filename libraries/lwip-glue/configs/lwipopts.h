/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H

/* NO_SYS==1: Use lwIP without OS-awareness (no thread and etc.) */
#define NO_SYS                     		1
#define LWIP_SOCKET                		0
#define LWIP_NETCONN               		0
#define LWIP_NETIF_API             		0
#define SYS_LIGHTWEIGHT_PROT    		0

/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an
   interface whenever the link changes (i.e., link down) */
#define LWIP_NETIF_LINK_CALLBACK		1

/* LWIP_IPV4==1: Enable IPv4 */
#define LWIP_IPV4                  		1

/* LWIP_IPV6==0: Disable IPv6 */
#define LWIP_IPV6                  		0

/* ---------- Memory Options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           		4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE               			65536

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF				32

/* MEMP_NUM_RAW_PCB: the number of UDP protocol control blocks. One
   per active RAW "connection". */
#define MEMP_NUM_RAW_PCB        		20

/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        		30

/* MEMP_NUM_TCP_PCB: the number of simultaneously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        		10

/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 		10

/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        		TCP_SND_QUEUELEN

/* MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active
   timeouts. */
#define MEMP_NUM_SYS_TIMEOUT    		17

/* ---------- PBUF Options ---------- */
#define PBUF_POOL_SIZE				30

#define LWIP_CHECKSUM_ON_COPY			1

/* ---------- ARP Options ---------- */
#define LWIP_ARP                		1
#define ARP_TABLE_SIZE          		255
#define ARP_QUEUEING            		1
#define ETHARP_TABLE_MATCH_NETIF		1
#define ETHARP_SUPPORT_STATIC_ENTRIES		1

/* ---------- IP Options ---------- */
#define IP_FORWARD              		0
#define IP_REASSEMBLY           		1
#define IP_FRAG                 		1
#define IP_REASS_MAX_PBUFS      		(5 * ((1500 + PBUF_POOL_BUFSIZE - 1) / PBUF_POOL_BUFSIZE))
#define MEMP_NUM_REASSDATA      		IP_REASS_MAX_PBUFS

/* ---------- UDP Options ---------- */
#define LWIP_UDP                		1
#define LWIP_UDPLITE            		LWIP_UDP
#define UDP_TTL                 		255

/* ---------- TCP Options ---------- */
#define LWIP_TCP                		1
#define TCP_TTL                 		255
#define LWIP_ALTCP              		LWIP_TCP

#define USE_DHCP				1
#define SO_REUSE				1

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         		1
#define TCP_OOSEQ_MAX_PBUFS			0
#define TCP_OOSEQ_MAX_BYTES   			0

#define LWIP_TCP_TIMESTAMPS   			1

/* TCP Maximum segment size. */
#define TCP_MSS                 		1460
/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             		8192

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#define TCP_SND_QUEUELEN       			(30 * TCP_SND_BUF / TCP_MSS)

/* TCP writable space (bytes). This must be less than or equal
   to TCP_SND_BUF. It is the amount of space which must be
   available in the tcp snd_buf for select to return writable */
#define TCP_SNDLOWAT           			(TCP_SND_BUF / 2)

/* TCP receive window. */
#define TCP_WND                 		(8 * TCP_MSS)

#define TCP_OVERSIZE				TCP_MSS

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX              		12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX           		4

/* ---------- ICMP Options ---------- */
#define LWIP_ICMP                  		LWIP_IPV4
#define ICMP_TTL                		255

/* ---------- DHCP Options ---------- */
#define LWIP_DHCP               		LWIP_UDP

/* ---------- AUTOIP Options ------- */
#define LWIP_AUTOIP				1
#define LWIP_DHCP_AUTOIP_COOP			1
#define LWIP_DHCP_AUTOIP_COOP_TRIES		2
#define LWIP_IGMP				1
#define LWIP_MDNS_RESPONDER			1
#define MDNS_MAX_SERVICES			255
#define LWIP_RAND				rand

#define LWIP_NETIF_EXT_STATUS_CALLBACK		1
#define MDNS_RESP_USENETIF_EXTCALLBACK		1
#define LWIP_NUM_NETIF_CLIENT_DATA		1

/* ---------- RAW Options ---------- */
#define LWIP_RAW                		1

#endif /* LWIP_LWIPOPTS_H */
