/*
 * UART_EIVE_Protocol.c
 *
 *  Created on: 27.03.2020
 *      Author: Valentin & Tobias
 */

#include "UART_EIVE_Protocol.h"
//#include "UART_io.h"
#include "CRC.h"
//#include "UART_io.h"
//#include "xparameters.h"
#include "UART_EIVE_Protocol_Flags.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>


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
int UART_Send_Data(uint8_t ID, uint8_t *databytes, int dataLength)
{

	/*
	 * lastCRC_send saves the calculated CRC for the last send package
	 * submittedCRC always saves the new received CRC
	 * lastCRC_rcv saves the last received CRC for the initval for checking the received package
	 */
	uint8_t lastCRC_send = 0x00, lastCRC_rcvd = 0x00;

	int status;

	printf("preconnect, lastCRC_send address: %p\n", &lastCRC_send);
	//connection establishment
	status = connect_(ID, databytes, dataLength, &lastCRC_send, &lastCRC_rcvd);

	puts("connect");
	//return Failure if connection could not be established
	if(status == XST_FAILURE)
		return XST_FAILURE;

	//send data
	puts("before send_data");
	status = send_data(ID, databytes, dataLength, &lastCRC_send, &lastCRC_rcvd);
	printf("after send_data, returnValue: %i\n", status);


	//return failure if the data could not be send
	if(status == XST_FAILURE)
		return XST_FAILURE;


	return XST_SUCCESS;

}

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
int connect_(uint8_t ID, uint8_t *databytes, uint8_t dataLength, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd)
{

	//int packageCount = package_count(dataLength);
	//uint8_t temp[BUFFER_SIZE * packageCount];
	uint8_t temp32[BUFFER_SIZE] = {0};
	uint8_t header[HEADER_SIZE] = {0};
	uint8_t data[PACKAGE_DATA_SIZE];

	uint8_t submittedCRC = INIT_CRC;

	int status;

	int connection = NACK;
	int conn_counter = 0;

	while(connection != ACK && conn_counter < 10)
	{
		uint8_t snd_flags = UNSET_ALL_FLAGS;
		uint8_t rcv_flags = UNSET_ALL_FLAGS;

		puts("before req2send");
		//Request to send, CRC initval = 0x00
		*lastCRC_send = INIT_CRC;
		status = send_request_to_send(ID, temp32, lastCRC_send, &snd_flags);

		puts("nach req2send");

		//check status of sending
		if(status != XST_SUCCESS)
			return XST_FAILURE;

		int acknowledge = NACK; //for CRC
		int succes = 1;

		while(acknowledge != ACK)
		{
			if(succes == 1)
			{
				printf("Wait on answer a\n");
				//wait on answer sending again temp32
				printf("Value: %i\n", wait_on_answer(temp32, ID, lastCRC_send));
			}
			else
			{
				printf("Wait on answer b\n");
				//wait on answer sending again NACK
				printf("Value: %i\n", wait_on_answer(NULL, ID, lastCRC_send));
			}

			printf("connect: get_received_data\n");
			//fill header, data, receive flags and submittedCRC with the received values
			get_received_data(header, data, &rcv_flags, &submittedCRC);

			//check received CRC
			printf("Check crc in connect\n");
			printf("Next crc check: Initval -> Last_CRC_rcvd: %i\n", *lastCRC_rcvd);
			if(check_crc(submittedCRC, RecvBuffer, *lastCRC_rcvd)!= XST_SUCCESS)
			{
				//CRC values defeer, send failure
				send_failure(lastCRC_send, ID, lastCRC_send);

			}
			else
			{
				acknowledge = ACK;

				//wird in check_crc übernommen
				//*lastCRC_rcvd = submittedCRC; //Test
			}
		}

		//set ACK = 1
		printf("Set ack flag in connect\n");
		set_ACK_Flag(&snd_flags, ACK);

		//Received CRC is correct
		//check ACK

		printf("get ack flag\n");
		if(get_ACK_flag(rcv_flags) == SET)
		{

				//check ready to receive
			printf("get ready to rcv flag\n");
				if(get_ready_to_recv_flag(rcv_flags) == SET)
				{
					//send_data();
					connection = ACK;
					*lastCRC_rcvd = submittedCRC; //Test
				}
				else
				{
					//NOT ready to receive
					conn_counter++;
				}
			}
		}

	printf("return at connect: conn_counter:%i\n", conn_counter);
	if(conn_counter == 10)
		return XST_FAILURE;


	return XST_SUCCESS;
}

/*
 *Request to send, to establish a connection
 *
 *@param:	ID			Identification number of the package to send
 *@param:	*lastCRC	Pointer, last CRC value to save the new CRC value for the next package
 *
 *Configures a package to send a request to send and saves the first CRC
 */
int send_request_to_send(uint8_t ID, uint8_t *temp32, uint8_t *lastCRC_send, uint8_t *flags)
{
	puts("asd");

	if(*flags == 0x00)
	{
		*flags = REQ_TO_SEND_MASK;
		printf("flags");
	}

	puts("ID_POS"); //try puts
	temp32[ID_POS] = ID;

	puts("DATA_SIZE_POS");
	temp32[DATA_SIZE_POS] = 0;

	puts("FLAG_POS");
	temp32[FLAGS_POS] = *flags;


	printf("Test: %p\n", lastCRC_send);//, temp32[0]);
	printf("Test2: %i\n", temp32[0]);

	printf("Next crc calc: Initval -> Last_CRC_send: %i\n", *lastCRC_send);
	int crc = calc_crc8(temp32, *lastCRC_send);

	*lastCRC_send = crc; //Kann auf nach gesetztem ACK-Flag verschoben werden.

	puts("nach crc");

	temp32[CRC_POS] = *lastCRC_send;
	//(*lastCRC_send) = newCRC;

	int status = XST_FAILURE;
	int try = 0;

	puts("vor while");
	while(status != XST_SUCCESS)
	{
		// -> Network now
		//status = UART_Send(temp32, 1);
		printf("SEND DATA!!!!!\n");
		status = XST_SUCCESS;
		if(send(sock, temp32, BUFFER_SIZE, 0) != BUFFER_SIZE)
		{
			status = XST_FAILURE;
			puts("ERROR sendreq2send");
		}

		puts("while");
		if(try == 50)
		{
			printf("try == 50\n");
			return XST_FAILURE;
		}

		try++;
	}

	return XST_SUCCESS;
}

/*
 * Package counter
 *
 * @param:	dataLength	number of bytes of the data to send
 *
 * returns the number of the needed packages to send all the databytes
 */
int package_count(int dataLength)
{

	if (dataLength % PACKAGE_DATA_SIZE > 0)
		return (dataLength / PACKAGE_DATA_SIZE + 1);
	else
		return (dataLength / PACKAGE_DATA_SIZE);
}

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
void get_received_data(uint8_t *header, uint8_t *data, uint8_t *flags, uint8_t *submittedCRC)
{
	printf("getReceivedData\n");
	extract_header(RecvBuffer, header, data);
	*flags = header[FLAGS_POS];
	*submittedCRC = header[CRC_POS];
}

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
int send_data(uint8_t ID, uint8_t *databytes, int dataLength, uint8_t *lastCRC_send, uint8_t *lastCRC_rcvd)
{
	uint8_t send_array[BUFFER_SIZE];
	uint8_t packageCount = package_count(dataLength);
	uint8_t header[HEADER_SIZE];
	uint8_t data[PACKAGE_DATA_SIZE];
	uint8_t flags;
	uint8_t submittedCRC;
	uint8_t temp[BUFFER_SIZE * packageCount];
	int crc = 0;
	int status = XST_SUCCESS;

	printf("fill packages\n");
	//fill array temp with the databytes and the header to send
	fill_packages(ID, dataLength, databytes, temp, packageCount);

	int package_counter = 0;
	int try = 0;

	while(package_counter < packageCount && try <= 10)
	{
		printf("while package_counter < packageCount: %i\n", packageCount);
		//Get packages
		for(int i = 0; i < BUFFER_SIZE; i++)
			send_array[i] = temp[package_counter * BUFFER_SIZE + i];

		printf("setAckflag\n");
		//Set acknowledge flag
		set_ACK_Flag(&send_array[FLAGS_POS], ACK);

		printf("before crc in while\n");
		printf("Next crc calc: Initval -> Last_CRC_send: %i\n", *lastCRC_send);
		//Calculate CRC value
		crc = calc_crc8(send_array, *lastCRC_send);

		send_array[CRC_POS] = crc;

		printf("after crc in while\n");
		//Send package -> Network now
		//status = UART_Send(send_array, 1);
		if(send(sock, send_array, BUFFER_SIZE, 0) != BUFFER_SIZE)
			puts("EEROR");

		printf("after send in while\n");
		if(status != XST_SUCCESS)
		{
			try++;
			continue;
		}

		//Wait on acknowledge package and check
		uint8_t acknowledge = NACK;
		int succes = 1;

		while(acknowledge != ACK)
		{
			printf("wait on ack in send_data\n");
			//wait on receive buffer to be filled
			if(succes == 1)
			{
				printf("SendArray\n");
				wait_on_answer(send_array, send_array[ID_POS], &send_array[CRC_POS]);
			}
			else
			{
				//wait_on_answer with NACK
				wait_on_answer(NULL, ID, lastCRC_send);
				printf("Send NACK-Package\n");
			}

			//get received information
			get_received_data(header, data, &flags, &submittedCRC);

			printf("Next crc check: Initval -> Last_CRC_rcvd: %i\n", *lastCRC_rcvd);
			//check received CRC
			if(check_crc(submittedCRC, RecvBuffer, *lastCRC_rcvd)!= XST_SUCCESS)
			{
				send_failure(lastCRC_send, ID, &crc);
				succes = 0;
			}
			else
			{
				acknowledge = ACK;

				//wird von check_crc übernommen
				*lastCRC_rcvd = submittedCRC; //Test
			}
		}

		printf("crc correct and get ack\n");
		//Received CRC is correct
		//check ACK
		if(get_ACK_flag(flags) != SET)
		{
			/* CRC is correct, NACK */
			printf("CRC corr, received NACK");
			continue;
		}

		printf("get last crc sent \n");
		*lastCRC_send = send_array[CRC_POS];

		package_counter++;
	}

	if(try == 10)
		return XST_FAILURE;

	return XST_SUCCESS;
}

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
int wait_on_answer(uint8_t *send_array, uint8_t ID, uint8_t *lastCRC_send)
{
	uint8_t nack_header[BUFFER_SIZE] = {0};

	if(NULL == send_array)
	{
		*lastCRC_send = fill_header_for_empty_data(nack_header, ID, UNSET_ALL_FLAGS, lastCRC_send);
	}

	int status = XST_NO_DATA;
	int timer;

	while(status != XST_SUCCESS)
	{
		printf("Wait for answer: while\n");
		// -> Network now
		//status = UART_Recv_Buffer();
		if(recv(sock, RecvBuffer, BUFFER_SIZE, 0) < 0)
			puts("ERROR waitOnAnswer1");


		status = XST_SUCCESS; //test

		printf("Answer received!!: \n");
		if(status != XST_NO_DATA && status != XST_SUCCESS)
			return XST_FAILURE;

		timer++;

		if(timer == MAX_TIMER)
		{
			//Timeout
			//send again array to send
			if(NULL == send_array)
			{
				printf("Timer is max timer\n");
				uint8_t temp[BUFFER_SIZE] = {nack_header[ID_POS], nack_header[CRC_POS], nack_header[DATA_SIZE_POS], nack_header[FLAGS_POS]};
				// -> Network now
				//UART_Send(temp, 1);
				if(send(sock, temp, BUFFER_SIZE, 0) != BUFFER_SIZE)
					puts("ERROR waitOnAnswer2");
			}
			else
			{
				// -> Network now
				//UART_Send(send_array, 1);
				if(send(sock, send_array, BUFFER_SIZE, 0) != BUFFER_SIZE)
					puts("ERROR waitOnAnswer3");
			}

			//reset timer
			timer = 0;
		}
	}
	return XST_SUCCESS;
}


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
void fill_packages(uint8_t ID, int dataLength, uint8_t *databytes, uint8_t *temp, int packageCount)
{
	/*Temporary arrays for header and data*/
	//uint8_t header[4];

	printf("Fill packages!!\n");
	uint8_t header[HEADER_SIZE] = {ID, INIT_CRC, 0, UNSET_ALL_FLAGS};

	//uint8_t flags = UNSET_ALL_FLAGS;

	for (int i = 0; i < packageCount; i++)
	{
		/*first package*/
		if(i == 0)
		{
			printf("fill header first pkg\n");
			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = PACKAGE_DATA_SIZE;

			printf("Set start flag\n");
			/*
			 * fill header with the given information
			 * flags for the start package
			 */
			set_Start_Flag(&header[FLAGS_POS], SET);

			//Setting end-flag if only one package will be send
			if(packageCount == 1)
				set_End_Flag(&header[FLAGS_POS], SET);

			printf("Start flag set\n");

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[k] = header[k];
			}

			for (int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				/*fill temporary arrays temp*/
				temp[j] = databytes[j - HEADER_SIZE];
			}
		}

		/*all packages except the first and the last one*/
		else if (i > 0 && i != packageCount - 1)
		{
			printf("fill header %i. pkg\n", i);

			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = PACKAGE_DATA_SIZE;

			/*
			 * fill header with the given information*/
			/* flags for the middle packages
			 */
			//flags = 0b00000000; //anpassen, ACK flag ist gesetzt!!!
			set_Start_Flag(&header[FLAGS_POS], NOT_SET);


			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[i * BUFFER_SIZE + k] = header[k];
			}

			/*fill temporary arrays temp and temp28*/
			for (int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				temp[i * BUFFER_SIZE + j] = databytes[i * PACKAGE_DATA_SIZE + j - HEADER_SIZE];
			}
		}

		/*last package*/
		else
		{
			int restsize = dataLength - PACKAGE_DATA_SIZE * (packageCount - 1);

			printf("fill header last pkg\n");

			//Fill header[DATA_SIZE_POS]
			header[DATA_SIZE_POS] = restsize;

			/*fill header with the given information*/
			/*flags for the end package*/
			set_End_Flag(&header[FLAGS_POS], SET);

			/*fill temporary array temp with the headers*/
			for (int k = 0; k < HEADER_SIZE; k++)
			{
				temp[i * BUFFER_SIZE + k] = header[k];
			}


			for(int j = HEADER_SIZE; j < BUFFER_SIZE; j++)
			{
				/*fill temp and temp28*/
				if(j < restsize)
				{
					/*fill with the rest databytes from position 0 to restsize*/
					temp[i * BUFFER_SIZE + j] = databytes[i * PACKAGE_DATA_SIZE + j - HEADER_SIZE];
				}
				else
				{
					/*fill with 0 from position restsize to 28*/
					temp[i * BUFFER_SIZE + j] = 0;
				}
			}


		}
	}

}

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
uint8_t fill_header_for_empty_data(uint8_t *header, uint8_t ID, uint8_t flags, uint8_t *lastCRC_send)
{
	printf("fill header for empty dataaa\n");
	uint8_t temp_array_CRC[BUFFER_SIZE] = {0};
	temp_array_CRC[ID_POS] = ID;
	temp_array_CRC[FLAGS_POS] = flags;

	printf("Next crc calc: Initval -> Last_CRC_send: %i\n", *lastCRC_send);
	/*calculate new CRC value*/
	uint8_t newCRC = calc_crc8(temp_array_CRC, *lastCRC_send);

	/*save new CRC value in old variable*/
	//(*lastCRC_send) = newCRC;

	/*fill header*/
	header[ID_POS] = ID;
	header[CRC_POS] = newCRC;
	header[DATA_SIZE_POS] = EMPTY_DATA_LENGTH;
	header[FLAGS_POS] = flags;

	return newCRC;
}

