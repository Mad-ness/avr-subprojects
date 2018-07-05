#ifndef __REQUESTMANAGER_H__
#define __REQUESTMANAGER_H__

#include <string.h>
#include <apptypes.h>
#include <crypto/sha256.h>
#include <surlparser.h>

#if __ARM_ARCH == 7
    typedef unsigned int request_time_t;
#elif __ARM_ARCH == 8
    typedef unsigned long request_time_t;
#else 										// for everything else take maximum possible (64-bit) size
    typedef unsigned long request_time_t;
#endif

struct RequestItem_t {
    string path;
    UserArgs_t args;
	string uri_hash;				// hashsum of uri
	int num_requests = 0;			// how many the same requests received
	int done_attempts = 0;			// how many attempts have been perfromed to transmit the request
	int failed_attempts = 0;		// how many attempts to send the request to the remote board failed
	bool has_sent = false;			// indicates whether the request sent to the remote board
	struct {
		request_time_t received;	// received from the end user
		request_time_t scheduled;	// transmitted to the remote board
		request_time_t completed;	// received a response from the remote board
	} when;
};


string
makeSHA256(const char *uri);


class RequestManager {
	private:
		SUrlParser m_parser;
		bool validateURI(const char *uri);
	public:
		bool addRequest(const char *uri, string *output);
};


#endif // __REQUESTMANAGER_H__

