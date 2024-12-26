#ifndef TEST_REGRESSION_SCPI_ZDSP1D_H
#define TEST_REGRESSION_SCPI_ZDSP1D_H

#include "mockzdsp1d.h"
#include "dspinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_scpi_zdsp1d : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void serverUp();
    void dumpScpi();
private:
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockZdsp1d> m_server;
    Zera::ProxyClientPtr m_proxyClient;
    std::shared_ptr<Zera::cDSPInterface> m_dspIFace;
};

#endif // TEST_REGRESSION_SCPI_ZDSP1D_H
