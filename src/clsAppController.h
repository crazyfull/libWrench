#ifndef CLSAPPCONTROLLER_H
#define CLSAPPCONTROLLER_H

#include <atomic>
#include <csignal>
#include <unistd.h>
#include <sys/eventfd.h>
#include <stdexcept>

class AppController
{
public:
    AppController();
    ~AppController();
    bool isShutdownRequested() const noexcept;
    void wait();
    int eventFd() const noexcept;

private:
    static inline AppController* instance {nullptr};
    static inline std::atomic<bool> shutdownRequested {false};
    int efd {-1};
    static void signalHandler(int sig);
    void installSignals();
};
#endif // CLSAPPCONTROLLER_H
