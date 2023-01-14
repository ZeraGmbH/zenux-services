#ifndef ETHSETTINGSSEC_H
#define ETHSETTINGSSEC_H

#include <xmlsettings.h>

class EthSettingsSec : public XMLSettings
{
    Q_OBJECT
public:
    enum ethmember
    {
        protobufserver,
        resourcemanager
    };

    enum ethconfigstate
    {
        setRmIpAdress,
        setProtobufServerPort,
        setRmPort
    };
    EthSettingsSec(Zera::XMLConfig::cReader *xmlread);
    QString getRMIPadr();
    quint16 getPort(ethmember member);
public slots:
    virtual void configXMLInfo(QString key);
private:
    QString m_sRMIPAdr;
    quint16 m_nServerPort, m_nRMPort;
};

#endif // ETHSETTINGSSEC_H
