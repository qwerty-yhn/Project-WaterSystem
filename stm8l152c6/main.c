#include"stm8l15x.h"
#include"stm8l15x_clk.h"
#include"stm8l15x_exti.h"
#include"stm8l15x_gpio.h"
#include"stm8l15x_rtc.h"
#include"stm8l15x_syscfg.h"
#include"stm8l15x_tim1.h"
#include"stm8l15x_tim4.h"
#include"stm8l15x_usart.h"
#include"stm8l15x_wwdg.h"
#include"stm8l15x_conf.h"
#include"lcd_1602.h"

#define UP 0x0A
#define DOWN 0x0B
#define LEFT 0x0C
#define RIGHT 0x0D
#define ENTER_DEF 0x0E
#define ESCAPE_DEF 0x0F

//////////////////////////
uint8_t ParamHour = 0x12;    //Начало полива Режима 1
uint8_t ParamMin = 0x00;
/////////////////////////
uint8_t ParamMins = 0x00;   //Время полива режима 1
uint8_t ParamSecs = 0x00;
/////////////////////////
uint8_t WeekDay = 0x80;    //Дни недели режима 1
/////////////////////////
uint8_t NumPomp = 1;      //Номер помпы
/////////////////////////
uint8_t ParamMode = 0;   // ВКЛ/ОТКЛ режим 1
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//////////////////////////
uint8_t ParamHour2 = 0x12;    //Начало полива Режима 2
uint8_t ParamMin2 = 0x00;
/////////////////////////
uint8_t ParamMins2 = 0x00;   //Время полива режима 2
uint8_t ParamSecs2 = 0x00;
/////////////////////////
uint8_t WeekDay2 = 0x80;    //Дни недели режима 2
/////////////////////////
uint8_t NumPomp2 = 1;      //Номер помпы
/////////////////////////
uint8_t ParamMode2 = 0;   // ВКЛ/ОТКЛ режим 2
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//////////////////////////
uint8_t ParamHour3 = 0x12;    //Начало полива Режима 3
uint8_t ParamMin3 = 0x00;
/////////////////////////
uint8_t ParamMins3 = 0x00;   //Время полива режима 3
uint8_t ParamSecs3 = 0x00;
/////////////////////////
uint8_t WeekDay3 = 0x80;    //Дни недели режима 3
/////////////////////////
uint8_t NumPomp3 = 1;      //Номер помпы
/////////////////////////
uint8_t ParamMode3 = 0;   // ВКЛ/ОТКЛ режим 3
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//////////////////////////
uint8_t ParamHour4 = 0x12;    //Начало полива Режима 3
uint8_t ParamMin4 = 0x00;
/////////////////////////
uint8_t ParamMins4 = 0x00;   //Время полива режима 3
uint8_t ParamSecs4 = 0x00;
/////////////////////////
uint8_t WeekDay4 = 0x80;    //Дни недели режима 3
/////////////////////////
uint8_t NumPomp4 = 1;      //Номер помпы
/////////////////////////
uint8_t ParamMode4 = 0;   // ВКЛ/ОТКЛ режим 3
//////////////////Свет///////////////////////////////////
uint8_t ParamStartLightHour = 0x12;
uint8_t ParamStartLightMin = 0x00;
///////////////////////////////////////////
uint8_t ParamTimeLightHour = 0x00;
uint8_t ParamTimeLightMin = 0x00;
/////////////////////////////////////////////
uint8_t WeekDayLight = 0x80;
////////////////////////////////////////////
uint8_t ParamModeLight = 0;
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


uint8_t Escape = 0,cursX = 1,cursY = 1,UpDate = 1;
uint8_t strb = 0,strbTop = 0,SaveCursY = 0x11,PreSaveCursY = 0,SaveTopMenu = 1;
uint8_t cst = 0x0,SVcst = 0x0,WeekStrb = 0,strb2 = 0,cursYold = 1;
uint8_t cursYStr = 1,s_cursY = 1,ParseButton = 0,cursXStr = 1;

void SystemClock_Config(void);
void ProgDelay(unsigned long time);

INTERRUPT_HANDLER(TIM1_IRQHandler, 23)     //Timer  
{
   UpDate = 1;

   Escape = 0;
   cursX = 1;
   cursY = 1;
   SaveTopMenu = 1;
   WeekDay|=0x80;

   GPIO_ToggleBits(GPIOB, GPIO_Pin_7);

   TIM1_ClearITPendingBit(TIM1_IT_Update);
   TIM1_Cmd(DISABLE);
}
INTERRUPT_HANDLER(EXTI1_IRQHandler, 8)       //Escape
{
   ProgDelay(2000);
   UpDate = 1;
   ParseButton = ESCAPE_DEF;
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_0)
   {
      Escape--; 
      EXTI_ClearITPendingBit(EXTI_IT_Pin0);
      TIM1_SetCounter(0); 
      TIM1_Cmd(ENABLE);                 
   }

}
INTERRUPT_HANDLER(EXTI2_IRQHandler, 9)       //enter
{
   ProgDelay(2000);
   UpDate = 1; 
   ParseButton = ENTER_DEF;
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_1)
   {
         if(WeekDay&0x80)
         {
            Escape++;
         }
         else
         {
            cst++;     
         }
         EXTI_ClearITPendingBit(EXTI_IT_Pin1);
         TIM1_SetCounter(0); 
         TIM1_Cmd(ENABLE);
   }

}
INTERRUPT_HANDLER(EXTI3_IRQHandler, 10)      //pomp
{
   ProgDelay(2000);
   UpDate = 1; 

EXTI_ClearITPendingBit(EXTI_IT_Pin2);
}
INTERRUPT_HANDLER(EXTI4_IRQHandler, 11)      //left
{
   ProgDelay(500);
   UpDate = 1;
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_3)
   { 
      ParseButton = LEFT;
      cursX--;
      EXTI_ClearITPendingBit(EXTI_IT_Pin3);
            TIM1_SetCounter(0); 
      TIM1_Cmd(ENABLE);
   }

}
INTERRUPT_HANDLER(EXTI5_IRQHandler, 12)     //down
{
   ProgDelay(2000);
   UpDate = 1;
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_4)
   { 
      ParseButton = DOWN;
      cursY++;
      EXTI_ClearITPendingBit(EXTI_IT_Pin4);
            TIM1_SetCounter(0); 
      TIM1_Cmd(ENABLE);
   }

}
INTERRUPT_HANDLER(EXTI6_IRQHandler, 13)     //right
{
   ProgDelay(500);
   UpDate = 1;
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_5)
   {
      ParseButton = RIGHT;
      cursX++;
      EXTI_ClearITPendingBit(EXTI_IT_Pin5);
            TIM1_SetCounter(0); 
      TIM1_Cmd(ENABLE);
   }

}
INTERRUPT_HANDLER(EXTI7_IRQHandler, 14)     //up
{
   ProgDelay(2000);
   UpDate = 1; 
   if(GPIO_ReadInputData(GPIOB) & GPIO_Pin_6)
   {
      ParseButton = UP;
      cursY--;
      EXTI_ClearITPendingBit(EXTI_IT_Pin6);
            TIM1_SetCounter(0); 
      TIM1_Cmd(ENABLE);
   }

}



int main()
{
   SystemClock_Config();
   
   TIM1_TimeBaseInit(65000,
                     TIM1_CounterMode_Up,
                     5000,
                     0);        //было 5000 это 17 секундр
   TIM1_ITConfig(TIM1_IT_Update, ENABLE);           
   TIM1_Cmd(ENABLE);

   RTC_InitTypeDef init;
   init.RTC_HourFormat = RTC_HourFormat_24;
   init.RTC_AsynchPrediv = 0x7f;
   init.RTC_SynchPrediv = 0xff;

   RTC_Init(&init);

   RTC_DateTypeDef Date;
   RTC_DateStructInit(&Date);

   RTC_TimeTypeDef default_time;

   default_time.RTC_Hours = 0x12;
   default_time.RTC_Minutes = 0x00;
   default_time.RTC_Seconds = 0;

   RTC_SetTime(RTC_Format_BCD, &default_time);

    RTC_DateTypeDef default_date;
   
     default_date.RTC_Date = 0x1;
     default_date.RTC_Month = RTC_Month_January;
     default_date.RTC_WeekDay = RTC_Weekday_Monday;
     default_date.RTC_Year = 0x21;

    RTC_SetDate(RTC_Format_BCD, &default_date);

   RTC_TimeTypeDef time;
   RTC_DateTypeDef date;
   RTC_GetTime(RTC_Format_BCD, &time);
   RTC_GetDate(RTC_Format_BCD, &date);

   GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow); //LED

   GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_4, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_5, GPIO_Mode_In_PU_IT); 
   GPIO_Init(GPIOB, GPIO_Pin_6, GPIO_Mode_In_PU_IT);

   GPIO_Init(GPIOE, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOE, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOE, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOE, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOE, GPIO_Pin_4, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOE, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOD, GPIO_Pin_1, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOD, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Slow); 
   GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow); 

   GPIO_Init(GPIOB, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow);
   GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Slow);

   EXTI_SetPinSensitivity(EXTI_Pin_0,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_2,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_3,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_4,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_5,EXTI_Trigger_Falling);
   EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);


   
   // GPIO_ResetBits(GPIOE, GPIO_Pin_0);  //доп
   // GPIO_ResetBits(GPIOE, GPIO_Pin_1);   //d6
   // GPIO_ResetBits(GPIOE, GPIO_Pin_2);  //d5
   // GPIO_SetBits(GPIOE, GPIO_Pin_3); //rW
   // GPIO_ResetBits(GPIOE, GPIO_Pin_4);// d4
   // GPIO_ResetBits(GPIOE, GPIO_Pin_5);//rs
   // GPIO_ResetBits(GPIOD, GPIO_Pin_0);  //E
   // GPIO_ResetBits(GPIOD, GPIO_Pin_3);   //d7
   
   
   InitLCD();
   const uint8_t customChar[] = {0x00,0x00,0x00,0x00,0x00,0x11,0x0A,0x04};
   CreateChar(Char0x00, customChar);
   const uint8_t customChar1[] = {0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0x00}; //П
   CreateChar(Char0x01, customChar1);
   const uint8_t customChar2[] = {0xF,0x05,0x05,0x05,0x05,0x15,0x09,0x00}; //Л
   CreateChar(Char0x02, customChar2);
   const uint8_t customChar3[] = {0x11,0x11,0x13,0x15,0x19,0x11,0x11,0x00}; //И
   CreateChar(Char0x03, customChar3);
   const uint8_t customChar4[] = {0x15,0x15,0x15,0x0E,0x15,0x15,0x15,0x00}; //Ж
   CreateChar(Char0x04, customChar4);
   const uint8_t customChar5[] = {0x0f,0x05,0x05,0x05,0x09,0x1F,0x11,0x00}; //Д
   CreateChar(Char0x05, customChar5);
   const uint8_t customChar6[] = {0x11,0x11,0x11,0x0F,0x01,0x01,0x01,0x00}; //Ч
   CreateChar(Char0x06, customChar6);
   const uint8_t customChar7[] = {0x00,0x00,0x01,0x02,0x14,0x08,0x00,0x00}; //галочка
   CreateChar(Char0x07, customChar7);

   const uint8_t  i000[] = {Char0x01,'O',Char0x02,Char0x03,'B'};
         const uint8_t  i100[] = {'P','E',Char0x04,Char0x03,'M','1'};
            const uint8_t  i110[] = {'H','A',Char0x06,'.',Char0x01,'O',Char0x02,Char0x03,'B','A',' '};
            const uint8_t  i120[] = {'B','P','E','M','.',Char0x01,'O',Char0x02,Char0x03,'B','A'};
            const uint8_t  i130[] = {'H','O','M','.','H','A','C','O','C','A'};
            const uint8_t  i140[] = {'B','K',Char0x02,' '};
            const uint8_t  i160[] = {'O','T','K',Char0x02};
            const uint8_t  i150[] = {Char0x05,'H',Char0x03,' ','H','E',Char0x05,'E',Char0x02,Char0x03};
         const uint8_t  i200[] = {'P','E',Char0x04,Char0x03,'M','2'};
         const uint8_t  i300[] = {'P','E',Char0x04,Char0x03,'M','3'};
         const uint8_t  i400[] = {'P','E',Char0x04,Char0x03,'M','4'};


   const uint8_t  i001[] = {'C','B','E','T',' '};

          const uint8_t  i101[] = {'H','A',Char0x06,'.','C','B','E','T'};     
          const uint8_t  i201[] = {'B','P','E','M','.','C','B','E','T'};

   const uint8_t  i002[] = {'H','A','C','T','P'};

         const uint8_t  i102[] = {'H','A','C','T','P','.','B','P','E','M','E','H',Char0x03};

   const uint8_t  i003[] = {Char0x01,'H'};
   const uint8_t  i004[] = {'B','T'};
   const uint8_t  i005[] = {'C','P'};
   const uint8_t  i006[] = {Char0x06,'T'};
   const uint8_t  i007[] = {Char0x01,'T'};
   const uint8_t  i008[] = {'C','T'};
   const uint8_t  i009[] = {'B','C'};
   const uint8_t  i00A[] = {'C','E','K'};
   const uint8_t  i00B[] = {'M',Char0x03,'H'};
   const uint8_t  i00C[] = {Char0x06,'A','C'};
   const uint8_t  i00D[] = {' ',' ',' '};
   const uint8_t  i00E[] = {Char0x05,'H',' '};


   //////////////////Символы///////////////////////////////
   const uint8_t  s00[] = {0x3E};  // '→'
   const uint8_t  s01[] = {0x3C};  // '←'
   const uint8_t  s03[] = {0xfe};  // ' '
   const uint8_t  s05[] = {0x5e};  // '↑'
   const uint8_t  s06[] = {Char0x00};  // '↓'/////&&
   const uint8_t  s07[] = {0x7E};  // '→'/////&&
   const uint8_t  s08[] = {0x78};  // 'x'/////&&
   const uint8_t  s09[] = {Char0x07};  // галочка/////&&
   /////////////////////////////////////////////////////////


   enableInterrupts();
   uint8_t WaterEnable = DISABLE;
   uint8_t WaterEnable2 = DISABLE;
   uint8_t WaterEnable3 = DISABLE;
   uint8_t WaterEnable4 = DISABLE;
   uint8_t LightEnable = DISABLE;
   uint8_t STRWater = 0;
   uint8_t STRWater2 = 0;
   uint8_t STRWater3 = 0;
   uint8_t STRWater4 = 0;
   uint8_t STRLight = 0;

   GPIO_ResetBits(GPIOD, GPIO_Pin_7);  //Pomp
   GPIO_ResetBits(GPIOD, GPIO_Pin_6);  //Light
   while (1)  
   {
 
   ////////////////Проверка Заданный настроек///////////////////////////////////
   if(ParamMode == ENABLE)
   {   

      if(WaterEnable == ENABLE)
      {  
         if(STRWater == 1)
         {
         STRWater = 0;
         ParamMins = ParamMins + time.RTC_Minutes;
         ParamSecs = ParamSecs + time.RTC_Seconds;}
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         if((ParamMins == time.RTC_Minutes) && (ParamSecs == time.RTC_Seconds))
         {
            WaterEnable = DISABLE;
         }

      }
      else
      { 
         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         STRWater = 1;

         if(WeekDay & 0x01)  //ПН
     {
         if(date.RTC_WeekDay == RTC_Weekday_Monday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x02)  //ВТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Tuesday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x04)  //СР
     {
         if(date.RTC_WeekDay == RTC_Weekday_Wednesday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x08)  //ЧТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Thursday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x10)  //ПТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Friday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x20)  //СБ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Saturday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
         if(WeekDay & 0x40)  //ВС
     {
         if(date.RTC_WeekDay == RTC_Weekday_Sunday)
         {
            if((ParamHour == time.RTC_Hours) && (ParamMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable = ENABLE;
            }
         }
     }
      }

   }
   if(ParamMode2 == ENABLE)
   {   

      if(WaterEnable2 == ENABLE)
      {  
         if(STRWater2 == 1)
         {
         STRWater2 = 0;
         ParamMins2 = ParamMins2 + time.RTC_Minutes;
         ParamSecs2 = ParamSecs2 + time.RTC_Seconds;}
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         if((ParamMins2 == time.RTC_Minutes) && (ParamSecs2 == time.RTC_Seconds))
         {
            WaterEnable2 = DISABLE;
         }

      }
      else
      { 
         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         STRWater2 = 1;

         if(WeekDay2 & 0x01)  //ПН
      {
         if(date.RTC_WeekDay == RTC_Weekday_Monday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x02)  //ВТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Tuesday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x04)  //СР
     {
         if(date.RTC_WeekDay == RTC_Weekday_Wednesday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x08)  //ЧТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Thursday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x10)  //ПТ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Friday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x20)  //СБ
     {
         if(date.RTC_WeekDay == RTC_Weekday_Saturday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
         if(WeekDay2 & 0x40)  //ВС
     {
         if(date.RTC_WeekDay == RTC_Weekday_Sunday)
         {
            if((ParamHour2 == time.RTC_Hours) && (ParamMin2 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable2 = ENABLE;
            }
         }
     }
      }

   }
   if(ParamMode3 == ENABLE)
   {   

      if(WaterEnable3 == ENABLE)
      {  
         if(STRWater3 == 1)
         {
         STRWater3 = 0;
         ParamMins3 = ParamMins3 + time.RTC_Minutes;
         ParamSecs3 = ParamSecs3 + time.RTC_Seconds;}
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         if((ParamMins3 == time.RTC_Minutes) && (ParamSecs3 == time.RTC_Seconds))
         {
            WaterEnable3 = DISABLE;
         }

      }
      else
      { 
         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         STRWater3 = 1;

         if(WeekDay3 & 0x01)  //ПН
         {
            if(date.RTC_WeekDay == RTC_Weekday_Monday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x02)  //ВТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Tuesday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x04)  //СР
         {
            if(date.RTC_WeekDay == RTC_Weekday_Wednesday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x08)  //ЧТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Thursday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x10)  //ПТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Friday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x20)  //СБ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Saturday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
         if(WeekDay3 & 0x40)  //ВС
         {   
            if(date.RTC_WeekDay == RTC_Weekday_Sunday)
            {
               if((ParamHour3 == time.RTC_Hours) && (ParamMin3 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable3 = ENABLE;
               }
            }
         }
      }

   }
   if(ParamMode4 == ENABLE)
   {   

      if(WaterEnable4 == ENABLE)
      {  
         if(STRWater4 == 1)
         {
         STRWater4 = 0;
         ParamMins4 = ParamMins4 + time.RTC_Minutes;
         ParamSecs4 = ParamSecs4 + time.RTC_Seconds;}
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         if((ParamMins4 == time.RTC_Minutes) && (ParamSecs4 == time.RTC_Seconds))
         {
            WaterEnable4 = DISABLE;
         }

      }
      else
      { 
         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         STRWater4 = 1;

         if(WeekDay4 & 0x01)  //ПН
         {
            if(date.RTC_WeekDay == RTC_Weekday_Monday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable4 = ENABLE;
               }
            }
         }
         if(WeekDay4 & 0x02)  //ВТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Tuesday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable4 = ENABLE;
            }
            }
         }
         if(WeekDay4 & 0x04)  //СР
         {
            if(date.RTC_WeekDay == RTC_Weekday_Wednesday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable4 = ENABLE;
            }
            }
         }
         if(WeekDay4 & 0x08)  //ЧТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Thursday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable4 = ENABLE;
               } 
            }
         }
         if(WeekDay4 & 0x10)  //ПТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Friday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable4 = ENABLE;
               }
            }
         }
         if(WeekDay4 & 0x20)  //СБ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Saturday)
         {
            if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               WaterEnable4 = ENABLE;
            }
         }
         }
         if(WeekDay4 & 0x40)  //ВС
         {
            if(date.RTC_WeekDay == RTC_Weekday_Sunday)
            {
               if((ParamHour4 == time.RTC_Hours) && (ParamMin4 == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  WaterEnable4 = ENABLE;
               }
            }
         }
      }

   }
   if(ParamModeLight == ENABLE)
   {   

      if(LightEnable == ENABLE)
      {  
         if(STRLight == 1)
         {
         STRLight = 0;
         ParamTimeLightHour = ParamTimeLightHour + time.RTC_Hours;
         ParamTimeLightMin = ParamTimeLightMin + time.RTC_Minutes;}
         GPIO_SetBits(GPIOD, GPIO_Pin_6);
         if((ParamTimeLightHour == time.RTC_Hours) && (ParamTimeLightMin == time.RTC_Minutes))
         {
            LightEnable = DISABLE;
         }

      }
      else
      { 
         GPIO_ResetBits(GPIOD, GPIO_Pin_6);
         STRLight = 1;

         if(WeekDayLight & 0x01)  //ПН
         {
            if(date.RTC_WeekDay == RTC_Weekday_Monday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  LightEnable = ENABLE;
               }
            }
         }
         if(WeekDayLight & 0x02)  //ВТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Tuesday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               LightEnable = ENABLE;
            }
            }
         }
         if(WeekDayLight & 0x04)  //СР
         {
            if(date.RTC_WeekDay == RTC_Weekday_Wednesday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               LightEnable = ENABLE;
            }
            }
         }
         if(WeekDayLight & 0x08)  //ЧТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Thursday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  LightEnable = ENABLE;
               } 
            }
         }
         if(WeekDayLight & 0x10)  //ПТ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Friday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  LightEnable = ENABLE;
               }
            }
         }
         if(WeekDayLight & 0x20)  //СБ
         {
            if(date.RTC_WeekDay == RTC_Weekday_Saturday)
         {
            if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
            {
               LightEnable = ENABLE;
            }
         }
         }
         if(WeekDayLight & 0x40)  //ВС
         {
            if(date.RTC_WeekDay == RTC_Weekday_Sunday)
            {
               if((ParamStartLightHour == time.RTC_Hours) && (ParamStartLightMin == time.RTC_Minutes) && (0x00 == time.RTC_Seconds))
               {
                  LightEnable = ENABLE;
               }
            }
         }
      }

   }
   /////////////////////////////////////////////////////////////////////////////
   RTC_GetDate(RTC_Format_BCD, &date);
   RTC_GetTime(RTC_Format_BCD, &time);

   switch (date.RTC_WeekDay)
   {
   case RTC_Weekday_Monday:
      StartAdress_PrintChar(0xC0, i003, sizeof(i003));
      break;
   case RTC_Weekday_Tuesday:
      StartAdress_PrintChar(0xC0, i004, sizeof(i004));
      break;
   case RTC_Weekday_Wednesday:
      StartAdress_PrintChar(0xC0, i005, sizeof(i005));
      break;
   case RTC_Weekday_Thursday:
      StartAdress_PrintChar(0xC0, i006, sizeof(i006));
      break;
   case RTC_Weekday_Friday:
      StartAdress_PrintChar(0xC0, i007, sizeof(i007));
      break;
   case RTC_Weekday_Saturday:
      StartAdress_PrintChar(0xC0, i008, sizeof(i008));
      break;
   case RTC_Weekday_Sunday:
      StartAdress_PrintChar(0xC0, i009, sizeof(i009));
      break;
   
   default:
      break;
   }
   PrintChoise(0xC3,time.RTC_Hours);
   PrintChar(0x3A);
   PrintChoise(0xC6,time.RTC_Minutes);

   if(UpDate)
   {
      UpDate = 0;
         if(strbTop&0x08)
            {
               strbTop &= (~0x08);
               clear();cursX = SaveTopMenu;
            }   
         switch(cursX)
         {
         case 1:
            if(strbTop&0x01)
            {
               strbTop &= (~0x01);
               clear();}
            SaveCursY = 0x11;  //позиция Начало подменю
            SaveTopMenu = cursX;
            strb |= 0x02;
            StartAdress_PrintChar(0x8f, s00, sizeof(s00));  
            StartAdress_PrintChar(0x80, s03, sizeof(s03));  
            StartAdress_PrintChar(0x85, i000, sizeof(i000));  //ПОЛИВ
            while(Escape == 1)
            {
                  if(strb&0x02)
                  {
                  strb &= (~0x02);
                  cursY = (SaveCursY & 0x0f);
                  //////////////////////////////////////
                  SaveCursY &= 0x0f;// позиция Начало подподменю   
                  SaveCursY |= 0x10;
                  //////////////////////////////////////
                  clear();
                  }

                  strbTop |= 0x01;  //Вкл clear(); в подменю выше
                  strbTop |= 0x08;
                  SaveCursY &= 0xf0;
                  SaveCursY |= cursY;

                  strb |= 0x01;

                  if(cursY==5){cursY = 4;}  //Ограничение курсора по
                  if(cursY==0){cursY = 1;}  //                шкале Y

                  switch (cursY)
                  {
                  case 1:
                      if(ParamMode&0x01){StartAdress_PrintChar(0x0e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x00e, s08, sizeof(s08));}  //крестик

                      if(ParamMode2&0x01){StartAdress_PrintChar(0x4e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x04e, s08, sizeof(s08));}  //крестик

                     StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                     StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i100, sizeof(i100));  //РЕЖИМ_1
                     StartAdress_PrintChar(0xC2, i200, sizeof(i200));  //РЕЖИМ_2
                     while(Escape == 2)
                     {
                           if(strb&0x01){
                              strb &= (~0x01);
                              cursY = ((SaveCursY >> 4) & 0x0f);   //Начало подменю
                              clear();}
                           SaveCursY &= 0x0f;
                           SaveCursY |= (cursY << 4); 
                           strb |= 0x02;  //Clean() для подменю выше

                           if(cursY == 6){cursY = 5;}
                           if(cursY == 0){cursY = 1;}

                           switch (cursY)
                           {
                           case 1:

                              if(strb&0x08){clear(); strb &= (~0x08);}
                              strb |= 0x04;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x12;}

                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x01; 

                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour);
                                 PrintChar(0x3a);
                                 PrintChoise(0x87, ParamMin);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0x0A, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamHour;cursYold = ParamHour;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamHour = s_cursY;  
                                 SetAdres(0x83);
                                 PrintChar(0x5B);       
                                 PrintChoise(0x84, ParamHour);
                                 PrintChar(0x5D);
                                 PrintChoise(0x87, ParamMin);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0x0A, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamMin;cursYold = ParamMin;}
                                 ParamMin = s_cursY;
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour);
                                 PrintChar(0x5B);
                                 PrintChoise(0x87, ParamMin);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0x0A, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                                 
                              }
                              break;
                           case 2:
                              if(strb&0x04){clear(); strb &= (~0x04);}
                              strb |= 0x10;
                              strb |= 0x08;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);s_cursY = 0;cst = 1;s_cursY = 0x00;}
                              strb |= 0x01;
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка      
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              
                              if(cursY != cursYStr)   //Настройка времени
                              {  
                                 cursYStr = cursY;
                                 if(ParseButton == UP){s_cursY++;}
                                 if(ParseButton == DOWN){s_cursY--;}
                              }
                              if(cursX != cursXStr)   //право влево работало
                              {  
                                 cursXStr = cursX;
                                 if(ParseButton == LEFT){cst = 1;}
                                 if(ParseButton == RIGHT){cst = 2;}
                              }
                                 if(cst == 3)
                                 {
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins);
                                 PrintChar(0x3a);
                                 PrintChoise(0xC7, ParamSecs);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;

                                 StartAdress_PrintChar(0xCA, i00D, sizeof(i00D)); //Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamMins;cursYold = ParamSecs;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamMins = s_cursY;  
                                 SetAdres(0xC3);
                                 PrintChar(0x5B);       
                                 PrintChoise(0xC4, ParamMins);
                                 PrintChar(0x5D);
                                 PrintChoise(0xC7, ParamSecs);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0xCA, i00B, sizeof(i00B)); //Мин
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamSecs;cursYold = ParamSecs;}
                                 ParamSecs = s_cursY;
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins);
                                 PrintChar(0x5B);
                                 PrintChoise(0xC7, ParamSecs);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0xCA, i00A, sizeof(i00A)); //Сек
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                              }
                              break;
                           case 3:

                              if(strb&0x10){clear(); strb &= (~0x10);}

                              WeekDay|=0x80;

                              strb |= 0x04;
                              strb |= 0x20;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                               while(Escape == 3)
                               {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = SVcst;cursX = 1;}
                                 if(cursY == 3){cursY = 2;}
                                 SVcst = cst;
                                 if(cst == 2){cst = 0;}  //костыль

                                 strb |= 0x01;

                                    if(WeekDay&0x01){StartAdress_PrintChar(0x01, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x01, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x02, i003, sizeof(i003));   //ПН

                                    if(WeekDay&0x02){StartAdress_PrintChar(0xC1, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC1, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC2, i004, sizeof(i004));   //ВТ

                                    if(WeekDay&0x04){StartAdress_PrintChar(0x05, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x05, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x06, i005, sizeof(i005));  //СР

                                    if(WeekDay&0x08){StartAdress_PrintChar(0xC5, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC5, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC6, i006, sizeof(i006));  //ЧТ

                                    if(WeekDay&0x10){StartAdress_PrintChar(0x09, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x09, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0A, i007, sizeof(i007));  //ПТ

                                    if(WeekDay&0x20){StartAdress_PrintChar(0xC9, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC9, s08, sizeof(s08));}                                    
                                    StartAdress_PrintChar(0xCA, i008, sizeof(i008));  //СТ

                                    if(WeekDay&0x40){StartAdress_PrintChar(0x0D, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x0D, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0E, i009, sizeof(i009));  //ВС


                                 if(cursY == 1)
                                 {
                                    WeekStrb |= 0x10;
                                    WeekStrb |= 0x20;
                                    WeekStrb |= 0x40;
                                    
                                    if(cursX == 1)  //ПН
                                    {
                                    if(WeekStrb&0x01){clear(); WeekStrb &= (~0x01); if(WeekDay&0x01){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;

                                    if(cst == 1){WeekDay|=0x01;}
                                    else{WeekDay &= (~0x01);}

                                    StartAdress_PrintChar(0x00, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 2) //СР
                                    {
                                    if(WeekStrb&0x02){clear(); WeekStrb &= (~0x02);if(WeekDay&0x04){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x01;

                                    if(cst == 1){WeekDay|=0x04;}
                                    else{WeekDay &= (~0x04);}

                                    StartAdress_PrintChar(0x04, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 3) //ПТ
                                    {
                                    if(WeekStrb&0x04){clear(); WeekStrb &= (~0x04);if(WeekDay&0x10){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x08;

                                    if(cst == 1){WeekDay|=0x10;}
                                    else{WeekDay &= (~0x10);}

                                    StartAdress_PrintChar(0x08, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s03, sizeof(s03));  //пусто
                                    }
                                    else    //ВС
                                    {
                                    if(WeekStrb&0x08){clear(); WeekStrb &= (~0x08);if(WeekDay&0x40){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;

                                    if(cst == 1){WeekDay|=0x40;}
                                    else{WeekDay &= (~0x40);}

                                    StartAdress_PrintChar(0x0C, s07, sizeof(s07));  //пусто   
                                    }

                                 }
                                 else
                                 {
                                    WeekStrb |= 0x01;
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x08;
                                    
                                    if(cursX == 1) //ВТ
                                    {
                                    if(WeekStrb&0x10){clear(); WeekStrb &= (~0x10);if(WeekDay&0x02){cst = 1;}else{cst = 0;}}   
                                    WeekStrb |= 0x20;  

                                    if(cst == 1){WeekDay|=0x02;}
                                    else{WeekDay &= (~0x02);}
                                    StartAdress_PrintChar(0x00, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 2) //ЧТ
                                    {
                                    if(WeekStrb&0x20){clear(); WeekStrb &= (~0x20);if(WeekDay&0x08){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x40;
                                    WeekStrb |= 0x10;

                                    if(cst == 1){WeekDay|=0x08;}
                                    else{WeekDay &= (~0x08);}

                                    StartAdress_PrintChar(0x04, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 3) //СТ
                                    {
                                    if(WeekStrb&0x40){clear(); WeekStrb &= (~0x40);if(WeekDay&0x20){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x20;

                                    if(cst == 1){WeekDay|=0x20;}
                                    else{WeekDay &= (~0x20);}

                                    StartAdress_PrintChar(0x08, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s07, sizeof(s07));  //пусто
                                    }
                                  }
                               }
                              break;
                           case 4:
                              if(strb&0x20){clear(); strb &= (~0x20);}
                              
                              strb |= 0x10;
                              strb |= 0x40;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                              while(Escape >= 3)
                              {
                                 if(strb&0x80){
                                    clear();
                                    strb &= (~0x80);
                                    cursY = NumPomp;
                                 }

                                 strb |= 0x01;
                                 if(Escape == 4){Escape = 2;}

                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ

                              NumPomp = cursY;

                              SetAdres(0xC4);
                              PrintChar(0x5B);   
                              PrintChoise(0xC5, NumPomp);  
                              PrintChar(0x5D);

                              }

                              break;
                           case 5:
                              if(strb&0x40){clear(); strb &= (~0x40);}

                              strb |= 0x20;

                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 

                              if(ParamMode&0x01){StartAdress_PrintChar(0x03, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                              else{StartAdress_PrintChar(0x03, i160, sizeof(i160));}

                              while(Escape >= 3)
                              {
                                 if(strb&0x20){
                                    strb &= (~0x20);
                                    if(ParamMode&0x01){cursY = 5;}
                                    else{cursY = 4;}
                                 }

                                 if(cursY == 6){cursY = 4;}
                                 if(cursY == 3){cursY = 5;}

                                 if(Escape == 4){Escape = 2;}

                                 strb |= 0x01;

                                 SetAdres(0x02);
                                 PrintChar(0x5B);
                                 if(cursY == 5)
                                 {
                                    ParamMode |= 0x01;
                                    StartAdress_PrintChar(0x03, i140, sizeof(i140));  //ВКЛ
                                 }
                                 else
                                 {
                                    ParamMode &= (~0x01);
                                    StartAdress_PrintChar(0x03, i160, sizeof(i160));  //ОТКЛ
                                 }
                                 PrintChar(0x5D);
                              }

                              break;
                           default:
                              break;
                           }
                     }
                     break;
                  case 2:
                     if(ParamMode&0x01){StartAdress_PrintChar(0x0e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x00e, s08, sizeof(s08));}  //крестик

                      if(ParamMode2&0x01){StartAdress_PrintChar(0x4e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x04e, s08, sizeof(s08));}  //крестик

                     StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i100, sizeof(i100));  //РЕЖИМ_1
                     StartAdress_PrintChar(0xC2, i200, sizeof(i200));  //РЕЖИМ_2
                     while(Escape == 2)
                     {
                           if(strb&0x01){
                              strb &= (~0x01);
                              cursY = ((SaveCursY >> 4) & 0x0f);   //Начало подменю
                              clear();}
                           SaveCursY &= 0x0f;
                           SaveCursY |= (cursY << 4); 
                           strb |= 0x02;  //Clean() для подменю выше

                           if(cursY == 6){cursY = 5;}
                           if(cursY == 0){cursY = 1;}

                           switch (cursY)
                           {
                           case 1:

                              if(strb&0x08){clear(); strb &= (~0x08);}
                              strb |= 0x04;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x12;}

                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x01; 

                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour2);
                                 PrintChar(0x3a);
                                 PrintChoise(0x87, ParamMin2);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0x0A, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamHour2;cursYold = ParamHour2;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamHour2 = s_cursY;  
                                 SetAdres(0x83);
                                 PrintChar(0x5B);       
                                 PrintChoise(0x84, ParamHour2);
                                 PrintChar(0x5D);
                                 PrintChoise(0x87, ParamMin2);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0x0A, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamMin2;cursYold = ParamMin2;}
                                 ParamMin2 = s_cursY;
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour2);
                                 PrintChar(0x5B);
                                 PrintChoise(0x87, ParamMin2);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0x0A, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                                 
                              }
                              break;
                           case 2:
                              if(strb&0x04){clear(); strb &= (~0x04);}
                              strb |= 0x10;
                              strb |= 0x08;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);s_cursY = 0;cst = 1;s_cursY = 0x00;}
                              strb |= 0x01;
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка      
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              
                              if(cursY != cursYStr)   //Настройка времени
                              {  
                                 cursYStr = cursY;
                                 if(ParseButton == UP){s_cursY++;}
                                 if(ParseButton == DOWN){s_cursY--;}
                              }
                              if(cursX != cursXStr)   //право влево работало
                              {  
                                 cursXStr = cursX;
                                 if(ParseButton == LEFT){cst = 1;}
                                 if(ParseButton == RIGHT){cst = 2;}
                              }
                                 if(cst == 3)
                                 {
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins2);
                                 PrintChar(0x3a);
                                 PrintChoise(0xC7, ParamSecs2);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;

                                 StartAdress_PrintChar(0xCA, i00D, sizeof(i00D)); //Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamMins2;cursYold = ParamSecs2;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamMins2 = s_cursY;  
                                 SetAdres(0xC3);
                                 PrintChar(0x5B);       
                                 PrintChoise(0xC4, ParamMins2);
                                 PrintChar(0x5D);
                                 PrintChoise(0xC7, ParamSecs2);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0xCA, i00B, sizeof(i00B)); //Мин
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamSecs2;cursYold = ParamSecs2;}
                                 ParamSecs2 = s_cursY;
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins2);
                                 PrintChar(0x5B);
                                 PrintChoise(0xC7, ParamSecs2);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0xCA, i00A, sizeof(i00A)); //Сек
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                              }
                              break;
                           case 3:

                              if(strb&0x10){clear(); strb &= (~0x10);}

                              WeekDay|=0x80;

                              strb |= 0x04;
                              strb |= 0x20;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                               while(Escape == 3)
                               {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = SVcst;cursX = 1;}
                                 if(cursY == 3){cursY = 2;}
                                 SVcst = cst;
                                 if(cst == 2){cst = 0;}  //костыль

                                 strb |= 0x01;

                                    if(WeekDay2&0x01){StartAdress_PrintChar(0x01, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x01, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x02, i003, sizeof(i003));   //ПН

                                    if(WeekDay2&0x02){StartAdress_PrintChar(0xC1, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC1, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC2, i004, sizeof(i004));   //ВТ

                                    if(WeekDay2&0x04){StartAdress_PrintChar(0x05, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x05, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x06, i005, sizeof(i005));  //СР

                                    if(WeekDay2&0x08){StartAdress_PrintChar(0xC5, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC5, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC6, i006, sizeof(i006));  //ЧТ

                                    if(WeekDay2&0x10){StartAdress_PrintChar(0x09, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x09, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0A, i007, sizeof(i007));  //ПТ

                                    if(WeekDay2&0x20){StartAdress_PrintChar(0xC9, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC9, s08, sizeof(s08));}                                    
                                    StartAdress_PrintChar(0xCA, i008, sizeof(i008));  //СТ

                                    if(WeekDay2&0x40){StartAdress_PrintChar(0x0D, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x0D, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0E, i009, sizeof(i009));  //ВС


                                 if(cursY == 1)
                                 {
                                    WeekStrb |= 0x10;
                                    WeekStrb |= 0x20;
                                    WeekStrb |= 0x40;
                                    
                                    if(cursX == 1)  //ПН
                                    {
                                    if(WeekStrb&0x01){clear(); WeekStrb &= (~0x01); if(WeekDay2&0x01){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;

                                    if(cst == 1){WeekDay2|=0x01;}
                                    else{WeekDay2 &= (~0x01);}

                                    StartAdress_PrintChar(0x00, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 2) //СР
                                    {
                                    if(WeekStrb&0x02){clear(); WeekStrb &= (~0x02);if(WeekDay2&0x04){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x01;

                                    if(cst == 1){WeekDay2|=0x04;}
                                    else{WeekDay2 &= (~0x04);}

                                    StartAdress_PrintChar(0x04, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 3) //ПТ
                                    {
                                    if(WeekStrb&0x04){clear(); WeekStrb &= (~0x04);if(WeekDay2&0x10){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x08;

                                    if(cst == 1){WeekDay2|=0x10;}
                                    else{WeekDay2 &= (~0x10);}

                                    StartAdress_PrintChar(0x08, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s03, sizeof(s03));  //пусто
                                    }
                                    else    //ВС
                                    {
                                    if(WeekStrb&0x08){clear(); WeekStrb &= (~0x08);if(WeekDay2&0x40){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;

                                    if(cst == 1){WeekDay2|=0x40;}
                                    else{WeekDay2 &= (~0x40);}

                                    StartAdress_PrintChar(0x0C, s07, sizeof(s07));  //пусто   
                                    }

                                 }
                                 else
                                 {
                                    WeekStrb |= 0x01;
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x08;
                                    
                                    if(cursX == 1) //ВТ
                                    {
                                    if(WeekStrb&0x10){clear(); WeekStrb &= (~0x10);if(WeekDay2&0x02){cst = 1;}else{cst = 0;}}   
                                    WeekStrb |= 0x20;  

                                    if(cst == 1){WeekDay2|=0x02;}
                                    else{WeekDay2 &= (~0x02);}
                                    StartAdress_PrintChar(0x00, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 2) //ЧТ
                                    {
                                    if(WeekStrb&0x20){clear(); WeekStrb &= (~0x20);if(WeekDay2&0x08){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x40;
                                    WeekStrb |= 0x10;

                                    if(cst == 1){WeekDay2|=0x08;}
                                    else{WeekDay2 &= (~0x08);}

                                    StartAdress_PrintChar(0x04, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 3) //СТ
                                    {
                                    if(WeekStrb&0x40){clear(); WeekStrb &= (~0x40);if(WeekDay2&0x20){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x20;

                                    if(cst == 1){WeekDay2|=0x20;}
                                    else{WeekDay2 &= (~0x20);}

                                    StartAdress_PrintChar(0x08, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s07, sizeof(s07));  //пусто
                                    }
                                  }
                               }
                              break;
                           case 4:
                              if(strb&0x20){clear(); strb &= (~0x20);}
                              
                              strb |= 0x10;
                              strb |= 0x40;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                              while(Escape >= 3)
                              {
                                 if(strb&0x80){
                                    clear();
                                    strb &= (~0x80);
                                    cursY = NumPomp2;
                                 }

                                 strb |= 0x01;
                                 if(Escape == 4){Escape = 2;}

                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ

                              NumPomp2 = cursY;

                              SetAdres(0xC4);
                              PrintChar(0x5B);   
                              PrintChoise(0xC5, NumPomp2);  
                              PrintChar(0x5D);

                              }

                              break;
                           case 5:
                              if(strb&0x40){clear(); strb &= (~0x40);}

                              strb |= 0x20;

                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 

                              if(ParamMode2&0x01){StartAdress_PrintChar(0x03, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                              else{StartAdress_PrintChar(0x03, i160, sizeof(i160));}

                              while(Escape >= 3)
                              {
                                 if(strb&0x20){
                                    strb &= (~0x20);
                                    if(ParamMode2&0x01){cursY = 5;}
                                    else{cursY = 4;}
                                 }

                                 if(cursY == 6){cursY = 4;}
                                 if(cursY == 3){cursY = 5;}

                                 if(Escape == 4){Escape = 2;}

                                 strb |= 0x01;

                                 SetAdres(0x02);
                                 PrintChar(0x5B);
                                 if(cursY == 5)
                                 {
                                    ParamMode2 |= 0x01;
                                    StartAdress_PrintChar(0x03, i140, sizeof(i140));  //ВКЛ
                                 }
                                 else
                                 {
                                    ParamMode2 &= (~0x01);
                                    StartAdress_PrintChar(0x03, i160, sizeof(i160));  //ОТКЛ
                                 }
                                 PrintChar(0x5D);
                              }

                              break;
                           default:
                              break;
                           }
                     }
                     break;
                  case 3:
                      if(ParamMode3&0x01){StartAdress_PrintChar(0x0e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x00e, s08, sizeof(s08));}  //крестик

                      if(ParamMode4&0x01){StartAdress_PrintChar(0x4e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x04e, s08, sizeof(s08));}  //крестик

                     StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i300, sizeof(i300));  //РЕЖИМ_3
                     StartAdress_PrintChar(0xC2, i400, sizeof(i400));  //РЕЖИМ_4
                     while(Escape == 2)
                     {
                        
                           if(strb&0x01){
                              strb &= (~0x01);
                              cursY = ((SaveCursY >> 4) & 0x0f);   //Начало подменю
                              clear();}
                           SaveCursY &= 0x0f;
                           SaveCursY |= (cursY << 4); 
                           strb |= 0x02;  //Clean() для подменю выше

                           if(cursY == 6){cursY = 5;}
                           if(cursY == 0){cursY = 1;}

                           switch (cursY)
                           {
                           case 1:

                              if(strb&0x08){clear(); strb &= (~0x08);}
                              strb |= 0x04;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x12;}

                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x01; 

                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour3);
                                 PrintChar(0x3a);
                                 PrintChoise(0x87, ParamMin3);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0x0A, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamHour3;cursYold = ParamHour3;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamHour3 = s_cursY;  
                                 SetAdres(0x83);
                                 PrintChar(0x5B);       
                                 PrintChoise(0x84, ParamHour3);
                                 PrintChar(0x5D);
                                 PrintChoise(0x87, ParamMin3);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0x0A, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamMin3;cursYold = ParamMin3;}
                                 ParamMin3 = s_cursY;
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour3);
                                 PrintChar(0x5B);
                                 PrintChoise(0x87, ParamMin3);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0x0A, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                                 
                              }
                              break;
                           case 2:
                              if(strb&0x04){clear(); strb &= (~0x04);}
                              strb |= 0x10;
                              strb |= 0x08;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);s_cursY = 0;cst = 1;s_cursY = 0x00;}
                              strb |= 0x01;
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка      
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              
                              if(cursY != cursYStr)   //Настройка времени
                              {  
                                 cursYStr = cursY;
                                 if(ParseButton == UP){s_cursY++;}
                                 if(ParseButton == DOWN){s_cursY--;}
                              }
                              if(cursX != cursXStr)   //право влево работало
                              {  
                                 cursXStr = cursX;
                                 if(ParseButton == LEFT){cst = 1;}
                                 if(ParseButton == RIGHT){cst = 2;}
                              }
                                 if(cst == 3)
                                 {
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins3);
                                 PrintChar(0x3a);
                                 PrintChoise(0xC7, ParamSecs3);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;

                                 StartAdress_PrintChar(0xCA, i00D, sizeof(i00D)); //Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamMins3;cursYold = ParamSecs3;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamMins3 = s_cursY;  
                                 SetAdres(0xC3);
                                 PrintChar(0x5B);       
                                 PrintChoise(0xC4, ParamMins3);
                                 PrintChar(0x5D);
                                 PrintChoise(0xC7, ParamSecs3);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0xCA, i00B, sizeof(i00B)); //Мин
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamSecs3;cursYold = ParamSecs3;}
                                 ParamSecs3 = s_cursY;
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins3);
                                 PrintChar(0x5B);
                                 PrintChoise(0xC7, ParamSecs3);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0xCA, i00A, sizeof(i00A)); //Сек
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                              }
                              break;
                           case 3:

                              if(strb&0x10){clear(); strb &= (~0x10);}

                              WeekDay|=0x80;

                              strb |= 0x04;
                              strb |= 0x20;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                               while(Escape == 3)
                               {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = SVcst;cursX = 1;}
                                 if(cursY == 3){cursY = 2;}
                                 SVcst = cst;
                                 if(cst == 2){cst = 0;}  //костыль

                                 strb |= 0x01;

                                    if(WeekDay3&0x01){StartAdress_PrintChar(0x01, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x01, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x02, i003, sizeof(i003));   //ПН

                                    if(WeekDay3&0x02){StartAdress_PrintChar(0xC1, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC1, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC2, i004, sizeof(i004));   //ВТ

                                    if(WeekDay3&0x04){StartAdress_PrintChar(0x05, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x05, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x06, i005, sizeof(i005));  //СР

                                    if(WeekDay3&0x08){StartAdress_PrintChar(0xC5, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC5, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC6, i006, sizeof(i006));  //ЧТ

                                    if(WeekDay3&0x10){StartAdress_PrintChar(0x09, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x09, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0A, i007, sizeof(i007));  //ПТ

                                    if(WeekDay3&0x20){StartAdress_PrintChar(0xC9, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC9, s08, sizeof(s08));}                                    
                                    StartAdress_PrintChar(0xCA, i008, sizeof(i008));  //СТ

                                    if(WeekDay3&0x40){StartAdress_PrintChar(0x0D, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x0D, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0E, i009, sizeof(i009));  //ВС


                                 if(cursY == 1)
                                 {
                                    WeekStrb |= 0x10;
                                    WeekStrb |= 0x20;
                                    WeekStrb |= 0x40;
                                    
                                    if(cursX == 1)  //ПН
                                    {
                                    if(WeekStrb&0x01){clear(); WeekStrb &= (~0x01); if(WeekDay3&0x01){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;

                                    if(cst == 1){WeekDay3|=0x01;}
                                    else{WeekDay3 &= (~0x01);}

                                    StartAdress_PrintChar(0x00, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 2) //СР
                                    {
                                    if(WeekStrb&0x02){clear(); WeekStrb &= (~0x02);if(WeekDay3&0x04){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x01;

                                    if(cst == 1){WeekDay3|=0x04;}
                                    else{WeekDay3 &= (~0x04);}

                                    StartAdress_PrintChar(0x04, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 3) //ПТ
                                    {
                                    if(WeekStrb&0x04){clear(); WeekStrb &= (~0x04);if(WeekDay3&0x10){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x08;

                                    if(cst == 1){WeekDay3|=0x10;}
                                    else{WeekDay3 &= (~0x10);}

                                    StartAdress_PrintChar(0x08, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s03, sizeof(s03));  //пусто
                                    }
                                    else    //ВС
                                    {
                                    if(WeekStrb&0x08){clear(); WeekStrb &= (~0x08);if(WeekDay3&0x40){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;

                                    if(cst == 1){WeekDay3|=0x40;}
                                    else{WeekDay3 &= (~0x40);}

                                    StartAdress_PrintChar(0x0C, s07, sizeof(s07));  //пусто   
                                    }

                                 }
                                 else
                                 {
                                    WeekStrb |= 0x01;
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x08;
                                    
                                    if(cursX == 1) //ВТ
                                    {
                                    if(WeekStrb&0x10){clear(); WeekStrb &= (~0x10);if(WeekDay3&0x02){cst = 1;}else{cst = 0;}}   
                                    WeekStrb |= 0x20;  

                                    if(cst == 1){WeekDay3|=0x02;}
                                    else{WeekDay3 &= (~0x02);}
                                    StartAdress_PrintChar(0x00, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 2) //ЧТ
                                    {
                                    if(WeekStrb&0x20){clear(); WeekStrb &= (~0x20);if(WeekDay3&0x08){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x40;
                                    WeekStrb |= 0x10;

                                    if(cst == 1){WeekDay3|=0x08;}
                                    else{WeekDay3 &= (~0x08);}

                                    StartAdress_PrintChar(0x04, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 3) //СТ
                                    {
                                    if(WeekStrb&0x40){clear(); WeekStrb &= (~0x40);if(WeekDay3&0x20){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x20;

                                    if(cst == 1){WeekDay3|=0x20;}
                                    else{WeekDay3 &= (~0x20);}

                                    StartAdress_PrintChar(0x08, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s07, sizeof(s07));  //пусто
                                    }
                                  }
                               }
                              break;
                           case 4:
                              if(strb&0x20){clear(); strb &= (~0x20);}
                              
                              strb |= 0x10;
                              strb |= 0x40;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                              while(Escape >= 3)
                              {
                                 if(strb&0x80){
                                    clear();
                                    strb &= (~0x80);
                                    cursY = NumPomp3;
                                 }

                                 strb |= 0x01;
                                 if(Escape == 4){Escape = 2;}

                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ

                              NumPomp3 = cursY;

                              SetAdres(0xC4);
                              PrintChar(0x5B);   
                              PrintChoise(0xC5, NumPomp3);  
                              PrintChar(0x5D);

                              }

                              break;
                           case 5:
                              if(strb&0x40){clear(); strb &= (~0x40);}

                              strb |= 0x20;

                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 

                              if(ParamMode3&0x01){StartAdress_PrintChar(0x03, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                              else{StartAdress_PrintChar(0x03, i160, sizeof(i160));}

                              while(Escape >= 3)
                              {
                                 if(strb&0x20){
                                    strb &= (~0x20);
                                    if(ParamMode3&0x01){cursY = 5;}
                                    else{cursY = 4;}
                                 }

                                 if(cursY == 6){cursY = 4;}
                                 if(cursY == 3){cursY = 5;}

                                 if(Escape == 4){Escape = 2;}

                                 strb |= 0x01;

                                 SetAdres(0x02);
                                 PrintChar(0x5B);
                                 if(cursY == 5)
                                 {
                                    ParamMode3 |= 0x01;
                                    StartAdress_PrintChar(0x03, i140, sizeof(i140));  //ВКЛ
                                 }
                                 else
                                 {
                                    ParamMode3 &= (~0x01);
                                    StartAdress_PrintChar(0x03, i160, sizeof(i160));  //ОТКЛ
                                 }
                                 PrintChar(0x5D);
                              }

                              break;
                           default:
                              break;
                           }
                     }
                     break;
                  case 4:
                      if(ParamMode3&0x01){StartAdress_PrintChar(0x0e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x00e, s08, sizeof(s08));}  //крестик

                      if(ParamMode4&0x01){StartAdress_PrintChar(0x4e, s09, sizeof(s09));}  //галочка
                      else{StartAdress_PrintChar(0x04e, s08, sizeof(s08));}  //крестик
                     StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 
                     StartAdress_PrintChar(0x02, i300, sizeof(i300));  //РЕЖИМ_3
                     StartAdress_PrintChar(0xC2, i400, sizeof(i400));  //РЕЖИМ_4
                     while(Escape == 2)
                     {
                           if(strb&0x01){
                              strb &= (~0x01);
                              cursY = ((SaveCursY >> 4) & 0x0f);   //Начало подменю
                              clear();}
                           SaveCursY &= 0x0f;
                           SaveCursY |= (cursY << 4); 
                           strb |= 0x02;  //Clean() для подменю выше

                           if(cursY == 6){cursY = 5;}
                           if(cursY == 0){cursY = 1;}

                           switch (cursY)
                           {
                           case 1:

                              if(strb&0x08){clear(); strb &= (~0x08);}
                              strb |= 0x04;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x12;}

                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x01; 

                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour4);
                                 PrintChar(0x3a);
                                 PrintChoise(0x87, ParamMin4);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0x0A, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamHour4;cursYold = ParamHour4;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamHour4 = s_cursY;  
                                 SetAdres(0x83);
                                 PrintChar(0x5B);       
                                 PrintChoise(0x84, ParamHour4);
                                 PrintChar(0x5D);
                                 PrintChoise(0x87, ParamMin4);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0x0A, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamMin4;cursYold = ParamMin4;}
                                 ParamMin4 = s_cursY;
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamHour4);
                                 PrintChar(0x5B);
                                 PrintChoise(0x87, ParamMin4);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0x0A, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                                 
                              }
                              break;
                           case 2:
                              if(strb&0x04){clear(); strb &= (~0x04);}
                              strb |= 0x10;
                              strb |= 0x08;
                              strb |= 0x80;
                              WeekDay|=0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              StartAdress_PrintChar(0xC2, i120, sizeof(i120));    //ВРЕМ.ПОЛИВА
                              while(Escape == 3)
                              {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);s_cursY = 0;cst = 1;s_cursY = 0x00;}
                              strb |= 0x01;
                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка      
                              StartAdress_PrintChar(0x02, i110, sizeof(i110));    //НАЧ.ПОЛИВА
                              
                              if(cursY != cursYStr)   //Настройка времени
                              {  
                                 cursYStr = cursY;
                                 if(ParseButton == UP){s_cursY++;}
                                 if(ParseButton == DOWN){s_cursY--;}
                              }
                              if(cursX != cursXStr)   //право влево работало
                              {  
                                 cursXStr = cursX;
                                 if(ParseButton == LEFT){cst = 1;}
                                 if(ParseButton == RIGHT){cst = 2;}
                              }
                                 if(cst == 3)
                                 {
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins4);
                                 PrintChar(0x3a);
                                 PrintChoise(0xC7, ParamSecs4);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;

                                 StartAdress_PrintChar(0xCA, i00D, sizeof(i00D)); //Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamMins4;cursYold = ParamSecs4;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamMins4 = s_cursY;  
                                 SetAdres(0xC3);
                                 PrintChar(0x5B);       
                                 PrintChoise(0xC4, ParamMins4);
                                 PrintChar(0x5D);
                                 PrintChoise(0xC7, ParamSecs4);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0xCA, i00B, sizeof(i00B)); //Мин
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}

                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamSecs4;cursYold = ParamSecs4;}
                                 ParamSecs4 = s_cursY;
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamMins4);
                                 PrintChar(0x5B);
                                 PrintChoise(0xC7, ParamSecs4);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0xCA, i00A, sizeof(i00A)); //Сек
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 2;}

                              }
                              break;
                           case 3:

                              if(strb&0x10){clear(); strb &= (~0x10);}

                              WeekDay|=0x80;

                              strb |= 0x04;
                              strb |= 0x20;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                               while(Escape == 3)
                               {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = SVcst;cursX = 1;}
                                 if(cursY == 3){cursY = 2;}
                                 SVcst = cst;
                                 if(cst == 2){cst = 0;}  //костыль

                                 strb |= 0x01;

                                    if(WeekDay4&0x01){StartAdress_PrintChar(0x01, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x01, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x02, i003, sizeof(i003));   //ПН

                                    if(WeekDay4&0x02){StartAdress_PrintChar(0xC1, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC1, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC2, i004, sizeof(i004));   //ВТ

                                    if(WeekDay4&0x04){StartAdress_PrintChar(0x05, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x05, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x06, i005, sizeof(i005));  //СР

                                    if(WeekDay4&0x08){StartAdress_PrintChar(0xC5, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC5, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC6, i006, sizeof(i006));  //ЧТ

                                    if(WeekDay4&0x10){StartAdress_PrintChar(0x09, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x09, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0A, i007, sizeof(i007));  //ПТ

                                    if(WeekDay4&0x20){StartAdress_PrintChar(0xC9, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC9, s08, sizeof(s08));}                                    
                                    StartAdress_PrintChar(0xCA, i008, sizeof(i008));  //СТ

                                    if(WeekDay4&0x40){StartAdress_PrintChar(0x0D, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x0D, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0E, i009, sizeof(i009));  //ВС


                                 if(cursY == 1)
                                 {
                                    WeekStrb |= 0x10;
                                    WeekStrb |= 0x20;
                                    WeekStrb |= 0x40;
                                    
                                    if(cursX == 1)  //ПН
                                    {
                                    if(WeekStrb&0x01){clear(); WeekStrb &= (~0x01); if(WeekDay4&0x01){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;

                                    if(cst == 1){WeekDay4|=0x01;}
                                    else{WeekDay4 &= (~0x01);}

                                    StartAdress_PrintChar(0x00, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 2) //СР
                                    {
                                    if(WeekStrb&0x02){clear(); WeekStrb &= (~0x02);if(WeekDay4&0x04){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x01;

                                    if(cst == 1){WeekDay4|=0x04;}
                                    else{WeekDay4 &= (~0x04);}

                                    StartAdress_PrintChar(0x04, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 3) //ПТ
                                    {
                                    if(WeekStrb&0x04){clear(); WeekStrb &= (~0x04);if(WeekDay4&0x10){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x08;

                                    if(cst == 1){WeekDay4|=0x10;}
                                    else{WeekDay4 &= (~0x10);}

                                    StartAdress_PrintChar(0x08, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s03, sizeof(s03));  //пусто
                                    }
                                    else    //ВС
                                    {
                                    if(WeekStrb&0x08){clear(); WeekStrb &= (~0x08);if(WeekDay4&0x40){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;

                                    if(cst == 1){WeekDay4|=0x40;}
                                    else{WeekDay4 &= (~0x40);}

                                    StartAdress_PrintChar(0x0C, s07, sizeof(s07));  //пусто   
                                    }

                                 }
                                 else
                                 {
                                    WeekStrb |= 0x01;
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x08;
                                    
                                    if(cursX == 1) //ВТ
                                    {
                                    if(WeekStrb&0x10){clear(); WeekStrb &= (~0x10);if(WeekDay4&0x02){cst = 1;}else{cst = 0;}}   
                                    WeekStrb |= 0x20;  

                                    if(cst == 1){WeekDay4|=0x02;}
                                    else{WeekDay4 &= (~0x02);}
                                    StartAdress_PrintChar(0x00, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 2) //ЧТ
                                    {
                                    if(WeekStrb&0x20){clear(); WeekStrb &= (~0x20);if(WeekDay4&0x08){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x40;
                                    WeekStrb |= 0x10;

                                    if(cst == 1){WeekDay4|=0x08;}
                                    else{WeekDay4 &= (~0x08);}

                                    StartAdress_PrintChar(0x04, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 3) //СТ
                                    {
                                    if(WeekStrb&0x40){clear(); WeekStrb &= (~0x40);if(WeekDay4&0x20){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x20;

                                    if(cst == 1){WeekDay4|=0x20;}
                                    else{WeekDay4 &= (~0x20);}

                                    StartAdress_PrintChar(0x08, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s07, sizeof(s07));  //пусто
                                    }
                                  }
                               }
                              break;
                           case 4:
                              if(strb&0x20){clear(); strb &= (~0x20);}
                              
                              strb |= 0x10;
                              strb |= 0x40;
                              strb |= 0x80;
                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ
                              StartAdress_PrintChar(0xC2, i130, sizeof(i130));  //НОМЕР НАСОСА
                              while(Escape >= 3)
                              {
                                 if(strb&0x80){
                                    clear();
                                    strb &= (~0x80);
                                    cursY = NumPomp4;
                                 }

                                 strb |= 0x01;
                                 if(Escape == 4){Escape = 2;}

                              StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                              StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                              StartAdress_PrintChar(0x02, i150, sizeof(i150));  //ДНИ НЕДЕЛИ

                              NumPomp4 = cursY;

                              SetAdres(0xC4);
                              PrintChar(0x5B);   
                              PrintChoise(0xC5, NumPomp4);  
                              PrintChar(0x5D);

                              }

                              break;
                           case 5:
                              if(strb&0x40){clear(); strb &= (~0x40);}

                              strb |= 0x20;

                              StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                              StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто

                              StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                              StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 

                              if(ParamMode4&0x01){StartAdress_PrintChar(0x03, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                              else{StartAdress_PrintChar(0x03, i160, sizeof(i160));}

                              while(Escape >= 3)
                              {
                                 if(strb&0x20){
                                    strb &= (~0x20);
                                    if(ParamMode4&0x01){cursY = 5;}
                                    else{cursY = 4;}
                                 }

                                 if(cursY == 6){cursY = 4;}
                                 if(cursY == 3){cursY = 5;}

                                 if(Escape == 4){Escape = 2;}

                                 strb |= 0x01;

                                 SetAdres(0x02);
                                 PrintChar(0x5B);
                                 if(cursY == 5)
                                 {
                                    ParamMode4 |= 0x01;
                                    StartAdress_PrintChar(0x03, i140, sizeof(i140));  //ВКЛ
                                 }
                                 else
                                 {
                                    ParamMode4 &= (~0x01);
                                    StartAdress_PrintChar(0x03, i160, sizeof(i160));  //ОТКЛ
                                 }
                                 PrintChar(0x5D);
                              }

                              break;
                           default:
                              break;
                           }

                     }
                     break;
                     default:
                     break;
                  }   
            }
            break;
         case 2:
            if(strbTop&0x02){
               strbTop &= (~0x02);
               clear();}
            SaveCursY = 0x11;  //позиция Начало подменю
            SaveTopMenu = cursX;
            strb |= 0x02;   
            StartAdress_PrintChar(0x8f, s00, sizeof(s00));  
            StartAdress_PrintChar(0x80, s01, sizeof(s01)); 
            StartAdress_PrintChar(0x85, i001, sizeof(i001)); //СВЕТ
            while(Escape == 1)
            {
                  if(strb&0x02)
                  {
                  strb &= (~0x02);
                  cursY = (SaveCursY & 0x0f);
                  //////////////////////////////////////
                  SaveCursY &= 0x0f;// позиция Начало подподменю   
                  SaveCursY |= 0x10;
                  //////////////////////////////////////
                  clear();
                  }

                  strbTop |= 0x02;  //Вкл clear(); в подменю выше
                  strbTop |= 0x08;
                  SaveCursY &= 0xf0;
                  SaveCursY |= cursY;

                  switch (cursY)
                  {
                  case 1:

                     if(strb&0x08){clear(); strb &= (~0x08);}

                     strb |= 0x10;
                     strb |= 0x80;
                     WeekDay|=0x80;
                     StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0x80, s03, sizeof(s03));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i101, sizeof(i101));  //Начало свет
                     StartAdress_PrintChar(0xC2, i201, sizeof(i201));  //Время свет
                     while(Escape == 2)
                     {
                                 if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x12;cursY = ((SaveCursY >> 4) & 0x0f);}
                                 SaveCursY &= 0x0f;
                                 SaveCursY |= (cursY << 4);
                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0xC2, i201, sizeof(i201));    
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x08; 
                                 strb |= 0x02;
                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamStartLightHour);
                                 PrintChar(0x3a);
                                 PrintChoise(0x87, ParamStartLightMin);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0x0A, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamStartLightHour;cursYold = ParamStartLightHour;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamStartLightHour = s_cursY;  
                                 SetAdres(0x83);
                                 PrintChar(0x5B);       
                                 PrintChoise(0x84, ParamStartLightHour);
                                 PrintChar(0x5D);
                                 PrintChoise(0x87, ParamStartLightMin);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0x0A, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}
                                 strb2 |= 0x01;
                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamStartLightMin;cursYold = ParamStartLightMin;}
                                 ParamStartLightMin = s_cursY;
                                 SetAdres(0x83);
                                 PrintChar(0xfe);
                                 PrintChoise(0x84, ParamStartLightHour);
                                 PrintChar(0x5B);
                                 PrintChoise(0x87, ParamStartLightMin);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0x0A, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 1;}
                     }                
                  break;
                  case 2:
                     if(strb&0x10){clear(); strb &= (~0x10);}

                     strb |= 0x20;
                     strb |= 0x08;
                     strb |= 0x80;
                     WeekDay|=0x80;
                     StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i101, sizeof(i101));  //Начало свет
                     StartAdress_PrintChar(0xC2, i201, sizeof(i201));  //Время свет
                     while(Escape == 2)
                     {
                                if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = 1;s_cursY = 0x00;cursY = ((SaveCursY >> 4) & 0x0f);}
                                 SaveCursY &= 0x0f;
                                 SaveCursY |= (cursY << 4);
                                 StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                                 StartAdress_PrintChar(0x02, i101, sizeof(i101));    
                                 StartAdress_PrintChar(0xC0, s06, sizeof(s06));
                                 strb |= 0x08; 
                                 strb |= 0x02;
                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst = 1;}
                                    if(ParseButton == RIGHT){cst = 2;}
                                 }
                                 if(cst == 3)
                                 {
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamTimeLightHour);
                                 PrintChar(0x3a);
                                 PrintChoise(0xC7, ParamTimeLightMin);
                                 PrintChar(0xfe);
                                 strb2 |= 0x01;
                                 StartAdress_PrintChar(0xCA, i00D, sizeof(i00D)); // Пусто
                                 }
                                 else if(cst == 1)
                                 {
                                 if(strb2&0x01){strb2&=(~0x01);s_cursY = ParamTimeLightHour;cursYold = ParamTimeLightHour;}
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x24){ s_cursY = 0x00;}
                                 if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                 if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                 ParamTimeLightHour = s_cursY;  
                                 SetAdres(0xC3);
                                 PrintChar(0x5B);       
                                 PrintChoise(0xC4, ParamTimeLightHour);
                                 PrintChar(0x5D);
                                 PrintChoise(0xC7, ParamTimeLightMin);
                                 PrintChar(0xfe);
  
                                 strb2 |= 0x02;

                                 StartAdress_PrintChar(0xCA, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 2)
                                 {
                                 if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                 if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                 if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                 if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                 if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                 if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                 if(s_cursY == 0xff){ s_cursY = 0x59;}
                                 if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                 if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                 if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                 if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                 if(s_cursY == 0x0f){ s_cursY = 0x09;}
                                 strb2 |= 0x01;
                                 if(strb2&0x02){strb2&=(~0x02);s_cursY = ParamTimeLightMin;cursYold = ParamTimeLightMin;}
                                 ParamTimeLightMin = s_cursY;
                                 SetAdres(0xC3);
                                 PrintChar(0xfe);
                                 PrintChoise(0xC4, ParamTimeLightHour);
                                 PrintChar(0x5B);
                                 PrintChoise(0xC7, ParamTimeLightMin);
                                 PrintChar(0x5D);
                                 StartAdress_PrintChar(0xCA, i00B, sizeof(i00B)); //Мин
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 4){cst = 1;Escape = 1;} 
                     }   
                  break;
                  case 3:
                     if(strb&0x20){clear(); strb &= (~0x20);}
                     strb |= 0x40;
                     strb |= 0x10;
                     strb |= 0x80;
                     WeekDay|=0x80;
                     StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0xC1, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s06, sizeof(s06)); 
                     StartAdress_PrintChar(0x02, i150, sizeof(i150));  //Дни недели
                     if(ParamModeLight&0x01){StartAdress_PrintChar(0xC2, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                     else{StartAdress_PrintChar(0xC2, i160, sizeof(i160));}
                     while(Escape == 2)
                     {
                                  if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    clear();WeekDay&=(~0x80);cst = SVcst;cursX = 1;}

                                 SaveCursY &= 0x0f;
                                 SaveCursY |= (cursY << 4);
                                 strb |= 0x02;
                                 if(cursY == 3){cursY = 2;}
                                 SVcst = cst;
                                 if(cst == 2){cst = 0;}  //костыль

                                 strb |= 0x01;

                                    if(WeekDayLight&0x01){StartAdress_PrintChar(0x01, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x01, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x02, i003, sizeof(i003));   //ПН

                                    if(WeekDayLight&0x02){StartAdress_PrintChar(0xC1, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC1, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC2, i004, sizeof(i004));   //ВТ

                                    if(WeekDayLight&0x04){StartAdress_PrintChar(0x05, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x05, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x06, i005, sizeof(i005));  //СР

                                    if(WeekDayLight&0x08){StartAdress_PrintChar(0xC5, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC5, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0xC6, i006, sizeof(i006));  //ЧТ

                                    if(WeekDayLight&0x10){StartAdress_PrintChar(0x09, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x09, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0A, i007, sizeof(i007));  //ПТ

                                    if(WeekDayLight&0x20){StartAdress_PrintChar(0xC9, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0xC9, s08, sizeof(s08));}                                    
                                    StartAdress_PrintChar(0xCA, i008, sizeof(i008));  //СТ

                                    if(WeekDayLight&0x40){StartAdress_PrintChar(0x0D, s09, sizeof(s09));} //галочка
                                    else{StartAdress_PrintChar(0x0D, s08, sizeof(s08));}
                                    StartAdress_PrintChar(0x0E, i009, sizeof(i009));  //ВС


                                 if(cursY == 1)
                                 {
                                    WeekStrb |= 0x10;
                                    WeekStrb |= 0x20;
                                    WeekStrb |= 0x40;
                                    
                                    if(cursX == 1)  //ПН
                                    {
                                    if(WeekStrb&0x01){clear(); WeekStrb &= (~0x01); if(WeekDayLight&0x01){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;

                                    if(cst == 1){WeekDayLight|=0x01;}
                                    else{WeekDayLight &= (~0x01);}

                                    StartAdress_PrintChar(0x00, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 2) //СР
                                    {
                                    if(WeekStrb&0x02){clear(); WeekStrb &= (~0x02);if(WeekDayLight&0x04){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x01;

                                    if(cst == 1){WeekDayLight|=0x04;}
                                    else{WeekDayLight &= (~0x04);}

                                    StartAdress_PrintChar(0x04, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s03, sizeof(s03));  //пусто
                                    }
                                    else if(cursX == 3) //ПТ
                                    {
                                    if(WeekStrb&0x04){clear(); WeekStrb &= (~0x04);if(WeekDayLight&0x10){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x08;

                                    if(cst == 1){WeekDayLight|=0x10;}
                                    else{WeekDayLight &= (~0x10);}

                                    StartAdress_PrintChar(0x08, s07, sizeof(s07));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s03, sizeof(s03));  //пусто
                                    }
                                    else    //ВС
                                    {
                                    if(WeekStrb&0x08){clear(); WeekStrb &= (~0x08);if(WeekDayLight&0x40){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x04;

                                    if(cst == 1){WeekDayLight|=0x40;}
                                    else{WeekDayLight &= (~0x40);}

                                    StartAdress_PrintChar(0x0C, s07, sizeof(s07));  //пусто   
                                    }

                                 }
                                 else
                                 {
                                    WeekStrb |= 0x01;
                                    WeekStrb |= 0x02;
                                    WeekStrb |= 0x04;
                                    WeekStrb |= 0x08;
                                    
                                    if(cursX == 1) //ВТ
                                    {
                                    if(WeekStrb&0x10){clear(); WeekStrb &= (~0x10);if(WeekDayLight&0x02){cst = 1;}else{cst = 0;}}   
                                    WeekStrb |= 0x20;  

                                    if(cst == 1){WeekDayLight|=0x02;}
                                    else{WeekDayLight &= (~0x02);}
                                    StartAdress_PrintChar(0x00, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC0, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 2) //ЧТ
                                    {
                                    if(WeekStrb&0x20){clear(); WeekStrb &= (~0x20);if(WeekDayLight&0x08){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x40;
                                    WeekStrb |= 0x10;

                                    if(cst == 1){WeekDayLight|=0x08;}
                                    else{WeekDayLight &= (~0x08);}

                                    StartAdress_PrintChar(0x04, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC4, s07, sizeof(s07));  //пусто
                                    }
                                    else if(cursX == 3) //СТ
                                    {
                                    if(WeekStrb&0x40){clear(); WeekStrb &= (~0x40);if(WeekDayLight&0x20){cst = 1;}else{cst = 0;}}
                                    WeekStrb |= 0x20;

                                    if(cst == 1){WeekDayLight|=0x20;}
                                    else{WeekDayLight &= (~0x20);}

                                    StartAdress_PrintChar(0x08, s03, sizeof(s03));  //стрелка 
                                    StartAdress_PrintChar(0xC8, s07, sizeof(s07));  //пусто
                                    }
                                  }                        
                     }   
                  break;
                  case 4:
                     if(strb&0x40){clear(); strb &= (~0x40);}
                     strb |= 0x20;
                     WeekDay|=0x80;
                     StartAdress_PrintChar(0x81, s03, sizeof(s03));  //пусто
                     StartAdress_PrintChar(0xC1, s07, sizeof(s07));  //стрелка
                     StartAdress_PrintChar(0x80, s05, sizeof(s05));  
                     StartAdress_PrintChar(0xC0, s03, sizeof(s03)); 
                     StartAdress_PrintChar(0x02, i150, sizeof(i150));  //Дни недели
                     
                     if(ParamModeLight&0x01){StartAdress_PrintChar(0xC2, i140, sizeof(i140));}  //ВКЛ/ОТКЛ
                     else{StartAdress_PrintChar(0xC2, i160, sizeof(i160));}

                     while(Escape == 2)
                     {
                        if(strb&0x20){
                           strb &= (~0x20);
                           WeekDay&=(~0x80);
                           cst = 0;
                           SaveCursY &= 0x0f;
                           SaveCursY |= (cursY << 4);
                           if(ParamModeLight&0x01){cursY = 5;}
                           else{cursY = 4;}
                        }

                        if(cursY == 6){cursY = 4;}
                        if(cursY == 3){cursY = 5;}

                        if(cst == 1){Escape = 1;cst = 0;}
                        strb |= 0x02;
                        strb |= 0x40;

                        SetAdres(0xC2);
                        PrintChar(0x5B);
                        if(cursY == 5)
                        {
                           ParamModeLight |= 0x01;
                           StartAdress_PrintChar(0xC3, i140, sizeof(i140));  //ВКЛ
                        }
                        else
                        {
                           ParamModeLight &= (~0x01);
                           StartAdress_PrintChar(0xC3, i160, sizeof(i160));  //ОТКЛ
                        }
                        PrintChar(0x5D);
                     }   
                  break;

                  default:
                     break;
                  }

            }
            break; 
         case 3:
            if(strbTop&0x04){
            strbTop &= (~0x04);
            clear();}
            SaveCursY = 0x11;  //позиция Начало подменю
            SaveTopMenu = cursX;
            strb |= 0x02;
            StartAdress_PrintChar(0x8f, s03, sizeof(s03));  
            StartAdress_PrintChar(0x80, s01, sizeof(s01)); 
            StartAdress_PrintChar(0x85, i002, sizeof(i002)); //НАСТР
            while(Escape == 1)
            {
               if(strb&0x02)
                  {
                  strb &= (~0x02);
                  cursY = (SaveCursY & 0x0f);
                  //////////////////////////////////////
                  SaveCursY &= 0x0f;// позиция Начало подподменю   
                  SaveCursY |= 0x10;
                  //////////////////////////////////////
                  clear();
                  }
               
               SaveCursY &= 0xf0;
               SaveCursY |= cursY;
               strb |= 0x80;
               WeekDay|=0x80;
               strbTop |= 0x04;  //Вкл clear(); в подменю выше
               strbTop |= 0x08;
               StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
               StartAdress_PrintChar(0x02, i102, sizeof(i102));  //Настройка
               while (Escape == 2)
               {
                               if(strb&0x80){
                                    strb &= (~0x80);
                                    cursY = 1;
                                    strb2|=0x01;
                                    clear();WeekDay&=(~0x80);cst = 2;s_cursY = 0x0;cursY = ((SaveCursY >> 4) & 0x0f);
                                    RTC_GetTime(RTC_Format_BCD, &time);RTC_GetDate(RTC_Format_BCD, &date);}
                                 SaveCursY &= 0x0f;
                                 SaveCursY |= (cursY << 4);
                                 StartAdress_PrintChar(0x81, s07, sizeof(s07));  //стрелка
   
                                 strb |= 0x02;
                                 if(cursY != cursYStr)   //Настройка времени
                                 {  
                                    cursYStr = cursY;
                                    if(ParseButton == UP){s_cursY++;}
                                    if(ParseButton == DOWN){s_cursY--;}
                                 }
                                 if(cursX != cursXStr)   //право влево работало
                                 {  
                                    cursXStr = cursX;
                                    if(ParseButton == LEFT){cst--;}
                                    if(ParseButton == RIGHT){cst++;}

                                    if(cst == 1){cst = 2;}
                                    if(cst == 5){cst = 4;}
                                 }

                                 if(cst == 0)
                                 {
                                    strb2|=0x01;
                                    RTC_GetDate(RTC_Format_BCD, &date);
                                    RTC_GetTime(RTC_Format_BCD, &time);
                                    SetAdres(0x02);
                                    PrintChar(0xfe);
                                    switch (date.RTC_WeekDay)
                                    {
                                    case RTC_Weekday_Monday:
                                       StartAdress_PrintChar(0x03, i003, sizeof(i003)); // ПН
                                       break;
                                    case RTC_Weekday_Tuesday:
                                       StartAdress_PrintChar(0x03, i004, sizeof(i004)); // ВТ
                                       break;
                                    case RTC_Weekday_Wednesday:
                                       StartAdress_PrintChar(0x03, i005, sizeof(i005)); // СР
                                       break;
                                    case RTC_Weekday_Thursday:
                                       StartAdress_PrintChar(0x03, i006, sizeof(i006)); // ЧТ
                                       break;
                                    case RTC_Weekday_Friday:
                                       StartAdress_PrintChar(0x03, i007, sizeof(i007)); // ПТ                                      
                                       break;
                                    case RTC_Weekday_Saturday:
                                       StartAdress_PrintChar(0x03, i008, sizeof(i008)); // СБ                                      
                                       break;
                                    case RTC_Weekday_Sunday:
                                       StartAdress_PrintChar(0x03, i008, sizeof(i008)); // ВС                                      
                                       break;

                                    default:
                                       break;
                                    } 
                                    PrintChar(0xfe);
                                    PrintChoise(0x07,time.RTC_Hours);
                                    PrintChar(0x3A);
                                    PrintChoise(0x0A,time.RTC_Minutes);
                                    PrintChar(0xfe);
                                    PrintChar(0xfe);
                                    PrintChar(0xfe);
                                    PrintChar(0xfe);
                                 }
                                 else if(cst == 1)
                                 {
                                    Escape = 1;
                                 }
                                 else if(cst == 2)
                                 {
                                    if(strb2&0x01){clear();strb2&=(~0x01);RTC_GetDate(RTC_Format_BCD, &date);RTC_GetTime(RTC_Format_BCD, &time);s_cursY = date.RTC_WeekDay;}
                                    strb2|=0x02;
                                    if(s_cursY <= 0){s_cursY = 1;}
                                    if(s_cursY >= 8){s_cursY = 7;}
                                    default_date.RTC_WeekDay = s_cursY;
                                    RTC_SetDate(RTC_Format_BCD, &default_date);
                                    SetAdres(0x02);
                                    PrintChar(0x5B);
                                    switch (default_date.RTC_WeekDay)
                                    {
                                    case RTC_Weekday_Monday:
                                       StartAdress_PrintChar(0x03, i003, sizeof(i003)); // ПН
                                       break;
                                    case RTC_Weekday_Tuesday:
                                       StartAdress_PrintChar(0x03, i004, sizeof(i004)); // ВТ
                                       break;
                                    case RTC_Weekday_Wednesday:
                                       StartAdress_PrintChar(0x03, i005, sizeof(i005)); // СР
                                       break;
                                    case RTC_Weekday_Thursday:
                                       StartAdress_PrintChar(0x03, i006, sizeof(i006)); // ЧТ
                                       break;
                                    case RTC_Weekday_Friday:
                                       StartAdress_PrintChar(0x03, i007, sizeof(i007)); // ПТ                                      
                                       break;
                                    case RTC_Weekday_Saturday:
                                       StartAdress_PrintChar(0x03, i008, sizeof(i008)); // СБ                                      
                                       break;
                                    case RTC_Weekday_Sunday:
                                       StartAdress_PrintChar(0x03, i009, sizeof(i009)); // ВС                                      
                                       break;

                                    default:
                                       break;
                                    } 
                                    PrintChar(0x5D);
                                    PrintChoise(0x07,time.RTC_Hours);
                                    PrintChar(0x3A);
                                    PrintChoise(0x0A,time.RTC_Minutes);
                                    StartAdress_PrintChar(0x0D, i00E, sizeof(i00E)); //ДН                                    
                                 }
                                 else if(cst == 3)
                                 {
                                    if(strb2&0x02){clear();strb2&=(~0x02);RTC_GetDate(RTC_Format_BCD, &date);RTC_GetTime(RTC_Format_BCD, &time);s_cursY = time.RTC_Hours;}
                                    strb2|=0x04;
                                    strb2|=0x01;
                                    if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                    if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                    if(s_cursY == 0x24){ s_cursY = 0x00;}
                                    if(s_cursY == 0xFF){ s_cursY = 0x23;}
                                    if(s_cursY == 0x1F){ s_cursY = 0x19;}
                                    if(s_cursY == 0x0F){ s_cursY = 0x09;}
                                    default_time.RTC_Hours = s_cursY;
                                    default_time.RTC_Minutes = time.RTC_Minutes;
                                    default_time.RTC_Seconds = 0;
                                    RTC_SetTime(RTC_Format_BCD, &default_time);
                                    //if(s_cursY < 0){s_cursY = 0;}

                                    switch (default_date.RTC_WeekDay)
                                    {
                                    case RTC_Weekday_Monday:
                                       StartAdress_PrintChar(0x03, i003, sizeof(i003)); // ПН
                                       break;
                                    case RTC_Weekday_Tuesday:
                                       StartAdress_PrintChar(0x03, i004, sizeof(i004)); // ВТ
                                       break;
                                    case RTC_Weekday_Wednesday:
                                       StartAdress_PrintChar(0x03, i005, sizeof(i005)); // СР
                                       break;
                                    case RTC_Weekday_Thursday:
                                       StartAdress_PrintChar(0x03, i006, sizeof(i006)); // ЧТ
                                    case RTC_Weekday_Friday:
                                       StartAdress_PrintChar(0x03, i007, sizeof(i007)); // ПТ                                      
                                       break;
                                    case RTC_Weekday_Saturday:
                                       StartAdress_PrintChar(0x03, i008, sizeof(i008)); // СБ                                      
                                       break;
                                    case RTC_Weekday_Sunday:
                                       StartAdress_PrintChar(0x03, i009, sizeof(i009)); // ВС                                      
                                       break;

                                    default:
                                       break;
                                    } 
                                    SetAdres(0x06);
                                    PrintChar(0x5B);
                                    PrintChoise(0x07,default_time.RTC_Hours);
                                    PrintChar(0x5D);
                                    PrintChoise(0x0A,default_time.RTC_Minutes);      
                                    StartAdress_PrintChar(0x0D, i00C, sizeof(i00C)); //Час
                                 }
                                 else if(cst == 4)
                                 {
                                    if(strb2&0x04){clear();strb2&=(~0x04);RTC_GetDate(RTC_Format_BCD, &date);RTC_GetTime(RTC_Format_BCD, &time);s_cursY = time.RTC_Minutes;}
                                    strb2|=0x02;
                                   if(s_cursY == 0x0A){ s_cursY = 0x10;}
                                   if(s_cursY == 0x1A){ s_cursY = 0x20;}
                                   if(s_cursY == 0x2A){ s_cursY = 0x30;}
                                   if(s_cursY == 0x3A){ s_cursY = 0x40;}
                                   if(s_cursY == 0x4A){ s_cursY = 0x50;}
                                   if(s_cursY == 0x5A){ s_cursY = 0x00;}
                                   if(s_cursY == 0xff){ s_cursY = 0x59;}
                                   if(s_cursY == 0x4f){ s_cursY = 0x49;}
                                   if(s_cursY == 0x3f){ s_cursY = 0x39;}
                                   if(s_cursY == 0x2f){ s_cursY = 0x29;}
                                   if(s_cursY == 0x1f){ s_cursY = 0x19;}
                                   if(s_cursY == 0x0f){ s_cursY = 0x09;}
                                    default_time.RTC_Hours = time.RTC_Hours;
                                    default_time.RTC_Minutes = s_cursY;
                                    default_time.RTC_Seconds = 0;
                                    RTC_SetTime(RTC_Format_BCD, &default_time);

                                    switch (default_date.RTC_WeekDay)
                                    {
                                    case RTC_Weekday_Monday:
                                       StartAdress_PrintChar(0x03, i003, sizeof(i003)); // ПН
                                       break;
                                    case RTC_Weekday_Tuesday:
                                       StartAdress_PrintChar(0x03, i004, sizeof(i004)); // ВТ
                                       break;
                                    case RTC_Weekday_Wednesday:
                                       StartAdress_PrintChar(0x03, i005, sizeof(i005)); // СР
                                       break;
                                    case RTC_Weekday_Thursday:
                                       StartAdress_PrintChar(0x03, i006, sizeof(i006)); // ЧТ
                                    case RTC_Weekday_Friday:
                                       StartAdress_PrintChar(0x03, i007, sizeof(i007)); // ПТ                                      
                                       break;
                                    case RTC_Weekday_Saturday:
                                       StartAdress_PrintChar(0x03, i008, sizeof(i008)); // СБ                                      
                                       break;
                                    case RTC_Weekday_Sunday:
                                       StartAdress_PrintChar(0x03, i009, sizeof(i009)); // ВС                                      
                                       break;

                                    default:
                                       break;
                                    } 

                                    PrintChoise(0x07,default_time.RTC_Hours);
                                    PrintChar(0x5B);
                                    PrintChoise(0x0A,default_time.RTC_Minutes);
                                    PrintChar(0x5D);      
                                    StartAdress_PrintChar(0x0D, i00B, sizeof(i00B)); //Час                                    
                                 }
                                 if(cursY == 2){cursY = 0;}
                                 if(cst == 5){cst = 0;Escape = 2;}
               }
            }
            break;
         default:
         break;
         }
   }

      
   }
   
}
void SystemClock_Config(void)
{
   CLK_HSEConfig(CLK_HSE_OFF);/*HSE Disable*/
   CLK_LSEConfig(CLK_LSE_OFF);/*LSE Cristal Resonator*/
   CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);/*System Clock Source HSI*/
   CLK_CCOConfig(CLK_CCOSource_LSI, CLK_CCODiv_16);/*LSE Clock Output & Clock Output Div 16*/
   CLK_BEEPClockConfig(CLK_BEEPCLKSource_Off);/*Clock BEEP Off*/
   CLK_RTCClockConfig(CLK_RTCCLKSource_LSI,CLK_RTCCLKDiv_1);/*Clock RTC : LSE & Clock RTC Div 1*/
   CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE); /*RTC ENABLE*/
   CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);/*UART ENABLE*/
   CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);/*TIM ENABLE*/
   CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);/*TIM ENABLE*/
   CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);/*i2c enable*/
   CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); /*Clock SYS DIV 1*/ 
}
void ProgDelay(unsigned long time)
{
   while(time--)
      asm("nop");
}




