/*********************************************************************
    Abhinav Thirupathi
    Computer Project #12
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
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
	Name: main
    Purpose:  Processes the command line argument and calls correct functions
    Receive:  The number command line arguments, and array of the arguments
    Return:   Return 0
---------------------------------------------------------------------*/
int main( int argc, char* argv[] )
{

	// Checks for the correct number of command line args
	if (argc != 1)
	{
    	fprintf( stderr, "Usage: %s\n", argv[0] );
    	exit( 1 );
  	}

	// Ask the OS to create socket and error checks 
  	int listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
  	if (listen_sd < 0) 
  	{
  	  perror( "socket" );
 	   exit( 2 );
  	}

	// Record "saddr" will be used to communicate with OS
	
  	struct sockaddr_in saddr;
	
  	bzero( &saddr, sizeof(saddr) );				// Sets all bytes of record to zero
  	saddr.sin_family = AF_INET;					// Sets family to AF_INET (Internet)
  	saddr.sin_port = 0;				// Sets port to zero (let OS select port)
  	saddr.sin_addr.s_addr = htonl( INADDR_ANY );// Sets address to default IP address of host
  	
	// Asks the OS to bind socket to port on host and error checks
  	int bstat = bind( listen_sd, (struct sockaddr *) &saddr, sizeof(saddr) );
  	if (bstat < 0)
  	{
  	  perror( "bind" );
  	  exit( 3 );
  	}
  	
  	// Ask OS to return info about socket
  	socklen_t saddr_size = sizeof(saddr);
  	getsockname( listen_sd, (struct sockaddr *) &saddr, &saddr_size );
  	
  	// Asks for the host name 
  	char hostname[256];
	gethostname( hostname, 256 );

	struct hostent* host;
	host = gethostbyname( hostname );
  	
  	// Display the host name and port number
	printf( "\n%s %d \n\n", host->h_name, ntohs(saddr.sin_port));
	
	// Listen for the incoming request and error checks
	int lstat = listen( listen_sd, 5 );
  	if (lstat < 0)
  	{
  	  perror( "listen" );
  	  exit( 4 );
  	}
  	
  	while (1)
  	{
		struct sockaddr_in caddr;
    	unsigned int clen = sizeof(caddr);

		// Accept the incoming request and error check
    	int comm_sd = accept( listen_sd, (struct sockaddr *) &caddr, &clen );
    	if (comm_sd < 0)
    	{
      		perror( "accept" );
      		exit( 5 );
    	}
    	
    	char buffer[BSIZE];		// Buffer for data to be sent and received
    	bzero( buffer, BSIZE );
    	
    	// Receives the file name to be opened and error checks
    	int recvData = recv( comm_sd, buffer, BSIZE, 0 );
    	if (recvData < 0) 
    	{
      		perror( "recv" );
      		exit( 8 );
    	}
    	
    	// Open the file and error checks
    	int openFile = open(buffer,O_RDONLY);
    	bzero( buffer, BSIZE );
    	// Opening the file fails
    	if(openFile == -1)
    	{
			std::string tempStr = "FAILURE";
			strcpy(buffer, tempStr.c_str());
    	}
    	// Opening the file succeeds
    	else 
    	{
    		std::string tempStr = "SUCCESS";
			strcpy(buffer, tempStr.c_str());
			
			// Sends 'SUCCESS' message to the client and error checks
			int sendData = send(comm_sd, buffer, strlen(buffer), 0 );
			if (sendData < 0) 
			{
		  		perror( "send" );
		  		exit( 9 );
			}
			else 
			{
				bzero( buffer, BSIZE );
				int recvData = recv( comm_sd, buffer, BSIZE, 0 );
				if (recvData < 0) 
				{
					perror( "recv" );
					exit( 8 );
				}
				else
				{
					// Client responds with proceed
					tempStr = "PROCEED";
					if (buffer == tempStr)
					{
						// File contents read and send to client 
						bzero( buffer, BSIZE );
						int readData = read(openFile, buffer, BSIZE);
						while(readData != 0)
						{
							// If bytes < buffer size then byte count is written
							if (readData < BSIZE)
							{
								sendData = send(comm_sd, buffer, readData, 0 );
							}
							// Else the buffer size is written to the destination file
							else
							{
								sendData = send(comm_sd, buffer, BSIZE, 0 );
							}
							// Error checks send
							if (sendData < 0) 
							{
						  		perror( "send" );
						  		exit( 9 );
							}
							readData = read(openFile, buffer , BSIZE);
						}
						// Error check read
						if (readData < 0) 
						{
							perror( "recv" );
							exit( 8 );
						}
					}
				}
			}
    	}
		// Closes the communication socket
    	close( comm_sd );
  	}
}
