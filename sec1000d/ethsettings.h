#ifndef ETHSETTINGS_H
#define ETHSETTINGS_H

#include <xmlsettings.h>

enum ethmember
{
    server,
    resourcemanager
};

enum ethconfigstate
{
    setRMIPAdress,
    setServerPort,
    setRMPort
};

class cETHSettings : public XMLSettings
{
    Q_OBJECT

public:
    cETHSettings(Zera::XMLConfig::cReader *xmlread);
    QString getRMIPadr();
    quint16 getPort(ethmember member);

public slots:
    virtual void configXMLInfo(QString key);

private:
    QString m_sRMIPAdr;
    quint16 m_nServerPort, m_nRMPort;
};

#endif // ETHSETTINGS_H
