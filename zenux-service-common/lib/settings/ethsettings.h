#ifndef ETHSETTINGS_H
#define ETHSETTINGS_H

#include <xmlsettings.h>
#include <memory>

class EthSettings : public XMLSettings
{
    Q_OBJECT
public:
    enum ethmember
    {
        protobufserver,
        scpiserver,
        resourcemanager
    };
    EthSettings(Zera::XMLConfig::cReader *xmlread);
    QString getRMIPadr();
    quint16 getPort(ethmember member);
    bool isSCPIactive();
public slots:
    void configXMLInfo(const QString &key) override;
private:
    QString m_sRMIPAdr;
    quint16 m_nProtobufServerPort, m_nSCPIServerPort, m_nRMPort;
    bool m_bSCPIactive;
};

typedef std::shared_ptr<EthSettings> EthSettingsPtr;

#endif // ETHSETTINGS_H
