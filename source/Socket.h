#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <netinet/in.h>

struct message
{
	short cmdno;
	short datasize;
	char talker[28];
};

class Socket
{
private:
	int sockfd;
	struct sockaddr_in sockaddr;

public:
	Socket () {}
	Socket (int fd, struct sockaddr_in *addr) {
		sockfd = fd;
		memcpy (&sockaddr, addr, sizeof (struct sockaddr_in));
	}

	void setSockfd (int sockfd) { 
		this->sockfd = sockfd;
	}
	int getSockfd (void) {
		return sockfd;
	}
	void setSockAddr (struct sockaddr_in *addr) {
		memcpy (&sockaddr, addr, sizeof (struct sockaddr_in));
	}
	struct sockaddr_in *getSockAddr (void) {
		return &sockaddr;
	}
	void closeSocket (void) {
		close (sockfd);
	}
	int readSocket (void *buf, int len)
	{
		int newlen;
		newlen = read (sockfd, buf, len);
		return newlen;
	}
	void writeSocket (const void *buf, int len)
	{
		const char *ptr = (const char *)buf;
		while (write (sockfd, ptr++, 1) >= 0);
	}
};

#endif
