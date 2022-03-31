#include <cstdio>
#include "Session.h"
#include "HTTP.h"

extern HttpMain hmain;

void *Session::run () {
	int idx;
	int len = sock->receiveRawData (_buf);
	int reslen = 0;
	string si = sock->getIP ();

	_buf[len] = '\0';
	printf ("User Instance #%d\n", id);

	hmain.processHttp (_buf, si, sock);
out:
	delete sock;
	return NULL;
}
