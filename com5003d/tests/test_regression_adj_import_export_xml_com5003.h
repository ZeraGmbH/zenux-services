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
    void cleanup();

    void directAcessExportXml();
    void directAcessFileImportXmlMinimal();
    void directAcessFileImportXmlPseudoRandom();
    void directAcessFileImportMissingType();
    void directAcessFileImportMissingSerNo();

    // there are no known users for scpi file read write - so for now just test
    // scpi xml transfers
    void scpiExportInitialAdjXml();
    void scpiImportPermissionQueryFail();
    void scpiImportNoPermission();
    void scpiImportInvalidXml();
    void scpiImportFailFlashWrite();
private:
    void setupServers(AtmelPermissionTemplate *permissionQueryHandler);

    std::unique_ptr<MockForSenseInterfaceCom5003> m_mockServer;
    std::unique_ptr<ResmanRunFacade> m_resmanServer;
    Zera::ProxyClientPtr m_pcbClient;
    std::unique_ptr<Zera::cPCBInterface> m_pcbIFace;
};

#endif // TEST_REGRESSION_ADJUSTMENT_COM5003_H
