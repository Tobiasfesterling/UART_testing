/*
 * UART_EIVE_PROTOCOL.h
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#ifndef SRC_UART_EIVE_PROTOCOL_H_
#define SRC_UART_EIVE_PROTOCOL_H_



/************************************Include Files************************************/
#include "stdio.h"
#include <stddef.h>
#include <stdint.h>
//#include "xparameters.h"
//#include "xuartps.h"
#include "UART_EIVE_Protocol_Flags.h"

/********/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
/*******/

/*************************************************************************************/

#define XST_SUCCESS 0
#define XST_NO_DATA 13
#define XST_FAILURE -1

/** Size of package data and header **/
#define BUFFER_SIZE 		32
#define PACKAGE_DATA_SIZE 	28
#define HEADER_SIZE 		4

int sock;

uint8_t RecvBuffer[BUFFER_SIZE];

#define EMPTY_DATA_LENGTH 	0

#define MAX_TIMER 			100000

/** Identification mask for TM/TC **/
#define TC_MASK				0b00000000
#define TM_MASK				0b11110000

/** Identification numbers for TM/TC **/
#define CAMERA_TC			0b00000000
#define CAMERA_TM			0b11110000

#define UART_TC				0b00001010
#define UART_TM				0b11111010

#define BRAM_TC				0b00000101
#define BRAM_TM				0b11110101

#define CPU_TC				0b00001111
#define CPU_TM				0b11111111

#define DOWNLINK_TC			0b00001001
#define DOWNLINK_TM			0b11111001

#define DAC_TC				0b00000110
#define DAC_TM				0b11110110


/** Header position numbers**/
#define ID_POS  			0
#define CRC_POS 			1
#define DATA_SIZE_POS 		2
#define FLAGS_POS 			3

//Long buffer for receiving data
uint8_t databuffer[357143];

/********************** Functions to send **********************/

/*
 * Main Method of the EIVE UART Protocol to send data
 *
 * @param:	ID		Identification number of the data to send
 * @param:	*databytes	Pointer to the data which is going to be send
 * @param:	dataLength	Length of the data which is going to be send
 *
 * @return:	XST_SUCCES	If the data was send properly
 * @return:	XST_FAILURE	If the data was not send properly
 *
 * This method uses the connection_establishment() -Method to establish a connection between the sender and the receiver.
 * It also uses the send_data() -method to send the transfered data to the receiver after a connection was established.
 * It returns an error if the data could not to be send and a success, if the transmission was possible
 */
int UART_Send_Data(uint8_t ID, uint8_t *databytes, int dataLength);

/*
 * Method to establish a connection to the receiver
 *
 * @param:	ID				Identification number of the package to send
 * @param:	*databytes		Pointer to the array of data which are going to be send
 * @param:	dataLength		Length of the data which is going to be send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 * @param:	*lastCRC_rcvd	Pointer to the last received CRC value
 *
 * @return:	XST_SUCCES	If the connection was established properly
 * @return:	XST_FAILURE	If the connection was not established properly
 */
int connect_(uint8_t ID, uint8_t *databytes, uint8_t dataLength, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd);

/*
 *Request to send, to establish a connection
 *
 *@param:	ID			Identification number of the package to send
 *@param:	*lastCRC	Pointer, last CRC value to save the new CRC value for the next package
 *
 *Configures a package to send a request to send and saves the first CRC
 */
int send_request_to_send(uint8_t ID, uint8_t *temp32, uint8_t *lastCRC_send, uint8_t *flags);

/*
 * Package counter
 *
 * @param:	dataLength	number of bytes of the data to send
 *
 * returns the number of the needed packages to send all the databytes
 */
int package_count(int dataLength);

/*
 * Method to save the submitted header, data, flags and CRC from the receiver
 *
 * @param:	*header			Pointer to an array of the size of HEADER_SIZE to save the received header
 * @param:	*data			Pointer to an array of the size of PACKAGE_DATA_SIZE to save the received data
 * @param:	*flags			Pointer, to save the received Flags
 * @param: 	*submittedCRC	Pointer, to save the submitted CRC value
 *
 * This method stores in the delivered parameters the received information
 */
void get_received_data(uint8_t *header, uint8_t *data, uint8_t *flags, uint8_t *submittedCRC);

/*
 * Method to send the data
 *
 * @param:	ID				Identification number of the package to send
 * @param:	*databytes		Pointer to the array of data which are going to be send
 * @param:	dataLength		Length of the data which is going to be send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 * @param:	*lastCRC_rcvd	Pointer to the last received CRC value
 *
 * @return:	XST_SUCCES	If the data was send properly
 * @return:	XST_FAILURE	If the data was not send properly
 */
int send_data(uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd);

/*
 * Method to wait on an answer of the receiver
 *
 * @param:	*send_array:	pointer to the array which is going to be send again if the timer expires and the RecvBuffer does not get filled
 *							NULL if the send_array is NACK
 * @param:	ID				Identification number of the package to send
 * @param:	*lastCRC_send	Pointer to the last send CRC value
 *
 * @return:	XST_SUCCES		If an answer was received
 * @return:	XST_FAILURE		If no answer was received
 */
int wait_on_answer(uint8_t *send_array, uint8_t ID, uint8_t *lastCRC_send);

/*
 * Method to fill the packages to send
 *
 * @param:	ID 				Identification number of the package to send
 * @param: 	dataLength		length of the data to send, must be given by the user
 * @param:	*databytes		Pointer to the data to send
 * @param:	temp			Pointer to the temporary array in the main method with the length of BUFFER_SIZE * packageCount,
 * 							which is filled with the header and the databytes
 * @param: 	packageCount	numbers of packages needed to send all the databytes
 *
 * Fills the submitted variable temp with the databytes and the headers
 */
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes, uint8_t *temp, int packageCount);

/*
 * Fill Header with submitted parameters
 *
 * @param:	*header			Pointer to store the header
 * @param:	ID 				Identification number of the package to send
 * @param: 	flags			Flags of the package which is going to be send
 * @param:	*lastCRC_send	Pointer to the last calculated CRC of the last send package
 *
 * This method fills the header of empty packages which are going to be send
 */
uint8_t fill_header_for_empty_data(uint8_t *header, uint8_t ID, uint8_t flags, uint8_t *lastCRC_send);

/******************** Functions to receive ********************/

/*
 * Receive data if its available.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 * 			XST_NO_DATA if no data was received.
 *
 */
int UART_Recv_Data();

/*
 * In this method the protocol for receiving data is implemented, which requires a connection
 * to the sender. A connection is established to receive packets longer than 28 bytes.
 * First, it is indicated that some data is to be transmitted, to which it responds that data
 * can be received. Then the actual data transmission takes place, where each received packet
 * is acknowledged positively or negatively.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 */
int receive();

/*
 * To connect with the sender, this method is used. This method checks the
 * received data for the send request flag and answers with the ready to receive flag.
 *
 * @param: *last_crc_send: 	The pointer to the last sent crc value.
 * 		   *new_flags:		The pointer to the flag value which will be send.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 */
int connection_establishment(uint8_t *last_crc_rcv, uint8_t *last_crc_send, uint8_t *new_flags, uint8_t *conn_id, uint8_t *calc_crc);

/*
 * With this method data will be received from the connected sender.
 *
 * @param: *crc_rcv: The pointer to the last received crc value.
 * 		   *crc_send: The pointer to the last sent crc value.
 * 		   rcvd_id: the message-id in the header
 * 		   last_sent_flags: the last sent flags
 */
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t rcvd_id, uint8_t last_sent_flags, uint8_t *calc_crc);

/*
 * This method splits the received data into header and payload data.
 *
 * @param: *rcvBuffer: 	The pointer to the Buffer with the received data.
 * 		   *header: 	The pointer to the header array to store the extracted header.
 * 		   *data:		The pointer to the data array to store the extracted payload data.
 *
 */
int extract_header(const uint8_t *rcvBuffer, uint8_t *header, uint8_t *data);

/*
 * Sends an answer without set the ACK flag
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   *old_id:	  the address of the id of the received package
 *
 * @return: The success or failure of sending the answer
 */
int send_failure(uint8_t *last_crc, uint8_t old_id, uint8_t *calc_crc);

/*
 * Sends an answer with set ACK flag and the wanted flags
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   id:	  	  the id of the received package
 * 		   flags: 	  the flags to send
 *
 * @return: The success or failure of sending the answer
 */
int send_success(uint8_t *last_crc, uint8_t id, uint8_t flags, uint8_t *calc_crc);

/*
 * Sends the answer package with empty data
 *
 * @param: *header: the filled header array
 *
 * @return: The success or failure of sending the answer.
 */
int UART_answer(uint8_t *header);

/*
 * Data callback for received TC. Every subsystem gets his own command.
 *
 * @param: *header: pointer to the header array
 * 		   *databytes: pointer to the data array
 *
 * @return: Success or failure of the data callback
 *
 */
int recv_TC(uint8_t *header, uint8_t *databytes, int size_of_data);

/*
 * This method is used to store the received TM.
 *
 */
int recv_TM();

/*
 *
 */
void default_operation();



#endif /* SRC_UART_EIVE_PROTOCOL_H_ */
