/*
 * UART_EIVE_Protocol_Recv.c
 *
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
//#include "UART_io.h"
#include "CRC.h"
#include "string.h"
#include "stdio.h"

/*
 * Receive data if its available.
 *
 * @return: XST_SUCCESS if the receiving was correct.
 * 			XST_FAILURE if an error occurs.
 * 			XST_NO_DATA if no data was received.
 *
 */
int UART_Recv_Data()
{
	/* variable for the status of the receive buffer: SUCCESS, FAILED, NO_DATA_AVAILABLE */
	int status = XST_SUCCESS;

	//Check the receive buffer if data is available ->Network now
	//status = UART_Recv_Buffer();
	if((status = recv(sock, RecvBuffer, BUFFER_SIZE, 0)) < 0)
		puts("ERROR UARTRecvData");

	//if data available continue, else return
	//if(status != XST_SUCCESS)
	if(status < 0)
	{
		if(status == XST_NO_DATA)
			return XST_NO_DATA;
		else
			return XST_FAILURE;
	}

	status = receive();

	if(status != XST_SUCCESS)
		return XST_FAILURE;

	return XST_SUCCESS;
}

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
int receive()
{
	printf("receive data...\n");
	int status = XST_SUCCESS;

	/* variable for the new flags which will be set */
	uint8_t new_flags = 0x00;

	/* variable to store the last sent CRC value for calculating next CRC */
	uint8_t last_crc_send = 0x00;

	uint8_t conn_id = 0x00;

	/* variable to store the last sent CRC value for calculating next CRC */
	uint8_t last_crc_rcv = 0x00;

	/*variable to store the calculated crc value */
	uint8_t calc_crc = INIT_CRC;

	//printf("connection establishment\n");
	status = connection_establishment(&last_crc_rcv, &last_crc_send, &new_flags, &conn_id, &calc_crc);

	if(status == XST_FAILURE)
		return XST_FAILURE;

	//receive the tm/tc
	printf("receive data\n");
	status = receive_data(&last_crc_rcv, &last_crc_send, conn_id, new_flags, &calc_crc);

	if(status == XST_FAILURE)
		return XST_FAILURE;

	return XST_SUCCESS;
}

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
int connection_establishment(uint8_t *last_crc_rcv, uint8_t *last_crc_send, uint8_t *new_flags, uint8_t *conn_id, uint8_t *calc_crc)
{
	printf("Connection establishment...\n");
	/* variable for header of the first received package */
	uint8_t header[HEADER_SIZE] = {0};

	/* variable for the received data from the package */
	uint8_t data[PACKAGE_DATA_SIZE] = {0};


	//extract header from buffer
	extract_header(RecvBuffer, header, data);
	printf("extracted header successfully\n");

	//
	*conn_id = header[ID_POS];

	/* crc_check */
	printf("CRC Check -> Initval crc 0x00 (connection establishment)\n");
	if(check_crc(header[CRC_POS], RecvBuffer, INIT_CRC) != XST_SUCCESS)
	{
		//Send answer without set ACK flag
		send_failure(last_crc_send, header[ID_POS], calc_crc, NOT_SET);
		printf("crc NACK, failure\n");

		return XST_FAILURE;
	}
	printf("crc ACK, success\n");

	//set acknowledge flag
	set_ACK_Flag(new_flags, ACK);

	//check idu flag
	if(check_ID(header[ID_POS]) == 0)
	{
		//Send answer without set ACK flag
		send_failure(last_crc_send, header[ID_POS], calc_crc, SET);
		printf("id not known, failure\n");

		return XST_FAILURE;

	}

	printf("ID check successful");

	/* check request 2 send */
	if(get_Req_to_send_flag(header[FLAGS_POS]) == NOT_SET)
	{
		//Send answer without set ACK flag
		send_failure(last_crc_send, header[ID_POS], calc_crc, NOT_SET);
		printf("req 2 send flag not received, failure\n");

		return XST_FAILURE;
	}

	printf("req 2 send received, success\n");
	//set the ready_to_receive flag
	set_Rdy_to_rcv_Flag(new_flags, 1);

	int status;

	*last_crc_rcv = header[CRC_POS];
	status = send_success(last_crc_send, *conn_id, *new_flags, calc_crc);
	printf("send success\n");

	return status;
}

/*
 * With this method data will be received from the connected sender.
 *
 * @param: *crc_rcv: The pointer to the last received crc value.
 * 		   *crc_send: The pointer to the last sent crc value.
 * 		   rcvd_id: the message-id in the header
 * 		   last_sent_flags: the last sent flags
 */
int receive_data(uint8_t *crc_rcv, uint8_t *crc_send, uint8_t rcvd_id, uint8_t last_sent_flags, uint8_t *calc_crc)
{
	printf("receive data...\n");
	/* Variable for next received header */
	uint8_t next_header[4];

	/* Variable for flags */
	uint8_t flags_to_send = last_sent_flags;

	/* Variable for next received data */
	uint8_t new_data[PACKAGE_DATA_SIZE];

	/* variable for last received CRC value */
	uint8_t last_crc_rcv = *crc_rcv;
	printf("LastCRC_RCV: %i\n", last_crc_rcv);

	/* variable for last sended CRC value */
	uint8_t last_crc_send = *crc_send;
	printf("LastCRC_SEND: %i\n", last_crc_send);


	/* variable for paketsize */
	int datacounter = 0;

	/* variable for counting package */
	int pkgCounter = 0;

	/* variable for loop */
	uint8_t end = 0;

	//status of the receiving buffer
	int status = XST_NO_DATA;

	//timer for receiving data
	int timer = 1;

	int success = SET;

	/*
	 * while not_end_flag
	 *
	 * loop for receiving long data until end_flag is set
	 */

	while(end != SET)
	{
		printf("data still available\n");
		//Receiving answer
		while(status == XST_NO_DATA)
		{
			printf("Timer: %i\n", timer);
			//timeout for receiving, reset timer for new sending
			if(timer == MAX_TIMER)
			{
				timer = 0;
				printf("timeout for receiving data\n");
			}

			// -> Send answer and wait for data
			if(timer == 0)
			{
				if(success == SET)
				{
					printf("success receiving data\n");
					// success == 1 -> send success
					send_success(&last_crc_send, rcvd_id, flags_to_send, calc_crc);
				}
				else
				{
					printf("failure receving data\n");
					//success == 0 -> send failure
					send_failure(&last_crc_send, next_header[ID_POS], calc_crc, NOT_SET);
				}

			}

			//increase timer
			timer++;

			//receive data if available -> Network now
			//status = UART_Recv_Buffer();
			if(recv(sock, RecvBuffer, BUFFER_SIZE, 0) < 0)
				puts("ERROR RecvAnswer");


			status = XST_SUCCESS; //Test

			printf("Received!");
			//check status of receiving
			if(status != XST_NO_DATA && status != XST_SUCCESS)
				return XST_FAILURE;
		}

		printf("data received\n");

		//reset flags
		flags_to_send = 0x00;

		//extracting the header of the package
		extract_header(RecvBuffer, next_header, new_data);

		//CRC check
		printf("CRC Check -> Initval crc (last CRC received): %i\n", last_crc_rcv);
		if(check_crc(next_header[CRC_POS], RecvBuffer, last_crc_rcv) != XST_SUCCESS)
		{
			printf("failure by received crc\n");
			//failure
			success = 0;
			timer = 0;
			status = XST_NO_DATA;
			//send_failure(&last_crc_send, &next_header[ID_POS]);
			continue;
			//return XST_FAILURE; //test
		}

		//
		set_ACK_Flag(&flags_to_send, 1);

		/*
		 * check ACK_flag in header
		 * ack = 0:	-> send last answer again
		 * 			-> go to top of the loop
		 *
		 * ack = 1: -> continue
		 *
		 */
		if(get_ACK_flag(next_header[FLAGS_POS]) != ACK)
		{
			printf("failure by received ack flag\n");
			//failure
			timer = 0;
			status = XST_NO_DATA;
			continue;
		}

		//store new crc in last_crc_send
		last_crc_send = *calc_crc;

		//store the last received CRC value for next calculating
		last_crc_rcv = next_header[CRC_POS];

		printf("---------------------> Received chars in this package: '");
		//data buffer
		for(int bytes = 0; bytes < next_header[DATA_SIZE_POS]; bytes++)
		{
			printf("%c", new_data[bytes]);
			databuffer[pkgCounter * PACKAGE_DATA_SIZE + bytes] = new_data[bytes];
		}

		datacounter += next_header[DATA_SIZE_POS];
		pkgCounter++;

		printf("'\nDATA SIZE IN THIS PACKAGE: %i <-> %i\n", (int) (sizeof(new_data)/sizeof(new_data[0])), next_header[DATA_SIZE_POS]);


		//check if this package was the last data package
		if(get_end_flag(next_header[FLAGS_POS]) == 1)
		{
			printf("last package\n");
			end = 1;
			send_success(&last_crc_send, rcvd_id, flags_to_send, calc_crc);
		}

		//reset timer
		timer = 0;

		status = XST_NO_DATA;

		success = 1;

	}
	// end while

	//check type
	uint8_t id = next_header[ID_POS] & TM_MASK;

	//data array with exact length
	uint8_t data[datacounter];
	printf("DataCounter: %i\n", datacounter);

	//fill data array
	for(int byte = 0; byte < datacounter; byte++)
		data[byte] = databuffer[byte];

	int size_of_data = (int) (sizeof(data) / sizeof(data[0]));
	//
	switch(id)
	{
		//received data is tc
		case TC_MASK:	recv_TC(next_header, data, size_of_data); break;

		//received data is tm
		case TM_MASK:	recv_TM(); break;

		//...
		default: 		default_operation();
	}

	return XST_SUCCESS;
}

/*
 * This method splits the received data into header and payload data.
 *
 * @param: *rcvBuffer: 	The pointer to the Buffer with the received data.
 * 		   *header: 	The pointer to the header array to store the extracted header.
 * 		   *data:		The pointer to the data array to store the extracted payload data.
 *
 */
int extract_header(const uint8_t *rcvBuffer, uint8_t *header, uint8_t *data)
{
	printf("extract header...\n");
	printf("fill header array with received header\n");
	for(int header_pos = 0; header_pos < HEADER_SIZE; header_pos++)
		header[header_pos] = rcvBuffer[header_pos];

	printf("fill *data array with received data\n");
	for(int data_byte = HEADER_SIZE; data_byte < BUFFER_SIZE; data_byte++)
		data[data_byte - HEADER_SIZE] = rcvBuffer[data_byte];

	return XST_SUCCESS;
}

/*
 * Checks if the id of the incoming packet is known or unknown
 *
 * @param: ID: 	The ID of the incoming packet
 *
 * @return:		1 -> if it's known
 * 				0 -> if it's unknown
 */
int check_ID(uint8_t ID)
{
	switch(ID)
	{
		case CAMERA_TC: 	return 1;
		case UART_TC: 		return 1;
		case CPU_TC: 		return 1;
		case BRAM_TC: 		return 1;
		case DOWNLINK_TC: 	return 1;
		case DAC_TC: 		return 1;
		default: 			return 0;
	}
}

/*
 * Sends an answer without set the ACK flag
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   *old_id:	  the address of the id of the received package
 * 		   id_unknown:	Set (1) if the error was caused by an unknown ID, else not set (0)
 *
 * @return: The success or failure of sending the answer
 */
int send_failure(uint8_t *last_crc, uint8_t old_id, uint8_t *calc_crc, int id_unknown)
{
	printf("sending failure...\n");
	//all flags are set to zero
	uint8_t failure_flags = UNSET_ALL_FLAGS;
	printf("unset all flags\n");

	if(id_unknown == SET)
		set_ID_Unknown_Flag(&failure_flags, SET);

	//header array
	uint8_t header[HEADER_SIZE];


	//set negative-acknowledge flag
	set_ACK_Flag(&failure_flags, NACK);

	//fill the header for the package to send
	*calc_crc = fill_header_for_empty_data(header, old_id, failure_flags, last_crc);
	printf("success filling header for empty data\n");

	//send answer package
	printf("UART answer\n");
	int status = UART_answer(header);

	printf("sent failure\n");
	return status;
}

/*
 * Sends an answer with set ACK flag and the wanted flags
 *
 * @param: *last_crc: the address of the last sent CRC-value
 * 		   id:	  	  the id of the received package
 * 		   flags: 	  the flags to send
 *
 * @return: The success or failure of sending the answer
 */
int send_success(uint8_t *last_crc, uint8_t id, uint8_t flags, uint8_t *calc_crc)
{
	printf("sending success...\n");
	//header array
	uint8_t header[HEADER_SIZE];

	//fill the header for the package to send
	*calc_crc = fill_header_for_empty_data(header, id, flags, last_crc);
	printf("success filling header for empty data\n");

	//send answer package
	printf("send answer package\n");
	int status = UART_answer(header);

	printf("sent success\n");
	return status;

}

/*
 * Sends the answer package with empty data
 *
 * @param: *header: the filled header array
 *
 * @return: The success or failure of sending the answer.
 */
int UART_answer(uint8_t *header)
{
	printf("UART answer...\n");
	uint8_t temp[BUFFER_SIZE] = {header[ID_POS], header[CRC_POS], header[DATA_SIZE_POS], header[FLAGS_POS]};

	//-> Network now
	//UART_Send(temp, 1);
	printf("send answer\n");
	if(send(sock, temp, BUFFER_SIZE, 0) != BUFFER_SIZE)
		puts("ERROR UARTAnswer");

	printf("success sending answer\n");
	return XST_SUCCESS;

}

/*
 * !!! �berarbeiten !!!
 * Data callback for received TC. Every subsystem gets his own command.
 *
 * @param: *header: pointer to the header array
 * 		   *databytes: pointer to the data array
 *
 * @return: Success or failure of the data callback
 *
 */
int recv_TC(uint8_t *header, uint8_t *databytes, int size_of_data)
{
	uint8_t id = header[ID_POS];
	FILE *fptr;

	switch(id)
	{
		case CAMERA_TC: break;
		case UART_TC: 	puts("--------------------------------------------------------------------------------");
						puts("--------------------------------------------------------------------------------");
						puts("Receiving done!");
						puts((char*) databytes);
						/*for(int i = 0; i < size_of_data; i ++)
							printf("%c", (char) databytes[i]);
						printf("\n");*/

						//fopen, fclose, Test to write on a document
						/*if((fptr = fopen("/Users/valentinstadtlander/Desktop/doc/Test.pdf", "wb")) == NULL)
							printf("Error opening File\n");
						fwrite(databytes, size_of_data, 1, fptr);
						fclose(fptr);*/
						break;
		case CPU_TC: break;
		case BRAM_TC: break;
		case DOWNLINK_TC: break;
		case DAC_TC: break;
		default: return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * !!! Noch zu deklarieren !!!
 * This method is used to store the received TM.
 *
 */
int recv_TM()
{

	return XST_SUCCESS;
}

/*
 *
 */
void default_operation()
{

}
