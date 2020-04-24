/*
 * CRC.h
 *
 *  Created on: 26.03.2020
 *      Author: Tobias & Valentin
 */

#ifndef CRC_H_
#define CRC_H_

/*
 * Generator polynom for cyclic redundancy check
 *
 *
 */
#define CRC_GENERATOR 0x1D;

/**
 *
 *Initial CRC value for calculating first crc
 *
 *
 */
#define INIT_CRC 0x00

/*
 * CRC calculation function for a message more than one byte
 *
 * bytes[]:     message with bytes for crc calculation
 * length:      length of the byte array
 * crc_initval: the start value for calculating crc of the data
 *
 * returns the calculated 8-bit crc value for the whole message
 */
uint8_t calc_crc8_for_data(uint8_t bytes[], int length,  uint8_t crc_initval);

/*
 * CRC calculation function for one byte
 *
 * start_crc: start value for crc calculation
 * byte: 	  byte for crc calculation
 *
 * returns the calculated 8-bit crc value for the given byte
 */
uint8_t calc_crc8_for_one_byte(uint8_t start_crc, uint8_t byte);


/*
 *
 */
int check_crc(uint8_t crc_val, uint8_t *rcv_buffer, uint8_t crc_initval);

uint8_t calc_crc8(uint8_t *send_array, uint8_t crc_initval);

#endif /* CRC_H_ */
