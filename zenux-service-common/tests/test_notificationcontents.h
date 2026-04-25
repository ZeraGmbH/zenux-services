#ifndef TEST_NOTIFICATIONCONTENTS_H
#define TEST_NOTIFICATIONCONTENTS_H

#include "pcbinterface.h"
#include "proxyclient.h"
#include "testfactoryi2cctrl.h"
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

    void rangeChangeWithoutValueDelayedRangeIo();
    void rangeChangeWithValueDelayedRangeIo();
private:
    void setupServers();
    void setupClient();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::shared_ptr<TestFactoryI2cCtrl> m_ctrlFactory;
    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;

    Zera::ProxyClientPtr m_pcbClient;
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // TEST_NOTIFICATIONCONTENTS_H
