#ifndef __MAIN_SIG_GEN_H__
#define __MAIN_SIG_GEN_H__

#define SIGGEN_WINDOW_SPACE			0
#define SIGGEN_VSPACE_BW_BUTTON		9
#define SIGGEN_HSPACE_BW_BUTTON		10
#define SIGGEN_LONG_BUTTON_WIDTH 	160
#define SIGGEN_LONG_BUTTON_HEIGHT	30
#define SIGGEN_MID_BUTTON_WIDTH 	70
#define SIGGEN_MID_BUTTON_HEIGHT	30
#define SIGGEN_SMALL_BUTTON_WIDTH 	43
#define SIGGEN_SMALL_BUTTON_HEIGHT	30

#define LCD_START_X					0
#define LCD_END_X					160
#define LCD_START_Y					0
#define LCD_END_Y					128

#define WAVE_TYPE_SINE				0
#define WAVE_TYPE_TRI				1
#define WAVE_TYPE_SQUARE			2

#define FREQ_VALUE_START_X	15
#define FREQ_VALUE_START_Y	32

#define FREQ_UNIT_START_X	40
#define FREQ_UNIT_START_Y	32

#define FREQ_WAVE_START_X	110
#define FREQ_WAVE_START_Y	32

#define AMP_START_Y			64



void dds_init (void);


#endif