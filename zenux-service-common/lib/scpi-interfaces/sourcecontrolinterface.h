#ifndef SOURCECONTROLINTERFACE_H
#define SOURCECONTROLINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "scpiserverconnection.h"
#include "sourcecontrolsettings.h"

class SourceControlInterface : public ScpiServerConnection
{
    Q_OBJECT
public:
    SourceControlInterface(std::shared_ptr<cSCPI> scpiInterface,
                           SourceControlSettings* settings,
                           AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection() override;
private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    static QJsonObject expandJsonCapabilities(const QJsonObject &capabilitiesRaw);

    QString m_sourceCapabilityFileName;
    QString m_sourceCapabilities;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

#endif // SOURCECONTROLINTERFACE_H
