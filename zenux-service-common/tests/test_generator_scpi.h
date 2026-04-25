#ifndef TEST_GENERATOR_SCPI_H
#define TEST_GENERATOR_SCPI_H

#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include <resmanrunfacade.h>

class test_generator_scpi : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void getSetValidSourceModeOn();
    void getSetInvalidSourceModeOn();

    void getSetValidSourceOn();

    void setChangeRangeByAmplitude();
    void queryRangeByAmplitudeDenied();
    void setChangeRangeByAmplitudeInvalidChannel();
    void setChangeRangeByAmplitudeInvalidValue();

    void getSetRange();
    void getRangeInvalidChannel();
    void setRangeInvalidChannel();

    void notifySourceModeOnChange();

    void getSetDspAmplitude();
    void getSetDspFrequency();
    void getSetDspAngle();

    void noCrossWrite();

private:
    void setupServers();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_GENERATOR_SCPI_H
