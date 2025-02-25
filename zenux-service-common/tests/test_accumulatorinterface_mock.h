#ifndef TEST_ACCUMULATORINTERFACE_MOCK_H
#define TEST_ACCUMULATORINTERFACE_MOCK_H

#include "resmanrunfacade.h"
#include "mockmt310s2d.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_accumulatorinterface_mock : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void readAccuStatusAccuDisabled();
    void readAccuStatusAccuEnabled();

    void readAccuStateOfChargeAccuDisabled();
    void readAccuStateOfChargeAccuEnabled();

private:
    void setupServers(QString configFileXml);
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2d> m_mt310s2d;
    Zera::ProxyClientPtr m_proxyClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_ACCUMULATORINTERFACE_MOCK_H
