#ifndef __REQHEADER_H__
#define __REQHEADER_H__

#include <iostream>
#include <map>
#include <list>
#include <cstring>

#include "ServerSocket.h"
#include "Semaphore.h"

using namespace std;

struct req_line {
	char method[10];
	char *uri;
	char version[10];
};

/* method */
enum { OPTIONS = 0, GET, HEADER, POST, PUT, DELETE, TRACE, CONNECT };
enum {
	/* General Header */
	CACHE_CONTROL, CONNECTION, DATE, PRAGMA,
	TRAILER, TRANSFER_ENCODING, UPGRADE, VIA, WARNING,
	/* Request Header */
	ACCEPT, ACCEPT_CHARSET, ACCEPT_ENCODING, ACCEPT_LANGUAGE,
	AUTHORIZATION, EXPECT, FROM, HOST, IF_MATCH,
	IF_MODIFIED_SINCE, IF_NONE_MATCH, IF_RANGE, IF_UNMODIFIED_SINCE,
	MAX_FORWARDS, PROXY_AUTHORIZATION, RANGE, REFERER, TE, USER_AGENT,
	/* Response Header */
	ACCEPT_RANGES, AGE, ETAG, LOCATION, PROXY_AUTHENTICATE, RETRY_AFTER,
	SERVER, VARY, WWW_AUTHENTICATE,
	/* Entity Header */
	ALLOW, CONTENT_ENCODING, CONTENT_LANGUAGE, CONTENT_LENGTH,
	CONTENT_LOCATION, CONTENT_MD5, CONTENT_RANGE, CONTENT_TYPE,
	EXPIRES, LAST_MODIFIED, X_PURPOSE
};

extern const char *req_version;
extern const char *req_method[];
extern const char *req_header[];

#define REQUEST_HEADER	9
#define RESPOSE_HEADER	(REQUEST_HEADER + 19)
#define ENTITY_HEADER	(RESPOSE_HEADER + 9)
#define TOTAL_HEADER	(ENTITY_HEADER + 11)

struct userid {
	char ip[20];
	char agent[200];
};

/*--------------------------------------------------------------*/
class UserID {
private:
	string ip;
	string agent;

public:
	UserID () {}
	UserID (string ip, string agent) {
		this->ip = ip;
		this->agent = agent;
	}

	~UserID () { }

	string getIP () { return ip; }
	string getAgent () { return agent; }
	string setIP (string ip) { this->ip = ip; }
	string setAgent (string agent) { this->agent = agent; }

	UserID operator= (UserID& ui) {
		ip = ui.getIP ();
		agent = ui.getAgent ();
		return *this;
	}

	bool operator== (UserID& ui) {
		if (ip == ui.getIP ()
			&& agent == ui.getAgent ()) {
			return true;
		}
		
		return false;
	}

	bool operator!= (UserID& ui) {
		return !(*this == ui);
	}
};
/*--------------------------------------------------------------*/
class HttpDesc {
private:
	int method;
	string uri;
	string valuemap[TOTAL_HEADER];
	string message;

public:
	HttpDesc () {}
	HttpDesc (int method, string uri, string vm[TOTAL_HEADER], string message) {
		this->method = method;
		this->uri = uri;
		for (int i = 0; i < TOTAL_HEADER; i++) {
			this->valuemap[i] = vm[i];
		}
		this->message = message;
	}


	HttpDesc& operator= (HttpDesc hd) {
		string *ptr;
		method = hd.getMethod ();
		uri = hd.getURI ();
		ptr = hd.getValueMapPtr ();
		for (int i = 0; i < TOTAL_HEADER; i++) {
			valuemap[i] = ptr[i];
		}
		message = hd.getMessage ();
	}

	int getMethod () { return method; }
	string getURI () { return uri; }
	string *getValueMapPtr () { return valuemap; }
	string getMessage () { return message; }

	void setValueMap (int num, const char *str) {
		valuemap[num] = str;
	}

	void setValueMap (int num, string str) {
		valuemap[num] = str;
	}

	bool getConnection () {
		if (valuemap[CONNECTION] == "keep-alive"
			|| valuemap[CONNECTION] == "Keep-Alive") { return true; }
		return false;
	}

	string& getUserAgent (void) {
		return valuemap[USER_AGENT];
	}

	string makeHeader (string number, string sign) {
		string sp = " ", crlf = "\r\n", sep = ": ";
		string retstr = req_version + sp + number + sp + sign + crlf;
		for (int i = 0; i < TOTAL_HEADER; i++)
		{
			if (valuemap[i] != "") {
				retstr += req_header[i] + sep + valuemap[i] + crlf;
			}
		}
		retstr += "\r\n";
		return retstr;
	}
};

/*--------------------------------------------------------------*/
class HttpParser {
private:
	int getHeaderNumber (string head);
	void analysisRequestLine (string line, int *, string&);
	void analysisRequestBody (string body, string *vm);

public:
	HttpDesc analysisRequest (char *buf);
};

/*--------------------------------------------------------------*/
class HttpProcessor {
protected:
	HttpDesc myDesc;
	UserID myID;
public:

	HttpProcessor (UserID ui, HttpDesc desc) {
		myDesc = desc;
		myID = ui;
	}

	bool isSame (UserID ui) {
		if (myID == ui) {
			return true;
		}
		return false;
	}

	void setHttpDescriptor (HttpDesc desc) {
		myDesc = desc;
	}

	virtual bool processRequest (SocketInstance *si) {}
};
/*--------------------------------------------------------------*/
typedef list<HttpProcessor *> HttpList;
typedef HttpProcessor *(*HttpCreator)(UserID, HttpDesc);

class HttpMain {
private:
	HttpList hlists;
	Semaphore hlsema;

	HttpParser parser;
	HttpCreator hpcreator;
public:
	HttpMain () {}
	~HttpMain ();	/* conns 다 죽여 */

	void setCreator (HttpCreator cr) { hpcreator = cr; }
	HttpProcessor *getProcessor (UserID ui, HttpDesc hd);
	void removeHPFromList (HttpProcessor *hp);
	void processHttp (char *buf, string ip, SocketInstance *);
};

#endif
