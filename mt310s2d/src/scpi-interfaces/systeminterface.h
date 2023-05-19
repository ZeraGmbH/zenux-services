#ifndef SYSTEMINTERFACE_H
#define SYSTEMINTERFACE_H

#include "scpiconnection.h"
#include "hotpluggablecontrollercontainer.h"
#include <QList>
#include <QJsonDocument>
#include <memory>

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
    cmdInterfaceRead,
    cmdTestMode
};
}


class cMT310S2dServer;
class adjFlash;

class cSystemInterface: public ScpiConnection
{
    Q_OBJECT

public:
    cSystemInterface(cMT310S2dServer* server, std::unique_ptr<HotPluggableControllerContainer> hotPluggableControllerContainer);
    virtual void initSCPIConnection(QString leadingNodes) override;
    void actualizeContollers(quint16 bitmaskAvailable);

protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;

private:
    cMT310S2dServer* m_pMyServer;
    std::unique_ptr<HotPluggableControllerContainer> m_hotPluggableControllerContainer;
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
    QString testMode(QString &Input);
    QJsonDocument getSoftwareVersion();

    void m_genAnswer(int select, QString& answer);
};


#endif // SYSTEMINTERFACE_H
