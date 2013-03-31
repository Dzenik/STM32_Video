#include "ssd1289.h"
#include <math.h>

void LCD_Init(void);
void LCD_Config(void);

//static u16 Channel2Pulse = 1000;

//static gl_sFONT *LCD_Currentfonts;

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A2 */
#define LCD_REG              (*((volatile unsigned short *) 0x6C000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6C000002)) /* RS = 1 */

static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;


void LCD_CtrlLinesConfig(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
                           | RCC_APB2Periph_GPIOG, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
                        
  /*
  FSMC_D0 ~ FSMC_D3
  PD14 FSMC_D0   PD15 FSMC_D1   PD0  FSMC_D2   PD1  FSMC_D3
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD,&GPIO_InitStructure);

  /*
  FSMC_D4 ~ FSMC_D12
  PE7 ~ PE15  FSMC_D4 ~ FSMC_D12
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE,&GPIO_InitStructure);

  /* FSMC_D13 ~ FSMC_D15   PD8 ~ PD10 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOD,&GPIO_InitStructure);

  /*
  FSMC_A0 ~ FSMC_A5   FSMC_A6 ~ FSMC_A9
  PF0     ~ PF5       PF12    ~ PF15
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
                                | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOF,&GPIO_InitStructure);

  /* FSMC_A10 ~ FSMC_A15  PG0 ~ PG5 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOG,&GPIO_InitStructure);

  /* FSMC_A16 ~ FSMC_A18  PD11 ~ PD13 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOD,&GPIO_InitStructure);

  /* RD-PD4 WR-PD5 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOD,&GPIO_InitStructure);

  /* NBL0-PE0 NBL1-PE1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOE,&GPIO_InitStructure);

  /* NE1/NCE2 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
  /* NE2 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOG,&GPIO_InitStructure);
  /* NE3 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOG,&GPIO_InitStructure);
  /* NE4 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG,&GPIO_InitStructure);
}

void LCD_FSMCConfig(void)
{  
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

    /*-- FSMC Configuration -------------------------------------------------*/
    Timing_read.FSMC_AddressSetupTime = 30;             /*   */
    Timing_read.FSMC_DataSetupTime = 30;                /*   */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;    /* FSMC*/

    Timing_write.FSMC_AddressSetupTime = 3;             /* */
    Timing_write.FSMC_DataSetupTime = 3;                /* */
    Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;   /* FSMC*/

    /* Color LCD configuration ------------------------------------
       LCD configured as follow:
          - Data/Address MUX = Disable
          - Memory Type = SRAM
          - Data Width = 16bit
          - Write Operation = Enable
          - Extended Mode = Enable
          - Asynchronous Wait = Disable */
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

    /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

void LCD_Setup(void)
{
  //LCD_BacklightConfig();
  LCD_CtrlLinesConfig();
  LCD_FSMCConfig();
  LCD_Init();
}

void LCD_DeInit(void)
{
  // to add some code
}

void LCD_WriteReg(u8 LCD_Reg,u16 LCD_RegValue)
{
  LCD_REG = LCD_Reg;
  LCD_RAM = LCD_RegValue;
}

uint16_t LCD_ReadReg(u8 LCD_Reg)
{
  LCD_REG = LCD_Reg;
  return (LCD_RAM);
}

void LCD_WriteRAM_Prepare(void)
{
  LCD_REG = R34;
}

void LCD_WriteRAM(uint16_t RGB_Code)					 
{
  LCD_RAM = RGB_Code;
}

uint16_t LCD_ReadRAM(void)
{
//  volatile uint16_t dummy;
  LCD_REG = R34; /* Select GRAM Reg */
//  dummy = LCD_RAM;
  
  return LCD_RAM;
}

void LCD_Delay(u16 nCount)
{
 uint16_t TimingDelay; 
 while(nCount--)
   {
    for(TimingDelay=0;TimingDelay<10000;TimingDelay++);
   }
}

void LCD_Init()
{
  LCD_Delay(5);	
  
//  GPIO_ResetBits(GPIOC, GPIO_Pin_0);
//  LCD_Delay(100);
//  GPIO_SetBits(GPIOC, GPIO_Pin_0);
  LCD_Delay(5);
  u16 DeviceCode = LCD_ReadReg(0x0000);
  if(DeviceCode==0x8989)
  {
    LCD_WriteReg(0x0000,0x0001);    LCD_Delay(5);
    LCD_WriteReg(0x0003,0xA8A4);    LCD_Delay(5);   
    LCD_WriteReg(0x000C,0x0000);    LCD_Delay(5);   
    LCD_WriteReg(0x000D,0x080C);    LCD_Delay(5);   
    LCD_WriteReg(0x000E,0x2B00);    LCD_Delay(5);   
    LCD_WriteReg(0x001E,0x00B0);    LCD_Delay(5);   
    LCD_WriteReg(0x0001,0x2B3F);    LCD_Delay(5);
    LCD_WriteReg(0x0002,0x0600);    LCD_Delay(5);
    LCD_WriteReg(0x0010,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0011,0x6030);    LCD_Delay(5);
    LCD_WriteReg(0x0005,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0006,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0016,0xEF1C);    LCD_Delay(5);
    LCD_WriteReg(0x0017,0x0003);    LCD_Delay(5);
    LCD_WriteReg(0x0007,0x0133);    LCD_Delay(5);         
    LCD_WriteReg(0x000B,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x000F,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0041,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0042,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0048,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0049,0x013F);    LCD_Delay(5);
    LCD_WriteReg(0x004A,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x004B,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0044,0xEF00);    LCD_Delay(5);
    LCD_WriteReg(0x0045,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0046,0x013F);    LCD_Delay(5);
    LCD_WriteReg(0x0030,0x0707);    LCD_Delay(5);
    LCD_WriteReg(0x0031,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0032,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0033,0x0502);    LCD_Delay(5);
    LCD_WriteReg(0x0034,0x0507);    LCD_Delay(5);
    LCD_WriteReg(0x0035,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0036,0x0204);    LCD_Delay(5);
    LCD_WriteReg(0x0037,0x0502);    LCD_Delay(5);
    LCD_WriteReg(0x003A,0x0302);    LCD_Delay(5);
    LCD_WriteReg(0x003B,0x0302);    LCD_Delay(5);
    LCD_WriteReg(0x0023,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0024,0x0000);    LCD_Delay(5);
    LCD_WriteReg(0x0025,0x8000);    LCD_Delay(5);
    LCD_WriteReg(0x004f,0);
    LCD_WriteReg(0x004e,0);
  }
  LCD_Delay(5);  /* delay 50 ms */		
}

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
  LCD_WriteReg(0x004e, Xpos); /* Row */
  LCD_WriteReg(0x004f, Ypos); /* Line */
}

void LCD_SetDisplayWindow(u16 Xpos, u16 Ypos, u16 Width, u16 Height)
{
  u16 x2 = Xpos - Width + 1;
  u16 y2 = Ypos - Height + 1;
  LCD_WriteReg(0x0044, (Xpos << 8) | x2);    // Source RAM address window 
  LCD_WriteReg(0x0045, y2);    // Gate RAM address window 
  LCD_WriteReg(0x0046, Ypos);    // Gate RAM address window 
  LCD_SetCursor(Xpos, Ypos);
}

void LCD_WindowModeDisable(void)
{
  LCD_SetDisplayWindow(239, 319, 240, 320);
  LCD_WriteReg(R11, 0x6018);
}

void LCD_SetPixel(u16 Xpos, u16 Ypos, u16 color)
{
  LCD_SetCursor(Xpos, Ypos);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(color);
}

void LCD_Clear(uint16_t Color)
{
  uint32_t index=0;
  LCD_SetDisplayWindow(239, 319, 240, 320);
  LCD_SetCursor(0,0); 
  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  for(index=0;index<76800;index++)
   {
     LCD_RAM=Color;
   }
}

uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
  LCD_SetCursor(Xpos,Ypos);
  return LCD_ReadRAM();
}

void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
  if ( ( Xpos > 239 ) ||( Ypos > 319 ) ) return;
  LCD_SetCursor(Xpos,Ypos);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(point);
}

void LCD_SetTextColor(__IO uint16_t Color)
{
  TextColor = Color;
}

void LCD_SetBackColor(__IO uint16_t Color)
{
  BackColor = Color;
}

void LCD_DrawLine(u8 Xpos, u16 Ypos, u16 Length, u8 Direction)
{
  uint16_t newentrymod;

  if (Direction == LCD_DIR_VERTICAL)
    newentrymod = 0x6030;   // we want a 'vertical line'
  else 
    newentrymod = 0x6018;   // we want a 'horizontal line'
    
  LCD_WriteReg(R11, newentrymod);
  LCD_SetCursor(Xpos, Ypos);
  LCD_WriteRAM_Prepare();
  
  while (Length--) {
    LCD_WriteRAM(TextColor); 
  }
  LCD_WriteReg(R11, 0x6018);
}


void LCD_PowerOn(void)
{
  //
}

void LCD_DisplayOn(void)
{
  u16 dummy = LCD_ReadReg(0x0007);
  dummy |= 0x0033;
  LCD_WriteReg(0x0007, dummy);
  LCD_BackLight(100);
}

void LCD_DisplayOff(void)
{
  u16 dummy = LCD_ReadReg(0x0007);
  dummy &= ~0x0033;
  LCD_WriteReg(0x0007, dummy);
  LCD_BackLight(0);
}

void LCD_BacklightConfig(void)
{
  
}

FunctionalState LCD_BackLight(uint8_t percent)
{
  if( percent > 0 && percent <= 100)
  {
    return ENABLE;
  } 
  else
    return DISABLE;
}

