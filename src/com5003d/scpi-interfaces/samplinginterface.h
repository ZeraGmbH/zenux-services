#ifndef SAMPLINGINTERFACE_H
#define SAMPLINGINTERFACE_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "samplerange.h"
#include "resource.h"
#include <notificationstring.h>

namespace SamplingSystem
{

const QString Version = "V1.00";

enum Commands
{
    cmdVersion,
    cmdSampleRate,
    cmdChannelCat,
    cmdChannelAlias,
    cmdChannelType,
    cmdChannelStatus,
    cmdChannelRange,
    cmdChannelRangeCat,
    cmdPLL,
    cmdPLLCat
};

}

class QString;
class cSCPIConnection;
class cSamplingSettings;
class cClientNetBase;
class cCOM5003dServer;


class cSamplingInterface: public cResource
{
    Q_OBJECT

public:
    cSamplingInterface(cCOM5003dServer* server);
    virtual void initSCPIConnection(QString leadingNodes);
    virtual void registerResource(cRMConnection *rmConnection, quint16 port);
    virtual void unregisterResource(cRMConnection *rmConnection);

protected slots:
    virtual void executeCommand(int cmdCode, cProtonetCommand* protoCmd);

private:
    cCOM5003dServer *m_pMyServer;
    quint16 m_nType;
    QString m_sVersion;
    QString m_sAlias;
    QString m_sName; // the samplingsystem's name
    QString m_sDescription; // the samplingsystem's brief description
    bool m_bAvail; // is this sampling system available ?
    QList<cSampleRange*> m_SampleRangeList;
    QStringList m_pllChannelList;

    QString m_ReadVersion(QString& sInput);
    QString m_ReadSampleRate(QString& sInput);
    QString m_ReadSamplingChannelCatalog(QString& sInput);
    QString m_ReadAlias(QString&sInput);
    QString m_ReadType(QString&sInput);
    QString m_ReadStatus(QString& sInput);
    QString m_ReadWriteSamplingRange(QString& sInput);
    QString m_ReadSamplingRangeCatalog(QString& sInput);
    QString m_ReadWritePLL(QString& sInput);
    QString m_ReadPLLCatalog(QString& sInput);

    cNotificationString notifierSampleChannelRange;

    void setNotifierSampleChannelRange();

};

#endif // SAMPLINGINTERFACE_H
