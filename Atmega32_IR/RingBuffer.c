/*
 * RingBuffer.c
 *
 * Created: 2013-02-13 08:57:54
 *  Author: tmf
 */ 

#include <util/atomic.h>
#include "RingBuffer.h"

bool cb_Add(CircBuffer *cb, CB_Element elem)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(cb_IsFull(cb)) return false;         //Czy jest miejsce w kolejce?
		uint8_t end = (cb->Beg + cb->Count) % CB_MAXTRANS;
		cb->elements[end] = elem;              //Dodaj transakcjê
		++cb->Count;                           //Liczba elementów w buforze
	}
	return true;      //Wszystko ok
}

CB_Element cb_Read(CircBuffer *cb)
{
	CB_Element elem;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if(cb_IsEmpty(cb)) return 0;       //Bufor pusty, nie mo¿na zwróciæ elementu
		elem = cb->elements[cb->Beg];
		cb->Beg = (cb->Beg + 1) % CB_MAXTRANS;
		-- cb->Count;                        //Zmniejszamy liczbê elementów pozosta³ych
	}		                                 //w buforze
	return elem;
}