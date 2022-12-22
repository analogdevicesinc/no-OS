#ifndef __PLATFORM_INCLUDES_H__
#define __PLATFORM_INCLUDES_H__

#ifdef STM32_PLATFORM
#include "stm32/parameters.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim/parameters.h"
#endif

#ifdef PICO_PLATFORM
#include "pico/parameters.h"
#endif

#ifdef IIO_SUPPORT
#include "iio_app.h"
#endif

#endif /* __PLATFORM_INCLUDES_H__ */
