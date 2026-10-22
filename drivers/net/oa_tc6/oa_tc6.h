/*******************************************************************************
 *   @file   oa_tc6.h
 *   @brief  Header file for the Open Alliance TC6 SPI driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef _NO_OS_OA_TC6_H
#define _NO_OS_OA_TC6_H

#include "capi_mutex.h"
#include "capi_spi.h"
#include "no_os_util.h"
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*
 * Per-frame buffer capacity: a full 1514-byte Ethernet frame, a 4-byte 802.1Q
 * tag, the 4-byte FCS the MAC-PHY forwards on RX, and the 8-byte worst-case
 * receive timestamp the MAC-PHY prepends when CONFIG0.FTSE is set (1530).
 */
#ifndef CONFIG_OA_CHUNK_BUFFER_SIZE
#define CONFIG_OA_CHUNK_BUFFER_SIZE	1530
#endif

/*
 * Safety cap on how many back-to-back data transactions one poll may clock
 * while draining RX. The drain normally stops as soon as the last footer
 * reports the RX FIFO empty (RCA == 0); this bounds the worst case so a
 * stuck-high RCA cannot hold the SPI bus forever. It must be high enough that
 * a full RX FIFO drains within a single poll at line rate -- a cap that is too
 * low leaves the FIFO refilling faster than it drains and overflows (LOFE).
 */
#ifndef CONFIG_OA_THREAD_RX_LIMIT
#define CONFIG_OA_THREAD_RX_LIMIT	5
#endif

/*
 * SPI data-chunk buffer size. A frame larger than the buffer or the current
 * TX credit is split across transactions, so this need not hold a whole MTU --
 * it bounds how many chunks one transaction can carry. A full credit window
 * (31 chunks, 5-bit TXC) at 64-byte chunks needs 31 * (64+4) = 2108 bytes; a
 * larger buffer only packs more pending RX per transaction.
 */
#ifndef CONFIG_OA_SPI_BUFF_LEN
#define CONFIG_OA_SPI_BUFF_LEN	1632
#endif
#define OA_SPI_BUFF_LEN		CONFIG_OA_SPI_BUFF_LEN

/* Most registers one control transaction carries. */
#define OA_CTRL_MAX_REGS	1

#define OA_CHUNK_SIZE		64 /* data-chunk payload */
#define OA_REG_LEN		4
#define OA_HEADER_LEN		4
#define OA_FOOTER_LEN		4

/* Header, OA_CTRL_MAX_REGS data words each followed by its PROTE complement,
 * and the trailing word the echo shifts into. */
#define OA_SPI_CTRL_LEN	(2 * OA_HEADER_LEN + OA_CTRL_MAX_REGS * 2 * OA_REG_LEN)

/* Most chunks in one data transaction. */
#define OA_SPI_MAX_CHUNKS	(OA_SPI_BUFF_LEN / (OA_CHUNK_SIZE + OA_HEADER_LEN))

#define OA_MMS_REG(m, r)	(((m) << 16) | ((r) & NO_OS_GENMASK(15, 0)))
#define OA_CTRL_ADDR_MMS_MASK	NO_OS_GENMASK(27, 8)

#define OA_CTRL_HDRB_MASK	NO_OS_BIT(30)
#define OA_CTRL_WNR_MASK	NO_OS_BIT(29)
#define OA_CTRL_AID_MASK	NO_OS_BIT(28)
#define OA_CTRL_MMS_MASK	NO_OS_GENMASK(27, 24)
#define OA_CTRL_ADDR_MASK	NO_OS_GENMASK(23, 8)
#define OA_CTRL_LEN_MASK	NO_OS_GENMASK(7, 1)
#define OA_CTRL_P_MASK		NO_OS_BIT(0)

#define OA_DATA_FOOTER_EXST_MASK	NO_OS_BIT(31)
#define OA_DATA_FOOTER_HDRB_MASK	NO_OS_BIT(30)
#define OA_DATA_FOOTER_SYNC_MASK	NO_OS_BIT(29)
#define OA_DATA_FOOTER_RCA_MASK		NO_OS_GENMASK(28, 24)
#define OA_DATA_FOOTER_VS_MASK		NO_OS_GENMASK(23, 22)
#define OA_DATA_FOOTER_DV_MASK		NO_OS_BIT(21)
#define OA_DATA_FOOTER_SV_MASK		NO_OS_BIT(20)
#define OA_DATA_FOOTER_SWO_MASK		NO_OS_GENMASK(19, 16)
#define OA_DATA_FOOTER_FD_MASK		NO_OS_BIT(15)
#define OA_DATA_FOOTER_EV_MASK		NO_OS_BIT(14)
#define OA_DATA_FOOTER_EBO_MASK		NO_OS_GENMASK(13, 8)
#define OA_DATA_FOOTER_RTSA_MASK    NO_OS_BIT(7)
#define OA_DATA_FOOTER_RTSP_MASK    NO_OS_BIT(6)
#define OA_DATA_FOOTER_TXC_MASK		NO_OS_GENMASK(5, 1)
#define OA_DATA_FOOTER_P_MASK		NO_OS_BIT(0)

/* Max TX credit the MAC can ever report: the footer TXC field is 5 bits. */
#define OA_TC6_MAX_TX_CREDIT	31u

#define OA_DATA_HEADER_DNC_MASK		NO_OS_BIT(31)
#define OA_DATA_HEADER_SEQ_MASK		NO_OS_BIT(30)
#define OA_DATA_HEADER_NORX_MASK	NO_OS_BIT(29)
#define OA_DATA_HEADER_VS_MASK		NO_OS_GENMASK(23, 22)
#define OA_DATA_HEADER_DV_MASK		NO_OS_BIT(21)
#define OA_DATA_HEADER_SV_MASK		NO_OS_BIT(20)
#define OA_DATA_HEADER_SWO_MASK		NO_OS_GENMASK(19, 16)
#define OA_DATA_HEADER_EV_MASK		NO_OS_BIT(14)
#define OA_DATA_HEADER_EBO_MASK		NO_OS_GENMASK(13, 8)
#define OA_DATA_HEADER_TSC_MASK		NO_OS_GENMASK(7, 6)
#define OA_DATA_HEADER_P_MASK		NO_OS_BIT(0)

/* Standard control and status registers (MMS 0) */

#define OA_TC6_IDVER_REG		OA_MMS_REG(0x0, 0x0000)
#define OA_TC6_PHYID_REG		OA_MMS_REG(0x0, 0x0001)
#define OA_TC6_STDCAP_REG		OA_MMS_REG(0x0, 0x0002)
#define OA_TC6_RESET_REG		OA_MMS_REG(0x0, 0x0003)
#define OA_TC6_CONFIG0_REG		OA_MMS_REG(0x0, 0x0004)
#define OA_TC6_CONFIG1_REG		OA_MMS_REG(0x0, 0x0005)
#define OA_TC6_CONFIG2_REG		OA_MMS_REG(0x0, 0x0006)
#define OA_TC6_STATUS0_REG		OA_MMS_REG(0x0, 0x0008)
#define OA_TC6_STATUS1_REG		OA_MMS_REG(0x0, 0x0009)
#define OA_TC6_BUFST_REG		OA_MMS_REG(0x0, 0x000B)
#define OA_TC6_IMSK0_REG		OA_MMS_REG(0x0, 0x000C)
#define OA_TC6_IMSK1_REG		OA_MMS_REG(0x0, 0x000D)

#define OA_TC6_TTSCAH_REG		OA_MMS_REG(0x0, 0x0010)
#define OA_TC6_TTSCAL_REG		OA_MMS_REG(0x0, 0x0011)
#define OA_TC6_TTSCBH_REG		OA_MMS_REG(0x0, 0x0012)
#define OA_TC6_TTSCBL_REG		OA_MMS_REG(0x0, 0x0013)
#define OA_TC6_TTSCCH_REG		OA_MMS_REG(0x0, 0x0014)
#define OA_TC6_TTSCCL_REG		OA_MMS_REG(0x0, 0x0015)
#define OA_TC6_MDIOACC0_REG		OA_MMS_REG(0x0, 0x0020)
#define OA_TC6_MDIOACC1_REG 		OA_MMS_REG(0x0, 0x0021)
#define OA_TC6_MDIOACC2_REG 		OA_MMS_REG(0x0, 0x0022)
#define OA_TC6_MDIOACC3_REG 		OA_MMS_REG(0x0, 0x0023)
#define OA_TC6_MDIOACC4_REG 		OA_MMS_REG(0x0, 0x0024)
#define OA_TC6_MDIOACC5_REG 		OA_MMS_REG(0x0, 0x0025)
#define OA_TC6_MDIOACC6_REG 		OA_MMS_REG(0x0, 0x0026)
#define OA_TC6_MDIOACC7_REG 		OA_MMS_REG(0x0, 0x0027)

#define OA_TC6_IMASK0_TXPEM		NO_OS_BIT(0)
#define OA_TC6_IMASK0_TXBOEM		NO_OS_BIT(1)
#define OA_TC6_IMASK0_TXBUEM		NO_OS_BIT(2)
#define OA_TC6_IMASK0_RXBOEM		NO_OS_BIT(3)
#define OA_TC6_IMASK0_LOFEM		NO_OS_BIT(4)
#define OA_TC6_IMASK0_HDREM		NO_OS_BIT(5)
#define OA_TC6_IMSK1_RX_RDY_MASK	NO_OS_BIT(4)

/* STATUS0 error bits. Same bit positions as the OA_TC6_IMASK0_* masks: the
 * interrupt-mask register and the status register share the layout. */
#define OA_TC6_STATUS0_TXPE	NO_OS_BIT(0)    /* TX protocol error */
#define OA_TC6_STATUS0_TXBOE	NO_OS_BIT(1)   /* TX buffer overflow */
#define OA_TC6_STATUS0_TXBUE	NO_OS_BIT(2)   /* TX buffer underflow */
#define OA_TC6_STATUS0_RXBOE	NO_OS_BIT(3)   /* RX buffer overflow */
#define OA_TC6_STATUS0_LOFE	NO_OS_BIT(4)    /* loss of framing */
#define OA_TC6_STATUS0_HDRE	NO_OS_BIT(5)    /* header error */
#define OA_TC6_STATUS0_RESETC	NO_OS_BIT(6)  /* reset complete */
#define OA_TC6_STATUS0_TXFCSE	NO_OS_BIT(11) /* TX frame check sequence error */
#define OA_TC6_STATUS0_CDPE	NO_OS_BIT(12)   /* control data protection error */

/* STATUS0 transmit-egress-timestamp-capture-available bits. Not errors: they
 * latch a TSC-requested capture for the application to read, so the error path
 * must leave them alone instead of clearing them out from under it. */
#define OA_TC6_STATUS0_TTSCAA	NO_OS_BIT(8)
#define OA_TC6_STATUS0_TTSCAB	NO_OS_BIT(9)
#define OA_TC6_STATUS0_TTSCAC	NO_OS_BIT(10)
#define OA_TC6_STATUS0_TTSCA_MASK \
	(OA_TC6_STATUS0_TTSCAA | OA_TC6_STATUS0_TTSCAB | OA_TC6_STATUS0_TTSCAC)

/* STATUS1 fault bits (single-port host subset; P2_* dual-port and TEST_ADC
 * bits are intentionally omitted). */
#define OA_TC6_STATUS1_RX_IFG_ERR NO_OS_BIT(8)	/* RX inter-frame-gap error */
#define OA_TC6_STATUS1_MAX_RR_ERR NO_OS_BIT(9)	/* TX max collision retry hit */
#define OA_TC6_STATUS1_SPI_ERR NO_OS_BIT(10)	/* SPI transaction error */
#define OA_TC6_STATUS1_RX_ECC_ERR	NO_OS_BIT(11) /* RX FIFO size ECC error */
#define OA_TC6_STATUS1_TX_ECC_ERR	NO_OS_BIT(12) /* TX FIFO size ECC error */
#define OA_TC6_STATUS1_RD_ACC_ERR	NO_OS_BIT(30) /* read access error */

/* Direct (memory-mapped) access to the Clause-22 PHY standard register space. */
#define OA_TC6_PHY_STD_REG_BASE		0xFF00
#define OA_TC6_PHY_STD_REG_MASK		0x001F
#define OA_TC6_PHY_STD_REG(reg)		\
	OA_MMS_REG(0x0,         \
		   OA_TC6_PHY_STD_REG_BASE | ((reg)&OA_TC6_PHY_STD_REG_MASK))

#define OA_TC6_RESET_SWRESET		NO_OS_BIT(0)

#define OA_TC6_CONFIG0_RESET	0x6
#define OA_TC6_CONFIG0_PROTE_MASK	NO_OS_BIT(5)
#define OA_TC6_CONFIG0_FTSS_MASK	NO_OS_BIT(6)
#define OA_TC6_CONFIG0_FTSE_MASK	NO_OS_BIT(7)
#define OA_TC6_CONFIG0_RXCTE_MASK	NO_OS_BIT(8)
#define OA_TC6_CONFIG0_TXCTE_MASK	NO_OS_BIT(9)
#define OA_TC6_CONFIG0_TXCTHRESH_MASK	NO_OS_GENMASK(11, 10)
#define OA_TC6_CONFIG0_ZARFE_MASK	NO_OS_BIT(12)
#define OA_TC6_CONFIG0_CSARFE_MASK	NO_OS_BIT(13)
#define OA_TC6_CONFIG0_TXFCSVE_MASK	NO_OS_BIT(14)
#define OA_TC6_CONFIG0_SYNC_MASK	NO_OS_BIT(15)


/* CONFIG0.CPS chunk-payload-size field; chunk_size = 1 << CPS (OA TC6 std). */
#define OA_TC6_CONFIG0_CPS_MASK		NO_OS_GENMASK(2, 0)
#define OA_TC6_CONFIG0_CPS_8	0x3
#define OA_TC6_CONFIG0_CPS_16	0x4
#define OA_TC6_CONFIG0_CPS_32	0x5
#define OA_TC6_CONFIG0_CPS_64		0x6

#define OA_TC6_BUFSTS_TXC_MASK 		NO_OS_GENMASK(15, 8)
#define OA_TC6_BUFSTS_RCA_MASK 		NO_OS_GENMASK(7, 0)

/**
 * @brief State for data buffers containing Ethernet frames
 */
enum oa_tc6_user_buffer_state {
/*
	 * Neither the OA nor the user application doesn't currently
	 * use this buffer. OA can start writing it.
 */
	OA_BUFF_FREE,

/*
	 * The buffer is currently written by OA, but it doesn't yet
	 * contain a complete frame. The user application cannot reference
	 * the data yet.
 */
	OA_BUFF_RX_IN_PROGRESS,

/*
	 * The buffer has a complete frame, and is no longer accessed by OA.
	 * The user application could use this.
 */
	OA_BUFF_RX_COMPLETE,

	/* The buffer is accessed by the user application. OA will not overwrite it */
	OA_BUFF_RX_USER_OWNED,

	/* The user writes to this buffer. OA doesn't access it. */
	OA_BUFF_TX_BUSY,

	/* The buffer is ready to be transmitted. The user won't access it anymore. */
	OA_BUFF_TX_READY,
};

/**
 * @brief Stores an Ethernet frame along with metadata needed for parsing.
 * The MAC driver or the user application will receive and submit frames for transmission
 * in this format.
 */
struct oa_tc6_frame_buffer {
	uint32_t index;
	uint32_t len;
	uint8_t data[CONFIG_OA_CHUNK_BUFFER_SIZE];
	enum oa_tc6_user_buffer_state state;
	uint8_t vs;	 /**< Vendor Specific. Tx or Rx */
	uint8_t tsc;	 /**< Timestamp capture. 2-bits. Tx Only */
	bool frame_drop; /**< Frame should be dropped (is invalid). Rx Only */
	bool rtsa;	 /**< Timestamp added. Rx Only */
	bool rtsp;	 /**< Timestamp parity. Rx Only */
};

/**
 * @brief Stores the status flags which are provided as part of the footer
 * during data transfers. These flags will be latched by the driver and can be
 * cleared by the user when reading.
 */
struct oa_tc6_flags {
	bool flags_valid; /**< Indicates the flags are latched and valid */
	bool exst;	  /**< Latched high until cleared */
	bool hdrb;	  /**< Latched high until cleared */
	bool sync;	  /**< Instantaneous value */
};

/**
 * @brief Detail payload passed to the event callback for STATUS0/STATUS1 error
 * events. Points to the raw register values read (and cleared) when the
 * extended-status footer was serviced. The callback receives a pointer to this
 * for the OA_TC6_EVENT_ERROR and OA_TC6_EVENT_ERR_* STATUS events and for a
 * RESET raised from STATUS0.RESETC; it is NULL for the per-chunk footer
 * errors, RX_READY, TX_CREDITS and a RESET raised from a SYNC=0 footer. The
 * pointer is valid only for the duration of the call.
 */
struct oa_tc6_error_detail {
	uint32_t status0; /**< Raw STATUS0 latched at the error */
	uint32_t status1; /**< Raw STATUS1 latched at the error */
};

/**
 * @brief Events reported to the user callback registered via
 * oa_tc6_register_callback().
 *
 *   OA_TC6_EVENT_ERROR      - STATUS0 had a set bit that maps to no known
 *                             event, and no other STATUS0/1 event fired.
 *                             STATUS0/1 have been cleared; the callback's
 *                             detail argument points to a struct
 *                             oa_tc6_error_detail with their raw values.
 *   OA_TC6_EVENT_RESET      - The MAC-PHY reset and lost its configuration
 *                             (data footer SYNC=0, or STATUS0.RESETC). Per the
 *                             OA-TC6 spec the host must terminate transfers,
 *                             reconfigure the MAC-PHY, and only then set
 *                             CONFIG0.SYNC. The driver does NOT re-assert SYNC
 *                             on its own; re-run configuration in response.
 *   OA_TC6_EVENT_RX_READY  - One or more RX chunks are waiting in the MAC
 *                             FIFO (RCA > 0 in the footer). The caller should
 *                             ensure another poll runs soon.
 *   OA_TC6_EVENT_TX_CREDITS - TX credits became non-zero after being fully
 *                             exhausted. Transmission can resume.
 *   OA_TC6_EVENT_RX         - A received frame was completed in the RX pool.
 *   OA_TC6_EVENT_TX         - The MAC-PHY accepted one or more queued TX
 *                             frames; their buffers are free again.
 *
 *   OA_TC6_EVENT_ERR_* -      A specific error bit was set in STATUS0/STATUS1
 *                             when a footer flagged EXST or HDRB, or
 *                             a per-chunk footer error (parity/frame-drop) was
 *                             detected. For the STATUS0/STATUS1 errors both
 *                             registers have been read and cleared; the callback
 *                             fires once per set bit so several may be reported
 *                             in a row, each with the detail argument pointing
 *                             at the raw STATUS0/1 snapshot (struct
 *                             oa_tc6_error_detail). The per-chunk footer errors
 *                             pass a NULL detail.
 *
 * The callback signature is void (*)(enum oa_tc6_event, void *detail, void *arg)
 * where arg is the value supplied to oa_tc6_register_callback() and detail is as
 * described above (NULL unless noted).
 */
enum oa_tc6_event {
	OA_TC6_EVENT_ERROR = 0,
	OA_TC6_EVENT_RX_READY = 1,
	OA_TC6_EVENT_TX_CREDITS = 2,

	OA_TC6_EVENT_ERR_TX_PROTOCOL,	/* STATUS0.TXPE  */
	OA_TC6_EVENT_ERR_TX_OVERFLOW,	/* STATUS0.TXBOE */
	OA_TC6_EVENT_ERR_TX_UNDERFLOW,	/* STATUS0.TXBUE */
	OA_TC6_EVENT_ERR_RX_OVERFLOW,	/* STATUS0.RXBOE */
	OA_TC6_EVENT_ERR_LOSS_OF_FRAME, /* STATUS0.LOFE  */
	OA_TC6_EVENT_ERR_HEADER,	/* STATUS0.HDRE  */

	/* Per-chunk footer errors (detected inline, not via STATUS0/1). */
	OA_TC6_EVENT_ERR_RX_PARITY,	/* footer parity (P) mismatch */
	OA_TC6_EVENT_ERR_RX_FRAME_DROP, /* footer FD=1: frame flagged bad */

	/* Further STATUS0 faults. */
	OA_TC6_EVENT_ERR_TX_FCS,	 /* STATUS0.TXFCSE */
	OA_TC6_EVENT_ERR_CTRL_DATA_PROT, /* STATUS0.CDPE   */

	/* STATUS1 faults. */
	OA_TC6_EVENT_ERR_RX_IFG,       /* STATUS1.RX_IFG_ERR */
	OA_TC6_EVENT_ERR_TX_MAX_RETRY, /* STATUS1.MAX_RR_ERR */
	OA_TC6_EVENT_ERR_SPI,	       /* STATUS1.SPI_ERR    */
	OA_TC6_EVENT_ERR_RX_ECC,       /* STATUS1.RX_ECC_ERR */
	OA_TC6_EVENT_ERR_TX_ECC,       /* STATUS1.TX_ECC_ERR */
	OA_TC6_EVENT_ERR_READ_ACCESS,  /* STATUS1.RD_ACC_ERR */

	/* Not an error: MAC-PHY reset / config sync lost -- reconfigure. */
	OA_TC6_EVENT_RESET, /* footer SYNC=0 or STATUS0.RESETC */

	/* Not errors: frame traffic. */
	OA_TC6_EVENT_RX, /* a received frame is ready in the RX pool */
	OA_TC6_EVENT_TX, /* the MAC-PHY accepted queued TX frames */
};

/**
 * @brief A TX priority queue: its own frame-buffer ring and credit.
 *
 * Per-queue credit is only observable from data-chunk footers: a chunk's
 * footer TXC is the credit for the queue named by that chunk's own
 * request-header VS (see tx_chunk_credit_q); there is no per-queue register.
 * init_credit seeds tx_credit at reset (TX FIFO capacity in chunks); 0 means
 * unknown.
 * Single-queue operation is a one-element array with vs == 0.
 */
struct oa_tc6_queue {
	struct oa_tc6_frame_buffer *pool;
	uint32_t pool_size;
	uint8_t vs;
	uint32_t init_credit;

	uint32_t head;
	uint32_t tail;
	uint32_t count;
	uint32_t tx_credit;
	uint32_t prev_tx_credit;
};

/**
 * @brief oa_tc6 software statistics counters.
 */
struct oa_tc6_stats {
	uint64_t rx_frames;	       /* frames delivered to the RX pool */
	uint64_t rx_bytes;	       /* payload bytes received (sum of frame len) */
	uint64_t tx_frames;	       /* frames accepted by the MAC-PHY */
	uint64_t tx_bytes;	       /* payload bytes transmitted */
	uint32_t rx_drop_fd;	       /* footer frame-drop (FD) flagged frames */
	uint32_t rx_drop_nobuf;	       /* frames dropped on a full RX pool */
	uint32_t exst_events;	       /* EXST latched in a data-chunk footer */
	uint32_t rx_footer_parity_err; /* data-chunk footer with a parity error */
};

/**
 * @brief Holds the frame buffers and the communication descriptor for the OA TC6 driver.
 */
struct oa_tc6_desc {
	struct capi_spi_device *comm_desc;
	uint8_t ctrl_chunks[OA_SPI_CTRL_LEN];
	uint8_t data_chunks[OA_SPI_BUFF_LEN];

/*
	 * Queue whose credit each TX chunk slot's footer TXC reports, or -1.
	 * That is the queue named by the chunk's request-header VS, not the
	 * footer's own VS field (the received frame's source, mapped
	 * differently).
 */
	int8_t tx_chunk_credit_q[OA_SPI_MAX_CHUNKS];
	/* Queue whose frame ends (EV) in each chunk slot, or -1. */
	int8_t tx_chunk_eof_q[OA_SPI_MAX_CHUNKS];
	/* Chunks clocked by the transaction being packed/in flight. */
	uint32_t tx_pass_chunks;

	/* Data-chunk payload size in bytes (1 << CONFIG0.CPS). */
	uint32_t chunk_size;

/*
	 * ctrl_lock serializes all control transactions (register read/write).
	 * data_lock serializes the TX/RX frame-buffer rings: the pool index
	 * bookkeeping (each tx_q head/tail/count, rx_head/rx_tail and per-slot
	 * state) touched by both the producer/consumer API (get/put_tx_frame,
	 * get/put_rx_frame) and the SPI transfer function (oa_tc6_thread). In a
	 * bare-metal single-threaded context both locks are no-ops; they become
	 * meaningful when a FreeRTOS task calls send/recv while another runs the
	 * poll loop.
 *
	 * ctrl_lock doubles as the SPI bus lock: oa_tc6_thread() holds it across
	 * its transfers. That is what mutually excludes register (ctrl) access
	 * from data transfers on the shared bus: a task issuing a register read
	 * blocks on ctrl_lock until the data transfers complete instead of
	 * colliding with them. Under
	 * an RTOS ctrl_lock must therefore be a recursive, priority-inheriting
	 * mutex -- recursive because the poll path re-enters it for register
	 * reads (BUFST bootstrap, STATUS0 service, event_cb), and
	 * priority-inheriting so a lower-priority poll task holding the bus
	 * cannot be starved by a higher-priority register reader waiting on it.
 *
	 * Ordering invariant: ctrl_lock is the outer lock, data_lock the inner one.
	 * A data_lock critical section is always leaf-level -- short ring
	 * bookkeeping, never held across an SPI transfer and never taken before
	 * ctrl_lock. The single global order (ctrl_lock -> data_lock) means the two
	 * cannot deadlock against each other.
 *
	 * Both are always non-NULL after init. With no mutex_cfg (bare-metal
	 * default) each points at a shared file-static no-op sentinel, so
	 * lock/unlock never need a NULL guard. With a real mutex_cfg,
	 * capi_mutex_init() points the handle at the backend's own storage.
 */
	struct capi_mutex *ctrl_lock;
	struct capi_mutex *data_lock;

	/* RX frame-buffer pool: caller-allocated array, set by oa_tc6_init(). */
	struct oa_tc6_frame_buffer *rx_pool;
	uint32_t rx_pool_size;

/*
	 * Scratch RX buffer used only when rx_pool is exhausted (-ENOBUFS from
	 * oa_tc6_get_empty_rx_buff()). Lives outside rx_pool[] so anything
	 * routed into it is never reachable from oa_tc6_get_rx_frame() (which
	 * only walks [rx_tail, rx_head)) -- a safe sink that lets footer
	 * parsing (credit, HDRB, sync, EXST) keep going instead of aborting
	 * the whole transaction when a frame can't be placed. Frames that close
	 * here are counted in rx_ring_full_total.
 */
	struct oa_tc6_frame_buffer rx_sink;

/*
	 * The TX and RX pools are managed as FIFO rings so frames are always
	 * transmitted and delivered in submission/receive order. Producers
	 * allocate at *_head and advance it; consumers drain from *_tail and
	 * advance it. A plain first-free scan would recycle freed slots out of
	 * order and reorder the stream once several frames are in flight.
 */
	uint32_t rx_head; /* slot the driver is currently writing into */
	uint32_t rx_tail; /* oldest completed slot to deliver next */

/*
	 * TX queues. Each has its own ring and footer-tracked credit.
	 * tx_q_storage backs the queue of the tx_pool.
 */
	struct oa_tc6_queue *tx_q;
	uint8_t tx_q_count;
	struct oa_tc6_queue tx_q_storage;

/*
	 * TX progress computed by oa_tc6_tx_frame_to_chunks() for the pass being
	 * packed, applied only for the chunks the MAC-PHY is confirmed to have
	 * accepted (oa_tc6_tx_commit()). tx_pending_done[i] counts whole frames
	 * of tx_q[i] packed this pass (the packing cursor past the real tail);
	 * tx_pending_incomplete_frame/_index is the at-most-one frame left open
	 * (spanning) and its new resume offset. Reset at the top of every
	 * oa_tc6_tx_frame_to_chunks() call; the real ring state (position,
	 * ->index) only moves in oa_tc6_tx_commit().
 */
	uint32_t tx_pending_done[4];
	struct oa_tc6_frame_buffer *tx_pending_incomplete_frame;
	uint32_t tx_pending_incomplete_index;

	uint32_t data_rx_credit;
	/* data_rx_credit has been seeded (from a BUFST read or a data-chunk
	 * footer). Until then the thread must read BUFST once to bootstrap. */
	bool credit_valid;

	struct oa_tc6_flags xfer_flags;
	bool prote_spi;

	bool skip_current_frame;

/*
	 * Latched when a footer reports SYNC=0 (MAC-PHY reset/config lost), so
	 * OA_TC6_EVENT_RESET fires once per loss and not on every SYNC=0 footer
	 * until config is restored (a footer with SYNC=1 re-arms it). The
	 * MAC-PHY ignores TX data while unconfigured (7.6), so frames stay
	 * queued meanwhile.
 */
	bool sync_lost;

/*
	 * A footer in the last processed transaction flagged EXST/HDRB, so a
	 * STATUS0 read/clear is owed. Serviced once the data pass is over, while
	 * the bus is free.
 */
	bool status0_pending;

/*
	 * Set by the caller before a poll when the MAC-PHY raised its interrupt
	 * (nINT). Consumed (and cleared) by the transfer function: if there is
	 * otherwise nothing to clock, one empty data chunk is forced onto the bus
	 * so its footer is read and any pending error (EXST) is decoded and
	 * reported.
	 * Portable equivalent of the Linux driver's int_flag. Set directly, no
	 * helper.
 */
	bool irq_pending;

/*
	 * Optional user callback invoked when a data-chunk footer or a
	 * STATUS0/1 read reports a noteworthy event (see oa_tc6_event). Called
	 * from the main-loop poll context, never from an ISR. NULL if no
	 * callback has been registered.
 */
	void (*event_cb)(enum oa_tc6_event, void *, void *);
	void *event_cb_arg;

/*
	 * Only ever incremented, except by oa_tc6_reset_stats(); callers that
	 * want a per-poll count sample it before and after a thread pass.
 */
	struct oa_tc6_stats stats;
};

/**
 * @brief Holds the initialization parameters for the OA TC6 driver.
 */
struct oa_tc6_init_param {
	/* SPI target device, initialized by the application (the SPI controller
	 * must be brought up via capi_spi_init() before this device is used). */
	struct capi_spi_device *comm_desc;

	/* The OASPI device uses Protected SPI for control transactions */
	bool prote_spi;


	/* Caller-allocated RX frame-buffer pool. Must remain valid for the
	 * lifetime of the descriptor. rx_pool_size must be non-zero. */
	struct oa_tc6_frame_buffer *rx_pool;
	uint32_t rx_pool_size;

	/* Caller-allocated TX frame-buffer pool, like rx_pool. */
	struct oa_tc6_frame_buffer *tx_pool;
	uint32_t tx_pool_size;

/*
	 * Optional mutex configuration for serialising control transactions.
	 * Pass NULL (or leave zeroed) in bare-metal single-threaded contexts --
	 * a built-in no-op lock is used.  In an RTOS context provide a
	 * capi_mutex_config with the appropriate platform ops so that concurrent
	 * reg_read/write calls from different tasks are safe. The mutex MUST be
	 * recursive (.recursive = true): the poll path re-enters ctrl_lock.
 */
	const struct capi_mutex_config *ctrl_mutex_cfg;

/*
	 * Optional mutex configuration for serialising the TX/RX frame-buffer
	 * rings (data path). Must be an INDEPENDENT mutex from ctrl_mutex_cfg
	 * (its own config and storage) -- the two locks are held separately and
	 * a shared handle would defeat the ctrl/data concurrency. Pass NULL (or
	 * leave zeroed) in bare-metal single-threaded contexts; a built-in no-op
	 * lock is used. In an RTOS context provide a capi_mutex_config so that a
	 * TX/RX producer task and the poll task can touch the pools concurrently.
 */
	const struct capi_mutex_config *data_mutex_cfg;
};

/* Parity bit that makes a 32-bit header/footer word carry odd parity: OR the
 * result into the word to set P, or test == 0 to validate a received word. */
uint8_t oa_tc6_crc1(uint32_t header);

/* Read a register from the MAC device */
int oa_tc6_reg_read(struct oa_tc6_desc *, uint32_t, uint32_t *);

/* Write a register of the MAC device */
int oa_tc6_reg_write(struct oa_tc6_desc *, uint32_t, uint32_t);

/* Update a register field */
int oa_tc6_reg_update(struct oa_tc6_desc *, uint32_t, uint32_t, uint32_t);

/* Get a received frame with a matching VS field (in the RX chunks) */
int oa_tc6_get_rx_frame_match_vs(struct oa_tc6_desc *,
				 struct oa_tc6_frame_buffer **, uint8_t,
				 uint8_t);

/* Get the first frame in the RX queue */
int oa_tc6_get_rx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer **);

/* Mark the frame buffer as ready to be reused for a new frame. */
int oa_tc6_put_rx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer *);

/* Get a frame buffer which can be filled and submitted for transmission. */
int oa_tc6_get_tx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer **);

/* Mark the frame buffer as filled and ready for transmission. On error the
 * buffer stays owned by the caller, who must fix ->len and put it again. */
int oa_tc6_put_tx_frame(struct oa_tc6_desc *, struct oa_tc6_frame_buffer *);

/* Gets the latched transfer flags, and optionally clears the latch */
int oa_tc6_get_xfer_flags(struct oa_tc6_desc *, struct oa_tc6_flags *, bool);

/*
 * Transmit all the frames in the OA_BUFF_TX_READY state and receive the
 * available chunks.
 */
int oa_tc6_thread(struct oa_tc6_desc *);

/*
 * Register a callback to be notified of OA-TC6 events (error conditions,
 * RX frames available, TX credits recovered). Pass NULL to unregister.
 * The callback is invoked from the poll context (not from an ISR). Its detail
 * argument points to a struct oa_tc6_error_detail for STATUS0/1 error events and
 * is NULL otherwise (see enum oa_tc6_event); arg is passed through verbatim.
 */
void oa_tc6_register_callback(struct oa_tc6_desc *,
			      void (*cb)(enum oa_tc6_event, void *, void *),
			      void *arg);

/* Trigger a soft reset of the MAC-PHY and wait for completion */
int oa_tc6_sw_reset(struct oa_tc6_desc *);

/* Initialize the OA TC6 SPI driver */
int oa_tc6_init(struct oa_tc6_desc *, struct oa_tc6_init_param *);

/* Free the resources allocated by the oa_tc6_init() function */
int oa_tc6_remove(struct oa_tc6_desc *);

/* Get a snapshot of the software statistics counters. */
int oa_tc6_get_stats(struct oa_tc6_desc *desc, struct oa_tc6_stats *stats);

/* Reset the software statistics counters to zero. */
int oa_tc6_reset_stats(struct oa_tc6_desc *desc);

#if defined(__cplusplus)
}
#endif

#endif /* _NO_OS_OA_TC6_H */
