/*
 * Perform a loopback test on various picozed sdr2 ccfmc modules.
 */

#include "sleep.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "xparameters.h"
#include "xstatus.h"

/* clock frequencies for ApisSys AF101 */
#define FMC_CLK0 200 /* MHz */
#define FMC_CLK1 350 /* MHz */

#define GPIO_WAIT_TIME 5000
#define GPOUT0_OFFSET 0x101
#define GPOUT1_OFFSET 0x111
#define GPOUT2_OFFSET 0x121
#define GPIN0_OFFSET 0x102
#define GPIN1_OFFSET 0x112
#define GPIN2_OFFSET 0x122

void delay_ms(u32 ms_count)
{
	u32 count;

	for (count = 0; count < ((ms_count*100000) + 1); count++) {
		asm("nop");
	}
}

u32 fmc_verify_clk(u32 rdata, u32 expected_clk)
{
	u32 received_clk = 0;
	u32 edata = 0;

	edata = (expected_clk << 16) / 100;
	received_clk = (rdata * 100) >> 16;

	if ((rdata <= (edata + 10)) && (rdata >= (edata - 10))) {
		return XST_SUCCESS;
	}

	xil_printf("FMC clocks: received: %08x (%d MHz), expected: %08x (%d MHz)\r\n",
			rdata, received_clk, edata, expected_clk);
	return XST_FAILURE;
}

u32 gt_init(u32 n)
{
	u32 ret = XST_SUCCESS;
	u32 rdata;

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x004) * 0x4)), 0x0); // cpll-pd
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x005) * 0x4)), 0x0); // drp-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00f) * 0x4)), 0x0); // rx-pll-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01f) * 0x4)), 0x0); // tx-pll-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x008) * 0x4)), 0x0); // rx-gt-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x018) * 0x4)), 0x0); // tx-gt-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00e) * 0x4)), 0x0); // rx-user-ready
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01e) * 0x4)), 0x0); // tx-user-ready
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x009) * 0x4)), 0x0); // rx-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x019) * 0x4)), 0x0); // tx-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00c) * 0x4)), 0x0); // rx-sync
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01c) * 0x4)), 0x0); // tx-sync
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00b) * 0x4)), 0x0); // rx-sysref-sel, sysref
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01b) * 0x4)), 0x0); // tx-sysref-sel, sysref

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00a) * 0x4)), 0x02); // rx-sys-clk-sel, out-clk-sel
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01a) * 0x4)), 0x02); // tx-sys-clk-sel, out-clk-sel

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x005) * 0x4)), 0x3); // drp-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00f) * 0x4)), 0x1); // rx-pll-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01f) * 0x4)), 0x1); // tx-pll-reset
	delay_ms(1);

	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00d) * 0x4))); // rx-status
	if ((rdata & 0xff) != 0xff) {
		ret = XST_FAILURE;
		xil_printf("GT RX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0xff);
	}
	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01d) * 0x4))); // tx-status
	if ((rdata & 0xff) != 0xff) {
		ret = XST_FAILURE;
		xil_printf("GT TX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0xff);
	}

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x008) * 0x4)), 0x1); // rx-gt-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x018) * 0x4)), 0x1); // tx-gt-reset
	delay_ms(1);

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00e) * 0x4)), 0x1); // rx-user-ready
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01e) * 0x4)), 0x1); // tx-user-ready
	delay_ms(1);

	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00d) * 0x4))); // rx-status
	if ((rdata & 0xffff) != 0xffff) {
		ret = XST_FAILURE;
		xil_printf("GT RX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0xffff);
	}
	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01d) * 0x4))); // tx-status
	if ((rdata & 0xffff) != 0xffff) {
		ret = XST_FAILURE;
		xil_printf("GT TX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0xffff);
	}

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x009) * 0x4)), 0x1); // rx-reset
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x019) * 0x4)), 0x1); // tx-reset
	delay_ms(1);

	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00c) * 0x4)), 0x1); // rx-sync
	Xil_Out32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01c) * 0x4)), 0x1); // tx-sync
	delay_ms(1);

	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x00d) * 0x4))); // rx-status
	if ((rdata & 0x1ffff) != 0x1ffff) {
		ret = XST_FAILURE;
		xil_printf("GT RX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0x1ffff);
	}
	rdata = Xil_In32((XPAR_AXI_PZSLB_GT_BASEADDR + (((0x100 * n) + 0x01d) * 0x4))); // tx-status
	if ((rdata & 0x1ffff) != 0x1ffff) {
		ret = XST_FAILURE;
		xil_printf("GT TX[%d]: received: %08x, expected: %08x\n\r", n, rdata, 0x1ffff);
	}

	delay_ms(1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + ((0x120 + (n * 0x10) + 1) * 0x4)), 0x0);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + ((0x120 + (n * 0x10) + 1) * 0x4)), 0x3);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + ((0x120 + (n * 0x10) + 1) * 0x4)), 0x0);

	delay_ms(100);
	rdata = Xil_In32((XPAR_AXI_GPREG_BASEADDR + ((0x120 + (n * 0x10) + 2) * 0x4)));
	if (rdata != 0) {
		ret = XST_FAILURE;
		xil_printf("GT[%d]: PN Status(%02x)!\n\r", n, rdata);
	}

	return ret;
}

u32 fmc_init(void)
{
	u32 ret = XST_SUCCESS;

	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x101 * 0x4)), 0x00000000);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x111 * 0x4)), 0x00000000);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x200 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x210 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x220 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x230 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x240 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x250 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x260 * 0x4)), 0x1);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (0x270 * 0x4)), 0x1);
	delay_ms(1);

	if (gt_init(0) != XST_SUCCESS)
		ret = XST_FAILURE;

	if ((fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x202 * 0x4)), FMC_CLK0) != XST_SUCCESS) ||
			(fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x212 * 0x4)), FMC_CLK1) != XST_SUCCESS));
		ret = XST_FAILURE;
	/* fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x222 * 0x4)), 250);
	 * fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x232 * 0x4)), 125);
	 * fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x242 * 0x4)), 125);
	 * fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x252 * 0x4)), 0);
	 * fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x262 * 0x4)), 0);
	 * fmc_verify_clk(Xil_In32(XPAR_AXI_GPREG_BASEADDR + (0x272 * 0x4)), 0);
	 */

	return ret;
}

void gpio_write(u32 value) {
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (GPOUT0_OFFSET * 0x4)), value);
	Xil_Out32((XPAR_AXI_GPREG_BASEADDR + (GPOUT1_OFFSET * 0x4)), (value | 0x40000000));
}

int gpio_read_fmc(u32 pin, u32 expected) {
	int ret = XST_SUCCESS;
	u32 rdata;

	rdata = Xil_In32(XPAR_AXI_GPREG_BASEADDR + (GPIN0_OFFSET * 0x4));
	if (rdata != expected) {
		xil_printf("FMC: loopback error on pin %d: "
			"wrote 0x%08x, read 0x%08x\r\n", pin, expected, rdata);
		ret = XST_FAILURE;
	}

	if (pin < 2) {
		rdata = Xil_In32(XPAR_AXI_GPREG_BASEADDR + (GPIN1_OFFSET * 0x4));
		if ((rdata & 0x3) != (expected & 0x3)) {
			xil_printf("FMC: loopback error on pin %d: "
				"wrote 0x%08x, read 0x%08x\r\n", (pin + 32), expected, rdata);
			ret = XST_FAILURE;
		}
	}

	return ret;
}

int gpio_read_pmod(u32 pin, u32 expected) {
	int ret = XST_SUCCESS;
	u32 rdata;

	rdata = Xil_In32(XPAR_AXI_GPREG_BASEADDR + (GPIN1_OFFSET * 0x4));
	if ((rdata & 0x3c) != (expected & 0x3c)) {
		xil_printf("PMOD: loopback error on pin %d: "
			"wrote 0x%08x, read 0x%08x\r\n", pin, expected, rdata);
		ret = XST_FAILURE;
	}

	return ret;
}

int gpio_read_camera(u32 pin, u32 expected) {
	int ret = XST_SUCCESS;
	u32 rdata;

	rdata = Xil_In32(XPAR_AXI_GPREG_BASEADDR + (GPIN1_OFFSET * 0x4));
	if ((rdata & 0x3fffe0) != (expected & 0x3fffe0)) {
		xil_printf("Camera: loopback error on pin %d: "
			"wrote 0x%08x, read 0x%08x\r\n", pin, expected, rdata);
		ret = XST_FAILURE;
	}

	return ret;
}

int gpio_read_sfp(u32 pin, u32 expected) {
	int ret = XST_SUCCESS;
	u32 rdata;

	rdata = Xil_In32(XPAR_AXI_GPREG_BASEADDR + (GPIN1_OFFSET * 0x4));
	if ((rdata & 0x1c00000) != (expected & 0x1c00000)) {
		xil_printf("SFP: loopback error on pin %d: "
			"wrote 0x%08x, read 0x%08x\r\n", pin, expected, rdata);
		ret = XST_FAILURE;
	}

	return ret;
}

void gpio_wait()
{
	u32 i;

	for(i = 0; i < GPIO_WAIT_TIME; i++) {
		asm("nop");
	}
}

int main()
{
	int ret = XST_SUCCESS;
	u32 n, wdata;

	Xil_DCacheDisable();
	Xil_ICacheDisable();

	fmc_init();

	/* walking 1 on FMC */
	for(n = 0; n < 32; n++) {
		wdata = 1 << n;
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_fmc(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* walking 0 on FMC */
	for(n = 0; n < 32; n++) {
		wdata = 1 << n;
		wdata = ~wdata;
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_fmc(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* walking 1 on PMOD */
	for(n = 0; n < 4; n++) {
		wdata = 1 << (n + 2);
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_pmod(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* walking 0 on PMOD */
	for(n = 0; n < 4; n++) {
		wdata = 1 << (n + 2);
		wdata = ~wdata;
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_pmod(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* walking 1 on camera */
	for(n = 0; n < 17; n++) {
		wdata = 1 << (n + 5);
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_camera(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* walking 0 on camera */
	for(n = 0; n < 17; n++) {
		wdata = 1 << (n + 5);
		wdata = ~wdata;
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_camera(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}

	/* Initialize GT for SFP */
	if (gt_init(1) != XST_SUCCESS)
		ret = XST_FAILURE;

	/* SFP pins aren't looped back normally */
	/* walking 1 on SFP */
	/*for(n = 0; n < 3; n++) {
		wdata = 1 << (n + 22);
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_sfp(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}*/

	/* walking 0 on SFP */
	/*for(n = 0; n < 3; n++) {
		wdata = 1 << (n + 22);
		wdata = ~wdata;
		gpio_write(wdata);
		gpio_wait();
		if (gpio_read_sfp(n, wdata) != XST_SUCCESS)
			ret = XST_FAILURE;
	}*/

	return ret;
}
