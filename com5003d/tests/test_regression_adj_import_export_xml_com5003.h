#ifndef TEST_REGRESSION_ADJUSTMENT_COM5003_H
#define TEST_REGRESSION_ADJUSTMENT_COM5003_H

#include "testserverforsenseinterfacecom5003.h"
#include "proxyclient.h"
#include "resmanrunfacade.h"
#include <QObject>

class test_regression_adj_import_export_xml_com5003 : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
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
private:
    void setupServers();

    VeinTcp::AbstractTcpNetworkFactoryPtr m_tcpNetworkFactory;
    std::unique_ptr<ResmanRunFacade> m_resman;
    std::unique_ptr<TestServerForSenseInterfaceCom5003> m_testServer;
    Zera::ProxyClientPtr m_proxyClient;
};

#endif // TEST_REGRESSION_ADJUSTMENT_COM5003_H
