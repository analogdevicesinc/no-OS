/***************************************************************************//**
 *   @file   imu_single_core_example.h
 *   @brief  Single-core dual-IMU example header.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *******************************************************************************/

#ifndef _IMU_SINGLE_CORE_EXAMPLE_H_
#define _IMU_SINGLE_CORE_EXAMPLE_H_

/**
 * @brief Single-core IMU example main function.
 * Initializes both the ADXL345 (accel, P0.19 CS) and ADIS16577-2 (6-axis IMU,
 * P0.11 CS) on SPI0, integrates gyro rate into roll/pitch/yaw angles on the
 * ARM, and logs both sensors at ~10 Hz over UART.
 * @return 0 on success, negative error code on failure.
 */
int imu_single_core_example_main(void);

#endif /* _IMU_SINGLE_CORE_EXAMPLE_H_ */
