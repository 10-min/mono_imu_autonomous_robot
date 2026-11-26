/*
 * imu.h
 *
 *  Created on: Nov 29, 2025
 *      Author: tm
 */

#ifndef IMU_H_
#define IMU_H_

#include "stm32f1xx_hal.h"

#define WHO_AM_I 0x00
#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define GYRO_CONFIG_1 0x01
#define ACCEL_CONFIG 0x14
#define ACCEL_XOUT_H 0x2D
#define ACCEL_YOUT_H 0x2F
#define ACCEL_ZOUT_H 0x31
#define GYRO_XOUT_H 0x33
#define GYRO_YOUT_H 0x35
#define GYRO_ZOUT_H 0x37

typedef enum {
    DPS_250 = 0,
    DPS_500,
    DPS_1000,
    DPS_2000,
} DPS;

typedef enum {
    FSR_2g = 0b00,
    FSR_4g = 0b01,
    FSR_8g = 0b10,
    FSR_16g = 0b11
} FSR;

typedef struct {
	SPI_HandleTypeDef* spi_handle;
	GPIO_TypeDef* spi_port;
	uint16_t sclk_pin;
	uint16_t miso_pin;
	uint16_t mosi_pin;
	uint16_t cs_pin;
	float gyro_sensitivity;
	float accel_sensitivity;
} ICM20948;

void Write_Data(ICM20948* imu, uint8_t addr, uint8_t data);

void Read_Data(ICM20948* imu, uint8_t addr, uint8_t* buffer);

void Read_16bit(ICM20948* imu, uint8_t addr, uint16_t* buffer);

int16_t uint16_To_int16(uint16_t value);

int Who_Am_I(ICM20948* imu);

void Set_Ub(ICM20948* imu, int ub);

int Set_Gyro_Config1(ICM20948* imu, uint8_t GYRO_DLPFCFG, uint8_t GYRO_FS_SEL, uint8_t GYRO_FCHOICE);

int Set_Accel_Config(ICM20948* imu, uint8_t ACCEL_DLPFCFG, uint8_t ACCEL_FS_SEL, uint8_t ACCEL_FCHOICE);

void Get_Gyro(ICM20948* imu, float* gyro);

void Get_Accel(ICM20948* imu, float* accel);

#endif /* INC_IMU_H_ */
