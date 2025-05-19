#ifndef SOURCECONTROLINTERFACE_H
#define SOURCECONTROLINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "scpiconnection.h"
#include "sourcecontrolsettings.h"

class SourceControlInterface : public ScpiConnection
{
    Q_OBJECT
public:
    SourceControlInterface(std::shared_ptr<cSCPI> scpiInterface,
                           SourceControlSettings* settings,
                           AbstractFactoryI2cCtrlPtr ctrlFactory);
    void initSCPIConnection(QString leadingNodes) override;
private:
    void executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd) override;
    QString m_sourceCapabilityFile;
    QString m_sourceCapabilities;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;
};

#endif // SOURCECONTROLINTERFACE_H
