/***************************************************************************//**
 *   @file   imu_dual_core_example.h
 *   @brief  Dual-core IMU example header (ARM side).
 *   @author Victor Pascu (victor.pascu@analog.com)
 *******************************************************************************/

#ifndef _IMU_DUAL_CORE_EXAMPLE_H_
#define _IMU_DUAL_CORE_EXAMPLE_H_

/**
 * @brief ARM dual-core IMU example main function.
 * Boots the RISC-V core, which polls an ADIS16577-2 over SPI0 and publishes raw
 * samples, initializes an ADXL345 on the same SPI0 bus, integrates gyro angles
 * on the ARM, and logs both sensors.
 * @return 0 on success, negative error code on failure.
 */
int imu_dual_core_example_main(void);

#endif /* _IMU_DUAL_CORE_EXAMPLE_H_ */
