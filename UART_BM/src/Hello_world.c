/**

 *
 *
 *
 *
 *
 */

/************************************Include Files************************************
#include "stdio.h"
#include "xparameters.h"
#include "xuartps.h"
//#include "xil_printf.h"
#include "UART_io.h"
#include "UART_EIVE_Protocol.h"
/*************************************************************************************




int UART_Send_Hello_World(XUartPs *UartDevice);
int UART_Recv_Hello_World(XUartPs *UartDevice);
int maiin(void);
/**
 * Main Method for UART
 *
int maiin(void)
{
	Initialize_UART_Device(UART0_DEVICE_ID);
	while(1)
	{
		UART_Recv_Hello_World(&Uart_Ps);
	}
}


/**
<<<<<<< Updated upstream
 * UART_Hello_World Method for sending
=======
 * UART_Hello_World Methods
>>>>>>> Stashed changes
 *

int UART_Send_Hello_World(XUartPs *UartDevice)
{
	int status;

	for(int i = 0; i < 31; i++)
	{
		SendBuffer[i] = i + 'A';
	}

	status = UART_Send_Buffer(SendBuffer);

	if(status != XST_SUCCESS)
		return XST_FAILURE;

	return XST_SUCCESS;
}

/*
<<<<<<< Updated upstream
 * UART_Hello_World Method for receiving
=======
 * Receiving method
 *
>>>>>>> Stashed changes
 *
int UART_Recv_Hello_World(XUartPs *UartDevice)
{
	UART_Recv_Buffer();
	UART_Send_Buffer(RecvBuffer);
	return XST_SUCCESS;
}
*/
