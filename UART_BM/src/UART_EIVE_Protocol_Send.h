/*
 * UART_EIVE_Protocol_Send.h
 *
 *  Created on: 24.04.2020
 *      Author: tobi
 */

#ifndef UART_EIVE_PROTOCOL_SEND_H_
#define UART_EIVE_PROTOCOL_SEND_H_

#include "UART_EIVE_Protocol.h"
#include "CRC.h"

extern uint8_t LAST_CRC_SEND 	= INIT_CRC;

extern uint8_t LAST_CRC_RCVD 	= INIT_CRC;

extern uint8_t CALC_CRC			= INIT_CRC;

#endif /* UART_EIVE_PROTOCOL_SEND_H_ */
