/*********************************************************************
    Abhinav Thirupathi
    Computer Project #12
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <bits/stdc++.h> 
#include <string>

#define BSIZE 128

/*-------------------------------------------------------------------- 
	Name: writeStdOut
    Purpose:  Displays contents of the file on the standard output stream
    Receive:  The buffer with the data to be displayed
    Return:   None
---------------------------------------------------------------------*/
void writeStdOut(const char buffer[])
{
	int wr;

	// If bytes < buffer size then byte count is written
	if (strlen(buffer) < BSIZE)
	{
		wr = write(1, buffer, strlen(buffer));
	}
	// Else the buffer size is written to the standard output
	else
	{
		wr = write(1, buffer, BSIZE);
	}
	
	// Error checks the write function
	if (wr < 0)
	{
		perror( "write" );
		exit(1);
	}
}


/*-------------------------------------------------------------------- 
	Name: main
    Purpose:  Processes the command line argument and calls correct functions
    Receive:  The number command line arguments, and array of the arguments
    Return:   Return 0
---------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{	
	// Checks for the correct number of command line args
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s <host> <port> <desired file>\n", argv[0] );
		exit( 1 );
	}
	
	// Gets and converts hostname in the command line to unsigned short int
	char * hostname = argv[1];
	unsigned short int port = atoi( argv[2] );

	// Creates the socket and error checks 
	int sd = socket( AF_INET, SOCK_STREAM, 0 );
	if (sd < 0)
	{
		perror( "socket" );
		exit( 2 );
	}
	
	// Gets the IP address of the hostname and error checks
	struct hostent * server = gethostbyname( hostname );
	if (server == NULL)
	{
		fprintf(stderr, "Error no such host %s\n", hostname );
		exit( 3 );
	}
	
	struct sockaddr_in saddr;
	
	bzero( &saddr, sizeof(saddr) );
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons( port );
	bcopy( server->h_addr, &saddr.sin_addr.s_addr, server->h_length );
	
	// Connect to the other process and error checks
	int connectServer = connect( sd, (struct sockaddr *) &saddr, sizeof(saddr) );
	if (connectServer < 0)
	{
		perror( "connect" );
		exit( 4 );
	}

	char buffer[BSIZE];	// Buffer for data to be sent and received
	
	// Initially stores the file name in the buffer
	strcpy(buffer, argv[3]);

	// Sends data to the server and error checks
	int sendData = send( sd, buffer, strlen(buffer), 0 );
	if (sendData < 0) 
	{
		perror( "send" );
		exit( 5 );
	}
	
	// Receives the data from the server and error checks
	bzero( buffer, BSIZE );
	int recvData = recv( sd, buffer, BSIZE, 0 );
	if (recvData < 0) 
	{
		perror( "recv" );
		exit( 4 );
	}
	
	// The server responds with 'SUCCESS', then client sends 'PROCEED'
	std::string tempStr = "SUCCESS";
	if (buffer == tempStr)
	{
		bzero( buffer, BSIZE );
		tempStr = "PROCEED";
		strcpy(buffer, tempStr.c_str());
		
		// Sends data to the server and error checks
		int sendData = send( sd, buffer, strlen(buffer), 0 );
		if (sendData < 0) 
		{
			perror( "send" );
			exit( 5 );
		}

		// Receives the contents of the file WHILE error checking
		bzero( buffer, BSIZE );
		recvData = recv( sd, buffer, BSIZE, 0 );
		if (recvData < 0) 
		{
			perror( "recv" );
			exit( 4 );
		}
		else
		{
			// Writes the contents of the file
			while(recvData > 0)
			{
			
				writeStdOut(buffer);
				bzero( buffer, BSIZE );
				recvData = recv( sd, buffer, BSIZE, 0 );
				if (recvData < 0) 
				{
					perror( "recv" );
					exit( 4 );
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "Server cannot open file '%s'\n",argv[3]);
	}
	
	// Closes the socket
	close( sd );
}
