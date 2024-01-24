#ifndef TEST_REGRESSION_ADJUSTMENT_MT310S2_H
#define TEST_REGRESSION_ADJUSTMENT_MT310S2_H

#include "testserverforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_xml_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase(); // We don't test clamps yet - keep this as reminder
    void init();
    void cleanup();

    void directAcessExportXml();
    void directAcessFileImportXmlMinimal();
    void directAcessArbitraryVerion();
    void directAcessFileImportXmlPseudoRandom();
    void directAcessFileImportMissingType();
    void directAcessFileImportMissingSerNo();
    void directAcessUnknownEntry();
    // there are no known users for scpi file read write - so for now just test
    // scpi xml transfers
    void scpiExportInitialAdjXml();

    // clamps
    void directAcessExportXmlClamps();
    void scpiExportInitialAdjXmlClamps();

private:
    void setupServers();

    std::unique_ptr<TestServerForSenseInterfaceMt310s2> m_testServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
    I2cCtrlEepromPermissionPtr m_permissionMock;
};

#endif // TEST_REGRESSION_ADJUSTMENT_MT310S2_H
