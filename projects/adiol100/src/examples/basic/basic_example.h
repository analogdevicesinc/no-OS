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

/* FlowCTRL definitions (Table 52) */
#define IOL_ISDU_START			0x10
#define IOL_ISDU_IDLE			0x11

/* Direct Parameter Page 1 addresses (Table B.1) */
enum iol_dpp {
	IOL_DPP_MASTERCMD      = 0x00,
	IOL_DPP_CYCLETIME      = 0x01,
	IOL_DPP_MINCYCLETIME   = 0x02,
	IOL_DPP_MSEQCAP        = 0x03,
	IOL_DPP_REVISIONID     = 0x04,
	IOL_DPP_PDIN           = 0x05,
	IOL_DPP_PDOUT          = 0x06,
	IOL_DPP_VENDORID_MSB   = 0x07,
	IOL_DPP_VENDORID_LSB   = 0x08,
	IOL_DPP_DEVICEID_B1    = 0x09,
	IOL_DPP_DEVICEID_B2    = 0x0A,
	IOL_DPP_DEVICEID_B3    = 0x0B,
	IOL_DPP_FUNCTIONID_MSB = 0x0C,
	IOL_DPP_FUNCTIONID_LSB = 0x0D,
};

#define IOL_CMD_MASTERIDENT		0x95
#define IOL_CMD_PREOPERATE		0x9A
#define IOL_CMD_OPERATE			0x99

/* Standard ISDU indices (Table B.8) */
enum iol_isdu_index {
	IOL_ISDU_VENDORNAME    = 16,
	IOL_ISDU_VENDORTEXT    = 17,
	IOL_ISDU_PRODUCTNAME   = 18,
	IOL_ISDU_PRODUCTID     = 19,
	IOL_ISDU_PRODUCTTEXT   = 20,
	IOL_ISDU_SERIALNUMBER  = 21,
};

/* CKT type bits (7:6) */
enum iol_ckt {
	IOL_CKT_TYPE0 = 0x00,
	IOL_CKT_TYPE1 = 0x40,
	IOL_CKT_TYPE2 = 0x80,
};

/* ISDU read request service (Table A.13) */
#define IOL_ISDU_READ_REQ		0x93
#define IOL_ISDU_READ_CMD(idx)		{ IOL_ISDU_READ_REQ, (idx), IOL_ISDU_READ_REQ ^ (idx) }

/* ISDU response parsing (Tables A.13 and A.14) */
#define IOL_ISDU_BUSY			0x01
#define IOL_ISDU_ISERVICE_MSK		0xF0
#define IOL_ISDU_READ_ERROR		0xC0
#define IOL_ISDU_LEN_MSK		0x0F
#define IOL_ISDU_LONG_FORM		0x01
#define IOL_ISDU_LONG_HDR_OVERHEAD	3
#define IOL_ISDU_SHORT_HDR_OVERHEAD	2

/* Buffer sizes */
#define IOL_MAX_OD			32
#define IOL_MAX_FRAME			(2 + IOL_MAX_OD)
#define IOL_MAX_ISDU_DATA		232

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
