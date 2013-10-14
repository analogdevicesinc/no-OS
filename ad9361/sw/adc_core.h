
#ifndef ADC_CORE_API_H_
#define ADC_CORE_API_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADI_REG_RSTN			0x0040
#define ADI_RSTN				(1 << 0)

#define ADI_REG_STATUS			0x005C
#define ADI_MUX_PN_ERR			(1 << 3)
#define ADI_MUX_PN_OOS			(1 << 2)
#define ADI_MUX_OVER_RANGE		(1 << 1)
#define ADI_STATUS				(1 << 0)

#define ADI_REG_DMA_CNTRL		0x0080
#define ADI_DMA_STREAM			(1 << 1)
#define ADI_DMA_START			(1 << 0)

#define ADI_REG_DMA_COUNT		0x0084
#define ADI_DMA_COUNT(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_DMA_COUNT(x)		(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_DMA_STATUS		0x0088
#define ADI_DMA_OVF				(1 << 2)
#define ADI_DMA_UNF				(1 << 1)
#define ADI_DMA_STATUS			(1 << 0)

#define ADI_REG_DMA_BUSWIDTH	0x008C
#define ADI_DMA_BUSWIDTH(x)		(((x) & 0xFFFFFFFF) << 0)
#define ADI_TO_DMA_BUSWIDTH(x)	(((x) >> 0) & 0xFFFFFFFF)

#define ADI_REG_CHAN_CNTRL(c)	(0x0400 + (c) * 0x40)
#define ADI_IQCOR_ENB			(1 << 9)
#define ADI_DCFILT_ENB			(1 << 8)
#define ADI_FORMAT_SIGNEXT		(1 << 6)
#define ADI_FORMAT_TYPE			(1 << 5)
#define ADI_FORMAT_ENABLE		(1 << 4)
#define ADI_PN23_TYPE			(1 << 1)
#define ADI_ENABLE				(1 << 0)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void adc_init(void);
void adc_capture(uint32_t qwcnt, uint32_t sa);

#endif
