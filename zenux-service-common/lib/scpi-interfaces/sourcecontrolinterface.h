#ifndef SOURCECONTROLINTERFACE_H
#define SOURCECONTROLINTERFACE_H

#include "abstractfactoryi2cctrl.h"
#include "jsondevicestatusapi.h"
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
    QJsonObject expandJsonCapabilities(const QJsonObject &capabilitiesRaw);
    QString m_sourceCapabilityFileName;
    QString m_sourceCapabilities;
    AbstractFactoryI2cCtrlPtr m_ctrlFactory;

    JsonDeviceStatusApi m_jsonSourceStateApi;
    NotificationString m_sourceState;
    NotificationString m_sourceLoadState;
};

#endif // SOURCECONTROLINTERFACE_H
