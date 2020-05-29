/*
 * UART_EIVE_Protocol_Flags.h
 *
 *  Created on: 09.04.2020
 *      Author: Tobias & Valentin
 */

#ifndef SRC_UART_EIVE_PROTOCOL_FLAGS_H_
#define SRC_UART_EIVE_PROTOCOL_FLAGS_H_

/******************** SET and NOT_SET ********************/

/*
 *
 * Return value for not set bit
 *
 *
 */
#define NOT_SET					0

/*
 *
 * Return value for set bit
 *
 *
 */
#define SET 					1

/******************** ACK and NACK ********************/

/*
 *
 * Value for positive acknowledging
 *
 *
 */
#define ACK 					1

/*
 *
 * Value for negative acknowledging
 *
 *
 */
#define NACK 					0

/******************** Masks for the flags (binary) ********************/

/*
 *
 * Mask for the acknowledge bit
 *
 *
 */
#define ACK_MASK            	0b10000000

/*
 *
 * Mask for the request to send bit
 *
 *
 */
#define REQ_TO_SEND_MASK    	0b01000000

/*
 *
 * Mask for the ready to receive bit
 *
 *
 */
#define READY_TO_RECV_MASK  	0b00100000

/*
 *
 * Mask for the start bit
 *
 *
 */
#define START_MASK          	0b00010000

/*
 *
 * Mask for the end bit
 *
 *
 */
#define END_MASK            	0b00001000

/*
 *
 * Mask for the "ID unknown" bit
 *
 *
 */
#define IDU_MASK				0b00000100

/******************** Masks for the flags (decimal) ********************/

/*
 *
 * Mask for the acknowledge bit
 *
 *
 */
#define ACK_DEC            		128

/*
 *
 * Mask for the request to send bit
 *
 *
 */
#define REQ_TO_SEND_DEC    		64

/*
 *
 * Mask for the ready to receive bit
 *
 *
 */
#define READY_TO_RECV_DEC  		32

/*
 *
 * Mask for the start bit
 *
 *
 */
#define START_DEC          		16

/*
 *
 * Mask for the end bit
 *
 *
 */
#define END_DEC            		8

/******************** Set or unset all flags ********************/

/*
 *
 * Set all flags
 *
 *
 */
#define SET_ALL_FLAGS			0b11111111
/*
 *
 * Reset all flags
 *
 *
 */
#define UNSET_ALL_FLAGS			0b00000000


/******************** Setter and getter methods for the protocol flags ********************/
/*
 * Setter Method for ACK-Flag
 *
 * @*flags 	flags array
 * @ack 	ACK or NACK
 */
void set_ACK_Flag(uint8_t *flags, uint8_t ack);

/*
 * Setter Method for Req_to_send-Flag
 *
 * @*flags 			flags array
 * @req_to_send 	bit req_to_send
 */
void set_Req_to_send_Flag(uint8_t *flags, uint8_t req_to_send);

/*
 * Setter Method for Rdy_to_rcv-Flag

 *
 * @*flags 		flags array
 * @rdy_to_rcv 	bit req_to_send
 */
void set_Rdy_to_rcv_Flag(uint8_t *flags, uint8_t rdy_to_rcv);

/*
 * Setter Method for Start-Flag
 *
 * @*flags 	flags array
 * @start 	first package
 */
void set_Start_Flag(uint8_t *flags, uint8_t start);

/*
 * Setter Method for End-Flag
 *
 * @*flags 	flags array
 * @end 	last package
 */
void set_End_Flag(uint8_t *flags, uint8_t end);

/*
 * Setter method for the id unknown flag
 *
 * @param: *flags: 	The pointer of the flag byte from the header
 * 		   idu:		Set (1) or Unset (0) the flag bit
 *
 */
void set_ID_Unknown_Flag(uint8_t *flags, uint8_t idu);

/*
 * Getter Method for ACK-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_ACK_flag(uint8_t flags);

/*
 * Getter Method for REQUEST_TO_SEND-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_Req_to_send_flag(uint8_t flags);

/*
 * Getter Method for READY_TO_RECEIVE-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_ready_to_recv_flag(uint8_t flags);


/*
 * Getter Method for START-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_start_flag(uint8_t flags);

/*
 * Getter Method for END-Flag
 *
 * @param: *flags: The pointer of the flag byte from the header
 *
 * @return: SET (1) if the bit is set else NOT_SET (0)
 */
int get_end_flag(uint8_t flags);


int get_ID_Unknown_Flag(uint8_t flags);


#endif /* SRC_UART_EIVE_PROTOCOL_FLAGS_H_ */
