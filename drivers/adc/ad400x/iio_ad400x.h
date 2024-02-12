/***************************************************************************//**
* @file   iio_ad400x.h
* @brief  Header file for AD400X IIO interface
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifndef _AD400X_IIO_H_
#define _AD400X_IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

#include "iio.h"
#include "iio_types.h"
#include "ad400x.h"

/******************************************************************************/
/********************** Public/Extern Declarations ****************************/
/******************************************************************************/

struct ad400x_iio_dev {
	struct ad400x_dev *ad400x_dev;
	struct iio_device *iio_dev;
};

/* Init the IIO interface */
int ad400x_iio_init(struct ad400x_iio_dev **dev,
		    struct ad400x_init_param *init_param);
int ad400x_iio_remove(struct ad400x_iio_dev *dev);
#endif /* __AD400X_IIO_H__ */
