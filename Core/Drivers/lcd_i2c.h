#ifndef LCD_I2C_H
#define LCD_I2C_H
 
#include "stm32l4xx_hal.h"
#include <stdint.h>
 
#define LCD_I2C_ADDR   (0x27 << 1)  /* PCF8574 default address */
#define LCD_COLS       16
#define LCD_ROWS       2
 
void lcd_init(I2C_HandleTypeDef *hi2c);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(const char *str);
void lcd_printf(uint8_t row, uint8_t col, const char *fmt, ...);
 
#endif
