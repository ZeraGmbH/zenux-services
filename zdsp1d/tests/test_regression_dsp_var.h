#ifndef TEST_REGRESSION_DSP_VAR_H
#define TEST_REGRESSION_DSP_VAR_H

#include "mockzdsp1d.h"
#include "dspinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_dsp_var : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    void createResultVariables();
    void createTempVariables();
    void createInternalVariables();
    void createTempGlobalVariables();
    void globalVariablesAreNotSharedByDefault();

    void readVariablesAndListenDeviceNode(); // we learned dspMemoryRead details by this
    void writeVariablesAndListenDeviceNode();

    void multipleClientsCreateResultVars();
private:
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    std::unique_ptr<MockZdsp1d> m_dspService;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cDSPInterface> m_dspIFace;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
};

#endif // TEST_REGRESSION_DSP_VAR_H
