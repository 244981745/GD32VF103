#include "Hardware/lcd.h"
#include "Hardware/oledfont.h"
#include "graphics.h"

extern u32 COLOR;
extern u32 BACK_COLOR;

/******************************************************************************
        Function description: LCD Set Color
        Entry data: color
        Return value: None
******************************************************************************/
void LCD_SetColor(u32 color)
{
        COLOR           =       color;
}


/******************************************************************************
        Function description: LCD Set Back Color
        Entry data: color
        Return value: None
******************************************************************************/
void LCD_SetBackColor(u32 color)
{
        BACK_COLOR      =       color;
}

/******************************************************************************
        Function description: LCD draws point
        Entry data: x, y starting coordinates
        Return value: None
******************************************************************************/
void LCD_DrawPoint(int x, int y, u32 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置 
	LCD_WR_COLOR(color);
}


/******************************************************************************
        Function description: display characters
        Entry data:     x, y starting point coordinates
                        num characters to display
                        mode 1 superimposed mode 0 non-superimposed mode
        Return value: None
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 mode,u32 color)
{
        u8 temp;
        u8 pos,t;
        u16 x0=x;    
        if(x>LCD_W-16||y>LCD_H-16)      return;	    //Settings window		   
        num=num-' ';//Get offset value

        LCD_Address_Set(x,y,x+8-1,y+16-1);      //Set cursor position
        if(!mode) //Non-overlapping
        {
		for(pos=0;pos<16;pos++)
		{ 
			temp=asc2_1608[(u16)num*16+pos];		 //Call 1608 font
                        for(t=0;t<8;t++)
                        {
#if MODE_16_18 == 0
                                if(temp&0x01)LCD_WR_DATA(color);
                                else LCD_WR_DATA(BACK_COLOR);
#else
                                if((temp>>t)&0x1)
                                {
                                        LCD_WR_DATA8(color>>16&0xff);
                                        LCD_WR_DATA8(color>>8&0xff);
                                        LCD_WR_DATA8(color&0xff);
                                }
                                else
                                {
                                        LCD_WR_DATA8(BACK_COLOR>>16&0xff);
                                        LCD_WR_DATA8(BACK_COLOR>>8&0xff);
                                        LCD_WR_DATA8(BACK_COLOR&0xff);
                                }
#endif
                                x++;
                      }
                        x=x0;
                        y++;
		}	
	}else//overlapping mode
	{
		for(pos=0;pos<16;pos++)
		{
		    temp=asc2_1608[(u16)num*16+pos];		 //Call 1608 font
			for(t=0;t<8;t++)
		    {                 
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos,color);//Draw a dot   
		        temp>>=1; 
		    }
		}
	}   	   	 	  
}


/******************************************************************************
        Function description: display string
        Entry data:     x, y starting point coordinates
                        *p string start address
        Return value: None
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u32 color)
{         
    while(*p!='\0')
    {       
        if(x>LCD_W-16){x=0;y+=16;}
        if(y>LCD_H-16){y=x=0;LCD_Clear(RED);}
        LCD_ShowChar(x,y,*p,0,color);
        x+=8;
        p++;
    }
}


/******************************************************************************
        Function description: draw a line
        Entry data:     x1, y1 starting coordinates
                        x2, y2 terminating coordinates
        Return value: None
******************************************************************************/
void LCD_DrawLine(int x0, int y0, int x1, int y1, u32 color)
{
        int i;

        if(x0 >= LCD_W)  return ;
        if(y0 >= LCD_H)  return ;
        
        if(x0 < 0)      x0 = 0;
        if(y0 < 0)      y0 = 0;
        if(x1 >= LCD_W) x1 = LCD_W-1;
        if(y1 >= LCD_H) y1 = LCD_H-1;

        LCD_Address_Set(x0,y0,x1,y1);
        for(i = 0; i < 320; i++){
                LCD_WR_COLOR(color);
        }
}


/******************************************************************************
        Function description: draw a line [dda]
        Entry data:     x1, y1 starting coordinates
                        x2, y2 terminating coordinates
        Return value: None
******************************************************************************/
void LCD_DDA_Line(int x0, int y0, int x1, int y1, u32 color)
{
        int dx,dy,step;
        dx = x1 - x0;
        dy = y1 - y0;
        
        step = dx < dy ? dx : dy;
        if(dx > dy)     //step = x
        {
                for(step = x0; step < x1; step++)
                {
                        LCD_DrawPoint(x0+step, y0+(dy/dx), color);
                }
        }
        else            //step = y
        {
                for(step = y0; step < y1; step++)
                {
                        LCD_DrawPoint(x0+(dx/dy), y0+step, color);
                }
        }
}


/******************************************************************************
        Function description: draw a rectangle
        Entry data:     x1, y1 starting coordinates
                        x2, y2 terminating coordinates
        Return value: None
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u32 color)
{
        u16 x_start,x_end,y_start,y_end;
        x_start =       x1 < x2 ? x1 : x2;
        x_end   =       x1 > x2 ? x1 : x2;
        y_start =       y1 < y2 ? y1 : y2;
        y_end   =       y1 > y2 ? y1 : y2;

        LCD_DrawLine(x_start, y_start, x_end,   y_start, color);
	LCD_DrawLine(x_start, y_start, x_start, y_end,   color);
	LCD_DrawLine(x_start, y_end,   x_end,   y_end,   color);
	LCD_DrawLine(x_end,   y_start, x_end,   y_end,   color);
}

/******************************************************************************
        Function description: draw circle
        Entry data:     x0, y0 center coordinates
                        r radius
        Return value: None
******************************************************************************/
void LCD_DrawCircle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	// int di;
	a=0;b=r;
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//Determine whether the points to be drawn are too far away
		{
                        b--;
		}
	}
}

/******************************************************************************
        Function description: fill color in the specified area
        Entry data:     xsta, ysta starting coordinates
                        xend, yend termination coordinates
        Return value: None
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u32 color)
{          
	u16 i,j; 
	LCD_Address_Set(xsta,ysta,xend,yend);      //设置光标位置 
	for(i=ysta;i<=yend;i++)
	{													   	 	
		for(j=xsta;j<=xend;j++)LCD_WR_COLOR(color);
	} 					  	    
}

/******************************************************************************
        Function description: display 160x80 18bit (RGB666) / 16bit (RGB565) picture
        Entry data: x, y starting point coordinates
        Return value: None
******************************************************************************/
void LCD_DrawPicture(u16 x1,u16 y1,u16 x2,u16 y2,unsigned char *source)
{
	int i;
	LCD_Address_Set(x1,y1,x2,y2);
	for(i=0;i<12800;i++)
	{
#if     MODE_16_18      //18bit
		LCD_WR_DATA8(source[i*3+0]);
                LCD_WR_DATA8(source[i*3+1]);
                LCD_WR_DATA8(source[i*3+2]);
#else                   //16bit
                LCD_WR_DATA8(source[i*3+0]<<2&0xf8 | source[i*3+1]>>3 &0x7);
                LCD_WR_DATA8(source[i*3+1]<<5&0xe0 | source[i*3+2]>>1 &0x1f);
#endif
	}
}