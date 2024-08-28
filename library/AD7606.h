/*
 * AD7606.h
 *
 *  Created on: 2024年8月27日
 *      Author: User
 */

#ifndef INC_AD7606_H_
#define INC_AD7606_H_

#ifdef __cplusplus
 extern "C" {
#endif

/*! -------------------------------------------------------------------------- */
/*! Include headers */
#include <stdint.h>

# define SAMPLING_CHANNEL 8
# define PWM_CHANNEL TIM_CHANNEL_2

/*! -------------------------------------------------------------------------- */
/*! Public function declarations */
void AD7606Init(void);
float AD7606ConvValue(uint16_t, uint8_t);
void AD7606Reset(void);
void AD7606BusyIrqCallback(uint16_t* , uint8_t, SPI_HandleTypeDef*);
void AD7606Stop(TIM_HandleTypeDef* , unsigned int);
void AD7606Start(TIM_HandleTypeDef* , unsigned int, uint8_t, uint8_t);
void AD7606Cs_High();
void AD7606Cs_Low();
void AD7606Rst_High();
void AD7606Rst_Low();
void AD7606SetOversampling(uint8_t);
void AD7606SetRange(uint8_t);

#ifdef __cplusplus
}
#endif

#endif /* INC_AD7606_H_ */
