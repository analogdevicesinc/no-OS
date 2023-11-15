#include <stdint.h>
#include <stdlib.h>

#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "maxq1065.h"
#include "maxq_perso.h"

extern mxq_u1 KeyPairAdmin[];
static struct maxq1065_desc *maxq1065_local_desc;

mxq_err_t maxq1065_reset(void)
{
	no_os_gpio_set_value(maxq1065_local_desc->reset_gpio, NO_OS_GPIO_LOW);
	no_os_mdelay(350);
	no_os_gpio_set_value(maxq1065_local_desc->reset_gpio, NO_OS_GPIO_HIGH);
	no_os_mdelay(350);

	return MXQ_OK;
}

// void MXQ_PRINTBUFFER(const unsigned char * buffer,unsigned int len)
// {
// 	unsigned int i;

// 	for(i=0;i<len;i++){
// 		printf("%02x ",buffer[i]);
// 	}
// }

void HardFault_Handler(void) {
	int a = 3;
	a++;
}

mxq_err_t maxq_authAdmin(uint16_t key_id, unsigned char * keypair)
{
	mxq_err_t err;

	mxq_u1 prng[16];
	static mxq_u1 signature[256];

	printf("[MAXQ10xx Perso] Admin Authentication ....\n");

	err = MXQ_Get_Challenge(prng, sizeof(prng));
	if(err != MXQ_OK) {
		printf("Error %s:%d\n",__FILE__, __LINE__);
		return err;
	}

	/* Locally sign the random number with the admin private key */
	err = signData(signature, keypair, prng, (mxq_length)sizeof(prng));
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return err;
	}

	/* Do the authentication */
	err = MXQ_AdminAuth(key_id, ALGO_ECDSA_SHA_256, signature, 32 * 2);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n",__FILE__, __LINE__);
		return err;
	}

	return MXQ_OK;
}

mxq_err_t maxq_import_psk(uint16_t objectid, uint8_t * psk, size_t psk_length ){

	mxq_err_t err;
	static mxq_u1 keydata[290];
	mxq_length keydatalen;

	mxq_u1 signature[64];
	mxq_length signlen = 32*2;

	err = maxq_authAdmin(PUBKEYADMINID, KeyPairAdmin);
	if(err != MXQ_OK) {
    	printf("Error %s:%d\n", __FILE__, __LINE__);
        return -1;
    }

	printf("Creating PSK in MAXQ...\n");
	MXQ_DeleteObject(objectid);

    err = MXQ_CreateObject(objectid, 96, MXQ_OBJTYPE_SECRETKEY, OBJPROP_PERSISTENT, "a=d,x=wx");
    if(err != MXQ_OK) {
    	printf("Error %s:%d\n", __FILE__, __LINE__);
        return -1;
    }

    keydatalen = sizeof(keydata);

	err = MXQ_BuildKey(keydata, &keydatalen,
			MXQ_KEYTYPE_PSK, MXQ_UNKNOWN_CURVE,
			psk_length, psk_length,
			MXQ_KEYUSE_TLSAUTH, ALGO_CIPHER_AES_any,
			(mxq_keyuse_t)0, ALGO_NONE,
			psk);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	err = signData(signature, KeyPairImport, keydata, keydatalen);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	err = MXQ_ImportKey(objectid, ALGO_ECDSA_SHA_256, PUBKEYIMPORTOBJID, keydata, keydatalen, signature, signlen);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	return MXQ_OK;
}

int maxq1065_init(struct maxq1065_desc **desc, struct maxq1065_init_param *param)
{
	struct maxq1065_desc *descriptor;
	uint8_t rdy_val;
	mxq_status_t s;
	mxq_err_t r;
	int ret;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if  (!descriptor)
		return -ENOMEM; 

	maxq1065_local_desc = descriptor;

	ret = no_os_spi_init(&descriptor->comm_desc, &param->comm_param);
	if (ret)
		goto free_descriptor;

	// ret = no_os_gpio_get_optional(&descriptor->rdy_gpio, &param->rdy_gpio_param);
	// if (ret)
	// 	goto free_spi;
	
	// if (descriptor->rdy_gpio) {
	// 	ret = no_os_gpio_direction_input(descriptor->rdy_gpio);
	// 	if (ret)
	// 		goto free_spi;
	// }

	ret = no_os_gpio_get(&descriptor->reset_gpio, &param->reset_gpio_param);
	if (ret)
		goto free_spi;

	ret = no_os_gpio_direction_output(descriptor->reset_gpio, NO_OS_GPIO_LOW);
	if (ret)
		goto free_reset;

	maxq1065_reset();

	ret = no_os_gpio_get_value(descriptor->rdy_gpio, &rdy_val);
	if (ret)
		return ret;

	HOST_set_interface(&maxq1065_ops);

	ret = MXQ_Module_Init();
	if (ret)
		return ret;

	MXQ_TLS_Enable();

	r = MXQ_Ping(1);
	if (r != MXQ_OK)
		return r;

	// r = MXQ_GetStatus(&s);
	// if (r != MXQ_OK)
	// 	return r;

	// MXQ_DisplayStatus(&s);
	// MXQ_DisplayConfig(&s);

	*desc = descriptor;

	return 0;


free_reset:
	no_os_gpio_remove(descriptor->reset_gpio);
free_spi:
	no_os_spi_remove(descriptor->comm_desc);
free_descriptor:
	free(descriptor);

	return ret;
}

int maxq1065_remove(struct maxq1065_desc *desc)
{
	free(desc);
}

mxq_err_t maxq1065_init_spi(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_init_serial(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest)
{
	uint8_t buff;
	uint32_t crc;
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = len,
		.cs_change = 1,
	};
	uint8_t rdy;
	int ret;

	memset(maxq1065_local_desc->rx_buff, 0, 512);
	memset(maxq1065_local_desc->tx_buff, 0, 512);
	memcpy(xfer.tx_buff, src, len);

	// for (int i = 0; i < len; i++) {
	// 	buff = no_os_bit_swap_constant_8(xfer.tx_buff[i]);
	// 	xfer.tx_buff[i] = buff;
	// }

	// no_os_swap(xfer.tx_buff[len - 1], xfer.tx_buff[len - 2]);

	ret = no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	memcpy(dest, maxq1065_local_desc->rx_buff, len);

	// for (int i = 0; i < len; i++)
	// 	dest[i] = no_os_bit_swap_constant_8(maxq1065_local_desc->rx_buff[i]);

	return len;
}

mxq_err_t maxq1065_send_bytes_spi(const mxq_u1* src, mxq_length len)
{
	uint8_t rx[len];
	int ret;

	ret = maxq1065_exchange_bytes_spi(src, len, rx);

	return ret;
}

mxq_err_t maxq1065_receive_bytes_spi(mxq_u1* dest, mxq_length len)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = len,
		.cs_change = 1,
	};
	int ret;

	memset(maxq1065_local_desc->rx_buff, 0xCC, 512);
	memset(maxq1065_local_desc->tx_buff, 0, 512);
	ret = no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	// for (int i = 0; i < len; i++)
	// 	dest[i] = no_os_bit_swap_constant_8(maxq1065_local_desc->rx_buff[i]);

	memcpy(dest, maxq1065_local_desc->rx_buff, len);

	return len;
}

mxq_err_t maxq1065_first_bytes_spi(mxq_u1* dest, mxq_length len)
{
	uint8_t rx[10] = {0};
	struct no_os_spi_msg xfer = {
		.tx_buff = maxq1065_local_desc->tx_buff,
		.rx_buff = maxq1065_local_desc->rx_buff,
		.bytes_number = 1,
		.cs_change = 1,
	};
	uint32_t timeout = 1000;
	mxq_u1 dummy = 0xCC;
	uint8_t rdy = 0;
	int ret;

	memset(maxq1065_local_desc->tx_buff, 0xCC, 512);
	memset(maxq1065_local_desc->rx_buff, 0, 512);
	while (maxq1065_local_desc->rx_buff[0] != 0x55 && timeout) {
		no_os_spi_transfer(maxq1065_local_desc->comm_desc, &xfer, 1);
		timeout--;
		no_os_mdelay(1);
	}

	if (!timeout)
		return -ETIMEDOUT;

	dest[0] = 0x55;

	ret = maxq1065_receive_bytes_spi(&dest[1], len - 1);
	if (ret < 0)
		return ret;

	return len;
}

mxq_err_t maxq1065_activate_tls(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_activate_gcu(void)
{
	return MXQ_OK;
}

mxq_err_t maxq1065_sleep(mxq_u4 us)
{
	no_os_udelay((uint32_t)us);

	return MXQ_OK;
}

// int maxq1065_admin_auth()
// {
// 	mxq_u1 prng[16];
// 	mxq_u1 signature[64];

// 	/**
// 	 *  Get a Random Number from the MAXQ106x to perform the challenge response authentication
// 	 */
// 	ASSERTSUCCESS(MXQ_Get_Challenge(prng, sizeof(prng)));

// 	/**
// 	 *  Locally sign the random number with a valid administrator private key.
// 	 *  Here we use the default Maxim Import key 'KeyPairAdmin' with the public part already loaded in the MAXQ106x
// 	 * and fully available in the source file "host_keys.c"
// 	 */
// 	ASSERTSUCCESS(ECDSA_sign_secp256r1_sha256(signature, KeyPairAdmin, prng, sizeof(prng)));

// 	/**
// 	 * Finally to perform the authentication by the sending back the random number along with the signature
// 	 */
// 	ASSERTSUCCESS(MXQ_AdminAuth(PUBKEYADMINID, ALGO_ECDSA_SHA_256, signature, KEYCOMPLEN * 2));

// 	/**
// 	 * We are now authenticated as an Administrator
// 	 */
// 	return 0;
// }

const struct maxq1065_no_os_ops maxq1065_ops = {
	.init_spi = maxq1065_init_spi,
	.init_serial = maxq1065_init_serial,
	.send_bytes_spi = maxq1065_send_bytes_spi,
	.exchange_bytes_spi = maxq1065_exchange_bytes_spi,
	.receive_bytes_spi = maxq1065_receive_bytes_spi,
	.receive_bytes_spi_first = maxq1065_first_bytes_spi,
	.reset = maxq1065_reset,
	.activate_tls = maxq1065_activate_tls,
	.activate_gcu = maxq1065_activate_gcu,
	.sleep_us = maxq1065_sleep,
};
