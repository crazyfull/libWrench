#ifndef CLSTHREAD_H
#define CLSTHREAD_H
#include "SocketHeader.h"

/*
 * for callback
 * static void onThread(clsThread *pTherad, void* pArg);
 */

class clsThread
{
    typedef void (*CallbackRoutine)(clsThread* pThread, void*pArgument);

private:
    pthread_attr_t m_ThreadAttr;
    pthread_t m_Thread;
    CallbackRoutine m_callback;
    void *m_pArgument;
    pthread_mutex_t m_Mutex;

    static void *_onThread(void* p);

public:
    clsThread();
    ~clsThread();

    //
    int _CreateThread(CallbackRoutine Routine, void *Argument, int stackSize);
    static clsThread* CreateThread(CallbackRoutine routine, void *Argument, int stackSize);
    void _MutexLock();
    void _MutexUnlock();
};



#endif // CLSTHREAD_H
