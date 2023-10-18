/*****************************************************************************
* | File      	:	LCD_Driver.c
* | Author      :   Waveshare team
* | Function    :	ILI9486 Drive function
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2018-01-11
* | Info        :   Basic version
*
******************************************************************************/

/**************************Intermediate driver layer**************************/
#include "LCD_Driver.h"
#include "Debug.h"
#include "no_os_delay.h"
#include "gpio.h"
#include "no_os_spi.h"
#include "maxim_spi.h"
#include "no_os_gpio.h"
#include "maxim_gpio.h"

LCD_DIS sLCD_DIS;
struct no_os_spi_desc *lcd;
struct no_os_gpio_desc *bl;
struct no_os_gpio_desc *rst;
struct no_os_gpio_desc *dc;
struct no_os_gpio_desc *cs;


void mylcdinit()
{
	struct max_spi_init_param max_spi_ip = {
		.num_slaves = 1,
		.polarity = SPI_SS_POL_LOW,
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};
	const struct no_os_spi_init_param lcd_config = {
		.device_id = 1,
		.max_speed_hz = 25000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.platform_ops = &max_spi_ops,
		.chip_select = 2, // useless
		.extra = &max_spi_ip,
	};
	no_os_spi_init(&lcd, &lcd_config);

	// backlight 
	const struct no_os_gpio_init_param bl_config =
	{
		.port = 1,
		.number = 30,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param) {
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		},
	};
	no_os_gpio_get(&bl, &bl_config);
	no_os_gpio_direction_output(bl, NO_OS_GPIO_HIGH);

	// reset
	const struct no_os_gpio_init_param reset_config =
	{
		.port = 1,
		.number = 31,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param) {
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		},
	};
	no_os_gpio_get(&rst, &reset_config);
	no_os_gpio_direction_output(rst, NO_OS_GPIO_HIGH);

	// data-command
	const struct no_os_gpio_init_param dc_config =
	{
		.port = 1,
		.number = 25,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param) {
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		},
	};
	no_os_gpio_get(&dc, &dc_config);
	no_os_gpio_direction_output(dc, NO_OS_GPIO_HIGH);

	// chip-select
	const struct no_os_gpio_init_param cs_config =
	{
		.port = 1,
		.number = 23,
		.platform_ops = &max_gpio_ops,
		.extra = &(struct max_gpio_init_param) {
			.vssel = MXC_GPIO_VSSEL_VDDIOH,
		},
	};
	no_os_gpio_get(&cs, &cs_config);
	no_os_gpio_direction_output(cs, NO_OS_GPIO_HIGH);

	return;
}

/*******************************************************************************
function:
	Hardware reset
*******************************************************************************/
static void LCD_Reset(void)
{
	no_os_gpio_set_value(rst, NO_OS_GPIO_HIGH);
	no_os_mdelay(1);
	no_os_gpio_set_value(rst, NO_OS_GPIO_LOW);
	no_os_mdelay(120);
	no_os_gpio_set_value(rst, NO_OS_GPIO_HIGH);
}

static void LCD_SetBackLight(uint16_t value)
{
	// PWM_SetValue(value);
}
/*******************************************************************************
function:
		Write register address and data
*******************************************************************************/
void LCD_WriteReg(uint8_t Reg)
{
	no_os_gpio_set_value(dc, NO_OS_GPIO_LOW);
	no_os_gpio_set_value(cs, NO_OS_GPIO_LOW);
 	no_os_spi_write_and_read(lcd, &Reg, 1);
	no_os_gpio_set_value(cs, NO_OS_GPIO_HIGH);
}

void LCD_WriteData(uint16_t Data)
{
	uint8_t data[2] = {Data >> 8, Data};
	no_os_gpio_set_value(dc, NO_OS_GPIO_HIGH);
	no_os_gpio_set_value(cs, NO_OS_GPIO_LOW);
 	no_os_spi_write_and_read(lcd, data, 2);
	no_os_gpio_set_value(cs, NO_OS_GPIO_HIGH);
}

/*******************************************************************************
function:
		Common register initialization
*******************************************************************************/
static void LCD_InitReg(void)
{
    LCD_WriteReg(0XF9);
    LCD_WriteData(0x00);
    LCD_WriteData(0x08);

    LCD_WriteReg(0xC0);
    LCD_WriteData(0x19);//VREG1OUT POSITIVE
    LCD_WriteData(0x1a);//VREG2OUT NEGATIVE

    LCD_WriteReg(0xC1);
    LCD_WriteData(0x45);//VGH,VGL    VGH>=14V.
    LCD_WriteData(0x00);

    LCD_WriteReg(0xC2);	//Normal mode, increase can change the display quality, while increasing power consumption
    LCD_WriteData(0x33);

    LCD_WriteReg(0XC5);
    LCD_WriteData(0x00);
    LCD_WriteData(0x28);//VCM_REG[7:0]. <=0X80.

    LCD_WriteReg(0xB1);//Sets the frame frequency of full color normal mode
    LCD_WriteData(0xA0);//0XB0 =70HZ, <=0XB0.0xA0=62HZ
    LCD_WriteData(0x11);

    LCD_WriteReg(0xB4);
    LCD_WriteData(0x02); //2 DOT FRAME MODE,F<=70HZ.

    LCD_WriteReg(0xB6);//
    LCD_WriteData(0x00);
    LCD_WriteData(0x42);//0 GS SS SM ISC[3:0];
    LCD_WriteData(0x3B);

    LCD_WriteReg(0xB7);
    LCD_WriteData(0x07);

    LCD_WriteReg(0xE0);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x25);
    LCD_WriteData(0x22);
    LCD_WriteData(0x0B);
    LCD_WriteData(0x06);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x4E);
    LCD_WriteData(0xC6);
    LCD_WriteData(0x39);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XE1);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x1F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x46);
    LCD_WriteData(0x49);
    LCD_WriteData(0x31);
    LCD_WriteData(0x05);
    LCD_WriteData(0x09);
    LCD_WriteData(0x03);
    LCD_WriteData(0x1C);
    LCD_WriteData(0x1A);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XF1);
    LCD_WriteData(0x36);
    LCD_WriteData(0x04);
    LCD_WriteData(0x00);
    LCD_WriteData(0x3C);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x0F);
    LCD_WriteData(0xA4);
    LCD_WriteData(0x02);

    LCD_WriteReg(0XF2);
    LCD_WriteData(0x18);
    LCD_WriteData(0xA3);
    LCD_WriteData(0x12);
    LCD_WriteData(0x02);
    LCD_WriteData(0x32);
    LCD_WriteData(0x12);
    LCD_WriteData(0xFF);
    LCD_WriteData(0x32);
    LCD_WriteData(0x00);

    LCD_WriteReg(0XF4);
    LCD_WriteData(0x40);
    LCD_WriteData(0x00);
    LCD_WriteData(0x08);
    LCD_WriteData(0x91);
    LCD_WriteData(0x04);

    LCD_WriteReg(0XF8);
    LCD_WriteData(0x21);
    LCD_WriteData(0x04);

    LCD_WriteReg(0X3A);	//Set Interface Pixel Format
    LCD_WriteData(0x55);

}

/********************************************************************************
function:	Set the display scan and color transfer modes
parameter:
		Scan_dir   :   Scan direction
		Colorchose :   RGB or GBR color format
********************************************************************************/
void LCD_SetGramScanWay(LCD_SCAN_DIR Scan_dir)
{
    uint16_t MemoryAccessReg_Data = 0; //addr:0x36
    uint16_t DisFunReg_Data = 0; //addr:0xB6

    // Gets the scan direction of GRAM
    switch (Scan_dir) {
    case L2R_U2D:
        MemoryAccessReg_Data = 0x08;//0x08 | 0X8
        DisFunReg_Data = 0x22;
        break;
    case L2R_D2U:
        MemoryAccessReg_Data = 0x08;
        DisFunReg_Data = 0x62;
        break;
    case R2L_U2D: //0X4
        MemoryAccessReg_Data = 0x08;
        DisFunReg_Data = 0x02;
        break;
    case R2L_D2U: //0XC
        MemoryAccessReg_Data = 0x08;
        DisFunReg_Data = 0x42;
        break;
    case U2D_L2R: //0X2
        MemoryAccessReg_Data = 0x28;
        DisFunReg_Data = 0x22;
        break;
    case U2D_R2L: //0X6
        MemoryAccessReg_Data = 0x28;
        DisFunReg_Data = 0x02;
        break;
    case D2U_L2R: //0XA
        MemoryAccessReg_Data = 0x28;
        DisFunReg_Data = 0x62;
        break;
    case D2U_R2L: //0XE
        MemoryAccessReg_Data = 0x28;
        DisFunReg_Data = 0x42;
        break;
    }

    //Get the screen scan direction
    sLCD_DIS.LCD_Scan_Dir = Scan_dir;

    //Get GRAM and LCD width and height
    if(Scan_dir == L2R_U2D || Scan_dir == L2R_D2U || Scan_dir == R2L_U2D || Scan_dir == R2L_D2U) {
        sLCD_DIS.LCD_Dis_Column	= LCD_HEIGHT ;
        sLCD_DIS.LCD_Dis_Page = LCD_WIDTH ;
    } else {
        sLCD_DIS.LCD_Dis_Column	= LCD_WIDTH ;
        sLCD_DIS.LCD_Dis_Page = LCD_HEIGHT ;
    }

    // Set the read / write scan direction of the frame memory
    LCD_WriteReg(0xB6);
    LCD_WriteData(0X00);
    LCD_WriteData(DisFunReg_Data);

    LCD_WriteReg(0x36);
    LCD_WriteData(MemoryAccessReg_Data);
}

/********************************************************************************
function:
	initialization
********************************************************************************/
void LCD_Init(LCD_SCAN_DIR LCD_ScanDir, uint16_t LCD_BLval)
{
    mylcdinit();

    //Hardware reset
    LCD_Reset();

	if(LCD_BLval > 255)
		LCD_BLval = 255;
	LCD_SetBackLight(LCD_BLval);
	
    //Set the initialization register
    LCD_InitReg();

    //Set the display scan and color transfer modes
    LCD_SetGramScanWay( LCD_ScanDir);
    no_os_mdelay(200);

    //sleep out
    LCD_WriteReg(0x11);
    no_os_mdelay(120);

    //Turn on the LCD display
    LCD_WriteReg(0x29);
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
	Xstart 	:   X direction Start coordinates
	Ystart  :   Y direction Start coordinates
	Xend    :   X direction end coordinates
	Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_SetWindow(POINT Xstart, POINT Ystart,	POINT Xend, POINT Yend)
{
    //set the X coordinates
    LCD_WriteReg(0x2A);
    LCD_WriteData(Xstart >> 8);	 				//Set the horizontal starting point to the high octet
    LCD_WriteData(Xstart & 0xff);	 				//Set the horizontal starting point to the low octet
    LCD_WriteData((Xend - 1) >> 8);	//Set the horizontal end to the high octet
    LCD_WriteData((Xend - 1) & 0xff);	//Set the horizontal end to the low octet

    //set the Y coordinates
    LCD_WriteReg(0x2B);
    LCD_WriteData(Ystart >> 8);
    LCD_WriteData(Ystart & 0xff );
    LCD_WriteData((Yend - 1) >> 8);
    LCD_WriteData((Yend - 1) & 0xff);
    LCD_WriteReg(0x2C);
}

/********************************************************************************
function:	Set the display point (Xpoint, Ypoint)
parameter:
	xStart :   X direction Start coordinates
	xEnd   :   X direction end coordinates
********************************************************************************/
void LCD_SetCursor(POINT Xpoint, POINT Ypoint)
{
    LCD_SetWindow(Xpoint, Ypoint, Xpoint, Ypoint);
}

/********************************************************************************
function:	Set show color
parameter:
		Color  :   Set show color,16-bit depth
********************************************************************************/
void LCD_SetColor(COLOR Color , POINT Xpoint, POINT Ypoint)
{
	uint32_t i;
	for(i = 0; i < Xpoint * Ypoint; i++) {
		no_os_udelay(1);
		LCD_WriteData(Color);
	}
}

/********************************************************************************
function:	Point (Xpoint, Ypoint) Fill the color
parameter:
	Xpoint :   The x coordinate of the point
	Ypoint :   The y coordinate of the point
	Color  :   Set the color
********************************************************************************/
void LCD_SetPointlColor( POINT Xpoint, POINT Ypoint, COLOR Color)
{
    if ((Xpoint <= sLCD_DIS.LCD_Dis_Column) && (Ypoint <= sLCD_DIS.LCD_Dis_Page)) {
        LCD_SetCursor (Xpoint, Ypoint);
        LCD_SetColor(Color, 1, 1);
    }
}

/********************************************************************************
function:	Fill the area with the color
parameter:
	Xstart :   Start point x coordinate
	Ystart :   Start point y coordinate
	Xend   :   End point coordinates
	Yend   :   End point coordinates
	Color  :   Set the color
********************************************************************************/
void LCD_SetArealColor(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,	COLOR Color)
{
    if((Xend > Xstart) && (Yend > Ystart)) {
        LCD_SetWindow(Xstart , Ystart , Xend , Yend  );
        LCD_SetColor ( Color , Xend - Xstart, Yend - Ystart);
    }
}

/********************************************************************************
function:
			Clear screen
********************************************************************************/
void LCD_Clear(COLOR  Color)
{
    LCD_SetArealColor(0, 0, sLCD_DIS.LCD_Dis_Column , sLCD_DIS.LCD_Dis_Page , Color);
}
