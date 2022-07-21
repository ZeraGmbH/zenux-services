#ifndef ETHSETTINGS_H
#define ETHSETTINGS_H

#include <xmlsettings.h>
#include <xmlconfigreader.h>

enum ethmember
{
    protobufserver,
    scpiserver,
    resourcemanager
};

enum ethconfigstate
{
    setRMIPAdress,
    setProtobufServerPort,
    setSCPIServerPort,
    setRMPort,
    setSCPIactive
};

class cETHSettings : public XMLSettings
{
    Q_OBJECT

public:
    cETHSettings(Zera::XMLConfig::cReader *xmlread);
    QString getRMIPadr();
    quint16 getPort(ethmember member);
    bool isSCPIactive();
public slots:
    virtual void configXMLInfo(QString key);
private:
    QString m_sRMIPAdr;
    quint16 m_nProtobufServerPort, m_nSCPIServerPort, m_nRMPort;
    bool m_bSCPIactive;
};

#endif // ETHSETTINGS_H
