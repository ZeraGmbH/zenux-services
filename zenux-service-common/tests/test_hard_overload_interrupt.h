#ifndef TEST_HARD_OVERLOAD_INTERRUPT_H
#define TEST_HARD_OVERLOAD_INTERRUPT_H

#include "mockmt310s2d.h"
#include <abstracttcpnetworkfactory.h>
#include <resmanrunfacade.h>

class test_hard_overload_interrupt : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

private:
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
};

#endif // TEST_HARD_OVERLOAD_INTERRUPT_H
