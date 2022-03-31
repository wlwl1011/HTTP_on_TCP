#include "ServerSocket.h"
#include "Session.h"
#include "HTTP.h"
#include "MyProcessor.h"
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <signal.h>

int cnt = 0;
char buf[4096];
ServerSocket mysock;
HttpMain hmain;

int main(int argc, char *argv[])
{
	mysock.openServer(9000);
	HttpCreator hc = GeneralProcessor::generalCreator;
	hmain.setCreator(hc);
	while (1)
	{
		SocketInstance *si;
		Session *session;
		si = mysock.acceptConnection();
		session = new Session(++cnt, si);
		session->start();
	}
	return 0;
}
