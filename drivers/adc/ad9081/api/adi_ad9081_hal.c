/*!
 * @brief     APIs to call HAL functions
 *
 * @copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup __AD9081_HAL__
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_ad9081_hal.h"

/*============= C O D E ====================*/
int32_t adi_ad9081_hal_hw_open(adi_ad9081_device_t *device)
{
	AD9081_NULL_POINTER_RETURN(device);
	if (device->hal_info.hw_open != NULL) {
		if (API_CMS_ERROR_OK !=
		    device->hal_info.hw_open(device->hal_info.user_data))
			return API_CMS_ERROR_HW_OPEN;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_hw_close(adi_ad9081_device_t *device)
{
	AD9081_NULL_POINTER_RETURN(device);
	if (device->hal_info.hw_close != NULL) {
		if (API_CMS_ERROR_OK !=
		    device->hal_info.hw_close(device->hal_info.user_data))
			return API_CMS_ERROR_HW_CLOSE;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_delay_us(adi_ad9081_device_t *device, uint32_t us)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(device->hal_info.delay_us);
	if (API_CMS_ERROR_OK !=
	    device->hal_info.delay_us(device->hal_info.user_data, us)) {
		return API_CMS_ERROR_DELAY_US;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_reset_pin_ctrl(adi_ad9081_device_t *device,
				      uint8_t enable)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(device->hal_info.reset_pin_ctrl);
	if (API_CMS_ERROR_OK != device->hal_info.reset_pin_ctrl(
					device->hal_info.user_data, enable)) {
		return API_CMS_ERROR_RESET_PIN_CTRL;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_log_write(adi_ad9081_device_t *device,
				 adi_cms_log_type_e log_type,
				 const char *comment, ...)
{
	va_list argp;
	AD9081_NULL_POINTER_RETURN(device);

	if (((log_type & ADI_REPORT_VERBOSE) > 0) &&
	    (device->hal_info.log_write != NULL)) {
		va_start(argp, comment);
		if (API_CMS_ERROR_OK !=
		    device->hal_info.log_write(device->hal_info.user_data,
					       log_type, comment, argp))
			return API_CMS_ERROR_LOG_WRITE;
		va_end(argp);
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_bf_get(adi_ad9081_device_t *device, uint32_t reg,
			      uint32_t info, uint8_t *value,
			      uint8_t value_size_bytes)
{
	int32_t err;
	uint8_t reg_offset = 0, data8 = 0;
	uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
	uint32_t data32 = 0, mask = 0, endian_test_val = 0x11223344;
	uint64_t bf_val = 0;
	uint8_t reg_bytes =
		((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
	uint8_t i = 0, j = 0, filled_bits = 0;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(value);
	AD9081_INVALID_PARAM_RETURN(width > 64);
	AD9081_INVALID_PARAM_RETURN(width < 1);
	AD9081_INVALID_PARAM_RETURN(value_size_bytes > 8);

	if (reg < 0x4000) {
		for (reg_offset = 0; reg_offset < reg_bytes; reg_offset++) {
			err = adi_ad9081_hal_reg_get(device, reg + reg_offset,
						     &data8);
			AD9081_ERROR_RETURN(err);
			if ((offset + width) <= 8) { /* last 8bits */
				mask = (1 << width) - 1;
				data8 = (data8 >> offset) & mask;
				bf_val = bf_val +
					 ((uint64_t)data8 << filled_bits);
				filled_bits = filled_bits + width;
			} else {
				mask = (1 << (8 - offset)) - 1;
				data8 = (data8 >> offset) & mask;
				bf_val = bf_val +
					 ((uint64_t)data8 << filled_bits);
				width = offset + width - 8;
				filled_bits = filled_bits + (8 - offset);
				offset = 0;
			}
		}
	} else { /* access extended space */
		for (reg_offset = 0; reg_offset < reg_bytes; reg_offset += 4) {
			err = adi_ad9081_hal_reg_get(device, reg + reg_offset,
						     (uint8_t *)&data32);
			AD9081_ERROR_RETURN(err);
			if ((offset + width) <= 32) { /* last 32bits */
				mask = ((uint64_t)1 << width) - 1;
				data32 = (data32 >> offset) & mask;
				bf_val = bf_val +
					 ((uint64_t)data32 << filled_bits);
				filled_bits = filled_bits + width;
			} else {
				mask = ((uint64_t)1 << (32 - offset)) - 1;
				data32 = (data32 >> offset) & mask;
				bf_val = bf_val +
					 ((uint64_t)data32 << filled_bits);
				width = offset + width - 32;
				filled_bits = filled_bits + (32 - offset);
				offset = 0;
			}
		}
	}

	/* save bitfield value to buffer */
	for (i = 0; i < value_size_bytes; i++) {
		j = (*(uint8_t *)&endian_test_val == 0x44) ?
			    (i) :
			    (value_size_bytes - 1 - i);
		value[j] = (uint8_t)(bf_val >> (i << 3));
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_bf_set(adi_ad9081_device_t *device, uint32_t reg,
			      uint32_t info, uint64_t value)
{
	int32_t err;
	uint8_t reg_offset = 0, data8 = 0;
	uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
	uint32_t data32 = 0, mask = 0;
	uint8_t reg_bytes =
		((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_INVALID_PARAM_RETURN(width > 64);
	AD9081_INVALID_PARAM_RETURN(width < 1);

	if (reg < 0x4000) {
		for (reg_offset = 0; reg_offset < reg_bytes; reg_offset++) {
			if ((offset + width) <= 8) { /* last 8bits */
				if ((offset > 0) || ((offset + width) < 8)) {
					err = adi_ad9081_hal_reg_get(
						device, reg + reg_offset,
						&data8);
					AD9081_ERROR_RETURN(err);
				}
				mask = (1 << width) - 1;
				data8 = data8 & (~(mask << offset));
				data8 = data8 | ((value & mask) << offset);
			} else {
				if (offset > 0) {
					err = adi_ad9081_hal_reg_get(
						device, reg + reg_offset,
						&data8);
					AD9081_ERROR_RETURN(err);
				}
				mask = (1 << (8 - offset)) - 1;
				data8 = data8 & (~(mask << offset));
				data8 = data8 | ((value & mask) << offset);
				value = value >> (8 - offset);
				width = offset + width - 8;
				offset = 0;
			}
			err = adi_ad9081_hal_reg_set(device, reg + reg_offset,
						     data8);
			AD9081_ERROR_RETURN(err);
		}
	} else { /* access extended space */
		for (reg_offset = 0; reg_offset < reg_bytes; reg_offset += 4) {
			if ((offset + width) <= 32) { /* last 32bits */
				if ((offset > 0) || ((offset + width) < 32)) {
					err = adi_ad9081_hal_reg_get(
						device, reg + reg_offset,
						(uint8_t *)&data32);
					AD9081_ERROR_RETURN(err);
				}
				mask = ((uint64_t)1 << width) - 1;
				data32 = data32 & (~(mask << offset));
				data32 = data32 | ((value & mask) << offset);
			} else {
				if (offset > 0) {
					err = adi_ad9081_hal_reg_get(
						device, reg + reg_offset,
						&data8);
					AD9081_ERROR_RETURN(err);
				}
				mask = ((uint64_t)1 << (32 - offset)) - 1;
				data32 = data32 & (~(mask << offset));
				data32 = data32 | ((value & mask) << offset);
				value = value >> (32 - offset);
				width = offset + width - 32;
				offset = 0;
			}
			err = adi_ad9081_hal_reg_set(device, reg + reg_offset,
						     data32);
			AD9081_ERROR_RETURN(err);
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_reg_get(adi_ad9081_device_t *device, uint32_t reg,
			       uint8_t *data)
{
	uint8_t in_data[6] = { 0 }, out_data[6] = { 0 };
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(device->hal_info.spi_xfer);
	AD9081_NULL_POINTER_RETURN(data);

	if (reg < 0x4000) {
		in_data[0] = ((reg >> 8) & 0x3F) | 0x80;
		in_data[1] = ((reg >> 0) & 0xFF);
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		*data = out_data[2];
		if (API_CMS_ERROR_OK !=
		    AD9081_LOG_SPIR((in_data[0] << 8) + in_data[1],
				    out_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
	} else { /* access extended 32-bit data space */
		in_data[0] = 0x3D;
		in_data[1] = 0x21;
		in_data[2] = (reg >> 8) & 0xC0;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d21, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		in_data[0] = 0x3D;
		in_data[1] = 0x22;
		in_data[2] = (reg >> 16) & 0xFF;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d22, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		in_data[0] = 0x3D;
		in_data[1] = 0x23;
		in_data[2] = (reg >> 24) & 0xFF;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d23, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		if (((reg >= 0x4F00000) && (reg <= 0x4FFFFFF)) ||
		    ((reg >= 0x6001000) && (reg <= 0x60010FF))) {
			/* 32-bit address, 8-bit data */
			in_data[0] = ((reg >> 8) & 0x3F) | 0xC0;
			in_data[1] = ((reg >> 0) & 0xFF);
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(device->hal_info.user_data,
						      in_data, out_data, 0x3))
				return API_CMS_ERROR_SPI_XFER;
			*data = out_data[2];
			if (API_CMS_ERROR_OK !=
			    AD9081_LOG_SPIR((in_data[0] << 8) + in_data[1],
					    out_data[2]))
				return API_CMS_ERROR_LOG_WRITE;
		} else {
			/* 32-bit address, 32-bit data */
			reg += (device->hal_info.addr_inc ==
				SPI_ADDR_INC_AUTO) ?
				       0 :
				       3;
			in_data[0] = ((reg >> 8) & 0x3F) | 0xC0;
			in_data[1] = ((reg >> 0) & 0xFF);
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(
				    device->hal_info.user_data, in_data,
				    out_data, 0x20000006))
				return API_CMS_ERROR_SPI_XFER;
			if (device->hal_info.addr_inc == SPI_ADDR_INC_AUTO) {
				*(uint32_t *)data = (out_data[2]) +
						    (out_data[3] << 8) +
						    (out_data[4] << 16) +
						    (out_data[5] << 24);
			} else { /* streaming addresses are decremented */
				*(uint32_t *)data = (out_data[5]) +
						    (out_data[4] << 8) +
						    (out_data[3] << 16) +
						    (out_data[2] << 24);
			}
			if (API_CMS_ERROR_OK !=
			    AD9081_LOG_SPIR32((in_data[0] << 8) + in_data[1],
					      *(uint32_t *)data))
				return API_CMS_ERROR_LOG_WRITE;
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_reg_set(adi_ad9081_device_t *device, uint32_t reg,
			       uint32_t data)
{
	uint8_t in_data[6] = { 0 }, out_data[6] = { 0 };
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(device->hal_info.spi_xfer);

	if (reg < 0x4000) {
		in_data[0] = (reg >> 8) & 0x3F;
		in_data[1] = (reg >> 0) & 0xFF;
		in_data[2] = (uint8_t)(data & 0xFF);
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK !=
		    AD9081_LOG_SPIW(reg & 0x3fff, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
	} else { /* access extended 32-bit data space */
		in_data[0] = 0x3D;
		in_data[1] = 0x21;
		in_data[2] = (reg >> 8) & 0xC0;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d21, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		in_data[0] = 0x3D;
		in_data[1] = 0x22;
		in_data[2] = (reg >> 16) & 0xFF;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d22, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		in_data[0] = 0x3D;
		in_data[1] = 0x23;
		in_data[2] = (reg >> 24) & 0xFF;
		if (API_CMS_ERROR_OK !=
		    device->hal_info.spi_xfer(device->hal_info.user_data,
					      in_data, out_data, 0x3))
			return API_CMS_ERROR_SPI_XFER;
		if (API_CMS_ERROR_OK != AD9081_LOG_SPIW(0x3d23, in_data[2]))
			return API_CMS_ERROR_LOG_WRITE;
		if (((reg >= 0x4F00000) && (reg <= 0x4FFFFFF)) ||
		    ((reg >= 0x6001000) && (reg <= 0x60010FF))) {
			/* 32-bit address, 8-bit data */
			in_data[0] = ((reg >> 8) & 0x3F) | 0x40;
			in_data[1] = ((reg >> 0) & 0xFF);
			in_data[2] = (uint8_t)(data & 0xFF);
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(device->hal_info.user_data,
						      in_data, out_data, 0x3))
				return API_CMS_ERROR_SPI_XFER;
			if (API_CMS_ERROR_OK !=
			    AD9081_LOG_SPIW((in_data[0] << 8) + in_data[1],
					    in_data[2]))
				return API_CMS_ERROR_LOG_WRITE;
		} else {
			/* 32-bit address, 32-bit data */
			if (device->hal_info.addr_inc == SPI_ADDR_INC_AUTO) {
				in_data[0] = ((reg >> 8) & 0x3F) | 0x40;
				in_data[1] = ((reg >> 0) & 0xFF);
				in_data[2] = (uint8_t)((data >> 0) & 0xFF);
				in_data[3] = (uint8_t)((data >> 8) & 0xFF);
				in_data[4] = (uint8_t)((data >> 16) & 0xFF);
				in_data[5] = (uint8_t)((data >> 24) & 0xFF);
			} else { /* streaming addresses are decremented */
				in_data[0] = (((reg + 3) >> 8) & 0x3F) | 0x40;
				in_data[1] = (((reg + 3) >> 0) & 0xFF);
				in_data[2] = (uint8_t)((data >> 24) & 0xFF);
				in_data[3] = (uint8_t)((data >> 16) & 0xFF);
				in_data[4] = (uint8_t)((data >> 8) & 0xFF);
				in_data[5] = (uint8_t)((data >> 0) & 0xFF);
			}
			if (API_CMS_ERROR_OK !=
			    device->hal_info.spi_xfer(
				    device->hal_info.user_data, in_data,
				    out_data, 0x20000006))
				return API_CMS_ERROR_SPI_XFER;
			if (API_CMS_ERROR_OK !=
			    AD9081_LOG_SPIW32((in_data[0] << 8) + in_data[1],
					      data))
				return API_CMS_ERROR_LOG_WRITE;
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbusjrx_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data,
				       uint8_t lane)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(data);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x406, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x409, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK !=
	    adi_ad9081_hal_reg_set(device, 0x409, 1 << lane))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_get(device, 0x40a, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x409, 0x00))
		return API_CMS_ERROR_SPI_XFER;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbusjrx_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data, uint8_t lane)
{
	AD9081_NULL_POINTER_RETURN(device);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x406, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x408, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x407, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x407, lane))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x407, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbusjtx_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data,
				       uint8_t lane)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(data);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x790, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x794, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK !=
	    adi_ad9081_hal_reg_set(device, 0x794, 1 << lane))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_get(device, 0x796, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x794, 0x00))
		return API_CMS_ERROR_SPI_XFER;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbusjtx_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data, uint8_t lane)
{
	AD9081_NULL_POINTER_RETURN(device);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x790, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x793, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x791, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x791, lane))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x791, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbuspll_reg_get(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t *data)
{
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_NULL_POINTER_RETURN(data);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x740, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x72E, 0x01))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_get(device, 0x742, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x72E, 0x00))
		return API_CMS_ERROR_SPI_XFER;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_cbuspll_reg_set(adi_ad9081_device_t *device,
				       uint32_t reg, uint8_t data)
{
	AD9081_NULL_POINTER_RETURN(device);
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x740, reg))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x741, data))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x72F, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x72F, 0x01))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_reg_set(device, 0x72F, 0x00))
		return API_CMS_ERROR_SPI_XFER;
	if (API_CMS_ERROR_OK != adi_ad9081_hal_delay_us(device, 100000))
		return API_CMS_ERROR_DELAY_US;

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_bf_wait_to_clear(adi_ad9081_device_t *device,
					uint32_t reg, uint32_t info)
{
	int32_t err;
	uint8_t i = 0, bf_value = 0;
	AD9081_NULL_POINTER_RETURN(device);
	for (i = 0; i < 200; i++) {
		err = adi_ad9081_hal_delay_us(device, 20);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, reg, info, &bf_value, 1);
		AD9081_ERROR_RETURN(err);
		if (bf_value == 0) {
			break;
		}
		if (i == 199) {
			return API_CMS_ERROR_ERROR;
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_bf_wait_to_set(adi_ad9081_device_t *device, uint32_t reg,
				      uint32_t info)
{
	int32_t err;
	uint8_t i = 0, bf_value = 0;
	AD9081_NULL_POINTER_RETURN(device);
	for (i = 0; i < 200; i++) {
		err = adi_ad9081_hal_delay_us(device, 20);
		AD9081_ERROR_RETURN(err);
		err = adi_ad9081_hal_bf_get(device, reg, info, &bf_value, 1);
		AD9081_ERROR_RETURN(err);
		if (bf_value == 1) {
			break;
		}
		if (i == 199) {
			return API_CMS_ERROR_ERROR;
		}
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_error_report(adi_ad9081_device_t *device,
				    adi_cms_log_type_e log_type, int32_t error,
				    const char *file_name,
				    const char *func_name, uint32_t line_num,
				    const char *var_name, const char *comment)
{
	if (device == NULL)
		return API_CMS_ERROR_NULL_PARAM;

	if (API_CMS_ERROR_OK !=
	    adi_ad9081_hal_log_write(
		    device, log_type, "%s, \"%s\" in %s(...), line%d in %s",
		    comment, var_name, func_name, line_num, file_name))
		return API_CMS_ERROR_LOG_WRITE;

	return API_CMS_ERROR_OK;
}

void adi_ad9081_hal_add_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
			    uint64_t *hi, uint64_t *lo)
{
	uint64_t rl = al + bl, rh = ah + bh;
	if (rl < al)
		rh++;
	*lo = rl;
	*hi = rh;
}

void adi_ad9081_hal_subt_128(uint64_t ah, uint64_t al, uint64_t bh, uint64_t bl,
			     uint64_t *hi, uint64_t *lo)
{
	uint64_t rl, rh;
	if (bl <= al) {
		rl = al - bl;
		rh = ah - bh;
	} else {
		rl = bl - al - 1;
		rl = 0xffffffffffffffffull - rl;
		ah--;
		rh = ah - bh;
	}
	*lo = rl;
	*hi = rh;
}

void adi_ad9081_hal_mult_128(uint64_t a, uint64_t b, uint64_t *hi, uint64_t *lo)
{
	uint64_t ah = a >> 32, al = a & 0xffffffff, bh = b >> 32,
		 bl = b & 0xffffffff, rh = ah * bh, rl = al * bl, rm1 = ah * bl,
		 rm2 = al * bh, rm1h = rm1 >> 32, rm2h = rm2 >> 32,
		 rm1l = rm1 & 0xffffffff, rm2l = rm2 & 0xffffffff,
		 rmh = rm1h + rm2h, rml = rm1l + rm2l,
		 c = ((rl >> 32) + rml) >> 32;
	rl = rl + (rml << 32);
	rh = rh + rmh + c;
	*lo = rl;
	*hi = rh;
}

void adi_ad9081_hal_lshift_128(uint64_t *hi, uint64_t *lo)
{
	*hi <<= 1;
	if (*lo & 0x8000000000000000ull)
		*hi |= 1ull;
	*lo <<= 1;
}

void adi_ad9081_hal_rshift_128(uint64_t *hi, uint64_t *lo)
{
	*lo >>= 1;
	if (*hi & 1ull)
		*lo |= 0x8000000000000000ull;
	*hi >>= 1;
}

void adi_ad9081_hal_div_128(uint64_t a_hi, uint64_t a_lo, uint64_t b_hi,
			    uint64_t b_lo, uint64_t *hi, uint64_t *lo)
{
	uint64_t remain_lo = a_lo, remain_hi = a_hi, part1_lo = b_lo,
		 part1_hi = b_hi;
	uint64_t result_lo = 0, result_hi = 0, mask_lo = 1, mask_hi = 0;

	while (!(part1_hi & 0x8000000000000000ull)) {
		adi_ad9081_hal_lshift_128(&part1_hi, &part1_lo);
		adi_ad9081_hal_lshift_128(&mask_hi, &mask_lo);
	}

	do {
		if ((remain_hi > part1_hi) ||
		    ((remain_hi == part1_hi) && (remain_lo >= part1_lo))) {
			adi_ad9081_hal_subt_128(remain_hi, remain_lo, part1_hi,
						part1_lo, &remain_hi,
						&remain_lo);
			adi_ad9081_hal_add_128(result_hi, result_lo, mask_hi,
					       mask_lo, &result_hi, &result_lo);
		}
		adi_ad9081_hal_rshift_128(&part1_hi, &part1_lo);
		adi_ad9081_hal_rshift_128(&mask_hi, &mask_lo);
	} while ((mask_hi != 0) || (mask_lo != 0));
	*lo = result_lo;
	*hi = result_hi;
}

int32_t adi_ad9081_hal_calc_rx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t adc_freq, int64_t nco_shift,
				       uint64_t *ftw)
{
	uint64_t a, b, hi, lo;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(adc_freq == 0);

	if (nco_shift >= 0) {
#ifdef __KERNEL__
		a = div64_u64((uint64_t)nco_shift, adc_freq);
#else
		a = nco_shift / adc_freq;
#endif
		b = nco_shift -
		    (a * adc_freq); /* b = fmod(nco_shift, adc_freq) */
		adi_ad9081_hal_mult_128(281474976710656ull, b, &hi, &lo);
		adi_ad9081_hal_add_128(hi, lo, 0, adc_freq - 1, &hi, &lo);
		adi_ad9081_hal_div_128(hi, lo, 0, adc_freq, &hi, ftw);
	} else {
#ifdef __KERNEL__
		a = div64_u64((uint64_t)-nco_shift, adc_freq);
#else
		a = -nco_shift / adc_freq;
#endif
		b = -nco_shift - (a * adc_freq);
		adi_ad9081_hal_mult_128(281474976710656ull, b, &hi, &lo);
		adi_ad9081_hal_add_128(hi, lo, 0, adc_freq - 1, &hi, &lo);
		adi_ad9081_hal_div_128(hi, lo, 0, adc_freq, &hi, ftw);
		*ftw = 281474976710656ull - *ftw;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_calc_tx_nco_ftw(adi_ad9081_device_t *device,
				       uint64_t dac_freq, int64_t nco_shift,
				       uint64_t *ftw)
{
	uint64_t hi, lo;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dac_freq == 0);

	if (nco_shift >= 0) {
		adi_ad9081_hal_mult_128(281474976710656ull, nco_shift, &hi,
					&lo);
		adi_ad9081_hal_div_128(hi, lo, 0, dac_freq, &hi, ftw);
	} else {
		adi_ad9081_hal_mult_128(281474976710656ull, -nco_shift, &hi,
					&lo);
		adi_ad9081_hal_div_128(hi, lo, 0, dac_freq, &hi, ftw);
		*ftw = 281474976710656ull - *ftw;
	}

	return API_CMS_ERROR_OK;
}

int32_t adi_ad9081_hal_calc_tx_nco_ftw32(adi_ad9081_device_t *device,
					 uint64_t dac_freq, int64_t nco_shift,
					 uint64_t *ftw)
{
	uint64_t hi, lo;
	AD9081_NULL_POINTER_RETURN(device);
	AD9081_LOG_FUNC();
	AD9081_INVALID_PARAM_RETURN(dac_freq == 0);

	if (nco_shift >= 0) {
		adi_ad9081_hal_mult_128(4294967296ull, nco_shift, &hi, &lo);
		adi_ad9081_hal_div_128(hi, lo, 0, dac_freq, &hi, ftw);
	} else {
		adi_ad9081_hal_mult_128(4294967296ull, -nco_shift, &hi, &lo);
		adi_ad9081_hal_div_128(hi, lo, 0, dac_freq, &hi, ftw);
		*ftw = 4294967296ull - *ftw;
	}

	return API_CMS_ERROR_OK;
}

/*! @} */