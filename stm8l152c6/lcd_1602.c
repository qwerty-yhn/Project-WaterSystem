#include"lcd_1602.h"

void delay(unsigned long time)
{
   while(time--)
      asm("nop");
}
void SendCMD(uint8_t dt, uint8_t RS)
{
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);     //RW
    delay(1000);
    if(RS == 1){GPIO_SetBits(GPIOE, GPIO_Pin_5);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_5);}
    GPIO_SetBits(GPIOD, GPIO_Pin_0);      //E 1
    if(dt & 0x10){GPIO_SetBits(GPIOE, GPIO_Pin_4);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_4);}
    if(dt & 0x20){GPIO_SetBits(GPIOE, GPIO_Pin_2);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_2);}
    if(dt & 0x40){GPIO_SetBits(GPIOE, GPIO_Pin_1);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_1);}
    if(dt & 0x80){GPIO_SetBits(GPIOD, GPIO_Pin_3);}else{GPIO_ResetBits(GPIOD, GPIO_Pin_3);}
    delay(100);
    GPIO_ResetBits(GPIOD, GPIO_Pin_0);    //E 0
    GPIO_SetBits(GPIOD, GPIO_Pin_0);      //E 1
    if(dt & 0x1){GPIO_SetBits(GPIOE, GPIO_Pin_4);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_4);}
    if(dt & 0x2){GPIO_SetBits(GPIOE, GPIO_Pin_2);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_2);}
    if(dt & 0x4){GPIO_SetBits(GPIOE, GPIO_Pin_1);}else{GPIO_ResetBits(GPIOE, GPIO_Pin_1);}
    if(dt & 0x8){GPIO_SetBits(GPIOD, GPIO_Pin_3);}else{GPIO_ResetBits(GPIOD, GPIO_Pin_3);}
    delay(100);
    GPIO_ResetBits(GPIOD, GPIO_Pin_0);    //E 0
}
void clear()
{
   SendCMD(0x01,0);
}
void CreateChar(uint8_t addr, const uint8_t *pdata)
{
    uint8_t StartAdres = 0x40;

    switch(addr)
    {
    case Char0x00: StartAdres = StartAdres + 0;break;
    case Char0x01: StartAdres = StartAdres + 0x8;break;
    case Char0x02: StartAdres = StartAdres + 0x10;break;
    case Char0x03: StartAdres = StartAdres + 0x18;break;
    case Char0x04: StartAdres = StartAdres + 0x20;break;
    case Char0x05: StartAdres = StartAdres + 0x28;break;
    case Char0x06: StartAdres = StartAdres + 0x30;break;
    case Char0x07: StartAdres = StartAdres + 0x38;break;
    }

    for(int i = 0;i < 8; i++)
    {
            SendCMD(StartAdres + i, 0); //адрес CGRAM
            SendCMD(pdata[i], 1); //дата
    }
}
void InitLCD()
{ 
   SendCMD(0x30, 0);    //включение 4 битного режима
   SendCMD(0x02, 0);    //нормальный режим
   SendCMD(0x0C, 0);    //курсор в начало
   SendCMD(0x01, 0);    //очистка дисплея 
}
void SetAdres(uint8_t addr)
{
    SendCMD(0x80|(addr & 0x7f),0);
}
void PrintChar(uint8_t addr)
{
    SendCMD(addr,1);
}
uint8_t HEXtoASII(uint8_t i)
{
    switch (i)
    {
    case 0x0: i = 0x30; break;
    case 0x1: i = 0x31; break;
    case 0x2: i = 0x32; break;
    case 0x3: i = 0x33; break;
    case 0x4: i = 0x34; break;
    case 0x5: i = 0x35; break;
    case 0x6: i = 0x36; break;
    case 0x7: i = 0x37; break;
    case 0x8: i = 0x38; break;
    case 0x9: i = 0x39; break;
    case 0xA: i = 0x41; break;
    case 0xB: i = 0x42; break;
    case 0xC: i = 0x43; break;
    case 0xD: i = 0x44; break;
    case 0xE: i = 0x45; break;
    case 0xF: i = 0x46; break;

    default:
        break;
    }

    return i;
}
void PrintChoise(uint8_t addr, uint8_t Choise)
{
    SetAdres(addr);
    PrintChar(HEXtoASII((Choise & 0xf0)>>4));
    SetAdres(addr + 1);
    PrintChar(HEXtoASII((Choise & 0x0f)));
}
void PrintChoise12(uint8_t addr, uint16_t Choise)
{
    SetAdres(addr);
    PrintChar(HEXtoASII((Choise & 0x0f00)>>8));
    SetAdres(addr + 1);
    PrintChar(HEXtoASII((Choise & 0x00f0)>>4));
    SetAdres(addr + 2);
    PrintChar(HEXtoASII((Choise & 0x000f)));
}
void StartAdress_PrintChar(uint8_t addr, const uint8_t *pdata, uint8_t size)
{
     SetAdres(addr);
     for(uint8_t i = 0; i < size; i++)
     {
         PrintChar(pdata[i]);
     }
}



