#include "Hardware/lcd.h"

void LCD_SetColor(u32 color);
void LCD_SetBackColor(u32 color);

void LCD_DrawPoint(int x, int y, u32 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u32 color);
void LCD_ShowString(u16 x,u16 y,const u8 *p,u32 color);

void LCD_DrawLine(int x0, int y0, int x1, int y1, u32 color);
void LCD_DDA_Line(int x0, int y0, int x1, int y1, u32 color);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u32 color);
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color);
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u32 color);
void LCD_DrawPicture(u16 x1,u16 y1,u16 x2,u16 y2,unsigned char *source);


