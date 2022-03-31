#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

#include "Socket.h"

#include <iostream>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class SocketInstance {
private:
	Socket mySock;

public:
	SocketInstance (int fd, struct sockaddr_in *addr) {
		mySock.setSockfd (fd);
		mySock.setSockAddr (addr);
	}
	~SocketInstance () { mySock.closeSocket (); }
	void receiveMessage (struct message *msg, char *buf) {
		mySock.readSocket (msg, sizeof (struct message));
		mySock.readSocket (buf, msg->datasize);
	}

	char *getIP () {
		return inet_ntoa (mySock.getSockAddr ()->sin_addr);
	}

	void sendMessage (short cmdno, const char *talker, const char *buf) {
		struct message msg;

		msg.cmdno = cmdno;
		msg.datasize = strlen (buf) + 1;

		if (talker) {
			memcpy (msg.talker, talker, 28);
		}

		mySock.writeSocket (&msg, sizeof (struct message));
		mySock.writeSocket (buf, msg.datasize);
	}

	int receiveRawData (char *buf) {
		int len = mySock.readSocket (buf, 4096);
		return len;
	}

	void sendRawData (const char *buf, int len) {
		mySock.writeSocket (buf, len);
	}

	void sendString (string& str) {
		char len = str.length () + 1;
		sendRawData (str.c_str (), len);
	}
};

class ServerSocket
{
private:
	Socket mySocket;

public:
	ServerSocket () {}
	~ServerSocket () { mySocket.closeSocket (); }

	void openServer (short port);
	void closeServer (void);
	SocketInstance *acceptConnection (void);
};
#endif
