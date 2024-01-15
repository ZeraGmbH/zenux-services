#ifndef TEST_REGRESSION_ADJ_CALC_COM5003_H
#define TEST_REGRESSION_ADJ_CALC_COM5003_H

#include "atmelcommon.h"
#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_regression_calc_com5003 : public QObject
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

private:
    void setupServers();
    double calcAdjValue(double value, double coeff0, double coeff1, double coeff2);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    AtmelPermissionTemplatePtrU m_permissionMock;
    std::unique_ptr<AdjustScpiValueFormatter> m_valueFormatter;
};

#endif // TEST_REGRESSION_ADJ_CALC_COM5003_H