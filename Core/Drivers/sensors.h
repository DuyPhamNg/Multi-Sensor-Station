#ifndef SENSORS_H
#define SENSORS_H
 
#include "stm32l4xx_hal.h"
#include <stdint.h>
 
void     sensors_init(ADC_HandleTypeDef *hadc);
uint8_t  water_level_get_percent(void);   /* 0–100 % */
uint16_t joystick_get_x(void);            /* 0–4095  */
uint16_t joystick_get_y(void);            /* 0–4095  */
 
#endif
