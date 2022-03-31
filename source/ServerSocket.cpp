#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <netinet/in.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerSocket.h"

void
ServerSocket::openServer (short port)
{
	int fd;
	struct sockaddr_in sockaddr;

	memset (&sockaddr, 0, sizeof (struct sockaddr_in));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	sockaddr.sin_port = htons (port);

	mySocket.setSockAddr (&sockaddr);
	signal(SIGPIPE, SIG_IGN);
	try
	{
		fd = socket (PF_INET, SOCK_STREAM, 0);
		if (fd == -1)
			throw "Connection Error!";
		while (bind (fd, (struct sockaddr *)mySocket.getSockAddr (),
					sizeof (struct sockaddr_in)) == -1);
		printf ("Established\n");
		if (listen (fd, 5) == -1)
			throw "Listening Error!";
	}
	catch (const char *msg)
	{
		printf ("%s\n", msg);
		exit (-1);
	}
	mySocket.setSockfd (fd);
}

void
ServerSocket::closeServer (void) {
	mySocket.closeSocket ();
}

SocketInstance *
ServerSocket::acceptConnection (void)
{
	int cfd;
	struct sockaddr_in client_sockaddr;
	socklen_t temp = sizeof (struct sockaddr_in);
	SocketInstance *newSocket;

	temp = sizeof (struct sockaddr_in);
	try {
		cfd = accept (mySocket.getSockfd (),
					  (struct sockaddr *)&client_sockaddr,
					  &temp);
		if (cfd == -1) {
			throw (void *)0;
		}
	} catch (void *ptr) {
		return NULL;
	}

	newSocket = new SocketInstance (cfd, &client_sockaddr);
	return newSocket;
}
