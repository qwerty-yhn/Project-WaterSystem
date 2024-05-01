
#ifndef __lcd_1602_H
#define __lcd_1602_H

#include"stm8l15x_gpio.h"

#define Char0x00 0x0
#define Char0x01 0x1
#define Char0x02 0x2
#define Char0x03 0x3
#define Char0x04 0x4
#define Char0x05 0x5
#define Char0x06 0x6
#define Char0x07 0x7


void delay(unsigned long time);
void LCD_SendString(char *str);
void I2C_send(uint8_t data, uint8_t flags);
void CreateChar(uint8_t addr, const uint8_t *pdata);
void clear();
void InitLCD();
void SendCMD(uint8_t dt, uint8_t RS);
void SetAdres(uint8_t addr);
void PrintChar(uint8_t addr);
void StringClear(uint8_t StrtAddr);
void StartAdress_PrintChar(uint8_t addr, const uint8_t *pdata, uint8_t size);
void StartAdress_PrintCharVar(uint8_t addr, uint8_t *pdata, uint8_t size);
void PrintChoise(uint8_t addr, uint8_t Choise);
void PrintChoise12(uint8_t addr, uint16_t Choise);
void CursEn();

#endif /*__STM8L051f3_RTC_H */

