Any accepted commit will be merged..
Peek from the main.c i wrote the source code beatuifully!! ;)
```
#include "main.h"

#include "stdio.h"
#include "myLCD.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

//conditioning
u8 sw5stsnow = 0;
u8 trainconnow = 0;

#define SW(x) (HAL_GPIO_ReadPin(SW##x##_GPIO_Port, SW##x##_Pin))
#define GPIO_WRITE_PIN(g, x) HAL_GPIO_WritePin(g##_GPIO_Port, g##_Pin, x)

#define CLOCK(x) GPIO_WRITE_PIN(CLOCK, x)
#define LATCH(x) GPIO_WRITE_PIN(LATCH, x)
#define DATA(x) GPIO_WRITE_PIN(DATA, x)
#define BUZZER(x) if(mute)GPIO_WRITE_PIN(BUZZER, x)
#define RED(x) GPIO_WRITE_PIN(LED_RED, !x)
#define GREEN(x) GPIO_WRITE_PIN(LED_GREEN, !x)

ADC_HandleTypeDef hadc1;

_Bool mute = 0, switchpressed[5] = { 0 }, erasecon = 0, brk, nsw[5] = { 0, 0, 0,
  0, 0 };
u8 menu = 1, mainmenu = 1, settings = 1, traincolortype, traingo, trainstop,
  traincon;
u16 dttrain16[4];
u32 sw5timer, eraset, timersec, traincolor[5] = { 0 }, dttrain32;
char lcd_buffer[32];

void SystemClock_Config (void);
static void MX_GPIO_Init (void);
static void MX_ADC1_Init (void);

void sw5ck ();
void trsrt (u8, _Bool, _Bool, u8);
void bkm1 ();
void senddt (u8, u8);
void ledtrstr ();
void allbtnfunc ();
u16 adcvreader ();

int main (void){

 HAL_Init();

 SystemClock_Config();

 MX_GPIO_Init();
 MX_ADC1_Init();

 //greets
 RED(0);
 GREEN(0);
 senddt(1, -1);
 senddt(2, -1);
 LCD_Init();
 LCD_SetBacklight(1);
 LCD_SetCursor(0, 0);
 LCD_Print("------TEMPLATE------");
 LCD_SetCursor(0, 1);
 LCD_Print(" LKS PROVINSI 2023  ");
 HAL_Delay(3000);

 while (1){
  allbtnfunc();
  switch (mainmenu){
   case 1:
    senddt(1, -1);
    senddt(2, -1);
    RED(0);
    GREEN(0);
    switch (menu){
     case 1:
      bkm1();
      LCD_SetCursor(0, 2);
      LCD_Print("[MANUAL            ]");
      LCD_SetCursor(0, 3);
      LCD_Print("                 1/3");
      break;
     case 2:
      bkm1();
      LCD_SetCursor(0, 2);
      LCD_Print("[REPORT            ]");
      LCD_SetCursor(0, 3);
      LCD_Print("                 2/3");
      break;
     case 3:
      bkm1();
      LCD_SetCursor(0, 2);
      LCD_Print("[SETTINGS          ]");
      LCD_SetCursor(0, 3);
      LCD_Print("                 3/3");
      break;
    }
    break;
   case 2:
    ledtrstr();
    LCD_SetCursor(0, 3);
    sprintf(lcd_buffer, "%3lu", traincolor[0]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(4, 3);
    sprintf(lcd_buffer, "%3lu", traincolor[1]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(8, 3);
    sprintf(lcd_buffer, "%3lu", traincolor[2]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(12, 3);
    sprintf(lcd_buffer, "%3lu", traincolor[3]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(16, 3);
    sprintf(lcd_buffer, "%4lu", traincolor[4]);
    LCD_Print(lcd_buffer);
    break;
   case 3:
    sw5ck();
    LCD_SetCursor(0, 0);
    LCD_Print("   STATION REPORT   ");
    LCD_SetCursor(0, 1);
    sprintf(lcd_buffer, "UPTIME:%3lumin%3lusec", (uwTick / 60000) % 60,
      uwTick / 1000 % 60);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(0, 2);
    sprintf(lcd_buffer, "RED:%-3lu", traincolor[0]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(0, 3);
    sprintf(lcd_buffer, "GRN:%-3lu", traincolor[1]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(7, 2);
    sprintf(lcd_buffer, "BLU:%-3lu", traincolor[2]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(7, 3);
    sprintf(lcd_buffer, "WHT:%-3lu", traincolor[3]);
    LCD_Print(lcd_buffer);
    LCD_SetCursor(14, 2);
    sprintf(lcd_buffer, "DFC:%-2lu", traincolor[4]);
    LCD_Print(lcd_buffer);
    if (uwTick - eraset >= 500){
     eraset = uwTick;
     if (erasecon == 0){
      erasecon = 1;
     }else{
      erasecon = 0;
     }
    }
    if (erasecon == 1){
     LCD_SetCursor(14, 3);
     LCD_Print("ERASE?");
    }else{
     LCD_SetCursor(14, 3);
     LCD_Print("      ");
    }
    break;
   case 4:
    LCD_SetCursor(0, 0);
    LCD_Print("  STATION SETTINGS  ");
    LCD_SetCursor(0, 1);
    LCD_Print("====================");
    if (HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin) == 1){
     LCD_SetCursor(3, 2);
     LCD_Print("BACKLIGHT: ON    ");
    }else{
     LCD_SetCursor(3, 2);
     LCD_Print("BACKLIGHT: OFF   ");
    }
    if (mute == 0){
     LCD_SetCursor(3, 3);
     LCD_Print("MUTE MODE: ON    ");
    }else{
     LCD_SetCursor(3, 3);
     LCD_Print("MUTE MODE: OFF   ");
    }
    // cursor
    switch (settings){
     case 1:
      LCD_SetCursor(0, 2);
      LCD_Print("=> ");
      LCD_SetCursor(0, 3);
      LCD_Print("   ");
      break;
     case 2:
      LCD_SetCursor(0, 2);
      LCD_Print("   ");
      LCD_SetCursor(0, 3);
      LCD_Print("=> ");
      break;
    }
    break;
  }
 }
}

void SystemClock_Config (void){
 RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
 RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
 RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
 RCC_OscInitStruct.HSEState = RCC_HSE_ON;
 RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
 RCC_OscInitStruct.HSIState = RCC_HSI_ON;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
 RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
  Error_Handler();
 }

 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
   | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
 RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
 RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
  Error_Handler();
 }
 PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
 PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
 if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK){
  Error_Handler();
 }
}

static void MX_ADC1_Init (void){

 ADC_ChannelConfTypeDef sConfig = { 0 };

 hadc1.Instance = ADC1;
 hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
 hadc1.Init.ContinuousConvMode = ENABLE;
 hadc1.Init.DiscontinuousConvMode = DISABLE;
 hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
 hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
 hadc1.Init.NbrOfConversion = 1;
 if (HAL_ADC_Init(&hadc1) != HAL_OK){
  Error_Handler();
 }

 sConfig.Channel = ADC_CHANNEL_0;
 sConfig.Rank = ADC_REGULAR_RANK_1;
 sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
 if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
  Error_Handler();
 }
}

static void MX_GPIO_Init (void){
 GPIO_InitTypeDef GPIO_InitStruct = { 0 };

 __HAL_RCC_GPIOD_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();

 HAL_GPIO_WritePin(GPIOA,
 LCD_D4_Pin | LCD_D5_Pin | LATCH_Pin | DATA_Pin | CLOCK_Pin, GPIO_PIN_RESET);

 HAL_GPIO_WritePin(GPIOB,
   LCD_D6_Pin | BUZZER_Pin | LED_GREEN_Pin | LED_RED_Pin | LCD_BL_Pin
     | LCD_EN_Pin | LCD_RW_Pin | LCD_RS_Pin | LCD_D7_Pin, GPIO_PIN_RESET);

 GPIO_InitStruct.Pin = SW1_Pin | SW2_Pin | SW3_Pin | SW4_Pin | SW5_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_PULLUP;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 GPIO_InitStruct.Pin = LCD_D4_Pin | LCD_D5_Pin | LATCH_Pin | DATA_Pin
   | CLOCK_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 GPIO_InitStruct.Pin = LCD_D6_Pin | BUZZER_Pin | LED_GREEN_Pin | LED_RED_Pin
   | LCD_BL_Pin | LCD_EN_Pin | LCD_RW_Pin | LCD_RS_Pin | LCD_D7_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void sw5ck (){
 switch (sw5stsnow){
  case 0:
   if (SW(5) == GPIO_PIN_RESET){
    sw5stsnow = 1;
   }
   break;
  case 1:
   if (uwTick - timersec >= 1000){
    timersec = uwTick;
    sw5timer++;
   }
   if (SW(5) == GPIO_PIN_SET){
    sw5stsnow = 2;
   }
   break;
  case 2:
   if (sw5timer >= 3){
    uwTick = 0;
    for (uint8_t i = 0; i < 5; i++){
     traincolor[i] = 0;
    }
   }else{
    mainmenu = 1;
   }
   sw5stsnow = 0;
   sw5timer = 0;
   break;
  default:
   break;
 }
}

void trsrt (u8 stopplc, _Bool REDb, _Bool GRNb, uint8_t whichtr){
 if (traingo <= stopplc){
  senddt(1, traingo);
  traingo++;
  BUZZER(mute);
  uint16_t temp = 100 + adcvreader();
  for (int i = 0; i < temp; i++){
   allbtnfunc();
   HAL_Delay(1);
  }
  BUZZER(0);
 }else{
  traingo = 0;
  trainconnow = 0;
  RED(REDb);
  GREEN(GRNb);
  traincolortype = 0;
  senddt(1, -1);
  senddt(2, stopplc);
  traincolor[whichtr]++;
  for (int i = 0; i < 3000; i++){
   allbtnfunc();
   HAL_Delay(1);
  }
  senddt(2, -1);
  RED(0);
  GREEN(0);
 }
}

void bkm1 (){
 LCD_SetCursor(0, 0);
 LCD_Print("  SORTING  STATION  ");
 LCD_SetCursor(0, 1);
 LCD_Print("  SELECT OPERATION  ");
}

void senddt (uint8_t line, uint8_t posisi){
 if (line == 1){
  dttrain16[1] = 1 << (15 - posisi);
 }
 if (line == 2){
  dttrain16[2] = 1 << (posisi);
 }
 for (uint8_t i = 0; i < 32; i++){
  DATA(!(((dttrain16[1] << 16 | dttrain16[2]) >> i) & 1));
  CLOCK(0);
  CLOCK(1);
 }
 LATCH(1);
 LATCH(0);
}

void ledtrstr (){
 switch (trainconnow){
  case 0:
   if (traingo <= 4){
    senddt(1, traingo);
    traingo++;
    BUZZER(1);
    uint16_t temp = 100 + adcvreader();
    for (int i = 0; i < temp; i++){
     allbtnfunc();
     HAL_Delay(1);
    }
    BUZZER(0);
   }else{
    trainconnow = 1;
   }
   break;
  case 1:
   for (int i = 0; i < 3000; i++){
    brk = 0;
    allbtnfunc();
    if (brk){
     break;
    }
    HAL_Delay(1);
   }
   if (traincolortype == 0){
    for (int i = 0; i < 3; i++){
     BUZZER(mute);
     RED(1);
     for (int i = 0; i < 400; i++){
      HAL_Delay(1);
      if (mainmenu != 2){
       break;
      }
     }
     BUZZER(0);
     RED(0);
     for (int i = 0; i < 200; i++){
      HAL_Delay(1);
      if (mainmenu != 2){
       break;
      }
     }
    }
   }
   trainconnow = 2;
   break;
  case 2:
   switch (traincolortype){
    case 0:
     trsrt(15, 0, 0, 4);
     break;
    case 1:
     trsrt(6, 1, 0, 0);
     break;
    case 2:
     trsrt(8, 0, 1, 1);
     break;
    case 3:
     trsrt(10, 1, 1, 2);
     break;
    case 4:
     trsrt(13, 0, 0, 3);
     break;
   }
   break;
 }
}

void allbtnfunc (){

 // f5
 if (!SW(5) && !nsw[5]){
  BUZZER(1);
  HAL_Delay(100);
  BUZZER(0);
  switch (mainmenu){
   case 1:
    switch (menu){
     case 1:
      mainmenu = 2;
      LCD_SetCursor(0, 0);
      LCD_Print("    MANUAL  MODE    ");
      LCD_SetCursor(0, 1);
      LCD_Print("====================");
      LCD_SetCursor(0, 2);
      LCD_Print("RED GRN BLU WHT DFCT");
      LCD_SetCursor(0, 3);
      sprintf(lcd_buffer, "%3lu", traincolor[0]);
      LCD_Print(lcd_buffer);
      LCD_SetCursor(4, 3);
      sprintf(lcd_buffer, "%3lu", traincolor[1]);
      LCD_Print(lcd_buffer);
      LCD_SetCursor(8, 3);
      sprintf(lcd_buffer, "%3lu", traincolor[2]);
      LCD_Print(lcd_buffer);
      LCD_SetCursor(12, 3);
      sprintf(lcd_buffer, "%3lu", traincolor[3]);
      LCD_Print(lcd_buffer);
      LCD_SetCursor(16, 3);
      sprintf(lcd_buffer, "%4lu", traincolor[4]);
      LCD_Print(lcd_buffer);
      break;
     case 2:
      mainmenu = 3;
      break;
     case 3:
      mainmenu = 4;
      break;
    }
    traingo = 0;
    traincon = 0;
    trainconnow = 0;
    break;
   case 2:
    bkm1();
    mainmenu = 1;
    brk = 1;
    LCD_SetCursor(0, 2);
    LCD_Print("[MANUAL            ]");
    LCD_SetCursor(0, 3);
    LCD_Print("                 1/3");
    break;
   case 4:
    bkm1();
    mainmenu = 1;
    LCD_SetCursor(0, 2);
    LCD_Print("[SETTINGS          ]");
    LCD_SetCursor(0, 3);
    LCD_Print("                 3/3");
    break;
  }

  nsw[5] = 1;
 }
 if (SW(5) && nsw[5]){
  nsw[5] = 0;
 }
 // end f5

 // f4
 if (!SW(4) && !nsw[4]){
  BUZZER(1);
  HAL_Delay(100);
  BUZZER(0);
  switch (mainmenu){
   case 1:
    menu++;
    if (menu == 4){
     menu = 1;
    }
    break;
   case 2:
    switch (trainconnow){
     case 1:
      traincolortype = 4;
      RED(0);
      GREEN(0);
      break;
     default:
      break;
    }
    break;
   case 4:
    switchpressed[4] = 0;
    if (settings == 2){
     settings = 1;
    }else{
     settings = 2;
    }
    break;
  }
  nsw[4] = 1;
 }
 if (SW(4) && nsw[4]){
  nsw[4] = 0;
 }
 // end f4

 // f3
 if (!SW(3) && !nsw[3]){
  BUZZER(1);
  HAL_Delay(100);
  BUZZER(0);
  switch (mainmenu){
   case 1:
    menu--;
    if (menu == 0){
     menu = 3;
    }
    break;
   case 2:
    switch (trainconnow){
     case 1:
      traincolortype = 3;
      RED(1);
      GREEN(1);
      break;
     default:
      break;
    }
    break;
   case 4:
    if (settings == 1){
     settings = 2;
    }else{
     settings = 1;
    }
    break;
  }
  nsw[3] = 1;
 }
 if (SW(3) && nsw[3]){
  nsw[3] = 0;
 }
 // end f3

 // f2
 if (!SW(2) && !nsw[2]){
  BUZZER(1);
  HAL_Delay(100);
  BUZZER(0);
  switch (mainmenu){
   case 2:
    switch (trainconnow){
     case 1:
      traincolortype = 2;
      RED(0);
      GREEN(1);
      break;
     default:
      break;
    }
    break;
   case 4:
    if (settings == 1 && HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin) == 1){
     LCD_SetBacklight(0);
    }else if (settings == 1
      && HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin) == 0){
     LCD_SetBacklight(1);
    }
    if (settings == 2 && mute == 0){
     mute = 1;
    }else if (settings == 2 && mute == 1){
     mute = 0;
    }
    break;
  }
  nsw[2] = 1;
 }
 if (SW(2) && nsw[2]){
  nsw[2] = 0;
 }
 // endf2

 // f1
 if (!SW(1) && !nsw[1]){
  switch (mainmenu){
   case 2:
    switch (trainconnow){
     case 1:
      traincolortype = 1;
      RED(1);
      GREEN(0);
      break;
     default:
      break;
    }
    break;
  }
  nsw[1] = 1;
 }
 if (SW(1) && nsw[1]){
  nsw[1] = 0;
 }
}

u16 adcvreader (){
 u16 adcv = 100;
 HAL_ADC_Start(&hadc1);
 HAL_ADC_PollForConversion(&hadc1, 100);
 adcv = +HAL_ADC_GetValue(&hadc1);
 HAL_ADC_Stop(&hadc1);
 return adcv;
}

void Error_Handler (void){
 __disable_irq();
 while (1){
  LCD_Print("CERR");
 }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
```
