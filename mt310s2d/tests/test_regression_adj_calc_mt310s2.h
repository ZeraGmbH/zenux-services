#ifndef TEST_REGRESSION_ADJ_CALC_MT310S2_H
#define TEST_REGRESSION_ADJ_CALC_MT310S2_H

#include "atmelpermissiontemplate.h"
#include "mockforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_calc_mt310s2 : public QObject
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
    void setupServers(AtmelPermissionTemplate *permissionQueryHandler);
    void addClamp();
    QString readFile(QString filename);
    double calcAdjValue(double value, double coeff0, double coeff1, double coeff2);

    std::unique_ptr<MockForSenseInterfaceMt310s2> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    AtmelPermissionTemplatePtrU m_permissionMock;
    std::unique_ptr<AdjustScpiValueFormatter> m_valueFormatter;
};

#endif // TEST_REGRESSION_ADJ_CALC_MT310S2_H
