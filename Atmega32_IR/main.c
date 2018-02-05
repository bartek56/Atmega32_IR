/*
 * Atmega32_IR.c
 *
 * Created: 2016-12-14 20:05:42
 * Author : Bartek
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "RingBuffer.h"
#include "lcd44780.h"

#define IR_NEC_PULSE        560  //Czas w us podstawowego interwa³u
#define IR_NEC_TOLERANCE    100  //Tolerancja czasu w us
#define IR_NEC_TRAILER      16   // Start ramki - 16 interwa³ów
#define IR_NEC_ZERO         2    //Czas trwania bitu 0
#define IR_NEC_ONE          4    //Czas trwania bitu 1
#define IR_BITSNO           32   //Liczba bitów kodu
#define IR_NEC_RPTLASTCMD   -1   //Kod powtórzenia ostatniego polecenia

static inline uint16_t IR_CalcTime(uint16_t time)
{
	return time*(F_CPU/1000000UL)/64;   //Przelicz czas w mikrosekundach na tykniêcia timera dla preskalera 64
}

enum IR_NEC_States {IR_NEC_Nothing, IR_NEC_Trailer, IR_NEC_FirstBit, IR_NEC_Receiving};
enum IR_NEC_States IR_State;           //Stan maszyny stanów

volatile uint8_t  IR_recpos;     //Nr aktualnie odbieranego bitu

CircBuffer IR_CMD_Buffer; //Instancja bufora ko³owego przechowuj¹cego polecenia

// przepe³nienie timera
ISR(TIMER1_COMPA_vect)
{
	if((IR_State==IR_NEC_Receiving) && (IR_recpos==0))
	cb_Add(&IR_CMD_Buffer, IR_NEC_RPTLASTCMD);
	IR_State=IR_NEC_Nothing;      //B³¹d transmisji lub powtórzenie - restart maszyny stanów
}

// zmiania stanu (zbocza) pinu
ISR(TIMER1_CAPT_vect)
{
	static uint32_t IR_RecCmd;   //Odebrana komenda z pilota
	
	TCNT1=0;             //Coœ odebraliœmy wiêc zerujemy licznik
	uint16_t cca=ICR1;   //Odczytaj marker czasowy
	uint8_t flag=TCCR1B & (1<<ICES1); //Zbocze, które wywo³a³o zdarzenie, opadaj¹ce(ICES1=0) b¹dŸ narastaj¹ce(ICES1=1)
	TCCR1B^=(1<<ICES1); //Zmieñ zbocze wywo³uj¹ce zdarzenie na przeciwne

	switch(IR_State)
	{
		case IR_NEC_Nothing:
		if(flag==0)
		{
			IR_State=IR_NEC_Trailer;
			IR_recpos=0;
			IR_RecCmd=0;
		}
		break;

		case IR_NEC_Trailer:
		if((cca>IR_CalcTime((IR_NEC_PULSE-IR_NEC_TOLERANCE)*IR_NEC_TRAILER))  //Koniec odbioru trailera
		&& ((cca<IR_CalcTime((IR_NEC_PULSE+IR_NEC_TOLERANCE)*IR_NEC_TRAILER))))
		IR_State=IR_NEC_FirstBit;
		break;

		case IR_NEC_FirstBit:
		IR_State=IR_NEC_Receiving;
		break;

		case IR_NEC_Receiving:
		if(flag==0)
		{
			IR_RecCmd<<=1;
			if(cca>IR_CalcTime((IR_NEC_PULSE+IR_NEC_TOLERANCE)*IR_NEC_ZERO)) IR_RecCmd|=1;
			++IR_recpos;
			if(IR_recpos==IR_BITSNO)
			{
				IR_State=IR_NEC_Nothing;
				cb_Add(&IR_CMD_Buffer, IR_RecCmd);  //Dodaj odebrane polecenie do bufora
			}
		}
		break;
	}
}

void IR_init()
{
	PORTD = (1<<PD6); //PD6 (ICP1) jest wejœciem z podci¹ganiem do 1
	OCR1A=IR_CalcTime((IR_NEC_PULSE+IR_NEC_TOLERANCE)*IR_NEC_TRAILER);         //Okres timera
	TCCR1A=0;                  //Od³¹cz piny OCx, wybierz tryb CTC  
	TCCR1B = (1<<ICNC1) | (1<<WGM12) | (1<<CS11) | (1<<CS10); // wybierz tryb 4, OCR1A okreœla top, preskaler 64, noise canceller, zdarzenie na opadaj¹cym zboczu
	TIMSK=(1<<TICIE1) | (1<<OCIE1A); // odblokowanie przerwaæ przy zmianie stanu pinu ICP1 oraz po prze³nieniu (CTC)
}

int main(void)
{
	char bufor[13];
	Lcd_init();
	lcd_cls();
	lcd_str("IR");
	IR_init();
	sei();
	while(1)
	{
		if(cb_IsEmpty(&IR_CMD_Buffer)==false)
		{
			CB_Element cmd=cb_Read(&IR_CMD_Buffer);
			
			if(cmd!=IR_NEC_RPTLASTCMD)
			{
				ultoa(cmd, bufor, 16);
				lcd_cls();
				lcd_locate(0,0);
				lcd_str(bufor);
			} 
			else 
			{
				lcd_cls();
				lcd_locate(0,0);
				lcd_str(bufor);
				lcd_locate(0,1);
				lcd_str("Powtorzenie");
			}			
		}
	}
}

