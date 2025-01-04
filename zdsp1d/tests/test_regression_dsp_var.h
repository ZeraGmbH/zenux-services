#ifndef TEST_REGRESSION_DSP_VAR_H
#define TEST_REGRESSION_DSP_VAR_H

#include "testzdsp1dforvaraccess.h"
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
    void createTempGlobalNoRead();
    void createGlobalNoRead();

    void readVariablesAndListenDeviceNode(); // we learned dspMemoryRead details by this
    void writeFloatVariablesAndListenDeviceNode();
    void writeIntVariablesAndListenDeviceNode();
    void writeMixVariablesAndListenDeviceNode();

    void multipleClientsCreateResultVars();

    void serverReadDspWorkspaceVariableAndListenDeviceNode();
    void serverWriteDspDialogWorkspaceVariableAndListenDeviceNode();

private:
    QByteArray floatToBuff(float value);
    QByteArray intToBuff(qint32 value);

    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    std::unique_ptr<TestZdsp1dForVarAccess> m_dspService;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cDSPInterface> m_dspIFace;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
};

#endif // TEST_REGRESSION_DSP_VAR_H
