#ifndef DS18B20_H
#define DS18B20_H
 
#include "stm32l4xx_hal.h"
#include <stdint.h>
 
typedef enum {
    DS18B20_OK    = 0,
    DS18B20_ERROR = 1
} DS18B20_Status;
 
void           ds18b20_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin);
DS18B20_Status ds18b20_read_temperature(float *temp_c);
 
#endif
 
