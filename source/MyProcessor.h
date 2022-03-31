#ifndef __MYPROCESSOR_H__
#define __MYPROCESSOR_H__

#include "HTTP.h"

class GeneralProcessor: public HttpProcessor {
public:
	GeneralProcessor (UserID& ui, HttpDesc& desc):
		HttpProcessor (ui, desc) { }

	string getErrorResponse (HttpDesc& responseDesc);
	bool processRequest (SocketInstance *si);
	static HttpProcessor *generalCreator (UserID ui, HttpDesc desc);
};

#endif
