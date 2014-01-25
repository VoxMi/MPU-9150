/*
 * Copyright (c) 2014 Ovcharenko Alexander <ferrocentos[dot]gmail.com>
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "twi.h"

#define TWI_FREQ 100000UL	// baund rate (100 kHz)
#define TWI_PRESCALER 1		// TWI clock prescaler (must be 1, 4, 16 or 64)
#define TWI_BUFFERSIZE 32	// TWI buffer size

// calculate baud rate with rounding the result to the nearest int value
#define TWI_BITRATE	((((F_CPU+TWI_FREQ/2)/TWI_FREQ)-16ul)/(2ul*TWI_PRESCALER))

// set correct bits for prescaler
#if (TWI_PRESCALER == 1)
#define TWI_PRESCALER_BITS (0<<TWPS1)|(0<<TWPS0)
#elif (TWI_PRESCALER == 4)
#define TWI_PRESCALER_BITS (0<<TWPS1)|(1<<TWPS0)
#elif (TWI_PRESCALER == 16)
#define TWI_PRESCALER_BITS (1<<TWPS1)|(0<<TWPS0)
#elif (TWI_PRESCALER == 64)
#define TWI_PRESCALER_BITS (1<<TWPS1)|(1<<TWPS0)
#endif

static uint8_t twi_Buffer[TWI_BUFFERSIZE];
static uint8_t twi_BufferLength;
static volatile uint8_t twi_error;

void TwiInit(void)
{
	TWBR = TWI_BITRATE;
	TWSR = TWI_PRESCALER_BITS;
	TWCR = _BV(TWEN);
}

uint8_t TwiBusy(void)
{
	return (TWCR & _BV(TWIE));
}

// write a series of bytes to device
uint8_t TwiWriteBytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t *data)
{
	uint8_t  i;
	// reset error state
	twi_error = 0xFF;
	
	// prepare message  for transmitter (slave device address + W bit and address of internal register)
	twi_BufferLength = length+2;
	twi_Buffer[0] = ((dev_address<<1)|(TW_WRITE));
	twi_Buffer[1] = reg_address;

	// prepare data to write to internal register.
	for(i = 2; i < length+2; i++)
		twi_Buffer[i] = data[i - 2];
	// send start condition
	TWCR = _BV(TWINT)|_BV(TWEA)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTA);
	// wait for write operation to complete
	while(TwiBusy());

	// check error status and return it
	if (twi_error == 0xFF)
		return 0;	// success
	else if (twi_error == TW_MT_SLA_NACK)
		return 2;	// error: address send, nack received
	else if (twi_error == TW_MT_DATA_NACK)
		return 3;	// error: data send, nack received
	else
		return 4;	// other twi error
}

// read a series of bytes from device
uint8_t TwiReadBytes(uint8_t dev_address, uint8_t reg_address, uint8_t length, uint8_t *data)
{
	uint8_t  i;
	// reset error state
	twi_error = 0xFF;

	// prepare 1st message for transmitter (slave device address + W bit and address of internal register)
	twi_BufferLength = 2;
	twi_Buffer[0]  = ((dev_address<<1)|(TW_WRITE));
	twi_Buffer[1] = reg_address;
	// send start condition
	TWCR = _BV(TWINT)|_BV(TWEA)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTA);
	// wait until TWI is ready for next transmission
	while(TwiBusy());
	
	// prepare 2nd message for transmitter (slave device address + R bit and address of internal register)
	twi_BufferLength = length+2;
	twi_Buffer[0]  = ((dev_address<<1)|(TW_READ));
	twi_Buffer[1] = reg_address;
	// reset error state
	twi_error = 0xFF;
	// send start condition
	TWCR = _BV(TWINT)|_BV(TWEA)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTA);
	// wait until TWI is ready for next transmission
	while(TwiBusy());												
	// reset error state
	twi_error = 0xFF;												

	// copy data from transceiver buffer (without first byte which contains the SLA+W)
	for(i=1; i<length+1; i++)
	{
		data[i-1] = twi_Buffer[i];
	}
	
	// check error status and return it
	if (twi_error == 0xFF)
		return 0;	// success
	else if (twi_error == TW_MT_SLA_NACK)
		return 2;	// error: address send, nack received
	else if (twi_error == TW_MT_DATA_NACK)
		return 3;	// error: data send, nack received
	else
		return 4;	// other twi error
}

// sends byte or readys receive line
void TwiReply(uint8_t ack)
{
	// transmit master read ready signal, with or without ack
	if(ack)
	{
		TWCR = _BV(TWEN)|_BV(TWIE)|_BV(TWINT)|_BV(TWEA);
	}
	else
	{
		TWCR = _BV(TWEN)|_BV(TWIE)|_BV(TWINT);
	}
}

void TwiStop(void)
{
	// send stop condition
	TWCR = _BV(TWEN)|_BV(TWINT)|_BV(TWSTO);

	// wait for stop condition to be exectued on bus TWINT is not set after a stop condition!
	while(TWCR & _BV(TWSTO))
	{
		continue;
	}
}

void TwiReleaseBus(void)
{
	// release TWI bus
	TWCR = _BV(TWEN)|_BV(TWIE)|_BV(TWEA)|_BV(TWINT);
}

ISR(TWI_vect)
{
	static uint8_t  twi_BufferIndex;

	switch(TW_STATUS)
	{
	case TW_START:				// start condition transmitted
	case TW_REP_START:			// repeated start condition transmitted
		twi_BufferIndex = 0;	// set buffer pointer to the TWI Address location
		
	// Master Transmitter
	case TW_MT_SLA_ACK:			// SLA+W transmitted, ACK received
	case TW_MT_DATA_ACK:		// data transmitted, ACK received
		if(twi_BufferIndex < twi_BufferLength)
		{
			TWDR = twi_Buffer[twi_BufferIndex++];
			TwiReply(1);
		}
		else	// Send STOP after last byte
		{
			TwiStop();
		}
		break;
	case TW_MT_SLA_NACK:  		// SLA+W transmitted, NACK received
		twi_error = TW_MT_SLA_NACK;
		TwiStop();
		break;
	case TW_MT_DATA_NACK:		// data transmitted, NACK received
		twi_error = TW_MT_DATA_NACK;
		TwiStop();
		break;
	case TW_MT_ARB_LOST:		// arbitration lost in SLA+W or data
		twi_error = TW_MT_ARB_LOST;
		TwiReleaseBus();
		break;
		
	// Master Receiver
	case TW_MR_DATA_ACK:		// data received, ACK returned
		twi_Buffer[twi_BufferIndex++] = TWDR;
	case TW_MR_SLA_ACK:			// SLA+R transmitted, ACK received
		if(twi_BufferIndex < (twi_BufferLength-1))	// Detect the last byte to NACK it.
		{
			TwiReply(1);
		}
		else	// Send NACK after next reception
		{
			TwiReply(0);
		}
		break;
	case TW_MR_DATA_NACK:		// data received, NACK returned
		twi_Buffer[twi_BufferIndex] = TWDR;
		TwiStop();
		break;
	case TW_MR_SLA_NACK:		// SLA+R transmitted, NACK received
		TwiStop();
		break;

	case TW_NO_INFO:			// no state information available
		break;
	case TW_BUS_ERROR:			// bus error, illegal start or stop condition
		twi_error = TW_BUS_ERROR;
		TwiStop();
		break;
	}
}
