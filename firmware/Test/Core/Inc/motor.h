#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32f1xx_hal.h"
#include "main.h"

typedef struct {
	TIM_HandleTypeDef* left_htim;
	TIM_HandleTypeDef* right_htim;
	uint32_t left_channel;
	uint32_t right_channel;
} MotorHandle;

typedef struct {
	GPIO_TypeDef* left_gpio_port;
	GPIO_TypeDef* right_gpio_port;
	uint16_t left_int1_pin;
	uint16_t left_int2_pin;
	uint16_t right_int1_pin;
	uint16_t right_int2_pin;
	MotorHandle motor_handle;
} Motor;

void Motor_Start(Motor* motor);
void Motor_Stop(Motor* motor);

void Set_Left_Motor(Motor* motor, float duty);
void Set_Right_Motor(Motor* motor, float duty);

#endif
