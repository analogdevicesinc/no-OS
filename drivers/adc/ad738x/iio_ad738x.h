/***************************************************************************//**
* @file   iio_ad738x.h
* @brief  Header file for AD738X IIO interface
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* Copyright (c) 2024 BayLibre, SAS.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifndef _AD738X_IIO_H_
#define _AD738X_IIO_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

#include "iio.h"
#include "iio_types.h"
#include "ad738x.h"

/******************************************************************************/
/********************** Public/Extern Declarations ****************************/
/******************************************************************************/

struct ad738x_iio_dev {
	struct ad738x_dev *ad738x_dev;
	struct iio_device *iio_dev;
};

/* Init the IIO interface */
int ad738x_iio_init(struct ad738x_iio_dev **dev,
		    struct ad738x_init_param *init_param);
int ad738x_iio_remove(struct ad738x_iio_dev *dev);
#endif /* __AD738X_IIO_H__ */
