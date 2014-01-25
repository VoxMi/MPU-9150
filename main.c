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
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdlib.h>
#include "uart.h"
#include "twi.h"
#include "ml.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"

#define MPU_DEBUG				// print some debug messages
#define DEFAULT_MPU_HZ  (100)	// starting sampling rate

// packet structure for InvenSense teapot demo
int8_t teapotPacket[24] = {'$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\r', '\n'};

bool mpuInterrupt;	// indicates whether MPU interrupt pin has gone high

FILE uart_stream = FDEV_SETUP_STREAM(UartPutchar, NULL, _FDEV_SETUP_RW);

uint16_t dmp_features;

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static int8_t gyro_orientation[9] = {1, 0, 0,
									 0, 1, 0,
									 0, 0, 1};

ISR(INT2_vect)
{
	if(bit_is_clear(PIND,PD2))
	{
		mpuInterrupt = true;
	}
}

void EnableInterrupt(void)
{
	PORTD |= _BV(PD2);
	EICRA = _BV(ISC21);	// Allow interrupts on INT2, triggered on falling edge
	EIMSK = _BV(INT2);	// Enable only on INT2
}

int main(void)
{
	int result;
	uint8_t accel_fsr;
	uint16_t gyro_rate, gyro_fsr;

	int16_t gyro[3], accel[3], sensors;
	int32_t quat[4];
	int16_t compass[3];
	uint8_t more;
	
	char temp_out[4];
	int32_t temp_in;
	
	stdout = &uart_stream;

	UartInit();
	TwiInit();

	sei();

	/* Initializing MPU */
	result = mpu_init(NULL);
#if defined MPU_DEBUG
	printf_P(PSTR("Result of mpu_init %d\r\n"), result);
#endif
	/* Get/set hardware configuration. Start gyro. */
	/* Wake up all sensors. */
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);

	/* Push both gyro and accel data into the FIFO. */
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(DEFAULT_MPU_HZ);

	/* Read back configuration in case it was set improperly. */
	mpu_get_sample_rate(&gyro_rate);
	mpu_get_gyro_fsr(&gyro_fsr);
	mpu_get_accel_fsr(&accel_fsr);

	/* Initialize DMP. */
	result = dmp_load_motion_driver_firmware();
#if defined MPU_DEBUG
	printf_P(PSTR("Result of load firmware %d\r\n"), result);
#endif
	dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
	dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	dmp_enable_feature(dmp_features);
	dmp_set_fifo_rate(DEFAULT_MPU_HZ);

	/* Set hardware to interrupt periodically. */
	dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);

	/* Enable system interrupt for set "DMP Ready flag" */
	EnableInterrupt();
	
	_delay_ms(500);

	for(;;)
	{
		/* Enable the DMP */
		mpu_set_dmp_state(1);

		if (mpuInterrupt)
		{
			dmp_read_fifo(gyro, accel, quat, &sensors, &more);

			// Read temperature data in q16 format
			mpu_get_temperature(&temp_in);

			// Read compass data
			mpu_get_compass_reg(compass);

			if (!more)
			{
				mpuInterrupt = false;
			}
			if (sensors & INV_XYZ_GYRO)
			{
				// Send data to the client application in quat packet type format
				teapotPacket[2] = (int8_t)(quat[0] >> 24);
				teapotPacket[3] = (int8_t)(quat[0] >> 16);
				teapotPacket[4] = (int8_t)(quat[0] >> 8);
				teapotPacket[5] = (int8_t)quat[0];
				teapotPacket[6] = (int8_t)(quat[1] >> 24);
				teapotPacket[7] = (int8_t)(quat[1] >> 16);
				teapotPacket[8] = (int8_t)(quat[1] >> 8);
				teapotPacket[9] = (int8_t)quat[1];
				teapotPacket[10] = (int8_t)(quat[2] >> 24);
				teapotPacket[11] = (int8_t)(quat[2] >> 16);
				teapotPacket[12] = (int8_t)(quat[2] >> 8);
				teapotPacket[13] = (int8_t)quat[2];
				teapotPacket[14] = (int8_t)(quat[3] >> 24);
				teapotPacket[15] = (int8_t)(quat[3] >> 16);
				teapotPacket[16] = (int8_t)(quat[3] >> 8);
				teapotPacket[17] = (int8_t)quat[3];

				// Convert temperature to ASCII format
				dtostrf((float)(temp_in/pow(2, 16)),4,1,temp_out);

				teapotPacket[18] = temp_out[0];
				teapotPacket[19] = temp_out[1];
				teapotPacket[20] = temp_out[2];
				teapotPacket[21] = temp_out[3];

				for (int i = 0; i < 24; ++i)
				{
					printf("%c", teapotPacket[i]);
				}

				//printf("gyro: %d\t%d\t%d\t\r\n", gyro[0], gyro[1], gyro[2]);
				//printf("accel: %d\t%d\t%d\t\r\n", accel[0], accel[1], accel[2]);
				//printf("comp: %d\t%d\t%d\t\r\n", compass[0], compass[2], compass[2]);
				//printf("quat: %ld\t%ld\t%ld\t%ld\t\r\n", quat[0], quat[1], quat[2], quat[3]);

				// for print value in float format you can use floating point printf version (see
				// Makefile)
				//printf("%.1f\r\n", temp_in/(pow(2, 16)));
			}
		}
	}
}