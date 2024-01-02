#ifndef TEST_REGRESSION_ADJUSTMENT_COM5003_H
#define TEST_REGRESSION_ADJUSTMENT_COM5003_H

#include "mockforsenseinterfacecom5003.h"
#include "pcbinterface.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_xml_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void directAcessFileExportXml();
    void directAcessFileImportXmlMinimal();
    void directAcessFileImportXmlPseudoRandom();
    void directAcessFileImportMissingType();
    void directAcessFileImportMissingSerNo();

    void scpiExportInitialAdjXml();
    void scpiImportPermissionQueryFail();
private:
    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJUSTMENT_COM5003_H
