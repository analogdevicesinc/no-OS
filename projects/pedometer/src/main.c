
#include <stdio.h>
#include <string.h>
#include "no_os_uart.h"
#include "maxim_uart.h"
#include "maxim_uart_stdio.h"
#include "no_os_i2c.h"
#include "maxim_i2c.h"
#include "ssd_1306.h"

enum max20303_chg_stat {
	CHARGER_OFF,		// 000 = Charger off
	CHARGER_TEMP_SUSPEND,	// 001 = Charging suspended due to temperature
	CHARGER_PRECHARGE,	// 010 = Pre-charge in progress
	CHARGER_FAST_CC,	// 011 = Fast-charge constant current mode in progress
	CHARGER_FAST_CV,	// 100 = Fast-charge constant voltage mode in progress
	CHARGER_MAINTAIN,	// 101 = Maintain charge in progress
	CHARGER_MAINTAIN_DONE,	// 110 = Maintain charger timer done
	CHARGER_FAULT		// 111 = Charger fault condition
};

#if 0
static const char *const max20303_chg_stat_msg[] = {
	"Charger off",
	"Charging suspended due to temperature",
	"Pre-charge in progress",
	"Fast-charge constant current mode in progress",
	"Fast-charge constant voltage mode in progress",
	"Maintain charge in progress",
	"Maintain charger timer done",
	"Charger fault condition"
};
#else
static const char *const max20303_chg_stat_msg[] = {
	"    not charging",
	"       suspended",
	"      pre-charge",
	"constant current",
	"constant voltage",
	" maintain charge",
	"   maintain done",
	" fault condition"
};
#endif

int max20303_read_reg(struct no_os_i2c_desc *desc,
		      uint8_t addr,
		      uint8_t *val)
{
	int ret;

	ret = no_os_i2c_write(desc,
			&addr,
			1,
			0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc,
			val,
			1,
			1);
	if (ret)
		return ret;

	return 0;
}

int max20303_read_reg16(struct no_os_i2c_desc *desc,
		      uint8_t addr,
		      uint16_t *val)
{
	int ret;
	uint8_t buf[2];

	ret = no_os_i2c_write(desc,
			&addr,
			1,
			0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(desc,
			buf,
			2,
			1);
	if (ret)
		return ret;

	*val = (buf[0] << 8) | buf[1];

	return 0;
}

int max20303_write_reg(struct no_os_i2c_desc *desc,
		      uint8_t addr,
		      uint8_t val)
{
	int ret;
	uint8_t buf[2];

	buf[0] = addr;
	buf[1] = val;

	ret = no_os_i2c_write(desc,
			buf,
			2,
			1);
	if (ret)
		return ret;

	return 0;
}

int max20303_write_reg16(struct no_os_i2c_desc *desc,
		      uint8_t addr,
		      uint16_t val)
{
	int ret;
	uint8_t buf[3];

	buf[0] = addr;
	buf[1] = (val >> 8) & 0xFF;
	buf[2] = val & 0xFF;

	ret = no_os_i2c_write(desc,
			buf,
			3,
			1);
	if (ret)
		return ret;

	return 0;
}

// 48x24 battery icon
static const uint8_t battery_empty_icon[] = {
    // First 8 rows (top third of battery)
    0x00,0x00,0x00,0x00,0xF8,0xF8,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0xF8,0xF8,0x00,0x00,0x00,0x00,
    // Middle 8 rows (middle third with battery tip)
    0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,
    // Last 8 rows (bottom third of battery)
    0x00,0x00,0x00,0x00,0x1F,0x1F,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x1F,0x1F,0x00,0x00,0x00,0x00
};

extern int32_t ssd_1306_print_icon(struct display_dev *device, const uint8_t *icon_buffer,
                uint8_t width, uint8_t height, uint8_t row, uint8_t col);

		// Function to display battery level (0-100)
void display_battery_level(struct display_dev *display, uint8_t percentage) {
    uint8_t battery_icon[144]; // 48x24 pixels = 144 bytes
    memcpy(battery_icon, battery_empty_icon, sizeof(battery_icon));
    
    // Calculate how many columns to fill
    int fill_columns = 0;
    if (percentage >= 100) {
        fill_columns = 35; // Maximum fillable columns (excluding borders)
    } else if (percentage > 0) {
        fill_columns = (percentage * 35) / 100;
    }
    
    // Fill the battery from left to right in all three sections
    for (int i = 1; i < fill_columns; i++) {
        // Fill in top section (0xFF for solid fill)
        battery_icon[6 + i] |= 0xF0;  // 1111 0000 - fills the height except border pixels
        // Fill in middle section (if not in the tip area)
        if (i < 35) {  // Don't fill where the tip is
            battery_icon[48 + 6 + i] |= 0xFF;  // 1111 1111 - fills entire height
        }
        // Fill in bottom section (0xFF for solid fill)
        battery_icon[96 + 6 + i] |= 0x0F;  // 0000 1111 - fills the height except border pixels
    }
    
    ssd_1306_print_icon(display, battery_icon, 48, 24, 0, 26);
}

int main(void)
{
	struct max_uart_init_param max_uart_extra_ip = {
		.flow = MAX_UART_FLOW_DIS
	};
	struct no_os_uart_init_param uart_ip = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &max_uart_extra_ip,
	};

	struct max_i2c_init_param max_i2c_extra_ip = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH
	};
	struct no_os_i2c_init_param max20303_i2c_init_param = {
		.device_id = 1,
		.max_speed_hz = 400000,
		.slave_address = 0x28,
		.platform_ops = &max_i2c_ops,
		.extra = &max_i2c_extra_ip,
	};
	struct no_os_i2c_init_param max20303_fuel_gauge_i2c_init_param = {
		.device_id = 1,
		.max_speed_hz = 400000,
		.slave_address = 0x36,
		.platform_ops = &max_i2c_ops,
		.extra = &max_i2c_extra_ip,
	};

	struct no_os_i2c_init_param oled_display_i2c_init_param = {
		.device_id = 2,
		.max_speed_hz = 400000,
		.slave_address = 0x3C,
		.platform_ops = &max_i2c_ops,
		.extra = &max_i2c_extra_ip,
	};

	ssd_1306_extra oled_display_extra = {
		.comm_type = SSD1306_I2C,
		.i2c_ip = &oled_display_i2c_init_param,
	};

	struct display_init_param oled_display_ini_param = {
		.cols_nb = 128,
		.rows_nb = 64,
		.controller_ops = &ssd1306_ops,
		.extra = &oled_display_extra,
	};

	struct no_os_uart_desc *uart_desc;
	struct no_os_i2c_desc *max20303_i2c_desc;
	struct no_os_i2c_desc *max20303_fuel_gauge_i2c_desc;
	uint8_t val;
	int ret;

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;

	ret = no_os_uart_write(uart_desc, "no_os_uart_write: Hello\n\r", 26);
	if (ret < 0)
		goto error;

	no_os_uart_stdio(uart_desc);

	printf("printf: Hello\n\r");

	ret = no_os_i2c_init(&max20303_i2c_desc, &max20303_i2c_init_param);
	if (ret) {
		printf("no_os_i2c_init: error\n\r");
		goto error;
	}

	ret = no_os_i2c_init(&max20303_fuel_gauge_i2c_desc, &max20303_fuel_gauge_i2c_init_param);
	if (ret) {
		printf("Failed to initialize fuel gauge I2C.\n\r");
		return ret;
	}

	struct display_dev *oled_display;
	struct no_os_i2c_desc *oled_display_i2c_desc;
	ret = no_os_i2c_init(&oled_display_i2c_desc, &oled_display_i2c_init_param);
	if (ret) {
		printf("Failed to initialize I2C.\n\r");
		return ret;
	}
	oled_display_extra.i2c_desc = oled_display_i2c_desc;
	ret = display_init(&oled_display, &oled_display_ini_param);
	if (ret) {
		printf("Failed to initialize display.\n\r");
		return ret;
	}
	display_clear(oled_display);
	display_on(oled_display);

	max20303_write_reg16(max20303_fuel_gauge_i2c_desc, 0x0a, 0x0000);


	char buf[100];
	uint16_t voltage;
	uint8_t level = 0;

	while(1) {
		max20303_read_reg(max20303_i2c_desc, 0x06, &val);
		sprintf(buf, "%s", max20303_chg_stat_msg[val & 0x07]);
		display_print_string(oled_display, buf, 7, 0);

		max20303_read_reg16(max20303_fuel_gauge_i2c_desc, 0x02, &voltage);
		sprintf(buf, "%.2f mV", (voltage * 78.125) / 1000);
		display_print_string(oled_display, buf, 6, 6);

		display_battery_level(oled_display, level);
		level++;
		if (level > 100)
			level = 0;
	}

	printf("printf: Bye\n\r");

	while(1);

error:
	no_os_uart_remove(uart_desc);

	return ret;
}

