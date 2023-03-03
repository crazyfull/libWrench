#ifndef CLSLINUXSERVICE_H
#define CLSLINUXSERVICE_H

#define LINUX_SERVICE_PLACE ("/etc/systemd/system/")    // /usr/lib/systemd/system
class LinuxService
{
public:
    LinuxService();
    static bool ishaveRootAccess();
    static bool CreateService(const char* ServiceName, const char* Description);
    static bool RemoveService(const char* ServiceName);
};

#endif // CLSLINUXSERVICE_H
