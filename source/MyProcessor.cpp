#include "MyProcessor.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>

using namespace std;

string toString (int num) {
	stringstream ss;
	ss << num;
	return ss.str ();
}

string GeneralProcessor::
getErrorResponse (HttpDesc& responseDesc) {
	string ret;
	string r = "<html><body>Error</body></html>";
	responseDesc.setValueMap (CONTENT_LENGTH, toString (r.length ()));
	ret = responseDesc.makeHeader (toString (404), "Bad Access");
	ret += r;

	return ret;
}

bool GeneralProcessor::
processRequest (SocketInstance *si) {
	int idx, pos;
	HttpDesc retdesc;
	string uri = myDesc.getURI ();
	string reladdr = "." + uri;
	string root = reladdr.substr (0, reladdr.find ("?"));
	string ret = "";

	retdesc.setValueMap (SERVER, "GeneralProcessor");
	retdesc.setValueMap (CONTENT_TYPE, "text/html; charset=utf-8");

	if (uri == "/") {
		root = "./index.htm";
	}
	ifstream in;
	in.open (root.c_str ());

	cout << root << endl;
	if (!in) {
		ret = getErrorResponse (retdesc);
	} else {
		in.seekg (0, ios::end);
		int size = in.tellg ();
		char *buf = new char[size + 1];
		in.seekg (0, ios::beg);
		in.read (buf, size);
		buf[size] = '\0';
		string filecont = buf;

		retdesc.setValueMap (CONTENT_LENGTH, toString (filecont.length ()));
		ret = retdesc.makeHeader (toString (200), "OK");
		ret += filecont;
	}

	cout << ret << endl;
	si->sendString (ret);
	return myDesc.getConnection ();
}

HttpProcessor *GeneralProcessor::
generalCreator (UserID ui, HttpDesc desc) {
	GeneralProcessor *mp = new GeneralProcessor (ui, desc);
	return mp;
}
