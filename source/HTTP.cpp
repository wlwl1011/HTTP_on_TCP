#include "HTTP.h"
#include <iostream>
#include <string>
#include <list>

using namespace std;

/*
	Request = Request-Line			;
			*(( general-header		;
			| request-header		;
			| entity-header ) CRLF	;
			CRLF					;
			[ message-body 			;
 */

/*
 */
const char *req_version = "HTTP/1.1";

const char *req_method[] = {
	"OPTIONS", "GET", "HEADER", "POST",
	"PUT", "DELETE", "TRACE", "CONNECT",
};

const char *req_header[] = {
	/*general-header*/
	"Cache-Control", "Connection", "Date",
	"Pragma", "Trailer", "Transfer-Encoding",
	"Upgrade", "Via", "Warning",
	/*request-header*/
	"Accept", "Accept-Charset", "Accept-Encoding",
	"Accept-Language", "Authorization", "Expect",
	"From", "Host", "If-Match",
	"If-Modified-Since", "If-None-Match",
	"If-Range", "If-Unmodified-Since",
	"Max-Forwards", "Proxy-Authorization",
	"Range", "Referer", "TE", "User-Agent",
	/*response-header*/
	"Accept-Ranges", "Age", "ETag", "Location",
	"Proxy-Authenticate", "Retry-After",
	"Server", "Vary", "WWW-Authenticate",
	/*entity-header*/
	"Allow", "Content-Encoding", "Content-Language",
	"Content-Length", "Content-Location", "Content-MD5",
	"Content-Range", "Content-Type", "Expires",
	"Last-Modified", "X-Purpose"
};

void HttpParser::
analysisRequestLine (string line, int *method, string &uri) {
	int pos, index;
	string _method, ver;
	pos = line.find (" ", 0);
	_method = line.substr (0, pos);
	index = pos + 1;
	pos = line.find (" ", index);
	uri = line.substr (index, pos - index);
	index = pos + 1;
	ver = line.substr (index);

	for (int i = 0; i < 8; i++) {
		if (_method == req_method[i]) {
			*method = i;
		}
	}
}

int HttpParser::
getHeaderNumber (string head) {
	int i;
	for (i = 0; i < TOTAL_HEADER; i++) {
		if (head == req_header[i]) {
			return i;
		}
	}

	return -1;
}

void HttpParser::
analysisRequestBody (string body, string *vm) {
	int idx, pos;
	idx = 0;
	pos = body.find ("\r\n", idx);
	do {
		string line = body.substr (idx, pos - idx);
		string head, value;
		int p = line.find (":", 0);
		int hdrnum = 0;

		head = line.substr (0, p);
		value = line.substr (line.find_first_not_of (" ", p + 1));

		hdrnum = getHeaderNumber (head);
		if (hdrnum == -1) {
			cout << "Unknown Header: " << head << endl;
		} else {
			vm[hdrnum] = value;
		}

		idx = pos + 2;
	} while ((pos = body.find ("\r\n", idx)) != string::npos);

	string line = body.substr (idx);
	int p = line.find (":", 0);
	string head = line.substr (0, p);
	string value = line.substr (line.find_first_not_of (" ", p + 1));
	int hdrnum = getHeaderNumber (head);

	if (hdrnum == -1) {
		cout << "Unknown Header:" << head << endl;
	} else {
		vm[hdrnum] = value;
	}
}

HttpDesc HttpParser::
analysisRequest (char *buf) {
	string request = buf;
	string reqhdr, reqmsg;
	string reqhdr_line, reqhdr_body;

	int method;
	string uri, vm[TOTAL_HEADER];

	for (int i = 0; i < TOTAL_HEADER; i++) {
		vm[i] = "";
	}

	int idx = request.find ("\r\n\r\n", 0);

	/* Request 헤더와 메시지의 분리 */
	reqhdr = request.substr (0, idx);
	if ((idx + 4) >= request.length ()) {
		idx = -1;
	} else {
		idx += 4;
	}
	reqmsg = ((idx == -1) ? "" : request.substr (idx));

	/* Request 라인과 바디의 분리 */
	idx = reqhdr.find ("\r\n");
	reqhdr_line = reqhdr.substr (0, idx);
	reqhdr_body = reqhdr.substr (idx + 2);

	/* Request라인 분석 */
	analysisRequestLine (reqhdr_line, &method, uri);
	/* Request바디 분석 */
	analysisRequestBody (reqhdr_body, vm);
	HttpDesc hd (method, uri, vm, reqmsg);

	return hd;
}

HttpMain::
~HttpMain () {
}

HttpProcessor *HttpMain::
getProcessor (UserID ui, HttpDesc hd) {
	HttpList::iterator it;
	for (it = hlists.begin ();
		 it != hlists.end ();
		 ++it) {
		HttpProcessor *hp = *it;
		if (hp->isSame (ui)) {
			hp->setHttpDescriptor (hd);
			return hp;
		}
	}

	HttpProcessor *hp = hpcreator (ui, hd);
	hlists.push_back (hp);
	return hp;
}

void HttpMain::
removeHPFromList (HttpProcessor *hp) {
	hlists.remove (hp);
	delete hp;
}

void HttpMain::
processHttp (char *buf, string ip, SocketInstance *si) {
	HttpDesc hd;

	hd = parser.analysisRequest (buf);
	UserID ui (ip, hd.getUserAgent ());

	HttpProcessor *hp = getProcessor (ui, hd);
	if (!hp->processRequest (si)) {
		removeHPFromList (hp);
	}
}
