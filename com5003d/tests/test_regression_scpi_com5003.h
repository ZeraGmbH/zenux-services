#ifndef TEST_REGRESSION_SCPI_COM5003_H
#define TEST_REGRESSION_SCPI_COM5003_H

#include "mockcom5003d.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_scpi_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void serverUp();
    void dumpScpi();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockCom5003d> m_server;
    Zera::ProxyClientPtr m_proxyClient;
    Zera::PcbInterfacePtr m_pcbIFace;
};

#endif // TEST_REGRESSION_SCPI_COM5003_H
