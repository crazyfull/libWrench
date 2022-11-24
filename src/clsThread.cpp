#include "clsThread.h"
#include "log.h"

//typedef void (*CallbackRoutine)(clsThread* pThread, void*pArgument);


clsThread::clsThread()
{
    /* */
    if (pthread_mutex_init(&m_Mutex, nullptr) != 0)
    {
        DebugPrint("mutex init failed");
    }
}

clsThread::~clsThread()
{
    pthread_attr_destroy(&m_ThreadAttr);
    pthread_detach(m_Thread);
    pthread_mutex_destroy(&m_Mutex);
}

void clsThread::MutexLock()
{
    pthread_mutex_lock(&m_Mutex);
}

void clsThread::MutexUnlock()
{
    pthread_mutex_unlock(&m_Mutex);
}

int clsThread::_CreateThread(clsThread::CallbackRoutine Routine, void *Argument, int stackSize)
{
    //CreateThread Linux
    pthread_attr_init(&m_ThreadAttr);
    if(stackSize > 0)
        pthread_attr_setstacksize(&m_ThreadAttr, stackSize);
    pthread_attr_setdetachstate(&m_ThreadAttr, PTHREAD_CREATE_JOINABLE);

    m_pArgument = Argument;
    m_callback = Routine;
    int intErr = pthread_create(&m_Thread, &m_ThreadAttr, _onThread, static_cast<void*>(this));
    return intErr;
}

clsThread *clsThread::CreateThread(CallbackRoutine routine, void *Argument, int stackSize)
{
    clsThread *newThread = new clsThread;
    int ret = newThread->_CreateThread(routine, Argument, stackSize);
    if(ret != 0){
        DebugPrint("cannot create thread!, err:(%d)", ret);
        delete newThread;
        return NULL;
    }

    return newThread;
}

void clsThread::Kill()
{
    //pthread_exit(0);
    pthread_cancel(m_Thread);
}

void *clsThread::_onThread(void *p)
{
    clsThread *pThis = (clsThread *)p;
    pThis->m_callback(pThis, pThis->m_pArgument);

    delete pThis;
    return 0;
}
