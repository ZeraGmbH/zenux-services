#ifndef SAMPLINGINTERFACE_H
#define SAMPLINGINTERFACE_H

#include "resource.h"
#include "samplingsettings.h"
#include "abstractfactoryi2cctrl.h"
#include <QStringList>

namespace SamplingSystem
{
    enum Commands
    {
        cmdSampleRate,
        cmdPLL,
        cmdPLLCat
    };
}

class cSamplingInterface: public cResource
{
    Q_OBJECT
public:
    cSamplingInterface(std::shared_ptr<cSCPI> scpiInterface,
                       SamplingSettings *samplingSettings,
                       AbstractFactoryI2cCtrlPtr ctrlFactory);
    virtual void initSCPIConnection(QString leadingNodes) override;
    virtual void registerResource(RMConnection *rmConnection, quint16 port) override;
protected:
    void executeProtoScpi(int cmdCode, cProtonetCommand* protoCmd) override;
private:
    QString m_ReadSampleRate(QString& sInput);
    QString m_ReadWritePLL(QString& sInput);
    QString m_ReadPLLCatalog(QString& sInput);

    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
    QString m_sDescription; // the samplingsystem's brief description
    bool m_bAvail; // is this sampling system available ?
    QStringList m_pllChannelList;
};

#endif // SAMPLINGINTERFACE_H
