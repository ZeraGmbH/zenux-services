#ifndef TEST_NOTIFICATIONCONTENTS_H
#define TEST_NOTIFICATIONCONTENTS_H

#include "pcbinterface.h"
#include "proxyclient.h"
#include "testserverforsenseinterfacemt310s2.h"
#include <resmanrunfacade.h>
#include <abstracttcpnetworkfactory.h>

class test_notificationcontents : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void rangeChangeWithoutValue();
    void rangeChangeWithValue();
    void rangesChangeWithoutValue();
    void rangesChangeWithValue();
private:
    void setupServers();
    void setupClient();
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TEST_NOTIFICATIONCONTENTS_H
