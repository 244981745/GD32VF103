#ifndef __LCD_H
#define __LCD_H		

#include "systick.h"
#include "stdlib.h"	
#include "gd32vf103_gpio.h"
#include <gd32vf103_spi.h>

/* =================== color =================== */
#define WHITE           0xFFFFFF
#define BLACK           0x000000

#define RED             0xFF0000
#define GREEN           0x00FF00
#define BLUE            0x0000FF

#define GRAY            0xC0COC0
#define BROWN           0xA52A2A

#define CYAN            0x00FFFF
#define YELLOW          0xFFFF00
#define MAGENTA         0xFF00FF

#define DARKBLUE      	 0x000080	//navy blue
#define LIGHTBLUE      	 0xADD8E6	//light blue 

#define DARKGREEN        0x006400
#define LIGHTGREEN     	 0X90EE90       //light green

#define LIGHTGRAY 	 0xD3D3D3       //Light gray
#define LIGHTCYAN        0xE0FFFF
#define LIGHTYELLOW      0xFFFFE0

/* =================== config =================== */
#define NEWLCD      1
#define MODE_16_18  1   // 0:16bit mode   1:18bit mode

#define USE_HORIZONTAL 2  //Set horizontal or vertical screen display 0 or 1 for vertical screen 2 or 3 for horizontal screen
#define HAS_BLK_CNTL    0

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 80
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 80
#endif

typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;


// #define LED_ON gpio_bit_reset(GPIOC,GPIO_PIN_13)
// #define LED_OFF gpio_bit_set(GPIOC,GPIO_PIN_13)

#define LED_ON 
#define LED_OFF 

#define SPI0_CFG 1  //hardware spi
// #define SPI0_CFG 2  //hardware spi dma
// #define SPI0_CFG 3  //software spi

#define FRAME_SIZE  25600

//-----------------OLED端口定义---------------- 
#if SPI0_CFG == 1
#define OLED_CS_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_2)     //CS PB2
#define OLED_CS_Set() gpio_bit_set(GPIOB,GPIO_PIN_2)
#elif SPI0_CFG == 2
#define OLED_SCLK_Clr() 
#define OLED_SCLK_Set() 

#define OLED_SDIN_Clr()
#define OLED_SDIN_Set()

#define OLED_CS_Clr()
#define OLED_CS_Set()
#else /* SPI0_CFG */
#define OLED_SCLK_Clr() gpio_bit_reset(GPIOA,GPIO_PIN_5)    //CLK PA5
#define OLED_SCLK_Set() gpio_bit_set(GPIOA,GPIO_PIN_5)

#define OLED_SDIN_Clr() gpio_bit_reset(GPIOA,GPIO_PIN_7)    //DIN PA7
#define OLED_SDIN_Set() gpio_bit_set(GPIOA,GPIO_PIN_7)

#define OLED_CS_Clr()  gpio_bit_reset(GPIOB,GPIO_PIN_2)     //CS PB2
#define OLED_CS_Set()  gpio_bit_set(GPIOB,GPIO_PIN_2)
#endif /* SPI0_CFG */

#define OLED_RST_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_1)     //RES PB1
#define OLED_RST_Set() gpio_bit_set(GPIOB,GPIO_PIN_1)

#define OLED_DC_Clr() gpio_bit_reset(GPIOB,GPIO_PIN_0)      //DC PB0
#define OLED_DC_Set() gpio_bit_set(GPIOB,GPIO_PIN_0)


#if     HAS_BLK_CNTL
#define OLED_BLK_Clr()  gpio_bit_reset(GPIOA,GPIO_PIN_5)//BLK
#define OLED_BLK_Set()  gpio_bit_set(GPIOA,GPIO_PIN_5)
#else
#define OLED_BLK_Clr()
#define OLED_BLK_Set()
#endif

#define OLED_CMD  0	//Write command
#define OLED_DATA 1	//Write data


extern u32 COLOR;
extern u32 BACK_COLOR;
extern unsigned char image[12800];

/* =================== function =================== */
void LCD_Writ_Bus(u8 dat);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);
void Lcd_Init(void);
void LCD_Clear(u32 Color);
void LCD_WR_COLOR(u32 Color);

#if 0
void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color);
#endif
  		 
#endif  
	 
	 



