#include "lcd_i2c.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
 
static I2C_HandleTypeDef *_hi2c;
 
/* PCF8574 bit mapping */
#define RS  (1 << 0)
#define RW  (1 << 1)
#define EN  (1 << 2)
#define BL  (1 << 3)  /* backlight */
 
static void lcd_write_byte(uint8_t data)
{
    HAL_I2C_Master_Transmit(_hi2c, LCD_I2C_ADDR, &data, 1, 10);
}
 
static void lcd_pulse_enable(uint8_t data)
{
    lcd_write_byte(data | EN | BL);
    HAL_Delay(1);
    lcd_write_byte((data & ~EN) | BL);
    HAL_Delay(1);
}
 
static void lcd_send_nibble(uint8_t nibble, uint8_t mode)
{
    uint8_t data = (nibble & 0xF0) | mode | BL;
    lcd_pulse_enable(data);
}
 
static void lcd_send_byte(uint8_t byte, uint8_t mode)
{
    lcd_send_nibble(byte & 0xF0, mode);
    lcd_send_nibble((byte << 4) & 0xF0, mode);
}
 
static void lcd_command(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);
}
 
static void lcd_data(uint8_t data)
{
    lcd_send_byte(data, RS);
}
 
void lcd_init(I2C_HandleTypeDef *hi2c)
{
    _hi2c = hi2c;
    HAL_Delay(50);
 
    /* Init sequence 4-bit mode */
    lcd_send_nibble(0x30, 0);
    HAL_Delay(5);
    lcd_send_nibble(0x30, 0);
    HAL_Delay(1);
    lcd_send_nibble(0x30, 0);
    HAL_Delay(1);
    lcd_send_nibble(0x20, 0);  /* switch to 4-bit */
    HAL_Delay(1);
 
    lcd_command(0x28);  /* 4-bit, 2 lines, 5x8 font */
    lcd_command(0x0C);  /* display on, cursor off */
    lcd_command(0x06);  /* entry mode: increment, no shift */
    lcd_command(0x01);  /* clear display */
    HAL_Delay(2);
}
 
void lcd_clear(void)
{
    lcd_command(0x01);
    HAL_Delay(2);
}
 
void lcd_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    lcd_command(0x80 | (addr + col));
}
 
void lcd_print(const char *str)
{
    while (*str)
        lcd_data((uint8_t)*str++);
}
 
void lcd_printf(uint8_t row, uint8_t col, const char *fmt, ...)
{
    char buf[LCD_COLS + 1];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
 
    /* Pad with spaces to clear previous content */
    uint8_t len = strlen(buf);
    while (len < LCD_COLS) buf[len++] = ' ';
    buf[LCD_COLS] = '\0';
 
    lcd_set_cursor(row, col);
    lcd_print(buf);
}
