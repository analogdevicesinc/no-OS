#ifndef _SPI_OPEN_ALLIANCE_H
#define _SPI_OPEN_ALLIANCE_H

#include "adin1110.h"

#define OA_CHUNK_MAX_LEN	8
#define OA_REG_LEN		4
#define OA_HEADER_LEN		4

#define OA_CTRL_HDRB_MASK	NO_OS_BIT(30)
#define OA_CTRL_WNR_MASK	NO_OS_BIT(29)
#define OA_CTRL_AID_MASK	NO_OS_BIT(28)
#define OA_CTRL_MMS_MASK	NO_OS_GENMASK(27, 24)
#define OA_CTRL_ADDR_MASK	NO_OS_GENMASK(23, 8)
#define OA_CTRL_LEN_MASK	NO_OS_GENMASK(7, 1)
#define OA_CTRL_P_MASK		NO_OS_BIT(0)

struct oa_data_header {
	uint8_t dnc: 1;
	uint8_t seq: 1;
	uint8_t norx: 1;
	uint8_t vs: 2;
	uint8_t dv: 1;
	uint8_t sv: 1;
	uint8_t swo: 4;
	uint8_t ev: 1;
	uint8_t ebo: 6;
	uint8_t tsc: 2;
	uint8_t p: 1;
};

struct oa_data_footer {
	uint8_t exts: 1;
	uint8_t hdrb: 1;
	uint8_t sync: 1;
	uint8_t rca: 5;
	uint8_t vs: 2;
	uint8_t dv: 1;
	uint8_t sv: 1;
	uint8_t swo: 3;
	uint8_t fd: 1;
	uint8_t ev: 1;
	uint8_t ebo: 6;
	uint8_t rtsa: 1;
	uint8_t rtsp: 1;
	uint8_t txc: 5;
	uint8_t p: 1;
};

struct oa_ctrl_header {
	uint8_t hdrb;
	uint8_t wnr;
	uint8_t aid;
	uint8_t mms;
	uint16_t addr;
	uint8_t len;
	uint8_t p;
};

struct oa_ctrl_footer {
	uint8_t dnc: 1;
	uint8_t seq: 1;
	uint8_t norx: 1;
	uint8_t rsvd1: 5;
	uint8_t vs: 2;
	uint8_t dv: 1;
	uint8_t sv: 1;
	uint8_t swo: 4;
	uint8_t rsvd2: 1;
	uint8_t ev: 1;
	uint8_t ebo: 6;
	uint8_t tsc: 2;
	uint8_t rsvd3: 5;
	uint8_t p: 1;
};

struct oa_tx_data_chunk {
	struct oa_data_header header;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_rx_data_chunk {
	struct oa_data_footer footer;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_tx_ctrl_chunk {
	struct oa_ctrl_header header;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_rx_ctrl_chunk {
	struct oa_ctrl_footer footer;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

int oa_reg_read(struct adin1110_desc *, uint32_t, uint32_t *);
int oa_reg_write(struct adin1110_desc *, uint32_t, uint32_t);
int oa_irq_start(struct adin1110_desc *);
int oa_irq_stop(struct adin1110_desc *);
int oa_init(struct adin1110_desc *, struct adin1110_init_param *);

#endif /* _SPI_OPEN_ALLIANCE_H */
