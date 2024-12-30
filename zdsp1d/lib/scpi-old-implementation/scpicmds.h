#include <QString>

#ifndef SCPIDSPS_H
#define SCPIDSPS_H

enum SCPICmdType  {
    nixCmd, // 0 is nix

    // die routinen für das system modell
    SetDspCommandStat,
    GetDspCommandStat,
    TriggerIntListHKSK,
    TriggerIntListALL,
    TestDsp,
    scpiGetDeviceVersion,
    scpiGetServerVersion,
    SetSamplingSystem,
    GetSamplingSystem,
    SetCommEncryption,
    GetCommEncryption,

    // die routinen für das status modell
    ResetDeviceLoadMax,
    GetDeviceLoadMax,
    GetDeviceLoadAct,
    GetDspStatus,
    GetDeviceStatus,

    // die routinen für das measure modell
    Fetch,
    Initiate,
    UnloadCmdList,
    LoadCmdList,
    scpiRavListGetSet,
    SetCmdIntList,
    GetCmdIntList,
    SetCmdList,
    GetCmdList,
    Measure,

    // die routinen für das memory modell
    scpiDspMemoryRead,
    scpiDspMemoryWrite,

    // common
    scpiInterfaceRead
};

class cbIFace { // call back interface 
public:    
    virtual ~cbIFace(){}; // rein virtuell
    virtual QString SCPICmd(SCPICmdType cmdEnum, QChar* scpi) = 0;
    virtual QString SCPIQuery(SCPICmdType cmdEnum) = 0;
};

#endif
