#ifndef TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_MT310S2_H
#define TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_scpi_query_format_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();

    void queryGainCorrectionSingleGen();
    void queryGainCorrectionSingle();
    void queryGainCorrectionTotalGen();
    void queryGainCorrectionTotal();

    void queryPhaseCorrectionSingleGen();
    void queryPhaseCorrectionSingle();
    void queryPhaseCorrectionTotalGen();
    void queryPhaseCorrectionTotal();

    void queryOffsetCorrectionSingleGen();
    void queryOffsetCorrectionSingle();
    void queryOffsetCorrectionTotalGen();
    void queryOffsetCorrectionTotal();

    void queryGainCoefficients();
    void queryGainNodes();
    void queryPhaseCoefficients();
    void queryPhaseNodes();
    void queryOffsetCoefficients();
    void queryOffsetNodes();
private:
    void setupServers();

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_MT310S2_H
