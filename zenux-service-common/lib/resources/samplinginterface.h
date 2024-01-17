#ifndef SAMPLINGINTERFACE_H
#define SAMPLINGINTERFACE_H

#include "samplerange.h"
#include "resource.h"
#include "samplingsettings.h"
#include "abstractfactoryi2cctrl.h"
#include <QStringList>

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

class cSamplingInterface: public cResource
{
    Q_OBJECT
public:
    cSamplingInterface(cSCPI *scpiInterface, SamplingSettings *samplingSettings, FactoryControllerAbstractPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    void setNotifierSampleChannelRange();
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

    FactoryControllerAbstractPtr m_ctrlFactory;
    quint16 m_nType;
    QString m_sVersion;
    QString m_sAlias;
    QString m_sName; // the samplingsystem's name
    QString m_sDescription; // the samplingsystem's brief description
    bool m_bAvail; // is this sampling system available ?
    QList<cSampleRange*> m_SampleRangeList;
    QStringList m_pllChannelList;
    NotificationString notifierSampleChannelRange;
};

#endif // SAMPLINGINTERFACE_H
