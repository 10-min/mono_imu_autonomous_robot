/*
 * motor.c
 *
 *  Created on: Nov 26, 2025
 *      Author: tm
 */


#include "motor.h"
#include <stdlib.h>

#ifndef MIN
#define MIN
float Min(float a, float b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}
#endif

void Motor_Start(Motor* motor) {
	HAL_TIM_PWM_Start(motor->motor_handle.left_htim, motor->motor_handle.left_channel);
	HAL_TIM_PWM_Start(motor->motor_handle.right_htim, motor->motor_handle.right_channel);
	Set_Left_Motor(motor, 0);
	Set_Right_Motor(motor, 0);
}

void Motor_Stop(Motor* motor) {
	HAL_TIM_PWM_Stop(motor->motor_handle.left_htim, motor->motor_handle.left_channel);
	HAL_TIM_PWM_Stop(motor->motor_handle.right_htim, motor->motor_handle.right_channel);
}

void Set_Left_Motor(Motor* motor, float duty) {
	uint16_t ccr = (uint16_t) (motor->motor_handle.left_htim->Instance->ARR * Min(100, fabsf(duty)) / 100);
	if (duty > 0) {
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int1_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int2_pin, GPIO_PIN_SET);
	} else if (duty < 0) {
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int1_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int2_pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int1_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->left_gpio_port, motor->left_int2_pin, GPIO_PIN_RESET);
	}
	__HAL_TIM_SET_COMPARE(motor->motor_handle.left_htim, motor->motor_handle.left_channel, ccr);
}

void Set_Right_Motor(Motor* motor, float duty) {
	uint16_t ccr = (uint16_t) (motor->motor_handle.right_htim->Instance->ARR * Min(100, fabsf(duty)) / 100);
	if (duty > 0) {
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int1_pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int2_pin, GPIO_PIN_RESET);
	} else if (duty < 0) {
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int1_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int2_pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int1_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(motor->right_gpio_port, motor->right_int2_pin, GPIO_PIN_RESET);
	}
	__HAL_TIM_SET_COMPARE(motor->motor_handle.right_htim, motor->motor_handle.right_channel, ccr);
}
