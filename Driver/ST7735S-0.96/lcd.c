#include "lcd.h"
#include "bmp.h"

u32 COLOR;         //color
u32 BACK_COLOR;   //Background color

/******************************************************************************
        Function description: LCD serial data write function
        Entry data: serial data to be written to dat
        Return value: None
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{
#if SPI0_CFG == 1
        OLED_CS_Clr();

        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));
        spi_i2s_data_transmit(SPI0, dat);
        while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));
        spi_i2s_data_receive(SPI0);

        OLED_CS_Set();
#elif SPI0_CFG == 2
	spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
#else
        u8 i;
        OLED_CS_Clr();
        for(i=0;i<8;i++)
        {			  
                OLED_SCLK_Clr();
                if(dat&0x80)
                        OLED_SDIN_Set();
                else
                        OLED_SDIN_Clr();
                OLED_SCLK_Set();
                dat<<=1;
        }	
  OLED_CS_Set();	
#endif
}


/******************************************************************************
        Function description: LCD write data 8bit mode
        Entry data: data written by dat
        Return value: None
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	OLED_DC_Set();          //Write data
	LCD_Writ_Bus(dat);
}


/******************************************************************************
        Function description: LCD write data 16bit mode
        Entry data: data written by dat
        Return value: None
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
        OLED_DC_Set();          //Write data
        LCD_Writ_Bus(dat>>8);
        LCD_Writ_Bus(dat);
}


/******************************************************************************
        Function description: LCD write command
        Entry data: command written by dat
        Return value: None
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
        OLED_DC_Clr();          //Write command
        LCD_Writ_Bus(dat);
}


/******************************************************************************
        Function description: Set start and end addresses
        Entry data:     x1, x2 set the start and end addresses of the column
                        y1, y2 set the start and end addresses of the line
        Return value: None
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);       //Column address settings
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);       //Row address setting
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);       //Memory write
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);       //Column address settings
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);       //Row address setting
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);       //Memory write
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);       //Column address settings
#ifndef NEWLCD
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
#else
                LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
#endif

		LCD_WR_REG(0x2b);       //Row address setting
#ifndef NEWLCD
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
#else
                LCD_WR_DATA(y1+24);
		LCD_WR_DATA(y2+24);
#endif

		LCD_WR_REG(0x2c);       //Memory write
	}
	else
	{
                LCD_WR_REG(0x2a);       //Column address settings
                LCD_WR_DATA(x1+1);
                LCD_WR_DATA(x2+1);
                LCD_WR_REG(0x2b);       //Row address setting
                LCD_WR_DATA(y1+26);
                LCD_WR_DATA(y2+26);
                LCD_WR_REG(0x2c);       //Memory write
	}
}

#if SPI0_CFG == 2
/******************************************************************************
        Function description: lcd dma spi config
        Entry data:     
        Return value: None
******************************************************************************/
void dma_config(void)
{
        dma_parameter_struct dma_init_struct;

        /* SPI0 transmit dma config:DMA0,DMA_CH2 */
        dma_deinit(DMA0, DMA_CH2);
        dma_struct_para_init(&dma_init_struct);

        dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0);
        dma_init_struct.memory_addr  = (uint32_t)image;
        dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
        dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
        dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
        dma_init_struct.priority     = DMA_PRIORITY_LOW;
        dma_init_struct.number       = FRAME_SIZE;
        dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
        dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
        dma_init(DMA0, DMA_CH2, &dma_init_struct);
        /* configure DMA mode */
        dma_circulation_disable(DMA0, DMA_CH2);
        dma_memory_to_memory_disable(DMA0, DMA_CH2);
}
#endif

#if SPI0_CFG == 1
/******************************************************************************
        Function description: configure the SPI peripheral
        Entry data:     
        Return value: None
******************************************************************************/
void spi_config(void)
{
        spi_parameter_struct spi_init_struct;
        /* deinitilize SPI and the parameters */
        OLED_CS_Set();
        spi_struct_para_init(&spi_init_struct);

        /* SPI0 parameter config */
        spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
        spi_init_struct.device_mode          = SPI_MASTER;
        spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
        spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
        spi_init_struct.nss                  = SPI_NSS_SOFT;
        spi_init_struct.prescale             = SPI_PSC_8;
        spi_init_struct.endian               = SPI_ENDIAN_MSB;
        spi_init(SPI0, &spi_init_struct);

        spi_crc_polynomial_set(SPI0,7);
        spi_enable(SPI0);
}
#endif

/******************************************************************************
        Function description: LCD initialization function
        Entry data: None
        Return value: None
******************************************************************************/
void Lcd_Init(void)
{
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);

#if SPI0_CFG == 1
        rcu_periph_clock_enable(RCU_AF);
        rcu_periph_clock_enable(RCU_SPI0);
        /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 |GPIO_PIN_6| GPIO_PIN_7);
        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

        spi_config();

#elif SPI0_CFG == 2
        rcu_periph_clock_enable(RCU_DMA0);
        rcu_periph_clock_enable(RCU_SPI0);

        /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
        /* SPI0 GPIO config: MISO/PA6 */
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

        dma_config();

        dma_channel_enable(DMA0,DMA_CH2);
#elif SPI0_CFG == 3
        gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

        gpio_bit_reset(GPIOA, GPIO_PIN_5 | GPIO_PIN_7);
        gpio_bit_reset(GPIOB, GPIO_PIN_2);
#endif

        gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
        gpio_bit_reset(GPIOB, GPIO_PIN_0 | GPIO_PIN_1);

        OLED_RST_Clr();
        delay_1ms(200);
        OLED_RST_Set();
        delay_1ms(20);
        OLED_BLK_Set();

        LCD_WR_REG(0x11);	// turn off sleep mode
        delay_1ms(100);

#ifdef NEWLCD
	LCD_WR_REG(0x20);	// display inversion mode
#else
        LCD_WR_REG(0x21);	// display inversion mode
#endif

        LCD_WR_REG(0xB1);	// Set the frame frequency of the full colors normal mode
                                                // Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
                                                // fosc = 850kHz
        LCD_WR_DATA8(0x05);	// RTNA
        LCD_WR_DATA8(0x3A);	// FPA
        LCD_WR_DATA8(0x3A);	// BPA

        LCD_WR_REG(0xB2);	// Set the frame frequency of the Idle mode
                                                // Frame rate=fosc/((RTNB x 2 + 40) x (LINE + FPB + BPB +2))
                                                // fosc = 850kHz
        LCD_WR_DATA8(0x05);	// RTNB
        LCD_WR_DATA8(0x3A);	// FPB
        LCD_WR_DATA8(0x3A);	// BPB

        LCD_WR_REG(0xB3);	// Set the frame frequency of the Partial mode/ full colors
        LCD_WR_DATA8(0x05);  
        LCD_WR_DATA8(0x3A);
        LCD_WR_DATA8(0x3A);
        LCD_WR_DATA8(0x05);
        LCD_WR_DATA8(0x3A);
        LCD_WR_DATA8(0x3A);

        LCD_WR_REG(0xB4);
        LCD_WR_DATA8(0x03);

        LCD_WR_REG(0xC0);
        LCD_WR_DATA8(0x62);
        LCD_WR_DATA8(0x02);
        LCD_WR_DATA8(0x04);

        LCD_WR_REG(0xC1);
        LCD_WR_DATA8(0xC0);

        LCD_WR_REG(0xC2);
        LCD_WR_DATA8(0x0D);
        LCD_WR_DATA8(0x00);

        LCD_WR_REG(0xC3);
        LCD_WR_DATA8(0x8D);
        LCD_WR_DATA8(0x6A);   

        LCD_WR_REG(0xC4);
        LCD_WR_DATA8(0x8D); 
        LCD_WR_DATA8(0xEE); 

        LCD_WR_REG(0xC5);  /*VCOM*/
        LCD_WR_DATA8(0x0E);    

        LCD_WR_REG(0xE0);
        LCD_WR_DATA8(0x10);
        LCD_WR_DATA8(0x0E);
        LCD_WR_DATA8(0x02);
        LCD_WR_DATA8(0x03);
        LCD_WR_DATA8(0x0E);
        LCD_WR_DATA8(0x07);
        LCD_WR_DATA8(0x02);
        LCD_WR_DATA8(0x07);
        LCD_WR_DATA8(0x0A);
        LCD_WR_DATA8(0x12);
        LCD_WR_DATA8(0x27);
        LCD_WR_DATA8(0x37);
        LCD_WR_DATA8(0x00);
        LCD_WR_DATA8(0x0D);
        LCD_WR_DATA8(0x0E);
        LCD_WR_DATA8(0x10);

        LCD_WR_REG(0xE1);
        LCD_WR_DATA8(0x10);
        LCD_WR_DATA8(0x0E);
        LCD_WR_DATA8(0x03);
        LCD_WR_DATA8(0x03);
        LCD_WR_DATA8(0x0F);
        LCD_WR_DATA8(0x06);
        LCD_WR_DATA8(0x02);
        LCD_WR_DATA8(0x08);
        LCD_WR_DATA8(0x0A);
        LCD_WR_DATA8(0x13);
        LCD_WR_DATA8(0x26);
        LCD_WR_DATA8(0x36);
        LCD_WR_DATA8(0x00);
        LCD_WR_DATA8(0x0D);
        LCD_WR_DATA8(0x0E);
        LCD_WR_DATA8(0x10);

        LCD_WR_REG(0x3A);	// define the format of RGB picture data

#if     MODE_16_18
        LCD_WR_DATA8(0x06);	// 18-bit/pixel
#else
        LCD_WR_DATA8(0x05);	// 16-bit/pixel
#endif

	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);

	LCD_WR_REG(0x29);	// Display On
}


/******************************************************************************
        Function description: LCD write color data function
        Entry data: None
        Return value: None
******************************************************************************/
void LCD_WR_COLOR(u32 Color)
{
#if MODE_16_18  //18bit
//        LCD_WR_DATA8(Color>>18  &0x3f);
//        LCD_WR_DATA8(Color>>10  &0x3f);
//        LCD_WR_DATA8(Color>>2   &0x3f);
        
        LCD_WR_DATA8(Color>>16  &0xff);
        LCD_WR_DATA8(Color>>8   &0xff);
        LCD_WR_DATA8(Color>>0   &0xff);

#else           //16bit
        LCD_WR_DATA8(Color>>16&0xf8 | Color>>13&0x7);
        LCD_WR_DATA8(Color>>10&0x7  | Color>>3&0x1f);
#endif
}

/******************************************************************************
        Function description: LCD clear screen function
        Entry data: None
        Return value: None
******************************************************************************/
void LCD_Clear(u32 Color)
{
        u16 i,j;  	
        LCD_Address_Set(0,0,LCD_W-1,LCD_H-1);
        for(i=0;i<LCD_W;i++)
        {
                for (j=0;j<LCD_H;j++)
                {
                        LCD_WR_COLOR(Color);
                }
        }
}


#if 0

/******************************************************************************
        Function description: LCD display Chinese characters
        Entry data:     x, y starting coordinates
                        index Chinese character number
                        size font size
        Return value: None
******************************************************************************/

void LCD_ShowChinese(u16 x,u16 y,u8 index,u8 size,u16 color)	
{  
        u8 i,j;
        u8 *temp,size1;
        if(size==16){temp=Hzk16;}//选择字号
        if(size==32){temp=Hzk32;}
        LCD_Address_Set(x,y,x+size-1,y+size-1); //设置一个汉字的区域
        size1=size*size/8;//一个汉字所占的字节
        temp+=index*size1;//写入的起始位
        {
                for(i=0;i<8;i++)
                {
                        if((*temp&(1<<i))!=0)//从数据的低位开始读
                        {
                                LCD_WR_DATA(color);//点亮
                        }
                        else
                        {
                                LCD_WR_DATA(BACK_COLOR);//不点
                        }
                        temp++;
                }
        }
}

#endif

