#include <QString>

#ifndef SCPIDSPS_H
#define SCPIDSPS_H

enum SCPICmdType  {
    nixCmd, // 0 is nix

    // die routinen für das system modell
    scpiDspCommandStatGetSet,
    TriggerIntListHKSK,
    scpiTriggerIntListALL,
    scpiGetDeviceVersion,
    scpiGetServerVersion,
    scpiSamplingSystemGetSet,

    // die routinen für das status modell
    scpiGetDeviceLoadAct,
    scpiGetDeviceLoadMax,
    scpiResetDeviceLoadMax,
    scpiGetDspStatus,
    scpiGetDeviceStatus,

    // die routinen für das measure modell
    scpiUnloadCmdList,
    scpiLoadCmdList,
    scpiRavListGetSet,
    scpiCmdIntListGetSet,
    scpiCmdCycListGetSet,
    scpiReadActualValues, // AKA data acquisition

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
