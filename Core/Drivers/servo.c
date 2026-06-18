#include "servo.h"
 
static TIM_HandleTypeDef *_htim;
static uint32_t           _channel;
 
/* TIM configured at 50Hz (20ms period), ARR = 19999
 * Pulse width : 1ms (0°) → 2ms (180°)
 * CCR range   : 1000 → 2000 (with 1MHz timer tick) */
 
void servo_init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    _htim    = htim;
    _channel = channel;
    HAL_TIM_PWM_Start(_htim, _channel);
    servo_set_angle(90);  /* center on startup */
}
 
void servo_set_angle(uint8_t angle)
{
    if (angle > 180) angle = 180;
    uint32_t ccr = 1000 + (uint32_t)(angle * 1000UL / 180UL);
    __HAL_TIM_SET_COMPARE(_htim, _channel, ccr);
}
 
