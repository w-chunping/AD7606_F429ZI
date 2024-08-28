/*
 * AD7606.c
 *
 *  Created on: 2024年8月27日
 *      Author: User
 */

/*============= include files ============== */
//#include "stm32f4xx_hal_conf.h"
//#include "stm32f4xx_it.h"
#include "main.h"
#include "AD7606.h"
#include "stm32f4xx.h"


void AD7606Rst_Low(){
	HAL_GPIO_WritePin(AD7606Rst_GPIO_Port, AD7606Rst_Pin, GPIO_PIN_RESET);
}

void AD7606Rst_High(){
	HAL_GPIO_WritePin(AD7606Rst_GPIO_Port, AD7606Rst_Pin, GPIO_PIN_SET);
}

void AD7606Cs_Low(){
	HAL_GPIO_WritePin(AD7606Cs_GPIO_Port, AD7606Cs_Pin, GPIO_PIN_RESET);
}

void AD7606Cs_High(){
	HAL_GPIO_WritePin(AD7606Cs_GPIO_Port, AD7606Cs_Pin, GPIO_PIN_SET);
}

void AD7606Start(TIM_HandleTypeDef* htim4, unsigned int PWM_channel, uint8_t OverSamplingState, uint8_t range)
{
    HAL_TIM_PWM_Start(htim4,PWM_channel);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    AD7606SetOversampling(OverSamplingState);
    AD7606SetRange(range);
}

void AD7606Stop(TIM_HandleTypeDef* htim4, unsigned int PWM_channel)
{
    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    HAL_TIM_PWM_Stop(htim4,PWM_channel);
    AD7606Cs_High();
}

void AD7606BusyIrqCallback(uint16_t *ad7606Val,uint8_t ad7606Chl, SPI_HandleTypeDef* hspi2)
{
    AD7606Cs_Low();
    HAL_SPI_Receive(hspi2,(uint8_t *)ad7606Val,ad7606Chl,10000);
    AD7606Cs_High();
}

void AD7606Reset(void)
{
    /*! ___|-----|________  >= 50ns */
    AD7606Rst_Low();
    AD7606Rst_High();
    for(int i = 20; i > 0; i--){
        __NOP();//1000/168 ns = 5.85ns
    }
    AD7606Rst_Low();
}

float AD7606ConvValue(uint16_t bin, uint8_t range)
{
    int _val;
    float adcValue;
    _val = bin&0x8000 ? (-((~bin+1)&0x7fff)) : bin;
    adcValue = (float)range*_val/32768.0;		//adjust for your range
    return adcValue;
}

void AD7606Init(void)
{
    AD7606Cs_High();
    AD7606Reset();
}

void AD7606SetOversampling(uint8_t oversampling_state){
	switch(oversampling_state){
		case 1:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_RESET);
			break;
		case 4:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_RESET);
			break;
		case 8:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_RESET);
			break;
		case 16:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_SET);
			break;
		case 32:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_SET);
			break;
		case 64:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_SET);
			break;
		default:
			HAL_GPIO_WritePin(AD7606Os0_GPIO_Port, AD7606Os0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os1_GPIO_Port, AD7606Os1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(AD7606Os2_GPIO_Port, AD7606Os2_Pin, GPIO_PIN_RESET);
			break;
	}
}

void AD7606SetRange(uint8_t range){
	if(range == 5) HAL_GPIO_WritePin(AD7606Range_GPIO_Port, AD7606Range_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(AD7606Range_GPIO_Port, AD7606Range_Pin, GPIO_PIN_SET);
}
