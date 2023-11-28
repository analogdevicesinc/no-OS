/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of the basic example.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdio.h>
#include "common_data.h"

#include "hpb.h"
#include "spixf.h"
#include "Ext_Flash.h"

#include "maxq1065.h"
#include "lwip_socket.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#include "adin1110.h"
#include "network_interface.h"
#include "mqtt_client.h"
#include "mqtt_noos_support.h"
#include "maxim_timer.h"
#include "no_os_timer.h"
#include "no_os_trng.h"
#include "maxim_trng.h"

#define EXT_FLASH_BAUD 4000000

static bool flash_test = false;
static bool ram1_test = false;
static bool ram2_test = false;

extern uint8_t __load_start_hpb_cs0;
extern uint8_t __load_length_hpb_cs0;
extern uint8_t __load_start_hpb_cs1;
extern uint8_t __load_length_hpb_cs1;
extern uint8_t __hpb_cs0_start;
extern uint8_t __hpb_cs1_start;
extern uint8_t __load_start_xip;
extern uint8_t __load_length_xip;

void spixf_cfg_setup()
{
	// Disable the SPIXFC before setting the SPIXF
	MXC_SPIXF_Disable();
	MXC_SPIXF_SetSPIFrequency(EXT_FLASH_BAUD);
	MXC_SPIXF_SetMode(MXC_SPIXF_MODE_0);
	MXC_SPIXF_SetSSPolActiveLow();
	MXC_SPIXF_SetSSActiveTime(MXC_SPIXF_SYS_CLOCKS_2);
	MXC_SPIXF_SetSSActiveTime(MXC_SPIXF_SYS_CLOCKS_3);

	MXC_SPIXF_SetCmdValue(EXT_FLASH_CMD_QREAD);
	MXC_SPIXF_SetCmdWidth(MXC_SPIXF_SINGLE_SDIO);
	MXC_SPIXF_SetAddrWidth(MXC_SPIXF_QUAD_SDIO);
	MXC_SPIXF_SetDataWidth(MXC_SPIXF_WIDTH_4);
	MXC_SPIXF_SetModeClk(EXT_FLASH_QREAD_DUMMY);

	MXC_SPIXF_Set3ByteAddr();
	MXC_SPIXF_SCKFeedbackEnable();
	MXC_SPIXF_SetSCKNonInverted();
}

static int ext_flash_board_init(void)
{
	return MXC_SPIXF_Init(0, EXT_FLASH_BAUD);
}

/******************************************************************************/
static int ext_flash_board_read(uint8_t* read, unsigned len, unsigned deassert,
				Ext_Flash_DataLine_t width)
{
	mxc_spixf_req_t req = {deassert, 0, NULL, read, (mxc_spixf_width_t)width, len, 0, 0, NULL};

	if (MXC_SPIXF_Transaction(&req) != len) {
		return E_COMM_ERR;
	}
	return E_NO_ERROR;
}

/******************************************************************************/
static int ext_flash_board_write(const uint8_t* write, unsigned len,
				 unsigned deassert,
				 Ext_Flash_DataLine_t width)
{
	mxc_spixf_req_t req = {deassert, 0, write, NULL, (mxc_spixf_width_t)width, len, 0, 0, NULL};

	if (MXC_SPIXF_Transaction(&req) != len) {
		return E_COMM_ERR;
	}
	return E_NO_ERROR;
}


static int ext_flash_clock(unsigned len, unsigned deassert)
{
	return MXC_SPIXF_Clocks(len, deassert);
}

__attribute__((section(".hpb_cs0_section"))) void test_func1(void)
{
	ram1_test = true;
}

__attribute__((section(".hpb_cs1_section"))) void test_func2(void)
{
	ram2_test = true;
}

__attribute__((section(".xip_section"))) void flash_test_func(void)
{
	flash_test = true;
}

void message_handler(struct mqtt_message_data *msg)
{
	msg->message.payload[msg->message.len] = 0;
	printf("Topic:%s -- Payload: %s\n", msg->topic, msg->message.payload);
}

struct no_os_timer_init_param adc_demo_tip = {
	.id = 0,
	.freq_hz = 32000,
	.ticks_count = 0,
	.platform_ops = &max_timer_ops,
	.extra = NULL,
};

extern struct no_os_timer_desc *timer;

#define CA_CERT                                                            \
    "-----BEGIN CERTIFICATE-----\r\n"                                      \
	"MIIDHzCCAgegAwIBAgIUCFbk7jeOFFqrrLVQjCoLQDCvNp0wDQYJKoZIhvcNAQEL\r\n"	\
	"BQAwHzEdMBsGA1UEAwwUQW5hbG9nIERldmljZXMsIEluYy4wHhcNMjMwMjI3MTAw\r\n"	\
	"NjQwWhcNMzIxMTI2MTAwNjQwWjAfMR0wGwYDVQQDDBRBbmFsb2cgRGV2aWNlcywg\r\n"	\
	"SW5jLjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPLUgsHWu4SCB11U\r\n"	\
	"GxlmqJ3pIzP4YWdH/cww2K+OjCS4KWy5Fb//KlZMxXjZeJ6iAd5pcOvcsE4w5CQM\r\n"	\
	"kRVxdqzLmLcVsh9hMW+dB+Aeeura8pav7TtlO3BOZxsQN/PWi8AX5g8rjO5xC9hs\r\n"	\
	"9kxR5MAXlUOlq9WJ2T8xdtxZSQHT5pnrHEVnjd72rZlC2rAS8vYTrMxQLWm2GK+2\r\n"	\
	"7jYhQ9jSKuvy/g5XSYI0OHlWFrzo1fDuR/Ma5aJZBzBRKOKAXl2uFweiEYVH1sZe\r\n"	\
	"iztZlhxtxHAfJHVhnEL/wf7GnaAClCoMd4amxBuiESxrP+DU/lwwNerBbB+wMxqr\r\n"	\
	"n+/Hb2MCAwEAAaNTMFEwHQYDVR0OBBYEFIfN1T+xxFvPuxcT8ROronOnTQDhMB8G\r\n"	\
	"A1UdIwQYMBaAFIfN1T+xxFvPuxcT8ROronOnTQDhMA8GA1UdEwEB/wQFMAMBAf8w\r\n"	\
	"DQYJKoZIhvcNAQELBQADggEBADpA6t61wwEqQ4yBXWk9sX5dW3NQpj/FigpWIUnf\r\n"	\
	"geQedXfrn/zZFOC7iA05uHdjRpP+Fp4ebJNxHOMbL7TCMPOG+SBYgbMv9ZgRDAYj\r\n"	\
	"Ca3Osm53EqeMi+26ka6xkBEHYZ+vVt1bwZOjwxzX56J6lNiKLvgQgn9EgeKpQker\r\n"	\
	"lV12T04/NgeIm38mfRqxueYG22YrLuo+dzijwzY2wwBGntwFviXWLGDYMGiBMPSp\r\n"	\
	"VtB5eptX+YUdO84E+86irjOIrsgqO1G7MYu8beuonIrjgmtApOtT7xPPDS39YO1J\r\n"	\
	"mQoyLhuU7nr8QZbiLTi+dSBlytJFvPkgjrgqR7WWkLPEj0w=\r\n"	\
    "-----END CERTIFICATE-----\r\n"

/* Populate here your device certificate content */
#define DEVICE_CERT                                                        \
    "-----BEGIN CERTIFICATE-----\n"                                        \
	"MIICyDCCAbACFFJnmg1mTQALnBB/w4LUQTQ+hckpMA0GCSqGSIb3DQEBCwUAMB8x\r\n" 	\
	"HTAbBgNVBAMMFEFuYWxvZyBEZXZpY2VzLCBJbmMuMB4XDTIzMDIyNzEwMTM1NloX\r\n"	\
	"DTIzMDMyOTEwMTM1NlowIjEgMB4GA1UEAwwXRW5lcmd5TW9uaXRvcmluZ0Rldmlj\r\n"	\
	"ZTEwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHjhmvEevCFkVm8g7D\r\n"	\
	"+9DJQHwt/pmWOg11EwmsmuaCt3V+j6KfujJRoqCUqlc0/vvqiaUKxlqKRq9Rdjnq\r\n"	\
	"YKcYvWOHT+pXpRlNPxbL/u9gL1t2PTb67UWYKQRYUaP+lH9UFJamKcVd6rDpJDbD\r\n"	\
	"lruzQUr542G5zlzeIsT8Bw9p2Qtk54bdenMK8bcGKcJTUSlLP++30lj1E5dg57XD\r\n"	\
	"uMiwz/dP3TC01/807K59vR3IwSCQUIbes1h3n4xTbgJrl1jx/G1FzGi/Oo5OBIQB\r\n"	\
	"3JFIs8nMMAElGutOAdznyLTzK0IzzvJylJeOJtlbafv2KB1CehNFkXVgkqnugZSX\r\n"	\
	"Vf4ZAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAAkI4/Bfo9bUUWmbzsfPRAACa1yr\r\n"	\
	"ZYhPm7gjB3UO2TNI1+kZq1mbptm4AQlRKHZOc4RwOKw2+gEc++FwK9v8Ai1EcVgW\r\n"	\
	"bCgmL1fWzxwmPCAatjWAIXIwtao4g3xPFOSw8Afg54upUNLzpLHyxyuByLOIs+TQ\r\n"	\
	"qCpjNXMsBblrU+7XOjo0Z5qxyWGxzFkI+/MfW9F5+q0dt77f6GxBmEAj42HIou32\r\n"	\
	"B8CohaPoJ8qJAWd9X+zKGzzstXEmmnlSX0T1H3jaL+ENRNzWpMT6ASt3u+Mq+WpY\r\n"	\
	"che9sgKqK4yPsqewrBSacSORc0aY+Htp1MwVWjnhKI/X+7eEV6CQ3Y7FYoc=\r\n"	\
    "-----END CERTIFICATE-----\r\n";

/* Populate here your device private key content */
#define DEVICE_PRIVATE_KEY                                                 \
    "-----BEGIN PRIVATE KEY-----\n"                                        \
	"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDHjhmvEevCFkVm\r\n"	\
	"8g7D+9DJQHwt/pmWOg11EwmsmuaCt3V+j6KfujJRoqCUqlc0/vvqiaUKxlqKRq9R\r\n"	\
	"djnqYKcYvWOHT+pXpRlNPxbL/u9gL1t2PTb67UWYKQRYUaP+lH9UFJamKcVd6rDp\r\n"	\
	"JDbDlruzQUr542G5zlzeIsT8Bw9p2Qtk54bdenMK8bcGKcJTUSlLP++30lj1E5dg\r\n"	\
	"57XDuMiwz/dP3TC01/807K59vR3IwSCQUIbes1h3n4xTbgJrl1jx/G1FzGi/Oo5O\r\n"	\
	"BIQB3JFIs8nMMAElGutOAdznyLTzK0IzzvJylJeOJtlbafv2KB1CehNFkXVgkqnu\r\n"	\
	"gZSXVf4ZAgMBAAECggEAb6Um5XXHSw0ewxvF+wwVoaL8VtdMomnUQZ3nGbSIJrXx\r\n"	\
	"fF9sAqUvpdCwurwakkHeOzfLKJ4U5avqRk8409JDamn7Fyc02tg5sagMXxFAZ7XX\r\n"	\
	"G+3fpr+84gaAsdDrSXFXU3k5V7mi/Ipjc+yY3xCj7wQmqGv4rvWvq3AUeVSR4Qta\r\n"	\
	"aq/yELOc6zkCuXoPVht6E5xsqhYE428MUVq3T8VJ8fIm410ymDRcroAeobhxzflO\r\n"	\
	"MOX84byL7kLt6QboOHoxUV/BBH8UQWQvZv8aP1gXvPp84jj+pAcf1MdF1sLR4yGa\r\n"	\
	"Mh5KkmcHi95SjvDS0B4Z3D2WgiN7IjeQ2tt0+TF+KQKBgQDqa9FmDEczE6aiJXCq\r\n"	\
	"XnP2J4e4V1WCHpNN2YUcogEWHtEN97XE3Sc7iqoYSCZW7cuR3nG8fqsiIcqf2hn7\r\n"	\
	"bfopUDqmNeV0xLCoDCXeuDnEkRrBBQXTPDoyekFH8KtqLvAFJApPBnX0TbU7bkzO\r\n"	\
	"gEqXLNJlK4MJRCSAWV2V+v8BiwKBgQDZ7KfypeaByTLwOB4WtmlwFZWq8EFN6Wwx\r\n"	\
	"5i3Eo8l5vhtkWws+DyJpBPpX+TAIpnYQ4PSMPNxsft2YFygIR5eBEoIEBtbDfiUv\r\n"	\
	"svlEH6+f+71wabu0ePkMTdEf8pl36YEspFq+0Iz+o53EiqGHb+kLTDc81CAQVBl2\r\n"	\
	"xaKQgDorawKBgQDlYLCZ2QPGL8FKQbZXjmqLfyynLRWnZ8GdWG2OkdrcSTUoJK1A\r\n"	\
	"v2FHOqyra9XQE4iw5+eEmLFdiZEaDzCDPJ6e1Dk/L9ehBWESXiikIMGt3IpAOmjz\r\n"	\
	"w6fygnvkJ9Oi5+DGNvi7UMgUUAE48PnIyfGysRICGqxyYbIRwN/5BIuHdwKBgCcI\r\n"	\
	"3/BzzP00Z95lfuY8mFhOVXe//0KQbCPoAgy19dHLvqZUNIhSN6yuCpWVeggioQVW\r\n"	\
	"9hbkk+sPMmwawb3x7O5evVExVGjCALExkrqkHlY+xmkLV2b1QE725V2em+TBu7Se\r\n"	\
	"X+7L9mVqM0lQN6zF2+19ImvP50pldgYzUnIltcWvAoGAcolYRYjTo3gjxwD9xhAq\r\n"	\
	"TLmCEugNmiYkCHgdOWmkHk+5AOJkctkwCGGu9+Bz1yJ46Afvv6uRO540QJKdaHBS\r\n"	\
	"np2l72ukSASAVaM1S+HyPrz9s1bCTIpB82kHKHuxx4SSSrLuxUkRLz7rZkRvsEai\r\n"	\
	"mKmY/gyPLJZRq4Lr1lOwoCM=\r\n"	\
    "-----END PRIVATE KEY-----\r\n";

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution.
*******************************************************************************/
int basic_example_main()
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	mxc_hpb_mem_config_t mem;
	mxc_hpb_mem_config_t mem2;
	mxc_hpb_cfg_reg_val_t cfg_reg[1];
	struct maxq1065_desc *maxq1065;
	struct lwip_network_desc *lwip_desc;
	uint8_t adin1110_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x60};
	uint32_t connect_timeout = 3000;

	char my_ca_cert[] = CA_CERT;
	char my_cli_cert[] = DEVICE_CERT;
	char my_cli_pk[] = DEVICE_PRIVATE_KEY;
	struct no_os_trng_init_param trng_ip = {
		.platform_ops = &max_trng_ops
	};

	struct secure_init_param sip = {
		.trng_init_param = &trng_ip,
		// .ca_cert = my_ca_cert,
		// .ca_cert_len = NO_OS_ARRAY_SIZE(my_ca_cert),
		// .cli_cert = my_cli_cert,
		// .cli_cert_len = NO_OS_ARRAY_SIZE(my_cli_cert),
		// .cli_pk = my_cli_pk,
		// .cli_pk_len = NO_OS_ARRAY_SIZE(my_cli_pk),
		.cert_verify_mode = MBEDTLS_SSL_VERIFY_NONE
	};

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	printf("UART test: PASSED\n");

	ret = maxq1065_init(&maxq1065, &maxq1065_ip);
	if (ret)
		return ret;

	// printf("MAXQ1065 ping: PASSED\n");

	cfg_reg[0].addr = 0x01000;
	cfg_reg[0].val  = 0x801f;
	mem.base_addr = (unsigned int)&__hpb_cs0_start;
	mem.device_type     = MXC_HPB_DEV_HYPER_RAM;
	mem.cfg_reg_val     = cfg_reg;
	mem.cfg_reg_val_len = 1;
	mem.read_cs_high    = MXC_HPB_CS_HIGH_10_5;
	mem.write_cs_high   = MXC_HPB_CS_HIGH_10_5;
	mem.read_cs_setup   = MXC_HPB_CS_SETUP_HOLD_16;
	mem.write_cs_setup  = MXC_HPB_CS_SETUP_HOLD_14;
	mem.read_cs_hold    = MXC_HPB_CS_SETUP_HOLD_5;
	mem.write_cs_hold   = MXC_HPB_CS_SETUP_HOLD_12;
	mem.latency_cycle   = MXC_V_HPB_MEMTIM_LAT_6CLK;
	mem.fixed_latency   = 0;

	mem2.base_addr = (unsigned int)&__hpb_cs1_start;
	mem2.device_type     = MXC_HPB_DEV_HYPER_RAM;
	mem2.cfg_reg_val     = cfg_reg;
	mem2.cfg_reg_val_len = 1;
	mem2.read_cs_high    = MXC_HPB_CS_HIGH_10_5;
	mem2.write_cs_high   = MXC_HPB_CS_HIGH_10_5;
	mem2.read_cs_setup   = MXC_HPB_CS_SETUP_HOLD_16;
	mem2.write_cs_setup  = MXC_HPB_CS_SETUP_HOLD_14;
	mem2.read_cs_hold    = MXC_HPB_CS_SETUP_HOLD_5;
	mem2.write_cs_hold   = MXC_HPB_CS_SETUP_HOLD_12;
	mem2.latency_cycle   = MXC_V_HPB_MEMTIM_LAT_6CLK;
	mem2.fixed_latency   = 0;

	ret = MXC_HPB_Init(&mem, &mem2);
	memcpy(&__hpb_cs0_start, &__load_start_hpb_cs0,
	       (uint32_t)&__load_length_hpb_cs0);
	memcpy(&__hpb_cs1_start, &__load_start_hpb_cs1,
	       (uint32_t)&__load_length_hpb_cs1);
	void (*func_ram1)(void);
	void (*func_ram2)(void);

	func_ram1 = (void(*)(void))((uint32_t)&__hpb_cs0_start | 1);
	func_ram2 = (void(*)(void))((uint32_t)&__hpb_cs1_start | 1);
	func_ram1();
	func_ram2();

	Ext_Flash_Config_t exf_cfg = {.init  = ext_flash_board_init,
				      .read  = ext_flash_board_read,
				      .write = ext_flash_board_write,
				      .clock = ext_flash_clock
				     };

	ret = Ext_Flash_Configure(&exf_cfg);
	if (ret)
		return ret;

	Ext_Flash_Init();
	MXC_GPIO_Init(MXC_GPIO_PORT_0);
	MXC_GPIO_Config(&gpio_cfg_spixf);
	MXC_SPIXF_Enable();

	Ext_Flash_Reset();
	uint32_t flash_id = Ext_Flash_ID();
	Ext_Flash_Erase(0x00000, Ext_Flash_Erase_64K);
	ret = Ext_Flash_Quad(1);
	if (ret)
		return ret;

	ret = Ext_Flash_Program_Page(0x0, &__load_start_xip,
				(uint32_t)(&__load_length_xip), Ext_Flash_DataLine_Single);

	spixf_cfg_setup();

	func_ram1 = (void(*)(void))((uint32_t)&__load_start_xip | 1);
	func_ram1();

	printf("RAM chip 1: ");
	if (ram1_test)
		printf("PASSED \n");
	else
		printf("FAILED \n");

	printf("RAM chip 2: ");
	if (ram2_test)
		printf("PASSED \n");
	else
		printf("FAILED \n");

	printf("Flash chip: ");
	if (flash_test)
		printf("PASSED \n");
	else
		printf("FAILED \n");

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	memcpy(lwip_ip.hwaddr, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);

	uint8_t send_buff[1000] = {0};
 	uint8_t read_buff[1000] = {0};

	ret = no_os_lwip_init(&lwip_desc, &lwip_ip);
	if (ret)
		return ret;

	uint32_t i = 0;
	struct tcp_socket_desc *tcp_socket;
	struct connection *client_socket;
	bool connected = false;
	struct tcp_socket_init_param tcp_ip = {
		.net = &lwip_desc->no_os_net,
		.max_buff_size = 0,
		.secure_init_param = &sip,
	};

	ret = socket_init(&tcp_socket, &tcp_ip);
	if (ret)
		return ret;

	struct socket_address ip_addr = {
		.addr = "169.254.97.30",
		.port = 4433
	};

	struct mqtt_desc *mqtt;
  	struct mqtt_init_param	mqtt_init_param = {
  		.timer_init_param = &adc_demo_tip,
		.sock = tcp_socket,
  		.command_timeout_ms = 20000,
  		.send_buff = send_buff,
  		.read_buff = read_buff,
  		.send_buff_size = 700,
  		.read_buff_size = 700,
  		.message_handler = message_handler
  	};

	ret = mqtt_init(&mqtt, &mqtt_init_param);

 	struct mqtt_connect_config conn_config = {
  		.version = MQTT_VERSION_3_1_1,
  		.keep_alive_ms = 72000,
  		.client_name = (int8_t *)"maxim",
  		.username = "testuser",
  		.password = NULL
  	};

	ret = socket_connect(tcp_socket, &ip_addr);
	// if (ret)
	// 	return ret;

	// while (1) {
	// 	ret = socket_connect(tcp_socket, &ip_addr);
	// 	no_os_lwip_step(tcp_socket->net->net, NULL);
	// }

	while (connect_timeout--) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		no_os_mdelay(1);
	}

	ret = secure_socket_connect(tcp_socket, &ip_addr);
	// if (ret)
	// 	return ret;

	while (connect_timeout--) {
		no_os_lwip_step(tcp_socket->net->net, NULL);
		no_os_mdelay(1);
	}

	socket_send(tcp_socket, "test message", 12);

	while(1);

  	ret = mqtt_connect(mqtt, &conn_config, NULL);
	if (ret) {
		socket_disconnect(tcp_socket);
		// return 0;
	}

	struct mqtt_message test_msg = {
		.qos = 0,
		.len = 4,
		.payload = "abcd",
		.retained = false
	};

	mqtt_publish(mqtt, "test", &test_msg);
	mqtt_publish(mqtt, "test", &test_msg);
	mqtt_publish(mqtt, "test", &test_msg);


	// ret = socket_bind(tcp_socket, 10000);
	// if (ret)
	// 	return ret;

	// ret = socket_listen(tcp_socket, MAX_BACKLOG);
	// if (ret)
	// 	return ret;

	while(1) {
		// ret = socket_accept(tcp_socket, &client_socket);
		// if (ret && ret != -EAGAIN)
		// 	return ret;

		// if (client_socket) {
		// 	connected = true;
		// }

		no_os_lwip_step(tcp_socket->net->net, NULL);

		// if (connected) {
		// 	ret = socket_recv(client_socket, read_buff, 1);
		// 	if (ret > 0)
		// 		socket_send(client_socket, read_buff, ret);
		// }
	}

	return 0;
}
