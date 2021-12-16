/*
 * axi_fmcadc5_sync.h
 *
 *  Created on: Mar 30, 2018
 *      Author: adrimbar
 */

#ifndef SRC_I5G_H_
#define SRC_I5G_H_

/* Register addresses & data (direct access) */
#define I5G_VERSION_ADDR		(0x0000 << 2)
#define I5G_IDENTIFIER_ADDR		(0x0001 << 2)
#define I5G_SCRATCH_ADDR		(0x0002 << 2)
#define I5G_TIMER_ADDR			(0x0003 << 2)
#define I5G_SPI_REQUEST_ADDR    (0x0010 << 2)
#define I5G_SPI_GRANT_ADDR 		(0x0011 << 2)
#define I5G_SPI_SELECT_N_ADDR	(0x0012 << 2)
#define I5G_SPI_TRANSMIT_ADDR	(0x0013 << 2)
#define I5G_SPI_RECEIVE_ADDR 	(0x0014 << 2)
#define I5G_SPI_BUSY_ADDR    	(0x0015 << 2)
#define I5G_DELAY_ADDR			(0x0020 << 2)
#define I5G_DELAY_VERIFY_ADDR	(0x0021 << 2)
#define I5G_DELAY_LOCKED_ADDR	(0x0022 << 2)
#define I5G_SYNC_CONTROL_ADDR	(0x0030 << 2)
#define I5G_SYNC_STATUS_ADDR	(0x0031 << 2)
#define I5G_SYSREF_CONTROL_ADDR	(0x0040 << 2)
#define I5G_SYSREF_REQUEST_ADDR (0x0041 << 2)
#define I5G_VCAL_CNT_ADDR		(0x0050 << 2)
#define I5G_VCAL_ENABLE_ADDR	(0x0051 << 2)
#define I5G_CAL_ENABLE_ADDR		(0x0060 << 2)
#define I5G_CAL_MAX_0_ADDR		(0x0064 << 2)
#define I5G_CAL_MIN_0_ADDR		(0x0065 << 2)
#define I5G_CAL_MAX_1_ADDR		(0x0066 << 2)
#define I5G_CAL_MIN_1_ADDR		(0x0067 << 2)
#define I5G_COR_ENABLE_ADDR		(0x0061 << 2)
#define I5G_COR_SCALE_0_ADDR	(0x0068 << 2)
#define I5G_COR_OFFSET_0_ADDR	(0x0069 << 2)
#define I5G_COR_SCALE_1_ADDR	(0x006a << 2)
#define I5G_COR_OFFSET_1_ADDR	(0x006b << 2)

/* Register addresses & data (direct access) */
#define I5G_VERSION      		0x040063	/* version (4.0a) */
#define I5G_SPI_REQUEST_ACCESS  0x000001 	/* request access */
#define I5G_SPI_REQUEST_RELEASE 0x000000 	/* release access */
#define I5G_SPI_ACCESS_ENABLED	0x000001 	/* request enabled */
#define I5G_SPI_ACCESS_DISABLED	0x000000 	/* request disabled */
#define I5G_SPI_BUSY         	0x000001 	/* access busy */
#define I5G_DELAY_LOCKED     	0x000001 	/* delay is locked */
#define I5G_SYNC_SET            0x000007 	/* dual mode and disabled */
#define I5G_SYNC_RELEASE        0x000004 	/* dual mode and enabled */
#define I5G_SYNC_OOS            0x000000 	/* out-of-sync */
#define I5G_SYSREF_SET          0x000021 	/* one-shot and disabled */
#define I5G_SYSREF_RELEASE      0x000020 	/* one-shot and enabled */
#define I5G_SYSREF_REQUEST      0x000001 	/* sysref-request */
#define I5G_SYSREF_BUSY			0x000001 	/* sysref-busy */
#define I5G_VCAL_CNT_10M		0x000004	/* 100/((n+1)*2) */
#define I5G_VCAL_ENABLE			0x000001	/* enable */
#define I5G_VCAL_DISABLE		0x000000	/* enable */
#define I5G_CAL_ENABLE			0x000001	/* enable */
#define I5G_CAL_DISABLE			0x000000	/* enable */
#define I5G_COR_ENABLE			0x000001	/* enable */
#define I5G_COR_DISABLE			0x000000	/* enable */

/* Register addresses & data (indirect access) */
#define	I5G_AD9625_ID_ADDR 0x000001	/* identifier address */
#define	I5G_AD9625_ID_DATA 0x000041	/* refer data sheet for details */
#define	I5G_AD9625_ST_ADDR 0x000072	/* sysref time-stamp address [7:6] */
#define	I5G_AD9625_ST_DATA 0x00008b	/* sysref timestamping enabled (2'b10) */
#define I5G_AD9625_SG_ADDR 0x00013c	/* sysref guard band [7:5] */
#define I5G_AD9625_SS_ADDR 0x000100	/* sysref status reporting [2] */
#define I5G_AD9625_SS_MASK 0x000004 	/* setup violations mask */
#define I5G_AD9625_SS_SET  0x000004 	/* setup violations detected */
#define I5G_AD9625_IO_ADDR 0x0000ff	/* internal update address [0] */
#define I5G_AD9625_IO_DATA 0x000001 	/* register update(1), self-clearing */
#define I5G_AD9625_SC_ADDR 0x00003a	/* sysref control address */

/* [6] run(0)/clear(1), [3] @pos(0)/@neg(1), [2] continous(0)/one-shot(1),
 * [1] disable(0)/enable(0)
 * */
#define I5G_AD9625_SC_ENABLE(sel)  	((sel == 1) ? 0x00000e : 0x000006)
#define I5G_AD9625_SC_RECEIVED(sel) ((sel == 1) ? 0x00000c : 0x000004)
#define I5G_AD9625_SC_CLEAR(sel)	((sel == 1) ? 0x00004e : 0x000046)

/* Default is ms, we need finer delays (10ns) */
#define I5G_TIMER_US(d) ((d*100)-1)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct s_i5g {
	struct ad9625_dev *ad9625_0_device;
	struct ad9625_dev *ad9625_1_device;
	int32_t	regs;
	int32_t	ad9625_cs_0;
	int32_t	ad9625_cs_1;
	int32_t	sysref_delay;
};

struct s_i5g_init {
	struct ad9625_dev *ad9625_0_device;
	struct ad9625_dev *ad9625_1_device;
	int32_t	regs;
	int32_t	ad9625_cs_0;
	int32_t	ad9625_cs_1;
	int32_t	sysref_delay;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Initializes core descriptor with the devices */
int32_t i5g_setup(struct s_i5g **descriptor,
		  struct s_i5g_init init_param);

/* Free the resources allocated by i5g_setup() */
int32_t i5g_remove(struct s_i5g *desc);

#endif /* SRC_I5G_H_ */
