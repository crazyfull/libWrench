#include "ThreadTimer.h"
#include "log.h"
#include <cstring>
#include <stdio.h>
#include "clsThread.h"



ThreadTimer::ThreadTimer()
{
    m_pthread = nullptr;
}

ThreadTimer::~ThreadTimer()
{
    clean();
}

void ThreadTimer::clean()
{
    if(m_pthread){
        m_pthread->Kill();
        delete m_pthread;
        m_pthread = nullptr;
    }
}

void ThreadTimer::OnTimerTimerOut()
{
    //
    LOG("OnTimerTimerOut()");
}

void ThreadTimer::__onThreadTimer(clsThread *pTherad, void *pArg)
{
    int interval = getInterval();
    bool isSingleshot = getIsSingleshot();
    for(;;){
        sleep(interval);
        m_pthread->MutexLock();
        OnTimerTimerOut();
        m_pthread->MutexUnlock();
        if(isSingleshot){
            break;
        }
    }
    m_pthread = nullptr;
}

int ThreadTimer::getInterval() const
{
    return interval;
}

bool ThreadTimer::getIsSingleshot() const
{
    return isSingleshot;
}

void ThreadTimer::SetInterval(int sec)
{
    interval = sec;
}

void ThreadTimer::SetSingleShot(bool isEnable)
{
    isSingleshot = isEnable;
}

static void onThreadTimer(clsThread *pTherad, void* pArg){
    ThreadTimer *pThis = static_cast<ThreadTimer*>(pArg);
    pThis->__onThreadTimer(pTherad, pArg);
}

void ThreadTimer::StartTimer(int sec)
{
    if(sec > 0)
        SetInterval(sec);

    if(interval > 0){
        if(!m_pthread){
            m_pthread = clsThread::CreateThread(onThreadTimer, this, 0);
        }
    }
}

void ThreadTimer::StopTimer()
{
    clean();
}

