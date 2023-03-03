#include "clsLinuxService.h"
#include <cstring>
#include <Wrench/clsCString.h>
#include <Wrench/clsFileDirectory.h>
#include <Wrench/log.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>


LinuxService::LinuxService()
{

}

bool LinuxService::ishaveRootAccess()
{
    if (getuid()) {
        return false;
    }else{
        return true;
    }
}

bool LinuxService::CreateService(const char *ServiceName, const char *Description)
{
    if(LinuxService::ishaveRootAccess() == false){
        //
        DebugPrint("need root access!");
        return false;
    }

    CString Buff = "[Unit]\n"
                   "Description=";
    Buff + Description;
    Buff + "\n"
           "\n";    //After=network.target

    //Type=forking

    Buff + "[Service]\n"
           "User=root\n"
           "Restart=always\n"
           "Type=simple\n"
           "ExecStart=";
    Buff + FileDirectory::GetFullProgramPath();
    Buff + "\n\n"

           "[Install]\n"
           "WantedBy=multi-user.target\n";

    CString ServicePath = LINUX_SERVICE_PLACE;
    ServicePath.append(ServiceName);
    ServicePath.append(".service");

    //save service file
    if(FileDirectory::SaveFile(Buff.Data() ,ServicePath.Data()) == false){
        DebugPrint("can not create/write service file!");
        return false;
    }

    CString EnableCMD = "systemctl enable ";
    EnableCMD + ServiceName;

    CString StartCMD = "systemctl start ";
    StartCMD + ServiceName;

    system("systemctl daemon-reload");
    system(EnableCMD.Data());
    system(StartCMD.Data());
    return true;
}

bool LinuxService::RemoveService(const char *ServiceName)
{
    if(LinuxService::ishaveRootAccess() == false){
        //
        DebugPrint("need root acess!");
        return false;
    }

    CString ServicePath = LINUX_SERVICE_PLACE;
    ServicePath.append(ServiceName);
    ServicePath.append(".service");

    CString DisableCMD = "systemctl disable ";
    DisableCMD + ServiceName;

    CString StopCMD = "systemctl stop ";
    StopCMD + ServiceName;

    system(StopCMD.Data());
    system(DisableCMD.Data());
    system("systemctl daemon-reload");

    if( remove(ServicePath.Data()) != 0)
    {
        DebugPrint("can not remove service file!");
        return false;
    }

    return true;
}
