#include <stdint.h>
#include <stdlib.h>

#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "maxq1065.h"
#include "maxq_perso.h"
#include "maxim_trng.h"
#include "no_os_trng.h"

unsigned char local_psk[] = {
        0x0d, 0xdd, 0x9f, 0x07, 0x9f, 0xc0, 0xd8, 0xdc, 0x6f, 0x91, 0x9f, 0x95, 0xff, 0x3c, 0x94, 0x19,
        0x93, 0x8e, 0x45, 0xc0, 0x58, 0x96, 0xf4, 0x67, 0x2b, 0xbc, 0x30, 0x5a, 0x38, 0x5c, 0x69, 0x9d
    };

mxq_u1 KeyPairAdmin[]=
		{0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,
		0xab,0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,
		0x2d,0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,
		0x95,0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,
		0xfd,0xe5,0xde,0x8f,0x48,

		0x19,0xe8,0xc6,0x4f,0xf2,0x46,0x10,0xe2,0x58,0xb9,0xb6,0x72,0x5e,0x88,0xaf,
		0xc2,0xee,0x8b,0x6f,0xe5,0x36,0xe3,0x60,0x7c,0xf8,0x2c,0xea,0x3a,0x4f,0xe3,
		0x6d,0x73};


mxq_u1 KeyPairImport[]=
		{0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,
		0xab,0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,
		0x2d,0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,
		0x95,0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,
		0xfd,0xe5,0xde,0x8f,0x48,

		0x19,0xe8,0xc6,0x4f,0xf2,0x46,0x10,0xe2,0x58,0xb9,0xb6,0x72,0x5e,0x88,0xaf,
		0xc2,0xee,0x8b,0x6f,0xe5,0x36,0xe3,0x60,0x7c,0xf8,0x2c,0xea,0x3a,0x4f,0xe3,
		0x6d,0x73};

extern int is_module_init;
static struct no_os_trng_init_param trng_param = {
	.dev_id = 0,
	.extra = NULL,
	.platform_ops = &max_trng_ops,
};

static struct no_os_trng_desc *trng_desc;
static struct maxq1065_desc *maxq1065_local_desc;

uint8_t psk_key[] = {0x31, 0x32};

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

static int rnd_std_rand(void *rng_state, unsigned char *output, size_t len)
{
	return no_os_trng_fill_buffer(trng_desc, output, len);
}

mxq_err_t signData(mxq_u1* dest, mxq_u1* key, mxq_u1* data, mxq_length data_length)
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d, r, s;
    unsigned char hash[32];

    mbedtls_ecp_group_init(&grp);
 	mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
	mbedtls_mpi_init(&d);
	mbedtls_mpi_init(&r);
	mbedtls_mpi_init(&s);


	mbedtls_mpi_read_binary(&d, key + 64,32);
	mbedtls_sha256(data, data_length, hash, 0);
	mbedtls_ecdsa_sign(&grp, &r, &s, &d, hash, 32, rnd_std_rand, 0);
	mbedtls_mpi_write_binary(&r, dest, 32);
	mbedtls_mpi_write_binary(&s, dest + 32, 32);

	return MXQ_OK;
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
	uint8_t key_data[64] = {0};
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

	err = MXQ_CreateObject(objectid, 96, MXQ_OBJTYPE_SECRETKEY, OBJPROP_PERSISTENT, "ahs=wdx:ahs=wdx:ahs=wdx");
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
        	return -1;
	}

	keydatalen = sizeof(key_data);

	err = MXQ_BuildKey(key_data, &keydatalen,
			MXQ_KEYTYPE_PSK, MXQ_UNKNOWN_CURVE,
			psk_length, psk_length,
			MXQ_KEYUSE_TLSAUTH, ALGO_CIPHER_AES_any,
			(mxq_keyuse_t)0, ALGO_NONE,
			local_psk);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	err = signData(signature, KeyPairImport, key_data, keydatalen);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	err = MXQ_ImportKey(objectid, ALGO_ECDSA_SHA_256, PUBKEYIMPORTOBJID, key_data, keydatalen, signature, 64);
	if(err != MXQ_OK) {
		printf("Error %s:%d\n", __FILE__, __LINE__);
		return -1;
	}

	return MXQ_OK;
}

int maxq1065_init(struct maxq1065_desc **desc, struct maxq1065_init_param *param)
{
	struct maxq1065_desc *descriptor;
	uint8_t maxq1065_objects[200] = {0};
	mxq_status_t maxq_status;
	uint32_t max_len = 200;
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

	// is_module_init = 1;

	// ret = MXQ_GetStatus(&maxq_status);
	// if (ret)
	// 	return ret;

	// MXQ_DisplayStatus(&maxq_status);

	ret = MXQ_ListObject(0, 0x3000, maxq1065_objects, &max_len);

	no_os_trng_init(&trng_desc, &trng_param);
	ret = maxq_import_psk(0x3000, local_psk, sizeof(local_psk));
	if (ret)
		return ret;

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
// 	int ret;

// 	/**
// 	 *  Get a Random Number from the MAXQ106x to perform the challenge response authentication
// 	 */
// 	ret = MXQ_Get_Challenge(prng, sizeof(prng));
// 	if (ret)
// 		return ret;

// 	/**
// 	 *  Locally sign the random number with a valid administrator private key.
// 	 *  Here we use the default Maxim Import key 'KeyPairAdmin' with the public part already loaded in the MAXQ106x
// 	 * and fully available in the source file "host_keys.c"
// 	 */
// 	ret = ECDSA_sign_secp256r1_sha256(signature, KeyPairAdmin, prng, sizeof(prng));
// 	if (ret)
// 		return ret;

// 	/**
// 	 * Finally to perform the authentication by the sending back the random number along with the signature
// 	 */
// 	ret = MXQ_AdminAuth(PUBKEYADMINID, ALGO_ECDSA_SHA_256, signature, 32 * 2);
// 	if (ret)
// 		return ret;

// 	/**
// 	 * We are now authenticated as an Administrator
// 	 */
// 	return 0;
// }

static int maxq1065_trng_init(struct no_os_trng_desc **desc,
			      const struct no_os_trng_init_param *param)
{
	*desc = no_os_calloc(1, sizeof(**desc));
	if (!*desc)
		return -ENOMEM;

	return 0;
}

static int maxq1065_trng_remove(struct no_os_trng_desc *desc)
{
	no_os_free(desc);

	return 0;
}

static int maxq1065_trng_fill_buffer(struct no_os_trng_desc *desc, uint8_t *buff, uint32_t len)
{
	return MXQ_Get_Random(buff, len);
}

const struct no_os_trng_platform_ops maxq1065_trng_ops = {
	.init = maxq1065_trng_init,
	.fill_buffer = maxq1065_trng_fill_buffer,
	.remove = maxq1065_trng_remove,
};

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
