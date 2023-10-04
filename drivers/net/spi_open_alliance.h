#ifndef _SPI_OPEN_ALLIANCE_H
#define _SPI_OPEN_ALLIANCE_H

#include "adin1110.h"
#include <stdint.h>

#define OA_SPI_BUFF_LEN		1530
#define OA_CHUNK_MAX_LEN	8
#define OA_REG_LEN		4
#define OA_HEADER_LEN		4
#define OA_FOOTER_LEN		4

#define OA_CTRL_HDRB_MASK	NO_OS_BIT(30)
#define OA_CTRL_WNR_MASK	NO_OS_BIT(29)
#define OA_CTRL_AID_MASK	NO_OS_BIT(28)
#define OA_CTRL_MMS_MASK	NO_OS_GENMASK(27, 24)
#define OA_CTRL_ADDR_MASK	NO_OS_GENMASK(23, 8)
#define OA_CTRL_LEN_MASK	NO_OS_GENMASK(7, 1)
#define OA_CTRL_P_MASK		NO_OS_BIT(0)

enum oa_chunk_size {
	OA_CHUNK_8BYTES = 3,
	OA_CHUNK_16BYTES = 4,
	OA_CHUNK_32BYTES = 5,
	OA_CHUNK_64BYTES = 6,
};

enum oa_txc_threshold {
	OA_TX_THRESHOLD_1,
	OA_TX_THRESHOLD_4,
	OA_TX_THRESHOLD_8,
	OA_TX_THRESHOLD_16,
};

struct oa_spi_buffer {
	uint8_t *buff;
	uint32_t len;
};

struct oa_list_element {
	uint8_t *data;
	uint32_t length;
	struct oa_list_element *next;
};

struct oa_data_header_bits {
	uint8_t p: 1;
	uint8_t rsvd3: 5;
	uint8_t tsc: 2;
	uint8_t ebo: 6;
	uint8_t ev: 1;
	uint8_t rsvd2: 1;
	uint8_t swo: 4;
	uint8_t sv: 1;
	uint8_t dv: 1;
	uint8_t vs: 2;
	uint8_t rsvd1: 5;
	uint8_t norx: 1;
	uint8_t seq: 1;
	uint8_t dnc: 1;
};

union oa_data_header {
	struct oa_data_header_bits bits;
	uint32_t val;
};

struct oa_data_footer_bits {
	uint8_t p: 1;
	uint8_t txc: 5;
	uint8_t rtsp: 1;
	uint8_t rtsa: 1;
	uint8_t ebo: 6;
	uint8_t ev: 1;
	uint8_t fd: 1;
	uint8_t swo: 3;
	uint8_t sv: 1;
	uint8_t dv: 1;
	uint8_t vs: 2;
	uint8_t rca: 5;
	uint8_t sync: 1;
	uint8_t hdrb: 1;
	uint8_t exts: 1;
};

union oa_data_footer {
	struct oa_data_footer_bits bits;
	uint32_t val;
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

struct oa_ctrl_footer_bits {
	uint8_t p: 1;
	uint8_t rsvd3: 5;
	uint8_t tsc: 2;
	uint8_t ebo: 6;
	uint8_t ev: 1;
	uint8_t rsvd2: 1;
	uint8_t swo: 4;
	uint8_t sv: 1;
	uint8_t dv: 1;
	uint8_t vs: 2;
	uint8_t rsvd1: 5;
	uint8_t norx: 1;
	uint8_t seq: 1;
	uint8_t dnc: 1;
};

union oa_ctrl_footer {
	struct oa_ctrl_footer_bits bits;
	uint32_t val;
};

struct oa_tx_data_chunk {
	// struct oa_data_header header;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_rx_data_chunk {
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_tx_ctrl_chunk {
	struct oa_ctrl_header header;
	uint8_t data[OA_CHUNK_MAX_LEN];
};

struct oa_rx_ctrl_chunk {
	uint8_t data[OA_CHUNK_MAX_LEN];
};

int oa_reg_read(struct adin1110_desc *, uint32_t, uint32_t *);
int oa_reg_write(struct adin1110_desc *, uint32_t, uint32_t);
int oa_irq_start(struct adin1110_desc *);
int oa_irq_stop(struct adin1110_desc *);
int oa_format_tx_chunks(struct adin1110_desc *, uint8_t *, uint32_t);
int oa_dequeue(struct adin1110_desc *, struct oa_list_element **,
	       uint8_t *, uint32_t *);
int oa_enqueue(struct adin1110_desc *, struct oa_list_element **,
	       uint8_t *, uint32_t);
int oa_rx_chunk_to_frame(struct adin1110_desc *, uint8_t *, uint32_t);
int oa_queue_length(struct adin1110_desc *);
int oa_init(struct adin1110_desc *, struct adin1110_init_param *);

#endif /* _SPI_OPEN_ALLIANCE_H */
