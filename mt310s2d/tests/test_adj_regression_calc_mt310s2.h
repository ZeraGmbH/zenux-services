#ifndef TEST_REGRESSION_ADJ_CALC_MT310S2_H
#define TEST_REGRESSION_ADJ_CALC_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_regression_calc_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanup();

    // internal
    void gainAdjValueSingle();
    void gainAdjValueTotal();

    void phaseAdjValueSingle();
    void phaseAdjValueTotal();

    void offsetAdjValueSingle();
    void offsetAdjValueTotal();

    // clamps
    void gainAdjValueSingleClamp();
    void gainAdjValueTotalClamp();

    void phaseAdjValueSingleClamp();
    void phaseAdjValueTotalClamp();

    void offsetAdjValueSingleClamp();
    void offsetAdjValueTotalClamp();
private:
    void setupServers();
    void addClamp();
    QString readFile(QString filename);
    double calcAdjValue(double value, double coeff0, double coeff1, double coeff2);

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    std::unique_ptr<AdjustScpiValueFormatter> m_valueFormatter;
};

#endif // TEST_REGRESSION_ADJ_CALC_MT310S2_H
