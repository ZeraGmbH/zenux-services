#ifndef TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_COM5003_H
#define TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_scpi_query_format_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void queryGainCorrectionSingle();
    void queryGainCorrectionTotal();
    void queryPhaseCorrectionSingle();
    void queryPhaseCorrectionTotal();
    void queryOffsetCorrectionSingle();
    void queryOffsetCorrectionTotal();

    void queryGainCoefficients();
    void queryGainNodes();
    void queryPhaseCoefficients();
    void queryPhaseNodes();
    void queryOffsetCoefficients();
    void queryOffsetNodes();
private:
    void setupServers(AtmelPermissionTemplate *permissionQueryHandler);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    AtmelPermissionTemplatePtrU m_permissionMock;
};

#endif // TEST_REGRESSION_ADJ_SCPI_QUERY_FORMAT_COM5003_H
