#ifndef TEST_ADJ_PERMISSIONS_H
#define TEST_ADJ_PERMISSIONS_H

#include "proxyclient.h"
#include "testserverforsenseinterfacemt310s2.h"
#include <resmanrunfacade.h>
#include <QObject>

class test_adj_permissions : public QObject
{
    Q_OBJECT
private slots:
private:
    void setupServers();
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_ADJ_PERMISSIONS_H
