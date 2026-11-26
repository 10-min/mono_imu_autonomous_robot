/*
 * imu.c
 *
 *  Created on: Nov 29, 2025
 *      Author: tm
 */

#include "imu.h"

void Write_Data(ICM20948* imu, uint8_t addr, uint8_t data) {
	uint8_t addr_to_send[2] = { 0x7F & addr, data };
	HAL_GPIO_WritePin(imu->spi_port, imu->cs_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(imu->spi_handle, addr_to_send, 2, 1000);
	HAL_GPIO_WritePin(imu->spi_port, imu->cs_pin, GPIO_PIN_SET);
}

void Read_Data(ICM20948* imu, uint8_t addr, uint8_t* buffer) {
	uint8_t data_to_send[2] = {0x80 | addr, 0x00};
	HAL_GPIO_WritePin(imu->spi_port, imu->cs_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(imu->spi_handle, &data_to_send[0], 1, 1000);
	HAL_SPI_TransmitReceive(imu->spi_handle, &data_to_send[1], buffer, 1, 1000);
	HAL_GPIO_WritePin(imu->spi_port, imu->cs_pin, GPIO_PIN_SET);
}

void Read_16bit(ICM20948* imu, uint8_t addr, uint16_t* buffer) {
	uint8_t high, low;
	Read_Data(imu, addr, &high);
	Read_Data(imu, addr + 1, &low);
	*buffer = (high << 8) | low;
}

int16_t uint16_To_int16(uint16_t value) {
	int16_t result = value;
	if (value & 0x8000) {
		result -= 65536;
	}
	return result;
}

int Who_Am_I(ICM20948* imu) {
	uint8_t buffer;
	Set_Ub(imu, 0);
	Read_Data(imu, WHO_AM_I, &buffer);
	return buffer == 0xEA;
}

void Set_Ub(ICM20948* imu, int ub) {
	if (ub < 0 || ub > 3) return;

	Write_Data(imu, 0x7F, ub << 4);
}

int Set_Gyro_Config1(ICM20948* imu, uint8_t GYRO_DLPFCFG, uint8_t GYRO_FS_SEL, uint8_t GYRO_FCHOICE) {
	Set_Ub(imu, 2);
	uint8_t config = (GYRO_DLPFCFG << 3) | (GYRO_FS_SEL << 1) | GYRO_FCHOICE;
	Write_Data(imu, GYRO_CONFIG_1, config);
	uint8_t buffer;
	Read_Data(imu, GYRO_CONFIG_1, &buffer);
	if (((buffer >> 3) & 0b111) != GYRO_DLPFCFG ||
		((buffer >> 1) & 0b11) != GYRO_FS_SEL ||
		(buffer & 0b1) != GYRO_FCHOICE)
	{
		return 1;
	}

	float sensitivity = 0;
	switch(GYRO_FS_SEL) {
	case DPS_250:
		sensitivity = 131.0;
		break;
	case DPS_500:
		sensitivity = 65.5;
		break;
	case DPS_1000:
		sensitivity = 32.8;
		break;
	case DPS_2000:
		sensitivity = 16.4;
		break;
	default:
		break;
	}

	imu->gyro_sensitivity = sensitivity;
	return 0;
}

int Set_Accel_Config(ICM20948* imu, uint8_t ACCEL_DLPFCFG, uint8_t ACCEL_FS_SEL, uint8_t ACCEL_FCHOICE) {
	Set_Ub(imu, 2);
	uint8_t config = (ACCEL_DLPFCFG << 3) | (ACCEL_FS_SEL << 1) | ACCEL_FCHOICE;
	Write_Data(imu, ACCEL_CONFIG, config);
	uint8_t buffer;
	Read_Data(imu, ACCEL_CONFIG, &buffer);
	if (((buffer >> 3) & 0b111) != ACCEL_DLPFCFG ||
		((buffer >> 1) & 0b11) != ACCEL_FS_SEL ||
		(buffer & 0b1) != ACCEL_FCHOICE)
	{
		return 1;
	}

	float sensitivity = 0;
	switch(ACCEL_FS_SEL) {
	case FSR_2g:
		sensitivity = 16384.0;
		break;
	case FSR_4g:
		sensitivity = 8192.0;
		break;
	case FSR_8g:
		sensitivity = 4096.0;
		break;
	case FSR_16g:
		sensitivity = 2048.0;
		break;
	default:
		break;
	}

	imu->accel_sensitivity = sensitivity;
	return 0;
}

void Get_Gyro(ICM20948* imu, float* gyro) {
	uint16_t buffer;
	Set_Ub(imu, 0);
	Read_16bit(imu, GYRO_XOUT_H, &buffer);
	gyro[0] = uint16_To_int16(buffer) / imu->gyro_sensitivity;
	Read_16bit(imu, GYRO_YOUT_H, &buffer);
	gyro[1] = uint16_To_int16(buffer) / imu->gyro_sensitivity;
	Read_16bit(imu, GYRO_ZOUT_H, &buffer);
	gyro[2] = uint16_To_int16(buffer) / imu->gyro_sensitivity;
}

void Get_Accel(ICM20948* imu, float* accel) {
	uint16_t buffer;
	Set_Ub(imu, 0);
	Read_16bit(imu, ACCEL_XOUT_H, &buffer);
	accel[0] = uint16_To_int16(buffer) / imu->accel_sensitivity;
	Read_16bit(imu, ACCEL_YOUT_H, &buffer);
	accel[1] = uint16_To_int16(buffer) / imu->accel_sensitivity;
	Read_16bit(imu, ACCEL_ZOUT_H, &buffer);
	accel[2] = uint16_To_int16(buffer) / imu->accel_sensitivity;
}
