#ifndef APP_H
#define APP_H
 
#include "stm32l4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
 
/* ── Shared queues ──────────────────────────────────────────────────────── */
extern QueueHandle_t temp_queue;   /* float temperature */
extern QueueHandle_t water_queue;  /* uint8_t percent   */
extern QueueHandle_t joy_queue;    /* uint16_t[2] x,y   */
 
/* ── Shared mutex ───────────────────────────────────────────────────────── */
extern SemaphoreHandle_t i2c_mutex;
 
/* ── Alert thresholds ───────────────────────────────────────────────────── */
#define TEMP_THRESHOLD_C     35.0f  /* buzzer above 35°C  */
#define WATER_THRESHOLD_PCT  80     /* buzzer above 80%   */
 
void app_init(I2C_HandleTypeDef *hi2c, TIM_HandleTypeDef *htim,
              ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_pwm);
void app_start(void);
 
#endif
