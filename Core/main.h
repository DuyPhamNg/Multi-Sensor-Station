#ifndef __MAIN_H
#define __MAIN_H
 
#include "stm32l4xx_hal.h"
 
/* ── Exported handles ───────────────────────────────────────────────────── */
extern ADC_HandleTypeDef  hadc1;
extern I2C_HandleTypeDef  hi2c1;
extern TIM_HandleTypeDef  htim2;   /* 1 MHz timebase — DS18B20 µs timing  */
extern TIM_HandleTypeDef  htim3;   /* 50 Hz PWM       — servo motor       */
extern UART_HandleTypeDef huart2;
 
/* ── Pin definitions ────────────────────────────────────────────────────── */
 
/* DS18B20 one-wire */
#define DS18B20_Pin       GPIO_PIN_3
#define DS18B20_GPIO_Port GPIOA
 
/* Water level sensor */
#define WATER_Pin         GPIO_PIN_1
#define WATER_GPIO_Port   GPIOA
 
/* Joystick X / Y */
#define JOY_X_Pin         GPIO_PIN_2
#define JOY_X_GPIO_Port   GPIOA
#define JOY_Y_Pin         GPIO_PIN_0
#define JOY_Y_GPIO_Port   GPIOB
 
/* Servo motor PWM — TIM3 CH1 */
#define SERVO_Pin         GPIO_PIN_6
#define SERVO_GPIO_Port   GPIOA
 
/* Buzzer */
#define BUZZER_Pin        GPIO_PIN_5
#define BUZZER_GPIO_Port  GPIOB
 
/* Green LED LD2 */
#define LD2_Pin           GPIO_PIN_5
#define LD2_GPIO_Port     GPIOA
 
/* User button B1 */
#define B1_Pin            GPIO_PIN_13
#define B1_GPIO_Port      GPIOC
 
/* ── Error handler ──────────────────────────────────────────────────────── */
void Error_Handler(void);
 
#endif
