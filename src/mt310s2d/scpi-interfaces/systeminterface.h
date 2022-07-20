#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include <QObject>
#include <QList>
#include <scpi.h>

#include <scpiconnection.h>

namespace SystemSystem
{

enum SystemCommands
{
    cmdVersionServer,
    cmdVersionDevice,
    cmdVersionPCB,
    cmdVersionCTRL,
    cmdVersionFPGA,
    cmdSerialNumber,
    cmdUpdateControlerBootloader,
    cmdUpdateControlerProgram,
    cmdUpdateControlerFlash,
    cmdUpdateControlerEEprom,
    cmdAdjFlashWrite,
    cmdAdjFlashRead,
    cmdAdjXMLImportExport,
    cmdAdjXMLWrite,
    cmdAdjXMLRead,
    cmdAdjFlashChksum,
    cmdInterfaceRead
};
}


class cMT310S2dServer;
class adjFlash;

class cSystemInterface: public cSCPIConnection
{
    Q_OBJECT

public:
    cSystemInterface(cMT310S2dServer* server);
    virtual void initSCPIConnection(QString leadingNodes);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cMT310S2dServer* m_pMyServer;
    QString m_ReadServerVersion(QString& sInput);
    QString m_ReadDeviceVersion(QString& sInput);
    QString m_ReadDeviceName(QString& sInput);
    QString m_ReadWritePCBVersion(QString& sInput);
    QString m_ReadCTRLVersion(QString& sInput);
    QString m_ReadFPGAVersion(QString& sInput);
    QString m_ReadWriteSerialNumber(QString& sInput);
    QString m_StartControlerBootloader(QString& sInput);
    QString m_StartControlerProgram(QString& sInput);
    QString m_LoadFlash(QString& sInput);
    QString m_LoadEEProm(QString& sInput);
    QString m_AdjFlashWrite(QString& sInput);
    QString m_AdjFlashRead(QString& sInput);
    QString m_AdjXmlImportExport(QString& sInput);
    QString m_AdjXMLWrite(QString& sInput);
    QString m_AdjXMLRead(QString& sInput);
    QString m_AdjFlashChksum(QString& sInput);
    QString m_InterfaceRead(QString& sInput);

    void m_genAnswer(int select, QString& answer);
};


#endif // SYSTEMINTERFACE_H
