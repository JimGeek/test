
void LCD_init(void);
void font_init(void);
void LCD_putpixel(unsigned int x, unsigned int y, unsigned int color1);
void LCD_OutChar(unsigned int *x ,unsigned int *y,char ch,unsigned int color);
void delay(unsigned int i);
void LCD_image (int Hstart,int Vstart, int Hend, int Vend, unsigned char rom *image);
//void LCD_bitmap (int Hstart,int Vstart, int Hend, int Vend, unsigned char rom *image);
void LCD_paint_block (int Hstart,int Vstart, int Hend, int Vend, int color);
void LCD_Rectangle (int x1, int y1, int x2, int y2, int color, int fill);
void LCD_FillRect (int Hstart,int Vstart, int Hend, int Vend, int color);
void DrawDCSymbol(int Hstart, int Vstart, int color);
void LCD_bitmap (unsigned int x, unsigned int y,unsigned char rom *bitmap ,unsigned int color);
void LCD_OutText(unsigned int x,unsigned int y,unsigned char* textString,unsigned int color);
//void LCD_OutChar(unsigned int *x ,unsigned int *y,unsigned char ch,unsigned int color);
void setupkey(void);
void setup_key(unsigned char);
#if0
void setup_right(void);
void setup_left(void);
void setup_ok(void);
void setup_esc(void);
void setup_up(void);
void setup_down(void);
void setup_menu(void);
#endif
unsigned int CapSense(void);
void Fsm(unsigned char key);
void DisplayVtg(void);
void DisplaySettings(void);
void DisplayLocked(void);

void DisplayPassword(void);
void DisplayPass(void);
void DisplayLock(void);
void DisplayMax(void);
void DisplayVoltage(void);
void DisplaySettingsIndex(unsigned char index);
void enable_relay(unsigned char main_relay_no,unsigned char fine_relay_no);
void SPI_Config(void);
int spi_send(unsigned char data,unsigned char slave_sel);
void DisplayNewPassword(void);
void WrongPassword();
void LCD_draw_line(int horiz_start, int vert_start, int horiz_end, int vert_end, int width, int color);
int LCD_draw_round_corner_box ( int horiz_start, int vert_start, int horiz_end, int vert_end,
                                int radius, int color, int fill);
void WrongPassword();
void DisplayUpMsg(void);
void enable_DAC(unsigned int,unsigned char);
void DisplayOkMsg(void);
void delay_ms(unsigned int ms);
void Timer1On();
void DisplayOverload(void);
void ConfigADC_status(void);
//void ConfigADC_Iac(void);
//unsigned int Read_Iac();
//void ConfigADC_Vac();
//void DisplayV();
void DisplayWarning(void);
void Transmit(void);
void DisplayThermalShutdown(void);
void DisplayMode(void);
void DisplayModeChange(void);
void configPWM(unsigned int DutyCycle);
void DisplayVI(void);
void DisplayMeasurementLCD(void);
void Display_dacip(unsigned char vol);
unsigned int Read_ADCValue(unsigned char);
void DisplaySettingsMenu(void);
void enable_rx(void);
void DisplayFactoryMenu(void);
void DisplayCalibration(void);
void DisplayReceivingFactoryData(void);
void CalibrationFunction(unsigned char Key, unsigned char Steps);
void keyDeBounce(void);
unsigned char read_lcd_id(void);
unsigned char read_lcd_status(void);
void DisplayAmplitude(void);
void DisplayFrequency(void);
void DisplayHome(void);
void update_amplitude_value();
void update_frequency_value();
void Display_Wave_Type(unsigned char waveType);
void display_home_page(void);




