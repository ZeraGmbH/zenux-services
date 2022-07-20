// globale definition

#ifndef SEC1000DGLOBAL_H
#define SEC1000DGLOBAL_H

enum DebugLevel {NoDebug, DebugLevel1, DebugLevel2, DebugLevel3 = 4, MaxDebugLevel = 7};

#define DEBUG1 (m_nDebugLevel & 1) // alle fehlermeldungen loggen
#define DEBUG2 (m_nDebugLevel & 2) // alle i2c aktivitäten loggen
#define DEBUG3 (m_nDebugLevel & 4) // alle client an-,abmeldungen

#define CheckSumOffset 56

#define LeiterkartenName "ADW5743"
#define ServerName "sec1000d"
#define ServerVersion "V1.00"
#define MaxClients 80
#define defaultDebugLevel NoDebug
#define defaultFPGADeviceNode "/dev/zFPGA1reg"
#define defaultServerPort 6320
#define defaultRMPort 6312
#define defaultXSDFile "/etc/zera/sec1000d/sec1000d.xsd"

enum ServerErrors
{
    noError,
    forkError,
    parameterError,
    pipeError,
    xsdfileError,
    xmlfileError,
    secDeviceError,
    rmConnectionError
};

// wenn SEC1000DDEBUG -> kein fork()
//#define SEC1000DDEBUG 1

#endif
