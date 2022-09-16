#include <xc.h>

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF       // Flash Program Memory Self Write Enable bits (0000h to 0FFFh write protected, 1000h to 1FFFh may be modified by EECON control)

#define	LCD_RS RD4
#define LCD_EN RD5
#define LCD_DATA PORTD
#define	LCD_STROBE()	((LCD_EN = 1), delay_LCD(1), (LCD_EN=0))

char GRADE = 0xDF; 		//codul ASCII pentru simbolul "�" (grade)
char MICRO = 0xE4; 		//codul ASCII pentru simbolul "�" (micro)
char ALPHA = 0xE0; 		//codul ASCII pentru simbolul "a" (alfa)
char BETA = 0xE2; 		//codul ASCII pentru simbolul "�" (beta)
char EPSILON = 0xE3; 	//codul ASCII pentru simbolul "e" (epsilon)
char THETA = 0xF2; 		//codul ASCII pentru simbolul "? " (theta)
char MIU = 0xE4; 		//codul ASCII pentru simbolul "�" (miu)
char OMEGA = 0xF4; 		//codul ASCII pentru simbolul "O" (omega)
char SIGMAm = 0xE5; 	//codul ASCII pentru simbolul "s" (sigma mic)
char RO = 0xE6; 		//codul ASCII pentru simbolul "?" (ro)
char SIGMAM = 0xF6; 	//codul ASCII pentru simbolul "S" (sigma mare)
char PI = 0xF7; 		//codul ASCII pentru simbolul "p" (pi)
char RADICAL = 0xE8; 	//codul ASCII pentru simbolul "radical"
char MINUS1 = 0xE9; 	//codul ASCII pentru simbolul "-1" (putere -1)
char INFINIT = 0xF3; 	//codul ASCII pentru simbolul "infinit"
char NEGRU = 0xFF; 		//codul ASCII pentru simbolul "celula neagra"
char ALB = 0xFE; 		//codul ASCII pentru simbolul "celula alba"
char ms1[8]={0x00,0x00,0x0F,0x10,0x0E,0x01,0x1E,0x04}; //litera "s,"
char ma1[8]={0x0A,0x04,0x0E,0x01,0x0F,0x11,0x0F,0x00}; //litera "a~"
char ma2[8]={0x04,0x0A,0x0E,0x01,0x0F,0x11,0x0F,0x00}; //litera "a^"
char mi1[8]={0x04,0x0A,0x00,0x0C,0x04,0x04,0x0E,0x00}; //litera "i^"
char mt1[8]={0x08,0x08,0x1C,0x08,0x08,0x09,0x06,0x04}; //litera "t,"
char mL1[8]={0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}; //caracter " |"
char mL2[8]={0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03}; //caracter " ||"
char mL3[8]={0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07}; //caracter " |||"
char mL4[8]={0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F}; //caracter "||||"
unsigned char a1, a2, i1, s1, t1, L1, L2, L3, L4; 	//denumirile acestor variabile se aleg
													//pentru a descrie caracterul diacritic sau grafic definit de
													//utilizator in cazul de fata este vorba de diacriticile "a~", "a^", "i^",
													//"s,","t," si de simbolurile grafice" |"," ||"," ||",� |||";
void init_LCD(void);
void lcd_write(unsigned char c);
void lcd_clear(void);
void lcd_goto(unsigned char pos);
void lcd_puts(char * s);
void lcd_putch(char c);
void delay_LCD(unsigned long t);
void initializare_diacritice(void);
void scrie_diacritice_in_CGRAM(char matrice[8], unsigned char pozitie_DDRAM,unsigned char *diacritic);
void afisare_diacritice(unsigned char diacritic_afisat, unsigned char linia);

void init_LCD(void)
{
    delay_LCD(2000);
    lcd_write(0x2C);
    delay_LCD(100);
    lcd_clear();	
	delay_LCD(3000);
    lcd_write(0x02);
    delay_LCD(2000);
    lcd_write(0x06);
    delay_LCD(100);
    lcd_write(0x0C);
    delay_LCD(100);
    lcd_write(0x10);
    delay_LCD(100);
    lcd_write(0x2C);
    delay_LCD(100);	
	initializare_diacritice();
}

void lcd_write(unsigned char c)
{
    LCD_DATA = (LCD_DATA & 0xF0) | (c >> 4);
	LCD_STROBE();
    LCD_DATA = (LCD_DATA & 0xF0) | ( c & 0x0F );
	LCD_STROBE();
}
void lcd_clear(void)
{
	LCD_RS = 0;
	lcd_write(0x01);
}
void lcd_puts( char * s)
{
	LCD_RS = 1;	// write characters	
	while(*s)
		lcd_write(*s++);
}
void lcd_putch(char c)
{
	LCD_RS = 1;	// write characters	
	lcd_write( c );
}
void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;
	lcd_write(pos);
}
void delay_LCD(unsigned long t)
{
    unsigned long var;
    for(var=0; var < t>>6; var++);	
} 

void initializare_diacritice(void)
{
	scrie_diacritice_in_CGRAM(ma1,0,&a1); //litera "a~"
	scrie_diacritice_in_CGRAM(ma2,1,&a2); //litera "a^"
	scrie_diacritice_in_CGRAM(mi1,2,&i1); //litera "i^"
	scrie_diacritice_in_CGRAM(ms1,3,&s1); //litera "s,"
	scrie_diacritice_in_CGRAM(mt1,4,&t1); //litera "t,"
	scrie_diacritice_in_CGRAM(mL1,5,&L1); //simbol grafic " |"
	scrie_diacritice_in_CGRAM(mL2,6,&L2); //simbol grafic " ||"
	scrie_diacritice_in_CGRAM(mL3,7,&L3); //simbol grafic " |||"
}
void scrie_diacritice_in_CGRAM(char matrice[8], unsigned char pozitie_DDRAM, unsigned char *diacritic)
{
	char i;
	*diacritic = pozitie_DDRAM;
	for (i=0; i<8; i++)
	{
		lcd_goto(0x40+(pozitie_DDRAM*8)+i);
		delay_LCD(100);
		lcd_putch(matrice[i]);
		delay_LCD(100);
	}
}
void afisare_diacritice(unsigned char diacritic_afisat, unsigned char linia)
{
	lcd_goto(linia);
	lcd_putch(diacritic_afisat);
}