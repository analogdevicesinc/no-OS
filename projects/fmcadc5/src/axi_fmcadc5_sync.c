#include <stdlib.h>
#include <stdio.h>
#include "ad9625.h"
#include "axi_fmcadc5_sync.h"
#include "no_os_axi_io.h"
#include "no_os_alloc.h"

/* Simple read */
static inline int i5g_read(struct s_i5g *st, int reg)
{
	uint32_t data;

	no_os_axi_io_read(st->regs, reg, &data);

	return data;
}

/* Simple write */
static inline void i5g_write(struct s_i5g *st, int reg, int val)
{
	no_os_axi_io_write(st->regs, reg, val);
	return;
}

/* Indirect access, returns within 32 clock cycles as long as a clock is
 * present, no need to timeout
 * */
static inline int i5g_spi_access(struct s_i5g *st, int data)
{
	i5g_write(st, I5G_SPI_TRANSMIT_ADDR, data);
	while(i5g_read(st, I5G_SPI_BUSY_ADDR) == I5G_SPI_BUSY) {}
	return(i5g_read(st, I5G_SPI_RECEIVE_ADDR));
}

/* Indirect read, straight forward just send 3 bytes and return the last byte */

static inline int i5g_spi_read(struct s_i5g *st, int sel, int reg)
{
	unsigned char val;
#if 0
	i5g_write(st, I5G_SPI_SELECT_N_ADDR, ~sel);
	val = i5g_spi_access(st, ((reg>>8) | 0x80));
	val = i5g_spi_access(st, reg);
	val = i5g_spi_access(st, 0);
	i5g_write(st, I5G_SPI_SELECT_N_ADDR, -1);
#else
	if (sel == st->ad9625_cs_0)
		ad9625_spi_read(st->ad9625_0_device, reg, &val);
	else
		ad9625_spi_read(st->ad9625_1_device, reg, &val);
#endif
	return(val);
}

/* Simple write, in this case just send 3 bytes */
static inline void i5g_spi_write(struct s_i5g *st, int sel, int reg, int val)
{
#if 0
	i5g_write(st, I5G_SPI_SELECT_N_ADDR, ~sel);
	i5g_spi_access(st, ((reg>>8) | 0x00));
	i5g_spi_access(st, reg);
	i5g_spi_access(st, val);
	i5g_write(st, I5G_SPI_SELECT_N_ADDR, -1);
	return;
#else
	if (sel == st->ad9625_cs_0)
		ad9625_spi_write(st->ad9625_0_device, reg, val);
	else
		ad9625_spi_write(st->ad9625_1_device, reg, val);
#endif
}

/* Device SPI settings - clear & enable violations reporting */
static inline void i5g_ad9625_setup(struct s_i5g *st, int sel, int band)
{
	/* Clear the SYSREF violations */
	i5g_spi_write(st, sel, I5G_AD9625_SG_ADDR, (band << 5));
	i5g_spi_write(st, sel, I5G_AD9625_ST_ADDR, I5G_AD9625_ST_DATA);
	i5g_spi_write(st, sel, I5G_AD9625_SC_ADDR, I5G_AD9625_SC_CLEAR(sel));
	i5g_spi_write(st, sel, I5G_AD9625_IO_ADDR, I5G_AD9625_IO_DATA);

	/* Enable the SYSREF violations */
	i5g_spi_write(st, sel, I5G_AD9625_SC_ADDR, I5G_AD9625_SC_ENABLE(sel));
	i5g_spi_write(st, sel, I5G_AD9625_IO_ADDR, I5G_AD9625_IO_DATA);
	return;
}

/* Device SPI status - SYSREF received and violations we need */

static inline int i5g_ad9625_status(struct s_i5g *st,
				    int sel,
				    int band,
				    int status)
{

	int data;

	/* SYSREF received */
	data = i5g_spi_read(st, sel, I5G_AD9625_SC_ADDR);
	if(data != I5G_AD9625_SC_RECEIVED(sel)) {
		printf("sysref status mismatch (%d, %d, %x)!\n", sel, band, data);
	}

	/* Sysref violations */
	data = i5g_spi_read(st, sel, I5G_AD9625_SS_ADDR);
	if((data & I5G_AD9625_SS_MASK) == I5G_AD9625_SS_SET) {
		return(status | (0x1 << band));
	}

	return(status);
}

/* Check violation band, mostly a gimmick function- but helps you understand the
 * logic of synchronization
 * */

static inline int i5g_status_check(int status)
{
	if (status == 0x3e) return(0); /* sysref at or within 235ps */
	if (status == 0x3c) return(0); /* sysref at or within 270ps */
	if (status == 0x38) return(0); /* sysref at or within 305ps */

	return(-1);
}

/* The mean thing that brutally overtakes everything else to synchronize the
 * devices for interleaving. If you need to resync, try re-entry to this
 * function. If handling differently by individual components, this is the part
 * you need to copy over. Remove probe based return codes, let user space fix
 * the data path and re-enter again
 * */
static int i5g_intlv(struct s_i5g *st)
{
	int delay;
	int band;
	int status_1;
	int status_2;
	int data;
	int timeout;

	/* Make sure everyone else is in the game and has individually achieved
	 * sync
	 * */
	data = i5g_read(st, I5G_SYNC_STATUS_ADDR);
	if (data == I5G_SYNC_OOS) {
		printf("link out-of-sync (%x), deferring probe!\n", data);
		return(-1);
	}
#if 1
	/* Request indirect access (this overrides the default access) */
	timeout = 100;
	i5g_write(st, I5G_SPI_REQUEST_ADDR, I5G_SPI_REQUEST_ACCESS);
	while (i5g_read(st, I5G_SPI_GRANT_ADDR) == I5G_SPI_ACCESS_DISABLED) {
		timeout = timeout - 1;
		if (timeout == 0) {
			printf("request for device access denied!\n");
			return(0);
		}
	}
#endif
	/* Check we got the right devices */
	data = i5g_spi_read(st, st->ad9625_cs_0, I5G_AD9625_ID_ADDR);
	if (data != I5G_AD9625_ID_DATA) {
		printf("unsupported device (%x) at (%d)!\n",
		       data,
		       (int)st->ad9625_cs_0);
		return(0);
	}

	data = i5g_spi_read(st, st->ad9625_cs_1, I5G_AD9625_ID_ADDR);
	if (data != I5G_AD9625_ID_DATA) {
		printf("unsupported device (%x) at (%d)!\n",
		       data,
		       (int)st->ad9625_cs_1);
		return(0);
	}

	/* Check delay is locked */
	data = i5g_read(st, I5G_DELAY_LOCKED_ADDR);
	if (data != I5G_DELAY_LOCKED) {
		printf("sysref delay controller is out-of-lock (%d)!\n", data);
		return(0);
	}

	/* So far so good, let's try synchronization:
	 * - set dual mode, this allows both devices to see the same sync
	 *   (importantly deassertion)
	 * - disable syncs, bring the link down (back to cgs) and initialize delay
	 * */
	i5g_write(st, I5G_SYNC_CONTROL_ADDR, I5G_SYNC_SET);
	i5g_write(st, I5G_SYSREF_CONTROL_ADDR, I5G_SYSREF_SET);

	/* Quick facts about clocks & sysref and all this mess:
	 * - device clock is 2.5G, fpga gets 625M, recovered clock is 156.25M;
	 * - 625M is derived from the device 2.5G clock;
	 * - 156.25 is derived from the recoverd (CDR) clock;
	 * - sysref is generated by fpga(!) at 156.25M;
	 * - we need to make sure that device 1 samples sysref first followed by
	 *   device 2;
	 * - sysref must meet setup time of device 1 only (device 2 follows);
	 * - ad9625 features a sysref monitoring routine that tells us where it is
	 *   w.r.t the 2.5G clock;
	 * - 2.5G == 400 ps, sysref setup requirement is 200ps(not a good candidate,
	 *   is it?), hold is -100ps;
	 * - i.e. actual sysref is 100ps, but has a significant internal delay
	 *   (guessing ~150ps or so);
	 * - most likely chance to miss the edge, (remember there is still a
	 *   possibility of device 2 getting it);
	 * - ideally you want the sysref setup time to be less than half the
	 *   sampling clock in this case;
	 * - so we swap the edges and allow us a full 400 ps for showing up before
	 *   the sampling edge;
	 * - ignore hold, sysref is being generated by a 156.25M clock (/16);
	 * - move sysref until we find the ideal spot that hit the devices, at that
	 *   point get out.
	 * */
	st->sysref_delay = -1;
	for (delay = 0; delay < 32; delay++) {
		/* Change the delay */
		i5g_write(st, I5G_DELAY_ADDR, delay);
		no_os_mdelay(100);
		data = i5g_read(st, I5G_DELAY_VERIFY_ADDR);
		if (data != delay) {
			printf("delay data mismatch(%d, %d)!\n", delay, data);
		}

		/* Change the guard band (does not affect actual timing) */
		status_1 = 0;
		status_2 = 0;
		for (band = 0; band < 6; band++) {
			/* Get the devices ready */
			i5g_ad9625_setup(st, st->ad9625_cs_0, band);
			i5g_ad9625_setup(st, st->ad9625_cs_1, band);

			/* Send the sysref */

			i5g_write(st, I5G_SYSREF_REQUEST_ADDR, I5G_SYSREF_REQUEST);
			while (i5g_read(st, I5G_SYSREF_REQUEST_ADDR) == I5G_SYSREF_BUSY) {}

			/* Check the sysref violations */
			status_1 = i5g_ad9625_status(st, st->ad9625_cs_0, band, status_1);
			status_2 = i5g_ad9625_status(st, st->ad9625_cs_1, band, status_2);
		}

		/* All we need is to keep the sysref edge close to the sampling clock
		 * edge here we are keeping sysref within 305ps ~ 235ps if you are
		 * experimenting, walk this through and print the bands
		 * */
		if ((i5g_status_check(status_1) == 0) &&
		    (i5g_status_check(status_2) == 0)) {
			st->sysref_delay = delay;
			printf("sysref synchronization @%d, status(%02x, %02x)!\n",
			       delay,
			       status_1,
			       status_2);
			break;
		}
	}

	/* Set delay, enable syncs back and check status */
	i5g_write(st, I5G_SYNC_CONTROL_ADDR, I5G_SYNC_RELEASE);
	i5g_write(st, I5G_SYSREF_CONTROL_ADDR, I5G_SYSREF_RELEASE);

	/* Give it some time */
	no_os_mdelay(1);

	/* Is anything wrong? */
	data = i5g_read(st, I5G_SYNC_STATUS_ADDR);
	if (data == I5G_SYNC_OOS) {
		printf("resync failed, may need to reset the transceiver chain (%x)!\n",
		       data);
	}
#if 1
	/* Release indirect access (this overrides the default access) */
	timeout = 100;
	i5g_write(st, I5G_SPI_REQUEST_ADDR, I5G_SPI_REQUEST_RELEASE);
	while (i5g_read(st, I5G_SPI_GRANT_ADDR) == I5G_SPI_ACCESS_ENABLED) {
		timeout = timeout - 1;
		if (timeout == 0) {
			printf("request for device release failed!\n");
			return(0);
		}
	}
#endif
	/* That's all folks! */
	return(0);
}

/* Calibrate the data paths, poor man's version (see hdl for more details)
 * during boot, the lowest frequency component must be present at full scale
 * assumes a solid linear frequency response on all the discrete components
 * (500K to 9G) make this function re-entrant if needs to be called again
 * remove probe based return codes, let user space fix the data path and
 * re-enter again
 * */
static int i5g_calibrate(struct s_i5g *st)
{
	int n;
	int16_t cal_max_0;
	int16_t cal_min_0;
	int16_t cal_max_1;
	int16_t cal_min_1;
	int16_t cal_offset_0;
	int16_t cal_offset_1;
	uint16_t cal_scale_0;
	uint16_t cal_scale_1;

	/* Calibrate gain and offset */
	i5g_write(st, I5G_VCAL_ENABLE_ADDR, I5G_VCAL_ENABLE);
	i5g_write(st, I5G_COR_ENABLE_ADDR, I5G_COR_DISABLE);

	/* Loop to get an average */
	cal_max_0 = 0;
	cal_min_0 = 0;
	cal_max_1 = 0;
	cal_min_1 = 0;

	for (n = 0; n < 16; n++) {
		i5g_write(st, I5G_CAL_ENABLE_ADDR, I5G_CAL_ENABLE);
		i5g_write(st, I5G_TIMER_ADDR, I5G_TIMER_US(100));
		while (i5g_read(st, I5G_TIMER_ADDR) != 0) {}

		i5g_write(st, I5G_CAL_ENABLE_ADDR, I5G_CAL_DISABLE);
		if (i5g_read(st, I5G_CAL_ENABLE_ADDR) != I5G_CAL_DISABLE) {
			printf("calibration failed, expect mismatch!\n");
		}

		cal_max_0 = cal_max_0 + i5g_read(st, I5G_CAL_MAX_0_ADDR);
		cal_min_0 = cal_min_0 + i5g_read(st, I5G_CAL_MIN_0_ADDR);
		cal_max_1 = cal_max_1 + i5g_read(st, I5G_CAL_MAX_1_ADDR);
		cal_min_1 = cal_min_1 + i5g_read(st, I5G_CAL_MIN_1_ADDR);
	}

	/* If you keep dither, need to filter out post adc-core */
	i5g_write(st, I5G_VCAL_ENABLE_ADDR, I5G_VCAL_DISABLE);
	i5g_write(st, I5G_COR_ENABLE_ADDR, I5G_COR_DISABLE);

	/* Peaks - divide or shift? */
	cal_max_0 = cal_max_0/16;
	cal_min_0 = cal_min_0/16;
	cal_max_1 = cal_max_1/16;
	cal_min_1 = cal_min_1/16;

	/* Offsets -divide or shift? -multiply or subtract? */
	cal_offset_0 = -1*((cal_max_0 + cal_min_0)/2);
	cal_offset_1 = -1*((cal_max_1 + cal_min_1)/2);

	i5g_write(st, I5G_COR_OFFSET_0_ADDR, cal_offset_0);
	i5g_write(st, I5G_COR_OFFSET_1_ADDR, cal_offset_1);
	i5g_write(st, I5G_COR_ENABLE_ADDR, I5G_COR_ENABLE);

	/* Scale */
	cal_scale_0 = (uint16_t)(cal_max_0 - cal_min_0);
	cal_scale_1 = (uint16_t)(cal_max_1 - cal_min_1);

	if (cal_scale_1 > cal_scale_0) {
		cal_scale_0 = ((uint32_t)(cal_scale_1*32768))/cal_scale_0;
		cal_scale_1 = 32768;
	} else {
		cal_scale_1 = ((uint32_t)(cal_scale_0*32768))/cal_scale_1;
		cal_scale_0 = 32768;
	}

	i5g_write(st, I5G_COR_SCALE_0_ADDR, cal_scale_0);
	i5g_write(st, I5G_COR_SCALE_1_ADDR, cal_scale_1);
	i5g_write(st, I5G_COR_ENABLE_ADDR, I5G_COR_ENABLE);

	/* FYI */
	printf("calibration values (0) (%d, %d)!\n", cal_max_0, cal_min_0);
	printf("correction values (0) (%d, %d)!\n", cal_offset_0, cal_scale_0);
	printf("calibration values (1) (%d, %d)!\n", cal_max_1, cal_min_1);
	printf("correction values (1) (%d, %d)!\n", cal_offset_1, cal_scale_1);

	return(0);
}

int32_t i5g_setup(struct s_i5g **descriptor,
		  struct s_i5g_init init_param)
{
	struct s_i5g *st;
	int32_t data;

	st = no_os_malloc(sizeof(*st));

	st->regs = init_param.regs;
	st->ad9625_cs_0 = init_param.ad9625_cs_0;
	st->ad9625_cs_1 = init_param.ad9625_cs_1;
	st->ad9625_0_device = init_param.ad9625_0_device;
	st->ad9625_1_device = init_param.ad9625_1_device;

	/* Check version, give up if not an exact match */
	data = i5g_read(st, I5G_VERSION_ADDR);
	if (data != I5G_VERSION) {
		printf("unsupported core version (%x)!\n", (int)data);
		return(-1);
	}

	data = i5g_spi_read(st, st->ad9625_cs_0, AD9625_REG_CHIP_ID);
	printf("%s CHIP ID (0x%x).\n", __func__, (int)data);
	data = i5g_spi_read(st, st->ad9625_cs_1, AD9625_REG_CHIP_ID);
	printf("%s CHIP ID (0x%x).\n", __func__, (int)data);

	/* Interleave & calibrate */
	if (i5g_intlv(st) != 0) {
		printf("i5g_intlv failed!\n");
		return(-1);
	}

	i5g_calibrate(st);

	*descriptor = st;

	return 0;
}

int32_t i5g_remove(struct s_i5g *desc)
{
	no_os_free(desc);

	return 0;
}
