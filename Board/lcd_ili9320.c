#include "board.h"

static uint16_t deviceid;

volatile uint32_t Tick = 0x00000000;
void SysTick_Handler()
{
    if(Tick>0)
        Tick--;
}

void SysTickDelay(uint32_t msDelay)
{
    Tick=msDelay;
    while(Tick);
}

void LCDInitHardware()
{
    SysTick_Config(SystemCoreClock/1000);

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
    					   RCC_APB2Periph_GPIOG, ENABLE);
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

//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//
//    GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
//                                    GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10|
//                                    GPIO_Pin_11| GPIO_Pin_14| GPIO_Pin_15; //Interface
//
//    GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
//                                  GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14 |
//                                  GPIO_Pin_15;	//Interface
//    GPIO_Init(GPIOE, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = BacklightPin;	//Backlight
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(BacklightPort, &GPIO_InitStructure);
//
//    GPIO_InitStructure.GPIO_Pin = ResetPin; 	//Reset
//    GPIO_Init(ResetPort, &GPIO_InitStructure);
//
//    GPIO_SetBits(ResetPort,ResetPin);

    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

    /*-- FSMC Configuration -------------------------------------------------*/
    Timing_read.FSMC_AddressSetupTime = 30;             /* ВµГ�Г–В·ВЅВЁГЃВўГЉВ±ВјГ¤  */
    Timing_read.FSMC_DataSetupTime = 30;                /* ГЉГЅВѕГќВЅВЁГЃВўГЉВ±ВјГ¤  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;    /* FSMC В·ГѓГЋГЉГ„ВЈГЉВЅ */

    Timing_write.FSMC_AddressSetupTime = 3;             /* ВµГ�Г–В·ВЅВЁГЃВўГЉВ±ВјГ¤  */
    Timing_write.FSMC_DataSetupTime = 3;                /* ГЉГЅВѕГќВЅВЁГЃВўГЉВ±ВјГ¤  */
    Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;   /* FSMC В·ГѓГЋГЉГ„ВЈГЉВЅ */

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

//    FSMC_Timing.FSMC_AddressSetupTime = 1;
//    FSMC_Timing.FSMC_AddressHoldTime = 0;
//    FSMC_Timing.FSMC_DataSetupTime = 5;
//    FSMC_Timing.FSMC_BusTurnAroundDuration = 0;
//    FSMC_Timing.FSMC_CLKDivision = 0;
//    FSMC_Timing.FSMC_DataLatency = 0;
//    FSMC_Timing.FSMC_AccessMode = FSMC_AccessMode_B;
//
//    FSMC_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
//    FSMC_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
//    FSMC_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
//    FSMC_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
//    FSMC_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
//    FSMC_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
//    FSMC_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
//    FSMC_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
//    FSMC_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
//    FSMC_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
//    FSMC_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
//    FSMC_InitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
//    FSMC_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
//    FSMC_InitStructure.FSMC_ReadWriteTimingStruct = &FSMC_Timing;
//    FSMC_InitStructure.FSMC_WriteTimingStruct = &FSMC_Timing;
//
//    FSMC_NORSRAMInit(&FSMC_InitStructure);
//    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}

void LCDInit()
{
	LCD_WRITE_REGISTER(0x0000,0x0001);
	SysTickDelay(50000);  //ВґГІВїВЄВѕВ§Г•Г±
	LCD_WRITE_REGISTER(0x0003,0xA8A4);
	SysTickDelay(50000);   //0xA8A4
	LCD_WRITE_REGISTER(0x000C,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x000D,0x080C);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x000E,0x2B00);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x001E,0x00B0);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0001,0x2B3F);
	SysTickDelay(50000);   //Г‡ГЅВ¶ВЇГЉГ¤ВіГ¶ВїГ�Г–Г†320*240  0x6B3F
	LCD_WRITE_REGISTER(0x0002,0x0600);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0010,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0011,0x6070);
	SysTickDelay(50000);        //0x4030           //В¶ВЁГ’ГҐГЉГЅВѕГќВёГ±ГЉВЅ  16ГЋВ»Г‰В« 		ВєГЎГ†ГЃ 0x6058
	LCD_WRITE_REGISTER(0x0005,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0006,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0016,0xEF1C);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0017,0x0003);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0007,0x0233);
	SysTickDelay(50000);        //0x0233
	LCD_WRITE_REGISTER(0x000B,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x000F,0x0000);
	SysTickDelay(50000);        //Г‰ВЁГѓГЁВїВЄГЉВјВµГ�Г–В·
	LCD_WRITE_REGISTER(0x0041,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0042,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0048,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0049,0x013F);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x004A,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x004B,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0044,0xEF00);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0045,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0046,0x013F);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0030,0x0707);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0031,0x0204);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0032,0x0204);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0033,0x0502);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0034,0x0507);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0035,0x0204);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0036,0x0204);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0037,0x0502);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x003A,0x0302);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x003B,0x0302);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0023,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0024,0x0000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x0025,0x8000);
	SysTickDelay(50000);
	LCD_WRITE_REGISTER(0x004f,0);        //ГђГђГЉГ—Г–В·0
	LCD_WRITE_REGISTER(0x004e,0);        //ГЃГђГЉГ—Г–В·0
    for(i=50000; i>0; i--);

//	LCDHardwareReset();
//
//	LCD_WRITE_REGISTER(CTR_OSC_START, 0x0001);
//	LCD_WRITE_REGISTER(CTR_DRV_OUTPUT1, 0x0100);
//	LCD_WRITE_REGISTER(CTR_DRV_WAVE, 0x0700);
//	LCD_WRITE_REGISTER(CTR_ENTRY_MODE, 0x1038);
//	LCD_WRITE_REGISTER(CTR_RESIZE, 0x0000);
//	LCD_WRITE_REGISTER(CTR_DISPLAY2, 0x0202);
//	LCD_WRITE_REGISTER(CTR_DISPLAY3, 0x0000);
//	LCD_WRITE_REGISTER(CTR_DISPLAY4, 0x0000);
//	LCD_WRITE_REGISTER(CTR_RGB_INTERFACE1, 0x0001);
//	LCD_WRITE_REGISTER(CTR_FRM_MARKER, 0x0000);
//	LCD_WRITE_REGISTER(CTR_RGB_INTERFACE2, 0x0000);
//
//	LCD_WRITE_REGISTER(CTR_POWER1, 0x0000);
//	LCD_WRITE_REGISTER(CTR_POWER2, 0x0007);
//	LCD_WRITE_REGISTER(CTR_POWER3, 0x0000);
//	LCD_WRITE_REGISTER(CTR_POWER4, 0x0000);
//
//	SysTickDelay(200);
//
//	LCD_WRITE_REGISTER(CTR_POWER1, 0x1590);
//	LCD_WRITE_REGISTER(CTR_POWER2, 0x0227);
//
//	SysTickDelay(50);
//
//	LCD_WRITE_REGISTER(CTR_POWER3, 0x009C);
//
//	SysTickDelay(50);
//
//	LCD_WRITE_REGISTER(CTR_POWER4, 0x1900);
//	LCD_WRITE_REGISTER(CTR_POWER7, 0x1900);
//	LCD_WRITE_REGISTER(CTR_FRM_COLOR, 0x000E);
//
//	SysTickDelay(50);
//
//	LCD_WRITE_REGISTER(CTR_HORZ_ADDRESS, 0x0000);
//	LCD_WRITE_REGISTER(CTR_VERT_ADDRESS, 0x0000);
//
//	LCD_WRITE_REGISTER(CTR_HORZ_START, 0x0000);
//	LCD_WRITE_REGISTER(CTR_HORZ_END, 239);
//	LCD_WRITE_REGISTER(CTR_VERT_START, 0x0000);
//	LCD_WRITE_REGISTER(CTR_VERT_END, 319);
//
//	LCD_WRITE_REGISTER(CTR_DRV_OUTPUT2, 0x2700);
//	LCD_WRITE_REGISTER(CTR_BASE_IMAGE, 0x0001);
//	LCD_WRITE_REGISTER(CTR_VERT_SCROLL, 0x0000);
//
//	GPIO_SetBits(BacklightPort,BacklightPin);
}

void LCDHardwareReset()
{
    GPIO_ResetBits(ResetPort,ResetPin);
    SysTickDelay(50);
    GPIO_SetBits(ResetPort,ResetPin);
    SysTickDelay(10);
}

void LCDOn()
{
	LCD_WRITE_REGISTER(CTR_DISPLAY1, 0x0133);
}

void LCDOff()
{
	LCD_WRITE_REGISTER(CTR_DISPLAY1, 0x0131);
}

void LCDSetBounds(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	LCD_WRITE_REGISTER(CTR_VERT_START, left);
	LCD_WRITE_REGISTER(CTR_VERT_END, right);
	LCD_WRITE_REGISTER(CTR_HORZ_START, top);
	LCD_WRITE_REGISTER(CTR_HORZ_END, bottom);

	LCD_WRITE_REGISTER(CTR_HORZ_ADDRESS, top);
	LCD_WRITE_REGISTER(CTR_VERT_ADDRESS, left);
}

void LCDClear(uint16_t color)
{
	LCDSetBounds(0,0,320-1,240-1);
	LCD_BEGIN_RAM_WRITE;
	uint32_t i;
	for(i=0;i<320*240;i++)
	LCD_WRITE_RAM(color);
}
