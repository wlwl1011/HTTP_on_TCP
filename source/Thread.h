#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <signal.h>

class Thread
{
private:
	pthread_t myThread;
	void *retval;

public:
	Thread () {
		retval = NULL;
		myThread = 0;
	}
	
	~Thread () {
		if (myThread != 0) {
			pthread_kill (myThread, SIGINT);
		}
	}

	virtual void *run () { return NULL; }

	static void *_run (void *ptr) {
		Thread *pthis = (Thread *)ptr;
		return pthis->run ();
	}

	void start () {
		pthread_create (&myThread, NULL, Thread::_run, this);
	}

	void kill () {
		pthread_kill (myThread, SIGINT);
		myThread = 0;
	}

	void *join () {
		pthread_join (myThread, &retval);
		return retval;
	}
};

#endif
