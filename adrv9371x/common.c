// ***************************************************************************
// ***************************************************************************
// Copyright 2011(c) Analog Devices, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//     - Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     - Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the
//       distribution.
//     - Neither the name of Analog Devices, Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//     - The use of this software may or may not infringe the patent rights
//       of one or more patent holders.  This license does not release you
//       from the requirement that you obtain separate licenses from these
//       patent holders to use this software.
//     - Use of the software either in source or binary form, must be run
//       on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.
//
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
// RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#include "common.h"
#include "platform_drivers.h"

/* we need to fill in the functions of common */

ADI_LOGLEVEL CMB_LOGLEVEL = ADIHAL_LOG_ERROR | ADIHAL_LOG_WARNING;
static uint32_t CMB_TIMEOUT_COUNT = 0;

/* close hardware pointers */

commonErr_t CMB_closeHardware(void)
{
  return(COMMONERR_OK);
}

/* GPIO function */

commonErr_t CMB_setGPIO(uint32_t GPIO)
{
  return(COMMONERR_OK);
}

/* hardware reset function */

commonErr_t CMB_hardReset(uint8_t spiChipSelectIndex)
{
  ad_gpio_set(AD9371_RESET_B, 0x1);
  ad_gpio_set(AD9371_RESET_B, 0x0);
  mdelay(1);
  ad_gpio_set(AD9371_RESET_B, 0x1);
  mdelay(1);
  return(COMMONERR_OK);
}

/* SPI read/write functions */

commonErr_t CMB_setSPIOptions(spiSettings_t *spiSettings)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_setSPIChannel(uint16_t chipSelectIndex)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteByte(spiSettings_t *spiSettings, uint16_t addr, uint8_t data)
{
	uint8_t buf[3];

  spi_device m_device;
  ad_spi_init(&m_device);
  m_device.chip_select = spiSettings->chipSelectIndex;

  buf[0] = (uint8_t) ((addr >> 8) & 0x7f);
  buf[1] = (uint8_t) (addr & 0xff);
  buf[2] = (uint8_t) data;

  ad_spi_xfer(&m_device, buf, 3);
  return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteBytes(spiSettings_t *spiSettings, uint16_t *addr, uint8_t *data, uint32_t count)
{
  uint32_t index;

  for (index = 0; index < count; index++)
  {
    if (CMB_SPIWriteByte(spiSettings, *(addr + index), *(data + index)) != COMMONERR_OK)
      return(COMMONERR_FAILED);
  }

  return(COMMONERR_OK);
}

commonErr_t CMB_SPIReadByte(spiSettings_t *spiSettings, uint16_t addr, uint8_t *readdata)
{
  uint8_t buf[3];

  spi_device m_device;
  ad_spi_init(&m_device);
  m_device.chip_select = spiSettings->chipSelectIndex;

  buf[0] = (uint8_t) ((addr >> 8) | 0x80);
  buf[1] = (uint8_t) (addr & 0xff);
  buf[2] = (uint8_t) 0x00;

  ad_spi_xfer(&m_device, buf, 3);
  *readdata = buf[2];
  return(COMMONERR_OK);
}

commonErr_t CMB_SPIWriteField(spiSettings_t *spiSettings, uint16_t addr, uint8_t  field_val, uint8_t mask, uint8_t start_bit)
{
  uint8_t m_data;

  if (CMB_SPIReadByte(spiSettings, addr, &m_data) != COMMONERR_OK)
    return(COMMONERR_FAILED);

  m_data = (m_data & ~mask) | ((field_val << start_bit) & mask);

  if (CMB_SPIWriteByte(spiSettings, addr, m_data) != COMMONERR_OK)
    return(COMMONERR_FAILED);

  return(COMMONERR_OK);
}

commonErr_t CMB_SPIReadField(spiSettings_t *spiSettings, uint16_t addr, uint8_t *field_val, uint8_t mask, uint8_t start_bit)
{
  uint8_t m_data;

  if (CMB_SPIReadByte(spiSettings, addr, &m_data) != COMMONERR_OK)
    return(COMMONERR_FAILED);

  m_data = (m_data & mask) >> start_bit;
  *field_val = m_data;
  return(COMMONERR_OK);
}

/* platform timer functions */

commonErr_t CMB_wait_ms(uint32_t time_ms)
{
  mdelay(time_ms);
  return(COMMONERR_OK);
}

commonErr_t CMB_wait_us(uint32_t time_us)
{
  udelay(time_us);
  return(COMMONERR_OK);
}

commonErr_t CMB_setTimeout_ms(uint32_t timeOut_ms)
{
  CMB_TIMEOUT_COUNT = timeOut_ms * 1000;
  return(COMMONERR_OK);
}

commonErr_t CMB_setTimeout_us(uint32_t timeOut_us)
{
  CMB_TIMEOUT_COUNT = timeOut_us;
  return(COMMONERR_OK);
}

commonErr_t CMB_hasTimeoutExpired()
{
  udelay(1);
  CMB_TIMEOUT_COUNT = CMB_TIMEOUT_COUNT - 1;
  if (CMB_TIMEOUT_COUNT > 0)
    return(COMMONERR_OK);

  return(COMMONERR_FAILED);
}

/* platform logging functions */

commonErr_t CMB_openLog(const char *filename)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_closeLog(void)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_writeToLog(ADI_LOGLEVEL level, uint8_t deviceIndex, uint32_t errorCode, const char *comment)
{
  if (level == ADIHAL_LOG_ERROR)
    ad_printf("AD9371-ERROR[%d]: %s\n", errorCode, comment);

  if (level == ADIHAL_LOG_WARNING)
    ad_printf("AD9371-WARNING[%d]: %s\n", errorCode, comment);

  return(COMMONERR_OK);
}

commonErr_t CMB_flushLog(void)
{
  return(COMMONERR_OK);
}

/* platform FPGA AXI register read/write functions */

commonErr_t CMB_regRead(uint32_t offset, uint32_t *data)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_regWrite(uint32_t offset, uint32_t data)
{
  return(COMMONERR_OK);
}

/* platform DDR3 memory read/write functions */

commonErr_t CMB_memRead(uint32_t offset, uint32_t *data, uint32_t len)
{
  return(COMMONERR_OK);
}

commonErr_t CMB_memWrite(uint32_t offset, uint32_t *data, uint32_t len)
{
  return(COMMONERR_OK);
}

// ***************************************************************************
// ***************************************************************************
