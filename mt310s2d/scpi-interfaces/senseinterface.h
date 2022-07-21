#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include "mt310s2d.h"
#include "adjflash.h"
#include "adjxml.h"
#include "resource.h"
#include <scpiconnection.h>
#include "sensechannel.h"
#include "systeminfo.h"
#include <QList>
#include <QHash>

namespace SenseSystem
{
    const QString Version = "V1.00";
    enum Commands
    {
        cmdVersion,
        cmdMMode,
        cmdMModeCat,
        cmdChannelCat,
        cmdGroupCat,
        initAdjData,
        computeAdjData,
        cmdStatAdjustment
    };
    enum MMode
    {
        modeAC = 1,
        modeHF = 2,
        modeDC = 4,
        modeADJ = 8
    };
    enum SensorType
    {
        Direct = 0x100,
        Clamp = 0x200
    };
    const QString sVoltageChannelDescription = "Measuring channel 0..250V 50Hz/150kHz";
    const QString sCurrentChannelDescription = "Measuring channel 0..1000A 50Hz/150kHz";
    const QString sMeasuringModeDescription = "Measuring mode switch AC,HF,ADJ";
}

class cSenseInterface : public cResource, public cAdjFlash, public cAdjXML
{
    Q_OBJECT
public:
    cSenseInterface(cMT310S2dServer *server);
    ~cSenseInterface();
    virtual void initSCPIConnection(QString leadingNoMModedes);
    cSenseChannel* getChannel(QString& name);
    QString getChannelSystemName(quint16 ctrlChannel);
    cSenseRange* getRange(QString channelName, QString rangeName);
    virtual quint8 getAdjustmentStatus(); // we return 0 if adj. otherwise  1 +2 +4
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);
    virtual QString exportXMLString(int indent = 1);
    void m_ComputeSenseAdjData();
protected:
    virtual void exportAdjData(QDataStream& stream);
    virtual bool importAdjData(QDataStream& stream);
    virtual bool importXMLDocument(QDomDocument* qdomdoc);
protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);
private:
    cMT310S2dServer* m_pMyServer;
    cSystemInfo* m_pSystemInfo;

    QList<cSenseChannel*> m_ChannelList;
    QString m_sVersion;
    QString m_sMMode;
    QHash<QString,quint8> m_MModeHash;

    quint8 m_nVersionStatus;
    quint8 m_nSerialStatus;

    QString m_ReadVersion(QString& sInput);
    void m_ReadWriteMMode(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);
    QString handleScpiReadAdjStatus(QString& sInput);

    cNotificationString notifierSenseMMode;
    cNotificationString notifierSenseChannelCat;

    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();
    bool setSenseMode(QString sMode);
};

#endif // SENSEINTERFACE_H
