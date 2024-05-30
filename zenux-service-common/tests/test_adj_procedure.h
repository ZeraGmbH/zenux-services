#ifndef TEST_ADJ_PROCEDURE_H
#define TEST_ADJ_PROCEDURE_H

#include "testserverforsenseinterfacemt310s2.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_adj_procedure : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();
    void cleanup();

    void adjDeviceDataScpiReadOk();
    void adjDeviceDataScpiAdjusted();
    void adjDeviceXml();
    void writeGainNodesStoreStartExport();
    void writeGainNodesComputeStoreStartExport();
    void writePhaseNodesStoreStartExport();
    void writePhaseNodesComputeStoreStartExport();

private:
    void setupServers();
    void prepareEepromTakenFromAdjustedDevice();
    void setGainNodesOnTwoRanges();
    void setPhaseNodesOnTwoRanges();
    void writeAdjToChip();
    void computeAdj();
    void simulReboot();

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_proxyClient;

};

#endif // TEST_ADJ_PROCEDURE_H
