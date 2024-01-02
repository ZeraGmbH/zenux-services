#ifndef TEST_REGRESSION_ADJUSTMENT_MT310S2_H
#define TEST_REGRESSION_ADJUSTMENT_MT310S2_H

#include "mockforsenseinterfacemt310s2.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_xml_mt310s2 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase(); // We don't test clamps yet - keep this as reminder
    void cleanup();

    void directAcessFileExportXml();
    void directAcessFileImportXmlMinimal();
    void directAcessFileImportXmlPseudoRandom();
    void directAcessFileImportMissingType();
    void directAcessFileImportMissingSerNo();

    void scpiExportInitialAdjXml();
    void scpiImportPermissionQueryFail();
private:
    void setupServers(AtmelPermissionTemplate *permissionQueryHandler);

    std::unique_ptr<MockForSenseInterfaceMt310s2> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJUSTMENT_MT310S2_H
