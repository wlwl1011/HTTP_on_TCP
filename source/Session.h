#ifndef __SESSION_H__
#define __SESSION_H__

#include "Thread.h"
#include "ServerSocket.h"

class Session: public Thread {
private:
	int id;
	char _buf[4096];
	SocketInstance *sock;

public:
	Session (int id, SocketInstance *sock) {
		this->id = id;
		this->sock = sock;
	}
	~Session () { delete sock; }

	void *run ();

};

#endif
