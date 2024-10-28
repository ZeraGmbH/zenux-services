#ifndef TEST_MOCKSERVICE_SEC1000D_H
#define TEST_MOCKSERVICE_SEC1000D_H

#include "mocksec1000d.h"
#include "resmanrunfacade.h"
#include "proxyfortest.h"
#include "secinterface.h"
#include <QObject>
#include <memory>

class test_mockservice_sec1000d : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void getChannelCatSec1000d();
    void freeSecResourcesWhenClientDisconnects();
private:
    Zera::cSECInterfacePtr getSecInterface();
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockSec1000d> m_sec1000d;
    std::unique_ptr<ProxyForTest> m_proxy;
    VeinTcp::AbstractTcpWorkerFactoryPtr m_tcpWorkerFactory;
};

#endif // TEST_MOCKSERVICE_SEC1000D_H
