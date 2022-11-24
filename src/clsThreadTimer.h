#ifndef CLSTHREADTIMER_H
#define CLSTHREADTIMER_H

class clsThread;
class clsThreadTimer
{
    int interval = 0;
    bool isSingleshot = false;
     clsThread * m_pthread;

     void clean();
public:
    clsThreadTimer();
    ~clsThreadTimer();
    virtual void OnTimerTimerOut();
     void __onThreadTimer(clsThread *pTherad, void* pArg);

    void SetInterval(int sec);
    void SetSingleShot(bool isEnable);
    void StartTimer(int sec = 0);
    void StopTimer();
    int getInterval() const;
    bool getIsSingleshot() const;
};

#endif // CLSTHREADTIMER_H
