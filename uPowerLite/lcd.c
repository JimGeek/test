#include "lcd.h"
#include "font.h"
void X_Y_Setting();
void delay(unsigned int i);
int LCD_draw_round_corner_box ( int Hstart, int Vstart, int Hend, int Vend,
                                int radius, int color, int fill);

void Send_Command(unsigned char cmd)
{
	rs=0;	     // set the command line low,
	rs=0;
	cs=0;        // set the chip select low,
	cs=0;
	wr=0;
	wr=0;		 // set the write line low,
	PORTA=cmd;	//PORTB=j&0xFFEF;
	delay(4);
	wr=1;
	wr=1;
	delay(2);
	cs=1;	    // Set the chip select high
	cs=1;	
	rs=1;
	rs=1;
}
void Send_Data(unsigned char dat)
{
	rs=1;        // set the command line high,
	rs=1; 
	cs=0;        // set the chip select low,
	cs=0;        // set the chip select low,
	wr=0;		  // set the write line low,
	wr=0;	
	PORTA=dat;	//PORTB=j&0xffef;//(i|(PORTB&0x807F));
	delay(4);
	wr=1;
	wr=1;
	delay(2);
  	cs=1;
	cs=1;
}


void LCD_init()
{

	rs = 1;
	wr=1;
	cs=1;
	PORTA=0xffff;
    Send_Command(LCD_RESET);
    delay(500);
	Send_Command(LCD_WAKEUP);
    delay(500);
	Send_Command(MEM_ACCESS);
	Send_Data(0xa0);

//	Send_Command(LCD_RESET);
//	Send_Command(LCD_WAKEUP);

	Send_Command(LCD_SET16BPP);	//set pixel format
	Send_Data(0x75);

	Send_Command(0x26);			//gamma settings
	Send_Data(0x04);

	Send_Command(0xc0);			//power settings
	Send_Data(0x05);
	Send_Data(0x05);

	Send_Command(0xc1);
	Send_Data(0x07);

	Send_Command(0xc2);
	Send_Data(0x05);
	Send_Data(0x02);

	Send_Command(0xc3);
	Send_Data(0x05);
	Send_Data(0x02);

	Send_Command(0xc4);
	Send_Data(0x05);
	Send_Data(0x02);

//	Send_Command(0xc5);
//	Send_Data(0xd0);
//
//	Send_Command(0xc6);
//	Send_Data(0x15);
	Send_Command(0x36);
	Send_Data(0xa0);

	X_Y_Setting();
	Send_Command(LCD_ON);
//	Send_Command(LCD_MEM_WRITE);
//	Send_Command(LCD_ON);

}
//-------------------------------------------------------------------------
// X CO-ORDINATES
//-------------------------------------------------------------------------
void X_Y_Setting()
{
	Send_Command(LCD_COLUMNSET);
	Send_Data(0x00);
	Send_Data(0x01);
	Send_Data(0x00);
	Send_Data(0x80);

	Send_Command(LCD_ROWSET);
	Send_Data(0x00);
	Send_Data(0x01);
	Send_Data(0x00);
	Send_Data(0xa0);
}



/******************************************************************
*  Function: LCD_paint_block
*
*  Purpose: Paints a block of the screen the specified color.
*           Note: works with two pixels at a time to maintain high
*           bandwidth.  Therefore, corner points must be even
*           numbered coordinates.  Use LCD_draw_solid_box() for
*           areas with odd-numbered corner points.
*           The color parameter must contain two pixel's worth
*           (32 bits).
*
******************************************************************/
void LCD_paint_block (int Hstart,int Vstart, int Hend, int Vend, int color)
{

    unsigned int i,count=((Hend-Hstart)+1)*((Vend-Vstart)+1)*2;

    //Set X co-orinate
	//Hstart+=1;
	//Vstart+=2;
	//Hend+=1;
	//Vend+=2;
    Send_Command(0x2A);
    Send_Data((Hstart & 0xFF00)>>8);//upper
    Send_Data(Hstart & 0xFF);//lower
   	Send_Data((Hend & 0xFF00)>>8);//upper
    Send_Data(Hend & 0xFF);//lower

    //Set Y co-orinate


	Send_Command(0x2B);
	Send_Data((Vstart & 0xFF00)>>8);//upper
    Send_Data(Vstart & 0xFF);//lower
    Send_Data((Vend & 0xFF00)>>8);//upper
    Send_Data(Vend & 0xFF);//lower

    //start to write memory

    Send_Command(0x2C);

    for (i = 0; i <count; i+=2)
	{
	  Send_Data(color >> 8);
      Send_Data(color);
    }
}
void LCD_putpixel(unsigned int x, unsigned int y, unsigned int color1)//, unsigned char color2)
{
	int i, temp;
	unsigned char c1, c2;

	c1 = (color1 & 0xFF00) >> 8;
	c2 = color1;

	//Set X co-orinate
	Send_Command(LCD_COLUMNSET);
	Send_Data((x & 0xFF00) >> 8);
	Send_Data(x);

	//Set Y co-orinate
	Send_Command(LCD_ROWSET);
	Send_Data((y & 0xFF00) >> 8);
	Send_Data(y);

	//start to write memory
	Send_Command(LCD_MEM_WRITE);
	Send_Data(c1);
	Send_Data(c2);
}
void LCD_horizontalline (short Hstart, short Hend, int V, int color)
{
    unsigned int i,count=(Hend-Hstart)*2;
	int temp;
	unsigned char c1, c2;

	c1 = (color & 0xFF00) >> 8;
	c2 = color;

	//Set X co-orinate
	Send_Command(LCD_COLUMNSET);

	Send_Data((Hstart & 0xFF00) >> 8);
	Send_Data(Hstart);
	Send_Data((Hend & 0xFF00) >> 8);
	Send_Data(Hend);

    //Set Y co-orinate
	Send_Command(LCD_ROWSET);

	Send_Data((V & 0xFF00) >> 8);
	Send_Data(V);

    //start to write memory
    Send_Command(LCD_MEM_WRITE);

    for (i = 0; i <count; i+=2)
    {
		Send_Data(c1);
		Send_Data(c2);
    }
}

/******************************************************************
*  LCD_verticalline
******************************************************************/
void LCD_verticalline (unsigned int x, unsigned int y1, unsigned int y2, unsigned int color)
{
    unsigned int i;
    for (i = y1; i <y2; i++)
        LCD_putpixel(x,i,color);
}

/******************************************************************
*  LCD_FillRect
******************************************************************/
void LCD_FillRect (int Hstart,int Vstart, int Hend, int Vend, int color)
{
    unsigned int i,count=((Hend-Hstart)+1)*((Vend-Vstart)+1)*2;
    unsigned char c1, c2;

	c1 = (color & 0xFF00) >> 8;
	c2 = color;
    //Set X co-orinate
	Send_Command(LCD_COLUMNSET);

	Send_Data((Hstart & 0xFF00) >> 8);
	Send_Data(Hstart);
	Send_Data((Hend & 0xFF00) >> 8);
	Send_Data(Hend);

    //Set Y co-orinate
    Send_Command(LCD_ROWSET);

	Send_Data((Vstart & 0xFF00) >> 8);
	Send_Data(Vstart);
    Send_Data((Vend & 0xFF00) >> 8);
	Send_Data(Vend);

    //start to write memory
    Send_Command(LCD_MEM_WRITE);
    for (i = 0; i <count; i+=2)
    {
        Send_Data(c1);
		Send_Data(c2);
    }
}
/******************************************************************
*  LCD_Rectangle
******************************************************************/
void LCD_Rectangle (int x1, int y1, int x2, int y2, int color, int fill)
{
    //If we want to fill in our box
    x1+=1;
	y1+=2;
	x2+=1;
	y2+=2;
    if (fill)
        LCD_FillRect (x1, y1, x2, y2, color);
    else
    {
        //If we're not filling in the box, just draw four lines.
        LCD_horizontalline(x1, x2, y1, color);
        LCD_verticalline(x1,y1, y2, color);
        LCD_horizontalline(x1, x2, y2, color);
        LCD_verticalline(x2, y1,y2, color);
    }
}
void LCD_image (int Hstart,int Vstart, int Hend, int Vend, unsigned char rom *image)
{

	
    unsigned int i,count=((Hend-Hstart)+1)*((Vend-Vstart)+1)*2;
	Hstart+=1;
	Vstart+=2;
	Hend+=1;
	Vend+=2;

    //Set X co-orinate
	
    Send_Command(0x2A);
    Send_Data((Hstart & 0xFF00)>>8);//upper
    Send_Data(Hstart & 0xFF);//lower
   	Send_Data((Hend & 0xFF00)>>8);//upper
    Send_Data(Hend & 0xFF);//lower

    //Set Y co-orinate

    
	Send_Command(0x2B);
	Send_Data((Vstart & 0xFF00)>>8);//upper
    Send_Data(Vstart & 0xFF);//lower
    Send_Data((Vend & 0xFF00)>>8);//upper
    Send_Data(Vend & 0xFF);//lower

    //start to write memory

    Send_Command(0x2C);

    for (i = 0; i <count; i+=1)
	{	
	  //Send_Data(*image >> 8);
      Send_Data(*image);
	  image++;
    }
	
}

void LCD_bitmap (unsigned int x, unsigned int y,unsigned char rom *bitmap ,unsigned int color)
{

    unsigned char raw,column,pixel=0,byte,k;
    x+=1;
    y+=2;
	for (raw=0;raw<32;raw++)
	{
		for(byte=0;byte<4;byte++,bitmap++)
		{
			for(column=8,k=0x01;column>0;column--,k=k<<1)
			{

				if((*bitmap & k)==k)
				{
					LCD_putpixel(x+column+(byte*8),y+raw,color);

				}
			}
		}
	}

}
/******************************************************************
*  Function: LCD_draw_sloped_line
*  Purpose: Draws a line between two end points using
*           Bresenham's line drawing algorithm.
*           width parameter is not used.
*           It is reserved for future use.
*
******************************************************************/
void LCD_draw_sloped_line( unsigned short horiz_start,
                           unsigned short vert_start,
                           unsigned short horiz_end,
                           unsigned short vert_end,
                           unsigned short width,
                           int color)
{
  // Find the vertical and horizontal distance between the two points
  int horiz_delta = horiz_end-horiz_start;
  int vert_delta = vert_end-vert_start;

  // Find out what direction we are going
  int horiz_incr, vert_incr;
  if (horiz_start > horiz_end) { horiz_incr=-1; } else { horiz_incr=1; }
  if (vert_start > vert_end) { vert_incr=-1; } else { vert_incr=1; }

  // Find out which axis is always incremented when drawing the line
  // If it's the horizontal axis
  if (horiz_delta >= vert_delta) {
    int dPr   = vert_delta<<1;
    int dPru  = dPr - (horiz_delta<<1);
    int P     = dPr - horiz_delta;

    // Process the line, one horizontal point at at time
    for (; horiz_delta >= 0; horiz_delta--) {
      // plot the pixel
      LCD_putpixel(horiz_start, vert_start, color);
      // If we're moving both up and right
      if (P > 0) {
        horiz_start+=horiz_incr;
        vert_start+=vert_incr;
        P+=dPru;
      } else {
        horiz_start+=horiz_incr;
        P+=dPr;
      }
    }
  // If it's the vertical axis
  } else {
    int dPr   = horiz_delta<<1;
    int dPru  = dPr - (vert_delta<<1);
    int P     = dPr - vert_delta;

    // Process the line, one vertical point at at time
    for (; vert_delta>=0; vert_delta--) {
      // plot the pixel
      LCD_putpixel(horiz_start, vert_start, color);
      // If we're moving both up and right
      if (P > 0) {
        horiz_start+=horiz_incr;
        vert_start+=vert_incr;
        P+=dPru;
      } else {
        vert_start+=vert_incr;
        P+=dPr;
      }
    }
  }
}



/******************************************************************
*  Function: _draw_line
*
*  Purpose: Draws a line between two end points. First checks
*           to see if the line is horizontal.  If it is, it calls
*           LCD_horizontalline(), which is much faster than
*           LCD_draw_sloped_line.
*
******************************************************************/
void LCD_draw_line(int horiz_start, int vert_start, int horiz_end, int vert_end, int width, int color)
{

  if( vert_start == vert_end )
  {
    LCD_horizontalline( (unsigned short)horiz_start,
                         (unsigned short)horiz_end,
                         (unsigned short)vert_start,
                         color );
  }
  else
  {
    LCD_draw_sloped_line( (unsigned short)horiz_start,
                          (unsigned short)vert_start,
                          (unsigned short)horiz_end,
                          (unsigned short)vert_end,
                          (unsigned short)width,
                          color );
  }
}

void LCD_round_corner_points( int cx, int cy, int x, int y,
                              int straight_width, int straight_height, int color,
                              char fill);
/******************************************************************
*  Function: fbvga16_draw_round_corner_box
*
*  Purpose: Draws a box on the screen with the specified corner
*  points.  The fill parameter tells the function whether or not
*  to fill in the box.  1 = fill, 0 = do not fill.
*
******************************************************************/
int LCD_draw_round_corner_box ( int Hstart, int Vstart, int Hend, int Vend,
                                int radius, int color, int fill)
{
  unsigned int x, y;
  int p;
  int diameter;
  int temp;
  unsigned int width, height, straight_width, straight_height;
	
	Hstart+=1;
	Vstart+=2;
	Hend+=1;
	Vend+=2;
  // Make sure the start point us up and left of the end point
  if( Hstart > Hend )
  {
    temp = Hend;
    Hend = Hstart;
    Hstart = temp;
  }

  if( Vstart > Vend )
  {
    temp = Vend;
    Vend = Vstart;
    Vstart = temp;
  }

  // These are the overall dimensions of the box
  width = Hend - Hstart;
  height = Vend - Vstart;

  // Make sure our radius isnt more than the shortest dimension
  // of the box, or it'll screw us all up
  if( radius > ( width / 2 ))
    radius = width / 2;

  if( radius > ( height / 2 ))
    radius = height / 2;

  // We use the diameter for some calculations, so we'll pre calculate it here.
  diameter = ( radius * 2 );

  // These are the lengths of the straight portions of the box edges.
  straight_width = width - diameter;
  straight_height = height - diameter;

  x = 0;
  y = radius;
  p = (5 - radius*4)/4;

  // Start the corners with the top, bottom, left, and right pixels.
  LCD_round_corner_points( Hstart + radius, Vstart + radius, x, y,
                           straight_width, straight_height, color, fill );

  // Now start moving out from those points until the lines meet
  while (x < y) {
    x++;
    if (p < 0) {
      p += 2*x+1;
    } else {
      y--;
      p += 2*(x-y)+1;
    }
    LCD_round_corner_points( Hstart + radius, Vstart + radius, x, y,
                             straight_width, straight_height, color, fill);
  }

  // If we want to fill in our box
  if (fill) {
     LCD_paint_block (Hstart, Vstart + radius, Hend, Vend - radius, color);
  // If we're not filling in the box, just draw four lines.
  } else {
    LCD_draw_line(Hstart, Vstart + radius, Hstart, Vend - radius , 1, color); //left
    LCD_draw_line(Hend, Vstart + radius, Hend, Vend - radius , 1, color); //right
    LCD_draw_line(Hstart + radius, Vstart, Hend - radius , Vstart, 1, color); //top
    LCD_draw_line(Hstart + radius, Vend, Hend - radius , Vend, 1, color); //bottom
  }

  return (0);
}


/******************************************************************
*  Function: LCD_round_corner_points
*
*  Purpose: Called by LCD_draw_round_corner_box() and
*  LCD_draw_circle() to plot the actual points of the round corners.
*  Draws horizontal lines to fill the shape.
*
******************************************************************/

void LCD_round_corner_points( int cx, int cy, int x, int y,
                              int straight_width, int straight_height, int color,
                              char fill)
{

    // If we're directly above, below, left and right of center (0 degrees), plot those 4 pixels
    if (x == 0) {
        // bottom
        LCD_putpixel(cx, cy + y + straight_height, color);
        LCD_putpixel(cx + straight_width, cy + y + straight_height, color);
        // top
        LCD_putpixel(cx, cy - y, color);
        LCD_putpixel(cx + straight_width, cy - y, color);

        if(fill) {
          LCD_draw_line(cx - y, cy, cx + y + straight_width, cy, 1, color);
          LCD_draw_line(cx - y, cy + straight_height, cx + y + straight_width, cy + straight_height, 1, color);
        } else {
          //right
          LCD_putpixel(cx + y + straight_width, cy, color);
          LCD_putpixel(cx + y + straight_width, cy + straight_height, color);
          //left
          LCD_putpixel(cx - y, cy, color);
          LCD_putpixel(cx - y, cy + straight_height, color);
        }

    } else
    // If we've reached the 45 degree points (x=y), plot those 4 pixels
    if (x == y) {
      if(fill) {
        LCD_draw_line(cx - x, cy + y + straight_height, cx + x + straight_width, cy + y + straight_height, 1, color); // lower
        LCD_draw_line(cx - x, cy - y, cx + x + straight_width, cy - y, 1, color); // upper

      } else {
        LCD_putpixel(cx + x + straight_width, cy + y + straight_height, color); // bottom right
        LCD_putpixel(cx - x, cy + y + straight_height, color); // bottom left
        LCD_putpixel(cx + x + straight_width, cy - y, color); // top right
        LCD_putpixel(cx - x, cy - y, color); // top left
      }
    } else
    // If we're between 0 and 45 degrees plot 8 pixels.
    if (x < y) {
        if(fill) {
          LCD_draw_line(cx - x, cy + y + straight_height, cx + x + straight_width, cy + y + straight_height, 1, color);
          LCD_draw_line(cx - y, cy + x + straight_height, cx + y + straight_width, cy + x + straight_height, 1, color);
          LCD_draw_line(cx - y, cy - x, cx + y + straight_width, cy - x, 1, color);
          LCD_draw_line(cx - x, cy - y, cx + x + straight_width, cy - y, 1, color);
        } else {
          LCD_putpixel(cx + x + straight_width, cy + y + straight_height, color);
          LCD_putpixel(cx - x, cy + y + straight_height, color);
          LCD_putpixel(cx + x + straight_width, cy - y, color);
          LCD_putpixel(cx - x, cy - y, color);
          LCD_putpixel(cx + y + straight_width, cy + x + straight_height, color);
          LCD_putpixel(cx - y, cy + x + straight_height, color);
          LCD_putpixel(cx + y + straight_width, cy - x, color);
          LCD_putpixel(cx - y, cy - x, color);
        }
    }
}
/******************************************************************
*  Function: LCD_OutChar
*
*  Purpose: Prints a character to the specified location of the
*           screen using the specified font and color.
*
******************************************************************/
void LCD_OutChar(unsigned int *x ,unsigned int *y,char ch,unsigned int color)
{
	glyph_table*    pChTable;
	unsigned rom char*  pChImage;
	unsigned short  chWidth;
	unsigned short  xCnt, yCnt,x1,y1;
	unsigned char   temp=0, mask;

	if((unsigned char)ch<(unsigned rom char)_fontFirstChar)
	    return;
	if((unsigned char)ch>(unsigned rom char)_fontLastChar)
	   return;

	pChTable = (glyph_table*)( ((font_image*)_font)->address+sizeof(font_header) ) + ((unsigned char)ch-(unsigned rom char)_fontFirstChar);

	pChImage = ( unsigned rom char*)( ((font_image*)_font)->address + pChTable->offsetLSB );

	chWidth = pChTable->width;

	x1 = *x;
	y1 = *y;

	for(yCnt=0; yCnt<_fontHeight; yCnt++)
	{
	  mask = 0;
	  x1 = *x;
	  for(xCnt=0; xCnt<chWidth; xCnt++)
	  {

	      if(mask == 0){
	          temp = *pChImage++;
	          mask = 0x80;
	      }
	      if(temp&mask)
		  {
	          LCD_putpixel(x1,y1,color);
	      }
	      x1++;
	      mask >>= 1;
	  }
	  y1++;
	}
	*x = x1;

}
/******************************************************************
*  Function: LCD_OutText
*
*  Purpose: Prints a string to the specified location of the screen
*           using the specified font and color.
*           Calls LCD_print_char
*
******************************************************************/
void LCD_OutText(unsigned int x,unsigned int y,char* textString,unsigned int color)
{
	unsigned int counter = 0,Xpos=x;
	x+=1;
	y+=2;

	while(textString[counter])
	{
		if(textString[counter]=='\n')
		{
			Xpos = x;
			y +=_fontHeight;
			counter++;
			continue;
		}
		LCD_OutChar(&Xpos,&y,(unsigned char )textString[counter],color);
		counter++;
	}
	return ;
}

