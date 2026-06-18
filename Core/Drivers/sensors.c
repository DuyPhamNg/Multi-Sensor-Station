#include "sensors.h"
 
static ADC_HandleTypeDef *_hadc;
 
void sensors_init(ADC_HandleTypeDef *hadc)
{
    _hadc = hadc;
}
 
/* ── Read ADC on given channel ──────────────────────────────────────────── */
static uint16_t adc_read_channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef cfg = {0};
    cfg.Channel      = channel;
    cfg.Rank         = ADC_REGULAR_RANK_1;
    cfg.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
    cfg.SingleDiff   = ADC_SINGLE_ENDED;
    cfg.OffsetNumber = ADC_OFFSET_NONE;
    HAL_ADC_ConfigChannel(_hadc, &cfg);
 
    /* Average 8 samples to reduce noise */
    uint32_t sum = 0;
    for (int i = 0; i < 8; i++)
    {
        HAL_ADC_Start(_hadc);
        HAL_ADC_PollForConversion(_hadc, 10);
        sum += HAL_ADC_GetValue(_hadc);
        HAL_ADC_Stop(_hadc);
    }
    return (uint16_t)(sum / 8);
}
 
/* ── Water level — ADC_CHANNEL_6 (PA1) ─────────────────────────────────── */
uint8_t water_level_get_percent(void)
{
    uint16_t raw = adc_read_channel(ADC_CHANNEL_6);
    uint8_t  pct = (uint8_t)(raw * 100UL / 4095UL);
    return pct;
}
 
/* ── Joystick X — ADC_CHANNEL_7 (PA2) ──────────────────────────────────── */
uint16_t joystick_get_x(void)
{
    return adc_read_channel(ADC_CHANNEL_7);
}
 
/* ── Joystick Y — ADC_CHANNEL_8 (PB0) ──────────────────────────────────── */
uint16_t joystick_get_y(void)
{
    return adc_read_channel(ADC_CHANNEL_8);
}
