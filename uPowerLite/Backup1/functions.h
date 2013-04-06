
void LCD_init(void);
void font_init(void);
void LCD_putpixel(unsigned int x, unsigned int y, unsigned int color1);
void LCD_OutChar(unsigned int *x ,unsigned int *y,char ch,unsigned int color);
void LCD_OutText(unsigned int x,unsigned int y,char* textString,unsigned int color);
void delay(unsigned int i);
void LCD_image (int Hstart,int Vstart, int Hend, int Vend, unsigned char rom *image);
//void LCD_bitmap (int Hstart,int Vstart, int Hend, int Vend, unsigned char rom *image);
void LCD_paint_block (int Hstart,int Vstart, int Hend, int Vend, int color);
void LCD_Rectangle (int x1, int y1, int x2, int y2, int color, int fill);
void LCD_bitmap (unsigned int x, unsigned int y,unsigned char rom *bitmap ,unsigned int color);

void DisplayVoltage(void);
void DisplayOkMsg(void)	;
void DisplayVtg(void);








