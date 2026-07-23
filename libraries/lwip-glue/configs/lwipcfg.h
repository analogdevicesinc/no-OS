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
#ifndef LWIP_LWIPCFG_H
#define LWIP_LWIPCFG_H

/** Default DHCP Configuration: (In Order to Enable, Set Definition to 1) **/
#define USE_DHCP    					1

/** Default Static IP Address Configuration: (Configured by User) **/
#define LWIP_PORT_INIT_IPADDR(addr)   	IP4_ADDR((addr), 192,168,100,200)
#define LWIP_PORT_INIT_GW(addr)       	IP4_ADDR((addr), 192,168,100,1)
#define LWIP_PORT_INIT_NETMASK(addr)  	IP4_ADDR((addr), 255,255,255,0)

/** Default Ethernet MAC Address Values: (Configured by User) **/
#define MAC_BYTE1			0x00
#define MAC_BYTE2			0x18
#define MAC_BYTE3			0x80
#define MAC_BYTE4			0x03
#define MAC_BYTE5			0x25
#define MAC_BYTE6			0x80
#define MAC_LEN				6

#endif /* LWIP_LWIPCFG_H */

