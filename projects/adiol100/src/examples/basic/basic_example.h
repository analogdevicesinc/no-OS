#ifndef __BASIC_EXAMPLE_H__
#define __BASIC_EXAMPLE_H__

#include <stdint.h>

/* IO-Link protocol defines */
#define IOL_READ			1
#define IOL_WRITE			0

#define IOL_CH_PROCESS			0
#define IOL_CH_PAGE			1
#define IOL_CH_DIAG			2
#define IOL_CH_ISDU			3

#define IOL_MC(rw, ch, addr)		(((rw) << 7) | ((ch) << 5) | (addr))

#define IOL_ISDU_START			0x10
#define IOL_ISDU_IDLE			0x11

#define IOL_DPP_MASTERCMD		0x00
#define IOL_DPP_CYCLETIME		0x01
#define IOL_DPP_MINCYCLETIME		0x02
#define IOL_DPP_MSEQCAP		0x03
#define IOL_DPP_PDIN			0x05
#define IOL_DPP_PDOUT			0x06

#define IOL_CMD_MASTERIDENT		0x95
#define IOL_CMD_PREOPERATE		0x9A
#define IOL_CMD_OPERATE			0x99

/* Standard ISDU indices (Table B.8) */
#define IOL_ISDU_VENDORNAME		16
#define IOL_ISDU_VENDORTEXT		17
#define IOL_ISDU_PRODUCTNAME		18
#define IOL_ISDU_PRODUCTID		19
#define IOL_ISDU_PRODUCTTEXT		20
#define IOL_ISDU_SERIALNUMBER		21

/* CKT type bits (7:6) */
#define IOL_CKT_TYPE0			0x00
#define IOL_CKT_TYPE1			0x40
#define IOL_CKT_TYPE2			0x80

static inline int iol_decode_pd_len(uint8_t pd_descriptor)
{
	int len = pd_descriptor & 0x1F;
	if (pd_descriptor & 0x80)
		len++;
	return len;
}

static inline int iol_decode_preop_od(uint8_t mseq_cap)
{
	static const int table[] = { 1, 2, 8, 32 };
	return table[(mseq_cap >> 4) & 0x03];
}

static inline int iol_decode_operate_od(uint8_t mseq_cap)
{
	static const int table[] = { 1, 2, 1, 1, 1, 2, 8, 32 };
	return table[(mseq_cap >> 1) & 0x07];
}

int basic_example_main(void);

#endif
