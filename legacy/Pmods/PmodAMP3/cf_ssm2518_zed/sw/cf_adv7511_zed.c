// ***************************************************************************
// ***************************************************************************
// Rejeesh.Kutty@Analog.com (c) Analog Devices Inc.
// ***************************************************************************
// ***************************************************************************

#include <stdio.h>
#include <xil_io.h>
#include "xiicps.h"
#include "xuartps.h"
#include "platform.h"
#include "xparameters_ps.h"
#include "xbasic_types.h"
#include "cf_adv7511_zed.h"

#define CF_CLKGEN_BASEADDR  0x66000000
#define CFV_BASEADDR        0x6c000000
#define CFA_BASEADDR        0x75c00000
#define DDR_BASEADDR        0x00000000
#define UART_BASEADDR       0xe0001000
#define VDMA_BASEADDR       0x43000000
#define ADMA_BASEADDR       0x40400000
#define IIC_BASEADDR        0x41600000
#define H_STRIDE            1920
#define H_COUNT             2200
#define H_ACTIVE            1920
#define H_WIDTH             44
#define H_FP                88
#define H_BP                148
#define V_COUNT             1125
#define V_ACTIVE            1080
#define V_WIDTH             5
#define V_FP                4
#define V_BP                36
#define A_SAMPLE_FREQ       48000
#define A_FREQ              1400

#define H_DE_MIN (H_WIDTH+H_BP)
#define H_DE_MAX (H_WIDTH+H_BP+H_ACTIVE)
#define V_DE_MIN (V_WIDTH+V_BP)
#define V_DE_MAX (V_WIDTH+V_BP+V_ACTIVE)
#define VIDEO_LENGTH  (H_ACTIVE*V_ACTIVE)
#define AUDIO_LENGTH  (A_SAMPLE_FREQ/A_FREQ)
#define VIDEO_BASEADDR DDR_BASEADDR + 0x2000000
#define AUDIO_BASEADDR DDR_BASEADDR + 0x1000000

//#define HDMI_MODE
#define CSC_ENABLE
#define IN_YCRCB
//#define CSC_SET1
#define CSC_SET2

// ***************************************************************************
// ***************************************************************************

void xil_printf( const char *ctrl1, ...);
void Xil_DCacheFlush(void);
char inbyte(void);

void delay_ms(u32 ms_count) {
  u32 count;
  for (count = 0; count < ((ms_count * 800000) + 1); count++) {
    asm("nop");
  }
}

u32 user_exit(void) {
  while (XUartPs_IsReceiveData(UART_BASEADDR)) {
    if (inbyte() == 'q') {
      return(1);
    }
  }
  return(0);
}

void iic_write(u32 daddr, u32 waddr, u32 wdata) {
  Xil_Out32((IIC_BASEADDR + 0x100), 0x002); // reset tx fifo
  Xil_Out32((IIC_BASEADDR + 0x100), 0x001); // enable iic
  Xil_Out32((IIC_BASEADDR + 0x108), (0x100 | (daddr<<1))); // select
  Xil_Out32((IIC_BASEADDR + 0x108), waddr); // address
  Xil_Out32((IIC_BASEADDR + 0x108), (0x200 | wdata)); // data
  while ((Xil_In32(IIC_BASEADDR + 0x104) & 0x80) == 0x00) {delay_ms(1);}
  delay_ms(10);
}

u32 iic_read(u32 daddr, u32 raddr, u32 display) {
  u32 rdata;
  Xil_Out32((IIC_BASEADDR + 0x100), 0x002); // reset tx fifo
  Xil_Out32((IIC_BASEADDR + 0x100), 0x001); // enable iic
  Xil_Out32((IIC_BASEADDR + 0x108), (0x100 | (daddr<<1))); // select
  Xil_Out32((IIC_BASEADDR + 0x108), raddr); // address
  Xil_Out32((IIC_BASEADDR + 0x108), (0x101 | (daddr<<1))); // select
  Xil_Out32((IIC_BASEADDR + 0x108), 0x201); // data
  while ((Xil_In32(IIC_BASEADDR + 0x104) & 0x40) == 0x40) {delay_ms(1);}
  delay_ms(10);
  rdata = Xil_In32(IIC_BASEADDR + 0x10c) & 0xff;
  if (display == 1) {
    xil_printf("iic_read: addr(%02x) data(%02x)\n\r", raddr, rdata);
  }
  delay_ms(10);
  return(rdata);
}

void ddr_video_wr() {

  u32 n;
  u32 d;
  u32 dcnt;
  u32 r;
  u32 g;
  u32 b;

  dcnt = 0;
  xil_printf("DDR write: started (length %d)\n\r", IMG_LENGTH);
  for (n = 0; n < IMG_LENGTH; n++) {
    for (d = 0; d < ((IMG_DATA[n]>>24) & 0xff); d++) {
      //r = (IMG_DATA[n] & 0x0000ff);
      //g = (IMG_DATA[n] & 0x00ff00);
      //b = ((IMG_DATA[n] & 0xff0000) >> 16);
      //Xil_Out32((VIDEO_BASEADDR+(dcnt*4)), (IMG_DATA[n] & 0x00ff00) | b << 16 | r);
      Xil_Out32((VIDEO_BASEADDR+(dcnt*4)), (IMG_DATA[n] & 0xffffff));
      dcnt = dcnt + 1;
    }
  }
  Xil_DCacheFlush();
  xil_printf("DDR write: completed (total %d)\n\r", dcnt);
}

void ddr_audio_wr() {

  u32 n;
  u32 scnt;
  u32 sincr;

  xil_printf("DDR audio write: started\n\r");
  scnt = 0;
  sincr = (65536*2)/AUDIO_LENGTH;
  for (n = 0; n < 32; n++) {
	  Xil_Out32((AUDIO_BASEADDR+(n*4)), 0x00); // init descriptors
  }
  Xil_Out32((AUDIO_BASEADDR+0x00), (AUDIO_BASEADDR + 0x40)); // next descriptor
  Xil_Out32((AUDIO_BASEADDR+0x08), (AUDIO_BASEADDR + 0x80)); // start address
  Xil_Out32((AUDIO_BASEADDR+0x40), (AUDIO_BASEADDR + 0x00)); // next descriptor
  Xil_Out32((AUDIO_BASEADDR+0x48), (AUDIO_BASEADDR + 0x80)); // start address
  Xil_Out32((AUDIO_BASEADDR+0x18), (0x8000000 | (AUDIO_LENGTH*8))); // no. of bytes
  Xil_Out32((AUDIO_BASEADDR+0x58), (0x4000000 | (AUDIO_LENGTH*8))); // no. of bytes
  Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
  Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
  for (n = 0; n < AUDIO_LENGTH; n++) {
    Xil_Out32((AUDIO_BASEADDR+0x80+(n*4)), ((scnt << 16) | scnt));
    scnt = (n > (AUDIO_LENGTH/2)) ? (scnt-sincr) : (scnt+sincr);
  }
  Xil_DCacheFlush();
  xil_printf("DDR audio write: completed (total %d)\n\r", AUDIO_LENGTH);
}

// ***************************************************************************
// ***************************************************************************

int main() {

  u32 data;

  init_platform();
  ddr_video_wr();
  ddr_audio_wr();

  data = Xil_In32(CF_CLKGEN_BASEADDR + (0x1f*4));
  if ((data & 0x1) == 0x0) {
    xil_printf("CLKGEN (148.5MHz) out of lock (0x%04x)\n\r", data);
    return(0);
  }

  Xil_Out32((VDMA_BASEADDR + 0x000), 0x00000003); // enable circular mode
  Xil_Out32((VDMA_BASEADDR + 0x05c), VIDEO_BASEADDR); // start address
  Xil_Out32((VDMA_BASEADDR + 0x060), VIDEO_BASEADDR); // start address
  Xil_Out32((VDMA_BASEADDR + 0x064), VIDEO_BASEADDR); // start address
  Xil_Out32((VDMA_BASEADDR + 0x058), (H_STRIDE*4)); // h offset (2048 * 4) bytes
  Xil_Out32((VDMA_BASEADDR + 0x054), (H_ACTIVE*4)); // h size (1920 * 4) bytes
  Xil_Out32((VDMA_BASEADDR + 0x050), V_ACTIVE); // v size (1080)

  Xil_Out32((CFV_BASEADDR + 0x08), ((H_WIDTH << 16) | H_COUNT));
  Xil_Out32((CFV_BASEADDR + 0x0c), ((H_DE_MIN << 16) | H_DE_MAX));
  Xil_Out32((CFV_BASEADDR + 0x10), ((V_WIDTH << 16) | V_COUNT));
  Xil_Out32((CFV_BASEADDR + 0x14), ((V_DE_MIN << 16) | V_DE_MAX));
  Xil_Out32((CFV_BASEADDR + 0x04), 0x00000000); // disable
  Xil_Out32((CFV_BASEADDR + 0x04), 0x00000001); // enable
  //Xil_Out32((CFV_BASEADDR + 0x04), 0x00000002); // disable
  //Xil_Out32((CFV_BASEADDR + 0x04), 0x00000003); // enable

  Xil_Out32((CFA_BASEADDR + 0x04), 0x040); // sample frequency
  Xil_Out32((CFA_BASEADDR + 0x00), 0x103); // clock ratio, data enable & signal enable

  // wait for hpd

  while ((iic_read(0x39, 0x96, 0x00) & 0x80) != 0x80) {
    delay_ms(1);
  }

  iic_write(0x39, 0x01, 0x00);
  iic_write(0x39, 0x02, 0x18);
  iic_write(0x39, 0x03, 0x00);

  // I2S sampling freq. 44.1KHz, Input format 16b, 4:2:2, YCrCb
  iic_write(0x39, 0x15, 0x01);

#ifdef IN_YCRCB
  // Output format 4:2:2, 8-bit, style 2, DDR falling, Output color space = YCrCb
  iic_write(0x39, 0x16, 0xb5);
#else
  // Output format 4:2:2, 8-bit, style 2, DDR falling, Output color space = RGB
  iic_write(0x39, 0x16, 0xb4);
#endif

#ifndef CSC_ENABLE
  // Color Space Converter disabled
  iic_write(0x39, 0x18, 0x46);
#else

#ifdef CSC_SET1
  iic_write(0x39, 0x18, 0xE7);
  //iic_write(0x39, 0x18, 0x87);
  iic_write(0x39, 0x19, 0x34);
  iic_write(0x39, 0x1A, 0x04);
  iic_write(0x39, 0x1B, 0xAD);
  iic_write(0x39, 0x1C, 0x00);
  iic_write(0x39, 0x1D, 0x00);
  iic_write(0x39, 0x1E, 0x1C);
  iic_write(0x39, 0x1F, 0x1B);

  iic_write(0x39, 0x20, 0x1D);
  iic_write(0x39, 0x21, 0xDC);
  iic_write(0x39, 0x22, 0x04);
  iic_write(0x39, 0x23, 0xAD);
  iic_write(0x39, 0x24, 0x1F);
  iic_write(0x39, 0x25, 0x24);
  iic_write(0x39, 0x26, 0x01);
  iic_write(0x39, 0x27, 0x35);

  iic_write(0x39, 0x28, 0x00);
  iic_write(0x39, 0x29, 0x00);
  iic_write(0x39, 0x2A, 0x04);
  iic_write(0x39, 0x2B, 0xAD);
  iic_write(0x39, 0x2C, 0x08);
  iic_write(0x39, 0x2D, 0x7C);
  iic_write(0x39, 0x2E, 0x1B);
  iic_write(0x39, 0x2F, 0x77);
#endif

#ifdef CSC_SET2
  iic_write(0x39, 0x18, 0xE6);
  iic_write(0x39, 0x19, 0x69);
  iic_write(0x39, 0x1A, 0x04);
  iic_write(0x39, 0x1B, 0xAC);
  iic_write(0x39, 0x1C, 0x00);
  iic_write(0x39, 0x1D, 0x00);
  iic_write(0x39, 0x1E, 0x1C);
  iic_write(0x39, 0x1F, 0x81);

  iic_write(0x39, 0x20, 0x1c);
  iic_write(0x39, 0x21, 0xbC);
  iic_write(0x39, 0x22, 0x04);
  iic_write(0x39, 0x23, 0xAD);
  iic_write(0x39, 0x24, 0x1e);
  iic_write(0x39, 0x25, 0x6e);
  iic_write(0x39, 0x26, 0x02);
  iic_write(0x39, 0x27, 0x20);

  iic_write(0x39, 0x28, 0x1f);
  iic_write(0x39, 0x29, 0xfe);
  iic_write(0x39, 0x2A, 0x04);
  iic_write(0x39, 0x2B, 0xAD);
  iic_write(0x39, 0x2C, 0x08);
  iic_write(0x39, 0x2D, 0x1a);
  iic_write(0x39, 0x2E, 0x1b);
  iic_write(0x39, 0x2F, 0xA9);
#endif

#endif

  iic_write(0x39, 0x40, 0x80);
  iic_write(0x39, 0x41, 0x10);
  iic_write(0x39, 0x48, 0x08);
  iic_write(0x39, 0x49, 0xa8);

  iic_write(0x39, 0x4c, 0x00);

  //iic_write(0x39, 0x55, 0x20);			// Y1Y0 (AVI InfoFrame) = YCbCr 422
  iic_write(0x39, 0x55, 0x00);		// Y1Y0 (AVI InfoFrame) = RGB

  iic_write(0x39, 0x56, 0x08);
  iic_write(0x39, 0x96, 0x20);
  iic_write(0x39, 0x98, 0x03);
  iic_write(0x39, 0x99, 0x02);
  iic_write(0x39, 0x9a, 0xe0);
  iic_write(0x39, 0x9c, 0x30);
  iic_write(0x39, 0x9d, 0x61);
  iic_write(0x39, 0xa2, 0xa4);
  iic_write(0x39, 0xa3, 0xa4);
    iic_write(0x39, 0xa4, 0x04);
  iic_write(0x39, 0xa5, 0x44);
  iic_write(0x39, 0xab, 0x40);

#ifdef HDMI_MODE
  iic_write(0x39, 0xaf, 0x06);			// HDCP disabled, HDMI mode
#else
  iic_write(0x39, 0xaf, 0x04);		// HDCP disabled, DVI mode
#endif

  iic_write(0x39, 0xba, 0x00);
  iic_write(0x39, 0xd0, 0x3c);
  iic_write(0x39, 0xd1, 0xff);
  iic_write(0x39, 0xde, 0x9c);
  iic_write(0x39, 0xe0, 0xd0);
  iic_write(0x39, 0xe4, 0x60);
  iic_write(0x39, 0xf9, 0x00);
  iic_write(0x39, 0xfa, 0x00);
  iic_write(0x39, 0x17, 0x02);

  iic_write(0x39, 0x0a, 0x10);
  iic_write(0x39, 0x0b, 0x8e);
  iic_write(0x39, 0x0c, 0x00);
  iic_write(0x39, 0x73, 0x01);
  iic_write(0x39, 0x14, 0x02);

  iic_read(0x39, 0x42, 0x01);
  iic_read(0x39, 0xc8, 0x01);
  iic_read(0x39, 0x9e, 0x01);
  iic_read(0x39, 0x96, 0x01);
  iic_read(0x39, 0x3e, 0x01);
  iic_read(0x39, 0x3d, 0x01);
  iic_read(0x39, 0x3c, 0x01);

  Xil_Out32((CFV_BASEADDR + 0x18), 0xff); // clear status

  xil_printf("Generating audio clicks (press 'q' to exit).\n\r");
  while (user_exit() == 0) {
    Xil_Out32((AUDIO_BASEADDR+0x1c), 0x00); // status
    Xil_Out32((AUDIO_BASEADDR+0x5c), 0x00); // status
    Xil_DCacheFlush();
    Xil_Out32((ADMA_BASEADDR + 0x00), 0); // clear dma operations
    Xil_Out32((ADMA_BASEADDR + 0x08), AUDIO_BASEADDR); // head descr.
	  Xil_Out32((ADMA_BASEADDR + 0x00), 1); // enable dma operations
    Xil_Out32((ADMA_BASEADDR + 0x10), (AUDIO_BASEADDR+0x40)); // tail descr.
    delay_ms(100);
  }

  xil_printf("done.\n\r");
  cleanup_platform();
  return(0);
}

// ***************************************************************************
// ***************************************************************************

