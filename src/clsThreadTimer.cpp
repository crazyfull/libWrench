#include "clsThreadTimer.h"
#include "log.h"
#include <cstring>
#include <stdio.h>
#include "clsThread.h"



clsThreadTimer::clsThreadTimer()
{
    m_pthread = nullptr;
}

clsThreadTimer::~clsThreadTimer()
{
    clean();
}

void clsThreadTimer::clean()
{
    if(m_pthread){
        m_pthread->Kill();
        delete m_pthread;
        m_pthread = nullptr;
    }
}

void clsThreadTimer::OnTimerTimerOut()
{
    //
    LOG("OnTimerTimerOut()");
}

void clsThreadTimer::__onThreadTimer(clsThread *pTherad, void *pArg)
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

int clsThreadTimer::getInterval() const
{
    return interval;
}

bool clsThreadTimer::getIsSingleshot() const
{
    return isSingleshot;
}

void clsThreadTimer::SetInterval(int sec)
{
    interval = sec;
}

void clsThreadTimer::SetSingleShot(bool isEnable)
{
    isSingleshot = isEnable;
}

static void onThreadTimer(clsThread *pTherad, void* pArg){
    clsThreadTimer *pThis = static_cast<clsThreadTimer*>(pArg);
    pThis->__onThreadTimer(pTherad, pArg);
}

void clsThreadTimer::StartTimer(int sec)
{
    if(sec > 0)
        SetInterval(sec);

    if(interval > 0){
        if(!m_pthread){
            m_pthread = clsThread::CreateThread(onThreadTimer, this, 0);
        }
    }
}

void clsThreadTimer::StopTimer()
{
    clean();
}

