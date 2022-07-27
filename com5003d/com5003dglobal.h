// globale definition

#ifndef COM5003DGLOBAL_H
#define COM5003DGLOBAL_H

enum DebugLevel {NoDebug, DebugLevel1, DebugLevel2, DebugLevel3 = 4, MaxDebugLevel = 7};

#define DEBUG1 (m_nDebugLevel & 1) // alle fehlermeldungen loggen
#define DEBUG2 (m_nDebugLevel & 2) // alle i2c aktivitäten loggen
#define DEBUG3 (m_nDebugLevel & 4) // alle client an-,abmeldungen

#define CheckSumOffset 56

#define LeiterkartenName "ADW5743"
#define ServerName "com5003d"
#define ServerVersion "V1.00"
#define MaxClients 30
#define defaultDebugLevel NoDebug
#define defaultFPGADeviceNode "/dev/zFPGA1reg"
#define defaultI2CDeviceNode "/dev/i2c-3"
#define defaultProtoBufServerPort 6307
#define defaultSCPIServerPort 6308
#define defaultRMPort 6312
#define defaultI2CMasterAdress 0x20
#define defaultI2CAtmelAdress 0x21
#define defaultI2CFlashAdress 0x50
#define defaultXSDFile "/etc/zera/com5003d/com5003d.xsd"
#define atmelFlashfilePath "/opt/zera/bin/atmel-com5003.hex"
#define atmelResetBit 16
#define defaultTMaxAtmel 10000

enum ServerErrors
{
    noError,
    forkError,
    parameterError,
    xsdfileError,
    xmlfileError,
    atmelError,
    rmConnectionError,
    atmelProgError
};

// wenn COM5003DDEBUG -> kein fork()
// #define COM5003DDEBUG 1

#endif
