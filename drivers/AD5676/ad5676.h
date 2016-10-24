#ifndef AD5676_H_
#define AD5676_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define AD5676_CMD_NOP					0x00 					// No operation.
#define AD5676_CMD_WR_IN_REG(x)			(0x10 | ((x) & 0xF))	// Write to Input Register x.
#define AD5676_CMD_UPDATE_DAC_REG(x)    (0x20 | ((x) & 0xF))	// Update DAC Register x.
#define AD5676_CMD_WR_DAC_REG(x)		(0x30 | ((x) & 0xF))	// Write to and update DAC Channel x.
#define AD5676_CMD_WR_PWR_DAC			0x40					// Power down/power up DAC.
#define AD5676_CMD_HW_LDAC_MASK			0x50					// Hardware LDAC mask register.
#define AD5676_CMD_SW_RESET			    0x60					// Software reset(power-on reset).
#define AD5676_CMD_INT_REF_SETUP		0x70					// Internal reference setup register.	
#define AD5676_CMD_SET_DCEN				0x80					// Set up DCEN register.
#define AD5676_CMD_SET_READBACK(x)		(0x90 | ((x) & 0xF))	// Set up readback register x.
#define AD5676_CMD_UPDATE_IN_REG_ALL	0xA0					// Update all channels of the Input register
#define AD5676_CMD_UPDATE_DAC_REG_ALL	0xB0					// Update all channels of the DAC register and Input register

/*************************** SPI specific macros *****************************/

/* AD5676_CMD_INT_REF_SETUP */
#define AD5676_CFG_GAIN(x)					(((x) & 0x1) << 2)
#define AD5676_CFG_INT_REF(x)				(((x) & 0x1) << 0)

#define AD5676_CMD_READBACK					0x90

/* Power Setup Register definition */
#define AD5676_PWR_PD7(x)      (((x) & 0x3) << 14)
#define AD5676_PWR_PD6(x)      (((x) & 0x3) << 12)
#define AD5676_PWR_PD5(x)      (((x) & 0x3) << 10)
#define AD5676_PWR_PD4(x)      (((x) & 0x3) << 8)
#define AD5676_PWR_PD3(x)      (((x) & 0x3) << 6)
#define AD5676_PWR_PD2(x)      (((x) & 0x3) << 4)
#define AD5676_PWR_PD1(x)      (((x) & 0x3) << 2)
#define AD5676_PWR_PD0(x)      (((x) & 0x3) << 0)

/*************************** I2C specific macros *****************************/

/* Define I2C slave hardwired address */
#define AD5687_I2C_HARDW_ADDR          		 0x0E // A1 = 1, A0 = 0
#define AD5687_I2C_HARDW_ADDR_2          	 0x0C // A1 = 0, A0 = 0

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* AD567x Versions */
typedef enum {
	ID_AD5676,		// SPI, 16-bit DAC, no internal voltage reference.
	ID_AD5676R,		// SPI, 16-bit DAC, with internal voltage reference.
	ID_AD5672R,		// SPI, 12-bit DAC, with internal voltage reference.	
	ID_AD5675,		// I2C, 16-bit DAC, no internal voltage reference.
	ID_AD5675R,		// I2C, 16-bit DAC, with internal voltage reference.
	ID_AD5671R,		// I2C, 12-bit DAC, with internal voltage reference.
	
	ID_AD5686,		// SPI, 16-bit DAC, no internal voltage reference.
	ID_AD5686R,		// SPI, 16-bit DAC, with internal voltage reference.
	ID_AD5685R,		// SPI, 14-bit DAC, with internal voltage reference.
	ID_AD5684,		// SPI, 12-bit DAC, no internal voltage reference.
	ID_AD5684R,		// SPI, 12-bit DAC, with internal voltage reference.
	
	ID_AD5696,		// I2C, 16-bit DAC, no internal voltage reference.
	ID_AD5696R,		// I2C, 16-bit DAC, with internal voltage reference.
	ID_AD5695R,		// I2C, 14-bit DAC, with internal voltage reference.
	ID_AD5694,		// I2C, 12-bit DAC, no internal voltage reference.
	ID_AD5694R,		// I2C, 12-bit DAC, with internal voltage reference.
}ad5676_supported_device_ids;

typedef enum{
    SPI,
    I2C
} ad5676_comm_type;

struct ad5676_chip_info {
	uint8_t *dev_name;
    uint8_t resolution;
    ad5676_comm_type communication;
};

typedef enum {
	AD5676_DISABLE,
	AD5676_ENABLE
} ad5676_state;

typedef enum {
	AD5676_INT_REF_ON,
	AD5676_INT_REF_OFF,
} ad5676_voltage_ref;

typedef enum {
	AD5676_AMP_GAIN_1,
	AD5676_AMP_GAIN_2,
} ad5676_amp_gain;

typedef enum {
	GND_GND,			// A1 = 0, A0 = 0
	GND_VLOGIC,			// A1 = 0, A0 = 1
	VLOGIC_GND,			// A1 = 1, A0 = 0
	VLOGIC_VLOGIC,		// A1 = 1, A0 = 1
} ad5676_i2c_dev_addr;

typedef enum {
	AD5676_PWR_NORMAL,
	AD5676_PD_1K,
	AD5676_PD_3STATE = 0x03
} ad5676_power_mode;

typedef enum {
	AD5676_DAC_0,
	AD5676_DAC_1,
	AD5676_DAC_2,
	AD5676_DAC_3,
	AD5676_DAC_4,
	AD5676_DAC_5,
	AD5676_DAC_6,
	AD5676_DAC_7
} ad5676_dac;

typedef struct {
	/* SPI */
	spi_device					spi_dev;
	/* I2C */
	i2c_device					i2c_dev;
	/* GPIO */
	gpio_device					gpio_dev;
	int8_t						gpio_reset;
	int8_t						gpio_ldac;
	/* Device Settings */
	ad5676_state				daisy_chain_en;
	ad5676_supported_device_ids act_device; 
} ad5676_dev;


typedef struct {
	/* SPI */
	uint8_t				spi_chip_select;
	spi_mode			spi_mode;
	spi_type			spi_type;
	uint32_t			spi_device_id;
	/* I2C */
	uint16_t			i2c_dev_addr_selection;
	/* GPIO */
	gpio_type			gpio_type;
	uint32_t			gpio_device_id;
	int8_t				gpio_reset;
	int8_t				gpio_ldac;
	/* Device Settings */
	ad5676_state		daisy_chain_en;
	ad5676_amp_gain 	gain;
	ad5676_voltage_ref	vref;
	uint16_t		    pwr_dac_setting;
} ad5676_init_param;

static const struct ad5676_chip_info chip_info[] = {
	{ "AD5676",	 16, SPI },	
	{ "AD5676R", 16, SPI },
	{ "AD5672R", 12, SPI },
	{ "AD5675",	 16, I2C },
	{ "AD5675R", 16, I2C },
	{ "AD5671R", 12, I2C },
	
	{ "AD5686",	 16, SPI },	
	{ "AD5686R", 16, SPI },
	{ "AD5685R", 14, SPI },
	{ "AD5684",  12, SPI },
	{ "AD5684R", 12, SPI },
	
	{ "AD5696",	 16, I2C },	
	{ "AD5696R", 16, I2C },
	{ "AD5695R", 14, I2C },
	{ "AD5694",  12, I2C },
	{ "AD5694R", 12, I2C },
	
}; 

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* SPI command write to device. */
int32_t ad5676_cmd_write(ad5676_dev *dev,
						 uint8_t cmd,
						 uint16_t data);

/* SPI readback register from device. */
int32_t ad5676_spi_readback_reg(ad5676_dev *dev,
								ad5676_dac dac,
								uint32_t *data);

/* Internal reference and gain setup */
int32_t ad5676_set_gain_voltage_ref(ad5676_dev *dev,
							        ad5676_voltage_ref vref,
							        ad5676_amp_gain gain);

/* Initialize the device. */
int32_t ad5676_setup(ad5676_supported_device_ids id,
					 ad5676_dev **device,
					 ad5676_init_param init_param);

/* Do a software reset. */
int32_t ad5676_do_soft_reset(ad5676_dev *dev);

/* Set the DAC register for the selected channel. */
int32_t ad5676_set_dac_reg(ad5676_dev *dev,
						   ad5676_dac dac,
						   uint16_t data);

/* Power down the selected channels. */
int32_t ad5676_set_pwr_dac(ad5676_dev *dev,
						   uint16_t setting);


#endif // AD5676_H_