/*
 * Server.c
 *
 *  Created on: 17.04.2020
 *      Author: valentinstadtlander
 */


/* server.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "UART_EIVE_Protocol.h"

#ifdef _WIN32
/* Headerfiles für Windows */
#include <winsock.h>
#include <io.h>

#else
/* Headerfiles für UNIX/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

/* Portnummer */
#define PORT 1234

/* Puffer für eingehende Nachrichten */
#define RCVBUFSIZE 1024

#ifdef _WIN32
   static void echo(SOCKET);
#else
   static void echo( int );
#endif

static void error_exit(char *errorMessage);

/* Die Funktion gibt Daten vom Client auf stdout aus,
 * die dieser mit der Kommandozeile übergibt. */
#ifdef _WIN32
static void echo(SOCKET client_socket)
#else
static void echo(int client_socket)
#endif
{
    uint8_t echo_buffer[RCVBUFSIZE];
    int recv_size;
    time_t zeit;
printf("block!\n");
    if((recv_size =
            recv(client_socket, echo_buffer, RCVBUFSIZE,0)) < 0)
        error_exit("Fehler bei recv()");
    echo_buffer[recv_size] = '\0';
    time(&zeit);
    printf("Nachrichten vom Client : %s \t%s",
            echo_buffer, ctime(&zeit));
}

/* Die Funktion gibt den aufgetretenen Fehler aus und
 * beendet die Anwendung. */
static void error_exit(char *error_message) {

#ifdef _WIN32
    fprintf(stderr,"%s: %d\n", error_message, WSAGetLastError());
#else
    fprintf(stderr, "%s: %s\n", error_message, strerror(errno));
#endif

    exit(EXIT_FAILURE);
}

int maiin( int argc, char *argv[]);

int main( int argc, char *argv[]) {
    struct sockaddr_in server, client;

#ifdef _WIN32
    SOCKET sock, fd;
#else
    int Ssock;
	//int fd;
#endif

    unsigned int len;

#ifdef _WIN32
    /* Initialisiere TCP für Windows ("winsock"). */
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD (1, 1);
    if (WSAStartup (wVersionRequested, &wsaData) != 0)
        error_exit( "Fehler beim Initialisieren von Winsock");
    else
        printf("Winsock initialisiert\n");
#endif

    /* Erzeuge das Socket. */
    Ssock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Ssock < 0)
        error_exit("Fehler beim Anlegen eines Sockets");

    /* Erzeuge die Socketadresse des Servers. */
    memset( &server, 0, sizeof (server));
    /* IPv4-Verbindung */
    server.sin_family = AF_INET;
    /* INADDR_ANY: jede IP-Adresse annehmen */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Portnummer */
    server.sin_port = htons(PORT);

    /* Erzeuge die Bindung an die Serveradresse
     * (genauer: an einen bestimmten Port). */
    if(bind(Ssock,(struct sockaddr*)&server, sizeof( server)) < 0)
        error_exit("Kann das Socket nicht \"binden\"");

    /* Teile dem Socket mit, dass Verbindungswünsche
     * von Clients entgegengenommen werden. */
    if(listen(Ssock, 5) == -1 )
         error_exit("Fehler bei listen");

    printf("Server bereit! - wartet auf Anfragen ...\n");
    /* Bearbeite die Verbindungswünsche von Clients
     * in einer Endlosschleife.
     * Der Aufruf von accept() blockiert so lange,
     * bis ein Client Verbindung aufnimmt. */
    for (;;) {
        len = sizeof(client);
        sock = accept(Ssock, (struct sockaddr*)&client, &len);
        if (sock < 0)
            error_exit("Fehler bei accept");
        printf("Bearbeite den Client mit der Adresse: %s\n",
           inet_ntoa(client.sin_addr));
        //*Daten vom Client auf dem Bildschirm ausgeben */
        //echo( fd );

        int status = UART_Recv_Data();

        if(status == XST_FAILURE)
        	printf("%i\n", status);



        /*int status;
        int try = 0;
        while((status = UART_Recv_Data()) != 0 && try != 50)
        {
        	printf("Status: %i, ", status);
        	printf("Try: %i\n", try);
        	try++;
        }

        /* Schließe die Verbindung. */
#ifdef _WIN32
        closesocket(fd);
#else
        close(sock);
#endif
    }
    return EXIT_SUCCESS;
}


