#ifndef TEST_REGRESSION_SCPI_MT310S2_H
#define TEST_REGRESSION_SCPI_MT310S2_H

#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_scpi_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void serverUp();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_server;
    Zera::ProxyClientPtr m_proxyClient;
    Zera::PcbInterfacePtr m_pcbIFace;
};

#endif // TEST_REGRESSION_SCPI_MT310S2_H
