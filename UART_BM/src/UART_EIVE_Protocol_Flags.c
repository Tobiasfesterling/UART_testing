/*
 * UART_EIVE_Protocol_Flags.c
 *
 *  Created on: 09.04.2020
 *      Author: Tobias & Valentin
 */

#include <stdlib.h>
#include <stdio.h>
#include "UART_EIVE_Protocol_Flags.h"

/*
 * Setter Method for ACK-Flag
 *
 * @*flags 	flags array
 * @ack 	ACK or NACK
 */
void set_ACK_Flag(uint8_t *flags, uint8_t ack)
{
	if(ack == ACK)
		*flags |= ACK_MASK;
	else
		*flags &= ~ACK_MASK;
}

/*
 * Setter Method for Req_to_send-Flag
 *
 * @*flags 			flags array
 * @req_to_send 	bit req_to_send
 */
void set_Req_to_send_Flag(uint8_t *flags, uint8_t req_to_send)
{
	if(req_to_send == 1)
		*flags |= REQ_TO_SEND_MASK;
	else
		*flags &= ~REQ_TO_SEND_MASK;
}

/*
 * Setter Method for Rdy_to_rcv-Flag

 *
 * @*flags 		flags array
 * @rdy_to_rcv 	bit req_to_send
 */
void set_Rdy_to_rcv_Flag(uint8_t *flags, uint8_t rdy_to_rcv)
{
	if(rdy_to_rcv == 1)
		*flags |= READY_TO_RECV_MASK;
	else
		*flags &= ~READY_TO_RECV_MASK;
}

/*
 * Setter Method for Start-Flag
 *
 * @*flags 	flags array
 * @start 	first package
 */
void set_Start_Flag(uint8_t *flags, uint8_t start)
{
	if(start == 1)
		*flags |= START_MASK;
	else
		*flags &= ~START_MASK;
}

/*
 * Setter Method for End-Flag
 *
 * @*flags 	flags array
 * @end 	last package
 */
void set_End_Flag(uint8_t *flags, uint8_t end)
{
	if(end == 1)
		*flags |= END_MASK;
	else
		*flags &= ~END_MASK;
}

/*
 * Getter Method for ACK-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_ACK_flag(uint8_t flags)
{
	if((flags & ACK_MASK) != 0)
		return SET;
	else return NOT_SET;
}

/*
 * Getter Method for REQUEST_TO_SEND-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_Req_to_send_flag(uint8_t flags)
{
	if((flags & REQ_TO_SEND_MASK) != 0)
		return SET;
	else return NOT_SET;
}

/*
 * Getter Method for READY_TO_RECEIVE-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_ready_to_recv_flag(uint8_t flags)
{
	if((flags & READY_TO_RECV_MASK) != 0)
		return SET;
	else return NOT_SET;
}

/*
 * Getter Method for START-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_start_flag(uint8_t flags)
{
	if((flags & START_MASK) != 0)
		return SET;
	else return NOT_SET;
}

/*
 * Getter Method for END-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_end_flag(uint8_t flags)
{
	if((flags & END_MASK) != 0)
		return SET;
	else return NOT_SET;
}
