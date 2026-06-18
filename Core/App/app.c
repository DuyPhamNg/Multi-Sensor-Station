#include "app.h"
#include "../Drivers/ds18b20.h"
#include "../Drivers/sensors.h"
#include "../Drivers/lcd_i2c.h"
#include <stdio.h>
 
/* ── Queue and mutex handles ────────────────────────────────────────────── */
QueueHandle_t    temp_queue;
QueueHandle_t    water_queue;
QueueHandle_t    joy_queue;
SemaphoreHandle_t i2c_mutex;
 
/* ── Peripheral handles ─────────────────────────────────────────────────── */
static I2C_HandleTypeDef  *_hi2c;
static TIM_HandleTypeDef  *_htim_pwm;
 
/* ── Buzzer pin ─────────────────────────────────────────────────────────── */
#define BUZZER_PORT  GPIOB
#define BUZZER_PIN   GPIO_PIN_5
 
/* ── Servo angle to PWM duty ────────────────────────────────────────────── */
static void servo_set_angle(uint8_t angle)
{
    /* Servo: 1ms (0°) to 2ms (180°) pulse on 20ms period
     * TIM configured for 50Hz (20ms), ARR = 19999
     * CCR = 1000 + angle * (1000/180) */
    uint32_t ccr = 1000 + (uint32_t)(angle * 1000UL / 180UL);
    __HAL_TIM_SET_COMPARE(_htim_pwm, TIM_CHANNEL_1, ccr);
}
 
/* ── task_temp : DS18B20 acquisition @ 1s ───────────────────────────────── */
static void task_temp(void *arg)
{
    float temp;
    while (1)
    {
        if (ds18b20_read_temperature(&temp) == DS18B20_OK)
            xQueueOverwrite(temp_queue, &temp);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
 
/* ── task_water : water level @ 500ms ──────────────────────────────────── */
static void task_water(void *arg)
{
    uint8_t pct;
    while (1)
    {
        pct = water_level_get_percent();
        xQueueOverwrite(water_queue, &pct);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
 
/* ── task_joystick : joystick + servo @ 50ms ────────────────────────────── */
static void task_joystick(void *arg)
{
    uint16_t joy[2];
    while (1)
    {
        joy[0] = joystick_get_x();
        joy[1] = joystick_get_y();
        xQueueOverwrite(joy_queue, joy);
 
        /* Map joystick X (0–4095) to servo angle (0°–180°) */
        uint8_t angle = (uint8_t)(joy[0] * 180UL / 4095UL);
        servo_set_angle(angle);
 
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
 
/* ── task_display : LCD refresh @ 200ms ─────────────────────────────────── */
static void task_display(void *arg)
{
    float    temp  = 0.0f;
    uint8_t  water = 0;
    uint16_t joy[2] = {0, 0};
    char     line[17];
 
    while (1)
    {
        xQueuePeek(temp_queue,  &temp,  0);
        xQueuePeek(water_queue, &water, 0);
        xQueuePeek(joy_queue,   joy,    0);
 
        if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
        {
            /* Line 0 : temperature + water level */
            snprintf(line, sizeof(line), "T:%.1fC W:%d%%", temp, water);
            lcd_printf(0, 0, "%s", line);
 
            /* Line 1 : joystick position */
            snprintf(line, sizeof(line), "JX:%4d JY:%4d", joy[0], joy[1]);
            lcd_printf(1, 0, "%s", line);
 
            xSemaphoreGive(i2c_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
 
/* ── task_alert : threshold check + buzzer @ 100ms ──────────────────────── */
static void task_alert(void *arg)
{
    float   temp  = 0.0f;
    uint8_t water = 0;
 
    while (1)
    {
        xQueuePeek(temp_queue,  &temp,  0);
        xQueuePeek(water_queue, &water, 0);
 
        if (temp > TEMP_THRESHOLD_C || water > WATER_THRESHOLD_PCT)
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
 
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
 
/* ── Public API ─────────────────────────────────────────────────────────── */
void app_init(I2C_HandleTypeDef *hi2c, TIM_HandleTypeDef *htim,
              ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_pwm)
{
    _hi2c     = hi2c;
    _htim_pwm = htim_pwm;
 
    sensors_init(hadc);
    ds18b20_init(htim, GPIOA, GPIO_PIN_3);  /* adjust pin */
 
    lcd_init(hi2c);
    lcd_clear();
    lcd_printf(0, 0, "Multi-Sensor");
    lcd_printf(1, 0, "  Station v1");
    HAL_Delay(1500);
    lcd_clear();
 
    HAL_TIM_PWM_Start(htim_pwm, TIM_CHANNEL_1);
    servo_set_angle(90);  /* center position */
 
    /* Buzzer GPIO */
    GPIO_InitTypeDef cfg = {0};
    cfg.Pin   = BUZZER_PIN;
    cfg.Mode  = GPIO_MODE_OUTPUT_PP;
    cfg.Pull  = GPIO_NOPULL;
    cfg.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &cfg);
}
 
void app_start(void)
{
    /* Create queues (depth 1, overwrite on full) */
    temp_queue  = xQueueCreate(1, sizeof(float));
    water_queue = xQueueCreate(1, sizeof(uint8_t));
    joy_queue   = xQueueCreate(1, sizeof(uint16_t[2]));
 
    /* Create mutex for I2C bus */
    i2c_mutex = xSemaphoreCreateMutex();
 
    /* Create tasks */
    xTaskCreate(task_temp,     "TEMP",     256, NULL, 2, NULL);
    xTaskCreate(task_water,    "WATER",    128, NULL, 2, NULL);
    xTaskCreate(task_joystick, "JOY",      128, NULL, 3, NULL);
    xTaskCreate(task_display,  "DISPLAY",  256, NULL, 1, NULL);
    xTaskCreate(task_alert,    "ALERT",    128, NULL, 3, NULL);
 
    /* Start FreeRTOS scheduler */
    vTaskStartScheduler();
}
