#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <semaphore.h>

class Semaphore
{
private:
	sem_t mySem;

public:
	Semaphore (void) {
		sem_init (&mySem, 0, 1);
	}
	Semaphore (int val) { sem_init (&mySem, 0, val); }
	~Semaphore (void) { sem_destroy (&mySem); }
	void push () { sem_wait (&mySem); }
	void up () { sem_post (&mySem); }
};

#endif
