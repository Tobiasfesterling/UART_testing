/*
 * CRC.c
 *
 *  Created on: 26.03.2020
 *      Author: Tobias & Valentin
 */

#include <stdio.h>
#include <stdlib.h>
#include "CRC.h"
//#include "UART_io.h"
//#include "xparameters.h"
#include "UART_EIVE_Protocol.h"

/*
 * CRC calculation function for one byte
 *
 * start_crc: start value for crc calculation
 * byte: 	  byte for crc calculation
 *
 * returns the calculated 8-bit crc value for the given byte
 */
uint8_t calc_crc8_for_one_byte(uint8_t start_crc, uint8_t byte)
{
	uint8_t crc = start_crc ^ byte;
	for(int i = 0; i < 8; i++)
	{
		if((crc & 0x80) != 0)
		{
			crc = (crc<<1) ^ CRC_GENERATOR;
		} else
		{
			crc <<= 1;
		}
	}

	return crc;
}

/*
 * CRC calculation function for a message more than one byte
 *
 * bytes[]: message with bytes for crc calculation
 * length:  length of the byte array
 * crc_initval: the start value for calculating crc of the data
 *
 * returns the calculated 8-bit crc value for the whole message
 */
uint8_t calc_crc8_for_data(uint8_t *bytes, int length, uint8_t crc_initval)
{

	uint8_t crc_val = crc_initval;
	for(int i = 0; i < length; i++)
	{
		crc_val = calc_crc8_for_one_byte(crc_val, bytes[i]);
	}

	printf("calculated crc: %i\n", crc_val);
	return crc_val;
}

uint8_t calc_crc8(uint8_t *send_array, uint8_t crc_initval)
{
	printf("crc8: initval: %i\n", crc_initval);
	uint8_t temp31[BUFFER_SIZE - 1] = {0};

	for(int i = 0; i < CRC_POS; i++)
	{
		temp31[i] = send_array[i];
	}

	for(int j = CRC_POS; j < BUFFER_SIZE - 1; j++)
	{
		temp31[j] = send_array[j + 1];
	}

	puts("calc crc8");
	return calc_crc8_for_data(temp31, BUFFER_SIZE - 1, crc_initval);
}

/*
 *
 */
int check_crc(uint8_t crc_val, uint8_t *rcv_buffer, uint8_t crc_initval)
{
	uint8_t calculated_crc;

	calculated_crc = calc_crc8(rcv_buffer, crc_initval);

	if(calculated_crc != crc_val)
		return XST_FAILURE;


	//*crc_initval = calculated_crc;

	return XST_SUCCESS;
}
