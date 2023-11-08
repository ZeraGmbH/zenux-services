// globale definition

#ifndef MT310S2DGLOBAL_H
#define MT310S2DGLOBAL_H

#define CheckSumOffset 56

// 28.08.2018 v1.01 erste lauffähige version mit geändertem netzwerk zugang systemctl zdsp1d.socket


#define LeiterkartenName "ADW5743"
#define ServerName "mt310s2d"
#define ServerVersion "V1.01"
#define MaxClients 30
#define defaultDebugLevel NoDebug
#define defaultCTRLDeviceNode "/dev/zFPGA1ctrl"
#define defaultProtoBufServerPort 6307
#define defaultXSDFile "/etc/zera/mt310s2d/mt310s2d.xsd"

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

enum atmelSysCntrlInterrupts
{
    logMessageInterrupt,
    deviceErrorInterrup,
    extensionPackInterrupt,
    accumulatorInterrupt
};

#endif
