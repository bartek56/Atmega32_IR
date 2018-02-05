//-----------------------------------------------------------------------------------------------------------
// *** Obs≥uga wyúwietlaczy alfanumerycznych zgodnych z HD44780 ***
//
// - Sterowanie: tryb 4-bitowy
// - Dowolne przypisanie kaødego sygna≥u sterujπcego do dowolnego pinu mikrokontrolera
// - Praca z pinem RW pod≥πczonym do GND lub do mikrokontrolera (sprawdzanie BusyFLAG - szybkie operacje LCD)
//
// Pliki 			: lcd44780.c , lcd44780.h
// Mikrokontrolery 	: Atmel AVR
// Kompilator 		: avr-gcc
// èrÛd≥o 			: http://www.atnel.pl
// Data 			: marzec 2010
// Autor 			: Miros≥aw Kardaú
//----------------------------------------------------------------------------------------------------------
// Rozmiar kodu z za≥πczonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str() 				(RW<-->GND)
// dla procesorÛw AVR: 240 bajtÛw !!!
// Rozmiar kodu z za≥πczonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str(), lcd_locate()	(RW<-->GND)
// dla procesorÛw AVR: 254 bajty
// Rozmiar kodu z za≥πczonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str() 				(RW<-->uC)
// dla procesorÛw AVR: 326 bajtÛw !!!
// Rozmiar kodu z za≥πczonymi tylko funkcjami: lcd_init(), lcd_cls(), lcd_str(), lcd_locate()	(RW<-->uC)
// dla procesorÛw AVR: 340 bajtÛw
//-----------------------------------------------------------------------------------------------------------
#ifndef LCD_H_
#define LCD_H_
//----------------------------------------------------------------------------------------
//
//		Parametry pracy sterownika
//
//----------------------------------------------------------------------------------------
// rozdzielczoúÊ wyúwietlacza LCD (wiersze/kolumny)
#define LCD_Y 2		// iloúÊ wierszy wyúwietlacza LCD
#define LCD_X 16	// iloúÊ kolumn wyúwietlacza LCD

// tu ustalamy za pomocπ zera lub jedynki czy sterujemy pinem RW
//	0 - pin RW pod≥πczony na sta≥e do GND
//	1 - pin RW pod≥πczony do mikrokontrolera
#define USE_RW 1

//----------------------------------------------------------------------------------------
//
//		Ustawienia sprzÍtowe po≥πczeÒ sterownika z mikrokontrolerem
//
//----------------------------------------------------------------------------------------
// tu konfigurujemy port i piny do jakich pod≥πczymy linie D7..D4 LCD
#define LCD_D7PORT  C
#define LCD_D7 5
#define LCD_D6PORT  C
#define LCD_D6 4
#define LCD_D5PORT  C
#define LCD_D5 3
#define LCD_D4PORT  C
#define LCD_D4 2


// tu definiujemy piny procesora do ktÛrych pod≥πczamy sygna≥y RS,RW, E
#define LCD_RSPORT B
#define LCD_RS 0

#define LCD_RWPORT B
#define LCD_RW 1

#define LCD_EPORT B
#define LCD_E 2
//------------------------------------------------  koniec ustawieÒ sprzÍtowych ---------------


//----------------------------------------------------------------------------------------
//****************************************************************************************
//*																						 *
//*		U S T A W I E N I A   KOMPILACJI												 *
//*																						 *
//*		W≥πczamy kompilacjÍ komend uøywanych lub wy≥πczamy nieuøywanych					 *
//*		(dziÍki temu regulujemy zajÍtoúÊ pamiÍci FLASH po kompilacji)					 *
//*																						 *
//*		1 - oznacza W£•CZENIE do kompilacji												 *
//*		0 - oznacza wy≥πczenie z kompilacji (funkcja niedostÍpna)						 *
//*																						 *
//****************************************************************************************
//----------------------------------------------------------------------------------------

#define USE_LCD_LOCATE	1			// ustawia kursor na wybranej pozycji Y,X (Y=0-3, X=0-n)

#define USE_LCD_CHAR 	1			// wysy≥a pojedynczy znak jako argument funkcji

#define USE_LCD_STR_P 	1			// wysy≥a string umieszczony w pamiÍci FLASH
#define USE_LCD_STR_E 	1			// wysy≥a string umieszczony w pamiÍci FLASH

#define USE_LCD_INT 	1			// wyúwietla liczbÍ dziesietnπ na LCD
#define USE_LCD_HEX 	0			// wyúwietla liczbÍ szesnastkowπ na LCD

#define USE_LCD_DEFCHAR		1		// wysy≥a zdefiniowany znak z pamiÍci RAM
#define USE_LCD_DEFCHAR_P 	1		// wysy≥a zdefiniowany znak z pamiÍci FLASH
#define USE_LCD_DEFCHAR_E 	1		// wysy≥a zdefiniowany znak z pamiÍci EEPROM

#define USE_LCD_CURSOR_ON 		0	// obs≥uga w≥πczania/wy≥πczania kursora
#define USE_LCD_CURSOR_BLINK 	0	// obs≥uga w≥πczania/wy≥πczania migania kursora
#define USE_LCD_CURSOR_HOME 	0	// ustawia kursor na pozycji poczπtkowej

//------------------------------------------------  koniec ustawieÒ kompilacji ---------------






// definicje adresÛw w DDRAM dla rÛønych wyúwietlaczy
// inne sπ w wyúwietlaczach 2wierszowych i w 4wierszowych
#if ( (LCD_Y == 4) && (LCD_X == 16) )
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54  	// adres 1 znaku 4 wiersza
#else
#define LCD_LINE1 0x00		// adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40		// adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10  	// adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50  	// adres 1 znaku 4 wiersza
#endif


// Makra upraszczajπce dostÍp do portÛw
// *** PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
// *** PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
// *** DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)


// Komendy sterujπce
#define LCDC_CLS					0x01
#define LCDC_HOME					0x02
#define LCDC_ENTRY					0x04
	#define LCDC_ENTRYR					0x02
	#define LCDC_ENTRYL					0
	#define LCDC_MOVE					0x01
#define LCDC_ONOFF					0x08
	#define LCDC_DISPLAYON				0x04
	#define LCDC_CURSORON				0x02
	#define LCDC_CURSOROFF				0
	#define LCDC_BLINKON				0x01
#define LCDC_SHIFT					0x10
	#define LCDC_SHIFTDISP				0x08
	#define LCDC_SHIFTR					0x04
	#define LCDC_SHIFTL					0
#define LCDC_FUNC					0x20
	#define LCDC_FUNC8B					0x10
	#define LCDC_FUNC4B					0
	#define LCDC_FUNC2L					0x08
	#define LCDC_FUNC1L					0
	#define LCDC_FUNC5x10				0x04
	#define LCDC_FUNC5x7				0
#define LCDC_SET_CGRAM				0x40
#define LCDC_SET_DDRAM				0x80




extern int lcdFlagClear;
// deklaracje funkcji na potrzeby innych modu≥Ûw
void Lcd_init(void);								// W£•CZONA na sta≥e do kompilacji
void lcd_cls(void);									// W£•CZONA na sta≥e do kompilacji
void lcd_str(char * str);							// W£•CZONA na sta≥e do kompilacji

void lcd_locate(uint8_t x, uint8_t y);				// domyúlnie W£•CZONA z kompilacji w pliku lcd.c

void lcd_char(char c);								// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_str_P(char * str);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_str_E(char * str);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_int(int val);								// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_hex(int val);								// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_defchar(uint8_t nr, uint8_t *def_znak);	// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_defchar_P(uint8_t nr, uint8_t *def_znak);	// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);	// domyúlnie wy≥πczona z kompilacji w pliku lcd.c

void lcd_home(void);								// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_cursor_on(void);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_cursor_off(void);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_blink_on(void);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c
void lcd_blink_off(void);							// domyúlnie wy≥πczona z kompilacji w pliku lcd.c

#endif /* LCD_H_ */
