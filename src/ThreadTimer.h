#ifndef THREADTIMER_H
#define THREADTIMER_H

class clsThread;
class ThreadTimer
{
    int interval = 0;
    bool isSingleshot = false;
     clsThread * m_pthread;

     void clean();
public:
    ThreadTimer();
    ~ThreadTimer();
    virtual void OnTimerTimerOut();
     void __onThreadTimer(clsThread *pTherad, void* pArg);

    void SetInterval(int sec);
    void SetSingleShot(bool isEnable);
    void StartTimer(int sec = 0);
    void StopTimer();
    int getInterval() const;
    bool getIsSingleshot() const;
};

#endif // THREADTIMER_H
