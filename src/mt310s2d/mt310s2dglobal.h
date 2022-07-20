// globale definition

#ifndef MT310S2DGLOBAL_H
#define MT310S2DGLOBAL_H

enum DebugLevel {NoDebug, DebugLevel1, DebugLevel2, DebugLevel3 = 4, MaxDebugLevel = 7};

#define DEBUG1 (m_nDebugLevel & 1) // alle fehlermeldungen loggen
#define DEBUG2 (m_nDebugLevel & 2) // alle i2c aktivitäten loggen
#define DEBUG3 (m_nDebugLevel & 4) // alle client an-,abmeldungen

#define CheckSumOffset 56

// 28.08.2018 v1.01 erste lauffähige version mit geändertem netzwerk zugang systemctl zdsp1d.socket


#define LeiterkartenName "ADW5743"
#define ServerName "mt310s2d"
#define ServerVersion "V1.01"
#define MaxClients 30
#define defaultDebugLevel NoDebug
#define defaultFPGADeviceNode "/dev/zFPGA1reg"
#define defaultCTRLDeviceNode "/dev/zFPGA1ctrl"
#define defaultI2CDeviceNode "/dev/i2c-3"
#define defaultProtoBufServerPort 6307
#define defaultSCPIServerPort 6308
#define defaultRMPort 6312
#define defaultI2CSysAtmelAdress 0x22
#define defaultI2CAtmelAdress 0x21
#define defaultI2CFlashMuxAdress 0x71
#define defaultI2CFlashAdress 0x50
#define defaultI2CClampFlashAdr 0x51
#define defaultXSDFile "/etc/zera/mt310s2d/mt310s2d.xsd"

enum ServerErrors
{
    noError,
    forkError,
    parameterError,
    pipeError,
    xsdfileError,
    xmlfileError,
    atmelError,
    ctrlDeviceError,
    fpgaDeviceError,
    rmConnectionError,
    atmelProgError
};


enum atmelInterrupts
{
    overloadInterrupt_I1,
    overloadInterrupt_I2,
    overloadInterrupt_I3,
    overloadInterrupt_I4,
    overloadInterrupt_U1,
    overloadInterrupt_U2,
    overloadInterrupt_U3,
    overloadInterrupt_U4,
    clampstatusInterrupt
};

// wenn MT310S2DDEBUG -> kein fork()
// #define MT310S2DDEBUG 1

#endif
