#ifndef SENSEINTERFACE_H
#define SENSEINTERFACE_H

#include <QObject>
#include <QList>
#include <QStateMachine>
#include <QState>
#include <QFinalState>

#include "adjflash.h"
#include "adjxml.h"
#include "resource.h"
#include <scpiconnection.h>
#include "sensechannel.h"
#include <notificationstring.h>

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
    computeAdjData
};

enum MMode
{
    modeAC,
    modeREF,
    modeAnz
};

const QString sVoltageChannelDescription = "Measuring channel 0..480V AC";
const QString sCurrentChannelDescription = "Measuring channel 0..160A AC";
const QString sReferenceChannelDescription = "Reference channel 0..10V DC";
const QString sMeasuringModeDescription = "Measuring mode switch AC,REF";
const QString sMMode[2] = {"AC", "REF"};
}

class cCOM5003dServer;
class cSenseSettings;
class QDataStream;


class cSenseInterface : public cResource, public cAdjFlash, public cAdjXML
{
    Q_OBJECT

public:
    cSenseInterface(cCOM5003dServer* server);
    ~cSenseInterface();
    virtual void initSCPIConnection(QString leadingNodes);
    cSenseChannel* getChannel(QString& name);
    quint8 getAdjustmentStatus(); // we return 0 if not adj. else 1
    virtual void exportAdjData(QDataStream& stream);
    virtual bool importAdjData(QString& s, QDataStream& stream);
    virtual void exportAdjData(QDomDocument& doc, QDomElement& qde); // the derived class exports adjdata beneath qdomelement
    virtual bool importAdjData(QDomNode& node);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer* m_pMyServer;
    QList<cSenseChannel*> m_ChannelList;
    QString m_sVersion;
    quint8 m_nMMode;

    QStateMachine m_ChangeSenseModeMachine;
    QState m_UnregisterSenseState;
    QState m_RegisterSenseState;
    QFinalState m_NotifySenseState;
    QList<cProtonetCommand*> sensemodeProtonetCmdList;

    QString m_ReadVersion(QString& sInput);
    void m_ReadWriteMModeVersion(cProtonetCommand* protoCmd);
    QString m_ReadMModeCatalog(QString& sInput);
    QString m_ReadSenseChannelCatalog(QString& sInput);
    QString m_ReadSenseGroupCatalog(QString& sInput);
    QString m_InitSenseAdjData(QString& sInput);
    QString m_ComputeSenseAdjData(QString& sInput);

    cNotificationString notifierSenseMMode;
    cNotificationString notifierSenseChannelCat;

    void setNotifierSenseMMode();
    void setNotifierSenseChannelCat();

private slots:
    void unregisterSense();
    void registerSense();
    void notifySense();


};

#endif // SENSEINTERFACE_H
