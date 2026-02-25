#include "clsAppController.h"

AppController::AppController()
{
    efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd == -1)
        throw std::runtime_error("eventfd failed");

    instance = this;
    installSignals();
}

AppController::~AppController()
{
    instance = nullptr;
    if (efd != -1)
        close(efd);
}

void AppController::wait()
{
    uint64_t val;
    while (!isShutdownRequested())
    {
        if (read(efd, &val, sizeof(val)) > 0)
            break;

        // block
        pause();
    }
}

int AppController::eventFd() const noexcept
{
    return efd;
}

void AppController::signalHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        shutdownRequested.store(true, std::memory_order_release);

        if (instance && instance->efd != -1)
        {
            uint64_t one = 1;
            write(instance->efd, &one, sizeof(one)); // async-signal-safe
        }
    }
}

void AppController::installSignals()
{
    struct sigaction sa {};
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // SA_RESTART نذار

    sigaction(SIGINT,  &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
}

bool AppController::isShutdownRequested() const noexcept
{
    return shutdownRequested.load(std::memory_order_acquire);
}
