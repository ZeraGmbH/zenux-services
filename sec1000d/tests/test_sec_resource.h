#ifndef TEST_SEC_RESOURCE_H
#define TEST_SEC_RESOURCE_H

#include "secgroupresourceandinterface.h"
#include <memory>
#include <QObject>

class test_sec_resource : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void setSecChannelsForAClient();
    void setAndFreeSecChannelsForAClient();
    void setSecChannelsForMultipleClientsFreeOneClient();
    void setSecChannelsForMultipleClientsOnePeerFreePeer();
    void setSecChannelsForMultipleClientsMultiplePeersFreeOnePeer();
    void freeChannelsFromInvalidPeer();
private:
    QString sendScpiCommand(XiQNetPeer *peer, QByteArray clientID, QString cmd);
    std::unique_ptr<Zera::XMLConfig::cReader> m_xmlConfigReader;
    std::unique_ptr<SecCalculatorSettings> m_ecalcSettings;
    std::unique_ptr<SecInputSettings> m_inputSettings;
    std::unique_ptr<SecGroupResourceAndInterface> m_secResource;
};

#endif // TEST_SEC_RESOURCE_H
