#ifndef TEST_ACCUMULATORINTERFACE_MOCK_H
#define TEST_ACCUMULATORINTERFACE_MOCK_H

#include "resmanrunfacade.h"
#include "mockmt310s2dfull.h"
#include "pcbinterface.h"
#include "proxyclient.h"

class test_accumulatorinterface_mock : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void readAccumulatorStatusAccuDisabled();
    void readAccumulatorStatusAccuEnabled();

    void readAccuStateOfChargeAccuDisabled();
    void readAccuStateOfChargeAccuEnabled();

private:
    void setupServers(QString configFileXml);
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<MockMt310s2dFull> m_mt310s2d;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_ACCUMULATORINTERFACE_MOCK_H
