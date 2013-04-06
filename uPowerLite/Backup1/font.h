extern struct font_image *font_Calibri_10 ;
extern struct font_image *font_Calibri_26 ;
//extern struct font_image *font_Calibri_26s ;
extern struct font_image *font_Calibri_26V ;

extern const unsigned rom char L30929[];

extern const unsigned rom char L13605[];

extern const unsigned rom char L981[];

extern const unsigned rom char L18982[];

void setfont(void * font);

#ifndef __FONT_H__
#define __FONT_H__

/*********************************************************************
* Overview: Structure describing the font header.
*
*********************************************************************/
typedef rom struct {
    unsigned rom char   info;		// Reserved for future use (must be set to 0).
    unsigned rom char   fontID;		// User assigned value
    unsigned rom short  firstChar;	// Character code of first character (e.g. 32).
    unsigned rom short  lastChar;	// Character code of last character in font (e.g. 3006).
    unsigned rom char   reserved;	// Reserved for future use (must be set to 0).
    unsigned rom char   height;		// Font characters height in pixels.
} font_header;

/**** Structure describing font glyph entry****/
typedef rom struct {
  unsigned rom char  offsetMSB;
  unsigned rom char  width;
  unsigned rom short offsetLSB;
} glyph_table;

/****** First and last characters in font*******/

extern unsigned short   _fontFirstChar;          // First character in the font table.
extern unsigned short   _fontLastChar;           // Last character in the font table.
extern unsigned short   _fontHeight;             // Installed font height
/*********************************************************************
* Overview: Structure for font stored in FLASH memory.
*
*********************************************************************/
typedef struct {
	unsigned int type;			// must be FLASH
	const rom char* address;	// font image address
}font_image;

extern void * _font;

#endif //__FONT_H__