#ifndef __SSD1289_H__
#define __SSD1289_H__

#include "board.h"

typedef struct 
{
  int16_t X;
  int16_t Y;
} Point, * pPoint;

#define R11 0x11
#define R34 0x22

#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001
#define LCD_PIXEL_WIDTH          0x0140
#define LCD_PIXEL_HEIGHT         0x00F0

void LCD_Setup(void);
void LCD_DeInit(void);
void LCD_SetTextColor(__IO u16 Color);
void LCD_SetBackColor(__IO u16 Color);
void LCD_Clear(uint16_t Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_SetPixel(u16 Xpos, u16 Ypos, u16 color);
void LCD_SetDisplayWindow(u16 Xpos, u16 Ypos, u16 Width, u16 Height);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(u8 Xpos, u16 Ypos, u16 Length, u8 Direction);

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);

void LCD_PowerOn(void);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);

void LCD_BacklightConfig(void);
FunctionalState LCD_BackLight(u8 percent);

void PMAToLCDBufferCopy(uint16_t wPMABufAddr, uint16_t offset ,uint16_t wNBytes);

#endif
