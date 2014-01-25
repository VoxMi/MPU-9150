/*
 * Copyright (c) 2014 Artem Dvinin <artem_dvinin[dot]mail.ru>
 * Site: http://mainloop.ru/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "fifo.h"
#include "uart.h"

FIFO(512)uart_tx_fifo;
FIFO(512)uart_rx_fifo;

void UartInit()
{
	UCSR0C = _BV(USBS0)|_BV(UCSZ00)|_BV(UCSZ01); 	// 8-bit of data, one stop bit, without parity
	UCSR0B = _BV(TXEN0)|_BV(RXCIE0); 				// enables the USART receiver and interrupt
	// bandwidth - 115200bps
	UBRR0H = (unsigned char)(8>>8);
	UBRR0L = (unsigned char)8;
}

// receive data is completed
ISR(USART0_RX_vect)
{
	unsigned char rxbyte = UDR0;
	if(!FIFO_IS_FULL(uart_rx_fifo))
	{
		FIFO_PUSH(uart_rx_fifo, rxbyte);
	}
}

// USART data register is empty
ISR(USART0_UDRE_vect)
{
	if(FIFO_IS_EMPTY(uart_tx_fifo))
	{
		UCSR0B &= ~(1<<UDRIE0);
	}
	else
	{
		char txbyte = FIFO_FRONT(uart_tx_fifo);
		FIFO_POP(uart_tx_fifo);
		UDR0 = txbyte;
	}
}

// send character via USART
int UartPutchar(char c, FILE *file)
{
	int ret;
	cli();
	if(!FIFO_IS_FULL(uart_tx_fifo))
	{
		FIFO_PUSH(uart_tx_fifo, c);
		UCSR0B |= (1<<UDRIE0);
		ret = 0;
	}
	else
	{
		ret = -1;
	}
	sei();
	return ret;
}

// receive character via USART
int UartGetchar(FILE* file)
{
	int ret;
	cli();
	if(!FIFO_IS_EMPTY(uart_rx_fifo))
	{
		ret = FIFO_FRONT(uart_rx_fifo);
		FIFO_POP(uart_rx_fifo);
	}
	else
	{
		ret = _FDEV_EOF;
	}
	sei();
	return ret;
}
