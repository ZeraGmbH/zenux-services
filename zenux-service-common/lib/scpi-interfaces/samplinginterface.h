#ifndef SAMPLINGINTERFACE_H
#define SAMPLINGINTERFACE_H

#include "scpiserverconnection.h"
#include "samplingsettings.h"
#include "abstractfactoryi2cctrl.h"
#include <QStringList>

class cSamplingInterface: public ScpiServerConnection
{
    Q_OBJECT
public:
    cSamplingInterface(const std::shared_ptr<cSCPI> &scpiInterface,
                       const SamplingSettingsPtr &samplingSettings,
                       const AbstractFactoryI2cCtrlPtr &ctrlFactory);
    void initSCPIConnection() override;
protected:
    void executeProtoScpi(int cmdCode, const ProtonetCommandPtr &protoCmd) override;
private:
    static QString scpiReadSampleRate(const QString& scpi);
    QString scpiReadWritePLL(const QString& scpi);
    QString scpiReadPLLCatalog(const QString& scpi);

    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QString m_sDescription; // the samplingsystem's brief description
    bool m_bAvail; // is this sampling system available ?
    QStringList m_pllChannelList;
};

#endif // SAMPLINGINTERFACE_H
