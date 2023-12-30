#ifndef TEST_REGRESSION_ADJUSTMENT_MT310S2_H
#define TEST_REGRESSION_ADJUSTMENT_MT310S2_H

#include "mockforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adjustment_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

private:
    std::unique_ptr<MockForSenseInterfaceMt310s2> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJUSTMENT_MT310S2_H
