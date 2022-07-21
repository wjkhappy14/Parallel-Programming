#include "SimpleEvent.h"

#ifdef _MSC_VER

SimpleEvent::SimpleEvent()
{
	cond = CreateEvent(NULL, true, false, NULL);
}

SimpleEvent::~SimpleEvent()
{
	CloseHandle(cond);
}

void SimpleEvent::SignalEvent()
{
	PulseEvent(cond);
}

void SimpleEvent::WaitEvent()
{
	WaitForSingleObject(cond, INFINITE);
}


#endif //_MSC_VER


#ifdef __GNUC__	
#include <errno.h>

SimpleEvent::SimpleEvent()
{
        bIsSignal = false;
	pthread_cond_init( &cond, NULL);
	pthread_mutex_init( &mutex, NULL);
}

SimpleEvent::~SimpleEvent()
{
	pthread_cond_destroy( &cond);
	pthread_mutex_destroy( &mutex);
}


void SimpleEvent::SignalEvent()
{
    pthread_mutex_lock(&mutex);
	bIsSignal =true;
	pthread_cond_signal( &cond);
    pthread_mutex_unlock(&mutex);
}

void SimpleEvent::WaitEvent()
{
    pthread_mutex_lock(&mutex);
    bIsSignal = false;
    while(!bIsSignal){
		pthread_cond_wait( &cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

}


#endif // __GNUC__

