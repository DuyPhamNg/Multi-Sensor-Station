#include "ds18b20.h"
 
static TIM_HandleTypeDef *_htim;
static GPIO_TypeDef      *_port;
static uint16_t           _pin;
 
/* ── µs delay via hardware timer ───────────────────────────────────────── */
static void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(_htim, 0);
    while (__HAL_TIM_GET_COUNTER(_htim) < us);
}
 
/* ── Set pin as output ──────────────────────────────────────────────────── */
static void set_output(void)
{
    GPIO_InitTypeDef cfg = {0};
    cfg.Pin   = _pin;
    cfg.Mode  = GPIO_MODE_OUTPUT_PP;
    cfg.Pull  = GPIO_NOPULL;
    cfg.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(_port, &cfg);
}
 
/* ── Set pin as input ───────────────────────────────────────────────────── */
static void set_input(void)
{
    GPIO_InitTypeDef cfg = {0};
    cfg.Pin  = _pin;
    cfg.Mode = GPIO_MODE_INPUT;
    cfg.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(_port, &cfg);
}
 
/* ── 1-Wire reset pulse ─────────────────────────────────────────────────── */
static DS18B20_Status one_wire_reset(void)
{
    set_output();
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
    delay_us(480);
    set_input();
    delay_us(70);
 
    if (HAL_GPIO_ReadPin(_port, _pin) == GPIO_PIN_RESET)
    {
        delay_us(410);
        return DS18B20_OK;
    }
    return DS18B20_ERROR;
}
 
/* ── Write one bit ──────────────────────────────────────────────────────── */
static void one_wire_write_bit(uint8_t bit)
{
    set_output();
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
    delay_us(bit ? 6 : 60);
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);
    delay_us(bit ? 54 : 10);
}
 
/* ── Read one bit ───────────────────────────────────────────────────────── */
static uint8_t one_wire_read_bit(void)
{
    uint8_t bit;
    set_output();
    HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET);
    delay_us(2);
    set_input();
    delay_us(10);
    bit = HAL_GPIO_ReadPin(_port, _pin);
    delay_us(50);
    return bit;
}
 
/* ── Write one byte ─────────────────────────────────────────────────────── */
static void one_wire_write_byte(uint8_t byte)
{
    for (int i = 0; i < 8; i++)
        one_wire_write_bit((byte >> i) & 0x01);
}
 
/* ── Read one byte ──────────────────────────────────────────────────────── */
static uint8_t one_wire_read_byte(void)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
        byte |= (one_wire_read_bit() << i);
    return byte;
}
 
/* ── Public API ─────────────────────────────────────────────────────────── */
void ds18b20_init(TIM_HandleTypeDef *htim, GPIO_TypeDef *port, uint16_t pin)
{
    _htim = htim;
    _port = port;
    _pin  = pin;
    HAL_TIM_Base_Start(_htim);
}
 
DS18B20_Status ds18b20_read_temperature(float *temp_c)
{
    uint8_t lsb, msb;
    int16_t raw;
 
    if (one_wire_reset() != DS18B20_OK) return DS18B20_ERROR;
    one_wire_write_byte(0xCC);  /* Skip ROM */
    one_wire_write_byte(0x44);  /* Convert T */
 
    HAL_Delay(750);             /* Wait conversion (12-bit = 750ms max) */
 
    if (one_wire_reset() != DS18B20_OK) return DS18B20_ERROR;
    one_wire_write_byte(0xCC);  /* Skip ROM */
    one_wire_write_byte(0xBE);  /* Read scratchpad */
 
    lsb = one_wire_read_byte();
    msb = one_wire_read_byte();
 
    raw    = (int16_t)(msb << 8 | lsb);
    *temp_c = (float)raw / 16.0f;  /* 12-bit resolution: LSB = 0.0625°C */
 
    return DS18B20_OK;
}
