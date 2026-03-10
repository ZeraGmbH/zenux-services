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

    void createInternalVariableNotAvailable();
    void createInternalVariableWrongSize();
    void createInternalVariableWrongType();
    void createInternalVariableOk();
    void createInternalVariablesOk();
    void createInternalVariablesSecondNotOk();

    void createAlignedVariableOk(); // We need more here!!!
    void createAlignedVariablesMultipleClients();

    void readVariablesAndListenDeviceNode(); // we learned dspMemoryRead details by this
    void writeFloatVariablesAndListenDeviceNode();
    void writeIntVariablesAndListenDeviceNode();
    void writeMixVariablesAndListenDeviceNode();

    void multipleClientsCreateResultVars();

    void serverReadDspWorkspaceVariableAndListenDeviceNode();
    void serverWriteDspDialogWorkspaceVariableAndListenDeviceNode();
    void serverReadDspUserWorkspaceVariableAndListenDeviceNode();
    void serverReadDspCmdListVariableAndListenDeviceNode();
    void serverReadDspChannelDataVariableAndListenDeviceNode();

    void dspVarMemSizeInitialHack();

    void dspInternalVariables21262();
    void dspInternalVariables21362();

private:
    QByteArray floatToBuff(float value);
    QByteArray intToBuff(qint32 value);

    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    std::unique_ptr<TestZdsp1dForVarAccess> m_dspService;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cDSPInterface> m_dspIFace;
    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
};

#endif // TEST_REGRESSION_DSP_VAR_H
